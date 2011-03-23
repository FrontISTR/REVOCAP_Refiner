/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : MeshDB                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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
#include "MeshDB/kmbMeshDB.h"

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbElementContainerArray.h"
#include "MeshDB/kmbElementContainerMArray.h"
#include "MeshDB/kmbElementContainerMixedOnDisk.h"
#include "MeshDB/kmbElementContainerTriangleArray.h"
#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_Geometry4D.h"
#include "Geometry/kmb_Point3DContainerVect.h"
#include "Geometry/kmb_Point3DContainerArray.h"
#include "Geometry/kmb_Point3DContainerMArray.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbScalarValueBindings.h"
#include "MeshDB/kmbScalarValueMArrayBindings.h"
#include "MeshDB/kmbScalarValueFileBindings.h"
#include "MeshDB/kmbVector2ValueBindings.h"
#include "MeshDB/kmbVector2ValueMArrayBindings.h"
#include "MeshDB/kmbVector2ValueFileBindings.h"
#include "MeshDB/kmbVector3ValueBindings.h"
#include "MeshDB/kmbVector3ValueMArrayBindings.h"
#include "MeshDB/kmbVector3ValueFileBindings.h"
#include "MeshDB/kmbTensor6ValueBindings.h"
#include "MeshDB/kmbTensor6ValueMArrayBindings.h"
#include "MeshDB/kmbTensor6ValueFileBindings.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbDataEvaluator.h"
#include "MeshDB/kmbVector2WithIntBindings.h"

#include <vector>
#include <utility>
#include <sstream>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#ifdef WIN32
 #define tempnam _tempnam
#else
#endif

kmb::MeshDB::MeshDB(void)
: MeshData()
, node2Ds(NULL)
, evaluator(NULL)
, dataEvaluator(NULL)
{
	defaultNodeContainerType = kmb::Point3DContainerMap::CONTAINER_TYPE;
	defaultElementContainerType = kmb::ElementContainerMap::CONTAINER_TYPE;
}



kmb::MeshDB::~MeshDB(void)
{
	clearModel();
}



void
kmb::MeshDB::clearModel(void)
{

	kmb::MeshData::clearModel();

	this->maxElementId = kmb::Element::nullElementId;
	this->defaultSpecType.clear();
	this->neighborInfo.clear();
	this->elementBucket.clear();
	this->elementOctree.clear();
	this->nodeOctree.clear();

	if( node2Ds ){
		delete node2Ds;
		node2Ds = NULL;
	}

	if( evaluator ){
		delete evaluator;
		evaluator = NULL;
	}

	if( dataEvaluator ){
		delete dataEvaluator;
		dataEvaluator = NULL;
	}
}

void
kmb::MeshDB::setDefaultNodeContainerType(const char* nodeContainerType)
{
	this->defaultNodeContainerType = nodeContainerType;
}

const char*
kmb::MeshDB::getDefaultNodeContainerType(void) const
{
	return this->defaultNodeContainerType.c_str();
}

void
kmb::MeshDB::setDefaultElementContainerType(const char* elementContainerType)
{
	this->defaultElementContainerType = elementContainerType;
}

const char*
kmb::MeshDB::getDefaultElementContainerType(void) const
{
	return this->defaultElementContainerType.c_str();
}

void
kmb::MeshDB::setDefaultDataContainerType(const char* dataContainerType)
{
	this->defaultDataContainerType = dataContainerType;
}

const char*
kmb::MeshDB::getDefaultDataContainerType(void) const
{
	return this->defaultDataContainerType.c_str();
}

void
kmb::MeshDB::setDefaultContainerType(const char* containerType)
{
	if( strcmp(containerType,"Post") == 0 ){

		this->defaultNodeContainerType = kmb::Point3DContainerArray::CONTAINER_TYPE;
		this->defaultElementContainerType = kmb::ElementContainerMArray::CONTAINER_TYPE;

		this->defaultDataContainerType = "Post";
	}
	if( strcmp(containerType,"File") == 0 ){
		this->defaultNodeContainerType = kmb::Point3DContainerMArray::CONTAINER_TYPE;
		this->defaultElementContainerType = kmb::ElementContainerMixedOnDisk::CONTAINER_TYPE;
		this->defaultDataContainerType = "File";
	}
	else if( strcmp(containerType,"Pre") == 0 ){
	}
}



int
kmb::MeshDB::getNodeDim(void)
{
	if( node3Ds != NULL ){
		return 3;
	}else if( node2Ds != NULL ){
		return 2;
	}else{
		return -1;
	}
}

void
kmb::MeshDB::beginNode(size_t size,kmb::Point3DContainer* point3Ds)
{
	if( node3Ds ){
		delete node3Ds;
		node3Ds = NULL;
	}
	if( node2Ds ){
		delete node2Ds;
		node2Ds = NULL;
	}
	if( evaluator ){
		delete evaluator;
		evaluator = NULL;
	}
	if( dataEvaluator ){
		delete dataEvaluator;
		dataEvaluator = NULL;
	}
	if( point3Ds != NULL ){
		node3Ds = point3Ds;
		node3Ds->initialize( size );
		evaluator = new kmb::ElementEvaluator( node3Ds );
		dataEvaluator = new kmb::DataEvaluator( node3Ds );
	}
}

