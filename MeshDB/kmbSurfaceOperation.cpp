/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : SurfaceOperation                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbSurfaceOperation.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbElementContainerOpenGLDraw.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbNodeNeighborFaceInfo.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "Common/kmbClassification.h"

#include <cmath>

kmb::SurfaceOperation::SurfaceOperation(void)
: mesh(NULL)
, elements(NULL)
, neighborInfo(NULL)
, dividedElementsInfo()
, edgesBetweenElements()
, dividedFacesInfo()
, edgesBetweenFaces()
, permitReverse(true)
{
}

kmb::SurfaceOperation::~SurfaceOperation(void)
{
	clear();
}

void
kmb::SurfaceOperation::clear(void)
{
	this->mesh = NULL;
	this->elements = NULL;
	if( this->neighborInfo != NULL ){
		delete this->neighborInfo;
		this->neighborInfo = NULL;
	}
	this->dividedElementsInfo.clear();
	this->dividedFacesInfo.clear();
}

void
kmb::SurfaceOperation::setMesh(kmb::MeshData* mesh)
{
	clear();
	this->mesh = mesh;
}

void
kmb::SurfaceOperation::setPermitReverse(bool f)
{
	this->permitReverse = f;
}

void
kmb::SurfaceOperation::setSurface(kmb::MeshData* mesh,kmb::bodyIdType bodyId)
{
	this->mesh = mesh;
	if( this->mesh ){
		this->elements = this->mesh->getBodyPtr(bodyId);
		if( elements->isUniqueDim(2) ){
			this->neighborInfo = new kmb::NodeNeighborInfo();
			this->neighborInfo->appendCoboundary( this->elements );
		}else{
			this->elements = NULL;
		}
	}
}

void
kmb::SurfaceOperation::divideSurface(bodyIdType bodyID,double angle,std::vector<kmb::bodyIdType> &surfIDs)
{
	if( mesh == NULL ) return;

	kmb::ElementContainer* body = mesh->getBodyPtr(bodyID);


	size_t surfNum = divideSurface( body, mesh->getNodes(), angle );




	kmb::elementIdType elementId = kmb::Element::nullElementId;
	this->dividedElementsInfo.first( elementId );
	for(size_t i=0;i<surfNum;++i){

		size_t triCount = 0;
		size_t quadCount = 0;
		this->getDividedSurfaceElementCount( body, elementId, triCount, quadCount );
		kmb::ElementContainerOpenGLDraw* surface = new kmb::ElementContainerOpenGLDraw(triCount,quadCount);

		this->getDividedSurface( body, elementId, surface );
		surface->calcNormals( mesh->getNodes() );
		surface->updateBoundingBox( mesh->getNodes() );
		surfIDs.push_back( mesh->appendBody( surface, false ) );
		this->dividedElementsInfo.next( elementId );
	}

	kmb::ElementContainerMap* ridges = new kmb::ElementContainerMap();
	this->getTotalRidge( body, ridges );
	mesh->replaceBody(bodyID,ridges);
	ridges->updateBoundingBox( mesh->getNodes() );
}

