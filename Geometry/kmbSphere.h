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
	/// 設定系
	void setCenter(Point3D &pt);
	void setCenter(double x,double y,double z);
	void setRadius(double r);
	double getRadius(void) const;
	Point3D getCenter(void) const;
	/// 判定関数
	virtual kmb::Region::locationType intersect(const kmb::Point3D &point) const;
	virtual double distanceSq(const kmb::Point3D &point) const;
	// 中心からの距離 - 半径
	virtual double distance(const kmb::Point3D &point) const;
	virtual bool intersect(const kmb::BoxRegion& box) const;
	// 球面の方程式での評価
	// 中心からの距離の２乗 - 半径の２乗
	// 球の内部ならばこの値は 0 以下
	double eval(const kmb::Point3D &point) const;
	// 生成関数群
	// 内接球
	static Sphere* createInscribedSphere(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);
	// 半径
	static double getInscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);
	// 中心
	static Point3D getInscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);
	// 外接球
	static Sphere* createCircumscribedSphere(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);
	// 半径
	static double getCircumscribedRadius(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);
	// 中心
	static Point3D getCircumscribedCenter(const Point3D &a,const Point3D &b,const Point3D &c,const Point3D &d);
};

}
