/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshRefiner                                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshGen/kmbMeshRefiner.h"

#include "Common/kmbCalculator.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbHexahedron2.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshGen/kmbMiddleNodeManager.h"

#include <cstring>

kmb::MeshRefiner::MeshRefiner(void)
: mesh(NULL)
, middleMan(NULL)
, middleManDeletable(false)
, interpRule(kmb::MeshRefiner::MIN)
, secondFitting(false)
{
	middleMan = new kmb::MiddleNodeManager();
	middleManDeletable = true;
}

kmb::MeshRefiner::~MeshRefiner(void)
{
	if( middleMan && middleManDeletable ){
		delete middleMan;
		middleMan = NULL;
	}
}

void
kmb::MeshRefiner::clear(void)
{
	middleMan->clear();
}

void
kmb::MeshRefiner::setInterpolationMode(const char* mode)
{
	if( strcmp("MIN",mode) == 0 ){
		this->interpRule = kmb::MeshRefiner::MIN;
	}
	else if( strcmp("MAX",mode) == 0 ){
		this->interpRule = kmb::MeshRefiner::MAX;
	}
	else if( strcmp("MIDDLE",mode) == 0 ){
		this->interpRule = kmb::MeshRefiner::MIDDLE;
	}
}

const char*
kmb::MeshRefiner::getInterpolationMode(void) const
{
	switch( interpRule )
	{
	case kmb::MeshRefiner::MIN:
		return "MIN";
	case kmb::MeshRefiner::MAX:
		return "MAX";
	case kmb::MeshRefiner::MIDDLE:
		return "MIDDLE";
	default:
		break;
	}
	return "";
}

void
kmb::MeshRefiner::setSecondFitting(bool flag)
{
	this->secondFitting = flag;
}

bool
kmb::MeshRefiner::setSecondFitting(void) const
{
	return this->secondFitting;
}

void
kmb::MeshRefiner::setMesh(kmb::MeshData* mesh)
{
	this->mesh = mesh;
	if( middleMan && mesh ){
		middleMan->setNodeContainer( mesh->getNodes() );
	}
}

void
kmb::MeshRefiner::setMiddleNodeManager(kmb::MiddleNodeManager* manager)
{
	if( middleMan && middleManDeletable ){
		delete middleMan;
		middleMan = NULL;
	}
	this->middleMan = manager;
	middleManDeletable = false;
	if( middleMan && mesh ){
		middleMan->setNodeContainer( this->mesh->getNodes() );
	}
}

void
kmb::MeshRefiner::setSecondNodes( kmb::ElementBase& element )
{
	if( middleMan && element.getDegree() == 2 ){
		const int ecount = element.getEdgeCount();
		for(int i=0;i<ecount;++i){
			middleMan->setDividedNode(
				element.getEdgeCellId(i,2),
				element.getEdgeCellId(i,0),
				element.getEdgeCellId(i,1) );
		}
	}
}

void
kmb::MeshRefiner::setSecondNodesByData(const char* name,const char* stype)
{
	if( mesh == NULL){
		return;
	}
	kmb::DataBindings* data = mesh->getDataBindingsPtr(name,stype);
	if( data ){
		switch( data->getBindingMode() )
		{
			case kmb::DataBindings::ElementGroup:
			case kmb::DataBindings::ElementVariable:
				{
					kmb::bodyIdType bodyId = data->getTargetBodyId();
					kmb::DataBindings::iterator eIter = data->begin();
					while( !eIter.isFinished() ){
						kmb::ElementContainer::const_iterator element = mesh->findElement( eIter.getId(), bodyId );
						setSecondNodes( element );
						++eIter;
					}
				}
				break;
			default:
				break;
		}
	}
}

void
kmb::MeshRefiner::setSecondNodesByBody(kmb::bodyIdType bodyId)
{
	if( mesh == NULL){
		return;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr( bodyId );
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( !eIter.isFinished() ){
			kmb::ElementContainer::const_iterator element = mesh->findElement( eIter.getId(), bodyId );
			setSecondNodes( element );
			++eIter;
		}
	}
}

// 要素の個数は同じなので、ElementContainer は変えないことに注意
// Data で更新すべきなのは NodeGroup のみ
kmb::bodyIdType
kmb::MeshRefiner::convertToSecondBody(kmb::bodyIdType bodyId)
{
	kmb::ElementContainer* body = NULL;
	if( mesh == NULL || middleMan == NULL || (body = mesh->getBodyPtr( bodyId )) == NULL ){
		return kmb::Body::nullBodyId;
	}
	// data pair に refined data のコンテナの登録
	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		const kmb::DataBindings* data = dIter->originalData;
		if( data == NULL ){
			return kmb::Body::nullBodyId;
		}
		// 未登録の時にデータベースを作る
		if( data != NULL && dIter->refinedData == NULL ){
			if( data->getBindingMode() == kmb::DataBindings::NodeGroup || data->getBindingMode() == kmb::DataBindings::NodeVariable ){
				dIter->refinedData = kmb::DataBindings::createDataBindings(
					data->getBindingMode(),
					data->getValueType(),
					data->getSpecType(),
					data->getTargetBodyId());
				if( dIter->refinedData == NULL ){
					return kmb::Body::nullBodyId;
				}
			}
		}
		++dIter;
	}

	kmb::nodeIdType* nodes = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	kmb::ElementContainer::iterator eIter = body->begin();
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	while( !eIter.isFinished() ){
		kmb::elementIdType id = eIter.getId();
		const int degree = eIter.getDegree();
		eIter.getElement( etype, nodes );
		kmb::elementType secondType = kmb::Element::getSecondType( etype );
		if( degree == 1 ){
			const int ecount = eIter.getEdgeCount();
			const int vcount = eIter.getVertexCount();
			for(int i=0;i<ecount;++i){
				// 1 次要素の getEdgeCellId で与えられる順番と
				// 2 次要素の nodes で与える順番は等しいことが保証されている
				kmb::nodeIdType n0 = eIter.getEdgeCellId(i,0);
				kmb::nodeIdType n1 = eIter.getEdgeCellId(i,1);
				kmb::nodeIdType n01 = nodes[vcount+i] = middleMan->getDividedNode( n0, n1 );
				std::vector< kmb::MeshRefiner::DataPair >::iterator pIter = dataPairs.begin();
				while( pIter != dataPairs.end() ){
					if( pIter->originalData && pIter->refinedData ){
						if( pIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
							nodeGroupUpdate( n0, n1, n01, pIter->originalData, pIter->refinedData );
						}
						else if( pIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
							pIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
						{
							nodeVariableUpdate( n0, n1, n01, pIter->originalData, pIter->refinedData );
						}
					}
					++pIter;
				}
			}
			++eIter;
			if( !body->deleteElement(id) || body->addElement( secondType, nodes, id ) == kmb::Element::nullElementId ){
				// failure!
				delete[] nodes;
				return kmb::Body::nullBodyId;
			}
		}else{
			++eIter;
		}
	}
	delete[] nodes;
	commitData();
	return bodyId;
}

size_t
kmb::MeshRefiner::getRefinedElementCount(const kmb::ElementContainer* orgBody) const
{
	size_t num = 0;
	if( orgBody == NULL ){
		return num;
	}
	num += orgBody->getCountByType( kmb::SEGMENT ) * 2;
	num += orgBody->getCountByType( kmb::SEGMENT2 ) * 2;
	num += orgBody->getCountByType( kmb::TRIANGLE ) * 4;
	num += orgBody->getCountByType( kmb::TRIANGLE2 ) * 4;
	num += orgBody->getCountByType( kmb::QUAD ) * 4;
	num += orgBody->getCountByType( kmb::QUAD2 ) * 4;
	num += orgBody->getCountByType( kmb::TETRAHEDRON ) * 8;
	num += orgBody->getCountByType( kmb::TETRAHEDRON2 ) * 8;
	num += orgBody->getCountByType( kmb::HEXAHEDRON ) * 8;
	num += orgBody->getCountByType( kmb::HEXAHEDRON2 ) * 8;
	num += orgBody->getCountByType( kmb::WEDGE ) * 8;
	num += orgBody->getCountByType( kmb::WEDGE2 ) * 8;
	num += orgBody->getCountByType( kmb::PYRAMID ) * 10;
	num += orgBody->getCountByType( kmb::PYRAMID2 ) * 10;
	return num;
}

void
kmb::MeshRefiner::appendData(const char* dataname,const char* stype)
{
	if( mesh != NULL ){
		kmb::DataBindings* d = mesh->getDataBindingsPtr( dataname, stype );
		if( d != NULL ){
			if( stype != NULL ){
				kmb::MeshRefiner::DataPair pair = {dataname,stype,d,NULL};
				dataPairs.push_back( pair );
			}else{
				kmb::MeshRefiner::DataPair pair = {dataname,d->getSpecType().c_str(),d,NULL};
				dataPairs.push_back( pair );
			}
		}
	}
}

size_t
kmb::MeshRefiner::getDataCount(void) const
{
	return dataPairs.size();
}

void
kmb::MeshRefiner::clearData(void)
{
	dataPairs.clear();
}

void
kmb::MeshRefiner::commitData(void)
{
	// dataPairs について original data を refined data に置き換え
	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->refinedData ){
			if( dIter->originalData ){
				bool res = mesh->replaceData( dIter->originalData, dIter->refinedData, dIter->name.c_str(), dIter->stype.c_str() );
				if( !res ){
					// replaceData できなかった
					// このとき mesh で getDataBindingPtr で参照されるのは originalData のまま
					delete dIter->refinedData;
					dIter->refinedData = NULL;
				}else{
					delete dIter->originalData;
					dIter->originalData = NULL;
				}
			}else{
				// original = NULL, refined != NULL の場合（あり得ないけど）
				// refinedData を name に割り当てておく
				mesh->setDataBindingsPtr( dIter->name.c_str(), dIter->refinedData, dIter->stype.c_str() );
			}
			if( dIter->originalData == NULL ){
				dIter->originalData = dIter->refinedData;
				dIter->refinedData = NULL;
			}
		}
		++dIter;
	}
}

kmb::bodyIdType
kmb::MeshRefiner::refineBody(kmb::bodyIdType orgBodyId)
{
	kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
	const kmb::ElementContainer* orgBody = NULL;
	if( mesh && (orgBody=mesh->getBodyPtr( orgBodyId )) != NULL )
	{
		size_t eCount = getRefinedElementCount( orgBody );
		bodyId = mesh->beginElement( eCount );
		mesh->endElement();
		kmb::ElementContainer* refinedBody = mesh->getBodyPtr( bodyId );
		if( refinedBody ){
			refineBody( orgBody, refinedBody );
			const kmb::BoundingBox bbox = orgBody->getBoundingBox();
			refinedBody->setBoundingBox( bbox.getMax(), bbox.getMin() );
		}
		mesh->updateBody( bodyId );
		mesh->setBodyName( bodyId, mesh->getBodyName(orgBodyId) );
		// face group の target body id を更新
		// body group or body variable は書き換え
		std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
		while( dIter != dataPairs.end() ){
			if( dIter->refinedData && dIter->refinedData->getBindingMode() == kmb::DataBindings::FaceGroup ){
				if( dIter->refinedData->getTargetBodyId() == orgBodyId ){
					dIter->refinedData->setTargetBodyId( bodyId );
				}
			}
			++dIter;
		}
		commitData();
	}
	return bodyId;
}

size_t
kmb::MeshRefiner::refineBody(const kmb::ElementContainer* orgBody, kmb::ElementContainer* refinedBody)
{
	if( mesh == NULL || middleMan == NULL || orgBody == NULL ){
		return 0;
	}
	// data pair に refined data のコンテナの登録
	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		const kmb::DataBindings* data = dIter->originalData;
		if( data == NULL ){
			return 0;
		}
		// 未登録の時にデータベースを作る
		if( data != NULL && dIter->refinedData == NULL ){
			dIter->refinedData = kmb::DataBindings::createDataBindings(
				data->getBindingMode(),
				data->getValueType(),
				data->getSpecType(),
				data->getTargetBodyId());
			if( dIter->refinedData == NULL ){
				return 0;
			}
		}
		++dIter;
	}

	size_t oldCount = (refinedBody) ? refinedBody->getCount() : 0;
	kmb::ElementContainer::const_iterator eIter = orgBody->begin();
	while( !eIter.isFinished() ){
		switch( eIter.getType() )
		{
		case kmb::SEGMENT:		refineSegment( eIter.getId(), eIter, refinedBody );		break;
		case kmb::SEGMENT2:		refineSegment2( eIter.getId(), eIter, refinedBody );	break;
		case kmb::TRIANGLE:		refineTriangle( eIter.getId(), eIter, refinedBody );	break;
		case kmb::TRIANGLE2:	refineTriangle2( eIter.getId(), eIter, refinedBody );	break;
		case kmb::QUAD:			refineQuad( eIter.getId(), eIter, refinedBody );		break;
		case kmb::QUAD2:		refineQuad2( eIter.getId(), eIter, refinedBody );		break;
		case kmb::TETRAHEDRON:	refineTetrahedron( eIter.getId(), eIter, refinedBody );	break;
		case kmb::TETRAHEDRON2:	refineTetrahedron2( eIter.getId(), eIter, refinedBody );break;
		case kmb::HEXAHEDRON:	refineHexahedron( eIter.getId(), eIter, refinedBody );	break;
		case kmb::HEXAHEDRON2:	refineHexahedron2( eIter.getId(), eIter, refinedBody );	break;
		case kmb::WEDGE:		refineWedge( eIter.getId(), eIter, refinedBody );		break;
		case kmb::WEDGE2:		refineWedge2( eIter.getId(), eIter, refinedBody );		break;
		case kmb::PYRAMID:		refinePyramid( eIter.getId(), eIter, refinedBody );		break;
		case kmb::PYRAMID2:		refinePyramid2( eIter.getId(), eIter, refinedBody );	break;
		default:	break;
		}
		++eIter;
	}
	if( refinedBody ){
		return refinedBody->getCount() - oldCount;
	}else{
		return 0;
	}
}

