/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BoxRegion2D                                             #
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
#include "Geometry/kmbRegion2D.h"

namespace kmb{


class BoxRegion2D : public Region2D
{
protected:
	Point2D maxPoint;
	Point2D minPoint;
public:
	BoxRegion2D(const Point2D &l,const Point2D &u);
	BoxRegion2D(double x0,double y0,double x1,double y1);
	BoxRegion2D(const BoxRegion2D &other);
	virtual ~BoxRegion2D(void);
	BoxRegion2D& operator=(const BoxRegion2D &other);

	double minX(void) const;
	double minY(void) const;
	double maxX(void) const;
	double maxY(void) const;
	double centerX(void) const;
	double centerY(void) const;
	double rangeX(void) const;
	double rangeY(void) const;

	double range(void) const;
	double diameter(void) const;
	const Point2D& getMin(void) const;
	const Point2D& getMax(void) const;
	void getCenter(kmb::Point2D& center) const;
	double getArea(void) const;

	virtual locationType intersect(const Point2D &point) const;
	virtual double distanceSq(const Point2D &point) const;
	virtual double distanceSq(const double x,const double y) const;

	bool intersect(const BoxRegion2D& box) const;
	double intersectArea(const BoxRegion2D& box) const;
	double distanceSq(const kmb::BoxRegion2D& box) const;

	void expand(double x,double y);
	void expand(double ratio);


	void crossOnLine(const kmb::Point2D& origin, const kmb::Vector2D& dir, double &min_t, double &max_t) const;
};

}
