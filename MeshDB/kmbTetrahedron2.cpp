/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Tetrahedron2                                            #
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
#include "MeshDB/kmbTetrahedron2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElementRelation.h"
#include "Geometry/kmb_Sphere.h"

/********************************************************************************
=begin

=== 2次四面体要素 (TETRAHEDRON2)

接続行列

	{ 0, 1, 1, 1, 0, 2, 2, 2, 0, 0},
	{ 1, 0, 1, 1, 2, 0, 2, 0, 2, 0},
	{ 1, 1, 0, 1, 2, 2, 0, 0, 0, 2},
	{ 1, 1, 1, 0, 0, 0, 0, 2, 2, 2},
	{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 2, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 2, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 0, 2, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}

面情報

	{ 1, 2, 3, 9, 8, 4},
	{ 0, 3, 2, 9, 5, 7},
	{ 0, 1, 3, 8, 7, 6},
	{ 0, 2, 1, 4, 6, 5}

辺

	{ 1, 2, 4},
	{ 0, 2, 5},
	{ 0, 1, 6},
	{ 0, 3, 7},
	{ 1, 3, 8},
	{ 2, 3, 9}

=end

形状関数 ( 0<= s <= 1, 0 <= t <= 1, 0 <= u <= 1, 0 <= s+t+u <= 1 )
0 : (1-s-t-u)*(1-2*s-2*t-2*u)  => (s,t,u) = (  0,   0,   0)
1 : s*(2*s-1)                  => (s,t,u) = (  1,   0,   0)
2 : t*(2*t-1)                  => (s,t,u) = (  0,   1,   0)
3 : u*(2*u-1)                  => (s,t,u) = (  0,   0,   1)
4 : 4*s*t                      => (s,t,u) = (0.5, 0.5,   0)
5 : 4*t*(1-s-t-u)              => (s,t,u) = (  0, 0.5,   0)
6 : 4*s*(1-s-t-u)              => (s,t,u) = (0.5,   0,   0)
7 : 4*u*(1-s-t-u)              => (s,t,u) = (  0,   0, 0.5)
8 : 4*u*s                      => (s,t,u) = (0.5,   0, 0.5)
9 : 4*u*t                      => (s,t,u) = (  0, 0.5, 0.5)

*********************************************************************************/
const int kmb::Tetrahedron2::nodeCount = 10;

const int kmb::Tetrahedron2::connectionTable[10][10] =
{
	{ 0, 1, 1, 1, 0, 2, 2, 2, 0, 0},
	{ 1, 0, 1, 1, 2, 0, 2, 0, 2, 0},
	{ 1, 1, 0, 1, 2, 2, 0, 0, 0, 2},
	{ 1, 1, 1, 0, 0, 0, 0, 2, 2, 2},
	{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 2, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 2, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 0, 2, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}
};









const int kmb::Tetrahedron2::faceTable[4][6] =
{
	{ 1, 2, 3, 9, 8, 4},
	{ 0, 3, 2, 9, 5, 7},
	{ 0, 1, 3, 8, 7, 6},
	{ 0, 2, 1, 4, 6, 5}
};

const int kmb::Tetrahedron2::edgeTable[6][3] =
{
	{ 1, 2, 4},
	{ 0, 2, 5},
	{ 0, 1, 6},
	{ 0, 3, 7},
	{ 1, 3, 8},
	{ 2, 3, 9}
};

kmb::Tetrahedron2::Tetrahedron2(void)
: kmb::Element(kmb::TETRAHEDRON2)
{
	cell = new kmb::nodeIdType[10];
}

kmb::Tetrahedron2::Tetrahedron2(kmb::nodeIdType *ary)
: kmb::Element(kmb::TETRAHEDRON2)
{
	cell = ary;
}

kmb::Tetrahedron2::~Tetrahedron2(void)
{
}

void
kmb::Tetrahedron2::shapeFunction(double s,double t,double u,double* coeff)
{
	coeff[0] = (1.0-s-t-u)*(1.0-2.0*s-2.0*t-2.0*u);
	coeff[1] = s*(2.0*s-1.0);
	coeff[2] = t*(2.0*t-1.0);
	coeff[3] = u*(2.0*u-1.0);
	coeff[4] = 4.0*s*t;
	coeff[5] = 4.0*t*(1.0-s-t-u);
	coeff[6] = 4.0*s*(1.0-s-t-u);
	coeff[7] = 4.0*u*(1.0-s-t-u);
	coeff[8] = 4.0*u*s;
	coeff[9] = 4.0*u*t;
}

