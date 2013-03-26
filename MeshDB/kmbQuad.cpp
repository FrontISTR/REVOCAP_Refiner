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
#include "MeshDB/kmbQuad.h"
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbGeometry3D.h"
#include <cfloat>

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbOptimization.h"

/********************************************************************************
=begin

=== 1次四角形要素 (QUAD)

接続行列

	{ 0, 1, 0,-1},
	{-1, 0, 1, 0},
	{ 0,-1, 0, 1},
	{ 1, 0,-1, 0}

辺

	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 3, 0},

=end

形状関数
0 : 1/4(1-s)(1-t) => (s,t) = (-1,-1)
1 : 1/4(1+s)(1-t) => (s,t) = ( 1,-1)
2 : 1/4(1+s)(1+t) => (s,t) = ( 1, 1)
3 : 1/4(1-s)(1+t) => (s,t) = (-1, 1)

*********************************************************************************/
const int kmb::Quad::nodeCount = 4;

const int kmb::Quad::connectionTable[4][4] =
{
	{ 0, 1, 0,-1},
	{-1, 0, 1, 0},
	{ 0,-1, 0, 1},
	{ 1, 0,-1, 0}
};

const int kmb::Quad::faceTable[4][2] =
{
	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 3, 0},
};

kmb::Quad::Quad(void)
: kmb::Element(kmb::QUAD)
{
	cell = new kmb::nodeIdType[4];
}

kmb::Quad::Quad(kmb::nodeIdType *ary)
: kmb::Element(kmb::QUAD)
{
	cell = ary;
}

kmb::Quad::Quad(kmb::nodeIdType i0,kmb::nodeIdType i1,kmb::nodeIdType i2,kmb::nodeIdType i3)
: kmb::Element(kmb::QUAD)
{
	cell = new kmb::nodeIdType[4];
	cell[0] = i0;
	cell[1] = i1;
	cell[2] = i2;
	cell[3] = i3;
}

kmb::Quad::~Quad(void)
{
}

