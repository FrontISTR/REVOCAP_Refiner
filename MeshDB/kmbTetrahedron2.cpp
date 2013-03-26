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
#include "MeshDB/kmbTetrahedron2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"
#include "Geometry/kmbSphere.h"

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Geometry/kmbOptimization.h"

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

void
kmb::Tetrahedron2::shapeFunction_ds(double s,double t,double u,double* coeff)
{
	coeff[0] = -3.0 + 4.0*s + 4.0*t + 4.0*u;
	coeff[1] = -1.0 + 4.0*s;
	coeff[2] =  0.0;
	coeff[3] =  0.0;
	coeff[4] =              + 4.0*t;
	coeff[5] =              - 4.0*t;
	coeff[6] =  4.0 - 8.0*s - 4.0*t - 4.0*u;
	coeff[7] =                      - 4.0*u;
	coeff[8] =                      + 4.0*u;
	coeff[9] =  0.0;
}

void
kmb::Tetrahedron2::shapeFunction_dt(double s,double t,double u,double* coeff)
{
	coeff[0] = -3.0 + 4.0*s + 4.0*t + 4.0*u;
	coeff[1] =  0.0;
	coeff[2] = -1.0         + 4.0*t;
	coeff[3] =  0.0;
	coeff[4] =      + 4.0*s;
	coeff[5] =  4.0 - 4.0*s - 8.0*t - 4.0*u;
	coeff[6] =      - 4.0*s;
	coeff[7] =                      - 4.0*u;
	coeff[8] =  0.0;
	coeff[9] =                      + 4.0*u;
}

void
kmb::Tetrahedron2::shapeFunction_du(double s,double t,double u,double* coeff)
{
	coeff[0] = -3.0 + 4.0*s + 4.0*t + 4.0*u;
	coeff[1] =  0.0;
	coeff[2] =  0.0;
	coeff[3] = -1.0                 + 4.0*u;
	coeff[4] =  0.0;
	coeff[5] =              - 4.0*t;
	coeff[6] =      - 4.0*s;
	coeff[7] =  4.0 - 4.0*s - 4.0*t - 8.0*u;
	coeff[8] =      + 4.0*s;
	coeff[9] =              + 4.0*t;
}

double
kmb::Tetrahedron2::checkShapeFunctionDomain(double s,double t,double u)
{
	kmb::Minimizer minimizer;
	minimizer.update( 1.0-s-t-u );
	minimizer.update( s );
	minimizer.update( t );
	minimizer.update( u );
	return minimizer.getMin();
}

bool
kmb::Tetrahedron2::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	/*
	 * 4面体2次の要素座標を求めるためにニュートン法を行う
	 */
	class nr_local : public kmb::OptTargetVV {
	public:
		kmb::Point3D target;
		const kmb::Point3D* points;
		int getDomainDim(void) const { return 3; };
		int getRangeDim(void) const { return 3; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			double coeff[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Tetrahedron2::shapeFunction( t[0], t[1], t[2], coeff );
			double x=0.0,y=0.0,z=0.0;
			for(int i=0;i<10;++i){
				x += coeff[i] * points[i].x();
				y += coeff[i] * points[i].y();
				z += coeff[i] * points[i].z();
			}
			val.setRow(0,x-target[0]);
			val.setRow(1,y-target[1]);
			val.setRow(2,z-target[2]);
			return true;
		}
		bool df(const kmb::ColumnVector &t,kmb::Matrix &jac){
			double coeff[10];
			double v = 0.0;

			kmb::Tetrahedron2::shapeFunction_ds(t[0],t[1],t[2],coeff);
			for(int i=0;i<3;++i){
				v = 0.0;
				for(int j=0;j<10;++j){
					v += coeff[j] * points[j][i];
				}
				jac.set(i,0,v);
			}

			kmb::Tetrahedron2::shapeFunction_dt(t[0],t[1],t[2],coeff);
			for(int i=0;i<3;++i){
				v = 0.0;
				for(int j=0;j<10;++j){
					v += coeff[j] * points[j][i];
				}
				jac.set(i,1,v);
			}


			kmb::Tetrahedron2::shapeFunction_du(t[0],t[1],t[2],coeff);
			for(int i=0;i<3;++i){
				v = 0.0;
				for(int j=0;j<10;++j){
					v += coeff[j] * points[j][i];
				}
				jac.set(i,2,v);
			}
			return true;
		}
		nr_local(const kmb::Point3D &t,const kmb::Point3D* pt)
		: target(t), points(pt){}
	};
	nr_local opt_obj(target,points);
	double min_t[3]  = { 0.0,  0.0,  0.0};
	double max_t[3]  = { 1.0,  1.0,  1.0};
	double init_t[3]  = { 0.25,  0.25,  0.25};
	kmb::Optimization opt;
	bool res = opt.calcZero_NR( opt_obj, naturalCoords, min_t, max_t, init_t );
	return res;
}

bool
kmb::Tetrahedron2::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[10];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<10;++j){
			target.addCoordinate(i,points[j].getCoordinate(i) * coeff[j]);
		}
	}
	return true;
}
