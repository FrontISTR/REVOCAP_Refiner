/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriInterface                                            #
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

class TriInterface : public Element
{
public:
	static const int nodeCount;
	TriInterface(void);
	TriInterface(kmb::nodeIdType *ary);
	virtual ~TriInterface(void);
public:
	static const int connectionTable[6][6];
	static const int faceTable[5][3];
	static const int edgeTable[12][2];

	static const int faceIndexTable[6][6];
};

}
