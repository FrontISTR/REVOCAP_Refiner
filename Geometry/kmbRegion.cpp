/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Region                                                  #
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
#include "Geometry/kmbRegion.h"
#include "Geometry/kmbGeometry3D.h"

kmb::Region::Region(void)
{
}

kmb::Region::~Region(void)
{
}

double
kmb::Region::distanceSq(const double x,const double y,const double z) const
{
	Point3D pt(x,y,z);
	return this->distanceSq( pt );
}

double
kmb::Region::distance(const double x,const double y,const double z) const
{
	return sqrt( this->distanceSq( x, y, z ) );
}

double
kmb::Region::distance(const kmb::Point3D& point) const
{
	return sqrt( this->distanceSq(point) );
}

