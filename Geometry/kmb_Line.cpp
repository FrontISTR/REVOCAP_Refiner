/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Line                                                    #
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
#include "Geometry/kmb_Line.h"
#include "Geometry/kmb_Geometry2D.h"
#include <cmath>
#include <cfloat>

kmb::Line2D::Line2D(double a,double b,double c)
{
	this->a = a;
	this->b = b;
	this->c = c;
}

kmb::Line2D::~Line2D(void)
{
}

double
kmb::Line2D::evaluate(kmb::Point2D &pt)
{
	return this->evaluate(pt.x(),pt.y());
}

double
kmb::Line2D::evaluate(double x,double y)
{
	return a*x+b*y+c;
}

double
kmb::Line2D::distance(kmb::Point2D &pt)
{
	return fabs(this->evaluate(pt));
}

kmb::Line2D*
kmb::Line2D::createFromPoints(kmb::Point2D &p,kmb::Point2D &q)
{
	kmb::Line2D* line = NULL;
	double a = p.y() - q.y();
	double b = - (p.x() - q.x());
	double c = p.x()*q.y() - p.y()*q.x();
	double r = sqrt(a*a + b*b);
	if( r != 0.0 ){
		line = new kmb::Line2D(a/r,b/r,c/r);
	}
	return line;
}

kmb::Line2D*
kmb::Line2D::createFromBaseDirection(kmb::Point2D &base,kmb::Vector2D &direction)
{
	kmb::Line2D* line = NULL;
	double a = direction.y();
	double b = - direction.x();
	double c = -(a*base.x() + b*base.y());
	double r = sqrt(a*a + b*b);
	if( r != 0.0 ){
		line = new kmb::Line2D(a/r,b/r,c/r);
	}
	return line;
}

double
kmb::Line2D::getXIntercept(void)
{
	if( a != 0.0 ){
		return -c/a;
	}else{
		return DBL_MAX;
	}
}

double
kmb::Line2D::getYIntercept(void)
{
	if( b != 0.0 ){
		return -c/b;
	}else{
		return DBL_MAX;
	}
}

double
kmb::Line2D::getXIntercept(kmb::Point2D &p,kmb::Point2D &q)
{
	double a = p.y() - q.y();
	double c = p.x()*q.y() - p.y()*q.x();
	if( a != 0.0 ){
		return -c/a;
	}else{
		return DBL_MAX;
	}
}

double
kmb::Line2D::getYIntercept(kmb::Point2D &p,kmb::Point2D &q)
{
	double b = - (p.x() - q.x());
	double c = p.x()*q.y() - p.y()*q.x();
	if( b != 0.0 ){
		return -c/b;
	}else{
		return DBL_MAX;
	}
}

kmb::Line3D::Line3D(kmb::Point3D &b,kmb::Vector3D &v)
{
	this->base = b;
	this->direction = v;
}

kmb::Line3D::~Line3D(void)
{
}

kmb::Line3D* kmb::Line3D::
createFromPoints(kmb::Point3D &p,kmb::Point3D &q)
{
	kmb::Line3D* line = NULL;
	kmb::Vector3D v(p,q);
	v.normalize();
	line = new kmb::Line3D(p,v);
	return line;
}

kmb::Line3D* kmb::Line3D::
createFromBaseDirection(kmb::Point3D &base,kmb::Vector3D &direction)
{
	kmb::Line3D* line = NULL;
	kmb::Vector3D v(direction);
	v.normalize();
	line = new kmb::Line3D(base,v);
	return line;
}

kmb::Vector3D kmb::Line3D::
getDirection(void)
{
	return this->direction;
}