void
kmb::SurfaceOperation::divideSurfaceWithRidge(bodyIdType bodyId,double angle,std::map<kmb::bodyIdType,kmb::bodyIdType> &surfIDs)
{
	if( mesh == NULL )	return;

	kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);


	size_t surfNum = divideSurface( body, mesh->getNodes(), angle );


	kmb::elementIdType elementId = kmb::Element::nullElementId;
	this->dividedElementsInfo.first( elementId );
	for(size_t i=0;i<surfNum;++i){
		size_t triCount = 0;
		size_t quadCount = 0;
		this->getDividedSurfaceElementCount( body, elementId, triCount, quadCount );
		kmb::ElementContainerOpenGLDraw* surface = new kmb::ElementContainerOpenGLDraw(triCount,quadCount);
		kmb::ElementContainerMap* ridge = new kmb::ElementContainerMap();
		this->getDividedSurface( body, elementId, surface );
		this->getSurfaceRidge( body, elementId, ridge );
		if( surface->getCount() > 0 || ridge->getCount() > 0 ){
			surface->calcNormals( mesh->getNodes() );
			surface->updateBoundingBox( mesh->getNodes() );
			ridge->updateBoundingBox( mesh->getNodes() );

			kmb::bodyIdType surfId = mesh->appendBody( surface, false );
			kmb::bodyIdType ridgeId = mesh->appendBody( ridge );
			surfIDs.insert( std::pair<kmb::bodyIdType,kmb::bodyIdType>(surfId,ridgeId) );
		}else{
			delete surface;
			delete ridge;
		}
		this->dividedElementsInfo.next( elementId );
	}

	kmb::ElementContainerMap* totalRidge = new kmb::ElementContainerMap();
	this->getTotalRidge( body, totalRidge );
	mesh->replaceBody(bodyId,totalRidge);
	totalRidge->updateBoundingBox( mesh->getNodes() );
}

kmb::bodyIdType
kmb::SurfaceOperation::divideFaceGroup(std::string name,double angle,std::vector<std::string> &subFaceGroups)
{
	if( mesh == NULL )
		return kmb::Body::nullBodyId;

	kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
	if( data == NULL )
		return kmb::Body::nullBodyId;

	kmb::ElementContainer* elements = mesh->getBodyPtr( data->getTargetBodyId() );
	if( elements == NULL )
		return kmb::Body::nullBodyId;

	size_t dividedNum = divideFaceGroup( data, elements, mesh->getNodes(), angle );

	kmb::Face face;
	this->dividedFacesInfo.first( face );
	for(size_t i=0;i<dividedNum;++i){
		std::string subfaceName = mesh->getUniqueDataName(name, static_cast<int>(i) );
		kmb::DataBindings* fdata =
			mesh->createDataBindings( subfaceName.c_str(), kmb::DataBindings::FaceGroup, kmb::PhysicalValue::None,
			"Brep", data->getTargetBodyId() );
		this->getDividedFaceGroup(face,fdata);
		this->dividedFacesInfo.next( face );
		subFaceGroups.push_back( subfaceName );
	}

	kmb::ElementContainerMap* ridges = new kmb::ElementContainerMap();
	this->getTotalRidge( elements, ridges );
	kmb::bodyIdType ridgeId = mesh->appendBody( ridges );
	return ridgeId;
}

kmb::bodyIdType
kmb::SurfaceOperation::divideFaceGroupWithRidge(std::string name,double angle,std::map<std::string,kmb::bodyIdType> &subFaceGroupMapper)
{
	if( mesh == NULL )
		return kmb::Body::nullBodyId;

	kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
	if( data == NULL )
		return kmb::Body::nullBodyId;

	kmb::ElementContainer* elements = mesh->getBodyPtr( data->getTargetBodyId() );
	if( elements == NULL )
		return kmb::Body::nullBodyId;

	size_t dividedNum = divideFaceGroup( data, elements, mesh->getNodes(), angle );

	kmb::Face face;
	this->dividedFacesInfo.first( face );
	for(size_t i=0;i<dividedNum;++i){
		std::string subfaceName = mesh->getUniqueDataName(name, static_cast<int>(i) );
		kmb::DataBindings* fdata =
			mesh->createDataBindings( subfaceName.c_str(), kmb::DataBindings::FaceGroup, kmb::PhysicalValue::None,
			"Brep", data->getTargetBodyId() );
		kmb::ElementContainerMap* ridge = new kmb::ElementContainerMap();
		this->getDividedFaceGroup(face,fdata);
		this->getSurfaceRidge(elements,face,ridge);
		ridge->updateBoundingBox( mesh->getNodes() );
		kmb::bodyIdType ridgeId = mesh->appendBody( ridge );
		subFaceGroupMapper.insert( std::pair<std::string,kmb::bodyIdType>(subfaceName,ridgeId) );
		this->dividedFacesInfo.next( face );
	}

	kmb::ElementContainerMap* totalRidge = new kmb::ElementContainerMap();
	this->getTotalRidge( elements, totalRidge );
	totalRidge->updateBoundingBox( mesh->getNodes() );
	kmb::bodyIdType ridgeId = mesh->appendBody( totalRidge );
	return ridgeId;
}

