/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Tetrahedron2                                            #
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

class Tetrahedron2 : public Element
{
public:
	static const int nodeCount;
	Tetrahedron2(void);
	Tetrahedron2(kmb::nodeIdType *ary);
	virtual ~Tetrahedron2(void);
public:
	static const int connectionTable[10][10];
	static const int faceTable[4][6];
	static const int edgeTable[6][3];

	static void shapeFunction(double s,double t,double u,double* coeff);
	static void shapeFunction_ds(double s,double t,double u,double* coeff);
	static void shapeFunction_dt(double s,double t,double u,double* coeff);
	static void shapeFunction_du(double s,double t,double u,double* coeff);
	static bool getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3]);
	static bool getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target);

	static double checkShapeFunctionDomain(double s,double t,double u);
};

}
