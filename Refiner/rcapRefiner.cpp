/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Program Name : rcapRefiner                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/*
 * g++ -c -DGFORTRAN -I.. -I. rcapRefiner.cpp
 */

/*
 * 実装における注意点：
 *  細分される要素は refineElement の中では記憶しておく必要はないが、
 *  getOriginal で親を調べるときにどの要素を使って細分したのかを調べるために
 *  appendBody で rcapRefinerDoc.mesh に記録しておく。clearRefiner でラッパコンテナを削除している。
 *  細分後の要素は呼び出し側に返して、このライブラリの中では記憶しない。
 *
 *  nodeOffset がある場合、nodeContainer には nodeOffset でずらして登録する
 *
 *  境界条件は細分前と細分後のデータの両方を rcapRefinerDoc.refiner または rcapRefinerDoc.boundaryRefiner
 *  が保持している。どちらが保持するかどうかは境界条件の種類に依存する。
 *  Commit したときに rcapRefinerDoc.mesh に登録されている境界条件を細分前のデータから
 *  細分後のデータに入れ替えている。
 */

#include "rcapRefiner.h"
#include "Geometry/kmb_idTypes.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElement.h"
#include "MeshGen/kmbMeshRefiner.h"
#include "Geometry/kmb_Point3DContainerMArray.h"
#include "MeshDB/kmbElementContainerNArray.h"
#include "MeshDB/kmbElementContainerMixedArray.h"
#include "MeshDB/kmbBoundaryExtractor.h"
#include "MeshDB/kmbIntegerMapBindings.h"
#include "MeshGen/kmbMiddleNodeManager.h"

#include "RevocapIO/kmbFFBIO.h"
#include "RevocapIO/kmbHECMWIO.h"
#include "RevocapIO/kmbRevocapNeutralIO.h"
#include "RevocapIO/kmbRevocapCouplerIO.h"

#include <stdio.h>
#include <string.h>

#ifdef WIN32
 #if _MSC_VER >= 1400
  #define strcpy(ss0,ss1) strcpy_s(ss0,sizeof(ss1),ss1)
 #endif
#endif

struct rcapRefinerInfo
{
	kmb::MeshData* mesh;
	kmb::MeshRefiner* refiner;
	kmb::MeshRefiner* boundaryRefiner;
	kmb::MiddleNodeManager* middleMan;
	int32_t nodeOffset;
	int32_t elementOffset;
	int32_t maxElementId;
	int32_t maxRefinedElementId;
} rcapRefinerDoc;

void rcapGetVersion( void )
{
	puts("REVOCAP Refiner version 0.4.0 (2010/6/1)");
}

void rcapInitRefiner(int32_t node_Offset,int32_t element_Offset)
{
	REVOCAP_Debug("rcapInitRefiner start version 0.4.0 (2010/6/1)\n");
	rcapRefinerDoc.mesh = NULL;
	rcapRefinerDoc.refiner = NULL;
	rcapRefinerDoc.boundaryRefiner = NULL;
	rcapRefinerDoc.middleMan = NULL;
	rcapRefinerDoc.nodeOffset = node_Offset;
	rcapRefinerDoc.elementOffset = element_Offset;



	rcapRefinerDoc.maxElementId = -1;
	rcapRefinerDoc.maxRefinedElementId = -1;

	rcapRefinerDoc.mesh = new kmb::MeshDB();
	rcapRefinerDoc.mesh->beginNode(100000,"double_marray");
	rcapRefinerDoc.mesh->endNode();

	rcapRefinerDoc.middleMan = new kmb::MiddleNodeManager();

	rcapRefinerDoc.refiner = new kmb::MeshRefiner();
	rcapRefinerDoc.refiner->setMiddleNodeManager(rcapRefinerDoc.middleMan);
	rcapRefinerDoc.refiner->setMesh(rcapRefinerDoc.mesh);

	rcapRefinerDoc.boundaryRefiner = new kmb::MeshRefiner();
	rcapRefinerDoc.boundaryRefiner->setMiddleNodeManager(rcapRefinerDoc.middleMan);
	rcapRefinerDoc.boundaryRefiner->setMesh(rcapRefinerDoc.mesh);

	REVOCAP_Debug("rcapInitRefiner finish\n");
}

void rcapSetCADFilename( const char* filename )
{
}

void rcapSetPartitionFilename( const char* filename )
{
	kmb::RevocapCouplerIO couplerIO;
	couplerIO.loadLocalNodesFromPartitionFile(filename,rcapRefinerDoc.mesh);
}

void rcapClearRefiner( void )
{
	REVOCAP_Debug("rcapClearRefiner\n");
	if(rcapRefinerDoc.mesh){
		rcapRefinerDoc.mesh->removeAllBodies();
	}
	if(rcapRefinerDoc.refiner){
		rcapRefinerDoc.refiner->clear();
	}
	if(rcapRefinerDoc.boundaryRefiner){
		rcapRefinerDoc.boundaryRefiner->clear();
	}
	rcapRefinerDoc.maxRefinedElementId = -1;
	rcapRefinerDoc.maxElementId = -1;
}

void rcapTermRefiner( void )
{
	REVOCAP_Debug("rcapTermRefiner start\n");
	if(rcapRefinerDoc.boundaryRefiner){
		delete rcapRefinerDoc.boundaryRefiner;
		rcapRefinerDoc.boundaryRefiner = NULL;
	}
	if(rcapRefinerDoc.refiner){
		delete rcapRefinerDoc.refiner;
		rcapRefinerDoc.refiner = NULL;
	}
	if(rcapRefinerDoc.middleMan){
		delete rcapRefinerDoc.middleMan;
		rcapRefinerDoc.middleMan = NULL;
	}
	if(rcapRefinerDoc.mesh){
		delete rcapRefinerDoc.mesh;
		rcapRefinerDoc.mesh = NULL;
	}
	REVOCAP_Debug("rcapTermRefiner finish\n");
}

