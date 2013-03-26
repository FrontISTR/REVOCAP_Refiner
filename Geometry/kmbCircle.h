/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Circle                                                  #
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
#include "Geometry/kmbGeometry3D.h"

namespace kmb{

class Circle
{
public:
	Circle(void);
	virtual ~Circle(void);
private:
	Point2D center;
	double radius;
public:

	void setCenter(const Point2D &pt);
	void setCenter(double x,double y);
	void setRadius(double r);
	double getRadius(void) const;
	Point2D getCenter(void) const;


	static Circle* createInscribedCircle(const Point2D &a,const Point2D &b,const Point2D &c);

	static double getInscribedRadius(const Point2D &a,const Point2D &b,const Point2D &c);

	static Point2D getInscribedCenter(const Point2D &a,const Point2D &b,const Point2D &c);

	static Circle* createCircumscribedCircle(const Point2D &a,const Point2D &b,const Point2D &c);

	static double getCircumscribedRadius(const Point2D &a,const Point2D &b,const Point2D &c);

	static Point2D getCircumscribedCenter(const Point2D &a,const Point2D &b,const Point2D &c);
};

class Circle3D
{
public:
	Circle3D(void);
	~Circle3D(void);
private:
	Point3D center;
	Vector3D normal;
	double radius;
public:

	void setCenter(const Point3D& pt);
	void setCenter(const double x,const double y,const double z);
	void setNormal(const Vector3D& pt);
	void setNormal(const double x,const double y,const double z);
	void setRadius(const double r);
	Point3D getCenter(void) const;
	Vector3D getNormal(void) const;
	double getRadius(void) const;


	static Circle3D* createInscribedCircle(const Point3D &a,const Point3D &b,const Point3D &c);
	static Point3D getInscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c);
	static double getInscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c);

	static Circle3D* createCircumscribedCircle(const Point3D &a,const Point3D &b,const Point3D &c);
	static Point3D getCircumscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c);
	static double getCircumscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c);
};

}
