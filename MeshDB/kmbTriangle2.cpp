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
#include "MeshDB/kmbTriangle2.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbSegment2.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"
#include "Geometry/kmbCircle.h"

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbOptimization.h"

/********************************************************************************
=begin

=== 2次三角形要素 (TRIANGLE2)

接続行列

	{  0,  1, -1,  0, -2,  2},
	{ -1,  0,  1,  2,  0, -2},
	{  1, -1,  0, -2,  2,  0},
	{  0, -2,  2,  0,  0,  0},
	{  2,  0, -2,  0,  0,  0},
	{ -2,  2,  0,  0,  0,  0},

辺

	{ 1, 2, 3},
	{ 2, 0, 4},
	{ 0, 1, 5},

=end

形状関数
0 : (1-s-t)*(1-2*s-2*t) => (s,t) = (  0,   0)
1 : s*(2*s-1)           => (s,t) = (  1,   0)
2 : t*(2*t-1)           => (s,t) = (  0,   1)
3 : 4*s*t               => (s,t) = (0.5, 0.5)
4 : 4*t*(1-s-t)         => (s,t) = (  0, 0.5)
5 : 4*s*(1-s-t)         => (s,t) = (0.5,   0)

*********************************************************************************/
const int kmb::Triangle2::nodeCount = 6;

const int kmb::Triangle2::connectionTable[6][6] =
{
	{  0,  1, -1,  0, -2,  2},
	{ -1,  0,  1,  2,  0, -2},
	{  1, -1,  0, -2,  2,  0},
	{  0, -2,  2,  0,  0,  0},
	{  2,  0, -2,  0,  0,  0},
	{ -2,  2,  0,  0,  0,  0},
};

const int kmb::Triangle2::faceTable[3][3] =
{
	{ 1, 2, 3},
	{ 2, 0, 4},
	{ 0, 1, 5},
};

kmb::Triangle2::Triangle2(void)
: kmb::Element(kmb::TRIANGLE2)
{
	cell = new kmb::nodeIdType[6];
}

kmb::Triangle2::Triangle2(kmb::nodeIdType *ary)
: kmb::Element(kmb::TRIANGLE2)
{
	cell = ary;
}

kmb::Triangle2::~Triangle2(void)
{
}

void
kmb::Triangle2::shapeFunction(double s,double t,double* coeff)
{
	coeff[0] = (1.0-s-t)*(1.0-2.0*s-2.0*t);
	coeff[1] = s*(2.0*s-1.0);
	coeff[2] = t*(2.0*t-1.0);
	coeff[3] = 4.0*s*t;
	coeff[4] = 4.0*t*(1.0-s-t);
	coeff[5] = 4.0*s*(1.0-s-t);
}

void
kmb::Triangle2::shapeFunction_ds(double s,double t,double* coeff)
{
	coeff[0] = -3.0 + 4.0*s + 4.0*t;
	coeff[1] = -1.0 + 4.0*s;
	coeff[2] =  0.0;
	coeff[3] =              + 4.0*t;
	coeff[4] =              - 4.0*t;
	coeff[5] =  4.0 - 8.0*s - 4.0*t;
}

void
kmb::Triangle2::shapeFunction_dt(double s,double t,double* coeff)
{
	coeff[0] = -3.0 + 4.0*s + 4.0*t;
	coeff[1] =  0.0;
	coeff[2] = -1.0         + 4.0*t;
	coeff[3] =      + 4.0*s;
	coeff[4] =  4.0 - 4.0*s - 8.0*t;
	coeff[5] =      - 4.0*s;
}


void
kmb::Triangle2::shapeFunction_dsds(double s,double t,double* coeff)
{
	coeff[0] =  4.0;
	coeff[1] =  4.0;
	coeff[2] =  0.0;
	coeff[3] =  0.0;
	coeff[4] =  0.0;
	coeff[5] = -8.0;
}

void
kmb::Triangle2::shapeFunction_dsdt(double s,double t,double* coeff)
{
	coeff[0] =  4.0;
	coeff[1] =  0.0;
	coeff[2] =  0.0;
	coeff[3] =  4.0;
	coeff[4] = -4.0;
	coeff[5] = -4.0;
}