size_t rcapRefineElement( size_t num, int8_t etype, int32_t* nodeArray, int32_t* resultNodeArray )
{
	REVOCAP_Debug("rcapRefineElement start\n");
	size_t refinedNum = 0;
	switch( etype ){
		case kmb::SEGMENT:		refinedNum = 2*num;	break;
		case kmb::SEGMENT2:		refinedNum = 2*num;	break;
		case kmb::TRIANGLE:		refinedNum = 4*num;	break;
		case kmb::TRIANGLE2:	refinedNum = 4*num;	break;
		case kmb::QUAD:			refinedNum = 4*num;	break;
		case kmb::QUAD2:		refinedNum = 4*num;	break;
		case kmb::TETRAHEDRON:	refinedNum = 8*num;	break;
		case kmb::TETRAHEDRON2:	refinedNum = 8*num;	break;
		case kmb::HEXAHEDRON:	refinedNum = 8*num;	break;
		case kmb::HEXAHEDRON2:	refinedNum = 8*num;	break;
		case kmb::WEDGE:		refinedNum = 8*num;	break;
		case kmb::WEDGE2:		refinedNum = 8*num;	break;
		case kmb::PYRAMID:		refinedNum = 10*num;	break;
		case kmb::PYRAMID2:		refinedNum = 10*num;	break;
		default:	break;
	}

	if( resultNodeArray == NULL || resultNodeArray[0] < 0 ||
		nodeArray == NULL || nodeArray[0] < 0 )
	{
		REVOCAP_Debug("rcapRefineElement return refined element count %d\n",refinedNum);
		return refinedNum;
	}
	kmb::elementType elemType = static_cast<kmb::elementType>(etype);




	kmb::ElementContainerNArray* orgElements = new kmb::ElementContainerNArray( elemType, num, reinterpret_cast<kmb::nodeIdType*>(nodeArray), false, rcapRefinerDoc.nodeOffset );
	orgElements->setOffsetId( rcapRefinerDoc.maxElementId+1 );
	rcapRefinerDoc.mesh->appendBody( orgElements );




	REVOCAP_Debug("rcapRefineElement new element container wrapper %d\n",refinedNum);
	kmb::ElementContainerNArray* refineElements = new kmb::ElementContainerNArray( elemType, refinedNum, reinterpret_cast<kmb::nodeIdType*>(resultNodeArray), true, rcapRefinerDoc.nodeOffset );
	refineElements->setOffsetId( rcapRefinerDoc.maxRefinedElementId+1 );


	rcapRefinerDoc.refiner->refineBody( orgElements, refineElements );

	rcapRefinerDoc.maxElementId = orgElements->getMaxId();
	rcapRefinerDoc.maxRefinedElementId = refineElements->getMaxId();
	REVOCAP_Debug("rcapRefineElement finish\n");
	size_t count = refineElements->getCount();


	delete refineElements;
	return count;
}

