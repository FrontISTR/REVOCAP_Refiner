/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : SurfaceMatching                                         #
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
#include "MeshDB/kmbSurfaceMatching.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbCollision.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbNodeNeighborFaceInfo.h"
#include "MeshDB/kmbElementRelation.h"
#include "Matrix/kmbMatrix_DoubleArray.h"
#include "Geometry/kmb_Calculator.h"
#include "Geometry/kmb_Debug.h"

kmb::SurfaceMatching::SurfaceMatching(void)
: mesh(NULL)
, masterId(kmb::Body::nullBodyId)
, masterSurf(NULL)
, slaveFaceGroup(NULL)
, neighborInfo(NULL)
, distanceMatrix(NULL)
, elementIds(NULL)
, faces(NULL)
, collision(NULL)
, insertedNodes(NULL)
{
}

kmb::SurfaceMatching::~SurfaceMatching(void)
{
	clear();
	if( collision != NULL ){
		delete collision;
		collision = NULL;
	}
	if( neighborInfo != NULL ){
		delete neighborInfo;
		neighborInfo = NULL;
	}
}

void
kmb::SurfaceMatching::clear(void)
{
	mapping.clear();
	if( neighborInfo != NULL ){
		neighborInfo->clear();
	}
	masterSurf = NULL;
	slaveFaceGroup = NULL;
	if( distanceMatrix != NULL ){
		delete distanceMatrix;
		distanceMatrix = NULL;
	}
	if( elementIds != NULL ){
		delete[] elementIds;
		elementIds = NULL;
	}
	if( faces != NULL ){
		delete[] faces;
		faces = NULL;
	}
}

void
kmb::SurfaceMatching::setMesh(kmb::MeshData* m,const char* insNodes)
{
	if( m != NULL ){
		this->mesh = m;
		if( collision ){
			delete collision;
			collision = NULL;
		}
		this->collision = new kmb::Collision(mesh);
		this->neighborInfo = new kmb::NodeNeighborFaceInfo();
		if( insNodes == NULL ){
			this->insertedNodes = NULL;
		}else{
			this->insertedNodes = this->mesh->getDataBindingsPtr( insNodes );
			if( this->insertedNodes == NULL ){
				this->insertedNodes = mesh->createDataBindings( insNodes, kmb::DataBindings::NODEGROUP, kmb::PhysicalValue::NONE );
			}
		}
	}
}

void
kmb::SurfaceMatching::setPair(kmb::bodyIdType bodyId,const char* faceGroup)
{
	if( this->mesh != NULL && this->collision != NULL && this->neighborInfo != NULL ){
		clear();
		masterId = bodyId;
		masterSurf = mesh->getBodyPtr(bodyId);
		if( masterSurf == NULL || masterSurf->getDimension() != 2 ){
			masterSurf = NULL;
			return;
		}
		slaveName = faceGroup;
		slaveFaceGroup = mesh->getDataBindingsPtr(faceGroup);
		if( slaveFaceGroup == NULL || slaveFaceGroup->getBindingMode() != kmb::DataBindings::FACEGROUP ){
			masterSurf = NULL;
			slaveFaceGroup = NULL;
			return;
		}
		kmb::ElementContainer* slaveElements = mesh->getBodyPtr( slaveFaceGroup->getTargetBodyId() );
		if( slaveElements == NULL ){
			masterSurf = NULL;
			slaveFaceGroup = NULL;
			return;
		}
		neighborInfo->appendCoboundary( slaveFaceGroup, slaveElements );
		int masterLen = static_cast<int>(masterSurf->getCount());
		int slaveLen = static_cast<int>(slaveFaceGroup->getIdCount());
		if( masterLen == 0 || slaveLen == 0 ){
			masterSurf = NULL;
			slaveFaceGroup = NULL;
			return;
		}



		distanceMatrix = new kmb::Matrix_DoubleArray( masterLen, slaveLen );
		elementIds = new kmb::elementIdType[masterLen];
		faces = new kmb::Face[slaveLen];
		Face f;
		int i=0,j=0;
		kmb::DataBindings::iterator sIter = slaveFaceGroup->begin();
		while( !sIter.isFinished() ){
			sIter.getFace( faces[j] );
			++sIter;
			++j;
		}
		kmb::ElementContainer::iterator mIter = masterSurf->begin();
		while( !mIter.isFinished() ){
			elementIds[i] = mIter.getId();
			for(j=0;j<slaveLen;++j){
				int index = -1;
				double d = collision->distanceByNode( mIter, faces[j], slaveElements, index );
				distanceMatrix->set(i,j,d);
			}
			++mIter;
			++i;
		}
		calcMapping();

	}
}


