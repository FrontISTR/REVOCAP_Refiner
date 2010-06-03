/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : NurbsCurve2D                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

#include "Shape/kmbNurbsCurve2D.h"

kmb::NurbsCurve2D::NurbsCurve2D(void)
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
kmb::NurbsCurve2D::newtonMethod( double &t, kmb::Point2D& point, double relax ) const
{
	kmb::Point2D pt;
	kmb::Vector2D subvec;
	kmb::Vector2D subacc;
	double weight = 0;
	if( !getPoint(t,pt) ||
		!getSubDerivative(t,subvec) ||
		!getSubSecondDerivative(t,subacc) ||
		!getWeight( t, weight ) ||
		weight == 0.0 )
	{
		return false;
	}
	kmb::Vector2D d(pt,point);
	double denominator = (subvec * subvec) / ( weight * weight ) + ( d * subacc );
	double numerator = d * subvec;
	if( denominator != 0.0 ){
		t -= relax * (numerator / denominator);
		return true;
	}
	return false;
}
