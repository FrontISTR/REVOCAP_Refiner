/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerMap                                     #
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
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTetrahedron.h"
#include "Common/kmbCalculator.h"
#include <limits.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::ElementContainerMap::CONTAINER_TYPE = "stl::map";

kmb::ElementContainerMap::ElementContainerMap(void)
: kmb::ElementContainer()
, minId(INT_MAX)
, maxId(-1)
{
}

kmb::ElementContainerMap::~ElementContainerMap(void)
{
	clear();
}

void kmb::ElementContainerMap::initialize(size_t size)
{
	clear();
}

const char*
kmb::ElementContainerMap::getContainerType() const
{
	return kmb::ElementContainerMap::CONTAINER_TYPE;
}

kmb::elementIdType
kmb::ElementContainerMap::addElement(kmb::Element* element)
{
	if( element != NULL ){
		++maxId;
		if( minId > maxId ){
			minId = maxId;
		}
		this->elements.insert( std::pair<kmb::elementIdType,kmb::Element*>(maxId,element) );
		++(this->typeCounter[ element->getType() ]);
		return offsetId + maxId;
	}else{
		return kmb::Element::nullElementId;
	}
}

kmb::elementIdType
kmb::ElementContainerMap::addElement(kmb::Element* element, kmb::elementIdType id)
{
	if( id >= offsetId && elements.find(id-offsetId) == elements.end() && element != NULL ){
		if( maxId < id - offsetId ){
			maxId = id - offsetId;
		}
		if( minId > id - offsetId ){
			minId = id - offsetId;
		}
		elements.insert( std::pair<kmb::elementIdType,kmb::Element*>(id-offsetId,element) );
		++(this->typeCounter[ element->getType() ]);
		return id;
	}else{
		return kmb::Element::nullElementId;
	}
}

kmb::elementIdType
kmb::ElementContainerMap::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	int nodeCount = kmb::Element::getNodeCount(etype);
	kmb::nodeIdType* nodeTable = new kmb::nodeIdType[nodeCount];

	for(int i=0;i<nodeCount;++i)
	{
		nodeTable[i] = nodes[i];
	}
	kmb::Element* element = kmb::Element::create(etype,nodeTable);
	if( element != NULL ){
		++maxId;
		if( minId > maxId ){
			minId = maxId;
		}
		this->elements.insert( std::pair<kmb::elementIdType,kmb::Element*>(maxId,element) );
		++(this->typeCounter[ element->getType() ]);
		return offsetId + maxId;
	}else{
		return kmb::Element::nullElementId;
	}
}

kmb::elementIdType
kmb::ElementContainerMap::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	if( id >= 0 && elements.find(id-offsetId) == elements.end() ){
		int nodeCount = kmb::Element::getNodeCount(etype);
		kmb::nodeIdType* nodeTable = new kmb::nodeIdType[nodeCount];
		for(int i=0;i<nodeCount;++i)
		{
			nodeTable[i] = nodes[i];
		}
		kmb::Element* element = kmb::Element::create(etype,nodeTable);
		if( element ){
			elements.insert( std::pair<kmb::elementIdType,kmb::Element*>(id-offsetId,element) );
			if( maxId < id - offsetId ){
				maxId = id - offsetId;
			}
			if( minId > id - offsetId ){
				minId = id - offsetId;
			}
			++(this->typeCounter[ etype ]);
			return id;
		}else{
			delete[] nodeTable;
			return kmb::Element::nullElementId;
		}
	}else{
		return kmb::Element::nullElementId;
	}
}

kmb::Element*
kmb::ElementContainerMap::getElementPtr(kmb::elementIdType id) const
{
	std::map< kmb::elementIdType, kmb::Element* >::const_iterator it = this->elements.find(id-offsetId);
	if( it != this->elements.end() ){
		return it->second;
	}else{
		return NULL;
	}
}

bool
kmb::ElementContainerMap::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	kmb::Element* element = getElementPtr(id);
	if( element ){
		etype = element->getType();
		const int len = element->getNodeCount();
		for(int i=0;i<len;++i){
			nodes[i] = element->getCellId(i);
		}
		return true;
	}
	return false;
}

kmb::Element*
kmb::ElementContainerMap::eraseElement(kmb::elementIdType id)
{
	std::map< kmb::elementIdType, kmb::Element* >::iterator it = this->elements.find(id-offsetId);
	if( it != this->elements.end() ){
		kmb::Element* element = it->second;
		if( element != NULL ){
			this->typeCounter[ element->getType() ]--;
		}
		elements.erase( it );
		return element;
	}else{
		return NULL;
	}
}

bool
kmb::ElementContainerMap::deleteElement(const kmb::elementIdType id)
{
	std::map< kmb::elementIdType, kmb::Element* >::iterator it = this->elements.find(id-offsetId);
	if( it != this->elements.end() ){
		kmb::Element* element = it->second;
		if( element != NULL ){
			this->typeCounter[ element->getType() ]--;
			delete element;
		}
		elements.erase( it );
		return true;
	}else{
		return false;
	}
}