void
kmb::MeshDB::beginNode(size_t size,kmb::Point2DContainer* point2Ds)
{
	if( node3Ds ){
		delete node3Ds;
		node3Ds = NULL;
	}
	if( node2Ds ){
		delete node2Ds;
		node2Ds = NULL;
	}
	if( evaluator ){
		delete evaluator;
		evaluator = NULL;
	}
	if( dataEvaluator ){
		delete dataEvaluator;
		dataEvaluator = NULL;
	}
	if( point2Ds != NULL ){
		node2Ds = point2Ds;
		node2Ds->initialize( size );
		evaluator = new kmb::ElementEvaluator( node2Ds );
		dataEvaluator = new kmb::DataEvaluator( node2Ds );
	}
}

void
kmb::MeshDB::beginNode(size_t size,const char* containerType)
{
	if( containerType == NULL ){
		containerType = getDefaultNodeContainerType();
	}
	kmb::Point3DContainer* point3Ds = kmb::Point3DContainer::createContainer( containerType );
	if( point3Ds ){
		this->beginNode(size,point3Ds);
	}else{
		kmb::Point2DContainer* point2Ds = kmb::Point2DContainer::createContainer( containerType );
		if( point2Ds ){
			this->beginNode(size,point2Ds);
		}else{

			this->beginNode(size,new kmb::Point3DContainerMap());
		}
	}
}

const char*
kmb::MeshDB::getNodeContainerType(void) const
{
	if( node3Ds != NULL ){
		return node3Ds->getContainerType();
	}else if( node2Ds != NULL ){
		return node2Ds->getContainerType();
	}else{
		return NULL;
	}
}

