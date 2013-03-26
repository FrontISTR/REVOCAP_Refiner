/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Circle                                                  #
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
#include "Geometry/kmbCircle.h"
#include "Geometry/kmbPlane.h"
#include <cfloat>
#include <cmath>
#include <cstdlib>

kmb::Circle::Circle(void)
{
}

kmb::Circle::~Circle(void)
{
}

void
kmb::Circle::setCenter(const kmb::Point2D &pt)
{
	this->setCenter(pt.x(),pt.y());
}

void
kmb::Circle::setCenter(double x,double y)
{
	this->center.setCoordinate(x,y);
}

void
kmb::Circle::setRadius(double r)
{
	this->radius = r;
}

double
kmb::Circle::getRadius(void) const
{
	return this->radius;
}

kmb::Point2D
kmb::Circle::getCenter(void) const
{
	return kmb::Point2D(this->center);
}


kmb::Circle*
kmb::Circle::createInscribedCircle(const Point2D &a,const Point2D &b,const Point2D &c)
{
	kmb::Circle* circle = NULL;
	circle = new kmb::Circle();
	if( circle ){
		double r = kmb::Circle::getInscribedRadius(a,b,c);
		circle->setRadius(r);
		kmb::Point2D center = kmb::Circle::getInscribedCenter(a,b,c);
		circle->setCenter(center);
	}
	return circle;
}


double
kmb::Circle::getInscribedRadius(const Point2D &a,const Point2D &b,const Point2D &c)
{


	double area = fabs(kmb::Point2D::area(a,b,c));

	double ab = a.distance(b);
	double bc = b.distance(c);
	double ca = c.distance(a);
	double d = ab+bc+ca;
	if( d != 0.0 ){
		return (area*2.0) / d;
	}else{
		return 0.0;
	}
}

kmb::Point2D
kmb::Circle::getInscribedCenter(const Point2D &a,const Point2D &b,const Point2D &c)
{

	double ab = a.distance(b);
	double bc = b.distance(c);
	double ca = c.distance(a);
	double d = ab+bc+ca;
	if( d != 0.0 ){
		double x = (ab*c.x() + bc*a.x() + ca*b.x()) / d;
		double y = (ab*c.y() + bc*a.y() + ca*b.y()) / d;
		return kmb::Point2D(x,y);
	}else{

		return kmb::Point2D(a);
	}
}


kmb::Circle*
kmb::Circle::createCircumscribedCircle(const Point2D &a,const Point2D &b,const Point2D &c)
{
	kmb::Circle* circle = NULL;
	circle = new kmb::Circle();
	if( circle ){
		double r = kmb::Circle::getCircumscribedRadius(a,b,c);
		circle->setRadius(r);
		kmb::Point2D center = kmb::Circle::getCircumscribedCenter(a,b,c);
		circle->setCenter(center);
	}
	return circle;
}

double
kmb::Circle::getCircumscribedRadius(const Point2D &a,const Point2D &b,const Point2D &c)
{


	double area = fabs(kmb::Point2D::area(a,b,c));

	double ab = a.distance(b);
	double bc = b.distance(c);
	double ca = c.distance(a);
	if( area != 0.0 ){
		return ab*bc*ca/(area*4.0);
	}else if( ab==0.0 && bc==0.0 ){

		return 0.0;
	}else if( ab==0.0 ){

		return bc*0.5;
	}else if( bc==0.0 ){
		return ca*0.5;
	}else if( ca==0.0 ){
		return ab*0.5;
	}else{

		return DBL_MAX;
	}
}

kmb::Point2D
kmb::Circle::getCircumscribedCenter(const Point2D &a,const Point2D &b,const Point2D &c)
{
	kmb::Matrix2x2 mat(
		a.x()-c.x(), a.y()-c.y(),
		a.x()-b.x(), a.y()-b.y());
	double aa = a.x()*a.x() + a.y()*a.y();
	double bb = b.x()*b.x() + b.y()*b.y();
	double cc = c.x()*c.x() + c.y()*c.y();
	kmb::Vector2D vec((aa-cc)*.5,(aa-bb)*.5);
	kmb::Vector2D* ans = mat.solve(vec);
	if( ans ){
		double x = ans->x();
		double y = ans->y();
		delete ans;
		return kmb::Point2D(x,y);
	}else{
		return kmb::Point2D(DBL_MAX,DBL_MAX);
	}
}

kmb::Circle3D::Circle3D(void)
{
}

kmb::Circle3D::~Circle3D(void)
{
}

