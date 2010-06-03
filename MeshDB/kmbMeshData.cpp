/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : MeshData                                                #
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
#include "MeshDB/kmbMeshData.h"
#include "Geometry/kmb_Calculator.h"
#include "Geometry/kmb_Point3DContainerVect.h"
#include "Geometry/kmb_Point3DContainerArray.h"
#include "Geometry/kmb_Geometry4D.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbElementContainerArray.h"
#include "MeshDB/kmbElementContainerTriangleArray.h"

#include <cstring>

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

kmb::MeshData::MeshData(void)
: node3Ds(NULL)
, currentBody(NULL)
, bindingsStackDim(0)
, maxElementId(kmb::Element::nullElementId)
, coordMatrix(NULL)
{
}

kmb::MeshData::~MeshData(void)
{
	clearModel();
}

void kmb::MeshData::clearModel(void)
{

	if( node3Ds ){
		delete node3Ds;
		node3Ds = NULL;
	}
	if( coordMatrix ){
		delete coordMatrix;
		coordMatrix = NULL;
	}

	removeAllBodies();

	std::multimap<std::string, kmb::DataBindings*>::iterator pIter = this->bindings.begin();
	while( pIter != this->bindings.end() )
	{
		kmb::DataBindings* data = pIter->second;
		if(data){
			delete data;
			data = NULL;
		}
		++pIter;
	}
	this->bindingsStack.clear();
	this->bindingsStackDim = 0;
	this->bindings.clear();
}

void
kmb::MeshData::beginModel(void)
{
}

void
kmb::MeshData::endModel(void)
{
}

kmb::Matrix4x4*
kmb::MeshData::getCoordMatrix(void)
{
	return coordMatrix;
}

const kmb::Matrix4x4*
kmb::MeshData::getCoordMatrix(void) const
{
	return coordMatrix;
}

void
kmb::MeshData::setCoordinate( const kmb::Matrix4x4& matrix )
{
	if( coordMatrix == NULL ){
		coordMatrix = new kmb::Matrix4x4();
	}
	*coordMatrix = matrix;
}

void
kmb::MeshData::translateCoordinate( double x, double y, double z )
{
	if( coordMatrix == NULL ){
		coordMatrix = new kmb::Matrix4x4();
	}
	coordMatrix->translate(x,y,z);
}

void
kmb::MeshData::beginNode(size_t size,kmb::Point3DContainer* point3Ds)
{
	if( node3Ds ){
		delete node3Ds;
		node3Ds = NULL;
	}
	if( point3Ds != NULL ){
		node3Ds = point3Ds;
		node3Ds->initialize( size );
	}
}

void
kmb::MeshData::beginNode(size_t size,const char* containerType)
{
	if( containerType == NULL ){
		this->beginNode(size,new kmb::Point3DContainerMap());
	}
	else if(0==strcmp(containerType,kmb::Point3DContainerMap::CONTAINER_TYPE) ){
		this->beginNode(size,new kmb::Point3DContainerMap());
	}
	else if(0==strcmp(containerType,kmb::Point3DContainerArray::CONTAINER_TYPE) ){
		this->beginNode(size,new kmb::Point3DContainerArray());
	}
	else if(0==strcmp(containerType,kmb::Point3DContainerVect::CONTAINER_TYPE) ){
		this->beginNode(size,new kmb::Point3DContainerVect());
	}
	else{
		this->beginNode(size,new kmb::Point3DContainerMap());
	}
}

