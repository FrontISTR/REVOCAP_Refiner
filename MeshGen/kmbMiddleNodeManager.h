/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MiddleNodeManager                                       #
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
//
// １次要素を２次要素にする場合など、辺の両端の点を与えて、その中間点を返す
//
// 四角形の四点の順番が違っても中心の点を正しく返すようなコンテナの設計
//
// NodePair には対角線のうち、最小の節点番号からの最長対角線を登録する
// 対角線が存在しないもの（三角形など）は最小の節点番号の対辺の中間節点のペアを登録する
//
// middlePoints の map に Face をいれると、中間節点に対して Face は
// 一意ではないのでよくないのでこのような実装とした
//
#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include <map>

namespace kmb{

class Point3DContainer;
class ElementBase;
class MeshSmoother;

class MiddleNodeManager
{
protected:
	struct originalPair{
		kmb::nodeIdType n0;
		kmb::nodeIdType n1;
		// 要素の中心・面から生成したとき。それ以外は kmb::Element::nullElementId
		kmb::elementIdType elementId;
	};
	// 第一成分の方が小さいとする
	typedef std::pair< kmb::nodeIdType, kmb::nodeIdType > NodePair;
	Point3DContainer* points;
	// 親節点の組 => 中間節点
	std::map< NodePair, kmb::nodeIdType > middlePoints;
	// 中間節点 => 親節点の情報
	std::map< kmb::nodeIdType, kmb::MiddleNodeManager::originalPair > originalPoints;
protected:
	// 内部の参照テーブルに追加する
	void appendMiddleNode( kmb::nodeIdType middle, kmb::nodeIdType org0, kmb::nodeIdType org1, kmb::elementIdType elemId=kmb::Element::nullElementId);
	// 等分した点を生成してその nodeId を返す
	virtual kmb::nodeIdType createMiddleNode(kmb::nodeIdType n0, kmb::nodeIdType n1);
	virtual kmb::nodeIdType createMiddleNode3(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2);
	virtual kmb::nodeIdType createMiddleNode4(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3);
	virtual kmb::nodeIdType createMiddleNode6
		(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2,
		 kmb::nodeIdType n3, kmb::nodeIdType n4,kmb::nodeIdType n5);
	// 0.125 * (n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7)
	virtual kmb::nodeIdType createMiddleNode8
		(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3,
		 kmb::nodeIdType n4, kmb::nodeIdType n5,kmb::nodeIdType n6, kmb::nodeIdType n7);
	// 0.375 * n0 + 0.75 * n1 - 0.125 * n2
	virtual kmb::nodeIdType createMiddleNode3s(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2);
	// 0.5 * n0 + 0.5 * n1 - 0.125 * n2 - 0.125 * n3 + 0.25 * n4
	virtual kmb::nodeIdType createMiddleNode5(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3, kmb::nodeIdType n4);
	// -0.25 * (n0 + n1 + n2 + n3) + 0.5 * (n4 + n5 + n6 + n7)
	virtual kmb::nodeIdType createMiddleNode8s
		(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3,
		 kmb::nodeIdType n4, kmb::nodeIdType n5,kmb::nodeIdType n6, kmb::nodeIdType n7);
public:
	MiddleNodeManager(void);
	virtual ~MiddleNodeManager(void);
	void clear(void);
	// コンテナの設定
	void setNodeContainer(Point3DContainer* points);
	size_t getMiddleNodeCount(void) const;
	// a と b の中間節点の nodeId を返す
	// 登録されていなかったら点を生成し、nodeContainer に追加してから返す
	// a と b からなる要素(SEGMENT)の場合は elemId を与える
	kmb::nodeIdType getDividedNode(kmb::nodeIdType a,kmb::nodeIdType b,kmb::elementIdType elemId=kmb::Element::nullElementId);
	// 要素の中間節点を与えるのは
	// SEGMENT QUAD HEXAHEDRON のとき
	// TRIANGLE TETRAHEDRON WEDGE のとき：middlePoints には対辺の中間節点を与える
	kmb::nodeIdType getCenterNode(const kmb::ElementBase &elem,kmb::elementIdType elementId);
	kmb::nodeIdType getCenterNode(const kmb::ElementBase &elem,int faceIndex,kmb::elementIdType elementId);
	// 直接節点番号で与える場合
	kmb::nodeIdType getCenterNode4(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3,kmb::elementIdType elementId);
	kmb::nodeIdType getCenterNode8(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3,kmb::nodeIdType n4,kmb::nodeIdType n5,kmb::nodeIdType n6,kmb::nodeIdType n7,kmb::elementIdType elementId);
	// 2次要素の時に形状関数を使って中間節点を与える
	kmb::nodeIdType getCenterNode2(const kmb::ElementBase &elem,kmb::elementIdType elementId);
	kmb::nodeIdType getCenterNode2(const kmb::ElementBase &elem,int faceIndex,kmb::elementIdType elementId);
	// a と b の中間節点の nodeId を返す
	// 登録されていなかったら nullNodeId を返す
	kmb::nodeIdType isDivided(kmb::nodeIdType a,kmb::nodeIdType b) const;
	// a と b の中点が middle であることを覚えておく
	void setDividedNode(kmb::nodeIdType middle, kmb::nodeIdType a,kmb::nodeIdType b);
	// middle point から元の点を検索する
	kmb::elementIdType getOriginalNode(kmb::nodeIdType centerId,kmb::nodeIdType &a,kmb::nodeIdType &b) const;

	// a と b の中間節点の nodeId を返す
	// a, b, c の3点をこの順番で通る2次曲線上に中間節点を取る
	// 登録されていなかったら点を生成し、nodeContainer に追加してから返す
	// a と b からなる要素(SEGMENT)の場合は elemId を与える
	kmb::nodeIdType getDividedNode3(kmb::nodeIdType a,kmb::nodeIdType b,kmb::nodeIdType c,kmb::elementIdType elemId=kmb::Element::nullElementId);

	// n0 と n1 の中間節点の nodeId を返す
	// 三角形2次の 4,5,2,1,3 の順
	kmb::nodeIdType getDividedNode5(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3,kmb::nodeIdType n4,kmb::elementIdType elemId=kmb::Element::nullElementId);

	// 四角錐２次の内部の中間節点を形状関数から作るための特別メソッド
	// index = 5, 6, 7, 8 と底の中心 c の間の中点を生成する
	kmb::nodeIdType getDividedNodePyrmid2c(int index,kmb::nodeIdType center,const kmb::ElementBase &elem,kmb::elementIdType elemId=kmb::Element::nullElementId);

	// 中間節点の位置を調整する
	// MeshSmoother を与えて、節点を動かす
	// 動かす対象は中間節点でのみ
	// 戻り値は動かした節点の個数
	// smoother にあらかじめ mesh と body を登録しておくこと
	virtual int smoothingMiddleNodes(kmb::MeshSmoother* smoother);
};

}
