/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FramedPlane                                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include <cmath>
#include "Geometry/kmbFramedPlane.h"
#include "Geometry/kmbIdTypes.h"

kmb::FramedPlane::FramedPlane(const double a0,const double b0,const double c0,const double d0)
: kmb::Plane(a0,b0,c0,d0)
{
	origin = this->projectOnPlane( kmb::Point3D(0,0,0) );
	initUVBase();
}

kmb::FramedPlane::FramedPlane(const kmb::Point3D &o,const kmb::Vector3D &v0,const kmb::Vector3D &v1)
: kmb::Plane(0.0, 0.0, 0.0, 0.0)
{
	this->origin.setCoordinate( o.x(), o.y(), o.z() );


	this->n0.setCoordinate( v0.x(), v0.y(), v0.z() );
	this->n1.setCoordinate( v1.x(), v1.y(), v1.z() );
	this->n0.normalize();
	this->n1 -= (n0*n1) * n0;
	this->n1.normalize();
	Vector3D n = n0 % n1;

	this->a = n.x();
	this->b = n.y();
	this->c = n.z();
	this->d = -( n.x()*o.x() + n.y()*o.y() + n.z()*o.z() );
}

kmb::FramedPlane::~FramedPlane(void)
{
}

void
kmb::FramedPlane::initUVBase(void)
{
	if( fabs(a) > fabs(b) && fabs(a) > fabs(c) ){

		n0.setCoordinate( -a*b, 1-b*b, -c*b);
	}else if( fabs(b) > fabs(a) && fabs(b) > fabs(c) ){

		n0.setCoordinate( -a*c, -b*c, 1-c*c);
	}else{

		n0.setCoordinate( 1-a*a, -b*a, -c*a);
	}
	n0.normalize();
	n1.setCoordinate(
		b*n0.z() - c*n0.y(),
		c*n0.x() - a*n0.z(),
		a*n0.y() - b*n0.x() );
	n1.normalize();
}

kmb::Point2D
kmb::FramedPlane::transformTo2D(const kmb::Point3D &point) const
{
	return kmb::Point2D(
		n0.x()*(point.x()-origin.x()) + n0.y()*(point.y()-origin.y()) + n0.z()*(point.z()-origin.z()),
		n1.x()*(point.x()-origin.x()) + n1.y()*(point.y()-origin.y()) + n1.z()*(point.z()-origin.z()) );
}

kmb::Point3D
kmb::FramedPlane::transformTo3D(const kmb::Point2D &point) const
{
	return transformTo3D( point.x(), point.y() );
}

kmb::Point3D
kmb::FramedPlane::transformTo3D(double u,double v) const
{
	return origin + u * n0 + v * n1;
}

void
kmb::FramedPlane::rotateUVBase(const double angle)
{
	double c = cos(angle);
	double s = sin(angle);
	kmb::Vector3D v0 = c*n0 - s*n1;
	kmb::Vector3D v1 = s*n0 + c*n1;
	n0.setCoordinate( v0.x(), v0.y(), v0.z() );
	n1.setCoordinate( v1.x(), v1.y(), v1.z() );
}

void
kmb::FramedPlane::setOrigin(const kmb::Point3D& point)
{
	setOrigin( point.x(), point.y(), point.z() );
}

void
kmb::FramedPlane::setOrigin(double x,double y,double z)
{
	origin.setCoordinate(x,y,z);
	d = -a*x-b*y-c*z;
}

void
kmb::FramedPlane::getOrigin(Point3D& point) const
{
	point.set( origin );
}

void
kmb::FramedPlane::slideOrigin(double s)
{
	setOrigin( origin.x() + s * a, origin.y() + s * b, origin.z() + s * c );
}

bool
kmb::FramedPlane::setNormal(double a,double b,double c)
{
	double r = sqrt(a*a+b*b+c*c);
	if( r != 0.0 ){
		this->a = a/r;
		this->b = b/r;
		this->c = c/r;
		this->d =
			- this->a*origin.x()
			- this->b*origin.y()
			- this->c*origin.z();
		initUVBase();
		return true;
	}
	return false;
}

void
kmb::FramedPlane::setNormalPolar(double phi,double theta)
{
	a = cos(phi)*sin(theta);
	b = sin(phi)*sin(theta);
	c = cos(theta);
	d = - a*origin.x() - b*origin.y() - c*origin.z();

	double r = a*a + b*b;
	if( r == 0.0 ){
		n0.setCoordinate( 1.0, 0.0, 0.0 );
		n1.setCoordinate( 0.0, 1.0, 0.0 );
	}else{
		r = sqrt(r);
		n0.setCoordinate( -b, a, 0 );
		n0.normalize();
		n1.setCoordinate( a*c/r, b*c/r, -r );
		n1.normalize();
	}
}

void
kmb::FramedPlane::getNormalPolar(double& phi,double& theta) const
{
	theta = atan2(sqrt(a*a+b*b),c);
	phi = atan2(b,a);
}
