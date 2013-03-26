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
#pragma once

#include "MeshDB/kmbTriangle.h"

namespace kmb{

class ElementContainer;

class TriangleItem :
	public Triangle
{
public:
	TriangleItem(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2);
	virtual ~TriangleItem(void);

	TriangleItem* getPrevious(void) const {	return previousTri;	}
	void setPrevious(kmb::TriangleItem* tri){ previousTri = tri; }
	TriangleItem* getNext(void) const {	return nextTri;	}
	void setNext(kmb::TriangleItem* tri){ nextTri = tri; }

	kmb::ElementContainer* getContainer(void) const;
	void setContainer(kmb::ElementContainer* container);

	void setNeighbors(kmb::TriangleItem* nei0, kmb::TriangleItem* nei1, kmb::TriangleItem* nei2 );
	void setNeighbor(int index, kmb::TriangleItem* nei);


	int changeNeighbor(kmb::TriangleItem* oldNei, kmb::TriangleItem* newNei);
	kmb::TriangleItem* getNeighbor(int index) const;
	int getNeighborIndex(kmb::TriangleItem* nei) const;


	bool push_back(kmb::TriangleItem* item);
	bool push_front(kmb::TriangleItem* item);

	static size_t deleteChain(kmb::TriangleItem* item);



	static void swapNeighbors(kmb::TriangleItem* tri0,kmb::TriangleItem* tri1,int n,kmb::TriangleItem* old);
private:
	TriangleItem* previousTri;
	TriangleItem* nextTri;
	TriangleItem* neighbors[3];
	kmb::ElementContainer* container;
};

}