bool
kmb::Tetrahedron2::getNaturalCoordinates(const double physicalCoords[3],const kmb::Point3D* points,double naturalCoords[3],double margin)
{
	if( points == NULL ){
		return false;
	}
	/*
	 * 4面体2次の要素座標を求めるためにニュートン法を行う
	 */
	const int len = 10;
	double d = 0.0;
	double thres = 1.0e-10;
	kmb::BoundingBox bbox;
	for(int i=0;i<len;++i){
		bbox.update( points[i] );
	}
	bbox.expand( margin );
	if( bbox.intersect(physicalCoords[0],physicalCoords[1],physicalCoords[2]) == kmb::BoxRegion::OUTSIDE ){
		return false;
	}

	for(int j=0;j<10;++j){
		for(int i=0;i<50;++i){
			d = kmb::Tetrahedron2::newtonMethod(physicalCoords,points,naturalCoords);
			if( d < thres &&
				0.0 <= naturalCoords[0] && naturalCoords[0] <= 1.0 &&
				0.0 <= naturalCoords[1] && naturalCoords[1] <= 1.0 &&
				0.0 <= naturalCoords[2] && naturalCoords[2] <= 1.0 )
			{
				return true;
			}
		}
		if( d < thres &&
			-margin <= naturalCoords[0] && naturalCoords[0] <= 1.0 + margin &&
			-margin <= naturalCoords[1] && naturalCoords[1] <= 1.0 + margin &&
			-margin <= naturalCoords[2] && naturalCoords[2] <= 1.0 + margin )
		{
			return true;
		}


		naturalCoords[0] = -0.01*j*naturalCoords[1];
		naturalCoords[1] = -0.01*j*naturalCoords[2];
		naturalCoords[2] = -0.01*j*naturalCoords[0];
	}
	return false;
}

bool
kmb::Tetrahedron2::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,double physicalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	double coeff[10];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	for(int i=0;i<3;++i){
		physicalCoords[i] = 0.0;
		for(int j=0;j<10;++j){
			physicalCoords[i] += points[j].getCoordinate(i) * coeff[j];
		}
	}
	return true;
}

double
kmb::Tetrahedron2::newtonMethod(const double physicalCoords[3], const kmb::Point3D* nodes, double naturalCoords[3])
{
	double retVal = DBL_MAX;
	if( nodes == NULL ){
		return retVal;
	}
	double q[3];
	if( !getPhysicalCoordinates( naturalCoords, nodes, q ) ){
		return retVal;
	}
	double ds[3] = {0.0,0.0,0.0};
	double dt[3] = {0.0,0.0,0.0};
	double du[3] = {0.0,0.0,0.0};

	for(int i=0;i<3;++i){
		ds[i] =
			  ( -3.0 + 4.0 * naturalCoords[0] + 4.0 * naturalCoords[1] + 4.0 * naturalCoords[2] )  * nodes[0].getCoordinate(i)
			+ ( 4.0 * naturalCoords[0] - 1.0 ) * nodes[1].getCoordinate(i)
			+ 4.0 * naturalCoords[1] * nodes[4].getCoordinate(i)
			- 4.0 * naturalCoords[1] * nodes[5].getCoordinate(i)
			+ 4.0 * ( 1.0 - 2.0 * naturalCoords[0] - naturalCoords[1] - naturalCoords[2] )  * nodes[6].getCoordinate(i)
			- 4.0 * naturalCoords[2] * nodes[7].getCoordinate(i)
			+ 4.0 * naturalCoords[2] * nodes[8].getCoordinate(i);
		dt[i] =
			  ( -3.0 + 4.0 * naturalCoords[0] + 4.0 * naturalCoords[1] + 4.0 * naturalCoords[2] )  * nodes[0].getCoordinate(i)
			+ ( 4.0 * naturalCoords[1] - 1.0 ) * nodes[2].getCoordinate(i)
			+ 4.0 * naturalCoords[0] * nodes[4].getCoordinate(i)
			+ 4.0 * ( 1.0 - naturalCoords[0] - 2.0 * naturalCoords[1] - naturalCoords[2] )  * nodes[5].getCoordinate(i)
			- 4.0 * naturalCoords[0] * nodes[6].getCoordinate(i)
			- 4.0 * naturalCoords[2] * nodes[7].getCoordinate(i)
			+ 4.0 * naturalCoords[2] * nodes[9].getCoordinate(i);
		du[i] =
			  ( -3.0 + 4.0 * naturalCoords[0] + 4.0 * naturalCoords[1] + 4.0 * naturalCoords[2] )  * nodes[0].getCoordinate(i)
			+ ( 4.0 * naturalCoords[2] - 1.0 ) * nodes[3].getCoordinate(i)
			- 4.0 * naturalCoords[1] * nodes[5].getCoordinate(i)
			- 4.0 * naturalCoords[0] * nodes[6].getCoordinate(i)
			+ 4.0 * ( 1.0 - naturalCoords[0] - naturalCoords[1] - 2.0 * naturalCoords[2] )  * nodes[7].getCoordinate(i)
			+ 4.0 * naturalCoords[0] * nodes[8].getCoordinate(i)
			+ 4.0 * naturalCoords[1] * nodes[9].getCoordinate(i);
	}
	kmb::Matrix3x3 mat(
		ds[0],dt[0],du[0],
		ds[1],dt[1],du[1],
		ds[2],ds[2],du[2]);
	kmb::Vector3D v( q[0]-physicalCoords[0], q[1]-physicalCoords[1], q[2]-physicalCoords[2] );
	kmb::Vector3D* sol = mat.solve(v);
	if( sol != NULL ){
		naturalCoords[0] -= sol->x();
		naturalCoords[1] -= sol->y();
		naturalCoords[2] -= sol->z();
		retVal = sol->length();
		delete sol;
	}
	return retVal;
}























































































































































































































































































































































































































































































































