size_t rcapRefineElementMulti( size_t num, int8_t* etypeArray, int32_t* nodeArray, size_t* refinedNum, int8_t* resultEtypeArray, int32_t* resultNodeArray )
{
	REVOCAP_Debug("rcapRefineElementMulti start\n");
	if( num <= 0 || etypeArray == NULL || etypeArray[0] < 0 || refinedNum == NULL ){
		return 0;
	}
	size_t refinedNodeArraySize = 0;
	size_t refinedElementSize = *refinedNum;
	REVOCAP_Debug("rcapRefineElementMulti argument refinedNum = %d\n", *refinedNumElementSize);
	if( refinedElementSize <= 0 ){
		refinedElementSize = 0;
		for(size_t i=0;i<num;++i){
			kmb::elementType etype = static_cast<kmb::elementType>( etypeArray[i] );
			switch( etype ){
				case kmb::SEGMENT:
					refinedNodeArraySize += 2*2;
					refinedElementSize += 2;
					break;
				case kmb::SEGMENT2:
					refinedNodeArraySize += 2*3;
					refinedElementSize += 2;
					break;
				case kmb::TRIANGLE:
					refinedNodeArraySize += 4*3;
					refinedElementSize += 4;
					break;
				case kmb::TRIANGLE2:
					refinedNodeArraySize += 4*6;
					refinedElementSize += 4;
					break;
				case kmb::QUAD:
					refinedNodeArraySize += 4*4;
					refinedElementSize += 4;
					break;
				case kmb::QUAD2:
					refinedNodeArraySize += 4*8;
					refinedElementSize += 4;
					break;
				case kmb::TETRAHEDRON:
					refinedNodeArraySize += 8*4;
					refinedElementSize += 8;
					break;
				case kmb::TETRAHEDRON2:
					refinedNodeArraySize += 8*10;
					refinedElementSize += 8;
					break;
				case kmb::HEXAHEDRON:
					refinedNodeArraySize += 8*8;
					refinedElementSize += 8;
					break;
				case kmb::HEXAHEDRON2:
					refinedNodeArraySize += 8*20;
					refinedElementSize += 8;
					break;
				case kmb::WEDGE:
					refinedNodeArraySize += 8*6;
					refinedElementSize += 8;
					break;
				case kmb::WEDGE2:
					refinedNodeArraySize += 8*15;
					refinedElementSize += 8;
					break;
				case kmb::PYRAMID:
					refinedNodeArraySize += (6*5 + 4*4);
					refinedElementSize += 10;
					break;
				case kmb::PYRAMID2:
					refinedNodeArraySize += (6*13 + 4*10);
					refinedElementSize += 10;
					break;
				default:
					break;
			}
		}
	}
	if( resultNodeArray == NULL || resultNodeArray[0] < 0 ||
		nodeArray == NULL || nodeArray[0] < 0 ){
		*refinedNum = refinedElementSize;
		return refinedNodeArraySize;
	}

	REVOCAP_Debug("rcapRefineElementMulti Org Element Count = %d\n", num);
	kmb::ElementContainerMixedArray* orgElements = new kmb::ElementContainerMixedArray( num, reinterpret_cast<char*>(etypeArray), reinterpret_cast<kmb::nodeIdType*>(nodeArray), false, rcapRefinerDoc.nodeOffset );
	orgElements->setOffsetId( rcapRefinerDoc.maxElementId+1 );
	rcapRefinerDoc.mesh->appendBody( orgElements );

	size_t count = 0;
	if( resultEtypeArray != NULL && resultEtypeArray[0] >= 0 ){
		REVOCAP_Debug("rcapRefineElementMulti Refined Element Count = %d\n", refinedElementSize);
		kmb::ElementContainerMixedArray* refineElements = new kmb::ElementContainerMixedArray( refinedElementSize, reinterpret_cast<char*>(resultEtypeArray), reinterpret_cast<kmb::nodeIdType*>(resultNodeArray), true, rcapRefinerDoc.nodeOffset );
		refineElements->setOffsetId( rcapRefinerDoc.maxRefinedElementId+1 );

		rcapRefinerDoc.refiner->refineBody( orgElements, refineElements );

		rcapRefinerDoc.maxElementId = orgElements->getMaxId();
		rcapRefinerDoc.maxRefinedElementId = refineElements->getMaxId();
		count = refineElements->getCount();
		delete refineElements;
	}else{
		REVOCAP_Debug("rcapRefineElementMulti Refined Element Count = %d (no etype)\n", refinedElementSize);
		char* rEtypeArray = new char[ refinedElementSize ];
		kmb::ElementContainerMixedArray* refineElements = new kmb::ElementContainerMixedArray( refinedElementSize, rEtypeArray, reinterpret_cast<kmb::nodeIdType*>(resultNodeArray), true, rcapRefinerDoc.nodeOffset );
		refineElements->setOffsetId( rcapRefinerDoc.maxRefinedElementId+1 );

		rcapRefinerDoc.refiner->refineBody( orgElements, refineElements );

		rcapRefinerDoc.maxElementId = orgElements->getMaxId();
		rcapRefinerDoc.maxRefinedElementId = refineElements->getMaxId();
		count = refineElements->getCount();
		delete refineElements;
		delete[] rEtypeArray;
	}
	REVOCAP_Debug("rcapRefineElementMulti finish\n");
	*refinedNum = refinedElementSize;
	return refinedNodeArraySize;
}

void rcapCommit(void)
{

	if(rcapRefinerDoc.boundaryRefiner && rcapRefinerDoc.boundaryRefiner->getDataCount() > 0){
		kmb::BoundaryExtractor bext;
		bext.setMesh( rcapRefinerDoc.mesh );
		kmb::bodyIdType bodyCount = rcapRefinerDoc.mesh->getBodyCount();
		for(kmb::bodyIdType bodyId = 0; bodyId < bodyCount; ++bodyId){
			bext.appendBody(bodyId);
		}
		kmb::bodyIdType boundaryId = bext.getBoundary();
		REVOCAP_Debug("rcapCommit boundary surface extract %d\n", rcapRefinerDoc.mesh->getElementCount(boundaryId) );
		kmb::ElementContainer* boundaryBody = rcapRefinerDoc.mesh->getBodyPtr(boundaryId);
		REVOCAP_Debug("boundary element count => %d\n", boundaryBody->getCount());
		rcapRefinerDoc.boundaryRefiner->refineBody( boundaryBody, NULL );
		rcapRefinerDoc.boundaryRefiner->commitData();
	}
	if(rcapRefinerDoc.refiner){
		rcapRefinerDoc.refiner->commitData();
	}
}

void rcapSetNode64( size_t num, float64_t* coords, int32_t* globalIds, int32_t* localIds )
{
	REVOCAP_Debug("rcapSetNode64 start\n");
	/* まだ global Id を考慮していない */
	if(rcapRefinerDoc.mesh == NULL){
		return;
	}
	if( localIds==NULL || localIds[0] < rcapRefinerDoc.nodeOffset ){
		REVOCAP_Debug("rcapSetNode64 localIds is NULL\n");
		for(unsigned int i=0;i<num;++i){
			rcapRefinerDoc.mesh->insertNode(coords[3*i],coords[3*i+1],coords[3*i+2]);
		}
	}else{
		for(unsigned int i=0;i<num;++i){
			rcapRefinerDoc.mesh->insertNodeWithId(coords[3*i],coords[3*i+1],coords[3*i+2],localIds[i]-rcapRefinerDoc.nodeOffset);
		}
	}
	REVOCAP_Debug("rcapSetNode64 finish\n");
}

void rcapSetNode32( size_t num, float32_t* coords, int32_t* globalIds, int32_t* localIds )
{
	/* まだ global Id を考慮していない */
	if(rcapRefinerDoc.mesh == NULL){
		return;
	}
	if( localIds==NULL || localIds[0] < rcapRefinerDoc.nodeOffset ){
		for(unsigned int i=0;i<num;++i){
			rcapRefinerDoc.mesh->insertNode(coords[3*i],coords[3*i+1],coords[3*i+2]);
		}
	}else{
		for(unsigned int i=0;i<num;++i){
			rcapRefinerDoc.mesh->insertNodeWithId(coords[3*i],coords[3*i+1],coords[3*i+2],localIds[i]-rcapRefinerDoc.nodeOffset);
		}
	}
}

