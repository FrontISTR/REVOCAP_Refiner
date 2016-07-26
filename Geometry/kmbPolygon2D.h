/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Polygon2D                                               #
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
//
// 平面上の多角形を扱うクラス
// 平面上の点の内部外部判定などの幾何処理を行う
//
#pragma once

#include <vector>
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbRegion.h"

namespace kmb{

class Polygon2D
{
	enum orientation{
		UNKNOWN = -1,
		CLOCKWISE,
		UNTICLOCKWISE
	};
protected:
	std::vector< kmb::nodeIdType > nodeArray;
	const kmb::Point2DContainer* points;
public:
	Polygon2D(void);
	virtual ~Polygon2D(void);
	void clear(void);
	void setPointContainer(const kmb::Point2DContainer* points);
	void appendNodeId( kmb::nodeIdType nodeId );
	int getSize(void) const;
	int getNodeIndex( kmb::nodeIdType nodeId ) const;
	// nodeId と offset を与えて、節点 Id を返す
	kmb::nodeIdType getNode( kmb::nodeIdType nodeId, int offset=0 ) const;
	kmb::nodeIdType getNodeId( int index ) const;

	Polygon2D::orientation getOrientation(void) const;
	kmb::Region::locationType intersect( kmb::Point2D &point ) const;
	
	// 節点ID n0 と n1 を結ぶ対角線で分割する
	void dividePolygon(kmb::nodeIdType n0,kmb::nodeIdType n1,Polygon2D* &p0,Polygon2D* &p1);

	// 内部に向きが逆の多角形を含むときに、外部の点 n0 から内部の点 n1 まで
	// 切れ目を入れて内部の多角形を解消する
	void branchPolygon(kmb::nodeIdType n0,kmb::nodeIdType n1,Polygon2D &negative,Polygon2D* &cancel);
};

}
