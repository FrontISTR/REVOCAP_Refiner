/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Quad2                                                   #
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
#include "MeshDB/kmbQuad2.h"
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbGeometry3D.h"
#include <cfloat>

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbOptimization.h"

/********************************************************************************
=begin

=== 2次四角形要素 (QUAD2)

接続行列

	{ 0, 1, 0,-1, 2, 0, 0,-2},
	{-1, 0, 1, 0,-2, 2, 0, 0},
	{ 0,-1, 0, 1, 0,-2, 2, 0},
	{ 1, 0,-1, 0, 0, 0,-2, 2},
	{-2, 2, 0, 0, 0, 0, 0, 0},
	{ 0,-2, 2, 0, 0, 0, 0, 0},
	{ 0, 0,-2, 2, 0, 0, 0, 0},
	{ 2, 0, 0,-2, 0, 0, 0, 0},

辺

	{ 0, 1, 4},
	{ 1, 2, 5},
	{ 2, 3, 6},
	{ 3, 0, 7},

=end

形状関数 serendipity
0 : 1/4(1-s)(1-t)(-1-s-t) => (s,t) = (-1,-1)
1 : 1/4(1+s)(1-t)(-1+s-t) => (s,t) = ( 1,-1)
2 : 1/4(1+s)(1+t)(-1+s+t) => (s,t) = ( 1, 1)
3 : 1/4(1-s)(1+t)(-1-s+t) => (s,t) = (-1, 1)
4 : 1/2(1-s*s)(1-t)       => (s,t) = ( 0,-1)
5 : 1/2(1+s)(1-t*t)       => (s,t) = ( 1, 0)
6 : 1/2(1-s*s)(1+t)       => (s,t) = ( 0, 1)
7 : 1/2(1-s)(1-t*t)       => (s,t) = (-1, 0)

*********************************************************************************/
const int kmb::Quad2::nodeCount = 8;

const int kmb::Quad2::connectionTable[8][8] =
{
	{ 0, 1, 0,-1, 2, 0, 0,-2},
	{-1, 0, 1, 0,-2, 2, 0, 0},
	{ 0,-1, 0, 1, 0,-2, 2, 0},
	{ 1, 0,-1, 0, 0, 0,-2, 2},
	{-2, 2, 0, 0, 0, 0, 0, 0},
	{ 0,-2, 2, 0, 0, 0, 0, 0},
	{ 0, 0,-2, 2, 0, 0, 0, 0},
	{ 2, 0, 0,-2, 0, 0, 0, 0},
};

const int kmb::Quad2::faceTable[4][3] =
{
	{ 0, 1, 4},
	{ 1, 2, 5},
	{ 2, 3, 6},
	{ 3, 0, 7},
};

kmb::Quad2::Quad2(void)
: Element(kmb::QUAD2)
{
	cell = new kmb::nodeIdType[8];
}

kmb::Quad2::Quad2(kmb::nodeIdType *ary)
: Element(kmb::QUAD2)
{
	cell = ary;
}

kmb::Quad2::~Quad2(void)
{
}

void
kmb::Quad2::shapeFunction(double s,double t,double* coeff)
{
	coeff[0] = 0.25*(1.0-s)*(1.0-t)*(-1.0-s-t);
	coeff[1] = 0.25*(1.0+s)*(1.0-t)*(-1.0+s-t);
	coeff[2] = 0.25*(1.0+s)*(1.0+t)*(-1.0+s+t);
	coeff[3] = 0.25*(1.0-s)*(1.0+t)*(-1.0-s+t);
	coeff[4] = 0.5*(1.0-s*s)*(1.0-t);
	coeff[5] = 0.5*(1.0+s)*(1.0-t*t);
	coeff[6] = 0.5*(1.0-s*s)*(1.0+t);
	coeff[7] = 0.5*(1.0-s)*(1.0-t*t);
}

void
kmb::Quad2::shapeFunction_ds(double s,double t,double* coeff)
{
	coeff[0] = 0.25 * (     + 2.0*s + t - 2.0*s*t - t*t);
	coeff[1] = 0.25 * (     + 2.0*s - t - 2.0*s*t + t*t);
	coeff[2] = 0.25 * (     + 2.0*s + t + 2.0*s*t + t*t);
	coeff[3] = 0.25 * (     + 2.0*s - t + 2.0*s*t - t*t);
	coeff[4] =        (     -     s     +     s*t      );
	coeff[5] = 0.5  * (+1.0                       - t*t);
	coeff[6] =        (     -     s     -     s*t      );
	coeff[7] = 0.5  * (-1.0                       + t*t);
}

void
kmb::Quad2::shapeFunction_dt(double s,double t,double* coeff)
{
	coeff[0] = 0.25 * (     + s + 2.0*t - 2.0*s*t - s*s);
	coeff[1] = 0.25 * (     - s + 2.0*t + 2.0*s*t - s*s);
	coeff[2] = 0.25 * (     + s + 2.0*t + 2.0*s*t + s*s);
	coeff[3] = 0.25 * (     - s + 2.0*t - 2.0*s*t + s*s);
	coeff[4] = 0.5  * (-1.0                       + s*s);
	coeff[5] =        (         -     t -     s*t      );
	coeff[6] = 0.5  * (+1.0                       - s*s);
	coeff[7] =        (         -     t +     s*t      );
}

