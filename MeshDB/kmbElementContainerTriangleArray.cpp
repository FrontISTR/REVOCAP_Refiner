/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerTriangleArray                           #
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
#include "MeshDB/kmbElementContainerTriangleArray.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::ElementContainerTriangleArray::CONTAINER_TYPE = "triangle_array";
unsigned int kmb::ElementContainerTriangleArray::unsignedNullNodeId = static_cast<unsigned int>(kmb::nullNodeId);

kmb::ElementContainerTriangleArray::ElementContainerTriangleArray(size_t size)
: ElementContainer()
, index(0)
, size(0)
, nodeTable(NULL)
{
	initialize(size);
}

kmb::ElementContainerTriangleArray::~ElementContainerTriangleArray(void)
{
	clear();
}

void
kmb::ElementContainerTriangleArray::clear(void)
{
	kmb::ElementContainer::clear();

	if( nodeTable != NULL ){
		delete[] nodeTable;
		nodeTable = NULL;
	}
	size = 0;
	index = 0;
}

void
kmb::ElementContainerTriangleArray::initialize(size_t size)
{
	clear();
	nodeTable = new unsigned int[3*size];
	this->size = size;
	this->index = 0;
	for(size_t i=0;i<3*size;++i){
		nodeTable[i] = unsignedNullNodeId;
	}
}

kmb::elementIdType
kmb::ElementContainerTriangleArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( nodeTable == NULL || nodes == NULL || etype != kmb::TRIANGLE ){
		return kmb::Element::nullElementId;
	}
	while( index < size && nodeTable[3*index] != unsignedNullNodeId ){
		++index;
	}
	if( index < size ){
		for(int i=0;i<3;++i){
			nodeTable[3*index+i] = nodes[i];
		}
		++(this->typeCounter[ kmb::TRIANGLE ]);
		++index;
		return static_cast< kmb::elementIdType >( offsetId + index - 1 );
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::ElementContainerTriangleArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	if( nodeTable == NULL || nodes == NULL || etype != kmb::TRIANGLE ){
		return kmb::Element::nullElementId;
	}
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable[3*i] == unsignedNullNodeId ){
		for(int j=0;j<3;++j){
			nodeTable[3*i+j] = nodes[j];
		}
		++(this->typeCounter[ kmb::TRIANGLE ]);
		return id;
	}
	return kmb::Element::nullElementId;
}

bool
kmb::ElementContainerTriangleArray::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[3*i] != unsignedNullNodeId ){
		etype = kmb::TRIANGLE;
		for(int j=0;j<3;++j){
			nodes[j] = nodeTable[3*i+j];
		}
		return true;
	}
	return false;
}

bool
kmb::ElementContainerTriangleArray::deleteElement(const kmb::elementIdType id)
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[3*i] != unsignedNullNodeId ){
		for(int j=0;j<3;++j){
			nodeTable[3*i+j] = unsignedNullNodeId;
		}
		--(this->typeCounter[ kmb::TRIANGLE ]);
		return true;
	}
	return false;
}

bool
kmb::ElementContainerTriangleArray::includeElement(const kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable[3*i] != unsignedNullNodeId ){
		return true;
	}else{
		return false;
	}
}

kmb::elementIdType
kmb::ElementContainerTriangleArray::getMaxId(void) const
{
	return static_cast< kmb::elementIdType >( this->offsetId + this->size - 1 );
}

kmb::elementIdType
kmb::ElementContainerTriangleArray::getMinId(void) const
{
	return this->offsetId;
}

size_t
kmb::ElementContainerTriangleArray::getCount(void) const
{
	return this->typeCounter[ kmb::TRIANGLE ];
}

const char*
kmb::ElementContainerTriangleArray::getContainerType() const
{
	return kmb::ElementContainerTriangleArray::CONTAINER_TYPE;
}