void
kmb::SurfaceOperation::decomposeSurface(kmb::bodyIdType bodyId, std::set< kmb::bodyIdType > &components)
{
	if( mesh == NULL ) return;
	kmb::ElementContainer* body = mesh->getBodyPtr( bodyId );
	if( body == NULL ) return;
	std::set< kmb::ElementContainer* > compBodies;
	this->decomposeByElementContainer( body, compBodies );
	std::set< kmb::ElementContainer* >::iterator bIter = compBodies.begin();
	while( bIter != compBodies.end() ){
		kmb::ElementContainer* elements = (*bIter);
		components.insert( mesh->appendBody( elements, false ) );
		++bIter;
	}

}

void
kmb::SurfaceOperation::decomposeByElementContainer(kmb::ElementContainer* elements,std::set< kmb::ElementContainer* > &components)
{
	if( elements == NULL || elements->getCount() == 0 )	return;


	kmb::NodeNeighborInfo neighborInfo;
	neighborInfo.appendCoboundary( elements );

	kmb::elementIdType neighbors[6] = {
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId,
		kmb::Element::nullElementId };


	kmb::Classification<kmb::elementIdType> classification;

	kmb::ElementContainer::iterator eIter = elements->begin();
	while( eIter != elements->end() )
	{
		const kmb::elementIdType id = eIter.getId();
		classification.add( id );
		const int neiNum = neighborInfo.getNeighborElements( id, neighbors, elements);
		++eIter;
		for(int i=0;i<neiNum;++i)
		{
			if( neighbors[i] != kmb::Element::nullElementId )
			{
				classification.equivalent( id, neighbors[i] );
			}
		}
	}

	size_t compNum = classification.getClassCount();
	kmb::elementIdType firstElemId = kmb::Element::nullElementId;
	classification.first( firstElemId );
	for(size_t i=0;i<compNum;++i){
		kmb::ElementContainer* container = NULL;
		container = new kmb::ElementContainerMap();
		kmb::Classification< kmb::elementIdType >::iterator eIter = classification.begin( firstElemId );
		kmb::Classification< kmb::elementIdType >::iterator eEnd = classification.end( firstElemId );
		while( eIter != eEnd ){
			kmb::elementIdType elemId = eIter.getValue();
			container->addElement( elements->eraseElement( elemId ), elemId );
			++eIter;
		}
		components.insert( container );
		classification.next( firstElemId );
	}
}


bool
kmb::SurfaceOperation::decomposeFaceGroup(std::string faceGroup, std::vector<std::string> &subFaceGroups)
{
	kmb::DataBindings* data = NULL;
	kmb::ElementContainer* elements = NULL;
	if( mesh == NULL ||
		(data=mesh->getDataBindingsPtr( faceGroup.c_str() )) == NULL ||
		(elements=mesh->getBodyPtr( data->getTargetBodyId() )) == NULL )
	{
		return false;
	}

	size_t dividedNum = divideComponentsFaceGroup( data, elements );

	kmb::Face face;
	this->dividedFacesInfo.first( face );
	for(size_t i=0;i<dividedNum;++i){
		std::string subfaceName = mesh->getUniqueDataName(faceGroup, static_cast<int>(i) );
		kmb::DataBindings* fdata =
			mesh->createDataBindings( subfaceName.c_str(), kmb::DataBindings::FaceGroup, kmb::PhysicalValue::None,
			data->getSpecType().c_str(), data->getTargetBodyId() );
		this->getDividedFaceGroup(face,fdata);
		this->dividedFacesInfo.next( face );
		subFaceGroups.push_back( subfaceName );
	}
	return true;
}

