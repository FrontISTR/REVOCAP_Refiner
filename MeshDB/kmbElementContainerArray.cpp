/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerArray                                   #
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
#include "MeshDB/kmbElementContainerArray.h"
#include "Common/kmbCalculator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::ElementContainerArray::CONTAINER_TYPE = "element_array";

kmb::ElementContainerArray::ElementContainerArray(size_t size)
: kmb::ElementContainer()
, index(0)
, size(0)
, count(0)
, elementArray(NULL)
{
	initialize( size );
}

kmb::ElementContainerArray::~ElementContainerArray(void)
{
	clear();
}

void kmb::ElementContainerArray::initialize(size_t size)
{
	clear();
	this->size = size;
	elementArray = new kmb::Element*[size];
	for(size_t i = 0;i<size;++i){
		elementArray[i] = NULL;
	}
}

const char*
kmb::ElementContainerArray::getContainerType() const
{
	return kmb::ElementContainerArray::CONTAINER_TYPE;
}

kmb::elementIdType
kmb::ElementContainerArray::addElement(kmb::Element* element)
{
	if( element != NULL ){
		while( index < size && elementArray[index] != NULL ){
			++index;
		}
		if( index < size ){
			elementArray[index] = element;
			++(this->typeCounter[ element->getType() ]);
			++count;
			++index;
			return static_cast< kmb::elementIdType >( offsetId + index - 1 );
		}
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::ElementContainerArray::addElement(kmb::Element* element, kmb::elementIdType id)
{
	if( element != NULL ){
		int i = id - offsetId;
		if( i < static_cast<int>(size) && elementArray[i] == NULL ){
			elementArray[i] = element;
			++(this->typeCounter[ element->getType() ]);
			++count;
			return static_cast< kmb::elementIdType >( id );
		}
	}
	return kmb::Element::nullElementId;
}

/*
kmb::elementIdType
kmb::ElementContainerArray::addElement(kmb::elementType etype,...)
{
	int nodeCount = kmb::Element::getNodeCount(etype);
	kmb::nodeIdType* nodes = new kmb::nodeIdType[nodeCount];

	va_list args;
	va_start(args,etype);
	for(int i=0;i<nodeCount;++i)
	{
		nodes[i] = va_arg(args,kmb::nodeIdType);
	}
	va_end(args);

	return addElement( kmb::Element::create(etype,nodes) );
}
*/

kmb::elementIdType
kmb::ElementContainerArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( nodes == NULL ){
		return kmb::Element::nullElementId;
	}
	int nodeCount = kmb::Element::getNodeCount(etype);
	kmb::nodeIdType* nodeTable = new kmb::nodeIdType[nodeCount];

	for(int i=0;i<nodeCount;++i)
	{
		nodeTable[i] = nodes[i];
	}

	kmb::Element* element = kmb::Element::create(etype,nodeTable);
	if( element != NULL ){
		kmb::elementIdType elementId = addElement( element );
		if( elementId != kmb::Element::nullElementId ){
			return elementId;
		}else{
			delete element;
		}
	}else{
		delete[] nodeTable;
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::ElementContainerArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	if( nodes == NULL ){
		return kmb::Element::nullElementId;
	}
	int nodeCount = kmb::Element::getNodeCount(etype);
	kmb::nodeIdType* nodeTable = new kmb::nodeIdType[nodeCount];

	for(int i=0;i<nodeCount;++i)
	{
		nodeTable[i] = nodes[i];
	}
	kmb::Element* element = kmb::Element::create(etype,nodeTable);
	if( element != NULL ){
		kmb::elementIdType elementId = addElement( element, id );
		if( elementId != kmb::Element::nullElementId ){
			return elementId;
		}else{
			delete element;
		}
	}else{
		delete[] nodeTable;
	}
	return kmb::Element::nullElementId;
}

kmb::Element*
kmb::ElementContainerArray::getElementPtr(kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) ){
		return elementArray[i];
	}
	return NULL;
}

bool
kmb::ElementContainerArray::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) ){
		kmb::Element* element = elementArray[i];
		if( element ){
			etype = element->getType();
			const int len = element->getNodeCount();
			for(int j=0;j<len;++j){
				nodes[j] = element->getCellId(j);
			}
			return true;
		}
	}
	return false;
}

kmb::Element*
kmb::ElementContainerArray::eraseElement(kmb::elementIdType id)
{
	int i = id - offsetId;
	if( 0<= i && i < static_cast<int>(size) ){
		kmb::Element* element = elementArray[i];
		if( element != NULL ){
			--(this->typeCounter[ element->getType() ]);
			--count;
			elementArray[i] = NULL;
			return element;
		}
	}
	return NULL;
}

bool
kmb::ElementContainerArray::deleteElement(const kmb::elementIdType id)
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) ){
		kmb::Element* element = elementArray[i];
		if( element != NULL ){
			--(this->typeCounter[ element->getType() ]);
			--count;
			elementArray[i] = NULL;
			delete element;
			return true;
		}
	}
	return false;
}

