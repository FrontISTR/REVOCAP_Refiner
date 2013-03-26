/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Region2D                                                #
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

#include "Geometry/kmbRegion2D.h"
#include "Geometry/kmbRegion.h"
#include "Geometry/kmbGeometry3D.h"

kmb::Region2D::Region2D(void)
{
}

kmb::Region2D::~Region2D(void)
{
}

double
kmb::Region2D::distanceSq(const double x,const double y) const
{
	Point2D pt(x,y);
	return this->distanceSq( pt );
}

double
kmb::Region2D::distance(const double x,const double y) const
{
	return sqrt( this->distanceSq( x, y ) );
}

double
kmb::Region2D::distance(const kmb::Point2D& point) const
{
	return sqrt( this->distanceSq(point) );
}

