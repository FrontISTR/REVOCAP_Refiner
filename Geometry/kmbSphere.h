/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Sphere                                                  #
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
#include "Geometry/kmbRegion.h"
#include "Geometry/kmbBoxRegion.h"

namespace kmb{

class Sphere : public Region
{
private:
	Point3D center;
	double radius;
public:
	Sphere(void);
	virtual ~Sphere(void);
public:

	void setCenter(Point3D &pt);
	void setCenter(double x,double y,double z);
	void setRadius(double r);
	double getRadius(void) const;
	Point3D getCenter(void) const;

	virtual kmb::Region::locationType intersect(const kmb::Point3D &point) const;
	virtual double distanceSq(const kmb::Point3D &point) const;

	virtual double distance(const kmb::Point3D &point) const;
	virtual bool intersect(const kmb::BoxRegion& box) const;



	double eval(const kmb::Point3D &point) const;


	static Sphere* createInscribedSphere(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);

	static double getInscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);

	static Point3D getInscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);

	static Sphere* createCircumscribedSphere(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);

	static double getCircumscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);

	static Point3D getCircumscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);
};

}