kmb::nodeIdType
kmb::MeshDB::addNode(double x,double y,double z)
{
	if( this->node3Ds != NULL )
		return this->node3Ds->addPoint( x,y,z );
	else if( this->node2Ds != NULL )
		return this->node2Ds->addPoint( x,y );
	else
		return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MeshDB::addNode(const Point3D& point)
{
	return kmb::MeshData::addNode(point.x(),point.y(),point.z());
}

kmb::nodeIdType
kmb::MeshDB::addNodeWithId(double x,double y,double z,kmb::nodeIdType id)
{
	if( this->node3Ds != NULL )			return this->node3Ds->addPoint(x,y,z,id);
	else if( this->node2Ds != NULL )	return this->node2Ds->addPoint(x,y,id);
	else								return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MeshDB::addNodeWithId(const Point3D& point,kmb::nodeIdType id)
{
	return kmb::MeshData::addNodeWithId(point.x(),point.y(),point.z(),id);
}

kmb::Point3DContainerMap::idContinuityType
kmb::MeshDB::getNodeContinuity() const
{
	if( this->node3Ds != NULL &&
		strcmp(this->node3Ds->getContainerType(), kmb::Point3DContainerMap::CONTAINER_TYPE)==0 )
		return static_cast< kmb::Point3DContainerMap* >(this->node3Ds)->getIdContinuity();
	else
		return kmb::Point3DContainerMap::UNKNOWN;
}

size_t
kmb::MeshDB::getNodeCount(void) const
{
	if( node3Ds )
		return this->node3Ds->getCount();
	else if( node2Ds )
		return this->node2Ds->getCount();
	else
		return 0;
}

bool
kmb::MeshDB::deleteNode(kmb::nodeIdType id)
{
	if( node3Ds ){
		return this->node3Ds->deletePoint(id);
	}else{
		return false;
	}
}

void
kmb::MeshDB::clearNodes(void)
{
	if( node3Ds ){
		this->node3Ds->clear();
	}
	if( node2Ds ){
		this->node2Ds->clear();
	}
}

void
kmb::MeshDB::convertAllNodes( kmb::Matrix4x4& mat )
{
	if( node3Ds ){
		node3Ds->convertAllNodes( mat );
	}
	const kmb::bodyIdType bodyCount = this->getBodyCount();
	for(kmb::bodyIdType i = 0;i<bodyCount;++i){
		kmb::Body* body = this->getBodyPtr(i);
		if( body ){
			kmb::Point3D bmax = body->getBoundingBox().getMax();
			kmb::Point3D bmin = body->getBoundingBox().getMin();
			mat.convert( bmax );
			mat.convert( bmin );
			body->setBoundingBox( bmax, bmin );
		}
	}
}

void
kmb::MeshDB::translateAllNodes( double x,double y,double z )
{
	if( node3Ds ){
		node3Ds->translateAllNodes(x,y,z);
	}
	const kmb::bodyIdType bodyCount = this->getBodyCount();
	for(kmb::bodyIdType i = 0;i<bodyCount;++i){
		kmb::Body* body = this->getBodyPtr(i);
		if( body ){
			kmb::Point3D bmax = body->getBoundingBox().getMax();
			kmb::Point3D bmin = body->getBoundingBox().getMin();
			bmax.addCoordinate(x,y,z);
			bmin.addCoordinate(x,y,z);
			body->setBoundingBox( bmax, bmin );
		}
	}
}

void
kmb::MeshDB::scaleAllNodes( double r )
{
	if( node3Ds ){
		node3Ds->scaleAllNodes(r);
	}
	const kmb::bodyIdType bodyCount = this->getBodyCount();
	for(kmb::bodyIdType i = 0;i<bodyCount;++i){
		kmb::Body* body = this->getBodyPtr(i);
		if( body ){
			kmb::Point3D bmax = body->getBoundingBox().getMax();
			kmb::Point3D bmin = body->getBoundingBox().getMin();
			bmax.scale(r);
			bmin.scale(r);
			body->setBoundingBox( bmax, bmin );
		}
	}
}

const kmb::Point3D*
kmb::MeshDB::getMaxPointPtr(void) const
{
	if( node3Ds == NULL ){
		return NULL;
	}else{
		return &(this->node3Ds->getBoundingBoxPtr()->getMax());
	}
}

const kmb::Point3D*
kmb::MeshDB::getMinPointPtr(void) const
{
	if( node3Ds == NULL ){
		return NULL;
	}else{
		return &(this->node3Ds->getBoundingBoxPtr()->getMin());
	}
}

const kmb::BoundingBox
kmb::MeshDB::getBoundingBox(void) const
{
	if( node3Ds != NULL ){
		return this->node3Ds->getBoundingBox();
	}else if( node2Ds != NULL ){
		kmb::BoundingBox bbox;
		bbox.update(
			this->node2Ds->getBoundingBox().maxX(),
			this->node2Ds->getBoundingBox().maxY(),
			0.0);
		bbox.update(
			this->node2Ds->getBoundingBox().minX(),
			this->node2Ds->getBoundingBox().minY(),
			0.0);
		return bbox;
	}else{
		kmb::BoundingBox bbox;
		return bbox;
	}
}

kmb::nodeIdType
kmb::MeshDB::insertNode(const Point3D& point)
{
	return kmb::MeshData::addNode(point.x(),point.y(),point.z());
}

kmb::nodeIdType
kmb::MeshDB::duplicatePoint(nodeIdType nodeId)
{
	if( this->node3Ds ){
		return this->node3Ds->duplicatePoint( nodeId );
	}else{
		return kmb::nullNodeId;
	}
}



kmb::bodyIdType
kmb::MeshDB::beginElement(size_t size,kmb::ElementContainer* container)
{
	kmb:: bodyIdType bodyId = kmb::MeshData::beginElement(size,container);

	if( this->currentBody &&
		strcmp( this->currentBody->getContainerType(), kmb::ElementContainerMap::CONTAINER_TYPE ) != 0 ){
		this->currentBody->setOffsetId( this->maxElementId + 1 );
	}
	return bodyId;
}

kmb::bodyIdType
kmb::MeshDB::beginElement(size_t size,const char* containerType)
{
	if( containerType == NULL ){
		containerType = defaultElementContainerType.c_str();
	}
	kmb::ElementContainer* elements = kmb::ElementContainer::createContainer( containerType, size );
	if( elements ){
		return this->beginElement(size,elements);
	}
	else{
		return this->beginElement(size,new kmb::ElementContainerMap());
	}
}

kmb::elementIdType
kmb::MeshDB::addElement(kmb::elementType type,kmb::nodeIdType *ary)
{
	if( this->currentBody && ary ){
		return currentBody->addElement( type, ary, generateElementId() );
	}else{
		return kmb::Element::nullElementId;
	}
}

kmb::elementIdType
kmb::MeshDB::addElementWithId(kmb::elementType type,kmb::nodeIdType *ary,kmb::elementIdType elementID)
{

	if(maxElementId >= elementID){
		kmb::bodyIdType bodyId = this->getBodyId(elementID);
		if(bodyId != kmb::Body::nullBodyId){
			return kmb::Element::nullElementId;
		}
	}
	if( this->currentBody ){
		elementID = this->currentBody->addElement(type,ary,elementID);
	}else{
		elementID = kmb::Element::nullElementId;
	}

	if( maxElementId < elementID ){
		maxElementId = elementID;
	}
	return elementID;
}

kmb::elementIdType
kmb::MeshDB::insertElement(kmb::bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary)
{
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	kmb::ElementContainer* body = this->getBodyPtr( bodyID );
	if( body != NULL ){
		kmb::elementIdType id = generateElementId();
		elementId = body->addElement(type,ary,id);
	}
	return elementId;
}

kmb::elementIdType
kmb::MeshDB::insertElementWithId(bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary,elementIdType elementId)
{

	if(maxElementId >= elementId){
		kmb::bodyIdType bodyId = this->getBodyId(elementId);
		if(bodyId != kmb::Body::nullBodyId){

			return kmb::Element::nullElementId;
		}
	}
	kmb::ElementContainer* body = this->getBodyPtr( bodyID );
	if( body == NULL ||
		body->addElement(type,ary,elementId) == kmb::Element::nullElementId ){

		return kmb::Element::nullElementId;
	}

	if( maxElementId < elementId ){
		maxElementId = elementId;
	}
	return elementId;
}














kmb::elementIdType
kmb::MeshDB::generateElementId(void)
{

	return ++maxElementId;
}

kmb::elementIdType
kmb::MeshDB::deleteElement(kmb::bodyIdType bodyId,kmb::elementIdType elementId)
{
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if(body && body->deleteElement(elementId)){
		return elementId;
	}
	return kmb::Element::nullElementId;
}

kmb::ElementContainer::iterator
kmb::MeshDB::findElement(kmb::elementIdType elementId,kmb::bodyIdType bodyId)
{
	kmb::Body* body = NULL;
	if( bodyId >= 0 && (body=this->getBodyPtr( bodyId )) != NULL ){
		return body->find( elementId );
	}else{
		const size_t len = this->getBodyCount();
		for(unsigned int i=0;i<len;++i)
		{
			body = this->getBodyPtr(i);
			if( body != NULL ){
				kmb::ElementContainer::iterator eIter = body->find( elementId );
				if( !eIter.isFinished() ){
					return eIter;
				}
			}
		}
	}
	return kmb::ElementContainer::getEndIterator();
}

kmb::ElementContainer::const_iterator
kmb::MeshDB::findElement(kmb::elementIdType elementId,kmb::bodyIdType bodyId) const
{
	const kmb::Body* body = NULL;
	if( bodyId >= 0 && (body=this->getBodyPtr( bodyId )) != NULL ){
		return body->find( elementId );
	}else{
		const size_t len = this->getBodyCount();
		for(unsigned int i=0;i<len;++i)
		{
			body = this->getBodyPtr(i);
			if( body != NULL ){
				kmb::ElementContainer::const_iterator eIter = body->find( elementId );
				if( !eIter.isFinished() ){
					return eIter;
				}
			}
		}
	}
	return kmb::ElementContainer::getEndConstIterator();
}

bool
kmb::MeshDB::elementToFace( const kmb::ElementBase &element, kmb::Face &face, kmb::bodyIdType bodyId)
{
	int index = -1;
	int otherIndex = -1;
	if( bodyId != kmb::Body::nullBodyId ){
		kmb::Body* body = this->getBodyPtr( bodyId );
		if( body != NULL ){

			if( this->neighborInfo.getSize() > 0 ){
				return this->neighborInfo.getFace( element, face, body );
			}else{
				kmb::ElementContainer::iterator eIter = body->begin();
				while( eIter != body->end() ){
					kmb::ElementRelation::relationType rel =
						kmb::ElementRelation::getRelation( eIter, index, element, otherIndex );
					if( rel == kmb::ElementRelation::COBOUNDARY ||
						rel == kmb::ElementRelation::ANTICOBOUNDARY ){
						face.setId( eIter.getId(), index );
						return true;
					}
					++eIter;
				}
			}
		}
	}else{
		kmb::bodyIdType bCount = this->getBodyCount();
		for( kmb::bodyIdType id = 0; id < bCount; ++id ){
			kmb::Body* body = this->getBodyPtr( id );
			if( body != NULL ){

				if( this->neighborInfo.getSize() > 0 ){
					if( this->neighborInfo.getFace( element, face, body ) ){
						return true;
					}
				}else{
					kmb::ElementContainer::iterator eIter = body->begin();
					while( eIter != body->end() ){
						kmb::ElementRelation::relationType rel =
							kmb::ElementRelation::getRelation( eIter, index, element, otherIndex );
						if( rel == kmb::ElementRelation::COBOUNDARY ||
							rel == kmb::ElementRelation::ANTICOBOUNDARY ){
							face.setId( eIter.getId(), index );
							return true;
						}
						++eIter;
					}
				}
			}
		}
	}
	return false;
}



kmb::bodyIdType
kmb::MeshDB::getBodyId(kmb::elementIdType elementID) const
{
	const kmb::bodyIdType len = this->getBodyCount();
	for(kmb::bodyIdType bodyID=0; bodyID<len; ++bodyID)
	{
		if( this->bodies[bodyID] != NULL &&
			this->bodies[bodyID]->includeElement(elementID) )
		{
			return bodyID;
		}
	}
	return kmb::Body::nullBodyId;
}

void
kmb::MeshDB::endModel(void)
{
	std::vector< kmb::ElementContainer* >::iterator bIter = this->bodies.begin();
	while( bIter != this->bodies.end() )
	{
		kmb::ElementContainer* body = (*bIter);
		if( body != NULL )
		{
			body->updateBoundingBox( this->getNodes() );
			if( this->maxElementId < body->getMaxId() ){
				this->maxElementId = body->getMaxId();
			}
		}
		++bIter;
	}
}

void
kmb::MeshDB::removeAllBodies(void)
{
	kmb::MeshData::removeAllBodies();
	this->maxElementId = kmb::Element::nullElementId;
	this->neighborInfo.clear();
	this->elementOctree.clear();
	this->elementBucket.clear();
}



kmb::Vector3D
kmb::MeshDB::getNormalVector(kmb::elementIdType elementID,kmb::bodyIdType bodyID) const
{
	kmb::ElementContainer::const_iterator element = this->findElement( elementID, bodyID );
	kmb::Vector3D v;
	if( !element.isFinished() && evaluator && evaluator->getNormalVector( element, v ) ){
		return v;
	}else{
		return kmb::Vector3D();
	}
}

kmb::Vector3D
kmb::MeshDB::getNormalVector(kmb::Face face,kmb::bodyIdType bodyID) const
{
	kmb::ElementContainer::const_iterator element = this->findElement( face.getElementId(), bodyID );
	kmb::Vector3D v;
	if( !element.isFinished() && evaluator && evaluator->getNormalVectorOfFace( element, face.getLocalFaceId(), v ) ){
		return v;
	}else{
		return kmb::Vector3D();
	}
}

kmb::Vector3D
kmb::MeshDB::getEdgeVector(nodeIdType i0,nodeIdType i1) const
{
	kmb::Point3D p0,p1;
	this->getNode(i0,p0);
	this->getNode(i1,p1);
	return kmb::Vector3D(p0,p1);
}

kmb::Vector3D
kmb::MeshDB::getAreaVectorOfSurface(kmb::bodyIdType bodyId) const
{
	kmb::Vector3D area;
	kmb::Point3D p0,p1,p2,p3;
	const kmb::ElementContainer* elements = this->getBodyPtr(bodyId);
	if( elements && elements->getDimension() == 2 ){
		for( kmb::ElementContainer::const_iterator eIter = elements->begin();
			!eIter.isFinished(); ++eIter)
		{
			switch( eIter.getType() )
			{
			case kmb::TRIANGLE:
			case kmb::TRIANGLE2:
				if( getNode( eIter[0], p0 ) &&
					getNode( eIter[1], p1 ) &&
					getNode( eIter[2], p2 ) )
				{
					area += kmb::Point3D::areaVector( p0, p1, p2 );
				}
				break;
			case kmb::QUAD:
			case kmb::QUAD2:
				if( getNode( eIter[0], p0 ) &&
					getNode( eIter[1], p1 ) &&
					getNode( eIter[2], p2 ) &&
					getNode( eIter[3], p3 ) )
				{
					area += kmb::Point3D::areaVector( p0, p1, p2 );
					area += kmb::Point3D::areaVector( p0, p2, p3 );
				}
				break;
			default:
				break;
			}
		}
	}
	return area;
}

kmb::Vector3D
kmb::MeshDB::getAreaVectorOfFaceGroup(const char* faceGroup) const
{
	kmb::Vector3D area;
	kmb::Point3D p0,p1,p2,p3;
	const kmb::DataBindings* data = this->getDataBindingsPtr(faceGroup);
	const kmb::ElementContainer* elements = NULL;
	kmb::Face f;
	if( data &&
		( data->getBindingMode() == kmb::DataBindings::FACEGROUP || data->getBindingMode() == kmb::DataBindings::FACEVARIABLE ) &&
		( elements = this->getBodyPtr( data->getTargetBodyId() ) ) != NULL )
	{
		for( kmb::DataBindings::const_iterator fIter = data->begin();
			!fIter.isFinished(); ++fIter )
		{
			if( fIter.getFace(f) ){
				kmb::ElementContainer::const_iterator eIter = elements->find( f.getElementId() );
				kmb::idType i = f.getLocalFaceId();
				switch( eIter.getBoundaryType(i) )
				{
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					if( getNode( eIter.getBoundaryCellId(i,0), p0 ) &&
						getNode( eIter.getBoundaryCellId(i,1), p1 ) &&
						getNode( eIter.getBoundaryCellId(i,2), p2 ) )
					{
						area += kmb::Point3D::areaVector( p0, p1, p2 );
					}
					break;
				case kmb::QUAD:
				case kmb::QUAD2:
					if( getNode( eIter.getBoundaryCellId(i,0), p0 ) &&
						getNode( eIter.getBoundaryCellId(i,1), p1 ) &&
						getNode( eIter.getBoundaryCellId(i,2), p2 ) &&
						getNode( eIter.getBoundaryCellId(i,3), p3 ) )
					{
						area += kmb::Point3D::areaVector( p0, p1, p2 );
						area += kmb::Point3D::areaVector( p0, p2, p3 );
					}
					break;
				default:
					break;
				}
			}
		}
	}
	return area;
}




kmb::DataBindings*
kmb::MeshDB::createDataBindings
(const char* name,kmb::DataBindings::bindingMode bmode,PhysicalValue::valueType vtype,const char* stype,kmb::bodyIdType targetBodyId)
{
	if( name == NULL || this->getDataBindingsPtr( name, stype ) != NULL ){

		return NULL;
	}else{
		kmb::DataBindings* data = NULL;
		if( defaultDataContainerType == "Post" ){
			switch( vtype )
			{
			case kmb::PhysicalValue::SCALAR:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					data = new kmb::ScalarValueBindings( getMaxNodeId()+1, bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){

					data = new kmb::ScalarValueMArrayBindings( getMaxElementId()+1, bmode );
				}
				break;
			case kmb::PhysicalValue::VECTOR2:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					data = new kmb::Vector2ValueBindings( getMaxNodeId()+1, bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){

					data = new kmb::Vector2ValueMArrayBindings( getMaxElementId()+1, bmode );
				}
				break;
			case kmb::PhysicalValue::VECTOR3:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					data = new kmb::Vector3ValueBindings( getMaxNodeId()+1, bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){

					data = new kmb::Vector3ValueMArrayBindings( getMaxElementId()+1, bmode );
				}
				break;
			case kmb::PhysicalValue::TENSOR6:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					data = new kmb::Tensor6ValueBindings( getMaxNodeId()+1, bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){

					data = new kmb::Tensor6ValueMArrayBindings( getMaxElementId()+1, bmode );
				}
				break;
			default:
				break;
			}
		}
		else if( defaultDataContainerType == "File" ){
			switch( vtype )
			{
			case kmb::PhysicalValue::SCALAR:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::ScalarValueFileBindings( getMaxNodeId()+1, tmpfile.c_str(), bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::ScalarValueFileBindings( getMaxElementId()+1, tmpfile.c_str(), bmode );
				}
				break;
			case kmb::PhysicalValue::VECTOR2:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::Vector2ValueFileBindings( getMaxNodeId()+1, tmpfile.c_str(), bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::Vector2ValueFileBindings( getMaxElementId()+1, tmpfile.c_str(), bmode );

				}
				break;
			case kmb::PhysicalValue::VECTOR3:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::Vector3ValueFileBindings( getMaxNodeId()+1, tmpfile.c_str(), bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::Vector3ValueFileBindings( getMaxElementId()+1, tmpfile.c_str(), bmode );
				}
				break;
			case kmb::PhysicalValue::TENSOR6:
				if( bmode == kmb::DataBindings::NODEVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::Tensor6ValueFileBindings( getMaxNodeId()+1, tmpfile.c_str(), bmode );
				}else if( bmode == kmb::DataBindings::ELEMENTVARIABLE ){
					std::string tmpfile = "tmp_";
					tmpfile.append(name);
					data = new kmb::Tensor6ValueFileBindings( getMaxElementId()+1, tmpfile.c_str(), bmode );
				}
				break;
			default:
				break;
			}
		}

		if( data == NULL ){
			if( vtype == kmb::PhysicalValue::VECTOR2WITHINT && bmode == kmb::DataBindings::NODEVARIABLE ){
				data = new kmb::Vector2WithIntBindings<kmb::nodeIdType>( getMaxNodeId()+1, bmode );
			}
		}
		if( data == NULL ){
			data = kmb::DataBindings::createDataBindings(bmode,vtype,stype,targetBodyId);
		}
		if( data ){
			if( setDataBindingsPtr( name, data, stype ) ){
				return data;
			}else{
				delete data;
			}
		}
		return NULL;
	}
}

std::string
kmb::MeshDB::getUniqueDataName(std::string prefix,int num)
{

	int count = num;
	std::stringstream stream;
	do{
		stream.str("");
		stream.clear();
		stream << prefix << count;
		++count;
	}while( this->hasData(stream.str().c_str()) );
	return stream.str();
}

bool
kmb::MeshDB::replaceIdOfData(const char* name,kmb::idType oldID,kmb::idType newID,const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		data->replaceId(oldID,newID);
		return true;
	}else{
		return false;
	}
}

size_t
kmb::MeshDB::getElementCountOfData(const char* name,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	size_t num = 0;
	if( data ){
		switch( data->getBindingMode() ){
			case kmb::DataBindings::NODEGROUP:
			case kmb::DataBindings::NODEVARIABLE:
				break;
			case kmb::DataBindings::ELEMENTGROUP:
			case kmb::DataBindings::FACEGROUP:
			case kmb::DataBindings::ELEMENTVARIABLE:
			case kmb::DataBindings::FACEVARIABLE:
			{
				num = data->getIdCount();
				break;
			}
			case kmb::DataBindings::BODYGROUP:
			case kmb::DataBindings::BODYVARIABLE:
			{
				kmb::DataBindings::const_iterator dIter = data->begin();
				kmb::DataBindings::const_iterator dEnd = data->end();
				while( dIter != dEnd ){
					kmb::bodyIdType id = static_cast< kmb::bodyIdType >( dIter.getId() );
					num += getElementCount(id);
					++dIter;
				}
				break;
			}
			case kmb::DataBindings::GLOBAL:
			{
				for(kmb::bodyIdType id = 0;
					id < this->getBodyCount();
					++id)
				{
					num += getElementCount(id);
				}
				break;
			}
			default:
				break;
		}
	}
	return num;
}

void
kmb::MeshDB::getNearestValue(std::string key,const kmb::PhysicalValue* value,kmb::idType& id) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(key.c_str());
	double minDist = DBL_MAX;
	kmb::idType minId = -1;
	if(data != NULL){
		switch( data->getBindingMode() ){
			case kmb::DataBindings::BODYVARIABLE:
			case kmb::DataBindings::ELEMENTVARIABLE:
			case kmb::DataBindings::NODEVARIABLE:
				{
					kmb::DataBindings::const_iterator dIter = data->begin();
					while( !dIter.isFinished() )
					{
						const kmb::PhysicalValue* v = dIter.getValue();
						if( v ){
							double d = v->distanceSq(value);
							if( d >= 0.0 && d < minDist ){
								minDist = d;
								minId = dIter.getId();
							}
						}
						++dIter;
					}
				}
				break;
			default:
				break;
		}
	}
	id = minId;
}