void
kmb::SurfaceMatching::calcMapping(void)
{
	if( distanceMatrix == NULL || elementIds == NULL || faces == NULL ){
		return;
	}
	int rSize = distanceMatrix->getRowSize();
	int cSize = distanceMatrix->getColSize();
	for(int i=0;i<rSize;++i){
		kmb::Minimizer min;
		int min_j = -1;
		for(int j=0;j<cSize;++j){
			if( mapping.find( faces[j] ) == mapping.end() ){
				if( min.update( distanceMatrix->get(i,j) ) ){
					min_j = j;
				}
			}
		}
		int index = -1;
		kmb::ElementContainer* slaveElements = mesh->getBodyPtr( slaveFaceGroup->getTargetBodyId() );
		if( slaveElements == NULL ){
			return;
		}
		kmb::ElementContainer::iterator eIter = masterSurf->find( elementIds[i] );
		collision->distanceByNode( eIter, faces[min_j], slaveElements, index );
		kmb::SurfaceMatching::rotatedElement re = {elementIds[i],index};
		mapping.insert( std::pair< kmb::Face, kmb::SurfaceMatching::rotatedElement >( faces[min_j], re ) );
	}
}

size_t
kmb::SurfaceMatching::constructDummyElements(void)
{
	size_t appendCount = 0;
	if( this->slaveFaceGroup == NULL )
	{
		return appendCount;
	}

	for(int i=0;i<3;++i){
		kmb::DataBindings::iterator fIter = slaveFaceGroup->begin();
		while( !fIter.isFinished() ){
			kmb::Face f;
			if( fIter.getFace(f) ){
				std::map< kmb::Face, kmb::SurfaceMatching::rotatedElement >::iterator mIter = mapping.find(f);
				if( mIter == mapping.end() ){
					kmb::elementIdType elementId = appendDummyElement(f);
					if( elementId != kmb::Element::nullElementId ){
						++appendCount;
					}
				}
			}
			++fIter;
		}
	}
	return appendCount;
}

kmb::elementIdType
kmb::SurfaceMatching::getMatchingElementId(kmb::Face f,int &index)
{
	REVOCAP_Debug("getMatchingElementId 0\n");
	std::map< kmb::Face, kmb::SurfaceMatching::rotatedElement >::iterator mIter = mapping.find(f);
	if( mIter == mapping.end() ){
		REVOCAP_Debug("getMatchingElementId 1 no match %d %d\n", f.getElementId(), f.getLocalFaceId() );

		kmb::elementIdType elementId = appendDummyElement(f);
		if( elementId != kmb::Element::nullElementId ){
			index = 0;
			return elementId;
		}
		return kmb::Element::nullElementId;
	}else{
		REVOCAP_Debug("getMatchingElementId 1 match %d %d => %d\n", f.getElementId(), f.getLocalFaceId(), mIter->second.elementId );
		index = mIter->second.index;
		return mIter->second.elementId;
	}
}