//
// [n0,n1]
// => [n0,n01]
//    [n01,n1]
//
void
kmb::MeshRefiner::refineSegment( kmb::elementIdType elementId, const kmb::ElementBase &segment, kmb::ElementContainer* refinedBody )
{
	kmb::nodeIdType nodeTable[2];
	kmb::elementIdType e[2] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId};
	kmb::nodeIdType n01 = middleMan->getDividedNode( segment.getCellId(0), segment.getCellId(1), elementId );
	if( refinedBody ){
		nodeTable[0] = segment.getCellId(0); nodeTable[1] = n01;
		e[0] = refinedBody->addElement( kmb::SEGMENT, nodeTable );
		nodeTable[0] = n01; nodeTable[1] = segment.getCellId(01);
		e[1] = refinedBody->addElement( kmb::SEGMENT, nodeTable );
	}
	const int faceNum = kmb::Element::getBoundaryCount( kmb::SEGMENT );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			nodeGroupUpdate( segment.getCellId(0), segment.getCellId(1), n01, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			nodeVariableUpdate( segment.getCellId(0), segment.getCellId(1), n01, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					// 細分しても SEGMENT の FaceGroup は増えない
					dIter->refinedData->addId( kmb::Face(e[i],i) );
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<2;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

//
// [n0,n1,n2]
// => [n0,n2,n02]
//    [n2,n1,n12]
//
void
kmb::MeshRefiner::refineSegment2( kmb::elementIdType elementId, const kmb::ElementBase &segment2, kmb::ElementContainer* refinedBody )
{
	kmb::nodeIdType nodeTable[3];
	kmb::elementIdType e[2] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId};
	kmb::nodeIdType n02 = kmb::nullNodeId;
	kmb::nodeIdType n12 = kmb::nullNodeId;
	if( secondFitting ){
		n02 = middleMan->getDividedNode3( segment2[0], segment2[2], segment2[1] );
		n12 = middleMan->getDividedNode3( segment2[1], segment2[2], segment2[0] );
	}else{
		n02 = middleMan->getDividedNode( segment2[0], segment2[2] );
		n12 = middleMan->getDividedNode( segment2[1], segment2[2] );
	}
	if( refinedBody ){
		nodeTable[0] = segment2.getCellId(0);
		nodeTable[1] = segment2.getCellId(2);
		nodeTable[2] = n02;
		e[0] = refinedBody->addElement( kmb::SEGMENT2, nodeTable );
		nodeTable[0] = segment2.getCellId(2);
		nodeTable[1] = segment2.getCellId(1);
		nodeTable[2] = n12;
		e[1] = refinedBody->addElement( kmb::SEGMENT2, nodeTable );
	}
	const int faceNum = kmb::Element::getBoundaryCount( kmb::SEGMENT2 );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			nodeGroupUpdate( segment2.getCellId(0), segment2.getCellId(2), n02, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( segment2.getCellId(1), segment2.getCellId(2), n12, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			nodeVariableUpdate( segment2.getCellId(0), segment2.getCellId(2), n02, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( segment2.getCellId(1), segment2.getCellId(2), n12, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					// 細分しても SEGMENT の FaceGroup は増えない
					dIter->refinedData->addId( kmb::Face(e[i],i) );
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<2;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// face の順番は親を引き継ぐ
// [n0,n1,n2]
// => [n0,n01,n02]
//    [n01,n1,n12]
//    [n02,n12,n2]
//    [n12,n02,n01]
void
kmb::MeshRefiner::refineTriangle( kmb::elementIdType elementId, const kmb::ElementBase &triangle, kmb::ElementContainer* refinedBody )
{
	kmb::nodeIdType nodeTable[3];
	kmb::elementIdType e[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType n12 = middleMan->getCenterNode( triangle, 0, elementId );
	kmb::nodeIdType n02 = middleMan->getCenterNode( triangle, 1, elementId );
	kmb::nodeIdType n01 = middleMan->getCenterNode( triangle, 2, elementId );
	if( refinedBody ){
		nodeTable[0] = triangle.getCellId(0);
		nodeTable[1] = n01;
		nodeTable[2] = n02;
		e[0] = refinedBody->addElement( kmb::TRIANGLE, nodeTable );

		nodeTable[0] = n01;
		nodeTable[1] = triangle.getCellId(1);
		nodeTable[2] = n12;
		e[1] = refinedBody->addElement( kmb::TRIANGLE, nodeTable );

		nodeTable[0] = n02;
		nodeTable[1] = n12;
		nodeTable[2] = triangle.getCellId(2);
		e[2] = refinedBody->addElement( kmb::TRIANGLE, nodeTable );

		nodeTable[0] = n12;
		nodeTable[1] = n02;
		nodeTable[2] = n01;
		e[3] = refinedBody->addElement( kmb::TRIANGLE, nodeTable );
	}

	const int faceNum = kmb::Element::getBoundaryCount( kmb::TRIANGLE );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			// Node Group の更新
			nodeGroupUpdate( triangle.getCellId(0), triangle.getCellId(1), n01, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle.getCellId(0), triangle.getCellId(2), n02, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle.getCellId(1), triangle.getCellId(2), n12, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			// Node Variable の更新
			nodeVariableUpdate( triangle.getCellId(0), triangle.getCellId(1), n01, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle.getCellId(0), triangle.getCellId(2), n02, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle.getCellId(1), triangle.getCellId(2), n12, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			// Face Group の更新
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					switch(i)
					{
					case 0:
						dIter->refinedData->addId( kmb::Face(e[1],0) ); // [1,12]
						dIter->refinedData->addId( kmb::Face(e[2],0) ); // [12,2]
						break;
					case 1:
						dIter->refinedData->addId( kmb::Face(e[2],1) ); // [2,02]
						dIter->refinedData->addId( kmb::Face(e[0],1) ); // [02,0]
						break;
					case 2:
						dIter->refinedData->addId( kmb::Face(e[0],2) ); // [0,01]
						dIter->refinedData->addId( kmb::Face(e[1],2) ); // [01,1]
						break;
					default:
						break;
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			// Element Group の更新
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<4;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// face の順番は親を引き継ぐ
// [n0,n1,n2,n3,n4,n5]
// => [n0,n5,n4,n45,n04,n05]
//    [n5,n1,n3,n13,n35,n15]
//    [n4,n3,n2,n23,n24,n34]
//    [n3,n4,n5,n45,n35,n34]
void
kmb::MeshRefiner::refineTriangle2( kmb::elementIdType elementId, const kmb::ElementBase &triangle2, kmb::ElementContainer* refinedBody )
{
	kmb::nodeIdType nodeTable[6];
	kmb::elementIdType e[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType n04 = kmb::nullNodeId;
	kmb::nodeIdType n05 = kmb::nullNodeId;
	kmb::nodeIdType n13 = kmb::nullNodeId;
	kmb::nodeIdType n15 = kmb::nullNodeId;
	kmb::nodeIdType n23 = kmb::nullNodeId;
	kmb::nodeIdType n24 = kmb::nullNodeId;
	kmb::nodeIdType n34 = kmb::nullNodeId;
	kmb::nodeIdType n35 = kmb::nullNodeId;
	kmb::nodeIdType n45 = kmb::nullNodeId;
	if( secondFitting ){
		n04 = middleMan->getDividedNode3( triangle2[0], triangle2[4], triangle2[2] );
		n05 = middleMan->getDividedNode3( triangle2[0], triangle2[5], triangle2[1] );
		n13 = middleMan->getDividedNode3( triangle2[1], triangle2[3], triangle2[2] );
		n15 = middleMan->getDividedNode3( triangle2[1], triangle2[5], triangle2[0] );
		n23 = middleMan->getDividedNode3( triangle2[2], triangle2[3], triangle2[1] );
		n24 = middleMan->getDividedNode3( triangle2[2], triangle2[4], triangle2[0] );
		n34 = middleMan->getDividedNode5( triangle2[3], triangle2[4], triangle2[1], triangle2[0], triangle2[5] );
		n35 = middleMan->getDividedNode5( triangle2[3], triangle2[5], triangle2[2], triangle2[0], triangle2[4] );
		n45 = middleMan->getDividedNode5( triangle2[4], triangle2[5], triangle2[2], triangle2[1], triangle2[3] );
	}else{
		n04 = middleMan->getDividedNode( triangle2[0], triangle2[4] );
		n05 = middleMan->getDividedNode( triangle2[0], triangle2[5] );
		n13 = middleMan->getDividedNode( triangle2[1], triangle2[3] );
		n15 = middleMan->getDividedNode( triangle2[1], triangle2[5] );
		n23 = middleMan->getDividedNode( triangle2[2], triangle2[3] );
		n24 = middleMan->getDividedNode( triangle2[2], triangle2[4] );
		n34 = middleMan->getDividedNode( triangle2[3], triangle2[4] );
		n35 = middleMan->getDividedNode( triangle2[3], triangle2[5] );
		n45 = middleMan->getDividedNode( triangle2[4], triangle2[5] );
	}
	if( refinedBody ){
		nodeTable[0] = triangle2.getCellId(0);
		nodeTable[1] = triangle2.getCellId(5);
		nodeTable[2] = triangle2.getCellId(4);
		nodeTable[3] = n45;
		nodeTable[4] = n04;
		nodeTable[5] = n05;
		e[0] = refinedBody->addElement( kmb::TRIANGLE2, nodeTable );

		nodeTable[0] = triangle2.getCellId(5);
		nodeTable[1] = triangle2.getCellId(1);
		nodeTable[2] = triangle2.getCellId(3);
		nodeTable[3] = n13;
		nodeTable[4] = n35;
		nodeTable[5] = n15;
		e[1] = refinedBody->addElement( kmb::TRIANGLE2, nodeTable );

		nodeTable[0] = triangle2.getCellId(4);
		nodeTable[1] = triangle2.getCellId(3);
		nodeTable[2] = triangle2.getCellId(2);
		nodeTable[3] = n23;
		nodeTable[4] = n24;
		nodeTable[5] = n34;
		e[2] = refinedBody->addElement( kmb::TRIANGLE2, nodeTable );

		nodeTable[0] = triangle2.getCellId(3);
		nodeTable[1] = triangle2.getCellId(4);
		nodeTable[2] = triangle2.getCellId(5);
		nodeTable[3] = n45;
		nodeTable[4] = n35;
		nodeTable[5] = n34;
		e[3] = refinedBody->addElement( kmb::TRIANGLE2, nodeTable );
	}

	const int faceNum = kmb::Element::getBoundaryCount( kmb::TRIANGLE2 );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			// Node Group の更新
			nodeGroupUpdate( triangle2.getCellId(0), triangle2.getCellId(4), n04, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(0), triangle2.getCellId(5), n05, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(1), triangle2.getCellId(3), n13, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(1), triangle2.getCellId(5), n15, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(2), triangle2.getCellId(3), n23, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(2), triangle2.getCellId(4), n24, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(3), triangle2.getCellId(4), n34, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(3), triangle2.getCellId(5), n35, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( triangle2.getCellId(4), triangle2.getCellId(5), n45, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			// Node Variable の更新
			nodeVariableUpdate( triangle2.getCellId(0), triangle2.getCellId(4), n04, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(0), triangle2.getCellId(5), n05, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(1), triangle2.getCellId(3), n13, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(1), triangle2.getCellId(5), n15, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(2), triangle2.getCellId(3), n23, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(2), triangle2.getCellId(4), n24, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(3), triangle2.getCellId(4), n34, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(3), triangle2.getCellId(5), n35, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( triangle2.getCellId(4), triangle2.getCellId(5), n45, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			// Face Group の更新
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					switch(i)
					{
					case 0:
						dIter->refinedData->addId( kmb::Face(e[1],0) ); // [1,3]
						dIter->refinedData->addId( kmb::Face(e[2],0) ); // [3,2]
						break;
					case 1:
						dIter->refinedData->addId( kmb::Face(e[2],1) ); // [2,4]
						dIter->refinedData->addId( kmb::Face(e[0],1) ); // [4,0]
						break;
					case 2:
						dIter->refinedData->addId( kmb::Face(e[0],2) ); // [0,5]
						dIter->refinedData->addId( kmb::Face(e[1],2) ); // [5,1]
						break;
					default:
						break;
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			// Element Group の更新
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<4;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3 ]
// => [n0, n01, c, n30]
// => [n01, n1, n12, c]
// => [c, n12, n2, n23]
// => [n30, c, n23, n3]
void
kmb::MeshRefiner::refineQuad( kmb::elementIdType elementId, const kmb::ElementBase &quad, kmb::ElementContainer* refinedBody )
{
	kmb::nodeIdType nodeTable[4];
	kmb::elementIdType e[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId};
	kmb::nodeIdType n01 = middleMan->getCenterNode( quad, 0, elementId );
	kmb::nodeIdType n12 = middleMan->getCenterNode( quad, 1, elementId );
	kmb::nodeIdType n23 = middleMan->getCenterNode( quad, 2, elementId );
	kmb::nodeIdType n30 = middleMan->getCenterNode( quad, 3, elementId );
	kmb::nodeIdType c   = middleMan->getCenterNode( quad, elementId );
	if( refinedBody ){
		nodeTable[0] = quad.getCellId(0);
		nodeTable[1] = n01;
		nodeTable[2] = c;
		nodeTable[3] = n30;
		e[0] = refinedBody->addElement( kmb::QUAD, nodeTable );

		nodeTable[0] = n01;
		nodeTable[1] = quad.getCellId(1);
		nodeTable[2] = n12;
		nodeTable[3] = c;
		e[1] = refinedBody->addElement( kmb::QUAD, nodeTable );

		nodeTable[0] = c;
		nodeTable[1] = n12;
		nodeTable[2] = quad.getCellId(2);
		nodeTable[3] = n23;
		e[2] = refinedBody->addElement( kmb::QUAD, nodeTable );

		nodeTable[0] = n30;
		nodeTable[1] = c;
		nodeTable[2] = n23;
		nodeTable[3] = quad.getCellId(3);
		e[3] = refinedBody->addElement( kmb::QUAD, nodeTable );
	}

	const int faceNum = kmb::Element::getBoundaryCount( kmb::QUAD );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			// Node Group の更新
			nodeGroupUpdate( quad, 0, n01, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad, 1, n12, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad, 2, n23, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad, 3, n30, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad, c, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			// Node Variable の更新
			nodeVariableUpdate( quad, 0, n01, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad, 1, n12, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad, 2, n23, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad, 3, n30, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad, c, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			// Face Group の更新
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					switch(i)
					{
					case 0:
						dIter->refinedData->addId( kmb::Face(e[0],0) ); // [n0,n01]
						dIter->refinedData->addId( kmb::Face(e[1],0) ); // [n01,n1]
						break;
					case 1:
						dIter->refinedData->addId( kmb::Face(e[1],1) ); // [n1,n12]
						dIter->refinedData->addId( kmb::Face(e[2],1) ); // [n12,n2]
						break;
					case 2:
						dIter->refinedData->addId( kmb::Face(e[2],2) ); // [n2,n23]
						dIter->refinedData->addId( kmb::Face(e[3],2) ); // [n23,n3]
						break;
					case 3:
						dIter->refinedData->addId( kmb::Face(e[3],3) ); // [n3,n30]
						dIter->refinedData->addId( kmb::Face(e[0],3) ); // [n30,n0]
						break;
					default:
						break;
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			// Element Group の更新
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<4;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4, n5, n6, n7 ]
// => [n0, n4, c,  n7, n04, n4c, n7c, n07]
// => [n4, n1, n5, c,  n14, n15, n5c, n4c]
// => [c,  n5, n2, n6, n5c, n25, n26, n6c]
// => [n7, c,  n6, n3, n7c, n6c, n36, n37]
void
kmb::MeshRefiner::refineQuad2( kmb::elementIdType elementId, const kmb::ElementBase &quad2, kmb::ElementContainer* refinedBody )
{
	kmb::nodeIdType nodeTable[8];
	kmb::elementIdType e[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType c   = kmb::nullNodeId;
	kmb::nodeIdType n04 = kmb::nullNodeId;
	kmb::nodeIdType n14 = kmb::nullNodeId;
	kmb::nodeIdType n15 = kmb::nullNodeId;
	kmb::nodeIdType n25 = kmb::nullNodeId;
	kmb::nodeIdType n26 = kmb::nullNodeId;
	kmb::nodeIdType n36 = kmb::nullNodeId;
	kmb::nodeIdType n37 = kmb::nullNodeId;
	kmb::nodeIdType n07 = kmb::nullNodeId;
	kmb::nodeIdType n4c = kmb::nullNodeId;
	kmb::nodeIdType n5c = kmb::nullNodeId;
	kmb::nodeIdType n6c = kmb::nullNodeId;
	kmb::nodeIdType n7c = kmb::nullNodeId;
	if( secondFitting ){
		c   = middleMan->getCenterNode2( quad2, elementId );
		n04 = middleMan->getDividedNode3( quad2[0], quad2[4], quad2[1] );
		n14 = middleMan->getDividedNode3( quad2[1], quad2[4], quad2[0] );
		n15 = middleMan->getDividedNode3( quad2[1], quad2[5], quad2[2] );
		n25 = middleMan->getDividedNode3( quad2[2], quad2[5], quad2[1] );
		n26 = middleMan->getDividedNode3( quad2[2], quad2[6], quad2[3] );
		n36 = middleMan->getDividedNode3( quad2[3], quad2[6], quad2[2] );
		n37 = middleMan->getDividedNode3( quad2[3], quad2[7], quad2[0] );
		n07 = middleMan->getDividedNode3( quad2[0], quad2[7], quad2[3] );
		n4c = middleMan->getDividedNode3( quad2[4], c, quad2[6] );
		n5c = middleMan->getDividedNode3( quad2[5], c, quad2[7] );
		n6c = middleMan->getDividedNode3( quad2[6], c, quad2[4] );
		n7c = middleMan->getDividedNode3( quad2[7], c, quad2[5] );
	}else{
		c   = middleMan->getCenterNode( quad2, elementId );
		n04 = middleMan->getDividedNode( quad2[0], quad2[4] );
		n14 = middleMan->getDividedNode( quad2[1], quad2[4] );
		n15 = middleMan->getDividedNode( quad2[1], quad2[5] );
		n25 = middleMan->getDividedNode( quad2[2], quad2[5] );
		n26 = middleMan->getDividedNode( quad2[2], quad2[6] );
		n36 = middleMan->getDividedNode( quad2[3], quad2[6] );
		n37 = middleMan->getDividedNode( quad2[3], quad2[7] );
		n07 = middleMan->getDividedNode( quad2[0], quad2[7] );
		n4c = middleMan->getDividedNode( quad2[4], c );
		n5c = middleMan->getDividedNode( quad2[5], c );
		n6c = middleMan->getDividedNode( quad2[6], c );
		n7c = middleMan->getDividedNode( quad2[7], c );
	}
	if( refinedBody ){
		nodeTable[0] = quad2.getCellId(0);
		nodeTable[1] = quad2.getCellId(4);
		nodeTable[2] = c;
		nodeTable[3] = quad2.getCellId(7);
		nodeTable[4] = n04;
		nodeTable[5] = n4c;
		nodeTable[6] = n7c;
		nodeTable[7] = n07;
		e[0] = refinedBody->addElement( kmb::QUAD2, nodeTable );

		nodeTable[0] = quad2.getCellId(4);
		nodeTable[1] = quad2.getCellId(1);
		nodeTable[2] = quad2.getCellId(5);
		nodeTable[3] = c;
		nodeTable[4] = n14;
		nodeTable[5] = n15;
		nodeTable[6] = n5c;
		nodeTable[7] = n4c;
		e[1] = refinedBody->addElement( kmb::QUAD2, nodeTable );

		nodeTable[0] = c;
		nodeTable[1] = quad2.getCellId(5);
		nodeTable[2] = quad2.getCellId(2);
		nodeTable[3] = quad2.getCellId(6);
		nodeTable[4] = n5c;
		nodeTable[5] = n25;
		nodeTable[6] = n26;
		nodeTable[7] = n6c;
		e[2] = refinedBody->addElement( kmb::QUAD2, nodeTable );

		nodeTable[0] = quad2.getCellId(7);
		nodeTable[1] = c;
		nodeTable[2] = quad2.getCellId(6);
		nodeTable[3] = quad2.getCellId(3);
		nodeTable[4] = n7c;
		nodeTable[5] = n6c;
		nodeTable[6] = n36;
		nodeTable[7] = n37;
		e[3] = refinedBody->addElement( kmb::QUAD2, nodeTable );
	}

	const int faceNum = kmb::Element::getBoundaryCount( kmb::QUAD2 );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			// Node Group の更新
			nodeGroupUpdate( quad2, c, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(0), quad2.getCellId(4), n04, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(1), quad2.getCellId(4), n14, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(1), quad2.getCellId(5), n15, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(2), quad2.getCellId(5), n25, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(2), quad2.getCellId(6), n26, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(3), quad2.getCellId(6), n36, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(3), quad2.getCellId(7), n37, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(0), quad2.getCellId(7), n07, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(4), c, n4c, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(5), c, n5c, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(6), c, n6c, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( quad2.getCellId(7), c, n7c, dIter->refinedData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			// Node Variable の更新
			nodeVariableUpdate( quad2, c, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(0), quad2.getCellId(4), n04, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(1), quad2.getCellId(4), n14, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(1), quad2.getCellId(5), n15, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(2), quad2.getCellId(5), n25, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(2), quad2.getCellId(6), n26, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(3), quad2.getCellId(6), n36, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(3), quad2.getCellId(7), n37, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(0), quad2.getCellId(7), n07, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(4), c, n4c, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(5), c, n5c, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(6), c, n6c, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( quad2.getCellId(7), c, n7c, dIter->refinedData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			// Face Group の更新
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					switch(i)
					{
					case 0:
						dIter->refinedData->addId( kmb::Face(e[0],0) ); // [n0,n4,n04]
						dIter->refinedData->addId( kmb::Face(e[1],0) ); // [n4,n1,n14]
						break;
					case 1:
						dIter->refinedData->addId( kmb::Face(e[1],1) ); // [n1,n5,n15]
						dIter->refinedData->addId( kmb::Face(e[2],1) ); // [n5,n2,n25]
						break;
					case 2:
						dIter->refinedData->addId( kmb::Face(e[2],2) ); // [n2,n6,n26]
						dIter->refinedData->addId( kmb::Face(e[3],2) ); // [n6,n3,n36]

						break;
					case 3:
						dIter->refinedData->addId( kmb::Face(e[3],3) ); // [n3,n7,n37]
						dIter->refinedData->addId( kmb::Face(e[0],3) ); // [n7,n0,n07]
						break;
					default:
						break;
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			// Element Group の更新
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<4;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3 ]
// => [n0,  n01, n02, n03]
// => [n01, n1,  n12, n13]
// => [n02, n12, n2 , n23]
// => [n03, n13, n23, n3]
// 残り４つは座標に依存
// elementId e[0] => n0 についている要素 faceの順番は親を引き継ぐ
//           e[1] => n1
//           e[2] => n2
//           e[3] => n3
//           ef[0] => face0 についている要素 0番目の面が外側
//           ef[1] => face1 についている要素 0番目の面が外側
//           ef[2] => face2 についている要素 0番目の面が外側
//           ef[3] => face3 についている要素 0番目の面が外側
void
kmb::MeshRefiner::refineTetrahedron( kmb::elementIdType elementId, const kmb::ElementBase &tetra, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId};
	kmb::elementIdType ef[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId};
	kmb::nodeIdType nodeTable[4];
	kmb::nodeIdType n12 = middleMan->getDividedNode( tetra.getCellId(1), tetra.getCellId(2) );
	kmb::nodeIdType n02 = middleMan->getDividedNode( tetra.getCellId(0), tetra.getCellId(2) );
	kmb::nodeIdType n01 = middleMan->getDividedNode( tetra.getCellId(0), tetra.getCellId(1) );
	kmb::nodeIdType n03 = middleMan->getDividedNode( tetra.getCellId(0), tetra.getCellId(3) );
	kmb::nodeIdType n13 = middleMan->getDividedNode( tetra.getCellId(1), tetra.getCellId(3) );
	kmb::nodeIdType n23 = middleMan->getDividedNode( tetra.getCellId(2), tetra.getCellId(3) );

	if( refinedBody ){
		nodeTable[0] = tetra.getCellId(0);
		nodeTable[1] = n01;
		nodeTable[2] = n02;
		nodeTable[3] = n03;
		e[0] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		nodeTable[0] = n01;
		nodeTable[1] = tetra.getCellId(1);
		nodeTable[2] = n12;
		nodeTable[3] = n13;
		e[1] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		nodeTable[0] = n02;
		nodeTable[1] = n12;
		nodeTable[2] = tetra.getCellId(2);
		nodeTable[3] = n23;
		e[2] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		nodeTable[0] = n03;
		nodeTable[1] = n13;
		nodeTable[2] = n23;
		nodeTable[3] = tetra.getCellId(3);
		e[3] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		// 内部の４要素の判定
		// 対角線の一番短いところを辺にする
		// 対角線を最初の２点にする
		// 外に面している面を０番目の面にする
		double d01_23 = mesh->getNodes()->distanceSq( n01, n23 );
		double d02_13 = mesh->getNodes()->distanceSq( n02, n13 );
		double d03_12 = mesh->getNodes()->distanceSq( n03, n12 );
		if( d03_12 <= d01_23 && d03_12 <= d02_13 ){
			// 03 -- 12

			nodeTable[0] = n03;
			nodeTable[1] = n12;
			nodeTable[2] = n23;
			nodeTable[3] = n13;
			ef[0] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n12;
			nodeTable[1] = n03;
			nodeTable[2] = n23;
			nodeTable[3] = n02;
			ef[1] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n12;
			nodeTable[1] = n03;
			nodeTable[2] = n01;
			nodeTable[3] = n13;
			ef[2] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n03;
			nodeTable[1] = n12;
			nodeTable[2] = n01;
			nodeTable[3] = n02;
			ef[3] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		}else if( d02_13 <= d01_23 && d02_13 <= d03_12 ){
			// 02 -- 13

			nodeTable[0] = n02;
			nodeTable[1] = n13;
			nodeTable[2] = n12;
			nodeTable[3] = n23;
			ef[0] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n13;
			nodeTable[1] = n02;
			nodeTable[2] = n03;
			nodeTable[3] = n23;
			ef[1] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n02;
			nodeTable[1] = n13;
			nodeTable[2] = n03;
			nodeTable[3] = n01;
			ef[2] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n13;
			nodeTable[1] = n02;
			nodeTable[2] = n12;
			nodeTable[3] = n01;
			ef[3] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		}else{
			// 01 -- 23

			nodeTable[0] = n01;
			nodeTable[1] = n23;
			nodeTable[2] = n13;
			nodeTable[3] = n12;
			ef[0] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n01;
			nodeTable[1] = n23;
			nodeTable[2] = n02;
			nodeTable[3] = n03;
			ef[1] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n23;
			nodeTable[1] = n01;
			nodeTable[2] = n13;
			nodeTable[3] = n03;
			ef[2] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

			nodeTable[0] = n23;
			nodeTable[1] = n01;
			nodeTable[2] = n02;
			nodeTable[3] = n12;
			ef[3] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		}
	}
	const int faceNum = kmb::Element::getBoundaryCount( kmb::TETRAHEDRON );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			nodeGroupUpdate( tetra.getCellId(1), tetra.getCellId(2), n12, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra.getCellId(0), tetra.getCellId(2), n02, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra.getCellId(0), tetra.getCellId(1), n01, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra.getCellId(0), tetra.getCellId(3), n03, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra.getCellId(1), tetra.getCellId(3), n13, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra.getCellId(2), tetra.getCellId(3), n23, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			nodeVariableUpdate( tetra.getCellId(1), tetra.getCellId(2), n12, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra.getCellId(0), tetra.getCellId(2), n02, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra.getCellId(0), tetra.getCellId(1), n01, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra.getCellId(0), tetra.getCellId(3), n03, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra.getCellId(1), tetra.getCellId(3), n13, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra.getCellId(2), tetra.getCellId(3), n23, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					for(int j=0;j<4;++j){
						if( i == j ){
							dIter->refinedData->addId( kmb::Face(ef[i],0) );
						}else{
							dIter->refinedData->addId( kmb::Face(e[j],i) );
						}
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<4;++i){
					dIter->refinedData->addId( e[i] );
					dIter->refinedData->addId( ef[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4, n5, n6, n7, n8, n9 ]
// => [n0, n6, n5, n7, n56, n05, n06, n07, n67, n57]
// => [n6, n1, n4, n8, n14, n46, n16, n68, n18, n48]
// => [n5, n4, n2, n9, n24, n25, n45, n59, n49, n29]
// => [n7, n8, n9, n3, n89, n79, n78, n37, n38, n39]
// 残り４つは座標に依存
// 8面体の対角線の中点を c とする
// elementId e[0] => n0 についている要素 faceの順番は親を引き継ぐ
//           e[1] => n1
//           e[2] => n2
//           e[3] => n3
//           ef[0] => face0 についている要素 0番目の面が外側
//           ef[1] => face1 についている要素 0番目の面が外側
//           ef[2] => face2 についている要素 0番目の面が外側
//           ef[3] => face3 についている要素 0番目の面が外側
void
kmb::MeshRefiner::refineTetrahedron2( kmb::elementIdType elementId, const kmb::ElementBase &tetra2, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId};
	kmb::elementIdType ef[4] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId};
	kmb::nodeIdType nodeTable[10];

	kmb::nodeIdType n05 = kmb::nullNodeId;
	kmb::nodeIdType n06 = kmb::nullNodeId;
	kmb::nodeIdType n07 = kmb::nullNodeId;
	kmb::nodeIdType n14 = kmb::nullNodeId;
	kmb::nodeIdType n16 = kmb::nullNodeId;
	kmb::nodeIdType n18 = kmb::nullNodeId;
	kmb::nodeIdType n24 = kmb::nullNodeId;
	kmb::nodeIdType n25 = kmb::nullNodeId;
	kmb::nodeIdType n29 = kmb::nullNodeId;
	kmb::nodeIdType n37 = kmb::nullNodeId;
	kmb::nodeIdType n38 = kmb::nullNodeId;
	kmb::nodeIdType n39 = kmb::nullNodeId;

	kmb::nodeIdType n45 = kmb::nullNodeId;
	kmb::nodeIdType n46 = kmb::nullNodeId;
	kmb::nodeIdType n48 = kmb::nullNodeId;
	kmb::nodeIdType n49 = kmb::nullNodeId;
	kmb::nodeIdType n56 = kmb::nullNodeId;
	kmb::nodeIdType n57 = kmb::nullNodeId;
	kmb::nodeIdType n59 = kmb::nullNodeId;
	kmb::nodeIdType n67 = kmb::nullNodeId;
	kmb::nodeIdType n68 = kmb::nullNodeId;
	kmb::nodeIdType n78 = kmb::nullNodeId;
	kmb::nodeIdType n79 = kmb::nullNodeId;
	kmb::nodeIdType n89 = kmb::nullNodeId;

	kmb::nodeIdType n47 = kmb::nullNodeId;
	kmb::nodeIdType n58 = kmb::nullNodeId;
	kmb::nodeIdType n69 = kmb::nullNodeId;

	if(secondFitting){
		n05 = middleMan->getDividedNode3( tetra2[0], tetra2[5], tetra2[2] );
		n06 = middleMan->getDividedNode3( tetra2[0], tetra2[6], tetra2[1] );
		n07 = middleMan->getDividedNode3( tetra2[0], tetra2[7], tetra2[3] );
		n14 = middleMan->getDividedNode3( tetra2[1], tetra2[4], tetra2[2] );
		n16 = middleMan->getDividedNode3( tetra2[1], tetra2[6], tetra2[0] );
		n18 = middleMan->getDividedNode3( tetra2[1], tetra2[8], tetra2[3] );
		n24 = middleMan->getDividedNode3( tetra2[2], tetra2[4], tetra2[1] );
		n25 = middleMan->getDividedNode3( tetra2[2], tetra2[5], tetra2[0] );
		n29 = middleMan->getDividedNode3( tetra2[2], tetra2[9], tetra2[3] );
		n37 = middleMan->getDividedNode3( tetra2[3], tetra2[7], tetra2[0] );
		n38 = middleMan->getDividedNode3( tetra2[3], tetra2[8], tetra2[1] );
		n39 = middleMan->getDividedNode3( tetra2[3], tetra2[9], tetra2[2] );

		n45 = middleMan->getDividedNode5( tetra2[4], tetra2[5], tetra2[1], tetra2[0], tetra2[6] );
		n46 = middleMan->getDividedNode5( tetra2[4], tetra2[6], tetra2[2], tetra2[0], tetra2[5] );
		n48 = middleMan->getDividedNode5( tetra2[4], tetra2[8], tetra2[2], tetra2[3], tetra2[9] );
		n49 = middleMan->getDividedNode5( tetra2[4], tetra2[9], tetra2[1], tetra2[3], tetra2[8] );
		n56 = middleMan->getDividedNode5( tetra2[5], tetra2[6], tetra2[2], tetra2[1], tetra2[4] );
		n57 = middleMan->getDividedNode5( tetra2[5], tetra2[7], tetra2[2], tetra2[3], tetra2[9] );
		n59 = middleMan->getDividedNode5( tetra2[5], tetra2[9], tetra2[0], tetra2[3], tetra2[7] );
		n67 = middleMan->getDividedNode5( tetra2[6], tetra2[7], tetra2[1], tetra2[3], tetra2[8] );
		n68 = middleMan->getDividedNode5( tetra2[6], tetra2[8], tetra2[0], tetra2[3], tetra2[7] );
		n78 = middleMan->getDividedNode5( tetra2[7], tetra2[8], tetra2[0], tetra2[1], tetra2[6] );
		n79 = middleMan->getDividedNode5( tetra2[7], tetra2[9], tetra2[0], tetra2[2], tetra2[5] );
		n89 = middleMan->getDividedNode5( tetra2[8], tetra2[9], tetra2[1], tetra2[2], tetra2[4] );
	}else{
		n05 = middleMan->getDividedNode( tetra2.getCellId(0), tetra2.getCellId(5) );
		n06 = middleMan->getDividedNode( tetra2.getCellId(0), tetra2.getCellId(6) );
		n07 = middleMan->getDividedNode( tetra2.getCellId(0), tetra2.getCellId(7) );
		n14 = middleMan->getDividedNode( tetra2.getCellId(1), tetra2.getCellId(4) );
		n16 = middleMan->getDividedNode( tetra2.getCellId(1), tetra2.getCellId(6) );
		n18 = middleMan->getDividedNode( tetra2.getCellId(1), tetra2.getCellId(8) );
		n24 = middleMan->getDividedNode( tetra2.getCellId(2), tetra2.getCellId(4) );
		n25 = middleMan->getDividedNode( tetra2.getCellId(2), tetra2.getCellId(5) );
		n29 = middleMan->getDividedNode( tetra2.getCellId(2), tetra2.getCellId(9) );
		n37 = middleMan->getDividedNode( tetra2.getCellId(3), tetra2.getCellId(7) );
		n38 = middleMan->getDividedNode( tetra2.getCellId(3), tetra2.getCellId(8) );
		n39 = middleMan->getDividedNode( tetra2.getCellId(3), tetra2.getCellId(9) );

		n45 = middleMan->getDividedNode( tetra2.getCellId(4), tetra2.getCellId(5) );
		n46 = middleMan->getDividedNode( tetra2.getCellId(4), tetra2.getCellId(6) );
		n48 = middleMan->getDividedNode( tetra2.getCellId(4), tetra2.getCellId(8) );
		n49 = middleMan->getDividedNode( tetra2.getCellId(4), tetra2.getCellId(9) );
		n56 = middleMan->getDividedNode( tetra2.getCellId(5), tetra2.getCellId(6) );
		n57 = middleMan->getDividedNode( tetra2.getCellId(5), tetra2.getCellId(7) );
		n59 = middleMan->getDividedNode( tetra2.getCellId(5), tetra2.getCellId(9) );
		n67 = middleMan->getDividedNode( tetra2.getCellId(6), tetra2.getCellId(7) );
		n68 = middleMan->getDividedNode( tetra2.getCellId(6), tetra2.getCellId(8) );
		n78 = middleMan->getDividedNode( tetra2.getCellId(7), tetra2.getCellId(8) );
		n79 = middleMan->getDividedNode( tetra2.getCellId(7), tetra2.getCellId(9) );
		n89 = middleMan->getDividedNode( tetra2.getCellId(8), tetra2.getCellId(9) );
	}

	if( refinedBody ){
		nodeTable[0] = tetra2.getCellId(0);
		nodeTable[1] = tetra2.getCellId(6);
		nodeTable[2] = tetra2.getCellId(5);
		nodeTable[3] = tetra2.getCellId(7);
		nodeTable[4] = n56;
		nodeTable[5] = n05;
		nodeTable[6] = n06;
		nodeTable[7] = n07;
		nodeTable[8] = n67;
		nodeTable[9] = n57;
		e[0] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

		nodeTable[0] = tetra2.getCellId(6);
		nodeTable[1] = tetra2.getCellId(1);
		nodeTable[2] = tetra2.getCellId(4);
		nodeTable[3] = tetra2.getCellId(8);
		nodeTable[4] = n14;
		nodeTable[5] = n46;
		nodeTable[6] = n16;
		nodeTable[7] = n68;
		nodeTable[8] = n18;
		nodeTable[9] = n48;
		e[1] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

		nodeTable[0] = tetra2.getCellId(5);
		nodeTable[1] = tetra2.getCellId(4);
		nodeTable[2] = tetra2.getCellId(2);
		nodeTable[3] = tetra2.getCellId(9);
		nodeTable[4] = n24;
		nodeTable[5] = n25;
		nodeTable[6] = n45;
		nodeTable[7] = n59;
		nodeTable[8] = n49;
		nodeTable[9] = n29;
		e[2] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

		nodeTable[0] = tetra2.getCellId(7);
		nodeTable[1] = tetra2.getCellId(8);
		nodeTable[2] = tetra2.getCellId(9);
		nodeTable[3] = tetra2.getCellId(3);
		nodeTable[4] = n89;
		nodeTable[5] = n79;
		nodeTable[6] = n78;
		nodeTable[7] = n37;
		nodeTable[8] = n38;
		nodeTable[9] = n39;
		e[3] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

		// 内部の４要素の判定
		// 対角線の一番短いところを辺にする
		// 対角線を最初の２点にする
		// 外に面している面を０番目の面にする
		// 4 = [12] 7 = [03]
		// 5 = [02] 8 = [13]
		// 6 = [01] 9 = [23]
		double d4_7 = mesh->getNodes()->distanceSq( tetra2[4], tetra2[7] );
		double d5_8 = mesh->getNodes()->distanceSq( tetra2[5], tetra2[8] );
		double d6_9 = mesh->getNodes()->distanceSq( tetra2[6], tetra2[9] );
		if( d4_7 <= d5_8 && d4_7 <= d6_9 ){
			// 4 -- 7 = 12 -- 03
			n47 = middleMan->getDividedNode( tetra2.getCellId(4), tetra2.getCellId(7) );

			nodeTable[0] = tetra2.getCellId(7); // 03
			nodeTable[1] = tetra2.getCellId(4); // 12
			nodeTable[2] = tetra2.getCellId(9); // 23
			nodeTable[3] = tetra2.getCellId(8); // 13
			nodeTable[4] = n49;
			nodeTable[5] = n79;
			nodeTable[6] = n47;
			nodeTable[7] = n78;
			nodeTable[8] = n48;
			nodeTable[9] = n89;
			ef[0] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(4); // 12
			nodeTable[1] = tetra2.getCellId(7); // 03
			nodeTable[2] = tetra2.getCellId(9); // 23
			nodeTable[3] = tetra2.getCellId(5); // 02
			nodeTable[4] = n79;
			nodeTable[5] = n49;
			nodeTable[6] = n47;
			nodeTable[7] = n45;
			nodeTable[8] = n57;
			nodeTable[9] = n59;
			ef[1] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(4); // 12
			nodeTable[1] = tetra2.getCellId(7); // 03
			nodeTable[2] = tetra2.getCellId(6); // 01
			nodeTable[3] = tetra2.getCellId(8); // 13
			nodeTable[4] = n67;
			nodeTable[5] = n46;
			nodeTable[6] = n47;
			nodeTable[7] = n48;
			nodeTable[8] = n78;
			nodeTable[9] = n68;
			ef[2] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(7); // 03
			nodeTable[1] = tetra2.getCellId(4); // 12
			nodeTable[2] = tetra2.getCellId(6); // 01
			nodeTable[3] = tetra2.getCellId(5); // 02
			nodeTable[4] = n46;
			nodeTable[5] = n67;
			nodeTable[6] = n47;
			nodeTable[7] = n57;
			nodeTable[8] = n45;
			nodeTable[9] = n56;
			ef[3] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

		}else if( d5_8 <= d4_7 && d5_8 <= d6_9 ){
			// 5 -- 8 == 02 -- 13
			n58 = middleMan->getDividedNode( tetra2.getCellId(5), tetra2.getCellId(8) );

			nodeTable[0] = tetra2.getCellId(5); // 02
			nodeTable[1] = tetra2.getCellId(8); // 13
			nodeTable[2] = tetra2.getCellId(4); // 12
			nodeTable[3] = tetra2.getCellId(9); // 23
			nodeTable[4] = n48;
			nodeTable[5] = n45;
			nodeTable[6] = n58;
			nodeTable[7] = n59;
			nodeTable[8] = n89;
			nodeTable[9] = n49;
			ef[0] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(8); // 13
			nodeTable[1] = tetra2.getCellId(5); // 02
			nodeTable[2] = tetra2.getCellId(7); // 03
			nodeTable[3] = tetra2.getCellId(9); // 23
			nodeTable[4] = n57;
			nodeTable[5] = n78;
			nodeTable[6] = n58;
			nodeTable[7] = n89;
			nodeTable[8] = n59;
			nodeTable[9] = n79;
			ef[1] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(5); // 02
			nodeTable[1] = tetra2.getCellId(8); // 13
			nodeTable[2] = tetra2.getCellId(7); // 03
			nodeTable[3] = tetra2.getCellId(6); // 01
			nodeTable[4] = n78;
			nodeTable[5] = n57;
			nodeTable[6] = n58;
			nodeTable[7] = n56;
			nodeTable[8] = n68;
			nodeTable[9] = n67;
			ef[2] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(8); // 13
			nodeTable[1] = tetra2.getCellId(5); // 02
			nodeTable[2] = tetra2.getCellId(4); // 12
			nodeTable[3] = tetra2.getCellId(6); // 01
			nodeTable[4] = n45;
			nodeTable[5] = n48;
			nodeTable[6] = n58;
			nodeTable[7] = n68;
			nodeTable[8] = n56;
			nodeTable[9] = n46;
			ef[3] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

		}else{
			// 6 -- 9 == 01 -- 23
			n69 = middleMan->getDividedNode( tetra2.getCellId(6), tetra2.getCellId(9) );

			nodeTable[0] = tetra2.getCellId(6); // 01
			nodeTable[1] = tetra2.getCellId(9); // 23
			nodeTable[2] = tetra2.getCellId(8); // 13
			nodeTable[3] = tetra2.getCellId(4); // 12
			nodeTable[4] = n89;
			nodeTable[5] = n68;
			nodeTable[6] = n69;
			nodeTable[7] = n46;
			nodeTable[8] = n49;
			nodeTable[9] = n48;
			ef[0] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(6); // 01
			nodeTable[1] = tetra2.getCellId(9); // 23
			nodeTable[2] = tetra2.getCellId(5); // 02
			nodeTable[3] = tetra2.getCellId(7); // 03
			nodeTable[4] = n59;
			nodeTable[5] = n56;
			nodeTable[6] = n69;
			nodeTable[7] = n67;
			nodeTable[8] = n79;
			nodeTable[9] = n57;
			ef[1] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(9); // 23
			nodeTable[1] = tetra2.getCellId(6); // 01
			nodeTable[2] = tetra2.getCellId(8); // 13
			nodeTable[3] = tetra2.getCellId(7); // 03
			nodeTable[4] = n68;
			nodeTable[5] = n89;
			nodeTable[6] = n69;
			nodeTable[7] = n79;
			nodeTable[8] = n67;
			nodeTable[9] = n78;
			ef[2] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

			nodeTable[0] = tetra2.getCellId(9); // 23
			nodeTable[1] = tetra2.getCellId(6); // 01
			nodeTable[2] = tetra2.getCellId(5); // 02
			nodeTable[3] = tetra2.getCellId(4); // 12
			nodeTable[4] = n56;
			nodeTable[5] = n59;
			nodeTable[6] = n69;
			nodeTable[7] = n49;
			nodeTable[8] = n46;
			nodeTable[9] = n45;
			ef[3] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

		}
	}
	const int faceNum = kmb::Element::getBoundaryCount( kmb::TETRAHEDRON );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			nodeGroupUpdate( tetra2.getCellId(0), tetra2.getCellId(5), n05, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(0), tetra2.getCellId(6), n06, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(0), tetra2.getCellId(7), n07, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(1), tetra2.getCellId(4), n14, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(1), tetra2.getCellId(6), n16, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(1), tetra2.getCellId(8), n18, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(2), tetra2.getCellId(4), n24, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(2), tetra2.getCellId(5), n25, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(2), tetra2.getCellId(9), n29, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(3), tetra2.getCellId(7), n37, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(3), tetra2.getCellId(8), n38, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(3), tetra2.getCellId(9), n39, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( tetra2.getCellId(4), tetra2.getCellId(5), n45, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(4), tetra2.getCellId(6), n46, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(4), tetra2.getCellId(8), n48, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(4), tetra2.getCellId(9), n49, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(5), tetra2.getCellId(6), n56, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(5), tetra2.getCellId(7), n57, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(5), tetra2.getCellId(9), n59, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(6), tetra2.getCellId(7), n67, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(6), tetra2.getCellId(8), n68, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(7), tetra2.getCellId(8), n78, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(7), tetra2.getCellId(9), n79, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(8), tetra2.getCellId(9), n89, dIter->originalData, dIter->refinedData );
			// どれか一つだけ有効
			nodeGroupUpdate( tetra2.getCellId(4), tetra2.getCellId(7), n47, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(5), tetra2.getCellId(8), n58, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( tetra2.getCellId(6), tetra2.getCellId(9), n69, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			nodeVariableUpdate( tetra2.getCellId(0), tetra2.getCellId(5), n05, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(0), tetra2.getCellId(6), n06, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(0), tetra2.getCellId(7), n07, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(1), tetra2.getCellId(4), n14, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(1), tetra2.getCellId(6), n16, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(1), tetra2.getCellId(8), n18, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(2), tetra2.getCellId(4), n24, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(2), tetra2.getCellId(5), n25, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(2), tetra2.getCellId(9), n29, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(3), tetra2.getCellId(7), n37, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(3), tetra2.getCellId(8), n38, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(3), tetra2.getCellId(9), n39, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( tetra2.getCellId(4), tetra2.getCellId(5), n45, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(4), tetra2.getCellId(6), n46, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(4), tetra2.getCellId(8), n48, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(4), tetra2.getCellId(9), n49, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(5), tetra2.getCellId(6), n56, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(5), tetra2.getCellId(7), n57, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(5), tetra2.getCellId(9), n59, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(6), tetra2.getCellId(7), n67, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(6), tetra2.getCellId(8), n68, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(7), tetra2.getCellId(8), n78, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(7), tetra2.getCellId(9), n79, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(8), tetra2.getCellId(9), n89, dIter->originalData, dIter->refinedData );
			// どれか一つだけ有効
			nodeVariableUpdate( tetra2.getCellId(4), tetra2.getCellId(7), n47, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(5), tetra2.getCellId(8), n58, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( tetra2.getCellId(6), tetra2.getCellId(9), n69, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					for(int j=0;j<4;++j){
						if( i == j ){
							dIter->refinedData->addId( kmb::Face(ef[i],0) );
						}else{
							dIter->refinedData->addId( kmb::Face(e[j],i) );
						}
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<4;++i){
					dIter->refinedData->addId( e[i] );
					dIter->refinedData->addId( ef[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4, n5, n6, n7 ]
// => [n0,  n01, nf0, n03, n04, nf2, c,   nf5]
// => [n01,  n1, n12, nf0, nf2, n15, nf3, c  ]
// => [nf0, n12, n2,  n23, c,   nf3, n26, nf4]
// => [n30, nf0, n23, n3,  nf5, c,   nf4, n37]
// => [n04, nf2, c,   nf5, n4,  n54, nf1, n47]
// => [nf2, n15, nf3, c,   n45, n5,  n56, nf1]
// => [c,   nf3, n26, nf4, nf1, n56, n6,  n67]
// => [nf5, c,   nf4, n37, n47, nf1, n67, n7 ]
// face の順番は親を引き継ぐ
void
kmb::MeshRefiner::refineHexahedron( kmb::elementIdType elementId, const kmb::ElementBase &hexa, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[8] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType nodeTable[8] = {
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId
	};
	// 12 = kmb::Element::getEdgeCount( kmb::HEXAHEDRON )
	kmb::nodeIdType ne[12];
	for(int i=0;i<12;++i){
		ne[i] = middleMan->getDividedNode( hexa.getEdgeCellId(i,0), hexa.getEdgeCellId(i,1) );
	}
	// ne[0] = n01
	// ne[1] = n12
	// ne[2] = n23
	// ne[3] = n03
	// ne[4] = n45
	// ne[5] = n56
	// ne[6] = n67
	// ne[7] = n47
	// ne[8] = n04
	// ne[9] = n15
	// ne[10] = n26
	// ne[11] = n37
	kmb::nodeIdType nf0 = middleMan->getCenterNode( hexa, 0, elementId );
	kmb::nodeIdType nf1 = middleMan->getCenterNode( hexa, 1, elementId );
	kmb::nodeIdType nf2 = middleMan->getCenterNode( hexa, 2, elementId );
	kmb::nodeIdType nf3 = middleMan->getCenterNode( hexa, 3, elementId );
	kmb::nodeIdType nf4 = middleMan->getCenterNode( hexa, 4, elementId );
	kmb::nodeIdType nf5 = middleMan->getCenterNode( hexa, 5, elementId );
	kmb::nodeIdType c   = middleMan->getCenterNode( hexa, elementId );


	if( refinedBody ){
		nodeTable[0] = hexa.getCellId(0);
		nodeTable[1] = ne[0];
		nodeTable[2] = nf0;
		nodeTable[3] = ne[3];
		nodeTable[4] = ne[8];
		nodeTable[5] = nf2;
		nodeTable[6] = c;
		nodeTable[7] = nf5;
		e[0] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );

		nodeTable[0] = ne[0];
		nodeTable[1] = hexa.getCellId(1);
		nodeTable[2] = ne[1];
		nodeTable[3] = nf0;
		nodeTable[4] = nf2;
		nodeTable[5] = ne[9];
		nodeTable[6] = nf3;
		nodeTable[7] = c;
		e[1] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );

		nodeTable[0] = nf0;
		nodeTable[1] = ne[1];
		nodeTable[2] = hexa.getCellId(2);
		nodeTable[3] = ne[2];
		nodeTable[4] = c;
		nodeTable[5] = nf3;
		nodeTable[6] = ne[10];
		nodeTable[7] = nf4;
		e[2] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );

		nodeTable[0] = ne[3];
		nodeTable[1] = nf0;
		nodeTable[2] = ne[2];
		nodeTable[3] = hexa.getCellId(3);
		nodeTable[4] = nf5;
		nodeTable[5] = c;
		nodeTable[6] = nf4;
		nodeTable[7] = ne[11];
		e[3] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );

		nodeTable[0] = ne[8];
		nodeTable[1] = nf2;
		nodeTable[2] = c;
		nodeTable[3] = nf5;
		nodeTable[4] = hexa.getCellId(4);
		nodeTable[5] = ne[4];
		nodeTable[6] = nf1;
		nodeTable[7] = ne[7];
		e[4] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );

		nodeTable[0] = nf2;
		nodeTable[1] = ne[9];
		nodeTable[2] = nf3;
		nodeTable[3] = c;
		nodeTable[4] = ne[4];
		nodeTable[5] = hexa.getCellId(5);
		nodeTable[6] = ne[5];
		nodeTable[7] = nf1;
		e[5] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );

		nodeTable[0] = c;
		nodeTable[1] = nf3;
		nodeTable[2] = ne[10];
		nodeTable[3] = nf4;
		nodeTable[4] = nf1;
		nodeTable[5] = ne[5];
		nodeTable[6] = hexa.getCellId(6);
		nodeTable[7] = ne[6];
		e[6] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );

		nodeTable[0] = nf5;
		nodeTable[1] = c;
		nodeTable[2] = nf4;
		nodeTable[3] = ne[11];
		nodeTable[4] = ne[7];
		nodeTable[5] = nf1;
		nodeTable[6] = ne[6];
		nodeTable[7] = hexa.getCellId(7);
		e[7] = refinedBody->addElement( kmb::HEXAHEDRON, nodeTable );
	}
	const int faceNum = kmb::Element::getBoundaryCount( kmb::HEXAHEDRON );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			nodeGroupUpdate( hexa, 0, nf0, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa, 1, nf1, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa, 4, nf4, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa, 5, nf5, dIter->originalData, dIter->refinedData );
			for(int i=0;i<12;++i){
				nodeGroupUpdate( hexa.getEdgeCellId(i,0), hexa.getEdgeCellId(i,1), ne[i], dIter->originalData, dIter->refinedData );
			}
			nodeGroupUpdate( hexa, c, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			nodeVariableUpdate( hexa, 0, nf0, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa, 1, nf1, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa, 4, nf4, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa, 5, nf5, dIter->originalData, dIter->refinedData );
			for(int i=0;i<12;++i){
				nodeVariableUpdate( hexa.getEdgeCellId(i,0), hexa.getEdgeCellId(i,1), ne[i], dIter->originalData, dIter->refinedData );
			}
			nodeVariableUpdate( hexa, c, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					for(int j=0;j<4;++j){
						// e の順番は元の節点を含むように作ってあるので
						// face の頂点番号を e の添字に使えばよい
						int index = kmb::Hexahedron::faceTable[i][j];
						dIter->refinedData->addId( kmb::Face(e[index],i) );
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<8;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19 ]
// => [n0,  n8,  nf0, n11, n16, nf2, c,   nf5, n0_8, nf0_8, nf0_11, n0_11, nf2_16, nf2_c, nf5_c, nf5_16, n0_16, nf2_8, nf0_c, nf5_11]
// => [n8,  n1,  n9,  nf0, nf2, n17, nf3, c  , n1_8, n1_9, nf0_9, nf0_8, nf2_17, nf3_17, nf3_c, nf2_c, nf2_8, n1_17, nf3_9, nf0_c]
// => [nf0, n9,  n2,  n10, c,   nf3, n18, nf4, nf0_9, n2_9, n2_10, nf0_10, nf3_c, nf3_18, nf4_18, nf4_c, nf0_c, nf3_9, n2_18, nf4_10]
// => [n11, nf0, n10, n3,  nf5, c,   nf4, n19, nf0_11, nf0_10, n3_10, n3_11, nf5_c, nf4_c, nf4_19, nf5_19, nf5_11, nf0_c, nf4_10, n3_19]
// => [n16, nf2, c,   nf5, n4,  n12, nf1, n15, nf2_16, nf2_c, nf5_c, nf5_16, n4_12, nf1_12, nf1_15, n4_15, n4_16, nf2_12, nf1_c, nf5_15]
// => [nf2, n17, nf3, c,   n12, n5,  n13, nf1, nf2_17, nf3_17, nf3_c, nf2_c, n5_12, n5_13, nf1_13, nf1_12, nf2_12, n5_17, nf3_13, nf1_c]
// => [c,   nf3, n18, nf4, nf1, n13, n6,  n14, nf3_c, nf3_18, nf4_18, nf4_c, nf1_13, n6_13, n6_14, nf1_14, nf1_c, nf3_13, n6_18, nf4_14]
// => [nf5, c,   nf4, n19, n15, nf1, n14, n7,  nf5_c, nf4_c, nf4_19, nf5_19, nf1_15, nf1_14, n7_14, n7_15, nf5_15, nf1_c, nf4_14, n7_19]
// face の順番は親を引き継ぐように並び替える
void
kmb::MeshRefiner::refineHexahedron2( kmb::elementIdType elementId, const kmb::ElementBase &hexa2, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[8] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType nodeTable[20] = {
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId
	};
	// 12 = kmb::Element::getEdgeCount( kmb::HEXAHEDRON2 )
	kmb::nodeIdType ne0[12];
	kmb::nodeIdType ne1[12];

	// ne0[0] = n0_8   ne1[0] = n8_1
	// ne0[1] = n1_9   ne1[1] = n9_2
	// ne0[2] = n2_10  ne1[2] = n10_3
	// ne0[3] = n0_11  ne1[3] = n11_3
	// ne0[4] = n4_12  ne1[4] = n12_5
	// ne0[5] = n5_13  ne1[5] = n13_6
	// ne0[6] = n6_14  ne1[6] = n14_7
	// ne0[7] = n4_15  ne1[7] = n15_7
	// ne0[8] = n0_16  ne1[8] = n16_4
	// ne0[9] = n1_17  ne1[9] = n17_5
	// ne0[10] = n2_18 ne1[10] = n18_6
	// ne0[11] = n3_19 ne1[11] = n19_7
	kmb::nodeIdType nf0 = kmb::nullNodeId;
	kmb::nodeIdType nf1 = kmb::nullNodeId;
	kmb::nodeIdType nf2 = kmb::nullNodeId;
	kmb::nodeIdType nf3 = kmb::nullNodeId;
	kmb::nodeIdType nf4 = kmb::nullNodeId;
	kmb::nodeIdType nf5 = kmb::nullNodeId;
	kmb::nodeIdType c   = kmb::nullNodeId;

	kmb::nodeIdType nf0_8  = kmb::nullNodeId;
	kmb::nodeIdType nf0_9  = kmb::nullNodeId;
	kmb::nodeIdType nf0_10 = kmb::nullNodeId;
	kmb::nodeIdType nf0_11 = kmb::nullNodeId;
	kmb::nodeIdType nf0_c  = kmb::nullNodeId;

	kmb::nodeIdType nf1_12 = kmb::nullNodeId;
	kmb::nodeIdType nf1_13 = kmb::nullNodeId;
	kmb::nodeIdType nf1_14 = kmb::nullNodeId;
	kmb::nodeIdType nf1_15 = kmb::nullNodeId;
	kmb::nodeIdType nf1_c  = kmb::nullNodeId;

	kmb::nodeIdType nf2_8  = kmb::nullNodeId;
	kmb::nodeIdType nf2_12 = kmb::nullNodeId;
	kmb::nodeIdType nf2_16 = kmb::nullNodeId;
	kmb::nodeIdType nf2_17 = kmb::nullNodeId;
	kmb::nodeIdType nf2_c  = kmb::nullNodeId;

	kmb::nodeIdType nf3_9  = kmb::nullNodeId;
	kmb::nodeIdType nf3_13 = kmb::nullNodeId;
	kmb::nodeIdType nf3_17 = kmb::nullNodeId;
	kmb::nodeIdType nf3_18 = kmb::nullNodeId;
	kmb::nodeIdType nf3_c  = kmb::nullNodeId;

	kmb::nodeIdType nf4_10 = kmb::nullNodeId;
	kmb::nodeIdType nf4_14 = kmb::nullNodeId;
	kmb::nodeIdType nf4_18 = kmb::nullNodeId;
	kmb::nodeIdType nf4_19 = kmb::nullNodeId;
	kmb::nodeIdType nf4_c  = kmb::nullNodeId;

	kmb::nodeIdType nf5_11 = kmb::nullNodeId;
	kmb::nodeIdType nf5_15 = kmb::nullNodeId;
	kmb::nodeIdType nf5_16 = kmb::nullNodeId;
	kmb::nodeIdType nf5_19 = kmb::nullNodeId;
	kmb::nodeIdType nf5_c  = kmb::nullNodeId;

	if( secondFitting ){
		for(int i=0;i<12;++i){
			ne0[i] = middleMan->getDividedNode3( hexa2.getEdgeCellId(i,0), hexa2.getEdgeCellId(i,2), hexa2.getEdgeCellId(i,1) );
			ne1[i] = middleMan->getDividedNode3( hexa2.getEdgeCellId(i,1), hexa2.getEdgeCellId(i,2), hexa2.getEdgeCellId(i,0) );
		}
		nf0 = middleMan->getCenterNode2( hexa2, 0, elementId );
		nf1 = middleMan->getCenterNode2( hexa2, 1, elementId );
		nf2 = middleMan->getCenterNode2( hexa2, 2, elementId );
		nf3 = middleMan->getCenterNode2( hexa2, 3, elementId );
		nf4 = middleMan->getCenterNode2( hexa2, 4, elementId );
		nf5 = middleMan->getCenterNode2( hexa2, 5, elementId );
		c   = middleMan->getCenterNode2( hexa2, elementId );

		nf0_8  = middleMan->getDividedNode3( hexa2[8],  nf0, hexa2[10] );
		nf0_9  = middleMan->getDividedNode3( hexa2[9],  nf0, hexa2[11] );
		nf0_10 = middleMan->getDividedNode3( hexa2[10], nf0, hexa2[8] );
		nf0_11 = middleMan->getDividedNode3( hexa2[11], nf0, hexa2[9] );
		nf0_c  = middleMan->getDividedNode3( nf0, c, nf1 );

		nf1_12 = middleMan->getDividedNode3( hexa2[12], nf1, hexa2[14] );
		nf1_13 = middleMan->getDividedNode3( hexa2[13], nf1, hexa2[15] );
		nf1_14 = middleMan->getDividedNode3( hexa2[14], nf1, hexa2[12] );
		nf1_15 = middleMan->getDividedNode3( hexa2[15], nf1, hexa2[13] );
		nf1_c  = middleMan->getDividedNode3( nf1, c, nf0 );

		nf2_8  = middleMan->getDividedNode3( hexa2[8],  nf2, hexa2[12] );
		nf2_12 = middleMan->getDividedNode3( hexa2[12], nf2, hexa2[8] );
		nf2_16 = middleMan->getDividedNode3( hexa2[16], nf2, hexa2[17] );
		nf2_17 = middleMan->getDividedNode3( hexa2[17], nf2, hexa2[16] );
		nf2_c  = middleMan->getDividedNode3( nf2, c, nf4 );

		nf3_9  = middleMan->getDividedNode3( hexa2[9],  nf3, hexa2[13] );
		nf3_13 = middleMan->getDividedNode3( hexa2[13], nf3, hexa2[9] );
		nf3_17 = middleMan->getDividedNode3( hexa2[17], nf3, hexa2[18] );
		nf3_18 = middleMan->getDividedNode3( hexa2[18], nf3, hexa2[17] );
		nf3_c  = middleMan->getDividedNode3( nf3, c, nf5 );

		nf4_10 = middleMan->getDividedNode3( hexa2[10], nf4, hexa2[14] );
		nf4_14 = middleMan->getDividedNode3( hexa2[14], nf4, hexa2[10] );
		nf4_18 = middleMan->getDividedNode3( hexa2[18], nf4, hexa2[19] );
		nf4_19 = middleMan->getDividedNode3( hexa2[19], nf4, hexa2[18] );
		nf4_c  = middleMan->getDividedNode3( nf4, c, nf2 );

		nf5_11 = middleMan->getDividedNode3( hexa2[11], nf5, hexa2[15] );
		nf5_15 = middleMan->getDividedNode3( hexa2[15], nf5, hexa2[11] );
		nf5_16 = middleMan->getDividedNode3( hexa2[16], nf5, hexa2[19] );
		nf5_19 = middleMan->getDividedNode3( hexa2[19], nf5, hexa2[16] );
		nf5_c  = middleMan->getDividedNode3( nf5, c, nf3 );
	}else{
		for(int i=0;i<12;++i){
			ne0[i] = middleMan->getDividedNode( hexa2.getEdgeCellId(i,0), hexa2.getEdgeCellId(i,2) );
			ne1[i] = middleMan->getDividedNode( hexa2.getEdgeCellId(i,2), hexa2.getEdgeCellId(i,1) );
		}
		nf0 = middleMan->getCenterNode( hexa2, 0, elementId );
		nf1 = middleMan->getCenterNode( hexa2, 1, elementId );
		nf2 = middleMan->getCenterNode( hexa2, 2, elementId );
		nf3 = middleMan->getCenterNode( hexa2, 3, elementId );
		nf4 = middleMan->getCenterNode( hexa2, 4, elementId );
		nf5 = middleMan->getCenterNode( hexa2, 5, elementId );
		c   = middleMan->getCenterNode( hexa2, elementId );

		nf0_8  = middleMan->getDividedNode( nf0, hexa2[8] );
		nf0_9  = middleMan->getDividedNode( nf0, hexa2[9] );
		nf0_10 = middleMan->getDividedNode( nf0, hexa2[10] );
		nf0_11 = middleMan->getDividedNode( nf0, hexa2[11] );
		nf0_c  = middleMan->getDividedNode( nf0, c );

		nf1_12 = middleMan->getDividedNode( nf1, hexa2[12] );
		nf1_13 = middleMan->getDividedNode( nf1, hexa2[13] );
		nf1_14 = middleMan->getDividedNode( nf1, hexa2[14] );
		nf1_15 = middleMan->getDividedNode( nf1, hexa2[15] );
		nf1_c  = middleMan->getDividedNode( nf1, c );

		nf2_8  = middleMan->getDividedNode( nf2, hexa2[8] );
		nf2_12 = middleMan->getDividedNode( nf2, hexa2[12] );
		nf2_16 = middleMan->getDividedNode( nf2, hexa2[16] );
		nf2_17 = middleMan->getDividedNode( nf2, hexa2[17] );
		nf2_c  = middleMan->getDividedNode( nf2, c );

		nf3_9  = middleMan->getDividedNode( nf3, hexa2[9] );
		nf3_13 = middleMan->getDividedNode( nf3, hexa2[13] );
		nf3_17 = middleMan->getDividedNode( nf3, hexa2[17] );
		nf3_18 = middleMan->getDividedNode( nf3, hexa2[18] );
		nf3_c  = middleMan->getDividedNode( nf3, c );

		nf4_10 = middleMan->getDividedNode( nf4, hexa2[10] );
		nf4_14 = middleMan->getDividedNode( nf4, hexa2[14] );
		nf4_18 = middleMan->getDividedNode( nf4, hexa2[18] );
		nf4_19 = middleMan->getDividedNode( nf4, hexa2[19] );
		nf4_c  = middleMan->getDividedNode( nf4, c );

		nf5_11 = middleMan->getDividedNode( nf5, hexa2[11] );
		nf5_15 = middleMan->getDividedNode( nf5, hexa2[15] );
		nf5_16 = middleMan->getDividedNode( nf5, hexa2[16] );
		nf5_19 = middleMan->getDividedNode( nf5, hexa2[19] );
		nf5_c  = middleMan->getDividedNode( nf5, c );
	}

	if( refinedBody ){
		nodeTable[0] = hexa2.getCellId(0);
		nodeTable[1] = hexa2.getCellId(8);
		nodeTable[2] = nf0;
		nodeTable[3] = hexa2.getCellId(11);
		nodeTable[4] = hexa2.getCellId(16);
		nodeTable[5] = nf2;
		nodeTable[6] = c;
		nodeTable[7] = nf5;

		nodeTable[8] = ne0[0];
		nodeTable[9] = nf0_8;
		nodeTable[10] = nf0_11;
		nodeTable[11] = ne0[3];

		nodeTable[12] = nf2_16;
		nodeTable[13] = nf2_c;
		nodeTable[14] = nf5_c;
		nodeTable[15] = nf5_16;

		nodeTable[16] = ne0[8];
		nodeTable[17] = nf2_8;
		nodeTable[18] = nf0_c;
		nodeTable[19] = nf5_11;

		e[0] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );

		nodeTable[0] = hexa2.getCellId(8);
		nodeTable[1] = hexa2.getCellId(1);
		nodeTable[2] = hexa2.getCellId(9);
		nodeTable[3] = nf0;
		nodeTable[4] = nf2;
		nodeTable[5] = hexa2.getCellId(17);
		nodeTable[6] = nf3;
		nodeTable[7] = c;

		nodeTable[8] = ne1[0];
		nodeTable[9] = ne0[1];
		nodeTable[10] = nf0_9;
		nodeTable[11] = nf0_8;

		nodeTable[12] = nf2_17;
		nodeTable[13] = nf3_17;
		nodeTable[14] = nf3_c;
		nodeTable[15] = nf2_c;

		nodeTable[16] = nf2_8;
		nodeTable[17] = ne0[9];
		nodeTable[18] = nf3_9;
		nodeTable[19] = nf0_c;

		e[1] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );

		nodeTable[0] = nf0;
		nodeTable[1] = hexa2.getCellId(9);
		nodeTable[2] = hexa2.getCellId(2);
		nodeTable[3] = hexa2.getCellId(10);
		nodeTable[4] = c;
		nodeTable[5] = nf3;
		nodeTable[6] = hexa2.getCellId(18);
		nodeTable[7] = nf4;

		nodeTable[8] = nf0_9;
		nodeTable[9] = ne1[1];
		nodeTable[10] = ne0[2];
		nodeTable[11] = nf0_10;

		nodeTable[12] = nf3_c;
		nodeTable[13] = nf3_18;
		nodeTable[14] = nf4_18;
		nodeTable[15] = nf4_c;

		nodeTable[16] = nf0_c;
		nodeTable[17] = nf3_9;
		nodeTable[18] = ne0[10];
		nodeTable[19] = nf4_10;

		e[2] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );

		nodeTable[0] = hexa2.getCellId(11);
		nodeTable[1] = nf0;
		nodeTable[2] = hexa2.getCellId(10);;
		nodeTable[3] = hexa2.getCellId(3);
		nodeTable[4] = nf5;
		nodeTable[5] = c;
		nodeTable[6] = nf4;
		nodeTable[7] = hexa2.getCellId(19);;

		nodeTable[8] = nf0_11;
		nodeTable[9] = nf0_10;
		nodeTable[10] = ne1[2];
		nodeTable[11] = ne1[3];

		nodeTable[12] = nf5_c;
		nodeTable[13] = nf4_c;
		nodeTable[14] = nf4_19;
		nodeTable[15] = nf5_19;

		nodeTable[16] = nf5_11;
		nodeTable[17] = nf0_c;
		nodeTable[18] = nf4_10;
		nodeTable[19] = ne0[11];

		e[3] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );

		nodeTable[0] = hexa2.getCellId(16);
		nodeTable[1] = nf2;
		nodeTable[2] = c;
		nodeTable[3] = nf5;
		nodeTable[4] = hexa2.getCellId(4);
		nodeTable[5] = hexa2.getCellId(12);
		nodeTable[6] = nf1;
		nodeTable[7] = hexa2.getCellId(15);

		nodeTable[8] = nf2_16;
		nodeTable[9] = nf2_c;
		nodeTable[10] = nf5_c;
		nodeTable[11] = nf5_16;

		nodeTable[12] = ne0[4];
		nodeTable[13] = nf1_12;
		nodeTable[14] = nf1_15;
		nodeTable[15] = ne0[7];

		nodeTable[16] = ne1[8];
		nodeTable[17] = nf2_12;
		nodeTable[18] = nf1_c;
		nodeTable[19] = nf5_15;

		e[4] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );

		nodeTable[0] = nf2;
		nodeTable[1] = hexa2.getCellId(17);
		nodeTable[2] = nf3;
		nodeTable[3] = c;
		nodeTable[4] = hexa2.getCellId(12);
		nodeTable[5] = hexa2.getCellId(5);
		nodeTable[6] = hexa2.getCellId(13);
		nodeTable[7] = nf1;

		nodeTable[8] = nf2_17;
		nodeTable[9] = nf3_17;
		nodeTable[10] = nf3_c;
		nodeTable[11] = nf2_c;

		nodeTable[12] = ne1[4];
		nodeTable[13] = ne0[5];
		nodeTable[14] = nf1_13;
		nodeTable[15] = nf1_12;

		nodeTable[16] = nf2_12;
		nodeTable[17] = ne1[9];
		nodeTable[18] = nf3_13;
		nodeTable[19] = nf1_c;

		e[5] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );

		nodeTable[0] = c;
		nodeTable[1] = nf3;
		nodeTable[2] = hexa2.getCellId(18);
		nodeTable[3] = nf4;
		nodeTable[4] = nf1;
		nodeTable[5] = hexa2.getCellId(13);
		nodeTable[6] = hexa2.getCellId(6);
		nodeTable[7] = hexa2.getCellId(14);

		nodeTable[8] = nf3_c;
		nodeTable[9] = nf3_18;
		nodeTable[10] = nf4_18;
		nodeTable[11] = nf4_c;

		nodeTable[12] = nf1_13;
		nodeTable[13] = ne1[5];
		nodeTable[14] = ne0[6];
		nodeTable[15] = nf1_14;

		nodeTable[16] = nf1_c;
		nodeTable[17] = nf3_13;
		nodeTable[18] = ne1[10];
		nodeTable[19] = nf4_14;

		e[6] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );

		nodeTable[0] = nf5;
		nodeTable[1] = c;
		nodeTable[2] = nf4;
		nodeTable[3] = hexa2.getCellId(19);
		nodeTable[4] = hexa2.getCellId(15);
		nodeTable[5] = nf1;
		nodeTable[6] = hexa2.getCellId(14);
		nodeTable[7] = hexa2.getCellId(7);

		nodeTable[8] = nf5_c;
		nodeTable[9] = nf4_c;
		nodeTable[10] = nf4_19;
		nodeTable[11] = nf5_19;

		nodeTable[12] = nf1_15;
		nodeTable[13] = nf1_14;
		nodeTable[14] = ne1[6];
		nodeTable[15] = ne1[7];

		nodeTable[16] = nf5_15;
		nodeTable[17] = nf1_c;
		nodeTable[18] = nf4_14;
		nodeTable[19] = ne1[11];

		e[7] = refinedBody->addElement( kmb::HEXAHEDRON2, nodeTable );
	}
	const int faceNum = kmb::Element::getBoundaryCount( kmb::HEXAHEDRON2 );

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			for(int i=0;i<12;++i){
				nodeGroupUpdate( hexa2.getEdgeCellId(i,0), hexa2.getEdgeCellId(i,2), ne0[i], dIter->originalData, dIter->refinedData );
				nodeGroupUpdate( hexa2.getEdgeCellId(i,2), hexa2.getEdgeCellId(i,1), ne1[i], dIter->originalData, dIter->refinedData );
			}
			nodeGroupUpdate( hexa2, 0, nf0, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa2, 1, nf1, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa2, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa2, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa2, 4, nf4, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa2, 5, nf5, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( hexa2, c, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( nf0, hexa2.getCellId(8),  nf0_8,  dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf0, hexa2.getCellId(9),  nf0_9,  dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf0, hexa2.getCellId(10), nf0_10, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf0, hexa2.getCellId(11), nf0_11, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf0, c, nf0_c, dIter->refinedData, dIter->refinedData );

			nodeGroupUpdate( nf1, hexa2.getCellId(12), nf1_12, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf1, hexa2.getCellId(13), nf1_13, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf1, hexa2.getCellId(14), nf1_14, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf1, hexa2.getCellId(15), nf1_15, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf1, c, nf1_c, dIter->refinedData, dIter->refinedData );

			nodeGroupUpdate( nf2, hexa2.getCellId(8),  nf2_8,  dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf2, hexa2.getCellId(12), nf2_12, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf2, hexa2.getCellId(16), nf2_16, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf2, hexa2.getCellId(17), nf2_17, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf2, c, nf2_c, dIter->refinedData, dIter->refinedData );

			nodeGroupUpdate( nf3, hexa2.getCellId(9),  nf3_9,  dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf3, hexa2.getCellId(13), nf3_13, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf3, hexa2.getCellId(17), nf3_17, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf3, hexa2.getCellId(18), nf3_18, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf3, c, nf3_c, dIter->refinedData, dIter->refinedData );

			nodeGroupUpdate( nf4, hexa2.getCellId(10), nf4_10, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf4, hexa2.getCellId(14), nf4_14, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf4, hexa2.getCellId(18), nf4_18, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf4, hexa2.getCellId(19), nf4_19, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf4, c, nf4_c, dIter->refinedData, dIter->refinedData );

			nodeGroupUpdate( nf5, hexa2.getCellId(11), nf5_11, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf5, hexa2.getCellId(15), nf5_15, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf5, hexa2.getCellId(16), nf5_16, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf5, hexa2.getCellId(19), nf5_19, dIter->refinedData, dIter->refinedData );
			nodeGroupUpdate( nf5, c, nf5_c, dIter->refinedData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			for(int i=0;i<12;++i){
				nodeVariableUpdate( hexa2.getEdgeCellId(i,0), hexa2.getEdgeCellId(i,2), ne0[i], dIter->originalData, dIter->refinedData );
				nodeVariableUpdate( hexa2.getEdgeCellId(i,0), hexa2.getEdgeCellId(i,1), ne1[i], dIter->originalData, dIter->refinedData );
			}
			nodeVariableUpdate( hexa2, 0, nf0, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa2, 1, nf1, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa2, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa2, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa2, 4, nf4, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa2, 5, nf5, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( hexa2, c, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( nf0, hexa2.getCellId(8),  nf0_8,  dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf0, hexa2.getCellId(9),  nf0_9,  dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf0, hexa2.getCellId(10), nf0_10, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf0, hexa2.getCellId(11), nf0_11, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf0, c, nf0_c, dIter->refinedData, dIter->refinedData );

			nodeVariableUpdate( nf1, hexa2.getCellId(12), nf1_12, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf1, hexa2.getCellId(13), nf1_13, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf1, hexa2.getCellId(14), nf1_14, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf1, hexa2.getCellId(15), nf1_15, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf1, c, nf1_c, dIter->refinedData, dIter->refinedData );

			nodeVariableUpdate( nf2, hexa2.getCellId(8),  nf2_8,  dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf2, hexa2.getCellId(12), nf2_12, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf2, hexa2.getCellId(16), nf2_16, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf2, hexa2.getCellId(17), nf2_17, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf2, c, nf2_c, dIter->refinedData, dIter->refinedData );

			nodeVariableUpdate( nf3, hexa2.getCellId(9),  nf3_9,  dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf3, hexa2.getCellId(13), nf3_13, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf3, hexa2.getCellId(17), nf3_17, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf3, hexa2.getCellId(18), nf3_18, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf3, c, nf3_c, dIter->refinedData, dIter->refinedData );

			nodeVariableUpdate( nf4, hexa2.getCellId(10), nf4_10, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf4, hexa2.getCellId(14), nf4_14, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf4, hexa2.getCellId(18), nf4_18, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf4, hexa2.getCellId(19), nf4_19, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf4, c, nf4_c, dIter->refinedData, dIter->refinedData );

			nodeVariableUpdate( nf5, hexa2.getCellId(11), nf5_11, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf5, hexa2.getCellId(15), nf5_15, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf5, hexa2.getCellId(16), nf5_16, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf5, hexa2.getCellId(19), nf5_19, dIter->refinedData, dIter->refinedData );
			nodeVariableUpdate( nf5, c, nf5_c, dIter->refinedData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					for(int j=0;j<4;++j){
						// e の順番は元の節点を含むように作ってあるので
						// face の頂点番号を e の添字に使えばよい
						int index = kmb::Hexahedron2::faceTable[i][j];
						dIter->refinedData->addId( kmb::Face(e[index],i) );
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<8;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4, n5 ]
// => [ n0,  ne2, ne1, ne6, nf2, nf4 ]
// => [ ne2, n1,  ne0, nf2, ne7, nf3 ]
// => [ ne1, ne0, n2,  nf4, nf3, ne8 ]
// => [ ne6, nf2, nf4, n3,  ne5, ne4 ]
// => [ nf2, ne7, nf3, ne5, n4,  ne3 ]
// => [ nf4, nf3, ne8, ne4, ne3, n5  ]
// => [ ne2, ne0, ne1, nf2, nf3, nf4 ]
// => [ ne5, ne4, ne3, nf2, nf4, nf3 ]
// face の順番が親を引き継ぐように並び替える（最後の2個を除く）
// 最後の2個は 0 番目の面が外側をむいている
// nf2 = n0143
// nf3 = n1254
// nf4 = n2035
// ne0 = n12
// ne1 = n02
// ne2 = n01
// ne3 = n45
// ne4 = n35
// ne5 = n34
// ne6 = n03
// ne7 = n14
// ne8 = n25
void
kmb::MeshRefiner::refineWedge( kmb::elementIdType elementId, const kmb::ElementBase &wedge, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[8] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType nodeTable[6] = {
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId
	};
	// 9 = kmb::Element::getEdgeCount( kmb::WEDGE )
	kmb::nodeIdType ne[9];
	for(int i=0;i<9;++i){
		ne[i] = middleMan->getDividedNode( wedge.getEdgeCellId(i,0), wedge.getEdgeCellId(i,1) );
	}
	kmb::nodeIdType nf2 = middleMan->getCenterNode( wedge, 2, elementId );
	kmb::nodeIdType nf3 = middleMan->getCenterNode( wedge, 3, elementId );
	kmb::nodeIdType nf4 = middleMan->getCenterNode( wedge, 4, elementId );

	if( refinedBody ){
		nodeTable[0] = wedge.getCellId(0);
		nodeTable[1] = ne[2];
		nodeTable[2] = ne[1];
		nodeTable[3] = ne[6];
		nodeTable[4] = nf2;
		nodeTable[5] = nf4;
		e[0] = refinedBody->addElement( kmb::WEDGE, nodeTable );

		nodeTable[0] = ne[2];
		nodeTable[1] = wedge.getCellId(1);
		nodeTable[2] = ne[0];
		nodeTable[3] = nf2;
		nodeTable[4] = ne[7];
		nodeTable[5] = nf3;
		e[1] = refinedBody->addElement( kmb::WEDGE, nodeTable );

		nodeTable[0] = ne[1];
		nodeTable[1] = ne[0];
		nodeTable[2] = wedge.getCellId(2);
		nodeTable[3] = nf4;
		nodeTable[4] = nf3;
		nodeTable[5] = ne[8];
		e[2] = refinedBody->addElement( kmb::WEDGE, nodeTable );

		nodeTable[0] = ne[6];
		nodeTable[1] = nf2;
		nodeTable[2] = nf4;
		nodeTable[3] = wedge.getCellId(3);
		nodeTable[4] = ne[5];
		nodeTable[5] = ne[4];
		e[3] = refinedBody->addElement( kmb::WEDGE, nodeTable );

		nodeTable[0] = nf2;
		nodeTable[1] = ne[7];
		nodeTable[2] = nf3;
		nodeTable[3] = ne[5];
		nodeTable[4] = wedge.getCellId(4);
		nodeTable[5] = ne[3];
		e[4] = refinedBody->addElement( kmb::WEDGE, nodeTable );

		nodeTable[0] = nf4;
		nodeTable[1] = nf3;
		nodeTable[2] = ne[8];
		nodeTable[3] = ne[4];
		nodeTable[4] = ne[3];
		nodeTable[5] = wedge.getCellId(5);
		e[5] = refinedBody->addElement( kmb::WEDGE, nodeTable );

		nodeTable[0] = ne[2];
		nodeTable[1] = ne[0];
		nodeTable[2] = ne[1];
		nodeTable[3] = nf2;
		nodeTable[4] = nf3;
		nodeTable[5] = nf4;
		e[6] = refinedBody->addElement( kmb::WEDGE, nodeTable );

		nodeTable[0] = ne[5];
		nodeTable[1] = ne[4];
		nodeTable[2] = ne[3];
		nodeTable[3] = nf2;
		nodeTable[4] = nf4;
		nodeTable[5] = nf3;
		e[7] = refinedBody->addElement( kmb::WEDGE, nodeTable );
	}

	const int faceNum = kmb::Element::getBoundaryCount( kmb::WEDGE );
	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			nodeGroupUpdate( wedge, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge, 4, nf4, dIter->originalData, dIter->refinedData );
			for(int i=0;i<9;++i){
				nodeGroupUpdate( wedge.getEdgeCellId(i,0), wedge.getEdgeCellId(i,1), ne[i], dIter->originalData, dIter->refinedData );
			}
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			nodeVariableUpdate( wedge, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge, 4, nf4, dIter->originalData, dIter->refinedData );
			for(int i=0;i<9;++i){
				nodeVariableUpdate( wedge.getEdgeCellId(i,0), wedge.getEdgeCellId(i,1), ne[i], dIter->originalData, dIter->refinedData );
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					// 頂点番号と細分した要素の添え字が一致するようにしている
					int faceVertexCount = kmb::Element::getBoundaryVertexCount( kmb::WEDGE, i );
					for(int j=0;j<faceVertexCount;++j){
						int index = kmb::Wedge::faceTable[i][j];
						dIter->refinedData->addId( kmb::Face(e[index],i) );
					}
					if( i == 0 ){
						dIter->refinedData->addId( kmb::Face(e[6],0) );
					}else if( i == 1 ){
						dIter->refinedData->addId( kmb::Face(e[7],0) );
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<8;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14 ]
// => [ n0,  n8,  n7,  n12, nf2, nf4, n7_8, ne01, ne02, nf2_f4, nf4_12, nf2_12, ne06, nf2_8, nf4_7]
// => [ n8,  n1,  n6,  nf2, n13, nf3, ne00, n6_8, ne12, nf3_13, nf2_f3, nf2_13, nf2_8, ne07, nf3_6]
// => [ n7,  n6,  n2,  nf4, nf3, n14, ne10, ne11, n6_7, nf3_14, nf4_14, nf3_f4, nf4_7, nf3_6, ne08]
// => [ n12, nf2, nf4, n3,  n11, n10, nf2_f4, nf4_12, nf2_12, n10_11, ne04, ne05, ne16, nf2_11, nf4_10]
// => [ nf2, n13, nf3, n11, n4,  n9,  nf3_13, nf2_f3, nf2_13, ne03, n9_11, ne15, nf2_11, ne17, nf3_9]
// => [ nf4, nf3, n14, n10, n9 , n5,  nf3_14, nf4_14, nf3_f4, ne13, ne14, n9_10, nf4_10, nf3_9, ne18]
// => [ n8,  n6,  n7,  nf2, nf3, nf4, n6_7, n7_8, n6_8, nf3_f4, nf2_f4, nf2_f3, nf2_8, nf3_6, nf4_7]
// => [ n11, n10, n9,  nf2, nf4, nf3, n9_10, n9_11, n10_11, nf3_f4, nf2_f3, nf2_f4, nf2_11, nf4_10, nf3_9]
// face の順番が親を引き継ぐように並び替える（最後の2個を除く）
// nf2 = n0143
// nf3 = n1254
// nf4 = n2035
// ne0_0 = n16  ne1_0 = n26
// ne0_1 = n07  ne1_1 = n27
// ne0_2 = n08  ne1_2 = n18
// ne0_3 = n49  ne1_3 = n59
// ne0_4 = n310 ne1_4 = n510
// ne0_5 = n311 ne1_5 = n411
// ne0_6 = n012 ne1_6 = n312
// ne0_7 = n113 ne1_7 = n413
// ne0_8 = n214 ne1_8 = n514
void
kmb::MeshRefiner::refineWedge2( kmb::elementIdType elementId, const kmb::ElementBase &wedge2, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[8] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType nodeTable[15] = {
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId
	};
	// 9 = kmb::Element::getEdgeCount( kmb::WEDGE )
	kmb::nodeIdType ne0[9];
	kmb::nodeIdType ne1[9];
	kmb::nodeIdType nf2 = kmb::nullNodeId;
	kmb::nodeIdType nf3 = kmb::nullNodeId;
	kmb::nodeIdType nf4 = kmb::nullNodeId;

	kmb::nodeIdType nf2_8  = kmb::nullNodeId;
	kmb::nodeIdType nf2_11 = kmb::nullNodeId;
	kmb::nodeIdType nf2_12 = kmb::nullNodeId;
	kmb::nodeIdType nf2_13 = kmb::nullNodeId;

	kmb::nodeIdType nf3_6  = kmb::nullNodeId;
	kmb::nodeIdType nf3_9  = kmb::nullNodeId;
	kmb::nodeIdType nf3_13 = kmb::nullNodeId;
	kmb::nodeIdType nf3_14 = kmb::nullNodeId;

	kmb::nodeIdType nf4_7  = kmb::nullNodeId;
	kmb::nodeIdType nf4_10 = kmb::nullNodeId;
	kmb::nodeIdType nf4_12 = kmb::nullNodeId;
	kmb::nodeIdType nf4_14 = kmb::nullNodeId;

	kmb::nodeIdType nf2_f3 = kmb::nullNodeId;
	kmb::nodeIdType nf2_f4 = kmb::nullNodeId;
	kmb::nodeIdType nf3_f4 = kmb::nullNodeId;

	kmb::nodeIdType n6_7  = kmb::nullNodeId;
	kmb::nodeIdType n6_8  = kmb::nullNodeId;
	kmb::nodeIdType n7_8  = kmb::nullNodeId;

	kmb::nodeIdType n9_10  = kmb::nullNodeId;
	kmb::nodeIdType n9_11  = kmb::nullNodeId;
	kmb::nodeIdType n10_11 = kmb::nullNodeId;

	if( secondFitting ){
		for(int i=0;i<9;++i){
			ne0[i] = middleMan->getDividedNode3( wedge2.getEdgeCellId(i,0), wedge2.getEdgeCellId(i,2), wedge2.getEdgeCellId(i,1) );
			ne1[i] = middleMan->getDividedNode3( wedge2.getEdgeCellId(i,1), wedge2.getEdgeCellId(i,2), wedge2.getEdgeCellId(i,0) );
		}
		nf2 = middleMan->getCenterNode2( wedge2, 2, elementId );
		nf3 = middleMan->getCenterNode2( wedge2, 3, elementId );
		nf4 = middleMan->getCenterNode2( wedge2, 4, elementId );

		nf2_8  = middleMan->getDividedNode3( wedge2[8],  nf2, wedge2[8] );
		nf2_11 = middleMan->getDividedNode3( wedge2[11], nf2, wedge2[11] );
		nf2_12 = middleMan->getDividedNode3( wedge2[12], nf2, wedge2[13] );
		nf2_13 = middleMan->getDividedNode3( wedge2[13], nf2, wedge2[12] );

		nf3_6  = middleMan->getDividedNode3( wedge2[6],  nf3, wedge2[9] );
		nf3_9  = middleMan->getDividedNode3( wedge2[9],  nf3, wedge2[6] );
		nf3_13 = middleMan->getDividedNode3( wedge2[13], nf3, wedge2[14] );
		nf3_14 = middleMan->getDividedNode3( wedge2[14], nf3, wedge2[13] );

		nf4_7  = middleMan->getDividedNode3( wedge2[7],  nf4, wedge2[10] );
		nf4_10 = middleMan->getDividedNode3( wedge2[10], nf4, wedge2[7] );
		nf4_12 = middleMan->getDividedNode3( wedge2[12], nf4, wedge2[14] );
		nf4_14 = middleMan->getDividedNode3( wedge2[14], nf4, wedge2[12] );

		nf2_f3 = middleMan->getDividedNode5( nf2, nf3, wedge2[12], wedge2[14], nf4 );
		nf2_f4 = middleMan->getDividedNode5( nf2, nf4, wedge2[13], wedge2[14], nf3 );
		nf3_f4 = middleMan->getDividedNode5( nf3, nf4, wedge2[13], wedge2[12], nf2 );

		n6_7  = middleMan->getDividedNode5( wedge2[6], wedge2[7], wedge2[1], wedge2[0], wedge2[8] );
		n6_8  = middleMan->getDividedNode5( wedge2[6], wedge2[8], wedge2[2], wedge2[0], wedge2[7] );
		n7_8  = middleMan->getDividedNode5( wedge2[7], wedge2[8], wedge2[2], wedge2[1], wedge2[6] );

		n9_10  = middleMan->getDividedNode5( wedge2[9],  wedge2[10], wedge2[4], wedge2[3], wedge2[11] );
		n9_11  = middleMan->getDividedNode5( wedge2[9],  wedge2[11], wedge2[5], wedge2[3], wedge2[10] );
		n10_11 = middleMan->getDividedNode5( wedge2[10], wedge2[11], wedge2[5], wedge2[4], wedge2[9] );
	}else{
		for(int i=0;i<9;++i){
			ne0[i] = middleMan->getDividedNode( wedge2.getEdgeCellId(i,0), wedge2.getEdgeCellId(i,2) );
			ne1[i] = middleMan->getDividedNode( wedge2.getEdgeCellId(i,2), wedge2.getEdgeCellId(i,1) );
		}
		nf2 = middleMan->getCenterNode( wedge2, 2, elementId );
		nf3 = middleMan->getCenterNode( wedge2, 3, elementId );
		nf4 = middleMan->getCenterNode( wedge2, 4, elementId );

		nf2_8  = middleMan->getDividedNode( nf2, wedge2[8] );
		nf2_11 = middleMan->getDividedNode( nf2, wedge2[11] );
		nf2_12 = middleMan->getDividedNode( nf2, wedge2[12] );
		nf2_13 = middleMan->getDividedNode( nf2, wedge2[13] );

		nf3_6  = middleMan->getDividedNode( nf3, wedge2[6] );
		nf3_9  = middleMan->getDividedNode( nf3, wedge2[9] );
		nf3_13 = middleMan->getDividedNode( nf3, wedge2[13] );
		nf3_14 = middleMan->getDividedNode( nf3, wedge2[14] );

		nf4_7  = middleMan->getDividedNode( nf4, wedge2[7] );
		nf4_10 = middleMan->getDividedNode( nf4, wedge2[10] );
		nf4_12 = middleMan->getDividedNode( nf4, wedge2[12] );
		nf4_14 = middleMan->getDividedNode( nf4, wedge2[14] );

		nf2_f3 = middleMan->getDividedNode( nf2, nf3 );
		nf2_f4 = middleMan->getDividedNode( nf2, nf4 );
		nf3_f4 = middleMan->getDividedNode( nf3, nf4 );

		n6_7  = middleMan->getDividedNode( wedge2[6], wedge2[7] );
		n6_8  = middleMan->getDividedNode( wedge2[6], wedge2[8] );
		n7_8  = middleMan->getDividedNode( wedge2[7], wedge2[8] );

		n9_10  = middleMan->getDividedNode( wedge2[9],  wedge2[10] );
		n9_11  = middleMan->getDividedNode( wedge2[9],  wedge2[11] );
		n10_11 = middleMan->getDividedNode( wedge2[10], wedge2[11] );
	}

	if( refinedBody ){
		nodeTable[0] = wedge2.getCellId(0);
		nodeTable[1] = wedge2.getCellId(8);
		nodeTable[2] = wedge2.getCellId(7);
		nodeTable[3] = wedge2.getCellId(12);
		nodeTable[4] = nf2;
		nodeTable[5] = nf4;
		nodeTable[6] = n7_8;
		nodeTable[7] = ne0[1];
		nodeTable[8] = ne0[2];
		nodeTable[9] = nf2_f4;
		nodeTable[10] = nf4_12;
		nodeTable[11] = nf2_12;
		nodeTable[12] = ne0[6];
		nodeTable[13] = nf2_8;
		nodeTable[14] = nf4_7;
		e[0] = refinedBody->addElement( kmb::WEDGE2, nodeTable );

// => [ n8,  n1,  n6,  nf2, n13, nf3, ne00, n6_8, ne12, nf3_13, nf2_f3, nf2_13, nf2_8, ne07, nf3_6]
		nodeTable[0] = wedge2.getCellId(8);
		nodeTable[1] = wedge2.getCellId(1);
		nodeTable[2] = wedge2.getCellId(6);
		nodeTable[3] = nf2;
		nodeTable[4] = wedge2.getCellId(13);
		nodeTable[5] = nf3;
		nodeTable[6] = ne0[0];
		nodeTable[7] = n6_8;
		nodeTable[8] = ne1[2];
		nodeTable[9] = nf3_13;
		nodeTable[10] = nf2_f3;
		nodeTable[11] = nf2_13;
		nodeTable[12] = nf2_8;
		nodeTable[13] = ne0[7];
		nodeTable[14] = nf3_6;
		e[1] = refinedBody->addElement( kmb::WEDGE2, nodeTable );

// => [ n7,  n6,  n2,  nf4, nf3, n14, ne10, ne11, n6_7, nf3_14, nf4_14, nf3_f4, nf4_7, nf3_6, ne08]
		nodeTable[0] = wedge2.getCellId(7);
		nodeTable[1] = wedge2.getCellId(6);
		nodeTable[2] = wedge2.getCellId(2);
		nodeTable[3] = nf4;
		nodeTable[4] = nf3;
		nodeTable[5] = wedge2.getCellId(14);
		nodeTable[6] = ne1[0];
		nodeTable[7] = ne1[1];
		nodeTable[8] = n6_7;
		nodeTable[9] = nf3_14;
		nodeTable[10] = nf4_14;
		nodeTable[11] = nf3_f4;
		nodeTable[12] = nf4_7;
		nodeTable[13] = nf3_6;
		nodeTable[14] = ne0[8];
		e[2] = refinedBody->addElement( kmb::WEDGE2, nodeTable );

// => [ n12, nf2, nf4, n3,  n11, n10, nf2_f4, nf4_12, nf2_12, n10_11, ne04, ne05, ne16, nf2_11, nf4_10]
		nodeTable[0] = wedge2.getCellId(12);
		nodeTable[1] = nf2;
		nodeTable[2] = nf4;
		nodeTable[3] = wedge2.getCellId(3);
		nodeTable[4] = wedge2.getCellId(11);
		nodeTable[5] = wedge2.getCellId(10);
		nodeTable[6] = nf2_f4;
		nodeTable[7] = nf4_12;
		nodeTable[8] = nf2_12;
		nodeTable[9] = n10_11;
		nodeTable[10] = ne0[4];
		nodeTable[11] = ne0[5];
		nodeTable[12] = ne1[6];
		nodeTable[13] = nf2_11;
		nodeTable[14] = nf4_10;
		e[3] = refinedBody->addElement( kmb::WEDGE2, nodeTable );

// => [ nf2, n13, nf3, n11, n4,  n9,  nf3_13, nf2_f3, nf2_13, ne03, n9_11, ne15, nf2_11, ne17, nf3_9]
		nodeTable[0] = nf2;
		nodeTable[1] = wedge2.getCellId(13);
		nodeTable[2] = nf3;
		nodeTable[3] = wedge2.getCellId(11);
		nodeTable[4] = wedge2.getCellId(4);
		nodeTable[5] = wedge2.getCellId(9);
		nodeTable[6] = nf3_13;
		nodeTable[7] = nf2_f3;
		nodeTable[8] = nf2_13;
		nodeTable[9] = ne0[3];
		nodeTable[10] = n9_11;
		nodeTable[11] = ne1[5];
		nodeTable[12] = nf2_11;
		nodeTable[13] = ne1[7];
		nodeTable[14] = nf3_9;
		e[4] = refinedBody->addElement( kmb::WEDGE2, nodeTable );

// => [ nf4, nf3, n14, n10, n9 , n5,  nf3_14, nf4_14, nf3_f4, ne13, ne14, n9_10, nf4_10, nf3_9, ne18]
		nodeTable[0] = nf4;
		nodeTable[1] = nf3;
		nodeTable[2] = wedge2.getCellId(14);
		nodeTable[3] = wedge2.getCellId(10);
		nodeTable[4] = wedge2.getCellId(9);
		nodeTable[5] = wedge2.getCellId(5);
		nodeTable[6] = nf3_14;
		nodeTable[7] = nf4_14;
		nodeTable[8] = nf3_f4;
		nodeTable[9] = ne1[3];
		nodeTable[10] = ne1[4];
		nodeTable[11] = n9_10;
		nodeTable[12] = nf4_10;
		nodeTable[13] = nf3_9;
		nodeTable[14] = ne1[8];
		e[5] = refinedBody->addElement( kmb::WEDGE2, nodeTable );

// => [ n8,  n6,  n7,  nf2, nf3, nf4, n6_7, n7_8, n6_8, nf3_f4, nf2_f4, nf2_f3, nf2_8, nf3_6, nf4_7]
		nodeTable[0] = wedge2.getCellId(8);
		nodeTable[1] = wedge2.getCellId(6);
		nodeTable[2] = wedge2.getCellId(7);
		nodeTable[3] = nf2;
		nodeTable[4] = nf3;
		nodeTable[5] = nf4;
		nodeTable[6] = n6_7;
		nodeTable[7] = n7_8;
		nodeTable[8] = n6_8;
		nodeTable[9] = nf3_f4;
		nodeTable[10] = nf2_f4;
		nodeTable[11] = nf2_f3;
		nodeTable[12] = nf2_8;
		nodeTable[13] = nf3_6;
		nodeTable[14] = nf4_7;
		e[6] = refinedBody->addElement( kmb::WEDGE2, nodeTable );

// => [ n11, n10, n9,  nf2, nf4, nf3, n9_10, n9_11, n10_11, nf3_f4, nf2_f3, nf2_f4, nf2_11, nf4_10, nf3_9]
		nodeTable[0] = wedge2.getCellId(11);
		nodeTable[1] = wedge2.getCellId(10);
		nodeTable[2] = wedge2.getCellId(9);
		nodeTable[3] = nf2;
		nodeTable[4] = nf4;
		nodeTable[5] = nf3;
		nodeTable[6] = n9_10;
		nodeTable[7] = n9_11;
		nodeTable[8] = n10_11;
		nodeTable[9] = nf3_f4;
		nodeTable[10] = nf2_f3;
		nodeTable[11] = nf2_f4;
		nodeTable[12] = nf2_11;
		nodeTable[13] = nf4_10;
		nodeTable[14] = nf3_9;
		e[7] = refinedBody->addElement( kmb::WEDGE2, nodeTable );
	}

	const int faceNum = kmb::Element::getBoundaryCount( kmb::WEDGE );
	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			for(int i=0;i<9;++i){
				nodeGroupUpdate( wedge2.getEdgeCellId(i,0), wedge2.getEdgeCellId(i,2), ne0[i], dIter->originalData, dIter->refinedData );
				nodeGroupUpdate( wedge2.getEdgeCellId(i,2), wedge2.getEdgeCellId(i,1), ne0[i], dIter->originalData, dIter->refinedData );
			}

			nodeGroupUpdate( wedge2, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge2, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge2, 4, nf4, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( nf2, wedge2.getCellId(8),  nf2_8, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf2, wedge2.getCellId(11), nf2_11, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf2, wedge2.getCellId(12), nf2_12, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf2, wedge2.getCellId(13), nf2_13, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( nf3, wedge2.getCellId(6),  nf3_6, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf3, wedge2.getCellId(9),  nf3_9, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf3, wedge2.getCellId(13), nf3_13, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf3, wedge2.getCellId(14), nf3_14, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( nf4, wedge2.getCellId(7),  nf4_7, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, wedge2.getCellId(10), nf4_10, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, wedge2.getCellId(12), nf4_12, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, wedge2.getCellId(14), nf4_14, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( nf2, nf3, nf2_f3, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf2, nf4, nf3_f4, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf3, nf4, nf3_f4, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( wedge2.getCellId(6), wedge2.getCellId(7), n6_7, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge2.getCellId(6), wedge2.getCellId(8), n6_8, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge2.getCellId(7), wedge2.getCellId(8), n7_8, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( wedge2.getCellId(9),  wedge2.getCellId(10), n9_10, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge2.getCellId(9),  wedge2.getCellId(11), n9_11, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( wedge2.getCellId(10), wedge2.getCellId(11), n10_11, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			for(int i=0;i<9;++i){
				nodeVariableUpdate( wedge2.getEdgeCellId(i,0), wedge2.getEdgeCellId(i,2), ne0[i], dIter->originalData, dIter->refinedData );
				nodeVariableUpdate( wedge2.getEdgeCellId(i,2), wedge2.getEdgeCellId(i,1), ne0[i], dIter->originalData, dIter->refinedData );
			}

			nodeVariableUpdate( wedge2, 2, nf2, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge2, 3, nf3, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge2, 4, nf4, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( nf2, wedge2.getCellId(8),  nf2_8, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf2, wedge2.getCellId(11), nf2_11, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf2, wedge2.getCellId(12), nf2_12, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf2, wedge2.getCellId(13), nf2_13, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( nf3, wedge2.getCellId(6),  nf3_6, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf3, wedge2.getCellId(9),  nf3_9, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf3, wedge2.getCellId(13), nf3_13, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf3, wedge2.getCellId(14), nf3_14, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( nf4, wedge2.getCellId(7),  nf4_7, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, wedge2.getCellId(10), nf4_10, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, wedge2.getCellId(12), nf4_12, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, wedge2.getCellId(14), nf4_14, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( nf2, nf3, nf2_f3, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf2, nf4, nf3_f4, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf3, nf4, nf3_f4, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( wedge2.getCellId(6), wedge2.getCellId(7), n6_7, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge2.getCellId(6), wedge2.getCellId(8), n6_8, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge2.getCellId(7), wedge2.getCellId(8), n7_8, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( wedge2.getCellId(9),  wedge2.getCellId(10), n9_10, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge2.getCellId(9),  wedge2.getCellId(11), n9_11, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( wedge2.getCellId(10), wedge2.getCellId(11), n10_11, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			for(int i=0;i<faceNum;++i){
				kmb::Face f(elementId, i);
				if( dIter->originalData->hasId( f ) ){
					// faceGroup の更新
					// 頂点番号と細分した要素の添え字が一致するようにしている
					int faceVertexCount = kmb::Element::getBoundaryVertexCount( kmb::WEDGE2, i );
					for(int j=0;j<faceVertexCount;++j){
						int index = kmb::Wedge::faceTable[i][j];
						dIter->refinedData->addId( kmb::Face(e[index],i) );
					}
					if( i == 0 ){
						dIter->refinedData->addId( kmb::Face(e[6],0) );
					}else if( i == 1 ){
						dIter->refinedData->addId( kmb::Face(e[7],0) );
					}
				}
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<8;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4 ]
// => [ n0,  ne0, ne1, ne2, ne3 ]
// => [ ne0, n1,  ne4, nf4, ne7 ]
// => [ ne1, ne4, n2,  ne5, nf4 ]
// => [ ne2, nf4, ne5, n3,  ne6 ]
// => [ ne3, ne7, nf4, ne6, n4  ]
// => [ nf4, ne0, ne4, ne1 ] => [ne0,ne4,ne1] が0番目の面
// => [ ne1, nf4, ne2, ne5 ] => [ne1,ne5,ne2] が1番目の面
// => [ ne2, ne6, nf4, ne3 ] => [ne2,ne6,ne3] が2番目の面
// => [ ne3, ne0, ne7, nf4 ] => [ne3,ne7,ne0] が3番目の面
// => [ nf4, ne3, ne2, ne1, ne0 ] => 外側を向いている面はない
// face の順番が親を引き継ぐように並び替えている
// nf4 => n1234
// ne0 => n01
// ne1 => n02
// ne2 => n03
// ne3 => n04
// ne4 => n12
// ne5 => n23
// ne6 => n34
// ne7 => n14
void
kmb::MeshRefiner::refinePyramid( kmb::elementIdType elementId, const kmb::ElementBase &pyramid, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[10] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType nodeTable[5] = {
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId
	};
	// 8 = kmb::Element::getEdgeCount( kmb::PYRAMID )
	kmb::nodeIdType ne[8];
	for(int i=0;i<8;++i){
		ne[i] = middleMan->getDividedNode( pyramid.getEdgeCellId(i,0), pyramid.getEdgeCellId(i,1) );
	}
	kmb::nodeIdType nf4 = middleMan->getCenterNode( pyramid, 4, elementId );

	if( refinedBody ){
		nodeTable[0] = pyramid.getCellId(0);
		nodeTable[1] = ne[0];
		nodeTable[2] = ne[1];
		nodeTable[3] = ne[2];
		nodeTable[4] = ne[3];
		e[0] = refinedBody->addElement( kmb::PYRAMID, nodeTable );

		nodeTable[0] = ne[0];
		nodeTable[1] = pyramid.getCellId(1);
		nodeTable[2] = ne[4];
		nodeTable[3] = nf4;
		nodeTable[4] = ne[7];
		e[1] = refinedBody->addElement( kmb::PYRAMID, nodeTable );

		nodeTable[0] = ne[1];
		nodeTable[1] = ne[4];
		nodeTable[2] = pyramid.getCellId(2);
		nodeTable[3] = ne[5];
		nodeTable[4] = nf4;
		e[2] = refinedBody->addElement( kmb::PYRAMID, nodeTable );

		nodeTable[0] = ne[2];
		nodeTable[1] = nf4;
		nodeTable[2] = ne[5];
		nodeTable[3] = pyramid.getCellId(3);
		nodeTable[4] = ne[6];
		e[3] = refinedBody->addElement( kmb::PYRAMID, nodeTable );

		nodeTable[0] = ne[3];
		nodeTable[1] = ne[7];
		nodeTable[2] = nf4;
		nodeTable[3] = ne[6];
		nodeTable[4] = pyramid.getCellId(4);
		e[4] = refinedBody->addElement( kmb::PYRAMID, nodeTable );

		nodeTable[0] = nf4;
		nodeTable[1] = ne[0];
		nodeTable[2] = ne[4];
		nodeTable[3] = ne[1];
		e[5] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		nodeTable[0] = ne[1];
		nodeTable[1] = nf4;
		nodeTable[2] = ne[2];
		nodeTable[3] = ne[5];
		e[6] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		nodeTable[0] = ne[2];
		nodeTable[1] = ne[6];
		nodeTable[2] = nf4;
		nodeTable[3] = ne[3];
		e[7] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		nodeTable[0] = ne[3];
		nodeTable[1] = ne[0];
		nodeTable[2] = ne[7];
		nodeTable[3] = nf4;
		e[8] = refinedBody->addElement( kmb::TETRAHEDRON, nodeTable );

		nodeTable[0] = nf4;
		nodeTable[1] = ne[3];
		nodeTable[2] = ne[2];
		nodeTable[3] = ne[1];
		nodeTable[4] = ne[0];
		e[9] = refinedBody->addElement( kmb::PYRAMID, nodeTable );
	}

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			nodeGroupUpdate( pyramid, 4, nf4, dIter->originalData, dIter->refinedData );
			for(int i=0;i<8;++i){
				nodeGroupUpdate( pyramid.getEdgeCellId(i,0), pyramid.getEdgeCellId(i,1), ne[i], dIter->originalData, dIter->refinedData );
			}
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			nodeVariableUpdate( pyramid, 4, nf4, dIter->originalData, dIter->refinedData );
			for(int i=0;i<8;++i){
				nodeVariableUpdate( pyramid.getEdgeCellId(i,0), pyramid.getEdgeCellId(i,1), ne[i], dIter->originalData, dIter->refinedData );
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			// faceGroup の更新
			kmb::Face f0(elementId,0);
			kmb::Face f1(elementId,1);
			kmb::Face f2(elementId,2);
			kmb::Face f3(elementId,3);
			kmb::Face f4(elementId,4);
			if( dIter->originalData->hasId( f0 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],0) );
				dIter->refinedData->addId( kmb::Face(e[1],0) );
				dIter->refinedData->addId( kmb::Face(e[2],0) );
				dIter->refinedData->addId( kmb::Face(e[5],0) );
			}
			if( dIter->originalData->hasId( f1 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],1) );
				dIter->refinedData->addId( kmb::Face(e[2],1) );
				dIter->refinedData->addId( kmb::Face(e[3],1) );
				dIter->refinedData->addId( kmb::Face(e[6],1) );
			}
			if( dIter->originalData->hasId( f2 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],2) );
				dIter->refinedData->addId( kmb::Face(e[3],2) );
				dIter->refinedData->addId( kmb::Face(e[4],2) );
				dIter->refinedData->addId( kmb::Face(e[7],2) );
			}
			if( dIter->originalData->hasId( f3 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],3) );
				dIter->refinedData->addId( kmb::Face(e[4],3) );
				dIter->refinedData->addId( kmb::Face(e[1],3) );
				dIter->refinedData->addId( kmb::Face(e[8],3) );
			}
			if( dIter->originalData->hasId( f4 ) ){
				dIter->refinedData->addId( kmb::Face(e[1],4) );
				dIter->refinedData->addId( kmb::Face(e[2],4) );
				dIter->refinedData->addId( kmb::Face(e[3],4) );
				dIter->refinedData->addId( kmb::Face(e[4],4) );
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<10;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

// [ n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12 ]
// => [ n0,  n5,  n6,  n7,  n8,  ne00,  ne01,  ne02,  ne03,  n5_6,   n6_7,   n7_8,   n5_8  ]
// => [ n5,  n1,  n9,  nf4, n12, ne10,  n5_9,  nf4_5, n5_12, ne04,   nf4_9,  nf4_12, ne07  ]
// => [ n6,  n9,  n2,  n10, nf4, n6_9,  ne11,  n6_10, nf4_6, ne14,   ne05,   nf4_10, nf4_9 ]
// => [ n7,  nf4, n10, n3,  n11, nf4_7, n7_10, ne12,  n7_11, nf4_10, ne15,   ne06,   nf4_11]
// => [ n8,  n12, nf4, n11, n4,  n8_12, nf4_8, n8_11, ne13 , nf4_12, nf4_11, ne16,   ne17 ]
// => [ nf4, n5 , n9,  n6,  n5_9,   nf4_9, nf4_5, nf4_6, n5_6,   n6_9   ] => [n5,n9,n6] が0番目の面
// => [ n6,  nf4, n7,  n10, nf4_7,  n6_7,  nf4_6, n6_10, nf4_10, n7_10  ] => [n6,n10,n7] が1番目の面
// => [ n7,  n11, nf4, n8,  nf4_11, nf4_7, n7_11, n7_8,  n8_11,  nf4_8  ] => [n7,n11,n8] が2番目の面
// => [ n8,  n5,  n12, nf4, n5_12,  n8_12, n5_8,  nf4_8, nf4_5,  nf4_12 ] => [n8,n12,n5] が3番目の面
// => [ nf4, n8,  n7,  n6, n5, nf4_8, nf4_7, nf4_6, nf4_5, n7_8, n6_7, n5_6, n5_8 ] => 外側を向いている面はない
// face の順番が親を引き継ぐように並び替えている
// nf4 => n1234
// ne0 => n5    ne00 => n0_5  ne10 => n1_5
// ne1 => n6    ne01 => n0_6  ne11 => n2_6
// ne2 => n7    ne02 => n0_7  ne12 => n3_7
// ne3 => n8    ne03 => n0_8  ne13 => n4_8
// ne4 => n9    ne04 => n1_9  ne14 => n2_9
// ne5 => n10   ne05 => n2_10 ne15 => n3_10
// ne6 => n11   ne06 => n3_11 ne16 => n4_11
// ne7 => n12   ne07 => n1_12 ne17 => n4_12
void
kmb::MeshRefiner::refinePyramid2( kmb::elementIdType elementId, const kmb::ElementBase &pyramid2, kmb::ElementContainer* refinedBody )
{
	kmb::elementIdType e[10] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId
	};
	kmb::nodeIdType nodeTable[13] = {
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId,
		kmb::nullNodeId
	};
	// 8 = kmb::Element::getEdgeCount( kmb::PYRAMID2 )
	kmb::nodeIdType ne0[8];
	kmb::nodeIdType ne1[8];
	kmb::nodeIdType nf4 = kmb::nullNodeId;
	kmb::nodeIdType nf4_5 = kmb::nullNodeId;
	kmb::nodeIdType nf4_6 = kmb::nullNodeId;
	kmb::nodeIdType nf4_7 = kmb::nullNodeId;
	kmb::nodeIdType nf4_8 = kmb::nullNodeId;
	kmb::nodeIdType nf4_9 = kmb::nullNodeId;
	kmb::nodeIdType nf4_10 = kmb::nullNodeId;
	kmb::nodeIdType nf4_11 = kmb::nullNodeId;
	kmb::nodeIdType nf4_12 = kmb::nullNodeId;

	kmb::nodeIdType n5_6 = kmb::nullNodeId;
	kmb::nodeIdType n5_9 = kmb::nullNodeId;
	kmb::nodeIdType n6_9 = kmb::nullNodeId;

	kmb::nodeIdType n6_7  = kmb::nullNodeId;
	kmb::nodeIdType n6_10 = kmb::nullNodeId;
	kmb::nodeIdType n7_10 = kmb::nullNodeId;

	kmb::nodeIdType n7_8  = kmb::nullNodeId;
	kmb::nodeIdType n7_11 = kmb::nullNodeId;
	kmb::nodeIdType n8_11 = kmb::nullNodeId;

	kmb::nodeIdType n5_8  = kmb::nullNodeId;
	kmb::nodeIdType n5_12 = kmb::nullNodeId;
	kmb::nodeIdType n8_12 = kmb::nullNodeId;
	if( secondFitting ){
		for(int i=0;i<8;++i){
			ne0[i] = middleMan->getDividedNode3( pyramid2.getEdgeCellId(i,0), pyramid2.getEdgeCellId(i,2), pyramid2.getEdgeCellId(i,1) );
			ne1[i] = middleMan->getDividedNode3( pyramid2.getEdgeCellId(i,1), pyramid2.getEdgeCellId(i,2), pyramid2.getEdgeCellId(i,0) );
		}
		nf4 = middleMan->getCenterNode2( pyramid2, 4, elementId );

		nf4_5 = middleMan->getDividedNodePyrmid2c( 5, nf4, pyramid2, elementId );
		nf4_6 = middleMan->getDividedNodePyrmid2c( 6, nf4, pyramid2, elementId );
		nf4_7 = middleMan->getDividedNodePyrmid2c( 7, nf4, pyramid2, elementId );
		nf4_8 = middleMan->getDividedNodePyrmid2c( 8, nf4, pyramid2, elementId );

		nf4_9  = middleMan->getDividedNode3( pyramid2[9],  nf4, pyramid2[11] );
		nf4_10 = middleMan->getDividedNode3( pyramid2[10], nf4, pyramid2[12] );
		nf4_11 = middleMan->getDividedNode3( pyramid2[11], nf4, pyramid2[9] );
		nf4_12 = middleMan->getDividedNode3( pyramid2[12], nf4, pyramid2[10] );

		// 実際には形状関数が三角形２次要素と一致していないのでおかしい
		// n5_6 n6_7 n7_8 n5_8 だけ正しい
		n5_6 = middleMan->getDividedNode5( pyramid2[5], pyramid2[6], pyramid2[1], pyramid2[2], pyramid2[9] );
		n5_9 = middleMan->getDividedNode5( pyramid2[5], pyramid2[9], pyramid2[0], pyramid2[2], pyramid2[6] );
		n6_9 = middleMan->getDividedNode5( pyramid2[6], pyramid2[9], pyramid2[0], pyramid2[1], pyramid2[5] );

		n6_7  = middleMan->getDividedNode5( pyramid2[6], pyramid2[7], pyramid2[2], pyramid2[3], pyramid2[10] );
		n6_10 = middleMan->getDividedNode5( pyramid2[6], pyramid2[10], pyramid2[0], pyramid2[3], pyramid2[7] );
		n7_10 = middleMan->getDividedNode5( pyramid2[7], pyramid2[10], pyramid2[0], pyramid2[2], pyramid2[6] );

		n7_8  = middleMan->getDividedNode5( pyramid2[7], pyramid2[8], pyramid2[3], pyramid2[4], pyramid2[11] );
		n7_11 = middleMan->getDividedNode5( pyramid2[7], pyramid2[11], pyramid2[0], pyramid2[4], pyramid2[8] );
		n8_11 = middleMan->getDividedNode5( pyramid2[8], pyramid2[11], pyramid2[0], pyramid2[3], pyramid2[7] );

		n5_8  = middleMan->getDividedNode5( pyramid2[5], pyramid2[8], pyramid2[1], pyramid2[4], pyramid2[12] );
		n5_12 = middleMan->getDividedNode5( pyramid2[5], pyramid2[12], pyramid2[0], pyramid2[4], pyramid2[8] );
		n8_12 = middleMan->getDividedNode5( pyramid2[8], pyramid2[12], pyramid2[0], pyramid2[1], pyramid2[5] );
	}else{
		for(int i=0;i<8;++i){
			ne0[i] = middleMan->getDividedNode( pyramid2.getEdgeCellId(i,0), pyramid2.getEdgeCellId(i,2) );
			ne1[i] = middleMan->getDividedNode( pyramid2.getEdgeCellId(i,2), pyramid2.getEdgeCellId(i,1) );
		}
		nf4 = middleMan->getCenterNode( pyramid2, 4, elementId );
		nf4_5 = middleMan->getDividedNode( nf4, pyramid2[5] );
		nf4_6 = middleMan->getDividedNode( nf4, pyramid2[6] );
		nf4_7 = middleMan->getDividedNode( nf4, pyramid2[7] );
		nf4_8 = middleMan->getDividedNode( nf4, pyramid2[8] );
		nf4_9 = middleMan->getDividedNode( nf4, pyramid2[9] );
		nf4_10 = middleMan->getDividedNode( nf4, pyramid2[10] );
		nf4_11 = middleMan->getDividedNode( nf4, pyramid2[11] );
		nf4_12 = middleMan->getDividedNode( nf4, pyramid2[12] );

		n5_6 = middleMan->getDividedNode( pyramid2[5], pyramid2[6] );
		n5_9 = middleMan->getDividedNode( pyramid2[5], pyramid2[9] );
		n6_9 = middleMan->getDividedNode( pyramid2[6], pyramid2[9] );

		n6_7  = middleMan->getDividedNode( pyramid2[6], pyramid2[7] );
		n6_10 = middleMan->getDividedNode( pyramid2[6], pyramid2[10] );
		n7_10 = middleMan->getDividedNode( pyramid2[7], pyramid2[10] );

		n7_8  = middleMan->getDividedNode( pyramid2[7], pyramid2[8] );
		n7_11 = middleMan->getDividedNode( pyramid2[7], pyramid2[11] );
		n8_11 = middleMan->getDividedNode( pyramid2[8], pyramid2[11] );

		n5_8  = middleMan->getDividedNode( pyramid2[5], pyramid2[8] );
		n5_12 = middleMan->getDividedNode( pyramid2[5], pyramid2[12] );
		n8_12 = middleMan->getDividedNode( pyramid2[8], pyramid2[12] );
	}

	if( refinedBody ){
// => [ n0,  n5,  n6,  n7,  n8,  ne00,  ne01,  ne02,  ne03,  n5_6,   n6_7,   n7_8,   n5_8  ]
		nodeTable[0] = pyramid2.getCellId(0);
		nodeTable[1] = pyramid2.getCellId(5);
		nodeTable[2] = pyramid2.getCellId(6);
		nodeTable[3] = pyramid2.getCellId(7);
		nodeTable[4] = pyramid2.getCellId(8);
		nodeTable[5] = ne0[0];
		nodeTable[6] = ne0[1];
		nodeTable[7] = ne0[2];
		nodeTable[8] = ne0[3];
		nodeTable[9] = n5_6;
		nodeTable[10] = n6_7;
		nodeTable[11] = n7_8;
		nodeTable[12] = n5_8;
		e[0] = refinedBody->addElement( kmb::PYRAMID2, nodeTable );

// => [ n5,  n1,  n9,  nf4, n12, ne10,  n5_9,  nf4_5, n5_12, ne04,   nf4_9,  nf4_12, ne07  ]
		nodeTable[0] = pyramid2.getCellId(5);
		nodeTable[1] = pyramid2.getCellId(1);
		nodeTable[2] = pyramid2.getCellId(9);
		nodeTable[3] = nf4;
		nodeTable[4] = pyramid2.getCellId(12);
		nodeTable[5] = ne1[0];
		nodeTable[6] = n5_9;
		nodeTable[7] = nf4_5;
		nodeTable[8] = n5_12;
		nodeTable[9] = ne0[4];
		nodeTable[10] = nf4_9;
		nodeTable[11] = nf4_12;
		nodeTable[12] = ne0[7];
		e[1] = refinedBody->addElement( kmb::PYRAMID2, nodeTable );

// => [ n6,  n9,  n2,  n10, nf4, n6_9,  ne11,  n6_10, nf4_6, ne14,   ne05,   nf4_10, nf4_9 ]
		nodeTable[0] = pyramid2.getCellId(6);
		nodeTable[1] = pyramid2.getCellId(9);
		nodeTable[2] = pyramid2.getCellId(2);
		nodeTable[3] = pyramid2.getCellId(10);
		nodeTable[4] = nf4;
		nodeTable[5] = n6_9;
		nodeTable[6] = ne1[1];
		nodeTable[7] = n6_10;
		nodeTable[8] = nf4_6;
		nodeTable[9] = ne1[4];
		nodeTable[10] = ne0[5];
		nodeTable[11] = nf4_10;
		nodeTable[12] = nf4_9;
		e[2] = refinedBody->addElement( kmb::PYRAMID2, nodeTable );

// => [ n7,  nf4, n10, n3,  n11, nf4_7, n7_10, ne12,  n7_11, nf4_10, ne15,   ne06,   nf4_11]
		nodeTable[0] = pyramid2.getCellId(7);
		nodeTable[1] = nf4;
		nodeTable[2] = pyramid2.getCellId(10);
		nodeTable[3] = pyramid2.getCellId(3);
		nodeTable[4] = pyramid2.getCellId(11);
		nodeTable[5] = nf4_7;
		nodeTable[6] = n7_10;
		nodeTable[7] = ne1[2];
		nodeTable[8] = n7_11;
		nodeTable[9] = nf4_10;
		nodeTable[10] = ne1[5];
		nodeTable[11] = ne0[6];
		nodeTable[12] = nf4_11;
		e[3] = refinedBody->addElement( kmb::PYRAMID2, nodeTable );

// => [ n8,  n12, nf4, n11, n4,  n8_12, nf4_8, n8_11, ne13 , nf4_12, nf4_11, ne16,   ne17 ]
		nodeTable[0] = pyramid2.getCellId(8);
		nodeTable[1] = pyramid2.getCellId(12);
		nodeTable[2] = nf4;
		nodeTable[3] = pyramid2.getCellId(11);
		nodeTable[4] = pyramid2.getCellId(4);
		nodeTable[5] = n8_12;
		nodeTable[6] = nf4_8;
		nodeTable[7] = n8_11;
		nodeTable[8] = ne1[3];
		nodeTable[9] = nf4_12;
		nodeTable[10] = nf4_11;
		nodeTable[11] = ne1[6];
		nodeTable[12] = ne1[7];
		e[4] = refinedBody->addElement( kmb::PYRAMID2, nodeTable );

// => [ nf4, n5 , n9,  n6,  n5_9,   nf4_9, nf4_5, nf4_6, n5_6,   n6_9   ] => [n5,n9,n6] が0番目の面
		nodeTable[0] = nf4;
		nodeTable[1] = pyramid2.getCellId(5);
		nodeTable[2] = pyramid2.getCellId(9);
		nodeTable[3] = pyramid2.getCellId(6);
		nodeTable[4] = n5_9;
		nodeTable[5] = nf4_9;
		nodeTable[6] = nf4_5;
		nodeTable[7] = nf4_6;
		nodeTable[8] = n5_6;
		nodeTable[9] = n6_9;
		e[5] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

// => [ n6,  nf4, n7,  n10, nf4_7,  n6_7,  nf4_6, n6_10, nf4_10, n7_10  ] => [n6,n10,n7] が1番目の面
		nodeTable[0] = pyramid2.getCellId(6);
		nodeTable[1] = nf4;
		nodeTable[2] = pyramid2.getCellId(7);
		nodeTable[3] = pyramid2.getCellId(10);
		nodeTable[4] = nf4_7;
		nodeTable[5] = n6_7;
		nodeTable[6] = nf4_6;
		nodeTable[7] = n6_10;
		nodeTable[8] = nf4_10;
		nodeTable[9] = n7_10;
		e[6] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

// => [ n7,  n11, nf4, n8,  nf4_11, nf4_7, n7_11, n7_8,  n8_11,  nf4_8  ] => [n7,n11,n8] が2番目の面
		nodeTable[0] = pyramid2.getCellId(7);
		nodeTable[1] = pyramid2.getCellId(11);
		nodeTable[2] = nf4;
		nodeTable[3] = pyramid2.getCellId(8);
		nodeTable[4] = nf4_11;
		nodeTable[5] = nf4_7;
		nodeTable[6] = n7_11;
		nodeTable[7] = n7_8;
		nodeTable[8] = n8_11;
		nodeTable[9] = nf4_8;
		e[7] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

// => [ n8,  n5,  n12, nf4, n5_12,  n8_12, n5_8,  nf4_8, nf4_5,  nf4_12 ] => [n8,n12,n5] が3番目の面
		nodeTable[0] = pyramid2.getCellId(8);
		nodeTable[1] = pyramid2.getCellId(5);
		nodeTable[2] = pyramid2.getCellId(12);
		nodeTable[3] = nf4;
		nodeTable[4] = n5_12;
		nodeTable[5] = n8_12;
		nodeTable[6] = n5_8;
		nodeTable[7] = nf4_8;
		nodeTable[8] = nf4_5;
		nodeTable[9] = nf4_12;
		e[8] = refinedBody->addElement( kmb::TETRAHEDRON2, nodeTable );

// => [ nf4, n8,  n7,  n6, n5, nf4_8, nf4_7, nf4_6, nf4_5, n7_8, n6_7, n5_6, n5_8 ] => 外側を向いている面はない
		nodeTable[0] = nf4;
		nodeTable[1] = pyramid2.getCellId(8);
		nodeTable[2] = pyramid2.getCellId(7);
		nodeTable[3] = pyramid2.getCellId(6);
		nodeTable[4] = pyramid2.getCellId(5);
		nodeTable[5] = nf4_8;
		nodeTable[6] = nf4_7;
		nodeTable[7] = nf4_6;
		nodeTable[8] = nf4_5;
		nodeTable[9] = n7_8;
		nodeTable[10] = n6_7;
		nodeTable[11] = n5_6;
		nodeTable[12] = n5_8;
		e[9] = refinedBody->addElement( kmb::PYRAMID2, nodeTable );
	}

	std::vector< kmb::MeshRefiner::DataPair >::iterator dIter = dataPairs.begin();
	while( dIter != dataPairs.end() ){
		if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeGroup ){
			for(int i=0;i<8;++i){
				nodeGroupUpdate( pyramid2.getEdgeCellId(i,0), pyramid2.getEdgeCellId(i,2), ne0[i], dIter->originalData, dIter->refinedData );
				nodeGroupUpdate( pyramid2.getEdgeCellId(i,2), pyramid2.getEdgeCellId(i,1), ne1[i], dIter->originalData, dIter->refinedData );
			}

			nodeGroupUpdate( pyramid2, 4, nf4, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( nf4, pyramid2.getCellId(5),  nf4_5, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, pyramid2.getCellId(6),  nf4_6, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, pyramid2.getCellId(7),  nf4_7, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, pyramid2.getCellId(8),  nf4_8, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, pyramid2.getCellId(9),  nf4_9, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, pyramid2.getCellId(10), nf4_10, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, pyramid2.getCellId(11), nf4_11, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( nf4, pyramid2.getCellId(12), nf4_12, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( pyramid2.getCellId(5), pyramid2.getCellId(6),  n5_6, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(5), pyramid2.getCellId(9),  n5_9, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(6), pyramid2.getCellId(9),  n6_9, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( pyramid2.getCellId(6), pyramid2.getCellId(7),   n6_7, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(6), pyramid2.getCellId(10),  n6_10, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(7), pyramid2.getCellId(10),  n7_10, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( pyramid2.getCellId(7), pyramid2.getCellId(8),   n7_8, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(7), pyramid2.getCellId(11),  n7_11, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(8), pyramid2.getCellId(11),  n8_11, dIter->originalData, dIter->refinedData );

			nodeGroupUpdate( pyramid2.getCellId(5), pyramid2.getCellId(8),   n5_8, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(5), pyramid2.getCellId(12),  n5_12, dIter->originalData, dIter->refinedData );
			nodeGroupUpdate( pyramid2.getCellId(8), pyramid2.getCellId(12),  n8_12, dIter->originalData, dIter->refinedData );
		}
		else if( dIter->originalData->getBindingMode() == kmb::DataBindings::NodeVariable &&
			dIter->originalData->getValueType() == kmb::PhysicalValue::Integer )
		{
			for(int i=0;i<8;++i){
				nodeVariableUpdate( pyramid2.getEdgeCellId(i,0), pyramid2.getEdgeCellId(i,2), ne0[i], dIter->originalData, dIter->refinedData );
				nodeVariableUpdate( pyramid2.getEdgeCellId(i,2), pyramid2.getEdgeCellId(i,1), ne1[i], dIter->originalData, dIter->refinedData );
			}

			nodeVariableUpdate( pyramid2, 4, nf4, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( nf4, pyramid2.getCellId(5),  nf4_5, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, pyramid2.getCellId(6),  nf4_6, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, pyramid2.getCellId(7),  nf4_7, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, pyramid2.getCellId(8),  nf4_8, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, pyramid2.getCellId(9),  nf4_9, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, pyramid2.getCellId(10), nf4_10, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, pyramid2.getCellId(11), nf4_11, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( nf4, pyramid2.getCellId(12), nf4_12, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( pyramid2.getCellId(5), pyramid2.getCellId(6),  n5_6, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(5), pyramid2.getCellId(9),  n5_9, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(6), pyramid2.getCellId(9),  n6_9, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( pyramid2.getCellId(6), pyramid2.getCellId(7),   n6_7, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(6), pyramid2.getCellId(10),  n6_10, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(7), pyramid2.getCellId(10),  n7_10, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( pyramid2.getCellId(7), pyramid2.getCellId(8),   n7_8, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(7), pyramid2.getCellId(11),  n7_11, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(8), pyramid2.getCellId(11),  n8_11, dIter->originalData, dIter->refinedData );

			nodeVariableUpdate( pyramid2.getCellId(5), pyramid2.getCellId(8),   n5_8, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(5), pyramid2.getCellId(12),  n5_12, dIter->originalData, dIter->refinedData );
			nodeVariableUpdate( pyramid2.getCellId(8), pyramid2.getCellId(12),  n8_12, dIter->originalData, dIter->refinedData );
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::FaceGroup ){
			// faceGroup の更新
			kmb::Face f0(elementId,0);
			kmb::Face f1(elementId,1);
			kmb::Face f2(elementId,2);
			kmb::Face f3(elementId,3);
			kmb::Face f4(elementId,4);
			if( dIter->originalData->hasId( f0 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],0) );
				dIter->refinedData->addId( kmb::Face(e[1],0) );
				dIter->refinedData->addId( kmb::Face(e[2],0) );
				dIter->refinedData->addId( kmb::Face(e[5],0) );
			}
			if( dIter->originalData->hasId( f1 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],1) );
				dIter->refinedData->addId( kmb::Face(e[2],1) );
				dIter->refinedData->addId( kmb::Face(e[3],1) );
				dIter->refinedData->addId( kmb::Face(e[6],1) );
			}
			if( dIter->originalData->hasId( f2 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],2) );
				dIter->refinedData->addId( kmb::Face(e[3],2) );
				dIter->refinedData->addId( kmb::Face(e[4],2) );
				dIter->refinedData->addId( kmb::Face(e[7],2) );
			}
			if( dIter->originalData->hasId( f3 ) ){
				dIter->refinedData->addId( kmb::Face(e[0],3) );
				dIter->refinedData->addId( kmb::Face(e[4],3) );
				dIter->refinedData->addId( kmb::Face(e[1],3) );
				dIter->refinedData->addId( kmb::Face(e[8],3) );
			}
			if( dIter->originalData->hasId( f4 ) ){
				dIter->refinedData->addId( kmb::Face(e[1],4) );
				dIter->refinedData->addId( kmb::Face(e[2],4) );
				dIter->refinedData->addId( kmb::Face(e[3],4) );
				dIter->refinedData->addId( kmb::Face(e[4],4) );
			}
		}
		else if( refinedBody && dIter->originalData->getBindingMode() == kmb::DataBindings::ElementGroup ){
			if( dIter->originalData->hasId(elementId) ){
				for(int i=0;i<10;++i){
					dIter->refinedData->addId( e[i] );
				}
			}
		}
		++dIter;
	}
}

kmb::elementType
kmb::MeshRefiner::getOriginal(kmb::nodeIdType middleNodeId, kmb::nodeIdType* originalNodes) const
{
	kmb::nodeIdType a=kmb::nullNodeId,b=kmb::nullNodeId;
	if( middleMan && mesh ){
		kmb::elementIdType elementId = middleMan->getOriginalNode(middleNodeId,a,b);
		if( elementId != kmb::Element::nullElementId ){
			kmb::ElementContainer::const_iterator eIter = mesh->findElement( elementId );
			if( !eIter.isFinished() ){
				// 要素の全体を使っているのか、面を使っているのかの判断をする
				int i0 = eIter.indexOf(a);
				int i1 = eIter.indexOf(b);
				int faceIndex = eIter.getFaceIndex(i0,i1);
				int boundaryCount = eIter.getBoundaryCount();
				if( faceIndex == -1 ){
					int num = eIter.getNodeCount();
					for(int i=0;i<num;++i){
						originalNodes[i] = eIter.getCellId(i);
					}
					return eIter.getType();
				}else if( faceIndex < boundaryCount ){
					int num = eIter.getBoundaryNodeCount( faceIndex );
					for(int i=0;i<num;++i){
						originalNodes[i] = eIter.getBoundaryCellId( faceIndex, i );
					}
					return eIter.getBoundaryType( faceIndex );
				}else if( faceIndex < boundaryCount + eIter.getEdgeCount() ){
					int num = eIter.getEdgeNodeCount( faceIndex - boundaryCount );
					for(int i=0;i<num;++i){
						originalNodes[i] = eIter.getEdgeCellId( faceIndex - boundaryCount, i );
					}
					return eIter.getEdgeType( faceIndex - boundaryCount );
				}
			}
		}else if( a != kmb::nullNodeId && b != kmb::nullNodeId ){
			originalNodes[0] = a;
			originalNodes[1] = b;
			return kmb::SEGMENT;
		}
	}
	return kmb::UNKNOWNTYPE;
}

// 注意
// 対角線を無理やり SEGMENT として中間点を求めると QUAD / HEXAHEDRON の中間点を求めてしまう
kmb::nodeIdType
kmb::MeshRefiner::getMiddle(kmb::ElementBase &element) const
{
	kmb::nodeIdType middle = kmb::nullNodeId;
	int minIndex = element.getIndexMinNodeId();
	int diagIndex = element.getDiagIndex(minIndex);
	if( middleMan && minIndex >= 0 && diagIndex >= 0 ){
		switch(element.getType()){
		case kmb::SEGMENT:
		case kmb::QUAD:
		case kmb::QUAD2:
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			middle = middleMan->isDivided( element.getCellId(minIndex), element.getCellId(diagIndex) );
			break;
		default:
			break;
		}
	}
	return middle;
}

bool
kmb::MeshRefiner::nodeGroupUpdate( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n01, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData)
{
	if( originalData==NULL || refinedData==NULL || n01 == kmb::nullNodeId ){
		return false;
	}
	bool res0 = originalData->hasId( n0 );
	bool res1 = originalData->hasId( n1 );
	if( res0 )	refinedData->addId( n0 );
	if( res1 )	refinedData->addId( n1 );
	if( res0 && res1 ){
		refinedData->addId( n01 );
		return true;
	}
	return false;
}

bool
kmb::MeshRefiner::nodeGroupUpdate( const kmb::ElementBase &elem, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData)
{
	if( originalData==NULL || refinedData==NULL || centerId == kmb::nullNodeId){
		return false;
	}
	int len = elem.getNodeCount();
	bool flag = true;
	for(int i=0;i<len;++i){
		kmb::nodeIdType nodeId = elem.getCellId(i);
		bool res = originalData->hasId( nodeId );
		if( res ){
			refinedData->addId( nodeId );
		}
		flag &= res;
	}
	if( flag ){
		refinedData->addId( centerId );
		return true;
	}
	return false;
}

bool
kmb::MeshRefiner::nodeGroupUpdate( const kmb::ElementBase &elem, int faceIndex, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData)
{
	if( originalData==NULL || refinedData==NULL || centerId == kmb::nullNodeId ){
		return false;
	}
	int len = elem.getBoundaryNodeCount(faceIndex);
	bool flag = true;
	for(int i=0;i<len;++i){
		kmb::nodeIdType nodeId = elem.getBoundaryCellId(faceIndex,i);
		bool res = originalData->hasId( nodeId );
		if( res ){
			refinedData->addId( nodeId );
		}
		flag &= res;
	}
	if( flag ){
		refinedData->addId( centerId );
		return true;
	}
	return false;
}

bool
kmb::MeshRefiner::nodeVariableUpdate( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n01, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData)
{
	if( originalData==NULL || refinedData==NULL || n01 == kmb::nullNodeId ){
		return false;
	}
	long v0 = 0, v1 = 0;
	bool res0 = originalData->getPhysicalValue(n0,&v0);
	bool res1 = originalData->getPhysicalValue(n1,&v1);
	if( res0 )	refinedData->setPhysicalValue(n0,&v0);
	if( res1 )	refinedData->setPhysicalValue(n1,&v1);
	if( res0 && res1 )
	{
		switch(interpRule){
			case kmb::MeshRefiner::MIN:
				{
					long v2 = (v0<v1) ? v0 : v1;
					refinedData->setPhysicalValue(n01,&v2);
				}
				break;
			case kmb::MeshRefiner::MAX:
				{
					long v2 = (v0>v1) ? v0 : v1;
					refinedData->setPhysicalValue(n01,&v2);
				}
				break;
			case kmb::MeshRefiner::MIDDLE:
				{
					long v2 = (v0+v1)/2;
					refinedData->setPhysicalValue(n01,&v2);
				}
				break;
			default:
				break;
		}
		return true;
	}
	return false;
}

bool
kmb::MeshRefiner::nodeVariableUpdate( const kmb::ElementBase &elem, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData)
{
	if( originalData==NULL || refinedData==NULL || centerId == kmb::nullNodeId ){
		return false;
	}
	kmb::IntegerCalculator intCalc;
	int len = elem.getNodeCount();
	bool flag = true;
	long v = 0L;
	for(int i=0;i<len;++i){
		kmb::nodeIdType nodeId = elem.getCellId(i);
		bool res = originalData->getPhysicalValue( nodeId, &v );
		if( res ){
			refinedData->setPhysicalValue(nodeId,&v);
			intCalc.update( v );
		}
		flag &= res;
	}
	if( flag ){
		switch(interpRule){
			case kmb::MeshRefiner::MIN:
			{
				v = intCalc.getMin();
				refinedData->setPhysicalValue(centerId,&v);
			}
			break;
			case kmb::MeshRefiner::MAX:
			{
				v = intCalc.getMax();
				refinedData->setPhysicalValue(centerId,&v);
			}
			break;
			case kmb::MeshRefiner::MIDDLE:
			{
				v = intCalc.getAverage();
				refinedData->setPhysicalValue(centerId,&v);
			}
			break;
		}
		return true;
	}
	return false;
}

bool
kmb::MeshRefiner::nodeVariableUpdate( const kmb::ElementBase &elem, int faceIndex, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData)
{
	if( originalData==NULL || refinedData==NULL || centerId == kmb::nullNodeId ){
		return false;
	}
	kmb::IntegerCalculator intCalc;
	int len = elem.getBoundaryNodeCount( faceIndex );
	bool flag = true;
	long v = 0L;
	for(int i=0;i<len;++i){
		kmb::nodeIdType nodeId = elem.getBoundaryCellId(faceIndex,i);
		bool res = originalData->getPhysicalValue( nodeId, &v );
		if( res ){
			refinedData->setPhysicalValue(nodeId,&v);
			intCalc.update( v );
		}
		flag &= res;
	}
	if( flag ){
		switch(interpRule){
			case kmb::MeshRefiner::MIN:
			{
				v = intCalc.getMin();
				refinedData->setPhysicalValue(centerId,&v);
			}
			break;
			case kmb::MeshRefiner::MAX:
			{
				v = intCalc.getMax();
				refinedData->setPhysicalValue(centerId,&v);
			}
			break;
			case kmb::MeshRefiner::MIDDLE:
			{
				v = intCalc.getAverage();
				refinedData->setPhysicalValue(centerId,&v);
			}
			break;
		}
		return true;
	}
	return false;
}
