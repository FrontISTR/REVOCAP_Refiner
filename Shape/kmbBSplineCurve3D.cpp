/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BSplineCurve3D                                          #
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

#include "Shape/kmbBSplineCurve3D.h"

kmb::BSplineCurve3D::BSplineCurve3D(void)
: kmb::Curve3D()
{
}

kmb::BSplineCurve3D::~BSplineCurve3D(void)
{
}

void
kmb::BSplineCurve3D::clear( void )
{
	bspline.clear();
	ctrlPts.clear();
}

void
kmb::BSplineCurve3D::getDomain( double &min_t, double &max_t ) const
{
	bspline.getDomain( min_t, max_t );
}

int
kmb::BSplineCurve3D::getKnotCount(void) const
{
	return bspline.getKnotCount();
}

int
kmb::BSplineCurve3D::getCtrlPtCount(void) const
{
	return static_cast<int>( ctrlPts.size() );
}

int
kmb::BSplineCurve3D::getOrder(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() );
}

int
kmb::BSplineCurve3D::getDegree(void) const
{
	return bspline.getKnotCount() - static_cast<int>( ctrlPts.size() ) - 1;
}

void
kmb::BSplineCurve3D::appendKnot(double k)
{
	bspline.appendKnot( k );
}

void
kmb::BSplineCurve3D::appendCtrlPt(double x,double y,double z)
{
	this->ctrlPts.push_back( kmb::Point3D(x,y,z) );
}

bool
kmb::BSplineCurve3D::getCtrlPt(int i,kmb::Point3D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

double
kmb::BSplineCurve3D::getKnot(int i) const
{
	return bspline.getKnot(i);
}

bool
kmb::BSplineCurve3D::isDomain( double t ) const
{
	return bspline.isDomain( t );
}

bool
kmb::BSplineCurve3D::getPoint( double t, kmb::Point3D& point ) const
{
	point.zero();
	int deg = getDegree();
	if( deg < 0 ){
		return false;
	}
	int num = getCtrlPtCount();
	for(int i=0;i<num;++i){
		double u = bspline.getValue(i,deg,t);
		point.addCoordinate( u*ctrlPts[i].x(), u*ctrlPts[i].y(), u*ctrlPts[i].z() );
	}
	return true;
}

bool
kmb::BSplineCurve3D::getDerivative( double t, kmb::Vector3D& tangent ) const
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
		tangent.addCoordinate( u*ctrlPts[i].x(), u*ctrlPts[i].y(), u*ctrlPts[i].z() );
	}
	return true;
}

bool
kmb::BSplineCurve3D::getSecondDerivative( double t, kmb::Vector3D& tangent ) const
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
		tangent.addCoordinate( u*ctrlPts[i].x(), u*ctrlPts[i].y(), u*ctrlPts[i].z() );
	}
	return true;
}