void
kmb::MeshDB::getNearestValue(std::string key,const kmb::PhysicalValue* value,kmb::Face& f) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(key.c_str());
	double minDist = DBL_MAX;
	kmb::Face minFace(-1,-1);
	if(data != NULL){
		switch( data->getBindingMode() )
		{
		case kmb::DataBindings::FACEVARIABLE:
		{
			kmb::DataBindings::const_iterator fIter = data->begin();
			while( !fIter.isFinished() ){
				double d = fIter.getValue()->distanceSq(value);
				if( d >= 0.0 && d < minDist ){
					minDist = d;
					fIter.getFace( minFace );
				}
				++fIter;
			}
			break;
		}
		default:
			break;
		}
	}
	f = minFace;
}

void
kmb::MeshDB::getBoundingBoxOfData(kmb::BoundingBox &bbox,const kmb::DataBindings* data) const
{
	kmb::MeshData::getBoundingBoxOfData(bbox,data);
	return;
}

const kmb::BoundingBox
kmb::MeshDB::getBoundingBoxOfData(const char* key,const char* stype) const
{
	kmb::BoundingBox bbox;
	const kmb::DataBindings* data = this->getDataBindingsPtr(key,stype);
	if(data != NULL){
		this->getBoundingBoxOfData(bbox,data);
	}
	return bbox;
}

