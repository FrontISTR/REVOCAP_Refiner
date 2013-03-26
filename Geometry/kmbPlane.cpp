/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Plane                                                   #
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
#include "Geometry/kmbPlane.h"

double kmb::Plane::thres = 1.0e-6;

kmb::Plane::Plane(const double a,const double b,const double c,const double d)
: a(1.0)
, b(0.0)
, c(0.0)
, d(0.0)
{
	double r = sqrt(a*a+b*b+c*c);
	if( r != 0.0 ){
		this->a = a/r;
		this->b = b/r;
		this->c = c/r;
		this->d = d/r;
	}
}

kmb::Plane::~Plane(void)
{
}

kmb::Plane* kmb::Plane::
createFromPoints(const kmb::Point3D &p,const kmb::Point3D &q,const kmb::Point3D &r)
{
	kmb::Vector3D pq(q,p);
	kmb::Vector3D pr(r,p);
	kmb::Vector3D normal = pq%pr;
	return kmb::Plane::createFromBaseNormal(p,normal);
}

kmb::Plane* kmb::Plane::
createFromBaseNormal(const kmb::Point3D &base,const kmb::Vector3D &normal)
{
	kmb::Plane* plane = NULL;
	double a = normal.x();
	double b = normal.y();
	double c = normal.z();
	double d = - (a*base.x() + b*base.y() + c*base.z());
	plane = new kmb::Plane(a,b,c,d);
	return plane;
}

kmb::Vector3D
kmb::Plane::calcNormal(const Point3D &a,const Point3D &b,const Point3D &c)
{
	kmb::Vector3D ab(b,a);
	kmb::Vector3D ac(c,a);
	kmb::Vector3D normal = ab%ac;
	normal.normalize();
	return normal;
}

double kmb::Plane::
evaluate(const kmb::Point3D &pt) const
{
	return this->evaluate(pt.x(),pt.y(),pt.z());
}

double kmb::Plane::
evaluate(const double x,const double y,const double z) const
{
	return a*x + b*y + c*z + d;
}

kmb::Vector3D
kmb::Plane::getNormal(void) const
{
	return kmb::Vector3D(a,b,c);
}

double
kmb::Plane::distance(const kmb::Point3D &pt) const
{
	return fabs(this->evaluate(pt));
}

double
kmb::Plane::distance(const double x,const double y,const double z) const
{
	return fabs(this->evaluate(x,y,z));
}

double
kmb::Plane::getConstant(void) const
{
	return d;
}

kmb::Point3D*
kmb::Plane::createIntersectPoint(const kmb::Point3D &p0, const kmb::Point3D &p1) const
{
	kmb::Point3D* point = NULL;
	double t0 = this->evaluate( p0 );
	double t1 = this->evaluate( p1 );
	if( t0 * t1 < 0 ){
		point = new kmb::Point3D(
		( t1*p0.x() - t0*p1.x() ) / ( t1-t0 ),
		( t1*p0.y() - t0*p1.y() ) / ( t1-t0 ),
		( t1*p0.z() - t0*p1.z() ) / ( t1-t0 ) );
	}
	return point;
}

bool
kmb::Plane::getIntersection(const kmb::Point3D &p0, const kmb::Point3D &p1, kmb::Point3D &pt, double &t) const
{
	double t0 = this->evaluate( p0 );
	double t1 = this->evaluate( p1 );
	if( t0 != t1 ){
		pt.setCoordinate(
		( t1*p0.x() - t0*p1.x() ) / ( t1-t0 ),
		( t1*p0.y() - t0*p1.y() ) / ( t1-t0 ),
		( t1*p0.z() - t0*p1.z() ) / ( t1-t0 ) );
		t = t0 / (t0-t1);
		return true;
	}
	return false;
}