kmb::SurfaceOperation::vertexType
kmb::SurfaceOperation::getVertexType(kmb::nodeIdType nodeId, kmb::MeshData* mesh, kmb::ElementContainer* elements, kmb::NodeNeighborInfo* neighborInfo)
{
	if( mesh==NULL || elements==NULL || neighborInfo==NULL ){
		return kmb::SurfaceOperation::Unknown;
	}
	kmb::ElementEvaluator evaluator( mesh->getNodes() );
	double angleSum = 0.0;

	kmb::NodeNeighbor::iterator eIter = neighborInfo->beginIteratorAt(nodeId);
	kmb::NodeNeighbor::iterator end = neighborInfo->endIteratorAt(nodeId);
	if( eIter != end ){
		kmb::elementIdType elementId = eIter->second;
		kmb::elementIdType nextElementId =
			neighborInfo->getNextElementAroundNode( elements, nodeId, elementId );
		kmb::ElementContainer::iterator element = elements->find( elementId );
		kmb::ElementContainer::iterator nextElement = elements->find( nextElementId );
		while( element != NULL && nextElement != NULL ){
			angleSum += evaluator.getAngle( element, nodeId );
			elementId = nextElementId;
			if( elementId == eIter->second ){
				break;
			}
			element = nextElement;
			nextElementId =
				neighborInfo->getNextElementAroundNode( elements, nodeId, elementId );
			nextElement = elements->find( nextElementId );
		}
		if( angleSum > 2*PI ){
			return kmb::SurfaceOperation::HYPERBOLIC;
		}else if( angleSum < 2*PI ){
			return kmb::SurfaceOperation::CONVEX;
		}else{
			return kmb::SurfaceOperation::FLAT;
		}
	}else{
		return kmb::SurfaceOperation::Unknown;
	}
}

kmb::SurfaceOperation::vertexType
kmb::SurfaceOperation::getVertexType(kmb::nodeIdType nodeId) const
{
	return kmb::SurfaceOperation::getVertexType
		(nodeId,this->mesh,this->elements,this->neighborInfo);
}

kmb::bodyIdType
kmb::SurfaceOperation::faceGroup2Surface(const char* name)
{
	kmb::DataBindings* data = NULL;
	kmb::Body* parentBody = NULL;
	if( mesh == NULL
		|| (data = mesh->getDataBindingsPtr(name)) == NULL
		|| data->getBindingMode() != kmb::DataBindings::FaceGroup
		|| (parentBody = mesh->getBodyPtr( data->getTargetBodyId() )) == NULL
		)
	{
		return kmb::Body::nullBodyId;
	}
	kmb::DataBindings::iterator dIter = data->begin();
	kmb::DataBindings::iterator dEnd = data->end();
	kmb::ElementContainerMap* body = new kmb::ElementContainerMap();
	Face f;
	while( dIter != dEnd ){
		if( dIter.getFace( f ) ){
			kmb::Element* element = f.createElement( parentBody );
			body->addElement( element );
		}
		++dIter;
	}
	return mesh->appendBody( body );
}