void
kmb::Triangle2::shapeFunction_dtdt(double s,double t,double* coeff)
{
	coeff[0] =  4.0;
	coeff[1] =  0.0;
	coeff[2] =  4.0;
	coeff[3] =  0.0;
	coeff[4] = -8.0;
	coeff[5] =  0.0;
}

double
kmb::Triangle2::checkShapeFunctionDomain(double s,double t)
{
	return kmb::Minimizer::getMin( 1.0-s-t, s, t );
}




bool
kmb::Triangle2::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[2])
{
	if( points == NULL ){
		return false;
	}
	/*
	 * 3角形2次の要素座標を求めるためにニュートン法を行う
	 */
	class nr_local : public kmb::OptTargetVV {
	public:
		kmb::Point3D target;
		const kmb::Point3D* points;
		int getDomainDim(void) const { return 2; };
		int getRangeDim(void) const { return 2; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			double coeff[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Triangle2::shapeFunction( t[0], t[1], coeff );
			double g[3] = {0.0,0.0,0.0};
			double dgds[3] = {0.0,0.0,0.0};
			double dgdt[3] = {0.0,0.0,0.0};
			for(int i=0;i<6;++i){
				g[0] += coeff[i] * points[i].x();
				g[1] += coeff[i] * points[i].y();
				g[2] += coeff[i] * points[i].z();
			}
			g[0] -= target[0];
			g[1] -= target[1];
			g[2] -= target[2];

			kmb::Triangle2::shapeFunction_ds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgds[i] = 0.0;
				for(int j=0;j<6;++j){
					dgds[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Triangle2::shapeFunction_dt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdt[i] = 0.0;
				for(int j=0;j<6;++j){
					dgdt[i] += coeff[j] * points[j][i];
				}
			}
			val[0] = kmb::Vector3D::inner(g,dgds);
			val[1] = kmb::Vector3D::inner(g,dgdt);
			return true;
		}
		bool df(const ColumnVector &t,Matrix &jac){
			double g[3] = {0.0,0.0,0.0};
			double dgds[3] = {0.0,0.0,0.0};
			double dgdt[3] = {0.0,0.0,0.0};
			double dgdss[3] = {0.0,0.0,0.0};
			double dgdst[3] = {0.0,0.0,0.0};
			double dgdtt[3] = {0.0,0.0,0.0};
			double coeff[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Triangle2::shapeFunction( t[0], t[1], coeff );
			for(int i=0;i<6;++i){
				g[0] += coeff[i] * points[i].x();
				g[1] += coeff[i] * points[i].y();
				g[2] += coeff[i] * points[i].z();
			}
			g[0] -= target[0];
			g[1] -= target[1];
			g[2] -= target[2];

			kmb::Triangle2::shapeFunction_ds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgds[i] = 0.0;
				for(int j=0;j<6;++j){
					dgds[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Triangle2::shapeFunction_dt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdt[i] = 0.0;
				for(int j=0;j<6;++j){
					dgdt[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Triangle2::shapeFunction_dsds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdss[i] = 0.0;
				for(int j=0;j<6;++j){
					dgdss[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Triangle2::shapeFunction_dsdt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdst[i] = 0.0;
				for(int j=0;j<6;++j){
					dgdst[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Triangle2::shapeFunction_dsds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdtt[i] = 0.0;
				for(int j=0;j<6;++j){
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
	double min_t[2]  = { 0.0,  0.0};
	double max_t[2]  = { 1.0,  1.0};
	kmb::Optimization opt;
	bool res = opt.calcZero_NR( opt_obj, naturalCoords, min_t, max_t );
	return res;
}

bool
kmb::Triangle2::getPhysicalCoordinates(const double naturalCoords[2],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	shapeFunction( naturalCoords[0], naturalCoords[1], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<6;++j){
			target.addCoordinate(i,points[j].getCoordinate(i) * coeff[j]);
		}
	}
	return true;
}