int
kmb::Plane::getIntersectionTriangle
(const kmb::Point3D &p0,const kmb::Point3D &p1, const kmb::Point3D &p2, kmb::Point3D &p3, kmb::Point3D &p4) const
{
	double t0 = this->evaluate( p0 );
	double t1 = this->evaluate( p1 );
	double t2 = this->evaluate( p2 );
	if( t0 * t1 < 0.0 && t0 * t2 < 0.0 ){
		p3.setCoordinate(
		( t1*p0.x() - t0*p1.x() ) / ( t1-t0 ),
		( t1*p0.y() - t0*p1.y() ) / ( t1-t0 ),
		( t1*p0.z() - t0*p1.z() ) / ( t1-t0 ) );
		p4.setCoordinate(
		( t2*p0.x() - t0*p2.x() ) / ( t2-t0 ),
		( t2*p0.y() - t0*p2.y() ) / ( t2-t0 ),
		( t2*p0.z() - t0*p2.z() ) / ( t2-t0 ) );
		return (t0 > 0.0) ? 0 : 3;
	}else if( t1 * t2 < 0.0 && t1 * t0 < 0.0 ){
		p3.setCoordinate(
		( t2*p1.x() - t1*p2.x() ) / ( t2-t1 ),
		( t2*p1.y() - t1*p2.y() ) / ( t2-t1 ),
		( t2*p1.z() - t1*p2.z() ) / ( t2-t1 ) );
		p4.setCoordinate(
		( t0*p1.x() - t1*p0.x() ) / ( t0-t1 ),
		( t0*p1.y() - t1*p0.y() ) / ( t0-t1 ),
		( t0*p1.z() - t1*p0.z() ) / ( t0-t1 ) );
		return (t1 > 0.0) ? 1 : 4;
	}else if( t2 * t0 < 0.0 && t2 * t1 < 0.0 ){
		p3.setCoordinate(
		( t0*p2.x() - t2*p0.x() ) / ( t0-t2 ),
		( t0*p2.y() - t2*p0.y() ) / ( t0-t2 ),
		( t0*p2.z() - t2*p0.z() ) / ( t0-t2 ) );
		p4.setCoordinate(
		( t1*p2.x() - t2*p1.x() ) / ( t1-t2 ),
		( t1*p2.y() - t2*p1.y() ) / ( t1-t2 ),
		( t1*p2.z() - t2*p1.z() ) / ( t1-t2 ) );
		return (t2 > 0.0) ? 2 : 5;
	}else if( t1 * t2 < 0.0 ){


		p3.setCoordinate(
		( t1*p2.x() - t2*p1.x() ) / ( t1-t2 ),
		( t1*p2.y() - t2*p1.y() ) / ( t1-t2 ),
		( t1*p2.z() - t2*p1.z() ) / ( t1-t2 ) );
		return (t1 > 0.0) ? 6 : 9;
	}else if( t2 * t0 < 0.0 ){


		p3.setCoordinate(
		( t2*p0.x() - t0*p2.x() ) / ( t2-t0 ),
		( t2*p0.y() - t0*p2.y() ) / ( t2-t0 ),
		( t2*p0.z() - t0*p2.z() ) / ( t2-t0 ) );
		return (t2 > 0.0) ? 7 : 10;
	}else if( t0 * t1 < 0.0 ){


		p3.setCoordinate(
		( t0*p1.x() - t1*p0.x() ) / ( t0-t1 ),
		( t0*p1.y() - t1*p0.y() ) / ( t0-t1 ),
		( t0*p1.z() - t1*p0.z() ) / ( t0-t1 ) );
		return (t0 > 0.0) ? 8 : 11;
	}
	return (t0 > 0.0) ? -1 : -2;
}

kmb::Point3D
kmb::Plane::projectOnPlane(const kmb::Point3D& p) const
{
	double t = this->evaluate(p);
	return kmb::Point3D(p.x() - t*a,p.y()-t*b,p.z()-t*c);
}

void
kmb::PlaneXY::setZ(double z0)
{
	this->d = -z0;
}

void
kmb::PlaneYZ::setX(double x0)
{
	this->d = -x0;
}

void
kmb::PlaneZX::setY(double y0)
{
	this->d = -y0;
}

bool
kmb::PlaneXY::getIntersection(const double z0,const kmb::Point3D &p0,const kmb::Point3D &p1, kmb::Point3D &pt, double &t)
{
	double diffZ = p1.z() - p0.z();
	if( diffZ != 0.0 ){
		t = ( z0 - p0.z() ) / diffZ;
		pt.setCoordinate( (1.0-t) * p0.x() + t * p1.x(), (1.0-t) * p0.y() + t * p1.y(), z0 );
		return true;
	}
	return false;
}

bool
kmb::PlaneYZ::getIntersection(const double x0,const kmb::Point3D &p0,const kmb::Point3D &p1, kmb::Point3D &pt, double &t)
{
	double diffX = p1.x() - p0.x();
	if( diffX != 0.0 ){
		t = ( x0 - p0.x() ) / diffX;
		pt.setCoordinate( x0, (1.0-t) * p0.y() + t * p1.y(), (1.0-t) * p0.z() + t * p1.z() );
		return true;
	}
	return false;
}

bool
kmb::PlaneZX::getIntersection(const double y0,const kmb::Point3D &p0,const kmb::Point3D &p1, kmb::Point3D &pt, double &t)
{
	double diffY = p1.y() - p0.y();
	if( diffY != 0.0 ){
		t = ( y0 - p0.y() ) / diffY;
		pt.setCoordinate( (1.0-t) * p0.x() + t * p1.x(), y0, (1.0-t) * p0.z() + t * p1.z() );
		return true;
	}
	return false;
}

