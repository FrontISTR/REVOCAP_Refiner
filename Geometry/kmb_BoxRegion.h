/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : BoxRegion                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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

#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_Region.h"
#include "Geometry/kmb_Sphere.h"

namespace kmb{


class BoxRegion : public Region
{
protected:

	static double thres;
protected:
	Point3D maxPoint;
	Point3D minPoint;
public:
	BoxRegion(void);
	BoxRegion(Point3D &l,Point3D &u);
	BoxRegion(double x0,double y0,double z0,double x1,double y1,double z1);
	BoxRegion(const BoxRegion &other);
	virtual ~BoxRegion(void);
	BoxRegion& operator=(const BoxRegion &other);

	void setMinMax(double x0,double y0,double z0,double x1,double y1,double z1);
	double minX(void) const;
	double minY(void) const;
	double minZ(void) const;
	double maxX(void) const;
	double maxY(void) const;
	double maxZ(void) const;
	double centerX(void) const;
	double centerY(void) const;
	double centerZ(void) const;
	double rangeX(void) const;
	double rangeY(void) const;
	double rangeZ(void) const;
	double diameter(void) const;
	const Point3D& getMin(void) const;
	const Point3D& getMax(void) const;
	void getCenter(kmb::Point3D& center) const;
	double getVolume(void) const;

	virtual kmb::Region::locationType intersect(const kmb::Point3D &point) const;
	virtual kmb::Region::locationType intersect(const double x,const double y,const double z) const;
	virtual double distanceSq(const kmb::Point3D &point) const;
	virtual double distanceSq(const double x,const double y,const double z) const;

	bool intersect(const kmb::BoxRegion& box) const;
	double intersectVolume(const BoxRegion& box) const;
	double distanceSq(const kmb::BoxRegion& box) const;

	void expand(double x,double y,double z);
	void expand(double ratio);



	bool intersect(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c) const;
	double intersectArea(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c) const;

	double getThreshold(void) const;
	void setThreshold(double thres);
private:


	double intersectArea_minx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_maxx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_miny(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_maxy(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_minz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_maxz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
};

}
