/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BSplineCurve2D                                          #
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

#include "Shape/kmbBSplineCurve2D.h"

kmb::BSplineCurve2D::BSplineCurve2D(void)
: kmb::Curve2D()
{
}

kmb::BSplineCurve2D::~BSplineCurve2D(void)
{
}

void
kmb::BSplineCurve2D::clear( void )
{
	bspline.clear();
	ctrlPts.clear();
}

void
kmb::BSplineCurve2D::getDomain( double &min_t, double &max_t ) const
{
	bspline.getDomain( min_t, max_t );
}

int
kmb::BSplineCurve2D::getKnotCount(void) const
{
	return bspline.getKnotCount();
}

int
kmb::BSplineCurve2D::getCtrlPtCount(void) const
{
	return static_cast<int>( ctrlPts.size() );
}

int
kmb::BSplineCurve2D::getOrder(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() );
}

int
kmb::BSplineCurve2D::getDegree(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() ) - 1;
}

void
kmb::BSplineCurve2D::appendKnot(double k)
{
	bspline.appendKnot( k );
}

void
kmb::BSplineCurve2D::appendCtrlPt(double x,double y)
{
	this->ctrlPts.push_back( kmb::Point2D(x,y) );
}

bool
kmb::BSplineCurve2D::getCtrlPt(int i,kmb::Point2D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

double
kmb::BSplineCurve2D::getKnot(int i) const
{
	return bspline.getKnot(i);
}

bool
kmb::BSplineCurve2D::isDomain( double t ) const
{
	return bspline.isDomain( t );
}

bool
kmb::BSplineCurve2D::getPoint( double t, kmb::Point2D& point ) const
{
	point.zero();
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}
	int num = getCtrlPtCount();
	for(int i=0;i<num;++i){
		double u = bspline.getValue(i,deg,t);
		point.addCoordinate( u*ctrlPts[i].x(), u*ctrlPts[i].y() );
	}
	return true;
}

bool
kmb::BSplineCurve2D::getDerivative( double t, kmb::Vector2D& tangent ) const
{
	tangent.zero();
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}

	if( deg < 1 ){
		return true;
	}
	int num = getCtrlPtCount();
	for(int i=0;i<num;++i){
		double u = bspline.getDerivative(i,deg,t);
		tangent.addCoordinate( u*ctrlPts[i].x(), u*ctrlPts[i].y() );
	}
	return true;
}

bool
kmb::BSplineCurve2D::getSecondDerivative( double t, kmb::Vector2D& tangent ) const
{
	tangent.zero();
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}

	if( deg < 2 ){
		return true;
	}
	int num = getCtrlPtCount();
	for(int i=0;i<num;++i){
		double u = bspline.getSecondDerivative(i,deg,t);
		tangent.addCoordinate( u*ctrlPts[i].x(), u*ctrlPts[i].y() );
	}
	return true;
}
