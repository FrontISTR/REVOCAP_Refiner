/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : NurbsSurface3D                                          #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

#include "Shape/kmbNurbsSurface3D.h"
#include "Geometry/kmb_Optimization.h"
#include "Geometry/kmb_Calculator.h"
#include "Matrix/kmbVector.h"
#include <cmath>

kmb::NurbsSurface3D::NurbsSurface3D(void)
: uOrder(0)
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
	for(int j=0;j<vNum;++j){
		double vj0 = vBspline.getValue(j,vOrder-1,v);
		for(int i=0;i<uNum;++i){
			double ui0 = uBspline.getValue(i,uOrder-1,u);
			point.addCoordinate(
				ui0*vj0*ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w(),
				ui0*vj0*ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w(),
				ui0*vj0*ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() );
			weight += ui0*vj0*ctrlPts[i+j*uNum].w();
		}
	}
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
	switch( d )
	{
	case kmb::Surface3D::DER_U:
		for(int j=0;j<vNum;++j){
			double vj0 = vBspline.getValue(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui1 = uBspline.getDerivative(i,uOrder-1,u);
				tangent.addCoordinate(
					ui1*vj0*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					ui1*vj0*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					ui1*vj0*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		return true;
	case kmb::Surface3D::DER_V:
		for(int j=0;j<vNum;++j){
			double vj1 = vBspline.getDerivative(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui0 = uBspline.getValue(i,uOrder-1,u);
				tangent.addCoordinate(
					ui0*vj1*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					ui0*vj1*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					ui0*vj1*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		return true;
	case kmb::Surface3D::DER_UU:
		for(int j=0;j<vNum;++j){
			double vj0 = vBspline.getValue(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui2 = uBspline.getSecondDerivative(i,uOrder-1,u);
				tangent.addCoordinate(
					ui2*vj0*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					ui2*vj0*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					ui2*vj0*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		{
			kmb::Vector3D tangent_u;
			double weight_u = 0.0;
			getSubDerivative( kmb::Surface3D::DER_U, u, v, tangent_u );
			getWeightDerivative( kmb::Surface3D::DER_U, u, v, weight_u );
			tangent -= 2.0 * weight_u * tangent_u;
		}
		return true;
	case kmb::Surface3D::DER_UV:
		{
			kmb::Vector4D uder;
			kmb::Vector4D vder;
			for(int j=0;j<vNum;++j){
				double vj1 = vBspline.getDerivative(j,vOrder-1,v);
				for(int i=0;i<uNum;++i){
					double ui1 = uBspline.getDerivative(i,uOrder-1,u);
					tangent.addCoordinate(
						ui1*vj1*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
						ui1*vj1*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
						ui1*vj1*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
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
		return true;
	case kmb::Surface3D::DER_VV:
		for(int j=0;j<vNum;++j){
			double vj2 = vBspline.getSecondDerivative(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui0 = uBspline.getValue(i,uOrder-1,u);
				tangent.addCoordinate(
					ui0*vj2*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.x()),
					ui0*vj2*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.y()),
					ui0*vj2*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		{
			kmb::Vector3D tangent_v;
			double weight_v = 0.0;
			getSubDerivative( kmb::Surface3D::DER_V, u, v, tangent_v );
			getWeightDerivative( kmb::Surface3D::DER_V, u, v, weight_v );
			tangent -= 2.0 * weight_v * tangent_v;
		}
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
	double weight = 0.0;
	for(int j=0;j<vNum;++j){
		double v0 = vBspline.getValue(j,vOrder-1,v);
		for(int i=0;i<uNum;++i){
			double u0 = uBspline.getValue(i,uOrder-1,u);
			weight += u0*v0*ctrlPts[i+j*uNum].w();
		}
	}
	w = weight;
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
	double weight = 0.0;
	switch( d )
	{
	case kmb::Surface3D::DER_U:
		for(int j=0;j<vNum;++j){
			double v0 = vBspline.getValue(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double u1 = uBspline.getDerivative(i,uOrder-1,u);
				weight += u1*v0*ctrlPts[i+j*uNum].w();
			}
		}
		w = weight;
		return true;
	case kmb::Surface3D::DER_V:
		for(int j=0;j<vNum;++j){
			double v1 = vBspline.getDerivative(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double u0 = uBspline.getValue(i,uOrder-1,u);
				weight += u0*v1*ctrlPts[i+j*uNum].w();
			}
		}
		w = weight;
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