bool
kmb::SurfaceOperation::surface2FaceGroup(kmb::bodyIdType bodyId,kmb::bodyIdType parentBodyId,const char* name)
{
	kmb::Body* parentBody = NULL;
	kmb::Body* body = NULL;
	if( mesh == NULL
		|| ((body=mesh->getBodyPtr(bodyId)) == NULL)
		|| ((parentBody=mesh->getBodyPtr(parentBodyId)) == NULL)
		){
		return false;
	}

	if( !(body->isUniqueDim(2) && parentBody->isUniqueDim(3))
		&& !(body->isUniqueDim(1) && parentBody->isUniqueDim(2))
		)
	{
		return false;
	}

	if( mesh->hasData(name) ){
		return false;
	}
	kmb::DataBindingsGroup<kmb::Face>* faceGroup
		= reinterpret_cast< kmb::DataBindingsGroup<kmb::Face>* >(
			mesh->createDataBindings
				(name,
				 kmb::DataBindings::FaceGroup,
				 kmb::PhysicalValue::None,
				 "", parentBodyId ) );
	int index = -1;
	int otherindex = -1;
	kmb::NodeNeighborInfo neighborInfo;
	neighborInfo.appendCoboundary( parentBody );
	kmb::ElementContainer::iterator eIter = body->begin();
	kmb::ElementContainer::iterator eEnd = body->end();
	while( eIter != eEnd )
	{
		kmb::nodeIdType n0 = eIter[0];
		kmb::NodeNeighbor::iterator neiIter = neighborInfo.beginIteratorAt(n0);
		kmb::NodeNeighbor::iterator neiEnd = neighborInfo.endIteratorAt(n0);
		while( neiIter != neiEnd ){
			kmb::ElementContainer::iterator parentElement = parentBody->find( neiIter->second );
			if( !parentElement.isFinished() ){
				kmb::ElementRelation::relationType rel =
					kmb::ElementRelation::getRelation( parentElement, index, eIter, otherindex );
				if( rel == kmb::ElementRelation::COBOUNDARY || (rel == kmb::ElementRelation::ANTICOBOUNDARY && permitReverse) ){
					faceGroup->addId( kmb::Face(neiIter->second,index) );
					goto findFace;
				}
			}
			++neiIter;
		}
findFace:
		++eIter;
	}
	return true;
}

bool
kmb::SurfaceOperation::surface2FaceGroup(kmb::bodyIdType bodyId,const char* parentFaceGroup,const char* name)
{
	if( mesh == NULL ){
		return false;
	}
	kmb::DataBindings* parentFg = mesh->getDataBindingsPtr( parentFaceGroup );
	if( parentFg == NULL ||
		parentFg->getBindingMode() != kmb::DataBindings::FaceGroup )
	{
		return false;
	}
	kmb::bodyIdType parentBodyId = parentFg->getTargetBodyId();
	kmb::Body* parentBody = NULL;
	kmb::Body* body = NULL;
	if( mesh == NULL
		|| ((body=mesh->getBodyPtr(bodyId)) == NULL)
		|| ((parentBody=mesh->getBodyPtr(parentBodyId)) == NULL)
		){
		return false;
	}

	if( !(body->isUniqueDim(2) && parentBody->isUniqueDim(3))
		&& !(body->isUniqueDim(1) && parentBody->isUniqueDim(2))
		)
	{
		return false;
	}

	if( mesh->hasData(name) ){
		return false;
	}
	kmb::DataBindingsGroup<kmb::Face>* faceGroup
		= reinterpret_cast< kmb::DataBindingsGroup<kmb::Face>* >(
			mesh->createDataBindings
				(name,
				 kmb::DataBindings::FaceGroup,
				 kmb::PhysicalValue::None,
				 "", parentBodyId ) );
	int index = -1;
	int otherindex = -1;
	kmb::NodeNeighborFaceInfo neighborInfo;
	neighborInfo.appendCoboundary( parentFg, parentBody );
	kmb::ElementContainer::iterator eIter = body->begin();
	kmb::ElementContainer::iterator eEnd = body->end();
	while( eIter != eEnd )
	{
		kmb::nodeIdType n0 = eIter.getCellId(0);
		kmb::NodeNeighborFace::iterator neiIter = neighborInfo.beginIteratorAt(n0);
		kmb::NodeNeighborFace::iterator neiEnd = neighborInfo.endIteratorAt(n0);
		while( neiIter != neiEnd ){
			kmb::Face f = neiIter->second;
			kmb::Element* e = f.createElement( parentBody );
			if( e ){
				kmb::ElementRelation::relationType rel =
					kmb::ElementRelation::getRelation( *e, index, eIter, otherindex );
				if( rel == kmb::ElementRelation::EQUAL || (rel == kmb::ElementRelation::REVERSE && permitReverse) ){
					faceGroup->addId( f );
					goto findFace;
				}
			}
			++neiIter;
		}
findFace:
		++eIter;
	}
	return true;
}



