/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TetraItem                                               #
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

#include "MeshDB/kmbTetrahedron.h"
#include "MeshGen/kmbListItem.h"

namespace kmb{

class Point3DContainer;

class TetraN : public Tetrahedron
{
protected:
	TetraN** neighbors;




	unsigned int face;
public:
	TetraN(nodeIdType n0,nodeIdType n1,nodeIdType n2,nodeIdType n3);
	virtual ~TetraN(void);

	void setNeighbor(int index, kmb::TetraN* nei,int f);


	kmb::TetraN* getNeighbor(int index) const;
	int getNeighborFace(int index) const;

	int getNeighborIndex(kmb::TetraN* nei) const;
};

class TetraItem : public Tetrahedron
{
protected:
	kmb::TetraItem* previousTetra;
	kmb::TetraItem* nextTetra;
	kmb::TetraItem* neighbors[4];




	unsigned int face;
public:
	TetraItem(nodeIdType n0,nodeIdType n1,nodeIdType n2,nodeIdType n3);
	virtual ~TetraItem(void);

	kmb::TetraItem* getPrevious(void) const { return previousTetra; }
	void setPrevious(kmb::TetraItem* tetra){ previousTetra = tetra; }
	kmb::TetraItem* getNext(void) const { return nextTetra; }
	void setNext(kmb::TetraItem* tetra){ nextTetra = tetra; }





	void detach(void);
	void detach(kmb::TetraItem* &first,kmb::TetraItem* &last);

	void attach(kmb::TetraItem* last);
	void attach(kmb::TetraItem* &first,kmb::TetraItem* &last);


	void setNeighbor(int index, kmb::TetraItem* nei,int f);


	kmb::TetraItem* getNeighbor(int index) const;
	int getNeighborFace(int index) const;

	int getNeighborIndex(kmb::TetraItem* nei) const;


	static void generateNeighborRelations( kmb::TetraItem* tetra );


	static void setAdjacentPair(kmb::TetraItem* t0,int i0,kmb::TetraItem* t1,int i1);


	static int deleteItems( kmb::TetraItem* first );


	static int debugItem( kmb::TetraItem* item, const char* tag=NULL  );
	static int debugItems( kmb::TetraItem* first, const char* tag=NULL );
	static bool checkVolume( kmb::TetraItem* tetra, kmb::Point3DContainer &points );
	static bool checkVolumes( kmb::TetraItem* tetra, kmb::Point3DContainer &points, const char* tag=NULL );
	static bool checkNeighbor( kmb::TetraItem* tetra );
	static bool checkNeighbors( kmb::TetraItem* tetra, const char* tag=NULL );
};

}
