/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Plane                                                   #
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
#include "Geometry/kmbGeometry2D.h"

namespace kmb{

/**
 * ãÛä‘ì‡ÇÃïΩñ 
 * ax + by + cz + d = 0
 * a*a + b*b + c*c = 1 Ç∆Ç»ÇÈÇÊÇ§Ç…ê≥ãKâªÇµÇƒÇ®Ç≠
 */

class Plane
{
protected:
	double a;
	double b;
	double c;
	double d;
	static double thres;
public:
	Plane(const double a,const double b,const double c,const double d);
	virtual ~Plane(void);
	static Plane* createFromPoints(const Point3D &p,const Point3D &q,const Point3D &r);
	static Plane* createFromBaseNormal(const Point3D &base,const Vector3D &normal);
	static Vector3D calcNormal(const Point3D &a,const Point3D &b,const Point3D &c);
	double distance(const Point3D &pt) const;
	double distance(const double x,const double y,const double z) const;
	double evaluate(const double x,const double y,const double z) const;
	double evaluate(const Point3D &pt) const;
	Vector3D getNormal(void) const;
	double getConstant(void) const;

	Point3D* createIntersectPoint(const Point3D &p0,const Point3D &p1) const;


	bool getIntersection(const Point3D &p0,const Point3D &p1, Point3D &pt, double &t) const;
















	int getIntersectionTriangle(const Point3D &p0,const Point3D &p1, const Point3D &p2, Point3D &p3, Point3D &p4) const;

	Point3D projectOnPlane(const Point3D& p) const;
};

class PlaneXY : public Plane
{
public:
	PlaneXY(const double z0) : Plane( 0.0, 0.0, 1.0, -z0){};
	virtual ~PlaneXY(void){};
	void setZ(double z0);
	static bool getIntersection(const double z0,const Point3D &p0,const Point3D &p1, Point3D &pt, double &t);
	static int getIntersectionTriangle(const double z0,const Point3D &p0,const Point3D &p1, const Point3D &p2, Point3D &p3, Point3D &p4);
};

class PlaneYZ : public Plane
{
public:
	PlaneYZ(const double x0) : Plane( 1.0, 0.0, 0.0, -x0){};
	virtual ~PlaneYZ(void){};
	void setX(double x0);
	static bool getIntersection(const double x0,const Point3D &p0,const Point3D &p1, Point3D &pt, double &t);
	static int getIntersectionTriangle(const double x0,const Point3D &p0,const Point3D &p1, const Point3D &p2, Point3D &p3, Point3D &p4);
};

class PlaneZX : public Plane
{
public:
	PlaneZX(const double y0) : Plane( 0.0, 1.0, 0.0, -y0){};
	virtual ~PlaneZX(void){};
	void setY(double y0);
	static bool getIntersection(const double y0,const Point3D &p0,const Point3D &p1, Point3D &pt, double &t);
	static int getIntersectionTriangle(
		const double y0,const Point3D &p0,const Point3D &p1, const Point3D &p2, Point3D &p3, Point3D &p4);
};

}