size_t rcapGetNodeCount( void )
{
	REVOCAP_Debug("rcapGetNodeCount start\n");
	if(rcapRefinerDoc.mesh){
		REVOCAP_Debug("rcapGetNodeCount ContainerType %s\n", rcapRefinerDoc.mesh->getNodes()->getContainerType() );
		return rcapRefinerDoc.mesh->getNodeCount();
	}else{
		return 0;
	}
}

void rcapGetNode64( size_t num, int32_t* localIds, float64_t* coords )
{
	REVOCAP_Debug("rcapGetNode64 start\n");
	if( rcapRefinerDoc.mesh != NULL ){
		kmb::Point3D point;
		for(size_t i=0;i<num;++i){
			rcapRefinerDoc.mesh->getNode(static_cast<kmb::nodeIdType>(localIds[i]-rcapRefinerDoc.nodeOffset),point);
			coords[3*i]   = point.x();
			coords[3*i+1] = point.y();
			coords[3*i+2] = point.z();
		}
	}
	REVOCAP_Debug("rcapGetNode64 finish\n");
}

void rcapGetNode32( size_t num, int32_t* localIds, float32_t* coords )
{
	if( rcapRefinerDoc.mesh != NULL ){
		kmb::Point3D point;
		for(size_t i=0;i<num;++i){
			rcapRefinerDoc.mesh->getNode(static_cast<kmb::nodeIdType>(localIds[i]-rcapRefinerDoc.nodeOffset),point);
			coords[3*i]   = static_cast<float>(point.x());
			coords[3*i+1] = static_cast<float>(point.y());
			coords[3*i+2] = static_cast<float>(point.z());
		}
	}
}

void rcapGetNodeSeq64( size_t num, size_t initId, float64_t* coords )
{
	REVOCAP_Debug("rcapGetNodeSeq64 start num = %d initId = %d\n", num, initId);
	if( rcapRefinerDoc.mesh != NULL ){
		size_t ini = initId - rcapRefinerDoc.nodeOffset;
		kmb::Point3D point;
		for(size_t i=ini;i<ini+num;++i){
			rcapRefinerDoc.mesh->getNode(static_cast<kmb::nodeIdType>(i),point);
			coords[3*i]   = point.x();
			coords[3*i+1] = point.y();
			coords[3*i+2] = point.z();
		}
	}
	REVOCAP_Debug("rcapGetNodeSeq64 finish\n");
}

void rcapGetNodeSeq32( size_t num, size_t initId, float32_t* coords )
{
	REVOCAP_Debug("rcapGetNodeSeq64 start num = %d initId = %d\n", num, initId);
	if( rcapRefinerDoc.mesh != NULL ){
		size_t ini = initId - rcapRefinerDoc.nodeOffset;
		kmb::Point3D point;
		for(size_t i=ini;i<ini+num;++i){
			rcapRefinerDoc.mesh->getNode(static_cast<kmb::nodeIdType>(i),point);
			coords[3*i]   = static_cast<float>(point.x());
			coords[3*i+1] = static_cast<float>(point.y());
			coords[3*i+2] = static_cast<float>(point.z());
		}
	}
	REVOCAP_Debug("rcapGetNodeSeq64 finish\n");
}

int8_t rcapGetOriginal( int32_t localNodeId, int32_t* originalNodeArray )
{
	if( rcapRefinerDoc.refiner != NULL ){
		kmb::elementType etype = rcapRefinerDoc.refiner->getOriginal(localNodeId-rcapRefinerDoc.nodeOffset,reinterpret_cast<kmb::nodeIdType*>(originalNodeArray));
		int len = kmb::Element::getNodeCount( static_cast<kmb::elementType>(etype) );
		for(int i=0;i<len;++i){
			originalNodeArray[i] += rcapRefinerDoc.nodeOffset;
		}
		return static_cast<int8_t>(etype);
	}
	return RCAP_UNKNOWNTYPE;
}

int32_t rcapGetMiddle( int8_t etype, int32_t* originalNodeArray )
{
	int len = kmb::Element::getNodeCount( static_cast<kmb::elementType>(etype) );
	kmb::Element* element = kmb::Element::create( static_cast<kmb::elementType>(etype) );
	for(int i=0;i<len;++i){
		element->setCellId( i, originalNodeArray[i] - rcapRefinerDoc.nodeOffset );
	}
	kmb::nodeIdType nodeId = rcapRefinerDoc.refiner->getMiddle( *element );
	delete element;
	if( nodeId == kmb::nullNodeId ){
		return -1;
	}else{
		return nodeId + rcapRefinerDoc.nodeOffset;
	}
}

void rcapAppendNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray )
{
	REVOCAP_Debug("Append Node Group [%s]\n",dataname);
	kmb::DataBindings* data = rcapRefinerDoc.mesh->createDataBindings( dataname, kmb::DataBindings::NODEGROUP, kmb::PhysicalValue::NONE, "NG" );
	if( data ){
		for(size_t i=0;i<num;++i){
			data->addId( static_cast<kmb::nodeIdType>(nodeArray[i]) - rcapRefinerDoc.nodeOffset );
		}
		rcapRefinerDoc.refiner->appendData( dataname, "NG" );
	}
}

