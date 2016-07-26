/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PolygonPartitioner                                      #
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

#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbFramedPlane.h"

#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbPolygon.h"

#include <set>

namespace kmb{

class MeshDB;

class PolygonPartitioner
{
public:
	// y 座標 monotonisity を考えた時のタイプ
	enum vertexType{
		kUNKNOWN,
		kSTART,
		kSPLIT,
		kEND,
		kMERGE,
		kREGULAR
	};

	PolygonPartitioner(void);
	virtual ~PolygonPartitioner(void);
	void clear(void);
	void setPoints(const kmb::Point2DContainer* targetPoints);
	void setInitialPolygon(kmb::ElementContainer* edges);
	size_t getCount(void) const;

	// 三角形化して body に登録する
	bool partition( kmb::ElementContainer &body );

	// 2D 実行ルーチン
	kmb::bodyIdType partitionToTriangles(kmb::MeshDB* mesh,kmb::bodyIdType edgeId);
	// 3D 実行ルーチン
	kmb::bodyIdType partitionToTriangles(kmb::MeshDB* mesh,kmb::bodyIdType edgeId,const kmb::FramedPlane &plane);
private:
	// 点と y 座標が同じ点を含む要素のうち、x 座標が最も近いもの
	// left = true 左側（x座標が小さいほう）false 右側を探す
	// 戻り値は辺 [n,n+1] が最も近い辺の時の n 
	kmb::elementIdType getNearestSegmentWithSameLevel(kmb::nodeIdType nodeId, bool left=true) const;

	// helper node を求める
	// vType nodeID の vertex type
	kmb::nodeIdType getHelperNode( kmb::nodeIdType nodeID, kmb::elementIdType leftID, kmb::elementIdType rightID, vertexType vType) const;

	// Monotone 多角形を三角形化
	bool triangulateMonotonePolygon(kmb::Polygon* polygon, kmb::ElementContainer &body );

	const kmb::Point2DContainer* points;	// ポインタのコピーだけで、生成と破棄は別のクラスで行う
	kmb::Polygon* initialPolygon;

	vertexType	getVertexType(kmb::Polygon* polygon,kmb::nodeIdType nodeId) const;
	static vertexType	getVertexType(const kmb::Point2D& previousPoint,const kmb::Point2D& targetPoint,const kmb::Point2D& nextPoint);
};

}

