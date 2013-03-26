/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Geometry2D                                              #
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
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbIdTypes.h"
#include "Geometry/kmbGeometry.h"
#include <cmath>
#include <cstring>
#include <cfloat>

kmb::Tuple2D::Tuple2D(void)
{
	this->v[0] = 0.0;
	this->v[1] = 0.0;
}

kmb::Tuple2D::~Tuple2D(void)
{
}

kmb::Tuple2D::Tuple2D(const double x, const double y)
{
	this->v[0] = x;
	this->v[1] = y;
}

kmb::Tuple2D::Tuple2D(const Tuple2D &other)
{
	this->v[0] = other.x();
	this->v[1] = other.y();
}

void kmb::Tuple2D::
zero()
{
	v[0] = 0.0;
	v[1] = 0.0;
}

void
kmb::Tuple2D::addCoordinate(const double x,const double y)
{
	v[0] += x;
	v[1] += y;
}

void
kmb::Tuple2D::scale(const double r)
{
	v[0] *= r;
	v[1] *= r;
}

kmb::Tuple2D&
kmb::Tuple2D::operator =(const Tuple2D& other)
{
	this->v[0] = other.x();
	this->v[1] = other.y();
	return *this;
}

kmb::Point2D kmb::Point2D::infinity(DBL_MAX,DBL_MAX);

double
kmb::Point2D::distance(const Point2D& other) const
{
	kmb::Vector2D v(*this,other);
	return v.length();
}

double
kmb::Point2D::distanceSq(const Point2D& other) const
{
	kmb::Vector2D v(*this,other);
	return v.lengthSq();
}

double
kmb::Point2D::distance(double x,double y) const
{
	return sqrt( distanceSq(x,y) );
}

double
kmb::Point2D::distanceSq(double x,double y) const
{
	return
		(this->x()-x)*(this->x()-x) +
		(this->y()-y)*(this->y()-y);
}

kmb::Point2D
kmb::Point2D::operator+(const Vector2D& other)
{
	return Point2D(v[0]+other.x(),v[1]+other.y());
}

kmb::Point2D
kmb::Point2D::operator-(const Vector2D& other)
{
	return Point2D(v[0]-other.x(),v[1]-other.y());
}

kmb::Point2D&
kmb::Point2D::operator+=(const Vector2D& other)
{
	this->v[0] += other.x();
	this->v[1] += other.y();
	return *this;
}

kmb::Point2D&
kmb::Point2D::operator-=(const Vector2D& other)
{
	this->v[0] -= other.x();
	this->v[1] -= other.y();
	return *this;
}

bool
kmb::Point2D::operator==(const Point2D& other) const
{
	return (this->x() == other.x()) && (this->y() == other.y());
}

double
kmb::Point2D::distanceSqToSegment(const Point2D& a,const Point2D& b) const
{
	double t;
	return distanceSqToSegment(a,b,t);
}

double
kmb::Point2D::distanceSqToSegment(const Point2D& a,const Point2D& b,double& t) const
{
	kmb::Vector2D ab(b,a);
	kmb::Vector2D xa(a,*this);
	t = - (ab * xa) / ab.lengthSq();
	if( t < 0 ){
		return this->distanceSq(a);
	}else if( t > 1 ){
		return this->distanceSq(b);
	}else{
		return ( xa + ab.scalar(t) ).lengthSq();
	}
}

double
kmb::Point2D::distanceToSegment(const Point2D& a,const Point2D& b) const
{
	return sqrt( distanceSqToSegment(a,b) );
}


kmb::Point2D
kmb::Point2D::dividingPoint(const Point2D& other,double m,double n) const
{
	return Point2D(
		(n*this->x() + m*other.x()) / (m+n),
		(n*this->y() + m*other.y()) / (m+n));
}

double
kmb::Point2D::distance(const kmb::Point2D& a,const kmb::Point2D& b)
{
	return a.distance(b);
}

double
kmb::Point2D::distanceSq(const kmb::Point2D& a,const kmb::Point2D& b)
{
	return a.distanceSq(b);
}



double
kmb::Point2D::area(const kmb::Point2D& a,const kmb::Point2D& b,const kmb::Point2D &c)
{
	if( LARGER_Y(a,b) ){

		if( LARGER_Y(c,a) ){

			return
				0.5 * (
				+ c.x()*a.y() - c.y()*a.x()
				+ a.x()*b.y() - a.y()*b.x()
				+ b.x()*c.y() - b.y()*c.x() );
		}else if( LARGER_Y(c,b) ){

			return
				0.5 * (
				- a.x()*c.y() + a.y()*c.x()
				- c.x()*b.y() + c.y()*b.x()
				- b.x()*a.y() + b.y()*a.x() );
		}else{

			return
				0.5 * (
				+ a.x()*b.y() - a.y()*b.x()
				+ b.x()*c.y() - b.y()*c.x()
				+ c.x()*a.y() - c.y()*a.x() );
		}
	}else{

		if( LARGER_Y(c,b) ){

			return
				0.5 * (
				- c.x()*b.y() + c.y()*b.x()
				- b.x()*a.y() + b.y()*a.x()
				- a.x()*c.y() + a.y()*c.x() );
		}else if( LARGER_Y(c,a) ){

			return
				0.5 * (
				+ b.x()*c.y() - b.y()*c.x()
				+ c.x()*a.y() - c.y()*a.x()
				+ a.x()*b.y() - a.y()*b.x() );
		}else{

			return
				0.5 * (
				- b.x()*a.y() + b.y()*a.x()
				- a.x()*c.y() + a.y()*c.x()
				- c.x()*b.y() + c.y()*b.x() );
		}
	}





}