size_t rcapGetNodeGroupCount( const char dataname[80] )
{
	return rcapRefinerDoc.mesh->getIdCount( dataname, "NG" );
}

void rcapGetNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray )
{
	REVOCAP_Debug("Get Node Group [%s]\n",dataname);
	const kmb::DataBindings* data = rcapRefinerDoc.mesh->getDataBindingsPtr( dataname, "NG" );
	if( data && data->getBindingMode() == kmb::DataBindings::NODEGROUP ){
		kmb::DataBindings::const_iterator dIter = data->begin();
		size_t i = 0;
		while( !dIter.isFinished() ){
			if( i > num ){
				break;
			}
			nodeArray[i] = dIter.getId() + rcapRefinerDoc.nodeOffset;
			++dIter;
			++i;
		}
	}
}

void rcapAppendBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray )
{
	REVOCAP_Debug("Append Boundary Node Group [%s]\n",dataname);
	kmb::DataBindings* data = rcapRefinerDoc.mesh->createDataBindings( dataname, kmb::DataBindings::NODEGROUP, kmb::PhysicalValue::NONE, "BNG" );
	if( data ){
		for(size_t i=0;i<num;++i){
			data->addId( static_cast<kmb::nodeIdType>(nodeArray[i]) - rcapRefinerDoc.nodeOffset );
		}
		rcapRefinerDoc.boundaryRefiner->appendData( dataname, "BNG" );
	}
}

size_t rcapGetBNodeGroupCount( const char dataname[80] )
{
	return rcapRefinerDoc.mesh->getIdCount( dataname, "BNG" );
}

void rcapGetBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray )
{
	REVOCAP_Debug("Get Boundary Node Group [%s]\n",dataname);
	const kmb::DataBindings* data = rcapRefinerDoc.mesh->getDataBindingsPtr( dataname, "BNG" );
	if( data && data->getBindingMode() == kmb::DataBindings::NODEGROUP ){
		kmb::DataBindings::const_iterator dIter = data->begin();
		size_t i = 0;
		while( !dIter.isFinished() ){
			if( i > num ){
				break;
			}
			nodeArray[i] = dIter.getId() + rcapRefinerDoc.nodeOffset;
			++dIter;
			++i;
		}
	}
}

void rcapAppendBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars )
{
	REVOCAP_Debug("Append Boundary Node Variable Int [%s]\n",dataname);
	kmb::DataBindings* data = new kmb::IntegerMapBindings();
	if( data ){
		rcapRefinerDoc.mesh->setDataBindingsPtr( dataname, data, "BNVI" );
		for(size_t i=0;i<num;++i){
			long l = static_cast<long>(nodeVars[i]);
			data->setPhysicalValue( static_cast<kmb::nodeIdType>(nodeArray[i]) - rcapRefinerDoc.nodeOffset, &l );
		}
		rcapRefinerDoc.boundaryRefiner->appendData( dataname, "BNVI" );
	}
}

size_t rcapGetBNodeVarIntCount( const char dataname[80] )
{
	return rcapRefinerDoc.mesh->getIdCount( dataname, "BNVI" );
}

void rcapGetBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars  )
{
	REVOCAP_Debug("Get Boundary Node Variable Int [%s]\n",dataname);
	const kmb::DataBindings* data = rcapRefinerDoc.mesh->getDataBindingsPtr( dataname, "BNVI" );
	if( data && data->getBindingMode() == kmb::DataBindings::NODEVARIABLE
		&& data->getValueType() == kmb::PhysicalValue::INTEGER )
	{
		kmb::DataBindings::const_iterator dIter = data->begin();
		size_t i = 0;
		while( !dIter.isFinished() ){
			if( i > num ){
				break;
			}
			long l = 0;
			nodeArray[i] = dIter.getId() + rcapRefinerDoc.nodeOffset;
			dIter.getValue(&l);
			nodeVars[i] = static_cast< int32_t >(l);
			++dIter;
			++i;
		}
	}
}

void rcapAppendElementGroup( const char dataname[80], size_t num, int32_t* elementArray )
{
	REVOCAP_Debug("Append Element Group [%s]\n",dataname);
	kmb::DataBindings* data = rcapRefinerDoc.mesh->createDataBindings( dataname, kmb::DataBindings::ELEMENTGROUP, kmb::PhysicalValue::NONE, "EG" );
	if( data ){
		for(size_t i=0;i<num;++i){
			data->addId( static_cast<kmb::elementIdType>(elementArray[i]-rcapRefinerDoc.elementOffset) );
		}
		rcapRefinerDoc.refiner->appendData( dataname, "EG" );
	}
}

size_t rcapGetElementGroupCount( const char dataname[80] )
{
	return rcapRefinerDoc.mesh->getIdCount( dataname, "EG" );
}

void rcapGetElementGroup( const char dataname[80], size_t num, int32_t* elementArray )
{
	const kmb::DataBindings* data = rcapRefinerDoc.mesh->getDataBindingsPtr( dataname, "EG" );
	if( data && data->getBindingMode() == kmb::DataBindings::ELEMENTGROUP){
		kmb::DataBindings::const_iterator dIter = data->begin();
		size_t i = 0;
		while( !dIter.isFinished() ){
			if( i > num ){
				break;
			}
			elementArray[i] = dIter.getId() + rcapRefinerDoc.elementOffset;
			++dIter;
			++i;
		}
	}
}

