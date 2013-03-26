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
----------------------------------------------------------------------*/
#include "Geometry/kmbSimplexRegion.h"
#include "Geometry/kmbBoxRegion.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbGeometry3D.h"

#include <cfloat>

kmb::SimplexRegion::SimplexRegion(double d)
: dist(d)
{
}

kmb::SimplexRegion::~SimplexRegion(void)
{
}

kmb::Region::locationType
kmb::SimplexRegion::intersect(const Point3D &point) const
{
	if( 0.0 < point[0] && 0.0 < point[1] && 0.0 < point[2] && point[0] + point[1] + point[2] < dist ){
		return kmb::Region::INSIDE;
	}else if( 0.0 <= point[0] && 0.0 <= point[1] && 0.0 <= point[2] && point[0] + point[1] + point[2] <= dist ){
		return kmb::Region::ON_BOUNDARY;
	}else{
		return kmb::Region::OUTSIDE;
	}
}

bool
kmb::SimplexRegion::intersect(const BoxRegion& box) const
{
	kmb::Point3D p0( 0.0, 0.0, 0.0);
	kmb::Point3D p1(dist, 0.0, 0.0);
	kmb::Point3D p2( 0.0,dist, 0.0);
	kmb::Point3D p3( 0.0, 0.0,dist);
	return
		box.intersect(p1,p2,p3) || box.intersect(p0,p3,p2) || box.intersect(p0,p1,p3) || box.intersect(p0,p2,p1);
}

double
kmb::SimplexRegion::distanceSq(const Point3D &point) const
{
	kmb::Minimizer minimizer;
	kmb::Point3D p0( 0.0, 0.0, 0.0);
	kmb::Point3D p1(dist, 0.0, 0.0);
	kmb::Point3D p2( 0.0,dist, 0.0);
	kmb::Point3D p3( 0.0, 0.0,dist);
	minimizer.update( point.distanceSqToTriangle(p1,p2,p3) );
	minimizer.update( point.distanceSqToTriangle(p0,p3,p2) );
	minimizer.update( point.distanceSqToTriangle(p0,p1,p3) );
	minimizer.update( point.distanceSqToTriangle(p0,p2,p1) );
	return minimizer.getMin();
}
