/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Quad                                                    #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
#pragma once
#include "MeshDB/kmbElement.h"

namespace kmb{

class Quad : public Element
{
public:
	static const int nodeCount;
	Quad(void);
	Quad(kmb::nodeIdType i0,kmb::nodeIdType i1,kmb::nodeIdType i2,kmb::nodeIdType i3);
	Quad(kmb::nodeIdType *ary);
	virtual ~Quad(void);




	static bool edgeSwap(kmb::ElementBase &quad0,kmb::ElementBase &quad1,bool orientation=true);


	static int isCoincident(kmb::nodeIdType t00,kmb::nodeIdType t01,kmb::nodeIdType t02,kmb::nodeIdType t03,
							kmb::nodeIdType t10,kmb::nodeIdType t11,kmb::nodeIdType t12,kmb::nodeIdType t13);
public:
	static const int	connectionTable[4][4];
	static const int	faceTable[4][2];

	static void shapeFunction(double s,double t,double* coeff);
};

}
