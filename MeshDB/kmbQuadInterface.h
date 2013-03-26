/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : QuadInterface                                           #
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
#include "MeshDB/kmbElement.h"

namespace kmb{

class Point2D;

class QuadInterface : public Element
{
public:
	static const int nodeCount;
	QuadInterface(void);
	QuadInterface(kmb::nodeIdType *ary);
	virtual ~QuadInterface(void);
public:
	static const int connectionTable[9][9];
	static const int faceTable[5][4];
	static const int edgeTable[16][2];

	static const int faceIndexTable[9][9];
};

}
