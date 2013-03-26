/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NurbsSurface3D                                          #
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

#include "Shape/kmbNurbsSurface3D.h"
#include "Geometry/kmbOptimization.h"
#include "Common/kmbCalculator.h"
#include "Matrix/kmbVector.h"
#include "Matrix/kmbMatrix_DoubleArray.h"
#include <cmath>

#ifdef _DEBUG
int kmb::NurbsSurface3D::calcCount(0);
#endif

kmb::NurbsSurface3D::NurbsSurface3D(void)
: kmb::Surface3D()
, uOrder(0)
, vOrder(0)
{
}

kmb::NurbsSurface3D::~NurbsSurface3D(void)
{
}

void
kmb::NurbsSurface3D::clear(void)
{
	ctrlPts.clear();
	uBspline.clear();
	vBspline.clear();
	bbox.initialize();
	this->uOrder = 0;
	this->vOrder = 0;
}

kmb::Surface3D::surfaceType
kmb::NurbsSurface3D::getSurfaceType(void) const
{
	return kmb::Surface3D::NURBS;
}

void
kmb::NurbsSurface3D::getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const
{
	uBspline.getDomain( min_u, max_u );
	vBspline.getDomain( min_v, max_v );
}

bool
kmb::NurbsSurface3D::isDomain( double u, double v ) const
{
	return uBspline.isDomain(u) && vBspline.isDomain(v);
}

bool
kmb::NurbsSurface3D::isUDomain( double u ) const
{
	return uBspline.isDomain(u);
}

bool
kmb::NurbsSurface3D::isVDomain( double v ) const
{
	return vBspline.isDomain(v);
}

bool
kmb::NurbsSurface3D::setOrder(unsigned int uOrder,unsigned int vOrder)
{
	this->uOrder = uOrder;
	this->vOrder = vOrder;
	return valid();
}

bool
kmb::NurbsSurface3D::getOrder(unsigned int &uOrder,unsigned int &vOrder) const
{
	uOrder = this->uOrder;
	vOrder = this->vOrder;
	return valid();
}

bool
kmb::NurbsSurface3D::getKnotCount(unsigned int &uCount,unsigned int &vCount) const
{
	uCount = uBspline.getKnotCount();
	vCount = vBspline.getKnotCount();
	return valid();
}

void
kmb::NurbsSurface3D::getDegree(unsigned int &uDegree,unsigned int &vDegree) const
{
	uDegree = this->uOrder-1;
	vDegree = this->vOrder-1;
}

int
kmb::NurbsSurface3D::getCtrlPtCount(void) const
{
	return static_cast<int>( ctrlPts.size() );
}

void
kmb::NurbsSurface3D::appendUKnot(double k)
{
	uBspline.appendKnot(k);
}

void
kmb::NurbsSurface3D::appendVKnot(double k)
{
	vBspline.appendKnot(k);
}

void
kmb::NurbsSurface3D::appendCtrlPt(double x,double y,double z,double w)
{
	this->ctrlPts.push_back( kmb::Point4D(x,y,z,w) );
	this->bbox.update(x,y,z);
}