size_t
kmb::SurfaceOperation::divideSurface(const kmb::ElementContainer* elements,const kmb::Point3DContainer* points,double angle)
{
	if( elements != NULL && elements->isUniqueDim(2) && points != NULL ){

		kmb::ElementEvaluator evaluator( points );
		double cosThres = cos(angle*PI/180.0);

		kmb::NodeNeighborInfo nodeNeighborInfo;
		nodeNeighborInfo.appendCoboundary( elements );

		kmb::elementIdType faceInfo[4];
		kmb::Vector3D eNormal;
		kmb::ElementContainer::const_iterator eIter = elements->begin();
		while( !eIter.isFinished() )
		{
			evaluator.getNormalVector( eIter, eNormal );
			kmb::elementIdType id = eIter.getId();
			this->dividedElementsInfo.add(id);

			int faceNum = nodeNeighborInfo.getNeighborElements(id,faceInfo,elements);
			for(int i=0;i<faceNum;++i){
				kmb::elementIdType neiId = faceInfo[i];
				if( neiId != kmb::Element::nullElementId ){
					kmb::ElementContainer::const_iterator nei = elements->find(neiId);
					kmb::Vector3D neiNormal;
					evaluator.getNormalVector( nei, neiNormal );
					double innerPro = eNormal*neiNormal;
					if( -innerPro < cosThres ){

						this->dividedElementsInfo.equivalent(id,neiId);
					}else{

						std::pair<kmb::elementIdType,kmb::elementIdType> edge;
						if(id < neiId){
							edge.first = id;
							edge.second = neiId;
						}else{
							edge.first = neiId;
							edge.second = id;
						}
						this->edgesBetweenElements.insert(edge);
					}
				}
			}
			++eIter;
		}
	}
	return this->dividedElementsInfo.getClassCount();
}

bool
kmb::SurfaceOperation::getDividedSurfaceElementCount(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,size_t &triCount,size_t &quadCount) const
{
	if( orgSurf != NULL ){
		triCount = 0;
		quadCount = 0;
		kmb::Classification<kmb::elementIdType>::iterator cIter = this->dividedElementsInfo.begin( initElementId );
		kmb::Classification<kmb::elementIdType>::iterator cEnd = this->dividedElementsInfo.end( initElementId );
		while( cIter != cEnd ){
			kmb::elementIdType elementId = cIter.getValue();
			kmb::ElementContainer::const_iterator eIter = orgSurf->find( elementId );
			if( !eIter.isFinished() ){
				switch( eIter.getType() )
				{
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					++triCount;
					break;
				case kmb::QUAD:
				case kmb::QUAD2:
					++quadCount;
					break;
				default:
					break;
				}
			}
			++cIter;
		}
	}
	return false;
}



size_t
kmb::SurfaceOperation::getDividedSurface(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,kmb::ElementContainer* dividedSurf) const
{
	size_t count = 0;
	if( orgSurf != NULL && dividedSurf != NULL ){
		kmb::nodeIdType* nodes = NULL;
		nodes = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
		kmb::elementType etype = kmb::UNKNOWNTYPE;
		kmb::Classification<kmb::elementIdType>::iterator cIter = this->dividedElementsInfo.begin( initElementId );
		kmb::Classification<kmb::elementIdType>::iterator cEnd = this->dividedElementsInfo.end( initElementId );
		while( cIter != cEnd ){
			kmb::elementIdType elementId = cIter.getValue();
			kmb::ElementContainer::const_iterator eIter = orgSurf->find( elementId );
			if( !eIter.isFinished() ){
				eIter.getElement( etype, nodes );
				dividedSurf->addElement( etype, nodes, eIter.getId() );
				++count;
			}
			++cIter;
		}
		delete[] nodes;
	}
	return count;
}

