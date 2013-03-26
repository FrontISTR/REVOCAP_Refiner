/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Curve2D                                                 #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "Shape/kmbCurve2D.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbOptimization.h"
#include <cmath>

kmb::Curve2D::Curve2D(void)
: epsilon(1.0e-8), iterMax(1000)
{
}

kmb::Curve2D::~Curve2D(void)
{
}

double
kmb::Curve2D::getEpsilon(void) const
{
	return epsilon;
}

void
kmb::Curve2D::setEpsilon(double e)
{
	this->epsilon = e;
}

int
kmb::Curve2D::getIterMax(void) const
{
	return iterMax;
}

void
kmb::Curve2D::setIterMax(int m)
{
	this->iterMax = m;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

bool
kmb::Curve2D::getDerivative( double t, kmb::Vector2D& tangent ) const
{
	tangent.zero();
	return false;
}

bool
kmb::Curve2D::getSecondDerivative( double t, kmb::Vector2D& tangent ) const
{
	tangent.zero();
	return false;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

bool
kmb::Curve2D::getNearest( const kmb::Point2D& point, double &t ) const
{
	class dist_local : public kmb::OptTargetSS_0 {
	private:
		const kmb::Curve2D* curve;
		const kmb::Point2D target;
	public:
		double f(double t){
			kmb::Point2D pt;
			if( !curve->getPoint(t,pt) ){
				return DBL_MAX;
			}
			return target.distanceSq( pt );
		}
		dist_local(const kmb::Curve2D* c,const kmb::Point2D p)
		: curve(c), target(p){}
	};

	class opt_local : public kmb::OptTargetSS {
	private:
		const kmb::Curve2D* curve;
		const kmb::Point2D target;
		double t0;
		bool calculated;
		kmb::Point2D pt;
		kmb::Vector2D vec;
		kmb::Vector2D acc;

		bool calc(double t){
			if( t == t0 && calculated ){
				return true;
			}
			if( curve->getPoint(t,pt) && curve->getDerivative(t,vec)
				&& curve->getSecondDerivative(t,acc) ){
				t0 = t;
				calculated = true;
				return true;
			}else{
				calculated = false;
				return false;
			}
		}
	public:
		double f(double t){
			if( !calc(t) ){
				return DBL_MAX;
			}
			kmb::Vector2D d(pt,target);
			return d*vec;
		}
		double df(double t){
			if( !calc(t) ){
				return DBL_MAX;
			}
			kmb::Vector2D d(pt,target);
			return vec*vec + d*acc;
		}
		opt_local(const kmb::Curve2D* c,const kmb::Point2D p)
		: curve(c), target(p), t0(0.0), calculated(false){}
	};

	dist_local distObj( this, point );
	opt_local optObj( this, point );
	kmb::Optimization opt;
	opt.setEpsilon(epsilon);
	opt.setIterMax(iterMax);
	double min_t, max_t;
	getDomain(min_t,max_t);
	double t0 = 0.0;

	opt.calcMinOnGrid( distObj, t0, min_t, max_t, 10 );

	double t1 = opt.calcZero_DN( optObj, t0 );
	if( min_t <= t1 && t1 <= max_t ){
		t = t1;
		return true;
	}else{

		t1 = opt.calcMin_GS( distObj, min_t, max_t );
		if( min_t <= t1 && t1 <= max_t ){
			t = t1;
			return true;
		}
	}
	return false;
}

