/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainer                                        #
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
#include <climits>
#include <cstring>
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementContainerArray.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbElementContainerMArray.h"
#include "MeshDB/kmbElementContainerMixedArray.h"
#ifdef REVOCAP_SUPPORT_DUMP_CONTAINER
#include "MeshDB/kmbElementContainerMixedOnDisk.h"
#endif
#include "MeshDB/kmbElementContainerTriangleArray.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTetrahedron.h"
#include "Common/kmbCalculator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const kmb::ElementContainer::iterator kmb::ElementContainer::endIterator;
const kmb::ElementContainer::const_iterator kmb::ElementContainer::endConstIterator;
const kmb::bodyIdType kmb::ElementContainer::nullBodyId = -1;

bool
kmb::ElementContainer::copy(const kmb::ElementContainer* org, kmb::ElementContainer* elements)
{
	if( org == NULL || elements == NULL ){
		return false;
	}
	bool ret = true;
	elements->initialize( org->getCount() );
	kmb::nodeIdType* nodes = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	kmb::elementType etype;
	kmb::ElementContainer::const_iterator eIter = org->begin();
	while( !eIter.isFinished() && ret ){
		ret &= eIter.getElement( etype ,nodes );
		if( ret ){
			elements->addElement( etype, nodes, eIter.getId() );
		}
		++eIter;
	}
	delete[] nodes;
	return ret;
}

kmb::ElementContainer*
kmb::ElementContainer::createContainer( const char* containerType, size_t size )
{
	if( containerType == NULL ){
		return NULL;
	}
	if(0==strcmp(containerType,kmb::ElementContainerMap::CONTAINER_TYPE) ){
		return new kmb::ElementContainerMap();
	}
	else if(0==strcmp(containerType,kmb::ElementContainerArray::CONTAINER_TYPE) ){
		return new kmb::ElementContainerArray(size);
	}
	else if(0==strcmp(containerType,kmb::ElementContainerMArray::CONTAINER_TYPE) ){
		return new kmb::ElementContainerMArray(size);
	}
	else if(0==strcmp(containerType,kmb::ElementContainerMixedArray::CONTAINER_TYPE) ){
		return new kmb::ElementContainerMixedArray(size);
	}
#ifdef REVOCAP_SUPPORT_DUMP_CONTAINER
	else if(0==strcmp(containerType,kmb::ElementContainerMixedOnDisk::CONTAINER_TYPE) ){
		return new kmb::ElementContainerMixedOnDisk(static_cast<unsigned long>(size));
	}
#endif
	else if(0==strcmp(containerType,kmb::ElementContainerTriangleArray::CONTAINER_TYPE) ){
		return new kmb::ElementContainerTriangleArray(size);
	}
	else{
		return NULL;
	}
}

kmb::ElementContainer::ElementContainer(void)
: offsetId(0)
{
	for(int i=0; i<kmb::ELEMENT_TYPE_NUM; ++i )
	{
		typeCounter[i] = 0;
	}
}

kmb::ElementContainer::~ElementContainer(void)
{
	clear();
}

void
kmb::ElementContainer::clear(void)
{
	for(int i=0; i<kmb::ELEMENT_TYPE_NUM; ++i )
	{
		this->typeCounter[i] = 0;
	}
	this->boundingBox.initialize();
	this->bodyName.clear();
	this->offsetId = 0;
}

kmb::elementIdType
kmb::ElementContainer::addElement(const kmb::Element* element)
{
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	if( element ){
		kmb::elementType etype = element->getType();
		const int nodeCount = element->getNodeCount();
		kmb::nodeIdType* cell = new kmb::nodeIdType[nodeCount];
		for(int i=0;i<nodeCount;++i){
			cell[i] = element->getCellId(i);
		}
		elementId = this->addElement( etype, cell );
		delete[] cell;
		delete element;
	}
	return elementId;
}

kmb::elementIdType
kmb::ElementContainer::addElement(const kmb::Element* element,const kmb::elementIdType id)
{
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	if( element ){
		kmb::elementType etype = element->getType();
		const int nodeCount = element->getNodeCount();
		kmb::nodeIdType* cell = new kmb::nodeIdType[nodeCount];
		for(int i=0;i<nodeCount;++i){
			cell[i] = element->getCellId(i);
		}
		elementId = this->addElement( etype, cell, id );
		delete[] cell;
		delete element;
	}
	return elementId;
}

