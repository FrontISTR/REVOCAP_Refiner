/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Collision                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbFace.h"

namespace kmb{

class Point3D;
class Point3DContainer;
class ElementBase;
class MeshData;

class Collision
{
protected:
	const Point3DContainer* points;
	MeshData* mesh;
	double accuracy;
public:
	Collision(kmb::MeshData* mesh);
	virtual ~Collision(void);
	void setAccuracy(double acc);
	double getAccuracy(void) const;
	// 点と三角形・四角形の距離
	double distanceSqPtTri(kmb::nodeIdType nodeId,kmb::ElementBase& tri,double s[2]) const;
	double distanceSqPtQuad(kmb::Point3D& pt,kmb::ElementBase& quad) const;
	// 線分同士の距離
	double distanceSqSegSeg(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::nodeIdType b0,kmb::nodeIdType b1,double &t1, double &t2) const;
	double distanceSqSegSeg(kmb::ElementBase& seg0,kmb::ElementBase& seg1,double &t1, double &t2) const;
	double distanceSegSeg(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::nodeIdType b0,kmb::nodeIdType b1) const;
	double distanceSegSeg(kmb::ElementBase& seg0,kmb::ElementBase& seg1) const;
	// 線分と三角形の距離
	double distanceSqSegTri(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::ElementBase& tri,double &s,double t[2]) const;
	double distanceSegTri(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::ElementBase& tri) const;
	// 三角形同士の距離
	double distanceSqTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1,double s[2],double t[2]) const;
	double distanceTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1) const;
	// 四角形同士の距離
	// 2つの三角形に分けて計算する
	double distanceSqQuadQuad(kmb::ElementBase& q0,kmb::ElementBase& q1) const;
	double distanceQuadQuad(kmb::ElementBase& q0,kmb::ElementBase& q1) const;
	// 三角形と四角形の距離
	double distanceSqTriQuad(kmb::ElementBase& tri,kmb::ElementBase& q) const;
	double distanceTriQuad(kmb::ElementBase& tri,kmb::ElementBase& q) const;
	// 型がわからない要素の間の距離
	double distanceSq(kmb::ElementBase& elem0,kmb::ElementBase& elem1) const;
	double distance(kmb::ElementBase& elem0,kmb::ElementBase& elem1) const;
	double distanceSq(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements) const;
	double distance(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements) const;

	enum collisionType{
		COLLISION_ERROR = -2,
		NOT_COLLIDE = -1,
		TOPOLOGICAL_CONTACT = 0, // 共有している点があるだけ
		GEOMETRICAL_CONTACT = 1,
		DETECT = 2,
		kCollisionError = -2,
		kNotCollision = -1,
		kTopologicalContact = 0,
		kGeometricalContact = 1,
		kCollision = 2
	};
	static const char* collisionTypeString( collisionType ctype );
	// 三角形と三角形の干渉関係
	collisionType detectTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1) const;
	collisionType detectTriTetra(kmb::ElementBase& tri,kmb::ElementBase& tetra) const;

	// 三角形 t0 と t1 を与えて、節点の順番で
	// 3 頂点間の距離の和が最小となる向き(t0について)を求める
	// element type はチェック済みとする
	double distanceByNodeTri(kmb::ElementBase& t0,kmb::ElementBase& t1,int &index) const;
	double distanceByNodeTri(kmb::ElementBase& t0,kmb::Face &f,const kmb::ElementContainer* elements,int &index) const;
	// 四角形 q0 と q1 を与えて、節点の順番で
	// 4 頂点間の距離の和が最小となる向き(q0について)を求める
	// element type はチェック済みとする
	double distanceByNodeQuad(kmb::ElementBase& q0,kmb::ElementBase& q1,int &index) const;
	double distanceByNodeQuad(kmb::ElementBase& q0,kmb::Face &f,const kmb::ElementContainer* elements,int &index) const;
	// 型がわからない要素の頂点間の距離の和
	double distanceByNode(kmb::ElementBase& elem0,kmb::ElementBase& elem1,int &index) const;
	double distanceByNode(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements,int &index) const;

	// 面グループの間の干渉関係を調べる
	// fg0 に対して fg1 の面のうち、距離が thres より小さいものを result に追加する
	// 追加した個数を返す
	int detectFaceGroup(const char* fg0,const char* fg1,double thres,const char* result) const;
	// 表面要素と面グループの間の干渉関係を調べる
	// bodyId0 に対して fg1 の面のうち、距離が thres より小さいものを result に追加する
	// 追加した個数を返す
	int detectSurfaceFaceGroup(kmb::bodyIdType bodyId0,const char* fg1,double thres,const char* result) const;
	// 表面要素と面グループの間の干渉関係を調べる
	// bodyId0 に対して fg1 の面のうち、距離が thres より小さい頂点が tCount 以上あるものを result に追加する
	// 追加した個数を返す
	int detectSurfaceFaceGroupByNode(kmb::bodyIdType bodyId0,const char* fg1,int tCount,double thres,const char* result) const;

	// quad or triangle  に対して、４頂点間または３頂点の距離の和が最小になる面を求める
	// 距離を計算する対象は同じ型のみ（quad vs quad または tri vs tri）
	// 戻り値は、距離、面、もとの quad or tri をどの向きにしたときに最小になるかの回転度
	double getNearestFace(kmb::ElementBase& q0,const char* fg,kmb::Face &nearestFace,int &index) const;

	// static 関数群
	// 距離の2乗を返す
	static double testSegSeg(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,double &t1, double &t2);
	static double testSegTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,double &s,double t[2]);
	static double testTriTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,double s[2],double t[2]);

	// topological なチェックはしない
	// 距離関数でチェック
	static collisionType detectSegTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2);
	static collisionType detectTriTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2);
	// 体積座標の符号をチェック
	static collisionType detectSegTri2(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2);
	static collisionType detectTriTri2(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2);
};

}
