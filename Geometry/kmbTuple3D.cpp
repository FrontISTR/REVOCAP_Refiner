/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Tuple3D                                                 #
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
#include "Geometry/kmbGeometry3D.h"

kmb::Tuple3D::Tuple3D(void)
{
	v[0] = 0.0;
	v[1] = 0.0;
	v[2] = 0.0;
}

kmb::Tuple3D::Tuple3D(const double x, const double y, const double z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

kmb::Tuple3D::Tuple3D(const Tuple3D &other)
{
	v[0] = other.v[0];
	v[1] = other.v[1];
	v[2] = other.v[2];
}

void
kmb::Tuple3D::zero(void)
{
	v[0] = 0.0;
	v[1] = 0.0;
	v[2] = 0.0;
}

void
kmb::Tuple3D::addCoordinate(const double x,const double y,const double z)
{
	v[0] += x;
	v[1] += y;
	v[2] += z;
}

void
kmb::Tuple3D::scale(const double r)
{
	v[0] *= r;
	v[1] *= r;
	v[2] *= r;
}

kmb::Tuple3D&
kmb::Tuple3D::operator =(const Tuple3D& other)
{
	this->v[0] = other.x();
	this->v[1] = other.y();
	this->v[2] = other.z();
	return *this;
}