kmb::elementIdType
kmb::SurfaceMatching::appendDummyElement(kmb::Face f)
{
	REVOCAP_Debug("appendDummyElement 0 [%d %d]\n", f.getElementId(), f.getLocalFaceId());
	if( this->mesh == NULL || this->neighborInfo == NULL ||
		this->masterSurf == NULL || this->slaveFaceGroup == NULL ||
		f.getElementId() == kmb::Element::nullElementId )
	{
		return kmb::Element::nullElementId;
	}
	REVOCAP_Debug("appendDummyElement 1\n");
	kmb::ElementContainer* slaveElements = mesh->getBodyPtr( slaveFaceGroup->getTargetBodyId() );
	if( slaveElements == NULL ){
		return kmb::Element::nullElementId;
	}
	if( !slaveFaceGroup->hasId(f) ){
		return kmb::Element::nullElementId;
	}
	REVOCAP_Debug("appendDummyElement 2\n");

	kmb::elementIdType appendId = kmb::Element::nullElementId;

	kmb::elementType etype = f.getFaceElementType( mesh );
	int len = kmb::Element::getBoundaryCount( etype );
	kmb::Face* bfaces = new kmb::Face[len];
	kmb::SurfaceMatching::rotatedElement* matchingElements = new kmb::SurfaceMatching::rotatedElement[len];
	for(int i=0;i<len;++i){
		bfaces[i].setId(kmb::Element::nullElementId,kmb::nullId);
		matchingElements[i].elementId = kmb::Element::nullElementId;
		matchingElements[i].index = -1;
		neighborInfo->getFaceNeighborByIndex(f,i,slaveElements,bfaces[i]);

		std::map< kmb::Face, kmb::SurfaceMatching::rotatedElement >::iterator mIter = mapping.find(bfaces[i]);
		if( mIter != mapping.end() ){
			matchingElements[i] = mIter->second;
		}
		REVOCAP_Debug(" [%d %d] => %d %d\n",
			bfaces[i].getElementId(), bfaces[i].getLocalFaceId(),
			matchingElements[i].elementId, matchingElements[i].index );
	}
	REVOCAP_Debug("appendDummyElement 3\n");




	if( etype == kmb::TRIANGLE ){
	}else if( etype == kmb::QUAD ){
		REVOCAP_Debug("appendDummyElement 4\n");
		kmb::nodeIdType nodes[4] = { kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId };
		kmb::Quad q0,q1;
		f.getFaceElement( slaveElements, q0 );
		for(int i=0;i<4;++i){
			if( bfaces[i].getElementId() != kmb::Element::nullElementId &&
				bfaces[i].getFaceElementType( slaveElements ) == kmb::QUAD &&
				matchingElements[i].elementId != kmb::Element::nullElementId )
			{
				kmb::ElementContainer::iterator elem = masterSurf->find( matchingElements[i].elementId );
				bfaces[i].getFaceElement( slaveElements, q1 );
				int i0=-1, i1=-1;
				if( kmb::ElementRelation::getQuadRelation(
						q0.getCellId(0), q0.getCellId(1), q0.getCellId(2), q0.getCellId(3),
						q1.getCellId(0), q1.getCellId(1), q1.getCellId(2), q1.getCellId(3),
						i0, i1 ) == kmb::ElementRelation::ADJACENT && i0 == i )
				{

					nodes[i] = elem.getCellId( (i1 + 1 + matchingElements[i].index)%4 );
					nodes[(i+1)%4] = elem.getCellId( (i1 + matchingElements[i].index)%4 );
				}
			}
		}
		REVOCAP_Debug("appendDummyElement [%d %d %d %d]\n", nodes[0], nodes[1], nodes[2], nodes[3] );
		if( nodes[0] != kmb::nullNodeId && nodes[1] != kmb::nullNodeId &&
			nodes[2] != kmb::nullNodeId && nodes[3] != kmb::nullNodeId )
		{
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] == kmb::nullNodeId && nodes[1] == kmb::nullNodeId &&
				 nodes[2] != kmb::nullNodeId && nodes[3] != kmb::nullNodeId )
		{
			nodes[0] = duplicateNode( nodes[3] );
			nodes[1] = duplicateNode( nodes[2] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] != kmb::nullNodeId && nodes[1] == kmb::nullNodeId &&
				 nodes[2] == kmb::nullNodeId && nodes[3] != kmb::nullNodeId )
		{
			nodes[1] = duplicateNode( nodes[0] );
			nodes[2] = duplicateNode( nodes[3] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] != kmb::nullNodeId && nodes[1] != kmb::nullNodeId &&
				 nodes[2] == kmb::nullNodeId && nodes[3] == kmb::nullNodeId )
		{
			nodes[2] = duplicateNode( nodes[1] );
			nodes[3] = duplicateNode( nodes[0] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] == kmb::nullNodeId && nodes[1] != kmb::nullNodeId &&
				 nodes[2] != kmb::nullNodeId && nodes[3] == kmb::nullNodeId )
		{
			nodes[3] = duplicateNode( nodes[2] );
			nodes[0] = duplicateNode( nodes[1] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] == kmb::nullNodeId && nodes[1] != kmb::nullNodeId &&
				 nodes[2] != kmb::nullNodeId && nodes[3] != kmb::nullNodeId )
		{
			nodes[0] = duplicateNode( nodes[3] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] != kmb::nullNodeId && nodes[1] == kmb::nullNodeId &&
				 nodes[2] != kmb::nullNodeId && nodes[3] != kmb::nullNodeId )
		{
			nodes[1] = duplicateNode( nodes[0] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] != kmb::nullNodeId && nodes[1] != kmb::nullNodeId &&
				 nodes[2] == kmb::nullNodeId && nodes[3] != kmb::nullNodeId )
		{
			nodes[2] = duplicateNode( nodes[1] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		else if( nodes[0] != kmb::nullNodeId && nodes[1] != kmb::nullNodeId &&
				 nodes[2] != kmb::nullNodeId && nodes[3] == kmb::nullNodeId )
		{
			nodes[3] = duplicateNode( nodes[2] );
			appendId = mesh->insertElement( masterId, kmb::QUAD, nodes );
		}
		REVOCAP_Debug("appendDummyElement 6 %d [%d %d %d %d]\n", appendId, nodes[0], nodes[1], nodes[2], nodes[3] );
	}
	delete[] matchingElements;
	delete[] bfaces;
	REVOCAP_Debug("appendDummyElement 7 %d\n", appendId );

	if( appendId != kmb::Element::nullElementId ){
		kmb::SurfaceMatching::rotatedElement re = {appendId,0};
		mapping.insert( std::pair< kmb::Face, kmb::SurfaceMatching::rotatedElement >( f, re ) );
	}
	return appendId;
}

kmb::nodeIdType
kmb::SurfaceMatching::duplicateNode(kmb::nodeIdType n0)
{
	if( this->mesh == NULL ){
		return kmb::nullNodeId;
	}

	kmb::Node point;
	kmb::nodeIdType n1;
	mesh->getNode( n0, point );
	n1 = mesh->insertNode( point.x(), point.y(), point.z() );
	if( this->insertedNodes != NULL ){
		insertedNodes->addId( n1 );
	}
	return n1;
}

