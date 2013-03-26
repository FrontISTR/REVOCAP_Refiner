/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriangleList                                            #
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
#include "MeshGen/kmbTriangleItem.h"
#include "MeshGen/kmbTriangleList.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::TriangleList::TriangleList(void)
: first(NULL)
, last(NULL)
, elementCount(0)
{
}

kmb::TriangleList::~TriangleList(void)
{
	clear();
}

const char* kmb::TriangleList::CONTAINER_TYPE = "TriangleList";

kmb::elementIdType
kmb::TriangleList::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( etype == kmb::TRIANGLE && nodes != NULL ){
		kmb::TriangleItem* tri = new kmb::TriangleItem( nodes[0], nodes[1], nodes[2] );
		appendItem( tri );
		return 0;
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::TriangleList::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	return kmb::Element::nullElementId;
}

bool
kmb::TriangleList::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	return false;
}

bool
kmb::TriangleList::deleteElement(const kmb::elementIdType id)
{
	return false;
}

bool
kmb::TriangleList::includeElement(const kmb::elementIdType id) const
{
	return false;
}

kmb::elementIdType
kmb::TriangleList::getMaxId(void) const
{
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::TriangleList::getMinId(void) const
{
	return kmb::Element::nullElementId;
}

size_t
kmb::TriangleList::getCount(void) const
{
	return elementCount;
}

void
kmb::TriangleList::clear(void)
{
	kmb::TriangleItem* tri = first;
	kmb::TriangleItem* nextTri = NULL;
	while( tri ){
		nextTri = tri->getNext();
		deleteItem( tri );
		tri = nextTri;
	}
	first = NULL;
	last = NULL;
	elementCount = 0;
}

void
kmb::TriangleList::initialize(size_t size)
{
	clear();
}

bool
kmb::TriangleList::deleteItem(TriangleItem* tri)
{
	if( tri && tri->getContainer() == this ){
		if( this->first == tri ){
			this->first = tri->getNext();
		}
		if( this->last == tri ){
			this->last = tri->getPrevious();
		}
		--elementCount;
		--(typeCounter[ kmb::TRIANGLE ]);
		delete tri;
		return true;
	}
	return false;
}

bool
kmb::TriangleList::appendItem(TriangleItem* tri)
{
	if( tri && tri->getContainer() == NULL ){
		tri->setContainer( this );
		if( first == NULL ){
			this->first = tri;
		}else{
			this->last->setNext( tri );
			tri->setPrevious( this->last );
		}
		this->last = tri;
		++elementCount;
		++(typeCounter[ kmb::TRIANGLE ]);
		return true;
	}
	return false;
}

size_t
kmb::TriangleList::appendChain(TriangleItem* head)
{
	if( head && head->getPrevious() == NULL && head->getContainer() == NULL ){
		if( this->last ){
			this->last->setNext( head );
			head->setPrevious( this->last );
		}else{
			this->first = head;
		}
		size_t count = 0;
		kmb::TriangleItem* tri = head;
		kmb::TriangleItem* nextTri = NULL;
		while( tri ){
			++elementCount;
			++count;
			tri->setContainer( this );
			nextTri = tri->getNext();
			if( nextTri == NULL ){
				this->last = tri;
				break;
			}
			tri = nextTri;
		}
		return count;
	}
	return 0;
}

kmb::TriangleItem*
kmb::TriangleList::eraseItem(TriangleItem* tri)
{
	if( tri && tri->getContainer() == this ){
		if( this->first == tri ){
			this->first = tri->getNext();
		}
		if( this->last == tri ){
			this->last = tri->getPrevious();
		}
		tri->setContainer(NULL);
		kmb::TriangleItem* previousTri = tri->getPrevious();
		kmb::TriangleItem* nextTri = tri->getNext();
		if( previousTri ){
			previousTri->setNext( nextTri );
			tri->setPrevious( NULL );
		}
		if( nextTri ){
			nextTri->setPrevious( previousTri );
			tri->setNext( NULL );
		}
		--elementCount;
		return tri;
	}
	return NULL;
}

kmb::ElementContainer::iterator
kmb::TriangleList::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::TriangleList::_iterator* _it = NULL;
	_it = new kmb::TriangleList::_iterator();;
	_it->tri = this->first;
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::TriangleList::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::TriangleList::_iterator* _it = NULL;
	_it = new kmb::TriangleList::_iterator();;
	_it->tri = this->first;
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::TriangleList::find(kmb::elementIdType id)
{
	return kmb::ElementContainer::iterator(NULL);
}

kmb::ElementContainer::const_iterator
kmb::TriangleList::find(kmb::elementIdType id) const
{
	return kmb::ElementContainer::const_iterator(NULL);
}

kmb::TriangleList::_iterator::_iterator(void)
: tri(NULL)
, localId(0)
{
}

kmb::TriangleList::_iterator::~_iterator(void)
{
}

kmb::elementIdType
kmb::TriangleList::_iterator::getId(void) const
{
	if( tri && tri->getContainer() ){
		return tri->getContainer()->getOffsetId() + localId;
	}else{
		return kmb::Element::nullElementId;
	}
}

bool
kmb::TriangleList::_iterator::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	if( tri ){
		etype = kmb::TRIANGLE;
		for(int i=0;i<3;++i){
			nodes[i] = tri->getCellId(i);
		}
		return true;
	}else{
		etype = kmb::UNKNOWNTYPE;
		return false;
	}
}

kmb::Element*
kmb::TriangleList::_iterator::getElement(void)
{
	return tri;
}

const kmb::Element*
kmb::TriangleList::_iterator::getElement(void) const
{
	return tri;
}

kmb::elementType
kmb::TriangleList::_iterator::getType(void) const
{
	if( tri ){
		return kmb::TRIANGLE;
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::TriangleList::_iterator::getCellId(int cellIndex) const
{
	if( tri ){
		return tri->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::TriangleList::_iterator::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	if( tri ){
		return tri->setCellId(cellIndex,nodeId);
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::TriangleList::_iterator::operator[](const int cellIndex) const
{
	if( tri ){
		return tri->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::_iterator*
kmb::TriangleList::_iterator::operator++(void)
{
	tri = tri->getNext();
	if( tri ){
		localId++;
		return this;
	}else{
		return NULL;
	}
}

kmb::ElementContainer::_iterator*
kmb::TriangleList::_iterator::operator++(int n)
{
	tri = tri->getNext();
	if( tri ){
		localId++;
		return this;
	}else{
		return NULL;
	}
}

kmb::ElementContainer::_iterator*
kmb::TriangleList::_iterator::clone(void)
{
	kmb::TriangleList::_iterator* obj = NULL;
	obj = new kmb::TriangleList::_iterator();
	obj->tri = this->tri;
	return obj;
}

