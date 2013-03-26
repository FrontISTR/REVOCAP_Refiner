/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Vector3D                                                #
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
#include <cfloat>

#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbGeometry2D.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbGeometry.h"

kmb::Vector3D::Vector3D(const Point3D& p,const Point3D& q)
{
	for(int i=0;i<3;++i){
		v[i] = p.getCoordinate(i) - q.getCoordinate(i);
	}
}



kmb::Vector3D
kmb::Vector3D::scalar(const double s) const{
	return kmb::Vector3D(s*x(),s*y(),s*z());
}


double
kmb::Vector3D::lengthSq(void) const
{
	return
		this->v[0] * this->v[0] +
		this->v[1] * this->v[1] +
		this->v[2] * this->v[2];
}

double
kmb::Vector3D::length(void) const
{
	return sqrt(lengthSq());
}

double
kmb::Vector3D::abs(void) const
{
	return sqrt(lengthSq());
}

double
kmb::Vector3D::normalize()
{
	double len = length();
	if( len != 0.0 ){
		this->v[0] /= len;
		this->v[1] /= len;
		this->v[2] /= len;
		return 1.0;
	}
	return 0.0;
}

double
kmb::Vector3D::triple(const Vector3D &v0,const Vector3D &v1,const Vector3D &v2)
{
	return
		+ v0.v[0] * v1.v[1] * v2.v[2]
		+ v0.v[1] * v1.v[2] * v2.v[0]
		+ v0.v[2] * v1.v[0] * v2.v[1]
		- v0.v[0] * v1.v[2] * v2.v[1]
		- v0.v[1] * v1.v[0] * v2.v[2]
		- v0.v[2] * v1.v[1] * v2.v[0];
}

double
kmb::Vector3D::cos(const Vector3D &v0,const Vector3D &v1)
{
	return (v0 * v1) / (v0.length() * v1.length());
}

double
kmb::Vector3D::sin(const Vector3D &v0,const Vector3D &v1)
{
	kmb::Vector3D outer = v0 % v1;
	return sqrt(outer.lengthSq() / (v0.lengthSq() * v1.lengthSq()) );
}

double
kmb::Vector3D::angle(const Vector3D &v0,const Vector3D &v1)
{
	double c = kmb::Vector3D::cos(v0,v1);
	return COS2ANGLE(c);
}

double
kmb::Vector3D::abs(const double v[3])
{
	return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

double
kmb::Vector3D::inner(const double v0[3],const double v1[3])
{
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}