void rcapAppendFaceGroup( const char dataname[80], size_t num, int32_t* faceArray )
{
	REVOCAP_Debug("Append Face Group [%s]\n",dataname);
	kmb::DataBindings* data = rcapRefinerDoc.mesh->createDataBindings( dataname, kmb::DataBindings::FACEGROUP, kmb::PhysicalValue::NONE, "FG" );
	if( data ){
		for(size_t i=0;i<num;++i){
			kmb::Face f( static_cast<kmb::elementIdType>(faceArray[2*i]-rcapRefinerDoc.elementOffset), faceArray[2*i+1] );
			data->addId( f );
		}
		rcapRefinerDoc.refiner->appendData( dataname, "FG" );
	}
}

size_t rcapGetFaceGroupCount( const char dataname[80] )
{
	return rcapRefinerDoc.mesh->getIdCount( dataname, "FG" );
}

void rcapGetFaceGroup( const char dataname[80], size_t num, int32_t* faceArray )
{
	const kmb::DataBindings* data = rcapRefinerDoc.mesh->getDataBindingsPtr( dataname, "FG" );
	if( data && data->getBindingMode() == kmb::DataBindings::FACEGROUP ){
		kmb::DataBindings::const_iterator dIter = data->begin();
		size_t i = 0;
		kmb::Face f;
		while( !dIter.isFinished() ){
			if( i > num ){
				break;
			}
			dIter.getFace( f );
			faceArray[2*i] = f.getElementId() + rcapRefinerDoc.elementOffset;
			faceArray[2*i+1] = f.getLocalFaceId();
			++dIter;
			++i;
		}
	}
}

void rcapSetInterpolateMode( const char mode[32] )
{
	rcapRefinerDoc.refiner->setInterpolationMode(mode);
	rcapRefinerDoc.boundaryRefiner->setInterpolationMode(mode);
}

void rcapGetInterpolateMode( char mode[32] )
{
	const char* m = rcapRefinerDoc.refiner->getInterpolationMode();
	strcpy(mode,m);
}

int32_t rcapLoadGFFile( const char* gffile, const char* bounfile )
{
	kmb::FFBIO ffbIO;
	rcapRefinerDoc.mesh->clearModel();
	rcapRefinerDoc.mesh->beginModel();
	ffbIO.loadFromMeshFile( gffile, rcapRefinerDoc.mesh );
	ffbIO.loadFromBoundaryFile( bounfile, rcapRefinerDoc.mesh );
	rcapRefinerDoc.mesh->endModel();
	rcapRefinerDoc.maxElementId = rcapRefinerDoc.mesh->getMaxElementId();

	rcapRefinerDoc.refiner->setMesh( rcapRefinerDoc.mesh );
	rcapRefinerDoc.boundaryRefiner->setMesh( rcapRefinerDoc.mesh );










	return rcapRefinerDoc.mesh->getElementCount();
}

int32_t rcapLoadHECFile( const char* hecfile )
{
	kmb::HECMWIO hecIO;
	rcapRefinerDoc.mesh->clearModel();
	hecIO.loadFromFile( hecfile, rcapRefinerDoc.mesh );
	return rcapRefinerDoc.mesh->getElementCount();
}