bool
kmb::ElementContainerMap::includeElement(const kmb::elementIdType id) const
{
	return ( this->elements.find(id-offsetId) != this->elements.end() );
}

kmb::elementIdType
kmb::ElementContainerMap::getMaxId(void) const
{
	return this->maxId + this->offsetId;
}

kmb::elementIdType
kmb::ElementContainerMap::getMinId(void) const
{
	return this->minId + this->offsetId;
}

size_t
kmb::ElementContainerMap::getCount(void) const
{
	return static_cast<int>( this->elements.size() );
}

void
kmb::ElementContainerMap::clear(void)
{
	kmb::ElementContainer::clear();
	std::map< kmb::elementIdType, kmb::Element* >::iterator eIter = this->elements.begin();
	while( eIter != this->elements.end() )
	{
		kmb::Element* element = eIter->second;
		if( element != NULL ){
			delete element;
			eIter->second = NULL;
		}
		++eIter;
	}
	this->elements.clear();
	minId = INT_MAX;
	maxId = -1;
}

kmb::ElementContainer::iterator
kmb::ElementContainerMap::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerMap::_iteratorMap* _it = NULL;
	_it = new kmb::ElementContainerMap::_iteratorMap();
	_it->elementIter = this->elements.begin();
	_it->endIterator = this->elements.end();
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMap::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerMap::_iteratorMap* _it = NULL;
	_it = new kmb::ElementContainerMap::_iteratorMap();
	_it->elementIter = this->elements.begin();
	_it->endIterator = this->elements.end();
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerMap::find(kmb::elementIdType id)
{
	std::map< kmb::elementIdType, kmb::Element* >::iterator it = this->elements.find(id-offsetId);
	if( it == this->elements.end() ){
		return kmb::ElementContainer::iterator(NULL);
	}else{
		kmb::ElementContainerMap::_iteratorMap* _it = NULL;
		_it = new kmb::ElementContainerMap::_iteratorMap();
		_it->elementIter = it;
		_it->endIterator = this->elements.end();
		return kmb::ElementContainer::iterator(_it,offsetId);
	}
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMap::find(kmb::elementIdType id) const
{
	std::map< kmb::elementIdType, kmb::Element* >::const_iterator it = this->elements.find(id-offsetId);
	if( it == this->elements.end() ){
		return kmb::ElementContainer::const_iterator(NULL);
	}else{
		kmb::ElementContainerMap::_iteratorMap* _it = NULL;
		_it = new kmb::ElementContainerMap::_iteratorMap();
		_it->elementIter = it;
		_it->endIterator = this->elements.end();
		return kmb::ElementContainer::const_iterator(_it,offsetId);
	}
}



kmb::ElementContainerMap::_iteratorMap::_iteratorMap(void)
{
}

kmb::ElementContainerMap::_iteratorMap::~_iteratorMap(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMap::_iteratorMap::clone(void)
{
	kmb::ElementContainerMap::_iteratorMap* obj = NULL;
	obj = new kmb::ElementContainerMap::_iteratorMap();
	obj->elementIter = this->elementIter;
	obj->endIterator = this->endIterator;
	return obj;
}

kmb::elementIdType
kmb::ElementContainerMap::_iteratorMap::getId(void) const
{
	return this->elementIter->first;
}

kmb::Element*
kmb::ElementContainerMap::_iteratorMap::getElement(void)
{
	return this->elementIter->second;
}

const kmb::Element*
kmb::ElementContainerMap::_iteratorMap::getElement(void) const
{
	return this->elementIter->second;
}

bool
kmb::ElementContainerMap::_iteratorMap::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	etype = this->elementIter->second->getType();
	int nodeCount = this->elementIter->second->getNodeCount();
	for(int i=0;i<nodeCount;++i){
		nodes[i] = this->elementIter->second->getCellId(i);
	}
	return true;
}

kmb::elementType
kmb::ElementContainerMap::_iteratorMap::getType(void) const
{
	if( this->elementIter->second != NULL ){
		return this->elementIter->second->getType();
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::ElementContainerMap::_iteratorMap::getCellId(int cellIndex) const
{
	if( this->elementIter->second != NULL ){
		return this->elementIter->second->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::ElementContainerMap::_iteratorMap::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	if( this->elementIter->second != NULL ){
		const int nodeCount = this->elementIter->second->getNodeCount();
		if( 0 <= cellIndex && cellIndex < nodeCount ){
			return this->elementIter->second->setCellId(cellIndex,nodeId);
		}
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::ElementContainerMap::_iteratorMap::operator[](int cellIndex) const
{
	if( this->elementIter->second != NULL ){
		return this->elementIter->second->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMap::_iteratorMap::operator++(void)
{
	++elementIter;
	if( elementIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMap::_iteratorMap::operator++(int n)
{
	elementIter++;
	if( elementIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

