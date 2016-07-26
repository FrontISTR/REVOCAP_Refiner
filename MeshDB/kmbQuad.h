/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Quad                                                    #
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

class Point2D;

class Quad : public Element
{
public:
	static const int nodeCount;
	Quad(void);
	Quad(kmb::nodeIdType i0,kmb::nodeIdType i1,kmb::nodeIdType i2,kmb::nodeIdType i3);
	Quad(kmb::nodeIdType *ary);
	virtual ~Quad(void);

	// 辺の交換
	// orientation=true  abcd と befc から abed と efcd に変換する（共通辺を順方向にずらす）
	// orientation=false abcd と befc から abef と afcd に変換する（共通辺を逆方向にずらす）
	static bool edgeSwap(kmb::ElementBase &quad0,kmb::ElementBase &quad1,bool orientation=true);
	// 四角形に特化した一致判定
	// 1 : 一致 -1 : 逆向き 0 : その他
	static int isCoincident(kmb::nodeIdType t00,kmb::nodeIdType t01,kmb::nodeIdType t02,kmb::nodeIdType t03,
							kmb::nodeIdType t10,kmb::nodeIdType t11,kmb::nodeIdType t12,kmb::nodeIdType t13);
public:
	static const int connectionTable[4][4];
	static const int faceTable[4][2];

	static void shapeFunction(double s,double t,double* coeff);
	static void shapeFunction_ds(double s,double t,double* coeff);
	static void shapeFunction_dt(double s,double t,double* coeff);
	static void shapeFunction_dsds(double s,double t,double* coeff);
	static void shapeFunction_dsdt(double s,double t,double* coeff);
	static void shapeFunction_dtdt(double s,double t,double* coeff);
	static bool getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* nodes,double naturalCoords[2]);
	static bool getPhysicalCoordinates(const double naturalCoords[2],const kmb::Point3D* nodes,kmb::Point3D &target);

	static double checkShapeFunctionDomain(double s,double t);

	static double jacobian(double s, double t,const kmb::Point2D* points);
	static double jacobian_ds(double s, double t,const kmb::Point2D* points);
	static double jacobian_dt(double s, double t,const kmb::Point2D* points);
};

}