kmb::nodeIdType
kmb::MeshData::addNode(double x,double y,double z)
{
	if( this->node3Ds ){
		return this->node3Ds->addPoint( x, y, z );
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MeshData::addNodeWithId(double x,double y,double z, kmb::nodeIdType id)
{
	if( this->node3Ds ){
		return this->node3Ds->addPoint( x, y, z, id );
	}
	return kmb::nullNodeId;
}

void kmb::MeshData::endNode(void)
{
}

const kmb::Point3DContainer*
kmb::MeshData::getNodes(void) const
{
	return this->node3Ds;
}

kmb::Point3DContainer*
kmb::MeshData::getNodes(void)
{
	return this->node3Ds;
}

size_t
kmb::MeshData::getNodeCount(void) const
{
	if( this->node3Ds ){
		return this->node3Ds->getCount();
	}
	return 0;
}

bool
kmb::MeshData::getNode(nodeIdType i,kmb::Point3D &node) const
{
	if( this->node3Ds != NULL ){
		bool res = node3Ds->getPoint(i,node);
		if( res && coordMatrix ){
			coordMatrix->convert( node );
		}
		return res;
	}
	return false;
}

bool
kmb::MeshData::getNodeXYZ(nodeIdType i,double &x,double &y,double &z) const
{
	if( node3Ds != NULL ){
		bool res = node3Ds->getXYZ(i,x,y,z);
		if( res && coordMatrix ){
			coordMatrix->convert( x, y, z );
		}
		return res;
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::MeshData::insertNode(double x,double y,double z)
{
	return addNode(x,y,z);
}

kmb::nodeIdType
kmb::MeshData::insertNodeWithId(double x,double y,double z, kmb::nodeIdType id)
{
	return addNodeWithId(x,y,z,id);
}

bool
kmb::MeshData::updateNode(double x,double y,double z, kmb::nodeIdType id)
{
	if( this->node3Ds ){
		return this->node3Ds->updateCoordinate( id, x, y, z );
	}
	return false;
}

const char*
kmb::MeshData::getNodeContainerType(void) const
{
	if( node3Ds != NULL ){
		return node3Ds->getContainerType();
	}else{
		return NULL;
	}
}

const kmb::BoundingBox
kmb::MeshData::getBoundingBox(void) const
{
	if( node3Ds != NULL ){
		return this->node3Ds->getBoundingBox();
	}else{
		kmb::BoundingBox bbox;
		return bbox;
	}
}

kmb::bodyIdType
kmb::MeshData::beginElement(size_t size,kmb::ElementContainer* container)
{
	if( this->currentBody ){
		return kmb::Body::nullBodyId;
	}
	if( container == NULL ){
		container = new kmb::ElementContainerMap();
	}else{
		container->setOffsetId( this->maxElementId + 1 );
	}
	this->currentBody = container;
	return static_cast<bodyIdType>( this->bodies.size() );
}

kmb::bodyIdType
kmb::MeshData::beginElement(size_t size,const char* containerType)
{
	if(containerType==NULL){
		return this->beginElement(size,new kmb::ElementContainerMap());
	}
	else if(0==strcmp(containerType,kmb::ElementContainerMap::CONTAINER_TYPE) ){
		return this->beginElement(size,new kmb::ElementContainerMap());
	}
	else if(0==strcmp(containerType,kmb::ElementContainerArray::CONTAINER_TYPE) ){
		return this->beginElement(size,new kmb::ElementContainerArray(size));
	}
	else if(0==strcmp(containerType,kmb::ElementContainerTriangleArray::CONTAINER_TYPE) ){
		return this->beginElement(size,new kmb::ElementContainerTriangleArray(size));
	}
	else{
		return this->beginElement(size,new kmb::ElementContainerMap());
	}
}

kmb::elementIdType
kmb::MeshData::addElement(kmb::elementType type,kmb::nodeIdType *ary)
{
	if( this->currentBody && ary ){
		return this->currentBody->addElement( type, ary );
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::MeshData::addElementWithId(kmb::elementType type,kmb::nodeIdType *ary,kmb::elementIdType elementId)
{

	if( this->currentBody && ary ){
		return this->currentBody->addElement(type,ary,elementId);
	}
	return kmb::Element::nullElementId;
}

void
kmb::MeshData::endElement(void)
{
	if( this->currentBody ){
		kmb::elementIdType maxId = this->currentBody->getMaxId();
		if( maxElementId < maxId ){
			maxElementId = maxId;
		}
		this->bodies.push_back( this->currentBody );
		this->currentBody = NULL;
	}
}

kmb::elementIdType
kmb::MeshData::getMaxElementId() const
{
	return maxElementId;
}

kmb::elementIdType
kmb::MeshData::insertElement(kmb::bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary)
{
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	kmb::ElementContainer* body = this->getBodyPtr( bodyID );
	if( body && ary ){
		elementId = body->addElement(type,ary);
	}
	return elementId;
}

kmb::elementIdType
kmb::MeshData::insertElementWithId(bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary,elementIdType elementId)
{

	kmb::ElementContainer* body = this->getBodyPtr( bodyID );
	if( body && ary ){
		return body->addElement(type,ary,elementId);
	}
	return kmb::Element::nullElementId;
}

bool
kmb::MeshData::moveElement(kmb::bodyIdType bodyId0,kmb::bodyIdType bodyId1,kmb::elementIdType elementId)
{
	kmb::ElementContainer* body0 = this->getBodyPtr(bodyId0);
	kmb::ElementContainer* body1 = this->getBodyPtr(bodyId1);
	if( body0==NULL || body1==NULL || body0==body1 ){
		return false;
	}
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	kmb::nodeIdType nodes[20];
	if( body0->getElement(elementId,etype,nodes) &&
		body0->deleteElement(elementId) &&
		body1->addElement(etype,nodes,elementId) == elementId )
	{
		return true;
	}
	return false;
}

kmb::bodyIdType
kmb::MeshData::getBodyCount(void) const
{
	return static_cast< kmb::bodyIdType >( this->bodies.size() );
}

kmb::bodyIdType
kmb::MeshData::appendBody( kmb::Body* body, bool offset )
{
	if( body == NULL )
		return kmb::Body::nullBodyId;
	if( offset ){
		body->setOffsetId( this->maxElementId + 1 );
	}
	if( this->maxElementId  < body->getMaxId() ){
		this->maxElementId = body->getMaxId();
	}

	this->bodies.push_back( body );
	return static_cast<bodyIdType>(this->bodies.size() - 1);
}

kmb::bodyIdType
kmb::MeshData::replaceBody( kmb::bodyIdType bodyId, kmb::Body* body, bool offset )
{
	kmb::Body* orgBody = getBodyPtr( bodyId );
	if( body == NULL || orgBody == NULL ){
		return kmb::Body::nullBodyId;
	}else{
		if( offset ){
			body->setOffsetId( this->getMaxElementId() + 1 );
		}
		if( this->maxElementId  < body->getMaxId() ){
			this->maxElementId = body->getMaxId();
		}
		delete orgBody;
		orgBody = NULL;
		this->bodies[ bodyId ] = body;
		return bodyId;
	}
}

bool
kmb::MeshData::updateBody( kmb::bodyIdType bodyId )
{
	kmb::Body* body = getBodyPtr( bodyId );
	if( body == NULL ){
		return false;
	}else{
		if( this->maxElementId  < body->getMaxId() ){
			this->maxElementId = body->getMaxId();
		}
		return true;
	}
}

void
kmb::MeshData::clearBody(bodyIdType bodyId)
{
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		body->clear();
	}
}

void
kmb::MeshData::removeAllBodies(void)
{
	std::vector<kmb::ElementContainer*>::iterator bIter = bodies.begin();
	while( bIter != this->bodies.end() ){
		kmb::ElementContainer* body = *bIter;
		if(body){
			delete body;
			body = NULL;
		}
		++bIter;
	}
	this->bodies.clear();
	if( this->currentBody ){
		delete this->currentBody;
		this->currentBody = NULL;
	}
}

const kmb::Body*
kmb::MeshData::getBodyPtr(kmb::bodyIdType bodyId) const
{
	if( 0 <= bodyId && bodyId < static_cast<kmb::bodyIdType>(this->getBodyCount()) ){
		return this->bodies[bodyId];
	}else{
		return NULL;
	}
}

kmb::Body*
kmb::MeshData::getBodyPtr(kmb::bodyIdType bodyId)
{
	if( 0 <= bodyId && bodyId < static_cast<kmb::bodyIdType>(this->getBodyCount()) ){
		return this->bodies[bodyId];
	}else{
		return NULL;
	}
}

size_t
kmb::MeshData::getElementCount(kmb::bodyIdType bodyId) const
{
	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		return body->getCount();
	}else{
		return 0;
	}
}

size_t
kmb::MeshData::getElementCount(void) const
{
	size_t elementCount = 0;
	const kmb::bodyIdType len = this->getBodyCount();
	const kmb::Body* body = NULL;
	for(kmb::bodyIdType bodyId=0; bodyId<len; ++bodyId)
	{
		body = this->getBodyPtr( bodyId );
		if( body != NULL ){
			elementCount += body->getCount();
		}
	}
	return elementCount;
}

size_t
kmb::MeshData::getElementCountOfDim(int dim) const
{
	size_t elementCount = 0;
	const kmb::bodyIdType len = this->getBodyCount();
	const kmb::Body* body = NULL;
	for(kmb::bodyIdType bodyId=0; bodyId<len; ++bodyId)
	{
		body = this->getBodyPtr( bodyId );
		if( body != NULL && body->getDimension()==dim ){
			elementCount += body->getCount();
		}
	}
	return elementCount;
}

int
kmb::MeshData::getDimension(kmb::bodyIdType bodyId) const
{
	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		return body->getDimension();
	}
	return -1;
}

int
kmb::MeshData::getDegree(kmb::bodyIdType bodyId) const
{
	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		return body->getDegree();
	}
	return -1;
}

bool
kmb::MeshData::isUniqueElementType(kmb::bodyIdType bodyId,kmb::elementType etype) const
{
	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		return body->isUniqueType( etype );
	}
	return false;
}

kmb::ElementContainer::const_iterator
kmb::MeshData::findElement(kmb::elementIdType elementId,kmb::bodyIdType bodyId) const
{
	const kmb::Body* body = this->getBodyPtr( bodyId );
	if( body != NULL ){
		return body->find( elementId );
	}else{
		const size_t len = this->getBodyCount();
		for(size_t i=0;i<len;++i)
		{
			body = this->getBodyPtr( static_cast<kmb::bodyIdType>(i) );
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

const char*
kmb::MeshData::getBodyName(bodyIdType bodyId) const
{
	const kmb::Body* body = this->getBodyPtr(bodyId);
	if( body )
		return body->getBodyName();
	else
		return NULL;
}

void
kmb::MeshData::setBodyName(bodyIdType bodyId,const char* name)
{
	kmb::Body* body = this->getBodyPtr(bodyId);
	if( body ){
		body->setBodyName( name );
	}
}

const char*
kmb::MeshData::getElementContainerType(bodyIdType bodyId) const
{
	const kmb::Body* body = this->getBodyPtr(bodyId);
	if( body )
		return body->getContainerType();
	else
		return NULL;
}

const std::multimap< std::string, kmb::DataBindings* >
kmb::MeshData::getDataBindingsMap(void) const
{
	return bindings;
}

kmb::DataBindings*
kmb::MeshData::createDataBindings
(const char* name,kmb::DataBindings::bindingMode bmode,PhysicalValue::valueType vtype,const char* stype,kmb::bodyIdType targetBodyId)
{
	return NULL;
}

size_t
kmb::MeshData::getDataCount(const char* stype) const
{
	if( stype == NULL ){
		return this->bindings.size();
	}else{
		size_t count = 0;
		std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = this->bindings.begin();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = this->bindings.end();
		while( dIter != dEnd ){
			kmb::DataBindings* binding = dIter->second;
			if( binding != NULL &&
				binding->getSpecType() == stype )
			{
				++count;
			}
			++dIter;
		}
		return count;
	}
}

bool
kmb::MeshData::hasData(const char* name,const char* stype) const
{
	return ( this->getDataBindingsPtr(name,stype) != NULL );
}

bool
kmb::MeshData::renameData(const char* oldname,const char* newname,const char* stype)
{
	std::multimap< std::string, kmb::DataBindings* >::iterator bIter = this->bindings.lower_bound(oldname);
	std::multimap< std::string, kmb::DataBindings* >::iterator end = this->bindings.upper_bound(oldname);
	while( bIter != end ){
		if( stype == NULL || bIter->second->getSpecType().compare(stype) == 0 ){
			kmb::DataBindings* data = bIter->second;
			if( data != NULL && this->getDataBindingsPtr(newname,data->getSpecType().c_str()) == NULL ){
				this->bindings.erase( bIter );
				this->bindings.insert( std::multimap< std::string, kmb::DataBindings* >::value_type( newname, data ) );
				return true;
			}else{
				return false;
			}
		}
		++bIter;
	}
	return false;
}

bool
kmb::MeshData::deleteData(const char* name,const char* stype)
{
	std::multimap< std::string, kmb::DataBindings* >::iterator bIter = this->bindings.lower_bound(name);
	std::multimap< std::string, kmb::DataBindings* >::iterator end = this->bindings.upper_bound(name);
	while( bIter != end ){
		kmb::DataBindings* data = bIter->second;
		if( stype == NULL || data->getSpecType().compare(stype) == 0 ){
			delete data;
			this->bindings.erase(bIter);
			return true;
		}
		++bIter;
	}
	return false;
}

bool
kmb::MeshData::clearData(const char* name,const char* stype)
{
	std::multimap< std::string, kmb::DataBindings* >::iterator bIter = this->bindings.lower_bound(name);
	std::multimap< std::string, kmb::DataBindings* >::iterator end = this->bindings.upper_bound(name);
	while( bIter != end ){
		kmb::DataBindings* data = bIter->second;
		if( stype == NULL || data->getSpecType().compare(stype) == 0 ){
			data->clear();
			return true;
		}
		++bIter;
	}
	return false;
}

bool
kmb::MeshData::replaceData(const kmb::DataBindings* olddata, kmb::DataBindings* newdata, const char* name,const char* stype)
{
	if( olddata == NULL || newdata == NULL ){
		return false;
	}
	std::multimap< std::string, kmb::DataBindings* >::iterator bIter = this->bindings.lower_bound(name);
	std::multimap< std::string, kmb::DataBindings* >::iterator end = this->bindings.upper_bound(name);
	while( bIter != end ){
		kmb::DataBindings* data = bIter->second;
		if( ( stype == NULL || stype[0] == '\0' || data->getSpecType().compare(stype) == 0 ) && data == olddata ){
			delete data;
			bIter->second = newdata;
			return true;
		}
		++bIter;
	}
	return false;
}

void
kmb::MeshData::setDefaultSpecType(const char* specType)
{
	if( specType ){
		this->defaultSpecType = specType;
	}else{
		this->defaultSpecType.clear();
	}
}

bool
kmb::MeshData::setDataSpecType(const char* name,const char* oldstype,const char* newstype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,oldstype);
	if( data && newstype ){
		data->setSpecType( newstype );
		return true;
	}
	return false;
}

size_t
kmb::MeshData::getIdCount(const char* name,const char* stype) const
{
	const kmb::DataBindings* data = getDataBindingsPtr(name,stype);
	if( data != NULL )
	{
		return data->getIdCount();
	}
	return 0;
}

void
kmb::MeshData::appendTargetData(const char* name, const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr( name, stype );
	if( data ){
		this->bindingsStack.push_back( data );
		switch( data->getValueType() ){
			case kmb::PhysicalValue::SCALAR:
				this->bindingsStackDim += 1;
				break;
			case kmb::PhysicalValue::VECTOR3:
				this->bindingsStackDim += 3;
				break;
			case kmb::PhysicalValue::TENSOR6:
				this->bindingsStackDim += 6;
				break;
			case kmb::PhysicalValue::POINT3VECTOR3:
				this->bindingsStackDim += 6;
				break;
			default:
				break;
		}
	}
}

void
kmb::MeshData::clearTargetData(void)
{
	this->bindingsStack.clear();
	this->bindingsStackDim = 0;
}

int
kmb::MeshData::getTargetDataNum(void)
{
	return static_cast<int>(this->bindingsStack.size());
}

int
kmb::MeshData::getTargetDataDim(void)
{
	return this->bindingsStackDim;
}

void
kmb::MeshData::setMultiPhysicalValues(kmb::idType id, std::vector< kmb::PhysicalValue* > &values)
{
	std::vector< kmb::DataBindings* >::iterator dIter = bindingsStack.begin();
	int i=0;
	while( dIter != bindingsStack.end() )
	{
		kmb::DataBindings* data = *dIter;
		kmb::PhysicalValue* v = values[i];
		if( data && v ){
			data->setPhysicalValue(id,v);
		}
		++i;
		++dIter;
	}
}

void
kmb::MeshData::setMultiPhysicalValues(kmb::idType id, double* values)
{
	std::vector< kmb::DataBindings* >::iterator dIter = bindingsStack.begin();
	int i=0;
	while( dIter != bindingsStack.end() )
	{
		kmb::DataBindings* data = *dIter;
		if( data ){
			switch( data->getValueType() ){
				case kmb::PhysicalValue::SCALAR:
					{
						data->setPhysicalValue(id,&values[i]);
						++i;
						break;
					}
				case kmb::PhysicalValue::VECTOR3:
					{
						data->setPhysicalValue(id,&values[i]);
						i += 3;
						break;
					}
				case kmb::PhysicalValue::TENSOR6:
					{
						data->setPhysicalValue(id,&values[i]);
						i += 6;
						break;
					}
				case kmb::PhysicalValue::POINT3VECTOR3:
					{
						data->setPhysicalValue(id,&values[i]);
						i += 6;
						break;
					}
				default:
					break;
			}
		}
		++dIter;
	}
}

void
kmb::MeshData::setMultiPhysicalValues(kmb::idType id, long* values)
{
	std::vector< kmb::DataBindings* >::iterator dIter = bindingsStack.begin();
	int i=0;
	while( dIter != bindingsStack.end() )
	{
		kmb::DataBindings* data = *dIter;
		if( data ){
			switch( data->getValueType() ){
				case kmb::PhysicalValue::INTEGER:
				case kmb::PhysicalValue::NODEID:
				case kmb::PhysicalValue::ELEMENTID:
					{
						data->setPhysicalValue(id,&values[i]);
						++i;
						break;
					}
				default:
					break;
			}
		}
		++dIter;
	}
}

void
kmb::MeshData::setMultiPhysicalValues(double* values)
{
	std::vector< kmb::DataBindings* >::iterator dIter = bindingsStack.begin();
	int i=0;
	while( dIter != bindingsStack.end() )
	{
		kmb::DataBindings* data = *dIter;
		kmb::PhysicalValue* v = NULL;
		if( data ){
			switch( data->getValueType() ){
				case kmb::PhysicalValue::SCALAR:
					{
						v = new kmb::ScalarValue(values[i]);
						++i;
						break;
					}
				case kmb::PhysicalValue::VECTOR3:
					{
						v = new kmb::Vector3Value(values[i],values[i+1],values[i+2]);
						i += 3;
						break;
					}
				case kmb::PhysicalValue::TENSOR6:
					{
						v = new kmb::Tensor6Value(values[i],values[i+1],values[i+2],values[i+3],values[i+4],values[i+5]);
						i += 6;
						break;
					}
				case kmb::PhysicalValue::POINT3VECTOR3:
					{
						v = new kmb::Point3Vector3Value(values[i],values[i+1],values[i+2],values[i+3],values[i+4],values[i+5]);
						i += 6;
						break;
					}
				default:
					break;
			}
			data->setPhysicalValue(v);
		}
		++dIter;
	}
}

void
kmb::MeshData::deriveTargetData(kmb::elementIdType elementId,kmb::elementIdType orgElementId)
{
	std::vector< kmb::DataBindings* >::iterator dIter = bindingsStack.begin();
	double v[6];
	while( dIter != bindingsStack.end() )
	{
		kmb::DataBindings* data = *dIter;
		if( data ){
			switch( data->getBindingMode() ){
				case kmb::DataBindings::ELEMENTGROUP:
					if( data->hasId( orgElementId ) ){
						data->addId( elementId );
					}
					break;
				case kmb::DataBindings::ELEMENTVARIABLE:
					if( data->getPhysicalValue( orgElementId, v ) ){
						data->setPhysicalValue( elementId, v );
					}
					break;
				default:
					break;
			}
		}
		++dIter;
	}
}

kmb::DataBindings*
kmb::MeshData::getDataBindingsPtr(const char* name,const char* stype)
{
	if( name == NULL ){
		return NULL;
	}
	if( stype == NULL && this->defaultSpecType.size() > 0 ){
		stype = this->defaultSpecType.c_str();
	}
	std::multimap< std::string, kmb::DataBindings* >::iterator bIter = this->bindings.lower_bound(name);
	std::multimap< std::string, kmb::DataBindings* >::iterator end = this->bindings.upper_bound(name);
	while( bIter != end ){
		if( stype == NULL || bIter->second->getSpecType().compare(stype) == 0){
			return bIter->second;
		}
		++bIter;
	}
	return NULL;
}

const kmb::DataBindings*
kmb::MeshData::getDataBindingsPtr(const char* name,const char* stype) const
{
	if( name == NULL ){
		return NULL;
	}
	if( stype == NULL && this->defaultSpecType.size() > 0 ){
		stype = this->defaultSpecType.c_str();
	}
	std::multimap< std::string, kmb::DataBindings* >::const_iterator bIter = this->bindings.lower_bound(name);
	std::multimap< std::string, kmb::DataBindings* >::const_iterator end = this->bindings.upper_bound(name);
	while( bIter != end ){
		if( stype == NULL || bIter->second->getSpecType().compare(stype) == 0){
			return bIter->second;
		}
		++bIter;
	}
	return NULL;
}

bool
kmb::MeshData::setDataBindingsPtr(const char* name,kmb::DataBindings* data,const char* stype)
{
	if( data == NULL || name == NULL || getDataBindingsPtr( name, stype ) != NULL ){
		return false;
	}
	if( stype ){
		data->setSpecType( stype );
	}
	this->bindings.insert( std::multimap< std::string, kmb::DataBindings* >::value_type(name,data) );
	return true;
}

kmb::DataBindings::bindingMode
kmb::MeshData::getDataMode(const char* name, const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		return data->getBindingMode();
	}else{
		return kmb::DataBindings::UNDEFINED;
	}
}

kmb::PhysicalValue::valueType
kmb::MeshData::getDataValueType(const char* name, const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		return data->getValueType();
	}else{
		return kmb::PhysicalValue::UNDEFINED;
	}
}

std::string
kmb::MeshData::getDataSpecType(const char* name) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name);
	if( data ){
		return data->getSpecType();
	}else{
		return "";
	}
}