int
kmb::PlaneXY::getIntersectionTriangle(
		const double zz,const kmb::Point3D &p0,const kmb::Point3D &p1, const kmb::Point3D &p2,kmb::Point3D &p3, kmb::Point3D &p4)
{
	double z0 = p0.z() - zz;
	double z1 = p1.z() - zz;
	double z2 = p2.z() - zz;
	if( z0 * z1 < 0.0 && z0 * z2 < 0.0 ){
		p3.setCoordinate(
			( z1*p0.x() - z0*p1.x() ) / ( z1-z0 ),
			( z1*p0.y() - z0*p1.y() ) / ( z1-z0 ),
			zz );
		p4.setCoordinate(
			( z2*p0.x() - z0*p2.x() ) / ( z2-z0 ),
			( z2*p0.y() - z0*p2.y() ) / ( z2-z0 ),
			zz );
		return (z0 > 0.0) ? 0 : 3;
	}else if( z1 * z2 < 0.0 && z1 * z0 < 0.0 ){
		p3.setCoordinate(
			( z2*p1.x() - z1*p2.x() ) / ( z2-z1 ),
			( z2*p1.y() - z1*p2.y() ) / ( z2-z1 ),
			zz );
		p4.setCoordinate(
			( z0*p1.x() - z1*p0.x() ) / ( z0-z1 ),
			( z0*p1.y() - z1*p0.y() ) / ( z0-z1 ),
			zz );
		return (z1 > 0.0) ? 1 : 4;
	}else if( z2 * z0 < 0.0 && z2 * z1 < 0.0 ){
		p3.setCoordinate(
			( z0*p2.x() - z2*p0.x() ) / ( z0-z2 ),
			( z0*p2.y() - z2*p0.y() ) / ( z0-z2 ),
			zz );
		p4.setCoordinate(
			( z1*p2.x() - z2*p1.x() ) / ( z1-z2 ),
			( z1*p2.y() - z2*p1.y() ) / ( z1-z2 ),
			zz );
		return (z2 > 0.0) ? 2 : 5;
	}else if( z1 * z2 < 0.0 && fabs(z0) < thres ){

		p3.setCoordinate(
			( z1*p2.x() - z2*p1.x() ) / ( z1-z2 ),
			( z1*p2.y() - z2*p1.y() ) / ( z1-z2 ),
			zz );
		return (z1 > 0.0) ? 6 : 9;
	}else if( z2 * z0 < 0.0 && fabs(z1) < thres ){

		p3.setCoordinate(
			( z2*p0.x() - z0*p2.x() ) / ( z2-z0 ),
			( z2*p0.y() - z0*p2.y() ) / ( z2-z0 ),
			zz );
		return (z2 > 0.0) ? 7 : 10;
	}else if( z0 * z1 < 0.0 && fabs(z2) < thres ){

		p3.setCoordinate(
			( z0*p1.x() - z1*p0.x() ) / ( z0-z1 ),
			( z0*p1.y() - z1*p0.y() ) / ( z0-z1 ),
			zz );
		return (z0 > 0.0) ? 8 : 11;
	}
	return (z0+z1+z2 > 0.0) ? -1 : -2;
}

int
kmb::PlaneYZ::getIntersectionTriangle(
		const double xx,const kmb::Point3D &p0,const kmb::Point3D &p1, const kmb::Point3D &p2,kmb::Point3D &p3, kmb::Point3D &p4)
{
	double x0 = p0.x() - xx;
	double x1 = p1.x() - xx;
	double x2 = p2.x() - xx;
	if( x0 * x1 < 0.0 && x0 * x2 < 0.0 ){
		p3.setCoordinate(
			xx,
			( x1*p0.y() - x0*p1.y() ) / ( x1-x0 ),
			( x1*p0.z() - x0*p1.z() ) / ( x1-x0 ) );
		p4.setCoordinate(
			xx,
			( x2*p0.y() - x0*p2.y() ) / ( x2-x0 ),
			( x2*p0.z() - x0*p2.z() ) / ( x2-x0 ) );
		return (x0 > 0.0) ? 0 : 3;
	}else if( x1 * x2 < 0.0 && x1 * x0 < 0.0 ){
		p3.setCoordinate(
			xx,
			( x2*p1.y() - x1*p2.y() ) / ( x2-x1 ),
			( x2*p1.z() - x1*p2.z() ) / ( x2-x1 ) );
		p4.setCoordinate(
			xx,
			( x0*p1.y() - x1*p0.y() ) / ( x0-x1 ),
			( x0*p1.z() - x1*p0.z() ) / ( x0-x1 ) );
		return (x1 > 0.0) ? 1 : 4;
	}else if( x2 * x0 < 0.0 && x2 * x1 < 0.0 ){
		p3.setCoordinate(
			xx,
			( x0*p2.y() - x2*p0.y() ) / ( x0-x2 ),
			( x0*p2.z() - x2*p0.z() ) / ( x0-x2 ) );
		p4.setCoordinate(
			xx,
			( x1*p2.y() - x2*p1.y() ) / ( x1-x2 ),
			( x1*p2.z() - x2*p1.z() ) / ( x1-x2 ) );
		return (x2 > 0.0) ? 2 : 5;
	}else if( x1 * x2 < 0.0 && fabs(x0) < thres ){

		p3.setCoordinate(
			xx,
			( x1*p2.y() - x2*p1.y() ) / ( x1-x2 ),
			( x1*p2.z() - x2*p1.z() ) / ( x1-x2 ) );
		return (x1 > 0.0) ? 6 : 9;
	}else if( x2 * x0 < 0.0 && fabs(x1) < thres ){

		p3.setCoordinate(
			xx,
			( x2*p0.y() - x0*p2.y() ) / ( x2-x0 ),
			( x2*p0.z() - x0*p2.z() ) / ( x2-x0 ) );
		return (x2 > 0.0) ? 7 : 10;
	}else if( x0 * x1 < 0.0 && fabs(x2) < thres ){

		p3.setCoordinate(
			xx,
			( x0*p1.y() - x1*p0.y() ) / ( x0-x1 ),
			( x0*p1.z() - x1*p0.z() ) / ( x0-x1 ) );
		return (x0 > 0.0) ? 8 : 11;
	}
	return (x0+x1+x2 > 0.0) ? -1 : -2;
}