size_t
kmb::SurfaceOperation::getTotalRidge(const kmb::ElementContainer* org,kmb::ElementContainer* ridges) const
{
	size_t count = 0;

	if( org != NULL && org->isUniqueDim(2) && ridges != NULL ){
		kmb::nodeIdType cell[2];
		std::set< std::pair<kmb::elementIdType, kmb::elementIdType> >::const_iterator
			eIter = edgesBetweenElements.begin();
		while( eIter != edgesBetweenElements.end() ){

			if( !dividedElementsInfo.isEquivalent( eIter->first, eIter->second ) ){
				kmb::ElementContainer::const_iterator elem0 = org->find( eIter->first );
				kmb::ElementContainer::const_iterator elem1 = org->find( eIter->second );
				kmb::elementType etype = kmb::ElementRelation::common( elem0, elem1, cell );
				if( etype == kmb::SEGMENT ){
					++count;
					ridges->addElement(etype,cell);
				}
			}
			++eIter;
		}
	}

	else if( org != NULL && org->isUniqueDim(3) && ridges != NULL ){
		kmb::nodeIdType cell[2];
		std::set< std::pair<kmb::Face, kmb::Face> >::const_iterator
			fIter = edgesBetweenFaces.begin();
		while( fIter != edgesBetweenFaces.end() ){

			if( !dividedFacesInfo.isEquivalent( fIter->first, fIter->second ) ){
				kmb::elementType etype = kmb::ElementRelation::common( fIter->first, fIter->second, org, cell );
				if( etype == kmb::SEGMENT ){
					++count;
					ridges->addElement(etype,cell);
				}
			}
			++fIter;
		}
	}
	return count;
}

size_t
kmb::SurfaceOperation::getSurfaceRidge(const kmb::ElementContainer* elements,kmb::Face initFace,kmb::ElementContainer* ridges) const
{
	size_t count = 0;
	if( elements != NULL && elements->isUniqueDim(3) && ridges != NULL ){
		kmb::nodeIdType cell[2];
		std::set< std::pair<kmb::Face, kmb::Face> >::const_iterator
			fIter = edgesBetweenFaces.begin();
		while( fIter != edgesBetweenFaces.end() ){


			if( !dividedFacesInfo.isEquivalent( fIter->first, fIter->second ) )
			{
				if( dividedFacesInfo.isEquivalent( fIter->first, initFace ) ){
					kmb::elementType etype = kmb::ElementRelation::common( fIter->first, fIter->second, elements,  cell );
					if( etype == kmb::SEGMENT ){
						++count;
						ridges->addElement(etype,cell);
					}
				}
				else if( dividedFacesInfo.isEquivalent( fIter->second, initFace ) ){

					kmb::elementType etype = kmb::ElementRelation::common( fIter->second, fIter->first, elements, cell );
					if( etype == kmb::SEGMENT ){
						++count;
						ridges->addElement(etype,cell);
					}
				}
			}
			++fIter;
		}
	}
	return count;
}

size_t
kmb::SurfaceOperation::divideFaceGroup(const kmb::DataBindings* data, const kmb::ElementContainer* elements, const kmb::Point3DContainer* points, double angle)
{
	if( data != NULL && data->getBindingMode() == kmb::DataBindings::FaceGroup && elements != NULL ){

		kmb::ElementEvaluator evaluator( points );
		double cosThres = cos(angle*PI/180.0);

		kmb::NodeNeighborFaceInfo nodeNeighborInfo;
		nodeNeighborInfo.appendCoboundary( data, elements );

		kmb::Face faceInfo[4];
		kmb::Face f;
		kmb::Vector3D normal;
		kmb::Vector3D neiNormal;
		kmb::DataBindings::const_iterator dIter = data->begin();
		while( !dIter.isFinished() )
		{
			dIter.getFace(f);
			evaluator.getNormalVector( f, elements, normal );
			this->dividedFacesInfo.add(f);
			int faceNum = nodeNeighborInfo.getFaceNeighbor(f,faceInfo,elements);
			for(int i=0;i<faceNum;++i){
				evaluator.getNormalVector( faceInfo[i], elements, neiNormal );
				double innerPro = normal*neiNormal;
				if( -innerPro < cosThres ){

					this->dividedFacesInfo.equivalent(f,faceInfo[i]);
				}else{

					if( f < faceInfo[i] ){
						this->edgesBetweenFaces.insert( std::pair<kmb::Face,kmb::Face>(f,faceInfo[i]) );
					}else{
						this->edgesBetweenFaces.insert( std::pair<kmb::Face,kmb::Face>(faceInfo[i],f) );
					}
				}
			}
			++dIter;
		}
	}
	return this->dividedFacesInfo.getClassCount();
}