void
kmb::ElementContainer::updateBoundingBox(const kmb::Point3DContainer* points)
{
	if( points == NULL ) return;
	kmb::ElementContainer::iterator eIter = this->begin();
	kmb::Point3D point;
	while( !eIter.isFinished() ){
		const int len = eIter.getNodeCount();
		for(int i=0;i<len;++i){
			if( points->getPoint( eIter.getCellId(i), point ) ){
				boundingBox.update( point );
			}
		}
		++eIter;
	}
}

kmb::Element*
kmb::ElementContainer::eraseElement(kmb::elementIdType id){
	return NULL;
}


kmb::elementIdType
kmb::ElementContainer::getOffsetId(void) const
{
	return this->offsetId;
}

void kmb::ElementContainer::setOffsetId(kmb::elementIdType id)
{
	this->offsetId = id;
}

size_t
kmb::ElementContainer::getCountByType(kmb::elementType elementType) const
{
	if(0 <= elementType && elementType < kmb::ELEMENT_TYPE_NUM){
		return this->typeCounter[ static_cast<int>(elementType) ];
	}else{
		return 0;
	}
}

void kmb::ElementContainer::getCountOfDim(int count[3]) const
{
	count[0] = 0;
	count[1] = 0;
	count[2] = 0;

	for(int i=0; i<kmb::ELEMENT_TYPE_NUM; ++i )
	{
		int dim = kmb::Element::getDimension( static_cast<kmb::elementType>(i) );
		if( 0 < dim && dim <= 3){
			count[ dim-1 ] += static_cast<int>( this->typeCounter[ i ] );
		}
	}
}

int kmb::ElementContainer::getDimension(void) const
{
	int count[3];
	getCountOfDim(count);
	if( count[0] > 0 && count[1] == 0 && count[2] == 0 ){
		return 1;
	}else if( count[0] == 0 && count[1] > 0 && count[2] == 0 ){
		return 2;
	}else if( count[0] == 0 && count[1] == 0 && count[2] > 0 ){
		return 3;
	}else{
		return -1;
	}
}

bool kmb::ElementContainer::isUniqueDim(int dim) const
{

	for(int i=0; i<kmb::ELEMENT_TYPE_NUM; ++i )
	{
		if( kmb::Element::getDimension( static_cast<kmb::elementType>(i) ) != dim &&
			this->typeCounter[ i ] > 0){
			return false;
		}
	}
	return true;
}

int
kmb::ElementContainer::getDegree(void) const
{
	int count[3] = {0,0,0};

	for(int i=0; i<kmb::ELEMENT_TYPE_NUM; ++i )
	{
		int deg = kmb::Element::getDegree( static_cast<kmb::elementType>(i) );
		count[ deg ] += static_cast<int>( this->typeCounter[ i ] );
	}
	if( count[0] > 0 && count[1] == 0 && count[2] == 0 ){
		return 0;
	}else if( count[0] == 0 && count[1] > 0 && count[2] == 0 ){
		return 1;
	}else if( count[0] == 0 && count[1] == 0 && count[2] > 0 ){
		return 2;
	}else{
		return -1;
	}
}

bool
kmb::ElementContainer::isUniqueType(kmb::elementType type) const
{
	return ( this->typeCounter[ static_cast<int>(type) ] == this->getCount() );
}

void
kmb::ElementContainer::getNodesOfBody(const kmb::Point3DContainer* points,kmb::Point3DContainer* result) const
{
	kmb::Point3D point;
	if( points != NULL && result != NULL )
	{
		kmb::ElementContainer::const_iterator eIter = this->begin();
		while( eIter != this->end() )
		{
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i)
			{
				const kmb::nodeIdType id = eIter.getCellId(i);
				if( points->getPoint( id, point ) ){

					result->addPoint( point, id );
				}
			}
			++eIter;
		}
	}
}

void
kmb::ElementContainer::getNodesOfBody(std::set<kmb::nodeIdType> &result) const
{
	kmb::ElementContainer::const_iterator eIter = this->begin();
	while( eIter != this->end() )
	{
		const int len = eIter.getNodeCount();
		for(int i=0;i<len;++i)
		{
			const kmb::nodeIdType id = eIter.getCellId(i);
			result.insert( id );
		}
		++eIter;
	}
}

void
kmb::ElementContainer::getMaxPoint(kmb::Point3D& pt) const
{
	pt.x( boundingBox.getMax().x() );
	pt.y( boundingBox.getMax().y() );
	pt.z( boundingBox.getMax().z() );
}

