/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : SurfaceMatching                                         #
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
#include "MeshDB/kmbSurfaceMatching.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbCollision.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbNodeNeighborFaceInfo.h"
#include "MeshDB/kmbElementRelation.h"
#include "Matrix/kmbMatrix_DoubleArray.h"
#include "Common/kmbCalculator.h"
#include "Common/kmbPermutation.h"

kmb::SurfaceMatching::SurfaceMatching(void)
: mesh(NULL)
, masterId(kmb::Body::nullBodyId)
, masterSurf(NULL)
, slaveFaceGroup(NULL)
, slaveElements(NULL)
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
}

void
kmb::SurfaceMatching::clear(void)
{
	clearPairInfo();
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
kmb::SurfaceMatching::clearPairInfo(void)
{
	connectionTable.clear();
	mapping.clear();
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
	if( neighborInfo != NULL ){
		neighborInfo->clear();
	}
}

void
kmb::SurfaceMatching::setMesh(kmb::MeshData* m,const char* insNodes)
{
	clear();
	if( m != NULL ){
		this->mesh = m;
		this->collision = new kmb::Collision(mesh);
		this->neighborInfo = new kmb::NodeNeighborFaceInfo();
		if( insNodes == NULL ){
			this->insertedNodes = NULL;
		}else{
			this->insertedNodes = this->mesh->getDataBindingsPtr( insNodes );
			if( this->insertedNodes == NULL ){
				this->insertedNodes = mesh->createDataBindings( insNodes, kmb::DataBindings::NodeGroup, kmb::PhysicalValue::None );
			}
		}
	}
}

void
kmb::SurfaceMatching::setPair(kmb::bodyIdType bodyId,const char* faceGroup)
{
	if( this->mesh != NULL && this->collision != NULL && this->neighborInfo != NULL ){
		clearPairInfo();
		masterId = bodyId;
		masterSurf = mesh->getBodyPtr(bodyId);
		if( masterSurf == NULL || masterSurf->getDimension() != 2 ){
			masterSurf = NULL;
			return;
		}
		slaveName = faceGroup;
		slaveFaceGroup = mesh->getDataBindingsPtr(faceGroup);
		if( slaveFaceGroup == NULL || slaveFaceGroup->getBindingMode() != kmb::DataBindings::FaceGroup ){
			masterSurf = NULL;
			slaveFaceGroup = NULL;
			return;
		}
		slaveElements = mesh->getBodyPtr( slaveFaceGroup->getTargetBodyId() );
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

		int i0,i1;
		for(int j0=0;j0<masterLen;++j0){
			kmb::ElementContainer::iterator m0Iter = masterSurf->find(elementIds[j0]);
			for(int j1=0;j1<masterLen;++j1){
				kmb::ElementContainer::iterator m1Iter = masterSurf->find(elementIds[j1]);
				if( kmb::ElementRelation::getRelation(m0Iter,i0,m1Iter,i1) == kmb::ElementRelation::ADJACENT ){
					connectionTable.insert( std::pair<int,int>(j0,j1) );
				}
			}
		}







		calcMapping();
	}
}



bool
kmb::SurfaceMatching::calcMapping(void)
{
	if( slaveElements == NULL || slaveFaceGroup == NULL ){
		return false;
	}
	if( distanceMatrix == NULL || mesh == NULL || elementIds == NULL || faces == NULL ){
		return false;
	}


	int rSize = distanceMatrix->getRowSize();
	int cSize = distanceMatrix->getColSize();
	if( cSize < rSize ){
		return false;
	}

	kmb::Minimizer min;
	kmb::Permutation perm;

	perm.initialize( cSize, rSize );

	int* minPerm = new int[rSize];
	for(int i=0;i<rSize;++i){
		minPerm[i] = -1;
	}

	while( !perm.isFinished() ){
		int check = checkTopologicalMapping(perm);
		if( check == -1 ){
			double sumDist = 0.0;
			for(int i=0;i<rSize;++i){
				sumDist += distanceMatrix->get(i,perm.getPerm(i));
				if( sumDist > min.getMin() ){
					perm.nextPerm(i);
					goto perm_iterate;
				}
			}
			if( min.update( sumDist ) && checkTopologicalMapping(perm) ){

				for(int i=0;i<rSize;++i){
					minPerm[i] = perm.getPerm(i);
				}
			}
		}else if( check >= 0 ){
			perm.nextPerm(check);
			goto perm_iterate;
		}else{
			break;
		}
		++perm;
		perm_iterate:
		;
	}

	if( minPerm[0] >= 0 ){

		for(int i=0;i<rSize;++i){
			int index = -1;
			kmb::ElementContainer::iterator eIter = masterSurf->find( elementIds[i] );
			collision->distanceByNode( eIter, faces[minPerm[i]], slaveElements, index );
			kmb::SurfaceMatching::rotatedElement re = {elementIds[i],index};
			mapping.insert( std::pair< kmb::Face, kmb::SurfaceMatching::rotatedElement >( faces[minPerm[i]], re ) );
		}
	}
	delete[] minPerm;
	return true;
}


int
kmb::SurfaceMatching::checkTopologicalMapping( kmb::Permutation& perm ) const
{
	if( slaveElements == NULL || faces == NULL ){
		return -2;
	}
	int i0,i1;
	std::set< std::pair<int,int> >::const_iterator cIter = connectionTable.begin();
	while( cIter != connectionTable.end() ){
		int j0 = perm.getPerm(cIter->first);
		int j1 = perm.getPerm(cIter->second);
		kmb::Face f0 = faces[j0];
		kmb::Face f1 = faces[j1];
		kmb::Element* e0 = f0.createElement( slaveElements );
		kmb::Element* e1 = f1.createElement( slaveElements );
		kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation(*e0,i0,*e1,i1);
		delete e0;
		delete e1;
		if( rel != kmb::ElementRelation::ADJACENT ){
			return (cIter->first < cIter->second) ? cIter->second : cIter->first;
		}
		++cIter;
	}
	return -1;
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
	std::map< kmb::Face, kmb::SurfaceMatching::rotatedElement >::const_iterator mIter = mapping.find(f);
	if( mIter == mapping.end() ){

		kmb::elementIdType elementId = appendDummyElement(f);
		if( elementId != kmb::Element::nullElementId ){
			index = 0;
			return elementId;
		}
		return kmb::Element::nullElementId;
	}else{
		index = mIter->second.index;
		return mIter->second.elementId;
	}
}



kmb::elementIdType
kmb::SurfaceMatching::appendDummyElement(kmb::Face f)
{
	if( this->mesh == NULL || this->neighborInfo == NULL ||
		this->masterSurf == NULL || this->slaveFaceGroup == NULL ||
		f.getElementId() == kmb::Element::nullElementId )
	{
		return kmb::Element::nullElementId;
	}
	kmb::ElementContainer* slaveElements = mesh->getBodyPtr( slaveFaceGroup->getTargetBodyId() );
	if( slaveElements == NULL ){
		return kmb::Element::nullElementId;
	}
	if( !slaveFaceGroup->hasId(f) ){
		return kmb::Element::nullElementId;
	}

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
	}




	if( etype == kmb::TRIANGLE ){
	}else if( etype == kmb::QUAD ){
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
	}
	delete[] matchingElements;
	delete[] bfaces;

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

