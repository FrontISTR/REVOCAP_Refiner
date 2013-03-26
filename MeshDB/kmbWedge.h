/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Wedge                                                   #
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

class Wedge : public Element
{
public:
	static const int nodeCount;
	Wedge(void);
	Wedge(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3, kmb::nodeIdType n4, kmb::nodeIdType n5);
	Wedge(kmb::nodeIdType *ary);
	virtual ~Wedge(void);

	static int divideIntoTetrahedrons(const kmb::ElementBase* element,kmb::nodeIdType tetrahedrons[][4]);



	static bool isEquivalent(int index[6]);
public:
	static const int connectionTable[6][6];
	static const int faceTable[5][4];
	static const int edgeTable[9][2];

	static void shapeFunction(double s,double t,double u,double* coeff);
	static bool getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3]);
	static bool getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target);

	static double checkShapeFunctionDomain(double s,double t,double u);
};

}
