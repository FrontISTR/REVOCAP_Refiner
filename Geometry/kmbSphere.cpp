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
#include <cmath>
#include <cfloat>

#include "Geometry/kmbSphere.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbGeometry2D.h"

kmb::Sphere::Sphere(void)
{
}

kmb::Sphere::~Sphere(void)
{
}

void kmb::Sphere::
setCenter(kmb::Point3D &pt)
{
	this->setCenter(pt.x(),pt.y(),pt.z());
}

void kmb::Sphere::
setCenter(double x,double y,double z)
{
	this->center.setCoordinate(x,y,z);
}

void kmb::Sphere::
setRadius(double r)
{
	this->radius = r;
}

double kmb::Sphere::
getRadius(void) const
{
	return this->radius;
}

kmb::Point3D kmb::Sphere::
getCenter(void) const
{
	return kmb::Point3D(this->center);
}

kmb::Region::locationType kmb::Sphere::
intersect(const kmb::Point3D &point) const
{
	double d = this->center.distanceSq( point );
	double sq = this->radius * this->radius;
	if( d < sq )
		return kmb::Region::INSIDE;
	else if( d == sq )
		return kmb::Region::ON_BOUNDARY;
	else
		return kmb::Region::OUTSIDE;
}



bool
kmb::Sphere::intersect(const kmb::BoxRegion& box) const
{
	kmb::Point3D center = this->getCenter();
	double r = this->getRadius();
	double boxMaxX = box.maxX();
	double boxMaxY = box.maxY();
	double boxMaxZ = box.maxZ();
	double boxMinX = box.minX();
	double boxMinY = box.minY();
	double boxMinZ = box.minZ();


	if( boxMaxX < center.x() - r || center.x() + r < boxMinX ||
		boxMaxY < center.y() - r || center.y() + r < boxMinY ||
		boxMaxZ < center.z() - r || center.z() + r < boxMinZ )
	{
		return false;
	}

	double rsq = r*r;
	return
		( center.distanceSq(boxMaxX,boxMaxY,boxMaxZ) <= rsq ) ||
		( center.distanceSq(boxMaxX,boxMaxY,boxMinZ) <= rsq ) ||
		( center.distanceSq(boxMaxX,boxMinY,boxMaxZ) <= rsq ) ||
		( center.distanceSq(boxMaxX,boxMinY,boxMinZ) <= rsq ) ||
		( center.distanceSq(boxMinX,boxMaxY,boxMaxZ) <= rsq ) ||
		( center.distanceSq(boxMinX,boxMaxY,boxMinZ) <= rsq ) ||
		( center.distanceSq(boxMinX,boxMinY,boxMaxZ) <= rsq ) ||
		( center.distanceSq(boxMinX,boxMinY,boxMinZ) <= rsq ) ;
}

double
kmb::Sphere::distanceSq(const kmb::Point3D& point) const
{
	double rdiff = point.distance( this->center ) - this->radius;
	return rdiff*rdiff;
}

double
kmb::Sphere::distance(const kmb::Point3D& point) const
{
	return point.distance( this->center ) - this->radius;
}

double
kmb::Sphere::eval(const kmb::Point3D &point) const
{
	return point.distanceSq( this->center ) - (this->radius * this->radius);
}




kmb::Sphere*
kmb::Sphere::createInscribedSphere(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Point3D &d)
{
	kmb::Sphere* sphere = NULL;
	sphere = new kmb::Sphere();
	if( sphere ){

		double volume = fabs(kmb::Point3D::volume(a,b,c,d));

		double abc = kmb::Point3D::area(a,b,c);
		double bcd = kmb::Point3D::area(b,c,d);
		double cda = kmb::Point3D::area(c,d,a);
		double dab = kmb::Point3D::area(d,a,b);

		double r = (volume*3.0)/(abc+bcd+cda+dab);
		double x = (abc*d.x() + bcd*a.x() + cda*b.x() + dab*c.x()) / (abc+bcd+cda+dab);
		double y = (abc*d.y() + bcd*a.y() + cda*b.y() + dab*c.y()) / (abc+bcd+cda+dab);
		double z = (abc*d.z() + bcd*a.z() + cda*b.z() + dab*c.z()) / (abc+bcd+cda+dab);
		sphere->setRadius(r);
		sphere->setCenter(x,y,z);
	}
	return sphere;
}

double
kmb::Sphere::getInscribedRadius(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Point3D &d)
{

	double volume = fabs(kmb::Point3D::volume(a,b,c,d));

	double abc = kmb::Point3D::area(a,b,c);
	double bcd = kmb::Point3D::area(b,c,d);
	double cda = kmb::Point3D::area(c,d,a);
	double dab = kmb::Point3D::area(d,a,b);

	return (volume*3.0)/(abc+bcd+cda+dab);
}

kmb::Point3D
kmb::Sphere::getInscribedCenter(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Point3D &d)
{
	double abc = kmb::Point3D::area(a,b,c);
	double bcd = kmb::Point3D::area(b,c,d);
	double cda = kmb::Point3D::area(c,d,a);
	double dab = kmb::Point3D::area(d,a,b);
	double x = (abc*d.x() + bcd*a.x() + cda*b.x() + dab*c.x()) / (abc+bcd+cda+dab);
	double y = (abc*d.y() + bcd*a.y() + cda*b.y() + dab*c.y()) / (abc+bcd+cda+dab);
	double z = (abc*d.z() + bcd*a.z() + cda*b.z() + dab*c.z()) / (abc+bcd+cda+dab);
	return kmb::Point3D(x,y,z);
}


kmb::Sphere*
kmb::Sphere::createCircumscribedSphere(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Point3D &d)
{
	kmb::Sphere* sphere = NULL;
	sphere = new kmb::Sphere();
	if( sphere ){
		kmb::Point3D center = getCircumscribedCenter(a,b,c,d);
		double r = center.distance(a);
		sphere->setRadius(r);
		sphere->setCenter(center);
	}
	return sphere;
}

double
kmb::Sphere::getCircumscribedRadius(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Point3D &d)
{
	kmb::Vector3D ab(b,a);
	kmb::Vector3D ac(c,a);
	kmb::Vector3D ad(d,a);
	double det = kmb::Vector3D::triple(ab,ac,ad);
	if( det != 0.0 ){
		kmb::Vector3D v = (
			(ab%ac).scalar(ad.lengthSq()) +
			(ac%ad).scalar(ab.lengthSq()) +
			(ad%ab).scalar(ac.lengthSq())).scalar(0.5/det);
		return v.length();
	}else{
		return DBL_MAX;
	}
}

kmb::Point3D
kmb::Sphere::getCircumscribedCenter(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Point3D &d)
{
	kmb::Vector3D ab(b,a);
	kmb::Vector3D ac(c,a);
	kmb::Vector3D ad(d,a);
	double det = kmb::Vector3D::triple(ab,ac,ad);
	if( det != 0.0 ){
		kmb::Vector3D v = (
			(ab%ac).scalar(ad.lengthSq()) +
			(ac%ad).scalar(ab.lengthSq()) +
			(ad%ab).scalar(ac.lengthSq())).scalar(0.5/det);
		return kmb::Point3D(a+v);
	}else{
		return kmb::Point3D::infinity;
	}
}
