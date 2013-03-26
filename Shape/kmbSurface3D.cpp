/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Surface3D                                               #
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
#include "Shape/kmbSurface3D.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbOptimization.h"
#include "Matrix/kmbVector.h"

kmb::Surface3D::Surface3D(void)
: epsilon(1.0e-8), iterMax(1000), surfaceId(-1)
{
}

kmb::Surface3D::~Surface3D(void)
{
}

double
kmb::Surface3D::getEpsilon(void) const
{
	return epsilon;
}

void
kmb::Surface3D::setEpsilon(double e)
{
	this->epsilon = e;
}

int
kmb::Surface3D::getIterMax(void) const
{
	return iterMax;
}

void
kmb::Surface3D::setIterMax(int m)
{
	this->iterMax = m;
}

long
kmb::Surface3D::getSurfaceId(void) const
{
	return this->surfaceId;
}

void
kmb::Surface3D::setSurfaceId(long surf_id)
{
	this->surfaceId = surf_id;
}

bool
kmb::Surface3D::getMiddlePoint( double u0, double v0, double u1, double v1, double &u, double &v, kmb::Point3D &point ) const
{
	u = 0.5*(u0+u1);
	v = 0.5*(v0+v1);
	return getPoint(u,v,point);
}

bool
kmb::Surface3D::getMiddlePointByNearest( double u0, double v0, double u1, double v1, double &u, double &v, kmb::Point3D &point ) const
{
	kmb::Point3D p0,p1,pm,pt;
	if( !getPoint(u0,v0,p0) ){
		return false;
	}
	if( !getPoint(u1,v1,p1) ){
		return false;
	}

	pm = kmb::Point3D::getCenter(p0,p1);

	kmb::Minimizer minimizer;

	bool res = false;
	double um = 0.5*(u0+u1);
	double vm = 0.5*(v0+v1);

	if( getPoint(um,vm,pt) && minimizer.update( pm.distanceSq(pt) ) ){
		u = um;
		v = vm;
		point.set(pt);
		res = true;
	}

	if( getNearest(pm,um,vm) && getPoint(um,vm,pt) && minimizer.update( pm.distanceSq(pt) ) ){
		u = um;
		v = vm;
		point.set(pt);
		res = true;
	}
	return res;
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
kmb::Surface3D::getDerivative( derivativeType d, double u, double v, kmb::Vector3D& tangent ) const
{
	return false;
}

bool
kmb::Surface3D::getBoundingBox( kmb::BoundingBox &bbox ) const
{
	bbox = this->bbox;
	return true;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

bool
kmb::Surface3D::getNearest( const kmb::Point3D& point, double &u, double &v ) const
{
	class dist_local : public kmb::OptTargetSV_0 {
	private:
		const kmb::Surface3D* surface;
		const kmb::Point3D target;
	public:
		int getDomainDim(void) const{
			return 2;
		};
		double f(const double* t){
			kmb::Point3D pt;
			if( !surface->getPoint(t[0],t[1],pt) ){
				return DBL_MAX;
			}
			return target.distanceSq( pt );
		}
		dist_local(const kmb::Surface3D* s,const kmb::Point3D p)
		: surface(s), target(p){}
	};
	dist_local dist_obj(this,point);

	class opt_local : public kmb::OptTargetVV {
	private:
		const kmb::Surface3D* surface;
		const kmb::Point3D target;
		double t0,t1;
		bool calculated;
		kmb::Point3D pt;
		kmb::Vector3D uVec, vVec, uuVec, uvVec, vvVec;

		bool calc(double u,double v){
			if( u == t0 && v == t1 && calculated ){
				return true;
			}
			if( surface->getPoint(u,v,pt) &&
					surface->getDerivative( kmb::Surface3D::DER_U, u, v, uVec) &&
					surface->getDerivative( kmb::Surface3D::DER_V, u, v, vVec) &&
					surface->getDerivative( kmb::Surface3D::DER_UU, u, v, uuVec) &&
					surface->getDerivative( kmb::Surface3D::DER_UV, u, v, uvVec) &&
					surface->getDerivative( kmb::Surface3D::DER_VV, u, v, vvVec) )
			{
				t0 = u;
				t1 = v;
				calculated = true;
				return true;
			}else{
				calculated = false;
				return false;
			}
		}
	public:
		int getDomainDim(void) const{
			return 2;
		}
		int getRangeDim(void) const{
			return 2;
		}
		bool f(const ColumnVector &t,ColumnVector &val){
			if( !calc(t[0],t[1]) ){
				return false;
			}
			kmb::Vector3D d(pt,target);
			val[0] = d*uVec;
			val[1] = d*vVec;
			return true;
		}
		bool df(const ColumnVector &t,Matrix &jac){
			if( !calc(t[0],t[1]) ){
				return false;
			}
			kmb::Vector3D d(pt,target);
			jac.set(0,0,uVec*uVec+d*uuVec);
			jac.set(0,1,uVec*vVec+d*uvVec);
			jac.set(1,0,vVec*uVec+d*uvVec);
			jac.set(1,1,vVec*vVec+d*vvVec);
			return true;
		}
		opt_local(const kmb::Surface3D* s,const kmb::Point3D p)
		: surface(s), target(p), t0(0.0), t1(0.0), calculated(false){}
	};
	opt_local opt_obj(this,point);
	kmb::Optimization opt;
	opt.setEpsilon(epsilon);
	opt.setIterMax(iterMax);
	double min_t[2]={0.0,0.0}, max_t[2]={0.0,0.0};
	getDomain(min_t[0],max_t[0],min_t[1],max_t[1]);
	double t0[2] = {0.0,0.0};
	double opt_t[2] = {0.0,0.0};
	int div[2] = {10,10};
	opt.calcMinOnGrid( dist_obj, t0, min_t, max_t, div );
	if( opt.calcZero_DN( opt_obj, opt_t, t0 ) &&
		min_t[0] <= opt_t[0] && opt_t[0] <= max_t[0] &&
		min_t[1] <= opt_t[1] && opt_t[1] <= max_t[1] ){
		u = opt_t[0];
		v = opt_t[1];
		return true;
	}

	if( opt.calcMin_GS( dist_obj, opt_t, min_t, max_t ) ){
		u = opt_t[0];
		v = opt_t[1];
		return true;
	}
	return false;
}