void
kmb::Quad2::shapeFunction_dsds(double s,double t,double* coeff)
{
	coeff[0] =  0.5 - 0.5*t;
	coeff[1] =  0.5 - 0.5*t;
	coeff[2] =  0.5 + 0.5*t;
	coeff[3] =  0.5 + 0.5*t;
	coeff[4] = -1.0 +     t;
	coeff[5] =  0.0;
	coeff[6] = -1.0 -     t;
	coeff[7] =  0.0;
}

void
kmb::Quad2::shapeFunction_dsdt(double s,double t,double* coeff)
{
	coeff[0] = 0.25 * ( 1.0 - 2.0*s - 2.0*t);
	coeff[1] = 0.25 * (-1.0 - 2.0*s + 2.0*t);
	coeff[2] = 0.25 * ( 1.0 + 2.0*s + 2.0*t);
	coeff[3] = 0.25 * (-1.0 + 2.0*s - 2.0*t);
	coeff[4] = s;
	coeff[5] = -t;
	coeff[6] = -s;
	coeff[7] = t;
}

void
kmb::Quad2::shapeFunction_dtdt(double s,double t,double* coeff)
{
	coeff[0] = 0.5 - 0.5*s;
	coeff[1] = 0.5 + 0.5*s;
	coeff[2] = 0.5 + 0.5*s;
	coeff[3] = 0.5 - 0.5*s;
	coeff[4] = 0.0;
	coeff[5] = -1.0 - s;
	coeff[6] = 0.0;
	coeff[7] = -1.0 + s;
}

double
kmb::Quad2::checkShapeFunctionDomain(double s,double t)
{
	kmb::Minimizer minimizer;
	minimizer.update( 1.0-s );
	minimizer.update( 1.0+s );
	minimizer.update( 1.0-t );
	minimizer.update( 1.0+t );
	return minimizer.getMin();
}

bool
kmb::Quad2::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[2])
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
		int getDomainDim(void) const { return 2; };
		int getRangeDim(void) const { return 2; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			double coeff[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Quad2::shapeFunction( t[0], t[1], coeff );
			double g[3] = {0.0,0.0,0.0};
			double dgds[3] = {0.0,0.0,0.0};
			double dgdt[3] = {0.0,0.0,0.0};
			int len = kmb::Element::getNodeCount(kmb::QUAD2);
			for(int i=0;i<len;++i){
				g[0] += coeff[i] * points[i].x();
				g[1] += coeff[i] * points[i].y();
				g[2] += coeff[i] * points[i].z();
			}
			g[0] -= target[0];
			g[1] -= target[1];
			g[2] -= target[2];

			kmb::Quad2::shapeFunction_ds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgds[i] = 0.0;
				for(int j=0;j<len;++j){
					dgds[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad2::shapeFunction_dt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdt[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdt[i] += coeff[j] * points[j][i];
				}
			}
			val[0] = kmb::Vector3D::inner(g,dgds);
			val[1] = kmb::Vector3D::inner(g,dgdt);
			return true;
		}
		bool df(const ColumnVector &t,Matrix &jac){
			int len = kmb::Element::getNodeCount(kmb::QUAD2);
			double g[3] = {0.0,0.0,0.0};
			double dgds[3] = {0.0,0.0,0.0};
			double dgdt[3] = {0.0,0.0,0.0};
			double dgdss[3] = {0.0,0.0,0.0};
			double dgdst[3] = {0.0,0.0,0.0};
			double dgdtt[3] = {0.0,0.0,0.0};
			double coeff[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Quad2::shapeFunction( t[0], t[1], coeff );
			for(int i=0;i<len;++i){
				g[0] += coeff[i] * points[i].x();
				g[1] += coeff[i] * points[i].y();
				g[2] += coeff[i] * points[i].z();
			}
			g[0] -= target[0];
			g[1] -= target[1];
			g[2] -= target[2];

			kmb::Quad2::shapeFunction_ds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgds[i] = 0.0;
				for(int j=0;j<len;++j){
					dgds[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad2::shapeFunction_dt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdt[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdt[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad2::shapeFunction_dsds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdss[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdss[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad2::shapeFunction_dsdt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdst[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdst[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad2::shapeFunction_dtdt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdtt[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdtt[i] += coeff[j] * points[j][i];
				}
			}
			jac.set(0,0,kmb::Vector3D::inner(dgds,dgds)+kmb::Vector3D::inner(g,dgdss));
			jac.set(0,1,kmb::Vector3D::inner(dgds,dgdt)+kmb::Vector3D::inner(g,dgdst));
			jac.set(1,0,jac.get(0,1));
			jac.set(1,1,kmb::Vector3D::inner(dgdt,dgdt)+kmb::Vector3D::inner(g,dgdtt));
			return true;
		}
		nr_local(const kmb::Point3D &t,const kmb::Point3D* pt)
		: target(t), points(pt){}
	};
	nr_local opt_obj(target,points);


	double init_t[2]  = { 0.0,  0.0};
	kmb::Optimization opt;
	bool res = opt.calcZero_DN( opt_obj, naturalCoords, init_t );
	return res;
}

bool
kmb::Quad2::getPhysicalCoordinates(const double naturalCoords[2],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	shapeFunction( naturalCoords[0], naturalCoords[1], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<8;++j){
			target.addCoordinate(i,points[j].getCoordinate(i) * coeff[j]);
		}
	}
	return true;
}