void
kmb::MeshData::addId(const char* name,kmb::idType id,const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		data->addId(id);
	}
}

void
kmb::MeshData::addId(const char* name,kmb::Face f,const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		data->addId(f);
	}
}

void
kmb::MeshData::setPhysicalValue(const char* name,kmb::PhysicalValue* val,const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data && val && data->getValueType() == val->getType() ){
		data->setPhysicalValue(val);
	}else{
		if( val ){
			delete val;
		}
	}
}

void
kmb::MeshData::setPhysicalValueAtId(const char* name,kmb::idType id,kmb::PhysicalValue* val,const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data && val && data->getValueType() == val->getType() ){
		data->setPhysicalValue(id,val);
	}else{
		if( val ){
			delete val;
		}
	}
}

void
kmb::MeshData::setPhysicalValueAtId(const char* name,kmb::Face f,kmb::PhysicalValue* val,const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data && val && data->getValueType() == val->getType() ){
		data->setPhysicalValue(f,val);
	}else{
		if( val ){
			delete val;
		}
	}
}

const kmb::PhysicalValue*
kmb::MeshData::getPhysicalValue(const char* name,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		return data->getPhysicalValue();
	}else{
		return NULL;
	}
}

const kmb::PhysicalValue*
kmb::MeshData::getPhysicalValueAtId(const char* name,kmb::idType id,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		return data->getPhysicalValue(id);
	}else{
		return NULL;
	}
}

