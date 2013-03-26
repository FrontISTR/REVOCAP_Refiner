/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Hexahedron                                              #
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
#include "Geometry/kmbPoint3DContainer.h"

namespace kmb{

class Tetrahedron;

class Hexahedron : public Element
{
public:
	static const int nodeCount;
	Hexahedron(void);
	Hexahedron(
		kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3,
		kmb::nodeIdType n4, kmb::nodeIdType n5, kmb::nodeIdType n6, kmb::nodeIdType n7);
	Hexahedron(kmb::nodeIdType *ary);
	virtual ~Hexahedron(void);

	static int divideIntoTetrahedrons(const kmb::ElementBase* element,kmb::nodeIdType tetrahedrons[][4]);



	static bool isEquivalent(int index[8]);



	static int isOppositeFace(const kmb::ElementBase* hexa,const kmb::ElementBase* quad0,const kmb::ElementBase* quad1);
public:
	static const int connectionTable[8][8];
	static const int faceTable[6][4];
	static const int edgeTable[12][2];

	static void shapeFunction(double s,double t,double u,double* coeff);

	static bool getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3]);
	static bool getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target);

	static double checkShapeFunctionDomain(double s,double t,double u);

	static double jacobian(double s, double t,double u,const kmb::Point3D* points);
	static double jacobian_ds(double s, double t,double u,const kmb::Point3D* points);
	static double jacobian_dt(double s, double t,double u,const kmb::Point3D* points);
	static double jacobian_du(double s, double t,double u,const kmb::Point3D* points);
};

}
