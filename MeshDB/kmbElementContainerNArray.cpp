/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerNArray                                  #
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
#include "MeshDB/kmbElementContainerNArray.h"
#include "MeshDB/kmbElement.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#pragma warning(disable:4996)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::ElementContainerNArray::CONTAINER_TYPE = "node_array";

kmb::ElementContainerNArray::ElementContainerNArray( kmb::elementType etype, size_t size )
: ElementContainerDirectAccessable()
, index(0)
, size(0)
, etype(kmb::UNKNOWNTYPE)
, ncount(0)
, nodeTable(NULL)
, nodeTableDeletable(true)
, nodeOffset(0)
{
	this->etype = etype;
	this->size = size;
	ncount = kmb::Element::getNodeCount( etype );
	initialize( size );
}

#define _DEBUG_ 1

kmb::ElementContainerNArray::ElementContainerNArray( kmb::elementType etype, size_t size, kmb::nodeIdType *nodeTable, bool writable, kmb::nodeIdType offset )
: ElementContainerDirectAccessable()
, index(0)
, size(0)
, etype(kmb::UNKNOWNTYPE)
, ncount(0)
, nodeTable(NULL)
, nodeTableDeletable(true)
, nodeOffset(0)
{
	this->etype = etype;
	this->size = size;
	this->ncount = kmb::Element::getNodeCount( etype );


	if( nodeTable ){
		this->nodeTable = nodeTable;
		this->nodeTableDeletable = false;
		if( writable ){
			std::fill( this->nodeTable, this->nodeTable+(ncount*size), kmb::nullNodeId );
		}
	}else{
		this->nodeTable = new kmb::nodeIdType[ ncount*size ];
		std::fill( this->nodeTable, this->nodeTable + (ncount*size), kmb::nullNodeId );
		this->nodeTableDeletable = true;
	}
	this->nodeOffset = offset;


	if( !writable ){
		this->typeCounter[ etype ] = size;
		this->index = size;
	}
}

kmb::ElementContainerNArray::~ElementContainerNArray(void)
{
	clear();
}

void
kmb::ElementContainerNArray::clear(void)
{
	kmb::ElementContainer::clear();

	if( nodeTableDeletable && nodeTable != NULL ){
		delete[] nodeTable;
		nodeTable = NULL;
		size = 0;
	}
	index = 0;
}

void
kmb::ElementContainerNArray::initialize(size_t size)
{
	clear();
	index = 0;
	if( nodeTableDeletable && nodeTable == NULL ){
		this->size = size;
		nodeTable = new kmb::nodeIdType[ ncount*size ];

		std::fill( nodeTable, nodeTable + (ncount*size), kmb::nullNodeId );
	}
}

size_t
kmb::ElementContainerNArray::getCount(void) const
{
	return this->typeCounter[ this->etype ];
}

kmb::elementIdType
kmb::ElementContainerNArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( nodeTable == NULL || nodes == NULL || etype != this->etype ){
		return kmb::Element::nullElementId;
	}
	while( index < size && nodeTable[ncount*index] != nodeTable[ncount*index+1] ){
		++index;
	}
	if( index < size ){
		for(size_t i=0;i<ncount;++i){
			nodeTable[ncount*index+i] = nodes[i] + this->nodeOffset;
		}
		++(this->typeCounter[ etype ]);
		++index;
		return static_cast< kmb::elementIdType >( offsetId + index - 1 );
	}
	return kmb::Element::nullElementId;

}

kmb::elementIdType
kmb::ElementContainerNArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	if( nodeTable == NULL || nodes == NULL || etype != this->etype ){
		return kmb::Element::nullElementId;
	}
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable[ncount*i] == nodeTable[ncount*i+1] ){
		for(size_t j=0;j<ncount;++j){
			nodeTable[ncount*i+j] = nodes[j] + this->nodeOffset;
		}
		++(this->typeCounter[ etype ]);
		return id;
	}
	return kmb::Element::nullElementId;
}

bool
kmb::ElementContainerNArray::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[ncount*i] != kmb::nullNodeId && nodes ){
		etype = this->etype;
		for(size_t j=0;j<ncount;++j){
			nodes[j] = nodeTable[ncount*i+j] - this->nodeOffset;
		}
		return true;
	}
	return false;
}

bool
kmb::ElementContainerNArray::deleteElement(const kmb::elementIdType id)
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[ncount*i] != kmb::nullNodeId ){
		for(size_t j=0;j<ncount;++j){
			nodeTable[ncount*i+j] = kmb::nullNodeId;
		}
		--(this->typeCounter[ etype ]);
		return true;
	}
	return false;
}

bool
kmb::ElementContainerNArray::includeElement(const kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[ncount*i] != kmb::nullNodeId){
		return true;
	}else{
		return false;
	}
}

kmb::elementIdType
kmb::ElementContainerNArray::getMaxId(void) const
{
	return static_cast< kmb::elementIdType >( this->offsetId + this->size - 1 );
}

kmb::elementIdType
kmb::ElementContainerNArray::getMinId(void) const
{
	return this->offsetId;
}

const char*
kmb::ElementContainerNArray::getContainerType() const
{
	return kmb::ElementContainerNArray::CONTAINER_TYPE;
}