const kmb::PhysicalValue*
kmb::MeshData::getPhysicalValueAtId(const char* name,kmb::Face f,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		return data->getPhysicalValue(f);
	}else{
		return NULL;
	}
}

bool
kmb::MeshData::getPhysicalValueAtId(const char* name,kmb::idType id,double* val,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		return data->getPhysicalValue(id,val);
	}else{
		return false;
	}
}

bool
kmb::MeshData::getPhysicalValueAtId(const char* name,kmb::Face f,double *val,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data ){
		return data->getPhysicalValue(f,val);
	}else{
		return false;
	}
}

kmb::bodyIdType
kmb::MeshData::faceGroupToBody(const char* name,const char* stype)
{
	kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	kmb::ElementContainer* elements = NULL;
	if( data && data->getBindingMode() == kmb::DataBindings::FACEGROUP &&
		( elements = this->getBodyPtr(data->getTargetBodyId()) ) != NULL )
	{
		kmb::ElementContainerMap* body = new kmb::ElementContainerMap();
		kmb::Face f;
		kmb::elementType etype;
		kmb::nodeIdType* nodes = new kmb::nodeIdType[kmb::Element::MAX_NODE_COUNT_DIM2];
		kmb::DataBindings::iterator dIter = data->begin();
		while( !dIter.isFinished() ){
			if( dIter.getFace(f) ){
				kmb::ElementContainer::iterator elem = elements->find( f.getElementId() );
				etype = elem.getBoundaryElement( f.getLocalFaceId(), nodes );
				body->addElement( etype, nodes );
			}
			++dIter;
		}
		delete[] nodes;
		return appendBody( body );
	}else{
		return kmb::Body::nullBodyId;
	}
}
void
kmb::MeshData::getNodeSetFromDataBindings(std::set<kmb::nodeIdType>&nodeSet,const char* name,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if(data != NULL){
		kmb::bodyIdType bodyId = data->getTargetBodyId();
		switch( data->getBindingMode() ){
			case kmb::DataBindings::BODYVARIABLE:
			case kmb::DataBindings::BODYGROUP:
			{
				kmb::DataBindings::const_iterator dIter = data->begin();
				while( !dIter.isFinished() ){
					kmb::bodyIdType bId = static_cast< kmb::bodyIdType >( dIter.getId() );
					const kmb::ElementContainer* body = this->getBodyPtr( bId );
					if( body != NULL ){
						kmb::ElementContainer::const_iterator eIter = body->begin();
						while( eIter != body->end() )
						{
							int len = eIter.getNodeCount();
							for(int i=0;i<len;++i){
								nodeSet.insert( eIter.getCellId(i) );
							}
							++eIter;
						}
					}
					++dIter;
				}
				break;
			}
			case kmb::DataBindings::ELEMENTVARIABLE:
			case kmb::DataBindings::ELEMENTGROUP:
			{
				kmb::DataBindings::const_iterator dIter = data->begin();
				while( !dIter.isFinished() ){
					kmb::elementIdType elementId = static_cast< kmb::elementIdType >( dIter.getId() );
					kmb::ElementContainer::const_iterator element = this->findElement( elementId, bodyId );
					int len = element.getNodeCount();
					for(int i=0;i<len;++i){
						nodeSet.insert( element.getCellId(i) );
					}
					++dIter;
				}
				break;
			}
			case kmb::DataBindings::NODEVARIABLE:
			case kmb::DataBindings::NODEGROUP:
			{
				kmb::DataBindings::const_iterator dIter = data->begin();
				while( !dIter.isFinished() ){
					kmb::nodeIdType nodeId = static_cast< kmb::nodeIdType >( dIter.getId() );
					nodeSet.insert( nodeId );
					++dIter;
				}
				break;
			}
			case kmb::DataBindings::FACEVARIABLE:
			case kmb::DataBindings::FACEGROUP:
			{
				kmb::DataBindings::const_iterator dIter = data->begin();
				kmb::Face f;
				while( !dIter.isFinished() ){
					if( dIter.getFace( f ) ){
						kmb::ElementContainer::const_iterator element = this->findElement( f.getElementId(), bodyId );
						if( !element.isFinished() ){
							const int localId = f.getLocalFaceId();
							const int boudaryNodeCount = element.getBoundaryNodeCount( localId );
							for(int i=0;i<boudaryNodeCount;++i){
								nodeSet.insert( element.getBoundaryCellId(localId,i) );
							}
						}
					}
					++dIter;
				}
				break;
			}
			default:
				break;
		}
	}
}

