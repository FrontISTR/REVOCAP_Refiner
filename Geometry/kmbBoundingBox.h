/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BoundingBox                                             #
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
#include "Geometry/kmbBoxRegion.h"
#include "Geometry/kmbBoxRegion2D.h"

namespace kmb{

class BoundingBox : public BoxRegion
{
public:
	BoundingBox(void);
public:
	virtual ~BoundingBox(void);
	void initialize(void);
	bool update(const Point3D& point);
	bool update(const double x, const double y,const double z);
	bool update(const BoxRegion& box);
	bool valid(void) const;
};

class BoundingBox2D : public BoxRegion2D
{
public:
	BoundingBox2D(void);
public:
	virtual ~BoundingBox2D(void);
	void initialize(void);
	bool update(const double x, const double y);
	bool update(const Point2D& point);
	bool valid(void) const;
};

class BoundingBox1D
{
public:
	BoundingBox1D(void);
public:
	virtual ~BoundingBox1D(void);
	double getMax(void) const { return maxValue; }
	double getMin(void) const { return minValue; }
	void initialize(void);
	bool update(double value);
	double center(void) const;
	double range(void) const;
	bool valid(void) const;

	bool intersect(const kmb::BoundingBox1D& box) const;
	double intersectLength(const kmb::BoundingBox1D& box) const;
	double distanceSq(const kmb::BoundingBox1D& box) const;
private:
	double maxValue;
	double minValue;
};

}