kmb::ElementContainer::iterator
kmb::ElementContainerNArray::begin(void)
{
	if( this->getCount() == 0 || nodeTable == NULL ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerNArray::_iteratorNA* _it = NULL;
	_it = new kmb::ElementContainerNArray::_iteratorNA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerNArray::begin(void) const
{
	if( this->getCount() == 0 || nodeTable == NULL ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerNArray::_iteratorNA* _it = NULL;
	_it = new kmb::ElementContainerNArray::_iteratorNA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerNArray::find(kmb::elementIdType id)
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[ncount*i] != nodeTable[ncount*i+1] ){
		kmb::ElementContainerNArray::_iteratorNA* _it = NULL;
		_it = new kmb::ElementContainerNArray::_iteratorNA();
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
kmb::ElementContainerNArray::find(kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( 0 <= i && i < static_cast<int>(size) && nodeTable && nodeTable[ncount*i] != nodeTable[ncount*i+1] ){
		kmb::ElementContainerNArray::_iteratorNA* _it = NULL;
		_it = new kmb::ElementContainerNArray::_iteratorNA();
		if( _it ){
			_it->elementContainer = this;
			_it->index = i;
		}
		return kmb::ElementContainer::const_iterator(_it,offsetId);
	}else{
		return kmb::ElementContainer::const_iterator(NULL);
	}
}

kmb::elementType kmb::ElementContainerNArray::getElementType(kmb::elementIdType elementId) const
{
	return etype;
}


kmb::nodeIdType kmb::ElementContainerNArray::operator()(kmb::elementIdType elementId,kmb::idType localId) const
{
	return nodeTable[ncount*(elementId-offsetId)+localId];
}

kmb::nodeIdType& kmb::ElementContainerNArray::operator()(kmb::elementIdType elementId,kmb::idType localId)
{
	return nodeTable[ncount*(elementId-offsetId)+localId];
}



kmb::ElementContainerNArray::_iteratorNA::_iteratorNA(void)
: index(0)
, elementContainer(NULL)
{
}

kmb::ElementContainerNArray::_iteratorNA::~_iteratorNA(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerNArray::_iteratorNA::clone(void)
{
	kmb::ElementContainerNArray::_iteratorNA* obj = NULL;
	obj = new kmb::ElementContainerNArray::_iteratorNA();
	obj->elementContainer = this->elementContainer;
	obj->index = this->index;
	return obj;
}

kmb::elementIdType
kmb::ElementContainerNArray::_iteratorNA::getId(void) const
{
	return static_cast< kmb::elementIdType >( index );
}

bool
kmb::ElementContainerNArray::_iteratorNA::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	if( elementContainer->nodeTable[ elementContainer->ncount*index ] != elementContainer->nodeTable[ elementContainer->ncount*index+1 ] ){
		etype = elementContainer->etype;
		for(size_t i=0;i<elementContainer->ncount;++i){
			nodes[i] = elementContainer->nodeTable[ elementContainer->ncount*index+i ] - elementContainer->nodeOffset;
		}
		return true;
	}
	return false;
}

kmb::elementType
kmb::ElementContainerNArray::_iteratorNA::getType(void) const
{
	if( elementContainer->nodeTable[ elementContainer->ncount*index ] != elementContainer->nodeTable[ elementContainer->ncount*index+1 ] ){
		return elementContainer->etype;
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::ElementContainerNArray::_iteratorNA::getCellId(int cellIndex) const
{
	if( 0 <= cellIndex && cellIndex < static_cast<int>(elementContainer->ncount) &&
		elementContainer->nodeTable[ (elementContainer->ncount)*index ] != elementContainer->nodeTable[ (elementContainer->ncount)*index+1 ] ){
		return elementContainer->nodeTable[ (elementContainer->ncount)*index+cellIndex ] - elementContainer->nodeOffset;
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::ElementContainerNArray::_iteratorNA::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	if( 0 <= cellIndex && cellIndex < static_cast<int>(elementContainer->ncount) ){
		elementContainer->nodeTable[ (elementContainer->ncount)*index+cellIndex ] = nodeId + elementContainer->nodeOffset;
		return true;
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::ElementContainerNArray::_iteratorNA::operator[](const int cellIndex) const
{
	if( 0 <= cellIndex && cellIndex < static_cast<int>(elementContainer->ncount) &&
		elementContainer->nodeTable[ (elementContainer->ncount)*index ] != elementContainer->nodeTable[ (elementContainer->ncount)*index+1 ] ){
		return elementContainer->nodeTable[ (elementContainer->ncount)*index+cellIndex ] - elementContainer->nodeOffset;
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerNArray::_iteratorNA::operator++(void)
{
	++index;
	while( index < elementContainer->size &&
		elementContainer->nodeTable[elementContainer->ncount*index] == elementContainer->nodeTable[elementContainer->ncount*index+1] ){
		++index;
	}
	if( index < elementContainer->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerNArray::_iteratorNA::operator++(int n)
{
	index++;
	while( index < elementContainer->size &&
		elementContainer->nodeTable[elementContainer->ncount*index] == elementContainer->nodeTable[elementContainer->ncount*index+1] ){
		index++;
	}
	if( index < elementContainer->size ){
		return this;
	}else{
		return NULL;
	}
}
