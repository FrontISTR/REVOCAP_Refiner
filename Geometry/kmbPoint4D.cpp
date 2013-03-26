/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point4D                                                 #
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
#include <cmath>
#include <cfloat>
#include "Geometry/kmbGeometry4D.h"

kmb::Point4D kmb::Point4D::infinity(DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX);

kmb::Point4D
kmb::Point4D::operator+(const Vector4D& other) const
{
	return Point4D(v[0]+other.x(),v[1]+other.y(),v[2]+other.z(),v[3]+other.w());
}

kmb::Point4D
kmb::Point4D::operator-(const Vector4D& other) const
{
	return Point4D(v[0]-other.x(),v[1]-other.y(),v[2]-other.z(),v[3]-other.w());
}

kmb::Point4D&
kmb::Point4D::operator+=(const Vector4D& other)
{
	this->v[0] += other.x();
	this->v[1] += other.y();
	this->v[2] += other.z();
	this->v[3] += other.w();
	return *this;
}

kmb::Point4D&
kmb::Point4D::operator-=(const Vector4D& other)
{
	this->v[0] -= other.x();
	this->v[1] -= other.y();
	this->v[2] -= other.z();
	this->v[3] -= other.w();
	return *this;
}

bool
kmb::Point4D::operator==(const Point4D& other) const
{
	return (this->x() == other.x()) && (this->y() == other.y()) && (this->z() == other.z()) && (this->w() == other.w());
}

double
kmb::Point4D::distance(const Point4D& other) const
{
	return this->distance(other.x(),other.y(),other.z(),other.w());
}

double
kmb::Point4D::distanceSq(const Point4D& other) const
{
	return this->distanceSq(other.x(),other.y(),other.z(),other.w());
}

double
kmb::Point4D::distance(double x,double y,double z,double w) const
{
	return sqrt( distanceSq(x,y,z,w) );
}

double
kmb::Point4D::distanceSq(double x,double y,double z,double w) const
{
	return
		(this->x()-x)*(this->x()-x) +
		(this->y()-y)*(this->y()-y) +
		(this->z()-z)*(this->z()-z) +
		(this->w()-w)*(this->w()-w);
}

double
kmb::Point4D::distance(const kmb::Point4D& a,const kmb::Point4D& b)
{
	return a.distance(b);
}

double
kmb::Point4D::distanceSq(const kmb::Point4D& a,const kmb::Point4D& b)
{
	return a.distanceSq(b);
}