void
kmb::Circle3D::setCenter(const Point3D& pt)
{
	this->center.setCoordinate( pt.x(), pt.y(), pt.z() );
}

void
kmb::Circle3D::setCenter(const double x,const double y,const double z)
{
	this->center.setCoordinate( x, y, z );
}

void
kmb::Circle3D::setNormal(const Vector3D& pt)
{
	this->normal.setCoordinate( pt.x(), pt.y(), pt.z() );
}

void
kmb::Circle3D::setNormal(const double x,const double y,const double z)
{
	this->normal.setCoordinate( x, y, z );
}

void
kmb::Circle3D::setRadius(const double r)
{
	this->radius = r;
}

kmb::Point3D
kmb::Circle3D::getCenter(void) const
{
	return kmb::Point3D( this->center );
}

kmb::Vector3D
kmb::Circle3D::getNormal(void) const
{
	return kmb::Vector3D( this->normal );
}

double
kmb::Circle3D::getRadius(void) const
{
	return this->radius;
}


kmb::Circle3D*
kmb::Circle3D::createInscribedCircle(const Point3D &a,const Point3D &b,const Point3D &c)
{
	kmb::Circle3D* circle = NULL;
	circle = new kmb::Circle3D();
	if( circle ){
		double r = kmb::Circle3D::getCircumscribedRadius(a,b,c);
		circle->setRadius(r);
		kmb::Point3D center = kmb::Circle3D::getCircumscribedCenter(a,b,c);
		circle->setCenter(center);
		kmb::Vector3D normal = kmb::Plane::calcNormal(a,b,c);
		circle->setNormal( normal );
	}
	return circle;
}

kmb::Point3D
kmb::Circle3D::getInscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c)
{

	double ab = a.distance(b);
	double bc = b.distance(c);
	double ca = c.distance(a);
	double x = (ab*c.x() + bc*a.x() + ca*b.x()) / (ab+bc+ca);
	double y = (ab*c.y() + bc*a.y() + ca*b.y()) / (ab+bc+ca);
	double z = (ab*c.z() + bc*a.z() + ca*b.z()) / (ab+bc+ca);
	return kmb::Point3D(x,y,z);
}

double
kmb::Circle3D::getInscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c)
{


	double area = fabs(kmb::Point3D::area(a,b,c));

	double ab = a.distance(b);
	double bc = b.distance(c);
	double ca = c.distance(a);
	double d = ab+bc+ca;
	if( d != 0.0 ){
		return (area*2.0) / d;
	}else{
		return 0.0;
	}
}


kmb::Circle3D*
kmb::Circle3D::createCircumscribedCircle(const Point3D &a,const Point3D &b,const Point3D &c)
{
	kmb::Circle3D* circle = NULL;
	circle = new kmb::Circle3D();
	if( circle ){
		double r = kmb::Circle3D::getCircumscribedRadius(a,b,c);
		circle->setRadius(r);
		kmb::Point3D center = kmb::Circle3D::getCircumscribedCenter(a,b,c);
		circle->setCenter(center);
		kmb::Vector3D normal = kmb::Plane::calcNormal(a,b,c);
		circle->setNormal( normal );
	}
	return circle;
}

kmb::Point3D
kmb::Circle3D::getCircumscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c)
{
	Vector3D ab(b,a);
	Vector3D ac(c,a);
	double bc = ab*ac;
	double b2 = ab.lengthSq();
	double c2 = ac.lengthSq();
	kmb::Matrix2x2 mat(bc ,c2, b2, bc);
	kmb::Vector2D vec(c2*.5,b2*.5);
	kmb::Vector2D* ans = mat.solve(vec);
	if( ans ){
		double x = ans->x();
		double y = ans->y();
		delete ans;
		return Point3D( a + ab.scalar(x) + ac.scalar(y) );
	}else{
		return Point3D( DBL_MAX, DBL_MAX, DBL_MAX );
	}
}

double
kmb::Circle3D::getCircumscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c)
{


	double area = fabs(kmb::Point3D::area(a,b,c));

	double ab = a.distance(b);
	double bc = b.distance(c);
	double ca = c.distance(a);
	if( area != 0.0 ){
		return ab*bc*ca/(area*4.0);
	}else if( ab==0.0 && bc==0.0 ){

		return 0.0;
	}else if( ab==0.0 ){

		return bc*0.5;
	}else if( bc==0.0 ){
		return ca*0.5;
	}else if( ca==0.0 ){
		return ab*0.5;
	}else{

		return DBL_MAX;
	}
}
