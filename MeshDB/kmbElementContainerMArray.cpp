/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerMArray                                  #
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
#include "MeshDB/kmbElementContainerMArray.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::ElementContainerMArray::CONTAINER_TYPE = "element_marray";

kmb::ElementContainerMArray::ElementContainerMArray(size_t size)
: kmb::ElementContainer()
, elementArray()
, aIndex()
, count(0)
{
	initialize( size );
}

kmb::ElementContainerMArray::~ElementContainerMArray(void)
{
	clear();
}

void kmb::ElementContainerMArray::initialize(size_t size)
{
	clear();
	if( size <= 0 ){
		size = 0x1000U;
	}else if( size > 0x100000U ){
		size = 0x100000U;
	}
	elementArray.initialize(size);
	aIndex = elementArray.getBLArrayIndex(0);
}

void
kmb::ElementContainerMArray::clear(void)
{
	kmb::ElementContainer::clear();

	elementArray.clear();
	aIndex = elementArray.getBLArrayIndex(0);
	count = 0;
}

const char*
kmb::ElementContainerMArray::getContainerType() const
{
	return kmb::ElementContainerMArray::CONTAINER_TYPE;
}

kmb::elementIdType
kmb::ElementContainerMArray::addElement(kmb::Element* element)
{
	if( element != NULL ){
		elementArray.set( aIndex, element );
		++(this->typeCounter[ element->getType() ]);
		++aIndex;
		++count;
		return static_cast< kmb::elementIdType >( offsetId + aIndex.getIndex() - 1 );
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::ElementContainerMArray::addElement(kmb::Element* element, kmb::elementIdType id)
{
	if( element != NULL && 0 <= id - offsetId ){
		size_t i = static_cast<unsigned int>(id - offsetId);
		kmb::BLArrayIndex ind = elementArray.getBLArrayIndex(i);
		if( elementArray.set(ind,element) ){
			aIndex = ind;
			++(this->typeCounter[ element->getType() ]);
			++aIndex;
			++count;
			return id;
		}
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::ElementContainerMArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
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
kmb::ElementContainerMArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
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
kmb::ElementContainerMArray::getElementPtr(kmb::elementIdType id) const
{
	if( 0 <= id - offsetId ){
		size_t i = static_cast<unsigned int>(id - offsetId);
		return elementArray.get(i);
	}
	return NULL;
}

bool
kmb::ElementContainerMArray::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	if( 0 <= id - offsetId ){
		size_t i = static_cast<unsigned int>(id - offsetId);
		kmb::Element* element = elementArray.get(i);
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
kmb::ElementContainerMArray::eraseElement(kmb::elementIdType id)
{
	if( 0 <= id - offsetId ){
		size_t i = static_cast<unsigned int>(id - offsetId);
		kmb::Element* element = elementArray.get(i);
		if( element != NULL ){
			--(this->typeCounter[ element->getType() ]);
			--count;
			elementArray.set(i,NULL);
			return element;
		}
	}
	return NULL;
}

bool
kmb::ElementContainerMArray::deleteElement(const kmb::elementIdType id)
{
	if( 0 <= id - offsetId ){
		size_t i = static_cast<unsigned int>(id - offsetId);
		kmb::Element* element = elementArray.get(i);
		if( element != NULL ){
			--(this->typeCounter[ element->getType() ]);
			--count;
			elementArray.set(i,NULL);
			delete element;
			return true;
		}
	}
	return false;
}

bool
kmb::ElementContainerMArray::includeElement(const kmb::elementIdType id) const
{
	if( 0 <= id - offsetId ){
		size_t i = static_cast<unsigned int>(id - offsetId);
		if( elementArray.get(i) != NULL ){
			return true;
		}
	}
	return false;
}

kmb::elementIdType
kmb::ElementContainerMArray::getMaxId(void) const
{
	return static_cast< kmb::elementIdType >( this->offsetId + aIndex.getIndex() - 1 );
}

kmb::elementIdType
kmb::ElementContainerMArray::getMinId(void) const
{
	return this->offsetId;
}

size_t
kmb::ElementContainerMArray::getCount(void) const
{
	return count;
}

kmb::ElementContainer::iterator
kmb::ElementContainerMArray::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerMArray::_iteratorMArray* _it = NULL;
	_it = new kmb::ElementContainerMArray::_iteratorMArray();
	if( _it ){
		_it->elementContainer = this;

		this->elementArray.first( _it->aIndex );
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMArray::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerMArray::_iteratorMArray* _it = NULL;
	_it = new kmb::ElementContainerMArray::_iteratorMArray();
	if( _it ){
		_it->elementContainer = const_cast<kmb::ElementContainerMArray*>(this);

		this->elementArray.first( _it->aIndex );
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerMArray::find(kmb::elementIdType id)
{
	if( 0 <= id - offsetId ){
		kmb::BLArrayIndex ind = this->elementArray.getBLArrayIndex( static_cast<size_t>(id - offsetId) );
		if( this->elementArray.has(ind) ){
			kmb::ElementContainerMArray::_iteratorMArray* _it = NULL;
			_it = new kmb::ElementContainerMArray::_iteratorMArray();
			if( _it ){
				_it->elementContainer = this;
				_it->aIndex = ind;
			}
			return kmb::ElementContainer::iterator(_it,offsetId);
		}
	}
	return kmb::ElementContainer::iterator(NULL);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMArray::find(kmb::elementIdType id) const
{
	if( 0 <= id - offsetId ){
		kmb::BLArrayIndex ind = this->elementArray.getBLArrayIndex( static_cast<size_t>(id - offsetId) );
		if( this->elementArray.has(ind) ){
			kmb::ElementContainerMArray::_iteratorMArray* _it = NULL;
			_it = new kmb::ElementContainerMArray::_iteratorMArray();
			if( _it ){
				_it->elementContainer = const_cast<kmb::ElementContainerMArray*>(this);
				_it->aIndex = ind;
			}
			return kmb::ElementContainer::iterator(_it,offsetId);
		}
	}
	return kmb::ElementContainer::const_iterator(NULL);
}



kmb::ElementContainerMArray::_iteratorMArray::_iteratorMArray(void)
: kmb::ElementContainer::_iterator()
, elementContainer(NULL)
, aIndex()
{
}

kmb::ElementContainerMArray::_iteratorMArray::~_iteratorMArray(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMArray::_iteratorMArray::clone(void)
{
	kmb::ElementContainerMArray::_iteratorMArray* obj = NULL;
	obj = new kmb::ElementContainerMArray::_iteratorMArray();
	if( obj ){
		obj->elementContainer = this->elementContainer;
		obj->aIndex = this->aIndex;
	}
	return obj;
}

kmb::elementIdType
kmb::ElementContainerMArray::_iteratorMArray::getId(void) const
{
	return static_cast< kmb::elementIdType >( aIndex.getIndex() );
}

kmb::Element*
kmb::ElementContainerMArray::_iteratorMArray::getElement(void)
{
	return this->elementContainer->elementArray.get( aIndex );
}

const kmb::Element*
kmb::ElementContainerMArray::_iteratorMArray::getElement(void) const
{
	return this->elementContainer->elementArray.get( aIndex );
}

bool
kmb::ElementContainerMArray::_iteratorMArray::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	kmb::Element* element = this->elementContainer->elementArray.get( aIndex );
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
kmb::ElementContainerMArray::_iteratorMArray::getType(void) const
{
	kmb::Element* element = this->elementContainer->elementArray.get( aIndex );
	if( element ){
		return element->getType();
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::ElementContainerMArray::_iteratorMArray::getCellId(int cellIndex) const
{
	kmb::Element* element = this->elementContainer->elementArray.get( aIndex );
	if( element ){
		return element->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::ElementContainerMArray::_iteratorMArray::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	kmb::Element* element = this->elementContainer->elementArray.get( aIndex );
	if( element ){
		const int nodeCount = element->getNodeCount();
		if( 0 <= cellIndex && cellIndex < nodeCount ){
			return element->setCellId(cellIndex,nodeId);
		}
	}
	return false;
}

kmb::nodeIdType
kmb::ElementContainerMArray::_iteratorMArray::operator[](int cellIndex) const
{
	kmb::Element* element = this->elementContainer->elementArray.get( aIndex );
	if( element ){
		return element->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMArray::_iteratorMArray::operator++(void)
{
	do{
		++aIndex;
		if( elementContainer->elementArray.has(aIndex) ){
			return this;
		}
	}while( elementContainer->elementArray.valid(aIndex) );
	return NULL;
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMArray::_iteratorMArray::operator++(int n)
{
	do{
		aIndex++;
		if( elementContainer->elementArray.has(aIndex) ){
			return this;
		}
	}while( elementContainer->elementArray.valid(aIndex) );
	return NULL;
}