int
kmb::PlaneZX::getIntersectionTriangle(
		const double yy,const kmb::Point3D &p0,const kmb::Point3D &p1, const kmb::Point3D &p2,kmb::Point3D &p3, kmb::Point3D &p4)
{
	double y0 = p0.y() - yy;
	double y1 = p1.y() - yy;
	double y2 = p2.y() - yy;
	if( y0 * y1 < 0.0 && y0 * y2 < 0.0 ){
		p3.setCoordinate(
			( y1*p0.x() - y0*p1.x() ) / ( y1-y0 ),
			yy,
			( y1*p0.z() - y0*p1.z() ) / ( y1-y0 ) );
		p4.setCoordinate(
			( y2*p0.x() - y0*p2.x() ) / ( y2-y0 ),
			yy,
			( y2*p0.z() - y0*p2.z() ) / ( y2-y0 ) );
		return (y0 > 0.0) ? 0 : 3;
	}else if( y1 * y2 < 0.0 && y1 * y0 < 0.0 ){
		p3.setCoordinate(
			( y2*p1.x() - y1*p2.x() ) / ( y2-y1 ),
			yy,
			( y2*p1.z() - y1*p2.z() ) / ( y2-y1 ) );
		p4.setCoordinate(
			( y0*p1.x() - y1*p0.x() ) / ( y0-y1 ),
			yy,
			( y0*p1.z() - y1*p0.z() ) / ( y0-y1 ) );
		return (y1 > 0.0) ? 1 : 4;
	}else if( y2 * y0 < 0.0 && y2 * y1 < 0.0 ){
		p3.setCoordinate(
		( y0*p2.x() - y2*p0.x() ) / ( y0-y2 ),
		yy,
		( y0*p2.z() - y2*p0.z() ) / ( y0-y2 ) );
		p4.setCoordinate(
		( y1*p2.x() - y2*p1.x() ) / ( y1-y2 ),
		yy,
		( y1*p2.z() - y2*p1.z() ) / ( y1-y2 ) );
		return (y2 > 0.0) ? 2 : 5;
	}else if( y1 * y2 < 0.0 && fabs(y0) < thres ){

		p3.setCoordinate(
			( y1*p2.x() - y2*p1.x() ) / ( y1-y2 ),
			yy,
			( y1*p2.z() - y2*p1.z() ) / ( y1-y2 ) );
		return (y1 > 0.0) ? 6 : 9;
	}else if( y2 * y0 < 0.0 && fabs(y1) < thres ){

		p3.setCoordinate(
			( y2*p0.x() - y0*p2.x() ) / ( y2-y0 ),
			yy,
			( y2*p0.z() - y0*p2.z() ) / ( y2-y0 ) );
		return (y2 > 0.0) ? 7 : 10;
	}else if( y0 * y1 < 0.0 && fabs(y2) < thres ){

		p3.setCoordinate(
			( y0*p1.x() - y1*p0.x() ) / ( y0-y1 ),
			yy,
			( y0*p1.z() - y1*p0.z() ) / ( y0-y1 ) );
		return (y0 > 0.0) ? 8 : 11;
	}
	return (y0+y1+y2 > 0.0) ? -1 : -2;
}
