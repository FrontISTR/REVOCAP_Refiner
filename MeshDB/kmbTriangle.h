/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Triangle                                                #
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

class Segment;

class Triangle : public Element
{
public:
	static const int nodeCount;
	Triangle(void);
	Triangle(kmb::nodeIdType i0,kmb::nodeIdType i1,kmb::nodeIdType i2);
	Triangle(kmb::nodeIdType *ary);
	virtual ~Triangle(void);



	static bool edgeSwap(kmb::ElementBase &triangle0,kmb::ElementBase &triangle1);


	static int isCoincident(kmb::nodeIdType t00,kmb::nodeIdType t01,kmb::nodeIdType t02,kmb::nodeIdType t10,kmb::nodeIdType t11,kmb::nodeIdType t12);

	static bool isCoincidentNodes(kmb::nodeIdType t0,kmb::nodeIdType t1,kmb::nodeIdType t2);
public:
	static const int connectionTable[3][3];
	static const int faceTable[3][2];

	static void shapeFunction(double s,double t,double* coeff);
	static bool getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* nodes,double naturalCoords[2]);
	static bool getPhysicalCoordinates(const double naturalCoords[2],const kmb::Point3D* nodes,kmb::Point3D &target);

	static double checkShapeFunctionDomain(double s,double t);

};

}