kmb::Point2D
kmb::Point2D::getCenter(const kmb::Point2D& a,const kmb::Point2D& b)
{
	return kmb::Point2D( (a.x() + b.x())*0.5, (a.y() + b.y())*0.5 );
}

kmb::Point2D
kmb::Point2D::getCenter(const kmb::Point2D& a,const kmb::Point2D& b,const kmb::Point2D &c)
{
	double r = 1.0/3.0;
	return kmb::Point2D( (a.x() + b.x() + c.x())*r, (a.y() + b.y() + c.y())*r );
}

double kmb::Point2D::angle(const kmb::Point2D &a,const kmb::Point2D &b,const kmb::Point2D &c)
{
	kmb::Vector2D v1(a,b);
	kmb::Vector2D v2(c,b);
	return kmb::Vector2D::angle(v1,v2);
}

double kmb::Point2D::angle2(const Point2D &a,const Point2D &b,const Point2D &c)
{
	if( a == c ){
		return 0.0;
	}
	kmb::Vector2D v1(a,b);
	kmb::Vector2D v2(c,b);
	return kmb::Vector2D::angle2(v1,v2);
}

double kmb::Point2D::cos(const Point2D &a,const Point2D &b,const Point2D &c)
{
	kmb::Vector2D v1(a,b);
	kmb::Vector2D v2(c,b);
	return kmb::Vector2D::cos(v1,v2);
}

double kmb::Point2D::sin(const Point2D &a,const Point2D &b,const Point2D &c)
{
	kmb::Vector2D v1(a,b);
	kmb::Vector2D v2(c,b);
	return kmb::Vector2D::sin(v1,v2);
}

void
kmb::Point2D::calcMinorCoordinate
( const kmb::Point2D& a, const kmb::Point2D& b, const kmb::Point2D& c, const kmb::Point2D& x,double coordinate[3])
{
	kmb::Vector2D xa(a,x);
	kmb::Vector2D xb(b,x);
	kmb::Vector2D xc(c,x);


	coordinate[0] = 0.5 * (xb % xc);


	coordinate[1] = 0.5 * (xc % xa);


	coordinate[2] = 0.5 * (xa % xb);
}

bool
kmb::Point2D::intesectSegments(const kmb::Point2D& a0, const kmb::Point2D& a1, const kmb::Point2D& b0, const kmb::Point2D& b1)
{
	if( kmb::Point2D::area(a0,a1,b0) * kmb::Point2D::area(a0,a1,b1) <= 0.0 &&
		kmb::Point2D::area(b0,b1,a0) * kmb::Point2D::area(b0,b1,a0) <= 0.0 )
	{
		return true;
	}else{
		return false;
	}
}


kmb::Vector2D::Vector2D(const Point2D& p,const Point2D& q)
{
	for(int i=0;i<2;++i){
		v[i] = p.getCoordinate(i) - q.getCoordinate(i);
	}
}

kmb::Vector2D
kmb::Vector2D::scalar(const double s) const
{
	return kmb::Vector2D(s*x(),s*y());
}

double
kmb::Vector2D::lengthSq() const
{
	return
		this->v[0] * this->v[0] +
		this->v[1] * this->v[1];
}

double
kmb::Vector2D::length() const
{
	return sqrt(lengthSq());
}

double
kmb::Vector2D::abs() const
{
	return sqrt(lengthSq());
}

double
kmb::Vector2D::normalize()
{
	double len = length();
	if( len != 0.0 ){
		this->v[0] /= len;
		this->v[1] /= len;
		return 1.0;
	}
	return 0.0;
}

void
kmb::Vector2D::rotate(double angle)
{
	double c = ::cos(angle);
	double s = ::sin(angle);
	double x = c*v[0] - s*v[1];
	double y = s*v[0] + c*v[1];
	v[0] = x;
	v[1] = y;
}

double
kmb::Vector2D::cos(const Vector2D &v0,const Vector2D &v1)
{
	return (v0 * v1) / (v0.length() * v1.length());
}

double
kmb::Vector2D::sin(const Vector2D &v0,const Vector2D &v1)
{
	double outer = v0 % v1;
	return outer / (v0.length() * v1.length());
}

double
kmb::Vector2D::angle(const Vector2D &v0,const Vector2D &v1)
{
	double c = kmb::Vector2D::cos(v0,v1);
	double s = v0 % v1;
	return ANGLE(c,s);
}

double
kmb::Vector2D::angle2(const Vector2D &v0,const Vector2D &v1)
{
	double c = kmb::Vector2D::cos(v0,v1);
	double s = v0 % v1;
	return ANGLE2(c,s);
}

double
kmb::Vector2D::abs(const double v[2])
{
	return sqrt( v[0]*v[0] + v[1]*v[1] );
}
