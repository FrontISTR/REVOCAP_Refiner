/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriangleItem                                            #
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

kmb::TriangleItem::TriangleItem(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2)
: kmb::Triangle(n0,n1,n2)
, previousTri(NULL)
, nextTri(NULL)
, container(NULL)
{
	neighbors[0] = NULL;
	neighbors[1] = NULL;
	neighbors[2] = NULL;
}

kmb::TriangleItem::~TriangleItem(void)
{
	if( previousTri ){
		previousTri->nextTri = this->nextTri;
	}
	if( nextTri ){
		nextTri->previousTri = this->previousTri;
	}
}

kmb::ElementContainer*
kmb::TriangleItem::getContainer(void) const
{
	return container;
}

void
kmb::TriangleItem::setContainer(kmb::ElementContainer* container)
{
	this->container = container;
}

void
kmb::TriangleItem::setNeighbors( kmb::TriangleItem* nei0, kmb::TriangleItem* nei1, kmb::TriangleItem* nei2 )
{
	neighbors[0] = nei0;
	neighbors[1] = nei1;
	neighbors[2] = nei2;
}

void
kmb::TriangleItem::setNeighbor(int index, kmb::TriangleItem* nei)
{
	neighbors[index] = nei;
}

int
kmb::TriangleItem::changeNeighbor(kmb::TriangleItem* oldNei, kmb::TriangleItem* newNei)
{
	int index = -1;
	for(int i=0;i<3;++i){
		if( this->neighbors[i] == oldNei ){
			index = i;
			this->neighbors[i] = newNei;
			break;
		}
	}
	return index;
}

kmb::TriangleItem*
kmb::TriangleItem::getNeighbor(int index) const
{
	return neighbors[index];
}

int
kmb::TriangleItem::getNeighborIndex(kmb::TriangleItem* nei) const
{
	int index = -1;
	for(int i=0;i<3;++i){
		if( this->neighbors[i] == nei ){
			index = i;
			break;
		}
	}
	return index;
}

bool
kmb::TriangleItem::push_back(kmb::TriangleItem* item)
{
	if( item && item->previousTri==NULL )
	{
		kmb::TriangleItem* tri = this;
		while( tri ){
			kmb::TriangleItem* nextTri = tri->getNext();
			if( nextTri==NULL ){
				tri->setNext(item);
				item->setPrevious(tri);
				break;
			}else{
				tri = nextTri;
			}
		}
		return true;
	}
	return false;
}

bool
kmb::TriangleItem::push_front(kmb::TriangleItem* item)
{
	if( item && item->nextTri==NULL )
	{
		kmb::TriangleItem* tri = this;
		while( tri ){
			kmb::TriangleItem* previousTri = tri->getPrevious();
			if( previousTri==NULL ){
				tri->setPrevious(item);
				item->setNext(tri);
				break;
			}else{
				tri = previousTri;
			}
		}
		return true;
	}
	return false;
}

size_t
kmb::TriangleItem::deleteChain(kmb::TriangleItem* item)
{
	size_t count = 0;
	if( item ){
		if( item->previousTri != NULL ){
			item->previousTri->setNext(NULL);
		}
		while( item ){
			kmb::TriangleItem* nextItem = item->getNext();
			++count;
			delete item;
			item = nextItem;
		}
	}
	return count;
}




void
kmb::TriangleItem::swapNeighbors(kmb::TriangleItem* tri0,kmb::TriangleItem* tri1,int n,kmb::TriangleItem* old)
{
	if( tri0 && tri1 && old ){
		tri0->setNeighbor(n,tri1);
		tri1->changeNeighbor(old,tri0);
	}
}
