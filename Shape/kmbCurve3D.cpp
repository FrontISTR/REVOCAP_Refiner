/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Curve3D                                                 #
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
#include "Shape/kmbCurve3D.h"
#include "Geometry/kmb_Calculator.h"
#include <cmath>

int kmb::Curve3D::iterMax = 100;
double kmb::Curve3D::thres = 1.0e-8;

kmb::Curve3D::Curve3D(void)
{
}

kmb::Curve3D::~Curve3D(void)
{
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

bool
kmb::Curve3D::getDerivative( double t, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	return false;
}

bool
kmb::Curve3D::getSecondDerivative( double t, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	return false;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

bool
kmb::Curve3D::getNearestOnInnerGrid( kmb::Point3D& point, unsigned int tgrid, double &t ) const
{
	if( tgrid <= 0 ){
		return false;
	}
	double min_t, max_t;
	kmb::Point3D pt;
	getDomain(min_t,max_t);
	kmb::Minimizer minimizer;
	for(unsigned int i = 1; i<tgrid; ++i ){
		double t0 = min_t + (max_t - min_t) * i / tgrid;
		getPoint(t0,pt);
		if( minimizer.update( pt.distanceSq( point ) ) ){
			t = t0;
		}
	}
	return true;
}

bool
kmb::Curve3D::getNearest( kmb::Point3D& point, double& t ) const
{
	double t0 = t;
	if( !isDomain(t0) ){
		getNearestOnInnerGrid(point,10,t0);
	}
	int count = kmb::Curve3D::iterMax;
	while( count > 0){
		if( !newtonMethod( t0, point ) ){
			return false;
		}
		if( fabs( t-t0 ) < kmb::Curve3D::thres ){
			t = t0;
			break;
		}
		--count;
		t = t0;
	}
	return ( count > 0 );
}

bool
kmb::Curve3D::newtonMethod( double &t, kmb::Point3D& point, double relax ) const
{
	kmb::Point3D pt;
	kmb::Vector3D vec;
	kmb::Vector3D acc;
	if( !getPoint(t,pt) || !getDerivative(t,vec) || !getSecondDerivative(t,acc) ){
		return false;
	}
	kmb::Vector3D d(pt,point);
	double denominator = (vec * vec) + ( d * acc );
	double numerator = d * vec;
	if( denominator != 0.0 ){
		t -= relax * (numerator / denominator);
		return true;
	}
	return false;
}
