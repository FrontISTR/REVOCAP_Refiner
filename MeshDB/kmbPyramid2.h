/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Pyramid2                                                #
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

class Pyramid2 : public Element
{
public:
	static const int nodeCount;
	Pyramid2(void);
	Pyramid2(kmb::nodeIdType *ary);
	virtual ~Pyramid2(void);
public:
	static const int connectionTable[13][13];
	static const int faceTable[5][8];
	static const int edgeTable[8][3];

	static void shapeFunction(double s,double t,double u,double* coeff);

	static double checkShapeFunctionDomain(double s,double t,double u);
};

}