bool
kmb::NurbsSurface3D::getCtrlPt(int i,kmb::Point4D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

double
kmb::NurbsSurface3D::getUKnot(int i) const
{
	return uBspline.getKnot(i);
}

double
kmb::NurbsSurface3D::getVKnot(int i) const
{
	return vBspline.getKnot(i);
}

bool
kmb::NurbsSurface3D::valid(void) const
{
	return ( (uBspline.getKnotCount()-uOrder)*(vBspline.getKnotCount()-vOrder) == ctrlPts.size() );
}

bool
kmb::NurbsSurface3D::getDerivative( derivativeType d, double u, double v, kmb::Vector3D& tangent ) const
{
	kmb::Vector3D sub;
	double w = 0.0;
	if( getSubDerivative( d, u, v, sub ) && getWeight(u,v,w) ){
		tangent = (1.0/w) * sub;
		return true;
	}
	return false;
}


bool
kmb::NurbsSurface3D::getPoint( double u, double v, kmb::Point3D& point ) const
{
	point.zero();
	if( !valid() ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;
	double weight = 0.0;


	double* u0 = new double[uNum];
	double* v0 = new double[vNum];
	for(int i=0;i<uNum;++i){
		u0[i] = uBspline.getValue(i,uOrder-1,u);
	}
	for(int j=0;j<vNum;++j){
		v0[j] = vBspline.getValue(j,vOrder-1,v);
	}
	for(int j=0;j<vNum;++j){
		for(int i=0;i<uNum;++i){
			point.addCoordinate(
				u0[i]*v0[j]*ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w(),
				u0[i]*v0[j]*ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w(),
				u0[i]*v0[j]*ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() );
			weight += u0[i]*v0[j]*ctrlPts[i+j*uNum].w();
		}
	}
	delete[] v0;
	delete[] u0;

	if( weight != 0.0 ){
		point.scale( 1.0/weight );
	}
	return true;
}

bool
kmb::NurbsSurface3D::getMiddlePoint( double u0, double v0, double u1, double v1, double &u, double &v, kmb::Point3D &point ) const
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

	if( getPoint(0.5*(u0+u1),0.5*(v0+v1),pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = 0.5*(u0+u1);
		v = 0.5*(v0+v1);
		point.set( pt );
		res = true;
	}

	if( getPoint(u0,0.5*(v0+v1),pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = u0;
		v = 0.5*(v0+v1);
		point.set( pt );
		res = true;
	}

	if( getPoint(u1,0.5*(v0+v1),pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = u1;
		v = 0.5*(v0+v1);
		point.set( pt );
		res = true;
	}

	if( getPoint(0.5*(u0+u1),v0,pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = 0.5*(u0+u1);
		v = v0;
		point.set( pt );
		res = true;
	}

	if( getPoint(0.5*(u0+u1),v1,pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = 0.5*(u0+u1);
		v = v1;
		point.set( pt );
		res = true;
	}
	return res;
}

bool
kmb::NurbsSurface3D::getMiddlePointByNearest( double u0, double v0, double u1, double v1, double &u, double &v, kmb::Point3D &point ) const
{

	kmb::Point3D p0,p1,pm,pt;
	if( !getPoint(u0,v0,p0) ){
		return false;
	}
	if( !getPoint(u1,v1,p1) ){
		return false;
	}

	double um = 0.5*(u0+u1);
	double vm = 0.5*(v0+v1);
	pm = kmb::Point3D::getCenter(p0,p1);

	kmb::Minimizer minimizer;
	bool res = false;

	if( getPoint(um,vm,pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = um;
		v = vm;
		point.set( pt );
		res = true;
	}

	if( getPoint(u0,vm,pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = u0;
		v = vm;
		point.set( pt );
		res = true;
	}

	if( getPoint(u1,vm,pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = u1;
		v = vm;
		point.set( pt );
		res = true;
	}

	if( getPoint(um,v0,pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = um;
		v = v0;
		point.set( pt );
		res = true;
	}

	if( getPoint(um,v1,pt) && minimizer.update( pt.distanceSq(pm) ) ){
		u = um;
		v = v1;
		point.set( pt );
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

bool
kmb::NurbsSurface3D::getNearest( const kmb::Point3D& point, double &u, double &v ) const
{

	class dist_local : public kmb::OptTargetSV {
	private:
		const kmb::NurbsSurface3D* surface;
		const kmb::Point3D target;
		kmb::Vector3D uVec, vVec;
		kmb::Point3D pt;
		double weight;
	public:
		int getDomainDim(void) const{
			return 2;
		};
		double f(const double* t){
			if( !surface->getPoint(t[0],t[1],pt) ){
				return DBL_MAX;
			}
			return target.distanceSq( pt );
		}
		bool df(const double* t,double* deriv){
			if( surface->getPoint( t[0], t[1], pt ) &&
				surface->getSubDerivative( kmb::Surface3D::DER_U, t[0], t[1], uVec ) &&
				surface->getSubDerivative( kmb::Surface3D::DER_V, t[0], t[1], vVec ) &&
				surface->getWeight( t[0], t[1], weight ) )
			{
				kmb::Vector3D d(pt,target);
				deriv[0] = 2.0 * d * uVec / weight;
				deriv[1] = 2.0 * d * vVec / weight;
				return true;
			}else{
				return false;
			}
		}
		dist_local(const kmb::NurbsSurface3D* s,const kmb::Point3D p)
		: surface(s), target(p){}
	};
	dist_local dist_obj(this,point);


	class opt_local : public kmb::OptTargetVV {
	private:
		const kmb::NurbsSurface3D* surface;
		const kmb::Point3D target;
		double t0,t1;
		bool calculated;
		double weight;
		kmb::Point3D pt;
		kmb::Vector3D uVec, vVec, uuVec, uvVec, vvVec;

		bool calc(double u,double v){
			if( u == t0 && v == t1 && calculated ){
				return true;
			}
			if( surface->getPoint(u,v,pt) &&
					surface->getSubDerivative( kmb::Surface3D::DER_U, u, v, uVec) &&
					surface->getSubDerivative( kmb::Surface3D::DER_V, u, v, vVec) &&
					surface->getSubDerivative( kmb::Surface3D::DER_UU, u, v, uuVec) &&
					surface->getSubDerivative( kmb::Surface3D::DER_UV, u, v, uvVec) &&
					surface->getSubDerivative( kmb::Surface3D::DER_VV, u, v, vvVec) &&
					surface->getWeight( u, v, weight ) )
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
			jac.set(0,0,uVec*uVec/weight+d*uuVec);
			jac.set(0,1,uVec*vVec/weight+d*uvVec);
			jac.set(1,0,vVec*uVec/weight+d*uvVec);
			jac.set(1,1,vVec*vVec/weight+d*vvVec);
			return true;
		}
		opt_local(const kmb::NurbsSurface3D* s,const kmb::Point3D p)
		: surface(s), target(p), t0(0.0), t1(0.0), calculated(false), weight(0.0){}
	};
	opt_local opt_obj(this,point);

	kmb::Optimization opt;
	double min_t[2]={0.0,0.0}, max_t[2]={0.0,0.0};
	getDomain(min_t[0],max_t[0],min_t[1],max_t[1]);
	double init_t[2] = {0.0,0.0};
	double opt_t[2] = {0.0,0.0};
	int div[2] = {20,20};
	opt.setEpsilon(epsilon);
	opt.setIterMax(iterMax);


	if( opt.calcMin_GS( dist_obj, opt_t, min_t, max_t ) ){
		double d = dist_obj.f( opt_t );
		if( d < epsilon ){
			u = opt_t[0];
			v = opt_t[1];
			return true;
		}
	}


	opt.calcMinOnGrid( dist_obj, init_t, min_t, max_t, div );
	double d = dist_obj.f( init_t );
	if( d < epsilon ){
		u = init_t[0];
		v = init_t[1];
		return true;
	}


	if( opt.calcMin_BFGS( dist_obj, opt_t, init_t ) &&
		min_t[0] <= opt_t[0] && opt_t[0] <= max_t[0] &&
		min_t[1] <= opt_t[1] && opt_t[1] <= max_t[1] )
	{
		u = opt_t[0];
		v = opt_t[1];
		return true;
	}


	if( opt.calcZero_DN( opt_obj, opt_t, init_t ) &&
		min_t[0] <= opt_t[0] && opt_t[0] <= max_t[0] &&
		min_t[1] <= opt_t[1] && opt_t[1] <= max_t[1] )
	{
		u = opt_t[0];
		v = opt_t[1];
		return true;
	}


	if( opt.calcMin_CG( dist_obj, opt_t, min_t, max_t ) ){
		u = opt_t[0];
		v = opt_t[1];
		return true;
	}

	return false;
}



bool
kmb::NurbsSurface3D::getSubDerivative( kmb::Surface3D::derivativeType d, double u, double v, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	if( !valid() ){
		return false;
	}

	kmb::Point3D pt;
	double weight = 0.0;
	if( !getPoint(u,v,pt) || !getWeight(u,v,weight) ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;

	double* u0 = NULL;
	double* v0 = NULL;
	double* u1 = NULL;
	double* v1 = NULL;
	double* u2 = NULL;
	double* v2 = NULL;

	switch( d )
	{
	case kmb::Surface3D::DER_U:
		u1 = new double[uNum];
		v0 = new double[vNum];
		for(int i=0;i<uNum;++i){
			u1[i] = uBspline.getDerivative(i,uOrder-1,u);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			v0[j] = vBspline.getValue(j,vOrder-1,v);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			for(int i=0;i<uNum;++i){
				tangent.addCoordinate(
					u1[i]*v0[j]*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					u1[i]*v0[j]*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					u1[i]*v0[j]*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		delete[] u1;
		delete[] v0;
		return true;
	case kmb::Surface3D::DER_V:
		u0 = new double[uNum];
		v1 = new double[vNum];
		for(int i=0;i<uNum;++i){
			u0[i] = uBspline.getValue(i,uOrder-1,u);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			v1[j] = vBspline.getDerivative(j,vOrder-1,v);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			for(int i=0;i<uNum;++i){
				tangent.addCoordinate(
					u0[i]*v1[j]*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					u0[i]*v1[j]*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					u0[i]*v1[j]*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		delete[] u0;
		delete[] v1;
		return true;
	case kmb::Surface3D::DER_UU:
		u2 = new double[uNum];
		v0 = new double[vNum];
		for(int i=0;i<uNum;++i){
			u2[i] = uBspline.getSecondDerivative(i,uOrder-1,u);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			v0[j] = vBspline.getValue(j,vOrder-1,v);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			for(int i=0;i<uNum;++i){
				tangent.addCoordinate(
					u2[i]*v0[j]*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					u2[i]*v0[j]*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					u2[i]*v0[j]*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		{
			kmb::Vector3D tangent_u;
			double weight_u = 0.0;
			getSubDerivative( kmb::Surface3D::DER_U, u, v, tangent_u );
			getWeightDerivative( kmb::Surface3D::DER_U, u, v, weight_u );
			tangent -= 2.0 * weight_u * tangent_u;
		}
		delete[] u2;
		delete[] v0;
		return true;
	case kmb::Surface3D::DER_UV:
		u1 = new double[uNum];
		v1 = new double[vNum];
		for(int i=0;i<uNum;++i){
			u1[i] = uBspline.getDerivative(i,uOrder-1,u);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			v1[j] = vBspline.getDerivative(j,vOrder-1,v);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		{
			kmb::Vector4D uder;
			kmb::Vector4D vder;
			for(int j=0;j<vNum;++j){
				for(int i=0;i<uNum;++i){
					tangent.addCoordinate(
						u1[i]*v1[j]*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
						u1[i]*v1[j]*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
						u1[i]*v1[j]*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
#ifdef _DEBUG
					++calcCount;
#endif
				}
			}
		}
		{
			kmb::Vector3D tangent_u;
			double weight_u = 0.0;
			kmb::Vector3D tangent_v;
			double weight_v = 0.0;
			getSubDerivative( kmb::Surface3D::DER_U, u, v, tangent_u );
			getWeightDerivative( kmb::Surface3D::DER_U, u, v, weight_u );
			getSubDerivative( kmb::Surface3D::DER_V, u, v, tangent_v );
			getWeightDerivative( kmb::Surface3D::DER_V, u, v, weight_v );
			tangent -= weight_v * tangent_v;
			tangent -= weight_u * tangent_u;
		}
		delete[] u1;
		delete[] v1;
		return true;
	case kmb::Surface3D::DER_VV:
		u0 = new double[uNum];
		v2 = new double[vNum];
		for(int i=0;i<uNum;++i){
			u0[i] = uBspline.getValue(i,uOrder-1,u);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			v2[j] = vBspline.getSecondDerivative(j,vOrder-1,v);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			for(int i=0;i<uNum;++i){
				tangent.addCoordinate(
					u0[i]*v2[j]*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					u0[i]*v2[j]*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					u0[i]*v2[j]*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		{
			kmb::Vector3D tangent_v;
			double weight_v = 0.0;
			getSubDerivative( kmb::Surface3D::DER_V, u, v, tangent_v );
			getWeightDerivative( kmb::Surface3D::DER_V, u, v, weight_v );
			tangent -= 2.0 * weight_v * tangent_v;
		}
		delete[] u0;
		delete[] v2;
		return true;
	default:
		break;
	}
	return false;
}

bool
kmb::NurbsSurface3D::getWeight( double u, double v, double &w ) const
{
	if( !valid() ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;
	double* u0 = new double[uNum];
	double* v0 = new double[vNum];
	for(int i=0;i<uNum;++i){
		u0[i] = uBspline.getValue(i,uOrder-1,u);
#ifdef _DEBUG
		++calcCount;
#endif
	}
	for(int j=0;j<vNum;++j){
		v0[j] = vBspline.getValue(j,vOrder-1,v);
#ifdef _DEBUG
		++calcCount;
#endif
	}
	double weight = 0.0;
	for(int j=0;j<vNum;++j){
		for(int i=0;i<uNum;++i){
			weight += u0[i]*v0[j]*ctrlPts[i+j*uNum].w();
		}
	}
	w = weight;
	delete[] u0;
	delete[] v0;
	return true;
}

bool
kmb::NurbsSurface3D::getWeightDerivative( derivativeType d, double u, double v, double &w ) const
{
	if( !valid() ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;

	double* u0 = NULL;
	double* v0 = NULL;
	double* u1 = NULL;
	double* v1 = NULL;

	double weight = 0.0;
	switch( d )
	{
	case kmb::Surface3D::DER_U:
		u1 = new double[uNum];
		v0 = new double[vNum];
		for(int i=0;i<uNum;++i){
			u1[i] = uBspline.getDerivative(i,uOrder-1,u);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			v0[j] = vBspline.getValue(j,vOrder-1,v);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			for(int i=0;i<uNum;++i){
				weight += u1[i]*v0[j]*ctrlPts[i+j*uNum].w();
			}
		}
		w = weight;
		delete[] u1;
		delete[] v0;
		return true;
	case kmb::Surface3D::DER_V:
		u0 = new double[uNum];
		v1 = new double[vNum];
		for(int i=0;i<uNum;++i){
			u0[i] = uBspline.getValue(i,uOrder-1,u);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			v1[j] = vBspline.getDerivative(j,vOrder-1,v);
#ifdef _DEBUG
			++calcCount;
#endif
		}
		for(int j=0;j<vNum;++j){
			for(int i=0;i<uNum;++i){
				weight += u0[i]*v1[j]*ctrlPts[i+j*uNum].w();
			}
		}
		w = weight;
		delete[] u0;
		delete[] v1;
		return true;
	default:
		break;
	}
	return false;
}

int
kmb::NurbsSurface3D::writeRNF( std::ofstream &output, std::string indent) const
{
	output << indent << "uknots:" << std::endl;
	output << indent << "  order: " << uOrder << std::endl;
	output << indent << "  vector: [ ";
	int uKnotCount = uBspline.getKnotCount();
	for(int i=0;i<uKnotCount;++i){
		if( i != 0 ){
			output << ", ";
		}
		output << std::fixed << uBspline.getKnot(i);
	}
	output << " ]" << std::endl;
	output << indent << "vknots:" << std::endl;
	output << indent << "  order: " << vOrder << std::endl;
	output << indent << "  vector: [ ";
	int vKnotCount = vBspline.getKnotCount();
	for(int i=0;i<vKnotCount;++i){
		if( i != 0 ){
			output << ", ";
		}
		output << std::fixed << vBspline.getKnot(i);
	}
	output << " ]" << std::endl;
	output << indent << "ctrlpts:" << std::endl;
	for(std::vector<kmb::Point4D>::const_iterator pIter = ctrlPts.begin();
		pIter != ctrlPts.end(); ++pIter )
	{
		output << indent << std::fixed
			<< "  - [ "
			<< pIter->x() << ", "
			<< pIter->y() << ", "
			<< pIter->z() << ", "
			<< pIter->w() << " ]" << std::endl;
	}
	return 0;
}
