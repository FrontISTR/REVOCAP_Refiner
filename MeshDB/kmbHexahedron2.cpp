/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Hexahedron2                                             #
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
#include "MeshDB/kmbHexahedron2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbOptimization.h"

/********************************************************************************
=begin

=== 2次六面体要素 (HEXAHEDRON2)

接続行列

	{ 0, 1, 0, 1, 1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0},
	{ 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
	{ 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0},
	{ 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0 ,0, 2},
	{ 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0},
	{ 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0},
	{ 0, 0, 1, 0, 0, 1, 0, 1, 0 ,0 ,0 ,0 ,0, 2, 2, 0, 0, 0, 2, 0},
	{ 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
	{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},

面

	{ 3, 2, 1, 0,10, 9, 8,11},
	{ 4, 5, 6, 7,12,13,14,15},
	{ 1, 5, 4, 0,17,12,16, 8},
	{ 1, 2, 6, 5, 9,18,13,17},
	{ 3, 7, 6, 2,19,14,18,10},
	{ 4, 7, 3, 0,15,19,11,16}

辺

	{ 0, 1, 8},
	{ 1, 2, 9},
	{ 2, 3, 10},
	{ 0, 3, 11},
	{ 4, 5, 12},
	{ 5, 6, 13},
	{ 6, 7, 14},
	{ 4, 7, 15},
	{ 0, 4, 16},
	{ 1, 5, 17},
	{ 2, 6, 18},
	{ 3, 7, 19}

=end

形状関数：
0 : 1/8(1-s)(1-t)(1-u)(-2-s-t-u) => (s,t,u) = (-1,-1,-1)
1 : 1/8(1+s)(1-t)(1-u)(-2+s-t-u) => (s,t,u) = ( 1,-1,-1)
2 : 1/8(1+s)(1+t)(1-u)(-2+s+t-u) => (s,t,u) = ( 1, 1,-1)
3 : 1/8(1-s)(1+t)(1-u)(-2-s+t-u) => (s,t,u) = (-1, 1,-1)
4 : 1/8(1-s)(1-t)(1+u)(-2-s-t+u) => (s,t,u) = (-1,-1, 1)
5 : 1/8(1+s)(1-t)(1+u)(-2+s-t+u) => (s,t,u) = ( 1,-1, 1)
6 : 1/8(1+s)(1+t)(1+u)(-2+s+t+u) => (s,t,u) = ( 1, 1, 1)
7 : 1/8(1-s)(1+t)(1+u)(-2-s+t+u) => (s,t,u) = (-1, 1, 1)
8 : 1/4(1-s*s)(1-t)(1-u)         => (s,t,u) = ( 0,-1,-1)
9 : 1/4(1+s)(1-t*t)(1-u)         => (s,t,u) = ( 1, 0,-1)
10: 1/4(1-s*s)(1+t)(1-u)         => (s,t,u) = ( 0, 1,-1)
11: 1/4(1-s)(1-t*t)(1-u)         => (s,t,u) = (-1, 0,-1)
12: 1/4(1-s*s)(1-t)(1+u)         => (s,t,u) = ( 0,-1, 1)
13: 1/4(1+s)(1-t*t)(1+u)         => (s,t,u) = ( 1, 0, 1)
14: 1/4(1-s*s)(1+t)(1+u)         => (s,t,u) = ( 0, 1, 1)
15: 1/4(1-s)(1-t*t)(1+u)         => (s,t,u) = (-1, 0, 1)
16: 1/4(1-s)(1-t)(1-u*u)         => (s,t,u) = (-1,-1, 0)
17: 1/4(1+s)(1-t)(1-u*u)         => (s,t,u) = ( 1,-1, 0)
18: 1/4(1+s)(1+t)(1-u*u)         => (s,t,u) = ( 1, 1, 0)
19: 1/4(1-s)(1+t)(1-u*u)         => (s,t,u) = (-1, 1, 0)

*********************************************************************************/
const int kmb::Hexahedron2::nodeCount = 20;


