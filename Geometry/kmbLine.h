/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Line                                                    #
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
 * 平面上の直線
 * ax + by +c = 0
 * a*a + b*b = 1 となるように正規化しておく
 */

class Line2D
{
private:
	double a;
	double b;
	double c;
private:
	// 必ず正規化するために、係数を与えて生成するのは private にする
	Line2D(double a,double b,double c);
public:
	virtual ~Line2D(void);
	static Line2D* createFromPoints(const kmb::Point2D &p,const kmb::Point2D &q);
	static Line2D* createFromBaseDirection(const kmb::Point2D &base,const kmb::Vector2D &direction);
	double evaluate(const kmb::Point2D &pt) const;
	double evaluate(double x,double y) const;
	kmb::Vector2D getDirection(void) const;
	// 原点から最も近い点を起点とする (-ca,-cb) 距離は c
	kmb::Point2D getBase(void) const;
	// 最近点をパラメータ表示で返す
	double nearest(const kmb::Point2D &pt) const;
	double distance(const kmb::Point2D &pt) const;
	double getXIntercept(void) const;
	double getYIntercept(void) const;
	// 2点を通る直線の x 切片と y 切片を求める
	static double getXIntercept(const kmb::Point2D &p,const kmb::Point2D &q);
	static double getYIntercept(const kmb::Point2D &p,const kmb::Point2D &q);
};

/**
 * 空間内の直線
 * 方向ベクトルは正規化しておく
 */

class Line3D
{
private:
	kmb::Point3D base;
	kmb::Vector3D direction;
private:
	Line3D(const kmb::Point3D &b,const kmb::Vector3D &v);
public:
	virtual ~Line3D(void);
	static Line3D* createFromPoints(const kmb::Point3D &p,const kmb::Point3D &q);
	static Line3D* createFromBaseDirection(const kmb::Point3D &base,const kmb::Vector3D &direction);
	kmb::Vector3D getDirection(void) const;
	kmb::Point3D getBase(void) const;
	// 最近点をパラメータ表示で返す
	double nearest(const kmb::Point3D &pt) const;
	double distance(const kmb::Point3D &pt) const;
	double distanceSq(const kmb::Point3D &pt) const;
};

}
