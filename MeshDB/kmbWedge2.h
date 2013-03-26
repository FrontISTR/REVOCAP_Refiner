/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Wedge2                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
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

class Tetrahedron;

class Wedge2 : public Element
{
public:
	static const int nodeCount;
	Wedge2(void);
	Wedge2(kmb::nodeIdType *ary);
	virtual ~Wedge2(void);
public:
	static const int connectionTable[15][15];
	static const int faceTable[5][8];
	static const int edgeTable[9][3];

	static void shapeFunction(double s,double t,double u,double* coeff);

	static double checkShapeFunctionDomain(double s,double t,double u);
};

}
