/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : TetraItem                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "MeshDB/kmbTetrahedron.h"

namespace kmb{

class TetraItem : public Tetrahedron
{
protected:
	kmb::TetraItem* previousTetra;
	kmb::TetraItem* nextTetra;
	kmb::TetraItem* neighbors[4];



	unsigned int face;
public:
	TetraItem(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3);
	virtual ~TetraItem(void);

	kmb::TetraItem* getPrevious(void) const {	return previousTetra;	}
	void setPrevious(kmb::TetraItem* tetra){ previousTetra = tetra; }
	kmb::TetraItem* getNext(void) const {	return nextTetra;	}
	void setNext(kmb::TetraItem* tetra){ nextTetra = tetra; }







	void setNeighbors(kmb::TetraItem* nei0, kmb::TetraItem* nei1, kmb::TetraItem* nei2, kmb::TetraItem* nei3);
	void setNeighborFaces(int f0,int f1,int f2,int f3);
	void setNeighbor(int index, kmb::TetraItem* nei,int f);
	kmb::TetraItem* getNeighbor(int index) const;
	int getNeighborFace(int index) const;
	int getNeighborIndex(kmb::TetraItem* nei) const;

	bool getInner(void) const;
	void setInner(bool f);
};

}
