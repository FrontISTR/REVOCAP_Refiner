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
#pragma once

#include "Geometry/kmbGeometry3D.h"

namespace kmb{

class BoxRegion;

class Region
{
public:
	enum locationType{
		UNKNOWN = -1,
		INSIDE,
		OUTSIDE,
		ON_BOUNDARY
	};
	Region(void);
	virtual ~Region(void);

	virtual locationType intersect(const Point3D &point) const = 0;
	virtual bool intersect(const BoxRegion& box) const = 0;

	virtual double distanceSq(const Point3D &point) const = 0;
	virtual double distanceSq(const double x,const double y,const double z) const;
	virtual double distance(const Point3D& point) const;
	virtual double distance(const double x,const double y,const double z) const;
};

}
