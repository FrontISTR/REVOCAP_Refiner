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
#pragma once

#include "Geometry/kmbGeometry2D.h"

namespace kmb{

class Region2D
{
public:
	enum locationType{
		UNKNWON = -1,
		INSIDE,
		OUTSIDE,
		ON_BOUNDARY
	};
	Region2D(void);
	virtual ~Region2D(void);

	virtual locationType intersect(const Point2D &point) const = 0;

	virtual double distanceSq(const Point2D &point) const = 0;
	virtual double distanceSq(const double x,const double y) const;
	virtual double distance(const Point2D& point) const;
	virtual double distance(const double x,const double y) const;
};

}
