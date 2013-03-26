/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NurbsCurve2D                                            #
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

#include "Shape/kmbNurbsCurve2D.h"
#include "Geometry/kmbOptimization.h"

kmb::NurbsCurve2D::NurbsCurve2D(void)
: kmb::Curve2D()
{
}

kmb::NurbsCurve2D::~NurbsCurve2D(void)
{
}

void
kmb::NurbsCurve2D::clear( void )
{
	bspline.clear();
	ctrlPts.clear();
}

void
kmb::NurbsCurve2D::getDomain( double &min_t, double &max_t ) const
{
	bspline.getDomain( min_t, max_t );
}

int
kmb::NurbsCurve2D::getKnotCount(void) const
{
	return bspline.getKnotCount();
}

int
kmb::NurbsCurve2D::getCtrlPtCount(void) const
{
	return static_cast<int>( ctrlPts.size() );
}

int
kmb::NurbsCurve2D::getOrder(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() );
}

int
kmb::NurbsCurve2D::getDegree(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() ) - 1;
}

void
kmb::NurbsCurve2D::appendKnot(double k)
{
	bspline.appendKnot( k );
}

void
kmb::NurbsCurve2D::appendCtrlPt(double x,double y,double w)
{
	this->ctrlPts.push_back( kmb::Point3D(x,y,w) );
}

bool
kmb::NurbsCurve2D::getCtrlPt(int i,kmb::Point3D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

double
kmb::NurbsCurve2D::getKnot(int i) const
{
	return bspline.getKnot(i);
}

bool
kmb::NurbsCurve2D::isDomain( double t ) const
{
	return bspline.isDomain( t );
}

bool
kmb::NurbsCurve2D::getPoint( double t, kmb::Point2D& point ) const
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
		point.addCoordinate( u*ctrlPts[i].x()*ctrlPts[i].z(), u*ctrlPts[i].y()*ctrlPts[i].z() );
		weight += u*ctrlPts[i].z();
	}
	if( weight != 0.0 ){
		point.scale( 1.0/weight );
	}
	return true;
}

bool
kmb::NurbsCurve2D::getWeight( double t, double &w ) const
{
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}
	int num = getCtrlPtCount();
	double weight = 0.0;
	for(int i=0;i<num;++i){
		double u = bspline.getValue(i,deg,t);
		weight += u*ctrlPts[i].z();
	}
	w = weight;
	return true;
}

bool
kmb::NurbsCurve2D::getSubDerivative( double t, kmb::Vector2D& tangent ) const
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
				ui*uj*(ctrlPts[i].x()-ctrlPts[j].x())*ctrlPts[i].z()*ctrlPts[j].z(),
				ui*uj*(ctrlPts[i].y()-ctrlPts[j].y())*ctrlPts[i].z()*ctrlPts[j].z());
		}
	}
	return true;
}

bool
kmb::NurbsCurve2D::getSubSecondDerivative( double t, kmb::Vector2D& tangent ) const
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
				(ui2*uj0+ui1*uj1)*(ctrlPts[i].x()-ctrlPts[j].x())*ctrlPts[i].z()*ctrlPts[j].z(),
				(ui2*uj0+ui1*uj1)*(ctrlPts[i].y()-ctrlPts[j].y())*ctrlPts[i].z()*ctrlPts[j].z());
		}
	}
	return true;
}

bool
kmb::NurbsCurve2D::getNearest( const kmb::Point2D& point, double& t ) const
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
		const kmb::NurbsCurve2D* curve;
		const kmb::Point2D target;
		double t0;
		bool calculated;
		double weight;
		kmb::Point2D pt;
		kmb::Vector2D subvec;
		kmb::Vector2D subacc;

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
			kmb::Vector2D d(pt,target);
			return d*subvec;
		}
		double df(double t){
			if( !calc(t) ){
				return DBL_MAX;
			}
			kmb::Vector2D d(pt,target);
			return (subvec*subvec) / (weight*weight) + (d*subacc);
		}
		opt_local(const kmb::NurbsCurve2D* c,const kmb::Point2D p)
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