void
kmb::Quad::shapeFunction(double s,double t,double* coeff)
{
	coeff[0] = 0.25*(1.0-s)*(1.0-t);
	coeff[1] = 0.25*(1.0+s)*(1.0-t);
	coeff[2] = 0.25*(1.0+s)*(1.0+t);
	coeff[3] = 0.25*(1.0-s)*(1.0+t);
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif
#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif
void
kmb::Quad::shapeFunction_ds(double s,double t,double* coeff)
{
	coeff[0] = 0.25*(-1.0+t);
	coeff[1] = 0.25*( 1.0-t);
	coeff[2] = 0.25*( 1.0+t);
	coeff[3] = 0.25*(-1.0-t);
}

void
kmb::Quad::shapeFunction_dt(double s,double t,double* coeff)
{
	coeff[0] = 0.25*(-1.0+s);
	coeff[1] = 0.25*(-1.0-s);
	coeff[2] = 0.25*( 1.0+s);
	coeff[3] = 0.25*( 1.0-s);
}

void
kmb::Quad::shapeFunction_dsds(double s,double t,double* coeff)
{
	coeff[0] = 0.0;
	coeff[1] = 0.0;
	coeff[2] = 0.0;
	coeff[3] = 0.0;
}

void
kmb::Quad::shapeFunction_dsdt(double s,double t,double* coeff)
{
	coeff[0] =  0.25;
	coeff[1] = -0.25;
	coeff[2] =  0.25;
	coeff[3] = -0.25;
}

void
kmb::Quad::shapeFunction_dtdt(double s,double t,double* coeff)
{
	coeff[0] = 0.0;
	coeff[1] = 0.0;
	coeff[2] = 0.0;
	coeff[3] = 0.0;
}
#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

double
kmb::Quad::checkShapeFunctionDomain(double s,double t)
{
	kmb::Minimizer minimizer;
	minimizer.update( 1.0-s );
	minimizer.update( 1.0+s );
	minimizer.update( 1.0-t );
	minimizer.update( 1.0+t );
	return minimizer.getMin();
}



bool
kmb::Quad::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[2])
{
	if( points == NULL ){
		return false;
	}
	/*
	 * 4角形の要素座標を求めるためにニュートン法を行う
	 */
	class nr_local : public kmb::OptTargetVV {
	public:
		kmb::Point3D target;
		const kmb::Point3D* points;
		int getDomainDim(void) const { return 2; };
		int getRangeDim(void) const { return 2; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			int len = kmb::Element::getNodeCount(kmb::QUAD);
			double coeff[4] = { 0.0, 0.0, 0.0, 0.0 };
			kmb::Quad::shapeFunction( t[0], t[1], coeff );
			double g[3] = {0.0,0.0,0.0};
			double dgds[3] = {0.0,0.0,0.0};
			double dgdt[3] = {0.0,0.0,0.0};
			for(int i=0;i<len;++i){
				g[0] += coeff[i] * points[i].x();
				g[1] += coeff[i] * points[i].y();
				g[2] += coeff[i] * points[i].z();
			}
			g[0] -= target[0];
			g[1] -= target[1];
			g[2] -= target[2];

			kmb::Quad::shapeFunction_ds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgds[i] = 0.0;
				for(int j=0;j<len;++j){
					dgds[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad::shapeFunction_dt(t[0],t[1],coeff);
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
			int len = kmb::Element::getNodeCount(kmb::QUAD);
			double g[3] = {0.0,0.0,0.0};
			double dgds[3] = {0.0,0.0,0.0};
			double dgdt[3] = {0.0,0.0,0.0};
			double dgdss[3] = {0.0,0.0,0.0};
			double dgdst[3] = {0.0,0.0,0.0};
			double dgdtt[3] = {0.0,0.0,0.0};
			double coeff[4] = { 0.0, 0.0, 0.0, 0.0 };
			kmb::Quad::shapeFunction( t[0], t[1], coeff );
			for(int i=0;i<len;++i){
				g[0] += coeff[i] * points[i].x();
				g[1] += coeff[i] * points[i].y();
				g[2] += coeff[i] * points[i].z();
			}
			g[0] -= target[0];
			g[1] -= target[1];
			g[2] -= target[2];

			kmb::Quad::shapeFunction_ds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgds[i] = 0.0;
				for(int j=0;j<len;++j){
					dgds[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad::shapeFunction_dt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdt[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdt[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad::shapeFunction_dsds(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdss[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdss[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad::shapeFunction_dsdt(t[0],t[1],coeff);
			for(int i=0;i<3;++i){
				dgdst[i] = 0.0;
				for(int j=0;j<len;++j){
					dgdst[i] += coeff[j] * points[j][i];
				}
			}

			kmb::Quad::shapeFunction_dtdt(t[0],t[1],coeff);
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
kmb::Quad::getPhysicalCoordinates(const double naturalCoords[2],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[4] = { 0.0, 0.0, 0.0, 0.0 };
	shapeFunction( naturalCoords[0], naturalCoords[1], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<4;++j){
			target.addCoordinate(i,points[j].getCoordinate(i) * coeff[j]);
		}
	}
	return true;
}

bool
kmb::Quad::edgeSwap(kmb::ElementBase &quad0,kmb::ElementBase &quad1,bool orientation)
{
	bool swapped = false;
	if( quad0.getType() == kmb::QUAD && quad1.getType() == kmb::QUAD )
	{

		unsigned int rel = 0;
		unsigned int flag = 0x0001;
		for(int i=0;i<4;++i){
			for(int j=0;j<4;++j){
				if( quad0.getCellId(i) == quad1.getCellId(j) )	rel |= flag;
				flag = flag << 1;
			}
		}
		kmb::nodeIdType nodes[6]
			= {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};

		switch( rel ){
		case 0x0081:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0012:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0024:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0048:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0810:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x0120:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x0240:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x0480:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x8100:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x1200:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x2400:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x4800:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x1008:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		case 0x2001:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		case 0x4002:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		case 0x8004:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		default:
			break;
		}
		if( swapped ){
			if( orientation ){

				quad0.setCellId(0, nodes[1]);
				quad0.setCellId(1, nodes[2]);
				quad0.setCellId(2, nodes[3]);
				quad0.setCellId(3, nodes[4]);
				quad1.setCellId(0, nodes[0]);
				quad1.setCellId(1, nodes[1]);
				quad1.setCellId(2, nodes[4]);
				quad1.setCellId(3, nodes[5]);
			}else{

				quad0.setCellId(0, nodes[0]);
				quad0.setCellId(1, nodes[1]);
				quad0.setCellId(2, nodes[2]);
				quad0.setCellId(3, nodes[5]);
				quad1.setCellId(0, nodes[2]);
				quad1.setCellId(1, nodes[3]);
				quad1.setCellId(2, nodes[4]);
				quad1.setCellId(3, nodes[5]);
			}
		}
	}
	return swapped;
}

int
kmb::Quad::isCoincident(kmb::nodeIdType t00,kmb::nodeIdType t01,kmb::nodeIdType t02,kmb::nodeIdType t03,
						kmb::nodeIdType t10,kmb::nodeIdType t11,kmb::nodeIdType t12,kmb::nodeIdType t13)
{
	unsigned int rel = 0;
	if( t00 == t10 )	rel |= 0x1000;
	if( t00 == t11 )	rel |= 0x2000;
	if( t00 == t12 )	rel |= 0x4000;
	if( t00 == t13 )	rel |= 0x8000;
	if( t01 == t10 )	rel |= 0x0100;
	if( t01 == t11 )	rel |= 0x0200;
	if( t01 == t12 )	rel |= 0x0400;
	if( t01 == t13 )	rel |= 0x0800;
	if( t02 == t10 )	rel |= 0x0010;
	if( t02 == t11 )	rel |= 0x0020;
	if( t02 == t12 )	rel |= 0x0040;
	if( t02 == t13 )	rel |= 0x0080;
	if( t03 == t10 )	rel |= 0x0001;
	if( t03 == t11 )	rel |= 0x0002;
	if( t03 == t12 )	rel |= 0x0004;
	if( t03 == t13 )	rel |= 0x0008;
	switch( rel ){

	case 0x1248:
	case 0x2481:
	case 0x4812:
	case 0x8124:
		return 1;

	case 0x8421:
	case 0x4218:
	case 0x2184:
	case 0x1842:
		return -1;
	default:
		return 0;
	}
}

double
kmb::Quad::jacobian(double s, double t,const kmb::Point2D* points)
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	double jsx = 0.25 * ( -points[0].x() + points[1].x() + points[2].x() - points[3].x() )
		+ 0.25 * t * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() );
	double jsy = 0.25 * ( -points[0].y() + points[1].y() + points[2].y() - points[3].y() )
		+ 0.25 * t * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() );
	double jtx = 0.25 * ( -points[0].x() - points[1].x() + points[2].x() + points[3].x() )
		+ 0.25 * s * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() );
	double jty = 0.25 * ( -points[0].y() - points[1].y() + points[2].y() + points[3].y() )
		+ 0.25 * s * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() );
	return jsx * jty - jsy * jtx;
}





#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

double
kmb::Quad::jacobian_ds(double s, double t,const kmb::Point2D* points)
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	double jsxs = 0.25 * ( -points[0].x() + points[1].x() + points[2].x() - points[3].x() );
	double jsys = 0.25 * ( -points[0].y() + points[1].y() + points[2].y() - points[3].y() );
	double jtxs = 0.25 * (  points[0].x() - points[1].x() + points[2].x() - points[3].x() );
	double jtys = 0.25 * (  points[0].y() - points[1].y() + points[2].y() - points[3].y() );
	return jsxs * jtys - jsys * jtxs;
}

double
kmb::Quad::jacobian_dt(double s, double t,const kmb::Point2D* points)
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	double jsxt = 0.25 * (  points[0].x() - points[1].x() + points[2].x() - points[3].x() );
	double jsyt = 0.25 * (  points[0].y() - points[1].y() + points[2].y() - points[3].y() );
	double jtxt = 0.25 * ( -points[0].x() - points[1].x() + points[2].x() + points[3].x() );
	double jtyt = 0.25 * ( -points[0].y() - points[1].y() + points[2].y() + points[3].y() );
	return jsxt * jtyt - jsyt * jtxt;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
