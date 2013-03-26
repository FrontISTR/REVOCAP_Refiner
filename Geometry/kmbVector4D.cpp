/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Vector4D                                                #
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
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbGeometry2D.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbGeometry.h"

kmb::Tuple4D::Tuple4D(void)
{
	v[0] = 0.0;
	v[1] = 0.0;
	v[2] = 0.0;
	v[3] = 0.0;
}

kmb::Tuple4D::Tuple4D(const double x, const double y, const double z, const double w)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

kmb::Tuple4D::Tuple4D(const Tuple4D &other)
{
	v[0] = other.v[0];
	v[1] = other.v[1];
	v[2] = other.v[2];
	v[3] = other.v[3];
}

void
kmb::Tuple4D::zero(void)
{
	v[0] = 0.0;
	v[1] = 0.0;
	v[2] = 0.0;
	v[3] = 0.0;
}

void
kmb::Tuple4D::addCoordinate(const double x,const double y,const double z,const double w)
{
	v[0] += x;
	v[1] += y;
	v[2] += z;
	v[3] += w;
}

void
kmb::Tuple4D::scale(const double r)
{
	v[0] *= r;
	v[1] *= r;
	v[2] *= r;
	v[3] *= r;
}

kmb::Tuple4D&
kmb::Tuple4D::operator =(const Tuple4D& other)
{
	this->v[0] = other.x();
	this->v[1] = other.y();
	this->v[2] = other.z();
	this->v[3] = other.w();
	return *this;
}



kmb::Vector4D
kmb::Vector4D::scalar(const double s) const{
	return kmb::Vector4D(s*x(),s*y(),s*z(),s*w());
}

double
kmb::Vector4D::lengthSq(void) const
{
	return
		this->v[0] * this->v[0] +
		this->v[1] * this->v[1] +
		this->v[2] * this->v[2] +
		this->v[3] * this->v[3];
}

double
kmb::Vector4D::length(void) const
{
	return sqrt(lengthSq());
}

double
kmb::Vector4D::abs(void) const
{
	return sqrt(lengthSq());
}

double
kmb::Vector4D::normalize()
{
	double len = length();
	if( len != 0.0 ){
		this->v[0] /= len;
		this->v[1] /= len;
		this->v[2] /= len;
		this->v[3] /= len;
		return 1.0;
	}
	return 0.0;
}
