/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : TetraItem                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshGen/kmbTetraItem.h"

kmb::TetraItem::TetraItem(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3)
: kmb::Tetrahedron(n0,n1,n2,n3)
, previousTetra(NULL)
, nextTetra(NULL)

, face(0)
{
	neighbors[0] = NULL;
	neighbors[1] = NULL;
	neighbors[2] = NULL;
	neighbors[3] = NULL;
	setNeighborFaces(-1,-1,-1,-1);
}

kmb::TetraItem::~TetraItem(void)
{
	if( previousTetra ){
		previousTetra->nextTetra = this->nextTetra;
	}
	if( nextTetra ){
		nextTetra->previousTetra = this->previousTetra;
	}
}

void
kmb::TetraItem::setNeighbors( kmb::TetraItem* nei0, kmb::TetraItem* nei1, kmb::TetraItem* nei2, kmb::TetraItem* nei3 )
{
	neighbors[0] = nei0;
	neighbors[1] = nei1;
	neighbors[2] = nei2;
	neighbors[3] = nei3;
}

void
kmb::TetraItem::setNeighborFaces(int f0,int f1,int f2,int f3)
{




	face &= ~0xffff;
	face |= (f0+2) + ((f1+2)<<4) + ((f2+2)<<8) + ((f3+2)<<12);
}

void
kmb::TetraItem::setNeighbor(int index, kmb::TetraItem* nei,int f)
{
	neighbors[index] = nei;

	face &= ~(0x0f<<(4*index));
	face |= (f+2)<<(4*index);
}

kmb::TetraItem*
kmb::TetraItem::getNeighbor(int index) const
{
	return neighbors[index];
}



int
kmb::TetraItem::getNeighborFace(int index) const
{
	return (face>>(index*4) & 0x0f) - 2;

}

int
kmb::TetraItem::getNeighborIndex(kmb::TetraItem* nei) const
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
kmb::TetraItem::getInner(void) const
{
	return ((face & (1<<16)) != 0);

}

void
kmb::TetraItem::setInner(bool f)
{
	if( f ){
		face |= (1<<16);
	}else{
		face &= ~(1<<16);
	}

}
