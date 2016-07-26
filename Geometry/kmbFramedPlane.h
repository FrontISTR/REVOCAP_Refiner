/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FramedPlane                                             #
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
#include "Geometry/kmbPlane.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbGeometry2D.h"

// 平面上に (u,v) 座標を定義したもの
// 原点は (0,0,0) の射影とする

namespace kmb{

class FramedPlane : public Plane
{
public:
	FramedPlane(const double a0,const double b0,const double c0,const double d0);
	FramedPlane(const Point3D &o,const Vector3D &v0,const Vector3D &v1);
	virtual ~FramedPlane(void);
	// origin を原点とするような UV 座標を返す
	Point2D transformTo2D(const Point3D &point) const;
	Point3D transformTo3D(const Point2D &point) const;
	Point3D transformTo3D(double u,double v) const;
	// UV 座標の基底ベクトルを回転する
	void rotateUVBase(const double angle);
	// 原点の移動
	void setOrigin(const Point3D& point);
	void setOrigin(double x,double y,double z);
	void getOrigin(Point3D& point) const;
	// origin = origin + s * normal
	void slideOrigin(double s);
	// 法線の変更
	bool setNormal(double a,double b,double c);
	// ラジアン
	void setNormalPolar(double phi,double theta);
	// -PI <= phi <= PI
	// 0 <= theta <= PI
	void getNormalPolar(double& phi,double& theta) const;
protected:
	void initUVBase(void);
private:
	// normal, n0, n1 がこの順番で正規直交基底
	Point3D origin;
	Vector3D n0;
	Vector3D n1;
};

}

