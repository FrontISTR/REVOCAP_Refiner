/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BoxRegion                                               #
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
#include "Geometry/kmbSphere.h"

namespace kmb{

/// 座標軸に平行な直方体
class BoxRegion : public Region
{
protected:
	// 干渉判定閾値
	static double thres;
protected:
	Point3D maxPoint;
	Point3D minPoint;
public:
	BoxRegion(void);
	BoxRegion(const Point3D &l,const Point3D &u);
	BoxRegion(double x0,double y0,double z0,double x1,double y1,double z1);
	BoxRegion(const BoxRegion &other);
	virtual ~BoxRegion(void);
	BoxRegion& operator=(const BoxRegion &other);

	void setMinMax(double x0,double y0,double z0,double x1,double y1,double z1);
	void setMinMax(kmb::Point3D& minPoint,kmb::Point3D& maxPoint);
	double minX(void) const;
	double minY(void) const;
	double minZ(void) const;
	double maxX(void) const;
	double maxY(void) const;
	double maxZ(void) const;
	void minX(double x);
	void minY(double y);
	void minZ(double z);
	void maxX(double x);
	void maxY(double y);
	void maxZ(double z);
	double centerX(void) const;
	double centerY(void) const;
	double centerZ(void) const;
	double rangeX(void) const;
	double rangeY(void) const;
	double rangeZ(void) const;
	// rangeX rangeY rangeZ の大きい方を返す
	double range(void) const;
	double diameter(void) const;
	double diameterSq(void) const;
	const Point3D& getMin(void) const;
	const Point3D& getMax(void) const;
	void getCenter(kmb::Point3D& center) const;
	double getVolume(void) const;

	virtual kmb::Region::locationType intersect(const kmb::Point3D &point) const;
	virtual kmb::Region::locationType intersect(const double x,const double y,const double z) const;
	virtual double distanceSq(const kmb::Point3D &point) const;
	virtual double distanceSq(const double x,const double y,const double z) const;
	// Box 同士の判定関数
	bool intersect(const kmb::BoxRegion& box) const;
	double intersectVolume(const BoxRegion& box) const;
	double distanceSq(const kmb::BoxRegion& box) const;
	// max min の両方を拡大する
	void expand(double x,double y,double z);
	void expand(double ratio);
	// 平行移動
	void translate(double x,double y,double z);
	// center を変えずに range を変える
	void setRange(double rangeX,double rangeY,double rangeZ);
	// diameter が ratio 倍になるように x y z を同じ長さだけ広げる
	void expandDiameter(double ratio);
	// 直線と重なっている部分
	// 重なっていなければ、mint_t = max_t = 0.0 を返す
	void crossOnLine(const kmb::Point3D& origin, const kmb::Vector3D& dir, double &min_t, double &max_t) const;
	// 三角形との判定
	// ab bc ca が6面と交差するかどうかの判定
	// 今の実装では intersectArea > 0.0 とした方が速い
	bool intersect(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c) const;
	// intersectArea から Area を除いたもの
	bool intersect3(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c) const;
	// 三角形との判定（面積つき）
	// 法線ベクトルがほしい場合もここで面積を計算してから法線を新たに計算するほうが速い
	// 理由：Vector3D 型を戻り値にするオーバーヘッドが大きい
	double intersectArea(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c) const;
private:
	// 内部下請け
	// この順に上から呼ばれる
	double intersectArea_minx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_maxx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_miny(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_maxy(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_minz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	double intersectArea_maxz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;

	bool intersect_minx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	bool intersect_maxx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	bool intersect_miny(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	bool intersect_maxy(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	bool intersect_minz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
	bool intersect_maxz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const;
};

}