kmb::ElementContainer::iterator
kmb::ElementContainerTriangleArray::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerTriangleArray::_iteratorTA* _it = NULL;
	_it = new kmb::ElementContainerTriangleArray::_iteratorTA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerTriangleArray::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerTriangleArray::_iteratorTA* _it = NULL;
	_it = new kmb::ElementContainerTriangleArray::_iteratorTA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerTriangleArray::find(kmb::elementIdType id)
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[3*i] != unsignedNullNodeId ){
		kmb::ElementContainerTriangleArray::_iteratorTA* _it = NULL;
		_it = new kmb::ElementContainerTriangleArray::_iteratorTA();
		if( _it ){
			_it->elementContainer = this;
			_it->index = i;
		}
		return kmb::ElementContainer::iterator(_it,offsetId);
	}else{
		return kmb::ElementContainer::iterator(NULL);
	}
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerTriangleArray::find(kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[3*i] != unsignedNullNodeId ){
		kmb::ElementContainerTriangleArray::_iteratorTA* _it = NULL;
		_it = new kmb::ElementContainerTriangleArray::_iteratorTA();
		if( _it ){
			_it->elementContainer = this;
			_it->index = i;
		}
		return kmb::ElementContainer::const_iterator(_it,offsetId);
	}else{
		return kmb::ElementContainer::const_iterator(NULL);
	}
}



kmb::ElementContainerTriangleArray::_iteratorTA::_iteratorTA(void)
: index(0)
, elementContainer(NULL)
{
}

kmb::ElementContainerTriangleArray::_iteratorTA::~_iteratorTA(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerTriangleArray::_iteratorTA::clone(void)
{
	kmb::ElementContainerTriangleArray::_iteratorTA* obj = NULL;
	obj = new kmb::ElementContainerTriangleArray::_iteratorTA();
	obj->elementContainer = this->elementContainer;
	obj->index = this->index;
	return obj;
}

kmb::elementIdType
kmb::ElementContainerTriangleArray::_iteratorTA::getId(void) const
{
	return static_cast< kmb::elementIdType >( index );
}

bool
kmb::ElementContainerTriangleArray::_iteratorTA::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	if( elementContainer->nodeTable[ 3*index ] != unsignedNullNodeId ){
		etype = kmb::TRIANGLE;
		for(int i=0;i<3;++i){
			nodes[i] = elementContainer->nodeTable[ 3*index+i ];
		}
		return true;
	}
	return false;
}

kmb::elementType
kmb::ElementContainerTriangleArray::_iteratorTA::getType(void) const
{
	if( elementContainer->nodeTable[ 3*index ] != unsignedNullNodeId ){
		return kmb::TRIANGLE;
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::ElementContainerTriangleArray::_iteratorTA::getCellId(int cellIndex) const
{
	if( 0 <= cellIndex && cellIndex < 3 && elementContainer->nodeTable[ 3*index ] != unsignedNullNodeId ){
		return elementContainer->nodeTable[ 3*index+cellIndex ];
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::ElementContainerTriangleArray::_iteratorTA::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	if( 0 <= cellIndex && cellIndex < 3 && elementContainer->nodeTable[ 3*index ] != unsignedNullNodeId ){
		elementContainer->nodeTable[ 3*index+cellIndex ] = nodeId;
		return true;
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::ElementContainerTriangleArray::_iteratorTA::operator[](const int i) const
{
	if( 0 <= i && i < 3 && elementContainer->nodeTable[ 3*index ] != unsignedNullNodeId ){
		return elementContainer->nodeTable[ 3*index+i ];
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerTriangleArray::_iteratorTA::operator++(void)
{
	while( ++index < elementContainer->size ){
		if( elementContainer->nodeTable[ 3*index ] != unsignedNullNodeId ){
			return this;
		}
	}
	return NULL;
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerTriangleArray::_iteratorTA::operator++(int n)
{
	index++;
	while( index < elementContainer->size ){
		if( elementContainer->nodeTable[ 3*index ] != unsignedNullNodeId ){
			return this;
		}
		index++;
	}
	return NULL;
}

