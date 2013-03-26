/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Triangle2                                               #
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

class Triangle2 : public Element
{
public:
	static const int nodeCount;
	Triangle2(void);
	Triangle2(kmb::nodeIdType *ary);
	virtual ~Triangle2(void);

public:
	static const int connectionTable[6][6];
	static const int faceTable[3][3];

	static void shapeFunction(double s,double t,double* coeff);
	static void shapeFunction_ds(double s,double t,double* coeff);
	static void shapeFunction_dt(double s,double t,double* coeff);
	static void shapeFunction_dsds(double s,double t,double* coeff);
	static void shapeFunction_dsdt(double s,double t,double* coeff);
	static void shapeFunction_dtdt(double s,double t,double* coeff);
	static bool getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* nodes,double naturalCoords[2]);
	static bool getPhysicalCoordinates(const double naturalCoords[2],const kmb::Point3D* nodes,kmb::Point3D &target);

	static double checkShapeFunctionDomain(double s,double t);
};

}
