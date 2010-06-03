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
#pragma once

#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_Geometry2D.h"

namespace kmb{

/**
 * ïΩñ è„ÇÃíºê¸
 * ax + by +c = 0
 * a*a + b*b = 1 Ç∆Ç»ÇÈÇÊÇ§Ç…ê≥ãKâªÇµÇƒÇ®Ç≠
 */

class Line2D
{
private:
	Line2D(double a,double b,double c);
public:
	virtual ~Line2D(void);
	static Line2D* createFromPoints(kmb::Point2D &p,kmb::Point2D &q);
	static Line2D* createFromBaseDirection(kmb::Point2D &base,kmb::Vector2D &direction);
	double evaluate(kmb::Point2D &pt);
	double evaluate(double x,double y);
	double distance(kmb::Point2D &pt);
	double getXIntercept(void);
	double getYIntercept(void);

	static double getXIntercept(kmb::Point2D &p,kmb::Point2D &q);
	static double getYIntercept(kmb::Point2D &p,kmb::Point2D &q);
private:
	double a;
	double b;
	double c;
};

/**
 * ãÛä‘ì‡ÇÃíºê¸
 * ï˚å¸ÉxÉNÉgÉãÇÕê≥ãKâªÇµÇƒÇ®Ç≠
 */

class Line3D
{
private:
	Line3D(kmb::Point3D &b,kmb::Vector3D &v);
public:
	virtual ~Line3D(void);
	static Line3D* createFromPoints(kmb::Point3D &p,kmb::Point3D &q);
	static Line3D* createFromBaseDirection(kmb::Point3D &base,kmb::Vector3D &direction);
	kmb::Vector3D	getDirection(void);
private:
	kmb::Point3D	base;
	kmb::Vector3D	direction;
};

}