size_t
kmb::SurfaceOperation::divideComponentsFaceGroup(const kmb::DataBindings* data, const kmb::ElementContainer* elements)
{
	if( data != NULL && data->getBindingMode() == kmb::DataBindings::FaceGroup && elements != NULL ){

		kmb::NodeNeighborFaceInfo nodeNeighborInfo;
		nodeNeighborInfo.appendCoboundary( data, elements );

		kmb::Face faceInfo[4];
		kmb::Face f;
		kmb::DataBindings::const_iterator dIter = data->begin();
		while( !dIter.isFinished() )
		{
			dIter.getFace(f);
			this->dividedFacesInfo.add(f);
			int faceNum = nodeNeighborInfo.getFaceNeighbor(f,faceInfo,elements);
			for(int i=0;i<faceNum;++i){
				this->dividedFacesInfo.equivalent(f,faceInfo[i]);
			}
			++dIter;
		}
	}
	return this->dividedFacesInfo.getClassCount();
}

size_t
kmb::SurfaceOperation::getDividedFaceGroup(kmb::Face initFace,kmb::DataBindings* data) const
{
	size_t count = 0;
	if( data != NULL && data->getBindingMode() == kmb::DataBindings::FaceGroup ){
		kmb::Classification< kmb::Face >::iterator fIter = this->dividedFacesInfo.begin( initFace );
		kmb::Classification< kmb::Face >::iterator fEnd = this->dividedFacesInfo.end( initFace );
		while( fIter != fEnd ){
			data->addId( fIter.getValue() );
			++count;
			++fIter;
		}
	}
	return count;
}

size_t
kmb::SurfaceOperation::getSurfaceRidge(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,kmb::ElementContainer* ridges) const
{
	size_t count = 0;
	if( orgSurf != NULL && orgSurf->isUniqueDim(2) && ridges != NULL ){
		kmb::nodeIdType cell[2];
		std::set< std::pair<kmb::elementIdType, kmb::elementIdType> >::const_iterator
			eIter = edgesBetweenElements.begin();
		while( eIter != edgesBetweenElements.end() ){


			if( !dividedElementsInfo.isEquivalent( eIter->first, eIter->second ) )
			{
				if( dividedElementsInfo.isEquivalent( eIter->first, initElementId ) ){
					kmb::ElementContainer::const_iterator elem0 = orgSurf->find( eIter->first );
					kmb::ElementContainer::const_iterator elem1 = orgSurf->find( eIter->second );
					kmb::elementType etype = kmb::ElementRelation::common( elem0, elem1, cell );
					if( etype == kmb::SEGMENT ){
						++count;
						ridges->addElement(etype,cell);
					}
				}
				else if( dividedElementsInfo.isEquivalent( eIter->second, initElementId ) ){

					kmb::ElementContainer::const_iterator elem0 = orgSurf->find( eIter->first );
					kmb::ElementContainer::const_iterator elem1 = orgSurf->find( eIter->second );
					kmb::elementType etype = kmb::ElementRelation::common( elem1, elem0, cell );
					if( etype == kmb::SEGMENT ){
						++count;
						ridges->addElement(etype,cell);
					}
				}
			}
			++eIter;
		}
	}
	return count;
}