void rcapRefineFFbModel( void )
{
	REVOCAP_Debug("inlt data size => %d\n", rcapRefinerDoc.mesh->getIdCount("BC_INLT") );
	REVOCAP_Debug("wall data size => %d\n", rcapRefinerDoc.mesh->getIdCount("BC_WALL") );
	REVOCAP_Debug("free data size => %d\n", rcapRefinerDoc.mesh->getIdCount("BC_FREE") );
	size_t dataNum = 0;
	dataNum += rcapRefinerDoc.mesh->getIdCount( "BC_INLT" );
	dataNum += rcapRefinerDoc.mesh->getIdCount( "BC_WALL" );
	dataNum += rcapRefinerDoc.mesh->getIdCount( "BC_FREE" );
	int32_t* nodeArray = new int32_t[dataNum];
	int32_t* nodeVars = new int32_t[dataNum];
	unsigned int i= 0,j=0;
	double inlet_v[3] = {0.0, 0.0, 1.0};
	kmb::DataBindings::iterator dIterInlt = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_INLT" )->begin();
	while( !dIterInlt.isFinished() ){
		nodeArray[i] = dIterInlt.getId();
		dIterInlt.getValue( inlet_v );
		nodeVars[i] = 1;
		++dIterInlt;
		++i;
	}
	kmb::DataBindings::iterator dIterWall = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_WALL" )->begin();
	while( !dIterWall.isFinished() ){
		nodeArray[i] = dIterWall.getId();
		nodeVars[i] = 3;
		++dIterWall;
		++i;
	}
	kmb::DataBindings::iterator dIterFree = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_FREE" )->begin();
	while( !dIterFree.isFinished() ){
		nodeArray[i] = dIterFree.getId();
		nodeVars[i] = 5;
		++dIterFree;
		++i;
	}
	rcapAppendBNodeVarInt( "BC", dataNum, nodeArray, nodeVars );
	delete[] nodeArray;
	delete[] nodeVars;
	REVOCAP_Debug("data size = %d\n", rcapGetBNodeVarIntCount("BC") );


	size_t elementCount = rcapRefinerDoc.mesh->getElementCount(0);
	REVOCAP_Debug("refine element count = %d\n", elementCount );
	int32_t* nodeTable = new int32_t[elementCount*8];
	int8_t* typeTable = new int8_t[elementCount];

	kmb::ElementContainer::const_iterator eIter = rcapRefinerDoc.mesh->getBodyPtr(0)->begin();
	i = 0; j = 0;
	while( !eIter.isFinished() ){
		int num = eIter.getNodeCount();
		for(int k=0;k<num;++k){
			nodeTable[i+k] = eIter.getCellId(k);
		}
		typeTable[j] = eIter.getType();
		i += num;
		++j;
		++eIter;
	}
	REVOCAP_Debug("input data generated.\n");

	size_t refineNum = 0;
	size_t refineNodeCount = 0;
	refineNodeCount = rcapRefineElementMulti( elementCount, typeTable, nodeTable, &refineNum, NULL, NULL );
	REVOCAP_Debug("refineElementCount = %d, refineNodeTableSize = %d\n", refineNum, refineNodeCount);
	int32_t* refineNodeTable = new int32_t[refineNodeCount];
	int8_t* refineTypeTable = new int8_t[refineNum];
	refineNodeTable[0] = 0;
	REVOCAP_Debug("RefineElement\n");
	rcapRefineElementMulti( elementCount, typeTable, nodeTable, &refineNum, refineTypeTable, refineNodeTable );

	REVOCAP_Debug("Commit\n");
	rcapCommit();

	rcapRefinerDoc.mesh->clearBody(0);
	rcapRefinerDoc.mesh->beginElement( refineNum );
	kmb::nodeIdType nodes[20];
	i = 0;
	for(j=0;j<refineNum;++j){
		kmb::elementType etype = static_cast<kmb::elementType>(refineTypeTable[j]);
		int num = kmb::Element::getNodeCount( etype );
		for(int k=0;k<num;++k){
			nodes[k] = refineNodeTable[i+k];
		}
		i += num;
		rcapRefinerDoc.mesh->addElement( etype, nodes );
	}
	rcapRefinerDoc.mesh->endElement();


	REVOCAP_Debug("refined data size = %d\n", rcapRefinerDoc.mesh->getIdCount("BC") );
	kmb::DataBindings::iterator dIterBC = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC" )->begin();
	kmb::DataBindings* inlt = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_INLT" );
	kmb::DataBindings* wall = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_WALL" );
	kmb::DataBindings* free = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_FREE" );
	inlt->clear();
	wall->clear();
	free->clear();
	long l=0;
	while( !dIterBC.isFinished() ){
		if( dIterBC.getValue( &l ) ){
			switch(l){
				case 1:
					inlt->setPhysicalValue( dIterBC.getId(), inlet_v );
					break;
				case 3:
					wall->addId( dIterBC.getId() );
					break;
				case 5:
					free->addId( dIterBC.getId() );
					break;
				default:
					break;
			}
		}
		++dIterBC;
	}
	REVOCAP_Debug("refined inlt data size => %d\n", inlt->getIdCount());
	REVOCAP_Debug("refined wall data size => %d\n", wall->getIdCount());
	REVOCAP_Debug("refined free data size => %d\n", free->getIdCount());

	size_t nodeCount = rcapRefinerDoc.mesh->getNodeCount();
	int8_t etype;
	int32_t orgNodes[20];
	for(j=0;j<nodeCount;++j){
		for(int k=0;k<8;++k){
			orgNodes[k]= -1;
		}
		etype = rcapGetOriginal( j, orgNodes );
		REVOCAP_Debug_3("original type %d => %d [%d %d %d %d %d %d %d %d]\n",
										j ,etype,
										orgNodes[0],
										orgNodes[1],
										orgNodes[2],
										orgNodes[3],
										orgNodes[4],
										orgNodes[5],
										orgNodes[6],
										orgNodes[7]);
	}

	delete[] nodeTable;
	delete[] refineNodeTable;
	delete[] typeTable;
	delete[] refineTypeTable;

	rcapRefinerDoc.mesh->clearBody(1);
	rcapRefinerDoc.mesh->clearBody(2);
}

int32_t rcapSaveGFFile( const char* gffile, const char* bounfile )
{
	kmb::FFBIO ffbIO;
	ffbIO.saveToMeshFile( gffile, rcapRefinerDoc.mesh );
	ffbIO.saveToBoundaryFile( bounfile, rcapRefinerDoc.mesh );
	kmb::RevocapNeutralIO rnfIO;
	rnfIO.saveToRNFFile( "output.rnf" , rcapRefinerDoc.mesh );
	return rcapRefinerDoc.mesh->getElementCount();
}

int32_t rcapSaveHECFile( const char* hecfile )
{
	kmb::HECMWIO hecIO;
	hecIO.saveToFile( hecfile, rcapRefinerDoc.mesh );
	return rcapRefinerDoc.mesh->getElementCount();
}

int32_t rcapSaveRNFFile( const char* rnffile )
{
	kmb::RevocapNeutralIO rnfIO;
	rnfIO.saveToRNFFile( rnffile, rcapRefinerDoc.mesh );
	return rcapRefinerDoc.mesh->getElementCount();
}

/* rcapxxx_  すべて小文字 */
#ifdef FORTRAN90
void rcapgetversion_( void ){ rcapGetVersion(); }
void rcapinitrefiner_( int32_t* nodeOffset, int32_t* elementOffset ){ rcapInitRefiner(*nodeOffset,*elementOffset); }
void rcapclearrefiner_( void ){ rcapClearRefiner(); }
void rcaptermrefiner_( void ){ rcapTermRefiner(); }

void rcapsetcadfilename_( const char* filename ){ rcapSetCADFilename( filename ); }
void rcapsetpartitionfilename_( const char* filename ){ rcapSetPartitionFilename( filename ); }