void
kmb::ElementContainer::getMinPoint(kmb::Point3D& pt) const
{
	pt.x( boundingBox.getMin().x() );
	pt.y( boundingBox.getMin().y() );
	pt.z( boundingBox.getMin().z() );
}

const kmb::BoundingBox&
kmb::ElementContainer::getBoundingBox(void) const
{
	return this->boundingBox;
}

void
kmb::ElementContainer::setBoundingBox(const kmb::Point3D& max,const kmb::Point3D& min)
{
	boundingBox.initialize();
	boundingBox.update( max );
	boundingBox.update( min );
}

void
kmb::ElementContainer::setBodyName(const char* name)
{
	if( name != NULL ){
		this->bodyName = name;
	}
}

const char*
kmb::ElementContainer::getBodyName(void) const
{
	return this->bodyName.c_str();
}



kmb::ElementContainer::iterator::iterator( const kmb::ElementContainer::iterator &other )
: _iter(NULL)
, offsetId(0)
{
	if( other._iter != NULL ){
		_iter = other._iter->clone();
		offsetId = other.offsetId;
	}
}

kmb::ElementContainer::iterator::~iterator(void)
{
	if( _iter != NULL ){
		delete _iter;
	}
	_iter = NULL;
}

kmb::elementIdType
kmb::ElementContainer::iterator::getId(void) const
{
	if( _iter != NULL ){
		return _iter->getId() + this->offsetId;
	}else{
		return kmb::Element::nullElementId;
	}
}

kmb::Element*
kmb::ElementContainer::iterator::getElement(void) const
{
	if( _iter != NULL ){
		return _iter->getElement();
	}else{
		return NULL;
	}
}

bool
kmb::ElementContainer::iterator::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	if( _iter != NULL ){
		etype = _iter->getType();
		int nodeCount = kmb::Element::getNodeCount( etype );
		for(int i=0;i<nodeCount;++i){
			nodes[i] = _iter->getCellId(i);
		}
		return true;
	}else{
		return false;
	}
}

bool
kmb::ElementContainer::iterator::getElement(kmb::Element &element) const
{
	if( _iter != NULL ){
		if( element.getType() == _iter->getType() ){
			const int len = element.getNodeCount();
			for(int i=0;i<len;++i){
				element.setCellId(i,_iter->getCellId(i));
			}
			return true;
		}
	}
	return false;
}

