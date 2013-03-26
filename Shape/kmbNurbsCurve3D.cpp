/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NurbsCurve3D                                            #
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

#include "Geometry/kmbGeometry4D.h"
#include "Geometry/kmbOptimization.h"
#include "Shape/kmbNurbsCurve3D.h"

kmb::NurbsCurve3D::NurbsCurve3D(void)
: kmb::Curve3D()
{
}

kmb::NurbsCurve3D::~NurbsCurve3D(void)
{
}

void
kmb::NurbsCurve3D::clear( void )
{
	bspline.clear();
	ctrlPts.clear();
}

void
kmb::NurbsCurve3D::getDomain( double &min_t, double &max_t ) const
{
	bspline.getDomain( min_t, max_t );
}

int
kmb::NurbsCurve3D::getKnotCount(void) const
{
	return bspline.getKnotCount();
}

int
kmb::NurbsCurve3D::getCtrlPtCount(void) const
{
	return static_cast<int>( ctrlPts.size() );
}

int
kmb::NurbsCurve3D::getOrder(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() );
}

int
kmb::NurbsCurve3D::getDegree(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() ) - 1;
}

void
kmb::NurbsCurve3D::appendKnot(double k)
{
	bspline.appendKnot( k );
}

void
kmb::NurbsCurve3D::appendCtrlPt(double x,double y,double z,double w)
{
	this->ctrlPts.push_back( kmb::Point4D(x,y,z,w) );
}

bool
kmb::NurbsCurve3D::getCtrlPt(int i,kmb::Point4D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

double
kmb::NurbsCurve3D::getKnot(int i) const
{
	return bspline.getKnot(i);
}

bool
kmb::NurbsCurve3D::isDomain( double t ) const
{
	return bspline.isDomain( t );
}

bool
kmb::NurbsCurve3D::getPoint( double t, kmb::Point3D& point ) const
{
	point.zero();
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}
	int num = getCtrlPtCount();
	double weight = 0.0;
	for(int i=0;i<num;++i){
		double u = bspline.getValue(i,deg,t);
		point.addCoordinate( u*ctrlPts[i].x()*ctrlPts[i].w(), u*ctrlPts[i].y()*ctrlPts[i].w(), u*ctrlPts[i].z()*ctrlPts[i].w() );
		weight += u*ctrlPts[i].w();
	}
	if( weight != 0.0 ){
		point.scale( 1.0/weight );
	}
	return true;
}

bool
kmb::NurbsCurve3D::getWeight( double t, double &w ) const
{
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}
	int num = getCtrlPtCount();
	double weight = 0.0;
	for(int i=0;i<num;++i){
		double u = bspline.getValue(i,deg,t);
		weight += u*ctrlPts[i].w();
	}
	w = weight;
	return true;
}

bool
kmb::NurbsCurve3D::getSubDerivative( double t, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}
	int num = getCtrlPtCount();
	for(int i=0;i<num;++i){
		double ui = bspline.getDerivative(i,deg,t);
		for(int j=0;j<num;++j){
			double uj = bspline.getValue(i,deg,t);
			tangent.addCoordinate(
				ui*uj*(ctrlPts[i].x()-ctrlPts[j].x())*ctrlPts[i].w()*ctrlPts[j].w(),
				ui*uj*(ctrlPts[i].y()-ctrlPts[j].y())*ctrlPts[i].w()*ctrlPts[j].w(),
				ui*uj*(ctrlPts[i].z()-ctrlPts[j].z())*ctrlPts[i].w()*ctrlPts[j].w());
		}
	}
	return true;
}

bool
kmb::NurbsCurve3D::getSubSecondDerivative( double t, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}
	int num = getCtrlPtCount();
	for(int i=0;i<num;++i){
		double ui1 = bspline.getDerivative(i,deg,t);
		double ui2 = bspline.getSecondDerivative(i,deg,t);
		for(int j=0;j<num;++j){
			double uj0 = bspline.getValue(i,deg,t);
			double uj1 = bspline.getDerivative(i,deg,t);
			tangent.addCoordinate(
				(ui2*uj0+ui1*uj1)*(ctrlPts[i].x()-ctrlPts[j].x())*ctrlPts[i].w()*ctrlPts[j].w(),
				(ui2*uj0+ui1*uj1)*(ctrlPts[i].y()-ctrlPts[j].y())*ctrlPts[i].w()*ctrlPts[j].w(),
				(ui2*uj0+ui1*uj1)*(ctrlPts[i].z()-ctrlPts[j].z())*ctrlPts[i].w()*ctrlPts[j].w());
		}
	}
	return true;
}

bool
kmb::NurbsCurve3D::getNearest( const kmb::Point3D& point, double& t ) const
{
	class dist_local : public kmb::OptTargetSS_0 {
	private:
		const kmb::Curve3D* curve;
		const kmb::Point3D target;
	public:
		double f(double t){
			kmb::Point3D pt;
			if( !curve->getPoint(t,pt) ){
				return DBL_MAX;
			}
			return target.distanceSq( pt );
		}
		dist_local(const kmb::Curve3D* c,const kmb::Point3D p)
		: curve(c), target(p){}
	};


	class opt_local : public kmb::OptTargetSS {
	private:
		const kmb::NurbsCurve3D* curve;
		const kmb::Point3D target;
		double t0;
		bool calculated;
		double weight;
		kmb::Point3D pt;
		kmb::Vector3D subvec;
		kmb::Vector3D subacc;

		bool calc(double t){
			if( t == t0 && calculated ){
				return true;
			}
			if( curve->getPoint(t,pt) && curve->getWeight(t,weight)
				&& curve->getSubDerivative(t,subvec)
				&& curve->getSubSecondDerivative(t,subacc) ){
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
			kmb::Vector3D d(pt,target);
			return d*subvec;
		}
		double df(double t){
			if( !calc(t) ){
				return DBL_MAX;
			}
			kmb::Vector3D d(pt,target);
			return (subvec*subvec) / (weight*weight) + (d*subacc);
		}
		opt_local(const kmb::NurbsCurve3D* c,const kmb::Point3D p)
		: curve(c), target(p), t0(0.0), calculated(false), weight(0.0){}
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