const int kmb::Hexahedron2::connectionTable[20][20] =
{
	{ 0, 1, 0, 1, 1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0},
	{ 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
	{ 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0},
	{ 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0 ,0, 2},
	{ 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0},
	{ 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0},
	{ 0, 0, 1, 0, 0, 1, 0, 1, 0 ,0 ,0 ,0 ,0, 2, 2, 0, 0, 0, 2, 0},
	{ 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
	{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
};




const int kmb::Hexahedron2::faceTable[6][8] =
{
	{ 3, 2, 1, 0,10, 9, 8,11},
	{ 4, 5, 6, 7,12,13,14,15},
	{ 1, 5, 4, 0,17,12,16, 8},
	{ 1, 2, 6, 5, 9,18,13,17},
	{ 3, 7, 6, 2,19,14,18,10},
	{ 4, 7, 3, 0,15,19,11,16}
};

const int kmb::Hexahedron2::edgeTable[12][3] =
{
	{ 0, 1, 8},
	{ 1, 2, 9},
	{ 2, 3, 10},
	{ 0, 3, 11},
	{ 4, 5, 12},
	{ 5, 6, 13},
	{ 6, 7, 14},
	{ 4, 7, 15},
	{ 0, 4, 16},
	{ 1, 5, 17},
	{ 2, 6, 18},
	{ 3, 7, 19}
};

kmb::Hexahedron2::Hexahedron2(void)
: kmb::Element(kmb::HEXAHEDRON2)
{
	cell = new kmb::nodeIdType[20];
}

kmb::Hexahedron2::Hexahedron2(kmb::nodeIdType *ary)
: kmb::Element(kmb::HEXAHEDRON2)
{
	cell = ary;
}

kmb::Hexahedron2::~Hexahedron2(void)
{
}

void
kmb::Hexahedron2::shapeFunction(double s,double t,double u,double* coeff)
{
	coeff[0] = 0.125*(1.0-s)*(1.0-t)*(1.0-u)*(-2.0-s-t-u);
	coeff[1] = 0.125*(1.0+s)*(1.0-t)*(1.0-u)*(-2.0+s-t-u);
	coeff[2] = 0.125*(1.0+s)*(1.0+t)*(1.0-u)*(-2.0+s+t-u);
	coeff[3] = 0.125*(1.0-s)*(1.0+t)*(1.0-u)*(-2.0-s+t-u);
	coeff[4] = 0.125*(1.0-s)*(1.0-t)*(1.0+u)*(-2.0-s-t+u);
	coeff[5] = 0.125*(1.0+s)*(1.0-t)*(1.0+u)*(-2.0+s-t+u);
	coeff[6] = 0.125*(1.0+s)*(1.0+t)*(1.0+u)*(-2.0+s+t+u);
	coeff[7] = 0.125*(1.0-s)*(1.0+t)*(1.0+u)*(-2.0-s+t+u);
	coeff[8] = 0.25*(1.0-s*s)*(1.0-t)*(1.0-u);
	coeff[9] = 0.25*(1.0+s)*(1.0-t*t)*(1.0-u);
	coeff[10] = 0.25*(1.0-s*s)*(1.0+t)*(1.0-u);
	coeff[11] = 0.25*(1.0-s)*(1.0-t*t)*(1.0-u);
	coeff[12] = 0.25*(1.0-s*s)*(1.0-t)*(1.0+u);
	coeff[13] = 0.25*(1.0+s)*(1.0-t*t)*(1.0+u);
	coeff[14] = 0.25*(1.0-s*s)*(1.0+t)*(1.0+u);
	coeff[15] = 0.25*(1.0-s)*(1.0-t*t)*(1.0+u);
	coeff[16] = 0.25*(1.0-s)*(1.0-t)*(1.0-u*u);
	coeff[17] = 0.25*(1.0+s)*(1.0-t)*(1.0-u*u);
	coeff[18] = 0.25*(1.0+s)*(1.0+t)*(1.0-u*u);
	coeff[19] = 0.25*(1.0-s)*(1.0+t)*(1.0-u*u);
}

double
kmb::Hexahedron2::checkShapeFunctionDomain(double s,double t,double u)
{
	kmb::Minimizer minimizer;
	minimizer.update( 1.0-s );
	minimizer.update( 1.0+s );
	minimizer.update( 1.0-t );
	minimizer.update( 1.0+t );
	minimizer.update( 1.0-u );
	minimizer.update( 1.0+u );
	return minimizer.getMin();
}

bool
kmb::Hexahedron2::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	/*
	 * 要素座標を求めるためにニュートン法を行う
	 */
	class nr_local : public kmb::OptTargetVV {
	public:
		kmb::Point3D target;
		const kmb::Point3D* points;
		int getDomainDim(void) const { return 3; };
		int getRangeDim(void) const { return 3; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			double coeff[20] = {
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Hexahedron2::shapeFunction( t[0], t[1], t[2], coeff );
			double x=0.0,y=0.0,z=0.0;
			for(int i=0;i<20;++i){
				x += coeff[i] * points[i].x();
				y += coeff[i] * points[i].y();
				z += coeff[i] * points[i].z();
			}
			val.setRow(0,x-target[0]);
			val.setRow(1,y-target[1]);
			val.setRow(2,z-target[2]);
			return true;
		}
		bool df(const ColumnVector &t,Matrix &jac){
			for(int i=0;i<3;++i){

				jac.set(i,0,
					0.125 * (
					-(1.0-t[1])*(1.0-t[2])*points[0][i]
					+(1.0-t[1])*(1.0-t[2])*points[1][i]
					+(1.0+t[1])*(1.0-t[2])*points[2][i]
					-(1.0+t[1])*(1.0-t[2])*points[3][i]
					-(1.0-t[1])*(1.0+t[2])*points[4][i]
					+(1.0-t[1])*(1.0+t[2])*points[5][i]
					+(1.0+t[1])*(1.0+t[2])*points[6][i]
					-(1.0+t[1])*(1.0+t[2])*points[7][i] ) );

				jac.set(i,1,
					0.125 * (
					-(1.0-t[0])*(1.0-t[2])*points[0][i]
					-(1.0+t[0])*(1.0-t[2])*points[1][i]
					+(1.0+t[0])*(1.0-t[2])*points[2][i]
					+(1.0-t[0])*(1.0-t[2])*points[3][i]
					-(1.0-t[0])*(1.0+t[2])*points[4][i]
					-(1.0+t[0])*(1.0+t[2])*points[5][i]
					+(1.0+t[0])*(1.0+t[2])*points[6][i]
					+(1.0-t[0])*(1.0+t[2])*points[7][i] ) );

				jac.set(i,2,
					0.125 * (
					-(1.0-t[0])*(1.0-t[1])*points[0][i]
					-(1.0+t[0])*(1.0-t[1])*points[1][i]
					-(1.0+t[0])*(1.0+t[1])*points[2][i]
					-(1.0-t[0])*(1.0+t[1])*points[3][i]
					+(1.0-t[0])*(1.0-t[1])*points[4][i]
					+(1.0+t[0])*(1.0-t[1])*points[5][i]
					+(1.0+t[0])*(1.0+t[1])*points[6][i]
					+(1.0-t[0])*(1.0+t[1])*points[7][i] ) );
			}
			return true;
		}
		nr_local(const kmb::Point3D &t,const kmb::Point3D* pt)
		: target(t), points(pt){}
	};
	nr_local opt_obj(target,points);
	double min_t[3]  = {-1.0, -1.0, -1.0};
	double max_t[3]  = { 1.0,  1.0,  1.0};
	kmb::Optimization opt;
	bool res = opt.calcZero_NR( opt_obj, naturalCoords, min_t, max_t );
	return res;
}

bool
kmb::Hexahedron2::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[20];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<20;++j){
			target.addCoordinate(i,points[j][i] * coeff[j]);
		}
	}
	return true;
}