kmb::elementType
kmb::ElementContainer::iterator::getType(void) const
{
	if( _iter != NULL ){
		return _iter->getType();
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::ElementContainer::iterator::getCellId(int cellIndex) const
{
	if( _iter != NULL ){
		return _iter->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::ElementContainer::iterator::setCellId(int cellIndex,kmb::nodeIdType nodeId)
{
	if( _iter != NULL ){
		return _iter->setCellId(cellIndex,nodeId);
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::ElementContainer::iterator::operator[](const int i) const
{
	if( _iter != NULL ){
		return _iter->getCellId(i);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::iterator&
kmb::ElementContainer::iterator::operator=(const kmb::ElementContainer::iterator& other)
{
	if( _iter != NULL && _iter != other._iter )
	{
		delete _iter;
		_iter = NULL;
		offsetId = 0;
	}


	if( other._iter ){
		_iter = other._iter->clone();
	}
	offsetId = other.offsetId;
	return *this;
}


kmb::ElementContainer::iterator&
kmb::ElementContainer::iterator::operator++(void)
{
	if( _iter != NULL ){
		kmb::ElementContainer::_iterator* retVal = ++(*_iter);
		if( retVal == NULL ){
			if( _iter ){
				delete _iter;
			}
			_iter = NULL;
		}
	}
	return *this;
}


kmb::ElementContainer::iterator
kmb::ElementContainer::iterator::operator++(int n)
{
	if( _iter != NULL ){
		kmb::ElementContainer::_iterator* _itClone = _iter->clone();
		kmb::ElementContainer::_iterator* retVal = (*_iter)++;
		if( retVal == NULL ){
			if( _iter ) delete _iter;
			_iter = NULL;
		}
		return kmb::ElementContainer::iterator(_itClone,this->offsetId);
	}else{
		return kmb::ElementContainer::iterator(NULL);
	}
}

bool
kmb::ElementContainer::iterator::operator==(const kmb::ElementContainer::iterator& other) const
{
	return ( _iter == other._iter );
}

bool
kmb::ElementContainer::iterator::operator!=(const kmb::ElementContainer::iterator& other) const
{
	return ( _iter != other._iter );
}



kmb::ElementContainer::const_iterator::const_iterator( const kmb::ElementContainer::const_iterator &other )
: _iter(NULL)
, offsetId(0)
{
	if( other._iter != NULL ){
		_iter = other._iter->clone();
		offsetId = other.offsetId;
	}
}

kmb::ElementContainer::const_iterator::const_iterator( const kmb::ElementContainer::iterator &other )
: _iter(NULL)
, offsetId(0)
{
	if( other._iter != NULL ){
		_iter = other._iter->clone();
		offsetId = other.offsetId;
	}
}

kmb::ElementContainer::const_iterator::~const_iterator(void)
{
	if( _iter != NULL ){
		delete _iter;
	}
	_iter = NULL;
}

kmb::elementIdType
kmb::ElementContainer::const_iterator::getId(void) const
{
	if( _iter != NULL ){
		return _iter->getId() + this->offsetId;
	}else{
		return kmb::Element::nullElementId;
	}
}

const kmb::Element*
kmb::ElementContainer::const_iterator::getElement(void) const
{
	if( _iter != NULL ){
		return _iter->getElement();
	}else{
		return NULL;
	}
}

bool
kmb::ElementContainer::const_iterator::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	if( _iter != NULL ){
		etype = _iter->getType();
		int nodeCount = kmb::Element::getNodeCount( etype );
		for(int i=0;i<nodeCount;++i){
			nodes[i] = _iter->getCellId(i);
		}
		return true;
	}else{
		return false;
	}
}

bool
kmb::ElementContainer::const_iterator::getElement(kmb::Element &element) const
{
	if( _iter != NULL ){
		if( element.getType() == _iter->getType() ){
			const int len = element.getNodeCount();
			for(int i=0;i<len;++i){
				element.setCellId(i,_iter->getCellId(i));
			}
			return true;
		}
	}
	return false;
}

kmb::elementType
kmb::ElementContainer::const_iterator::getType(void) const
{
	if( _iter != NULL ){
		return _iter->getType();
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::ElementContainer::const_iterator::getCellId(int cellIndex) const
{
	if( _iter != NULL ){
		return _iter->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::ElementContainer::const_iterator::setCellId(int cellIndex,kmb::nodeIdType nodeId)
{
	return false;
}

kmb::nodeIdType
kmb::ElementContainer::const_iterator::operator[](const int i) const
{
	if( _iter != NULL ){
		return _iter->getCellId(i);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::const_iterator&
kmb::ElementContainer::const_iterator::operator=(const kmb::ElementContainer::const_iterator& other)
{
	if( _iter != NULL && _iter != other._iter )
	{
		delete _iter;
		_iter = NULL;
		offsetId = 0;
	}


	if( other._iter ){
		_iter = other._iter->clone();
		offsetId = other.offsetId;
	}
	return *this;
}

kmb::ElementContainer::const_iterator&
kmb::ElementContainer::const_iterator::operator=(const kmb::ElementContainer::iterator& other)
{
	if( _iter != NULL && _iter != other._iter )
	{
		delete _iter;
		_iter = NULL;
		offsetId = 0;
	}


	if( other._iter ){
		_iter = other._iter->clone();
		offsetId = other.offsetId;
	}
	return *this;
}


kmb::ElementContainer::const_iterator&
kmb::ElementContainer::const_iterator::operator++(void)
{
	if( _iter != NULL ){
		kmb::ElementContainer::_iterator* retVal = ++(*_iter);
		if( retVal == NULL ){
			if( _iter ){
				delete _iter;
			}
			_iter = NULL;
		}
	}
	return *this;
}


kmb::ElementContainer::const_iterator
kmb::ElementContainer::const_iterator::operator++(int n)
{
	if( _iter != NULL ){
		kmb::ElementContainer::_iterator* _itClone = _iter->clone();
		kmb::ElementContainer::_iterator* retVal = (*_iter)++;
		if( retVal == NULL ){
			if( _iter ) delete _iter;
			_iter = NULL;
		}
		return kmb::ElementContainer::const_iterator(_itClone,this->offsetId);
	}else{
		return kmb::ElementContainer::const_iterator(NULL);
	}
}

bool
kmb::ElementContainer::const_iterator::operator==(const kmb::ElementContainer::const_iterator& other) const
{
	return ( _iter == other._iter );
}

bool
kmb::ElementContainer::const_iterator::operator!=(const kmb::ElementContainer::const_iterator& other) const
{
	return ( _iter != other._iter );
}