bool
kmb::ElementContainerArray::includeElement(const kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && elementArray[i] != NULL ){
		return true;
	}else{
		return false;
	}
}

kmb::elementIdType
kmb::ElementContainerArray::getMaxId(void) const
{
	return static_cast< kmb::elementIdType >( this->offsetId + this->size - 1 );
}

kmb::elementIdType
kmb::ElementContainerArray::getMinId(void) const
{
	return this->offsetId;
}

size_t
kmb::ElementContainerArray::getCount(void) const
{
	return count;
}

void
kmb::ElementContainerArray::clear(void)
{
	kmb::ElementContainer::clear();

	if( elementArray ){
		for(size_t i=0;i<size;++i){
			if( elementArray[i] != NULL ){
				delete elementArray[i];
				elementArray[i] = NULL;
			}
		}
		delete[] elementArray;
		elementArray = NULL;
	}
	count = 0;
	index = 0;
	size = 0;
}

kmb::ElementContainer::iterator
kmb::ElementContainerArray::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerArray::_iteratorArray* _it = NULL;
	_it = new kmb::ElementContainerArray::_iteratorArray();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerArray::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerArray::_iteratorArray* _it = NULL;
	_it = new kmb::ElementContainerArray::_iteratorArray();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerArray::find(kmb::elementIdType id)
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && elementArray[i] != NULL ){
		kmb::ElementContainerArray::_iteratorArray* _it = NULL;
		_it = new kmb::ElementContainerArray::_iteratorArray();
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
kmb::ElementContainerArray::find(kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && elementArray[i] != NULL ){
		kmb::ElementContainerArray::_iteratorArray* _it = NULL;
		_it = new kmb::ElementContainerArray::_iteratorArray();
		if( _it ){
			_it->elementContainer = this;
			_it->index = i;
		}
		return kmb::ElementContainer::const_iterator(_it,offsetId);
	}else{
		return kmb::ElementContainer::const_iterator(NULL);
	}
}



kmb::ElementContainerArray::_iteratorArray::_iteratorArray(void)
: index(0)
, elementContainer(NULL)
{
}

kmb::ElementContainerArray::_iteratorArray::~_iteratorArray(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerArray::_iteratorArray::clone(void)
{
	kmb::ElementContainerArray::_iteratorArray* obj = NULL;
	obj = new kmb::ElementContainerArray::_iteratorArray();
	obj->elementContainer = this->elementContainer;
	obj->index = this->index;
	return obj;
}

kmb::elementIdType
kmb::ElementContainerArray::_iteratorArray::getId(void) const
{
	return static_cast< kmb::elementIdType >( index );
}

kmb::Element*
kmb::ElementContainerArray::_iteratorArray::getElement(void)
{
	return this->elementContainer->elementArray[ index ];
}

const kmb::Element*
kmb::ElementContainerArray::_iteratorArray::getElement(void) const
{
	return this->elementContainer->elementArray[ index ];
}

bool
kmb::ElementContainerArray::_iteratorArray::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	kmb::Element* element = this->elementContainer->elementArray[ index ];
	if( element ){
		etype = element->getType();
		int nodeCount = element->getNodeCount();
		for(int i=0;i<nodeCount;++i){
			nodes[i] = element->getCellId(i);
		}
		return true;
	}
	return false;
}

kmb::elementType
kmb::ElementContainerArray::_iteratorArray::getType(void) const
{
	kmb::Element* element = this->elementContainer->elementArray[ index ];
	if( element ){
		return element->getType();
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::ElementContainerArray::_iteratorArray::getCellId(int cellIndex) const
{
	kmb::Element* element = this->elementContainer->elementArray[ index ];
	if( element ){
		return element->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::ElementContainerArray::_iteratorArray::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	kmb::Element* element = this->elementContainer->elementArray[ index ];
	if( element ){
		const int nodeCount = element->getNodeCount();
		if( 0 <= cellIndex && cellIndex < nodeCount ){
			return element->setCellId(cellIndex,nodeId);
		}
	}
	return false;
}

kmb::nodeIdType
kmb::ElementContainerArray::_iteratorArray::operator[](const int cellIndex) const
{
	kmb::Element* element = this->elementContainer->elementArray[ index ];
	if( element ){
		return element->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}


kmb::ElementContainer::_iterator*
kmb::ElementContainerArray::_iteratorArray::operator++(void)
{
	while( ++index < elementContainer->size ){
		if( elementContainer->elementArray[index] != NULL ){
			return this;
		}
	}
	return NULL;
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerArray::_iteratorArray::operator++(int n)
{
	index++;
	while( index < elementContainer->size ){
		if( elementContainer->elementArray[index] != NULL ){
			return this;
		}
		index++;
	}
	return NULL;
}