void rcapsetnode64_( int32_t* num, float64_t* coords, int32_t* globalIds, int32_t* localIds ){
	rcapSetNode64(static_cast<size_t>(*num),coords,globalIds,localIds);
}
void rcapsetnode32_( int32_t* num, float32_t* coords, int32_t* globalIds, int32_t* localIds ){
	rcapSetNode32(static_cast<size_t>(*num),coords,globalIds,localIds);
}
int32_t rcapgetnodecount_( void ){ return static_cast<int32_t>(rcapGetNodeCount()); }
void rcapgetnode64_( int32_t* num, int32_t* localIds, float64_t* coords ){
	rcapGetNode64(static_cast<size_t>(*num),localIds,coords);
}
void rcapgetnode32_( int32_t* num, int32_t* localIds, float32_t* coords ){
	rcapGetNode32(static_cast<size_t>(*num),localIds,coords);
}
void rcapgetnodeseq64_( int32_t* num, int32_t* initId, float64_t* coords ){
	rcapGetNodeSeq64(static_cast<size_t>(*num),static_cast<size_t>(*initId),coords);
}
void rcapgetnodeseq32_( int32_t* num, int32_t* initId, float32_t* coords ){
	rcapGetNodeSeq32(static_cast<size_t>(*num),static_cast<size_t>(*initId),coords);
}

int32_t rcaprefineelement_( int32_t* num, int8_t* etype, int32_t* nodeArray, int32_t* resultNodeArray ){
	return static_cast<int32_t>(rcapRefineElement(static_cast<size_t>(*num),*etype,nodeArray,resultNodeArray));
}

int32_t rcaprefineelementmulti_( int32_t* num, int8_t* etypeArray, int32_t* nodeArray, int32_t* refinedNum, int8_t* resultEtypeArray, int32_t* resultNodeArray ){
	size_t s = static_cast<size_t>(*refinedNum);
	int32_t res = static_cast<int32_t>(rcapRefineElementMulti(static_cast<size_t>(*num),etypeArray,nodeArray,&s,resultEtypeArray,resultNodeArray));
	*refinedNum = static_cast<int32_t>(s);
	return res;
}

void rcapcommit_( void ){
	rcapCommit();
}

void rcapappendnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray ){
	rcapAppendNodeGroup(dataname,static_cast<size_t>(*num),nodeArray);
}
int32_t rcapgetnodegroupcount_( const char dataname[80] ){
	return static_cast<int32_t>(rcapGetNodeGroupCount(dataname));
}
void rcapgetnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray ){
	rcapGetNodeGroup( dataname, static_cast<size_t>(*num), nodeArray );
}

void rcapappendbnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray ){
	rcapAppendBNodeGroup( dataname, static_cast<size_t>(*num), nodeArray );
}

int32_t rcapgetbnodegroupcount_( const char dataname[80] ){
	return static_cast< int32_t >(rcapGetBNodeGroupCount( dataname ));
}

void rcapgetbnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray ){
	rcapGetBNodeGroup( dataname, static_cast<size_t>(*num), nodeArray );
}

void rcapappendbnodevarint_( const char dataname[80], int32_t* num, int32_t* nodeArray, int32_t* nodeVars ){
	rcapAppendBNodeVarInt( dataname, static_cast<size_t>(*num), nodeArray, nodeVars );
}

int32_t rcapgetbnodevarintcount_( const char dataname[80] ){
	return static_cast< int32_t >(rcapGetBNodeVarIntCount( dataname ));
}

void rcapgetbnodevarint_( const char dataname[80], int32_t* num, int32_t* nodeArray, int32_t* nodeVars  ){
	rcapGetBNodeVarInt( dataname, static_cast<size_t>(*num), nodeArray, nodeVars  );
}

void rcapappendelementgroup_( const char dataname[80], int32_t* num, int32_t* elementArray ){
	rcapAppendElementGroup(dataname,static_cast<size_t>(*num),elementArray);
}

int32_t rcapgetelementgroupcount_( const char dataname[80] ){
	return static_cast<int32_t>(rcapGetElementGroupCount(dataname));
}

void rcapgetelementgroup_( const char dataname[80], int32_t* num, int32_t* elementArray ){
	rcapGetElementGroup( dataname, static_cast<size_t>(*num), elementArray );
}

void rcapappendfacegroup_( const char dataname[80], int32_t* num, int32_t* faceArray ){
	rcapAppendFaceGroup(dataname,static_cast<size_t>(*num),faceArray);
}

int32_t rcapgetfacegroupcount_( const char dataname[80] ){
	return static_cast<int32_t>(rcapGetFaceGroupCount(dataname));
}
void rcapgetfacegroup_( const char dataname[80], int32_t* num, int32_t* faceArray ){
	rcapGetElementGroup( dataname, static_cast<size_t>(*num), faceArray );
}

void rcapsetinterpolatemode_( const char mode[32] ){
	rcapSetInterpolateMode(mode);
}
void rcapgetinterpolatemode_( char mode[32] ){
	rcapGetInterpolateMode(mode);
}

int8_t rcapgetoriginal_( int32_t* localNodeId, int32_t* originalNodeArray ){
	return rcapGetOriginal(*localNodeId,originalNodeArray);
}

int32_t rcapgetmiddle_( int8_t *etype, int32_t* originalNodeArray ){
	return rcapGetMiddle(*etype,originalNodeArray);
}

int32_t rcaploadgffile_( const char* gffile, const char* bounfile ){
	return rcapLoadGFFile( gffile, bounfile );
}

int32_t rcaploadhecfile_( const char* hecfile ){
	return rcapLoadHECFile( hecfile );
}

int32_t rcapsavegffile_( const char* gffile, const char* bounfile ){
	return rcapSaveGFFile( gffile, bounfile );
}

int32_t rcapsavehecfile_( const char* hecfile ){
	return rcapSaveHECFile( hecfile );
}

int32_t rcapsavernffile_( const char* hecfile ){
	return rcapSaveRNFFile( hecfile );
}
#endif