bool
kmb::MeshDB::getMinMaxValue(const kmb::DataBindings* data,kmb::BoundingBox1D &bbox) const
{
	if( data != NULL && this->dataEvaluator ){
		this->dataEvaluator->getMinMax( data, bbox );
		return true;
	}
	return false;
}

bool
kmb::MeshDB::getMinMaxValue(const kmb::DataBindings* data,kmb::BoundingBox &bbox) const
{
	if( data != NULL && this->dataEvaluator ){
		this->dataEvaluator->getMinMax( data, bbox );
		return true;
	}
	return false;
}

bool
kmb::MeshDB::getMinMaxValueWithId(const kmb::DataBindings* data,kmb::MinMaxWithId& minmax,int comp) const
{
	if( data != NULL && this->dataEvaluator ){
		this->dataEvaluator->getMinMaxWithId( data, minmax, comp );
		return true;
	}
	return false;
}








int
kmb::MeshDB::convertData(const char* org, const char* conv, const char* orgstype,const char* convstype)
{
	kmb::DataBindings* orgData = this->getDataBindingsPtr(org,orgstype);
	kmb::DataBindings* convData = this->getDataBindingsPtr(conv,convstype);
	return convertData(orgData,convData);
}

int
kmb::MeshDB::convertData(const kmb::DataBindings* orgData, kmb::DataBindings* convData)
{
	int dataCount = 0;
	if( orgData == NULL || convData == NULL ){
		return -1;
	}

	if( orgData->getBindingMode() == kmb::DataBindings::NODEGROUP &&
		convData->getBindingMode() == kmb::DataBindings::FACEGROUP )
	{
		const kmb::DataBindings* nodeGroup = orgData;
		kmb::DataBindings* faceGroup = convData;
		kmb::bodyIdType bodyId  = faceGroup->getTargetBodyId();
		kmb::ElementContainer* elements = this->getBodyPtr( bodyId );
		if( elements ){

			kmb::DataBindings* elementGroup
				= kmb::DataBindings::createDataBindings( kmb::DataBindings::ELEMENTGROUP, kmb::PhysicalValue::NONE, "" );
			kmb::NodeNeighborInfo neighborInfo;
			neighborInfo.appendCoboundary( elements );
			std::vector< kmb::elementIdType > surrounding;
			kmb::DataBindings::const_iterator nIter = nodeGroup->begin();
			while( !nIter.isFinished() ){
				kmb::nodeIdType nodeId = nIter.getId();
				surrounding.clear();
				neighborInfo.getSurroundingElements( nodeId, surrounding );
				std::vector< kmb::elementIdType >::iterator sIter = surrounding.begin();
				while( sIter != surrounding.end() ){
					kmb::elementIdType elementId = *sIter;
					elementGroup->addId( elementId );
					++sIter;
				}
				++nIter;
			}
			kmb::DataBindings::iterator eIter = elementGroup->begin();
			while( !eIter.isFinished() ){
				kmb::ElementContainer::iterator elem = elements->find( eIter.getId() );
				int boundaryCount = elem.getBoundaryCount();
				for(int i=0;i<boundaryCount;++i){
					bool isFace = true;
					int boundaryNodeCount = elem.getBoundaryNodeCount(i);
					for(int j=0;j<boundaryNodeCount;++j){
						if( !nodeGroup->hasId( elem.getBoundaryCellId(i,j) ) ){
							isFace = false;
							break;
						}
					}
					if( isFace ){
						kmb::Face f( elem.getId(), i );
						faceGroup->addId( f );
						++dataCount;
					}
				}
				++eIter;
			}
			delete elementGroup;
		}
		if( nodeGroup->getValueType() != kmb::PhysicalValue::NONE &&
			nodeGroup->getValueType() == faceGroup->getValueType() )
		{
			faceGroup->setPhysicalValue( nodeGroup->getPhysicalValue() );
		}
	}

	else
	if( orgData->getBindingMode() == kmb::DataBindings::NODEGROUP &&
		convData->getBindingMode() == kmb::DataBindings::ELEMENTGROUP )
	{
		const kmb::DataBindings* nodeGroup = orgData;
		kmb::DataBindings* elementGroup = convData;
		kmb::bodyIdType bodyId  = elementGroup->getTargetBodyId();
		kmb::ElementContainer* elements = this->getBodyPtr( bodyId );
		if( elements ){

			kmb::DataBindings* eGroup
				= kmb::DataBindings::createDataBindings( kmb::DataBindings::ELEMENTGROUP, kmb::PhysicalValue::NONE, "" );
			kmb::NodeNeighborInfo neighborInfo;
			neighborInfo.appendCoboundary( elements );
			std::vector< kmb::elementIdType > surrounding;
			kmb::DataBindings::const_iterator nIter = nodeGroup->begin();
			while( !nIter.isFinished() ){
				kmb::nodeIdType nodeId = nIter.getId();
				surrounding.clear();
				neighborInfo.getSurroundingElements( nodeId, surrounding );
				std::vector< kmb::elementIdType >::iterator sIter = surrounding.begin();
				while( sIter != surrounding.end() ){
					kmb::elementIdType elementId = *sIter;
					eGroup->addId( elementId );
					++sIter;
				}
				++nIter;
			}

			kmb::DataBindings::iterator eIter = eGroup->begin();
			while( !eIter.isFinished() ){
				kmb::ElementContainer::iterator elem = elements->find( eIter.getId() );
				int nodeCount = elem.getNodeCount();
				bool isElement = true;
				for(int i=0;i<nodeCount;++i){
					if( !nodeGroup->hasId( elem.getCellId(i) ) ){
						isElement = false;
						break;
					}
				}
				if( isElement ){
					elementGroup->addId( elem.getId() );
					++dataCount;
				}
				++eIter;
			}
			delete eGroup;
		}
		if( nodeGroup->getValueType() != kmb::PhysicalValue::NONE &&
			nodeGroup->getValueType() == elementGroup->getValueType() )
		{
			elementGroup->setPhysicalValue( nodeGroup->getPhysicalValue() );
		}
	}

	else
	if( orgData->getBindingMode() == kmb::DataBindings::FACEGROUP &&
		convData->getBindingMode() == kmb::DataBindings::NODEGROUP )
	{
		const kmb::DataBindings* faceGroup = orgData;
		kmb::DataBindings* nodeGroup = convData;
		kmb::bodyIdType bodyId  = faceGroup->getTargetBodyId();
		kmb::ElementContainer* elements = this->getBodyPtr( bodyId );
		if( elements ){
			kmb::DataBindings::const_iterator fIter = faceGroup->begin();
			while( !fIter.isFinished() ){
				kmb::Face f;
				fIter.getFace( f );
				kmb::ElementContainer::iterator elem = elements->find( f.getElementId() );
				kmb::idType localIndex = f.getLocalFaceId();
				if( !elem.isFinished() ){
					int len = elem.getBoundaryNodeCount(localIndex);
					for(int i=0;i<len;++i){
						if( nodeGroup->addId( elem.getBoundaryCellId( localIndex, i ) ) ){
							++dataCount;
						}
					}
				}
				++fIter;
			}
		}
		if( faceGroup->getValueType() != kmb::PhysicalValue::NONE &&
			faceGroup->getValueType() == nodeGroup->getValueType() )
		{
			nodeGroup->setPhysicalValue( faceGroup->getPhysicalValue() );
		}
	}

	else
	if( orgData->getBindingMode() == kmb::DataBindings::FACEGROUP &&
		convData->getBindingMode() == kmb::DataBindings::FACEGROUP )
	{
		const kmb::DataBindings* faceGroup0 = orgData;
		kmb::DataBindings* faceGroup1 = convData;
		kmb::DataBindings::const_iterator fIter = faceGroup0->begin();
		while( !fIter.isFinished() ){
			kmb::Face f;
			fIter.getFace( f );
			faceGroup1->addId( f );
			++dataCount;
			++fIter;
		}
		if( faceGroup0->getValueType() != kmb::PhysicalValue::NONE &&
			faceGroup0->getValueType() == faceGroup1->getValueType() )
		{
			faceGroup1->setPhysicalValue( faceGroup0->getPhysicalValue() );
		}
	}

	else
	if( orgData->getBindingMode() == kmb::DataBindings::FACEGROUP &&
		convData->getBindingMode() == kmb::DataBindings::ELEMENTGROUP )
	{
		const kmb::DataBindings* faceGroup = orgData;
		kmb::DataBindings* elementGroup = convData;
		kmb::bodyIdType bodyId  = faceGroup->getTargetBodyId();
		kmb::ElementContainer* elements = this->getBodyPtr( bodyId );
		if( elements ){
			kmb::DataBindings::const_iterator fIter = faceGroup->begin();
			while( !fIter.isFinished() ){
				kmb::Face f;
				fIter.getFace( f );
				if( elementGroup->addId( f.getElementId() ) ){
					++dataCount;
				}
				++fIter;
			}
		}
		if( faceGroup->getValueType() != kmb::PhysicalValue::NONE &&
			faceGroup->getValueType() == elementGroup->getValueType() )
		{
			elementGroup->setPhysicalValue( faceGroup->getPhysicalValue() );
		}
	}

	else
	if( orgData->getBindingMode() == kmb::DataBindings::ELEMENTGROUP &&
		convData->getBindingMode() == kmb::DataBindings::NODEGROUP )
	{
		const kmb::DataBindings* elementGroup = orgData;
		kmb::DataBindings* nodeGroup = convData;
		kmb::bodyIdType bodyId  = elementGroup->getTargetBodyId();
		kmb::ElementContainer* elements = this->getBodyPtr( bodyId );
		if( elements ){
			kmb::DataBindings::const_iterator eIter = elementGroup->begin();
			while( !eIter.isFinished() ){
				kmb::elementIdType elemId = eIter.getId();
				kmb::ElementContainer::iterator elem = elements->find( elemId );
				if( !elem.isFinished() ){
					int len = elem.getNodeCount();
					for(int i=0;i<len;++i){
						if( nodeGroup->addId( elem.getCellId( i ) ) ){
							++dataCount;
						}
					}
				}
				++eIter;
			}
		}
		if( elementGroup->getValueType() != kmb::PhysicalValue::NONE &&
			elementGroup->getValueType() == nodeGroup->getValueType() )
		{
			nodeGroup->setPhysicalValue( elementGroup->getPhysicalValue() );
		}
	}
	return dataCount;
}

int
kmb::MeshDB::convertBody2Data(kmb::bodyIdType bodyId, const char* name,const char* stype)
{
	int dataCount = 0;
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);

	if( data != NULL && data->getBindingMode() == kmb::DataBindings::NODEGROUP )
	{
		kmb::ElementContainer* elements = this->getBodyPtr( bodyId );
		if( elements ){
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				int len = eIter.getNodeCount();
				for(int i=0;i<len;++i){
					if( data->addId( eIter[i] ) ){
						++dataCount;
					}
				}
				++eIter;
			}
		}
	}
	return dataCount;
}
