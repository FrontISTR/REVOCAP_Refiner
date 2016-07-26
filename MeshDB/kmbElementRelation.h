/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementRelation                                         #
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

#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElement.h"

#include <map>

namespace kmb{

class Face;

class ElementRelation
{
public:
	// 要素間の関係
	enum relationType{
		UNKNOWNRELATION = -1,// 論理的にありえない関係
		// 同じ次元の要素についての関係
		EQUAL,			// 向きも含めて等しい
		REVERSE,		// 逆向きで等しい
		ADJACENT,		// 接する
		ANTIADJACENT,	// 逆向きで接する
		// 相手が自分よりも次元が１高い場合
		BOUNDARY,		// 境界になっている
		ANTIBOUNDARY,	// 逆向きの境界になっている
		// 相手が自分よりも次元が１低い場合
		COBOUNDARY,		// 境界として含む
		ANTICOBOUNDARY, // 逆向きの境界として含む
		// 上記のいずれでもないが、共有しているものがあるとき
		COMMONEDGE,		// 共有辺あり
		COMMONNODE,		// 共有頂点あり
		NOCOMMON,		// 共有なし
		OTHERRELATION   // その他の関係
	};
	// 要素間の関係を文字列に変換
	static std::string	getRelationTypeString(kmb::ElementRelation::relationType rel);
	
	// 要素間の関係を求める
	// 接している時はどの面で接しているかを index0 index1 の値で返す
	static kmb::ElementRelation::relationType
		getRelation(const kmb::ElementBase &eIter0, int &index0,
					const kmb::ElementBase &eIter1, int &index1 );

	// 線分が一致しているかどうか
	static kmb::ElementRelation::relationType
		getSegmentRelation
			(int a0,int a1,int b0,int b1,int &aIndex,int &bIndex);
	// 三角形が一致しているかどうか
	static kmb::ElementRelation::relationType
		getTriangleRelation
			(int a0,int a1,int a2,int b0,int b1,int b2,int &aIndex,int &bIndex);
	// 四角形が一致しているかどうか
	static kmb::ElementRelation::relationType
		getQuadRelation
			(int a0,int a1,int a2,int a3,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);
	// 三角形と四角形の間の関係
	static kmb::ElementRelation::relationType
		getTriangleQuadRelation
			(int a0,int a1,int a2,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);
	// 四面体が一致しているかどうか
	static kmb::ElementRelation::relationType
		getTetrahedronRelation
			(int a0,int a1,int a2,int a3,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);
	// 三角形と四面体の関係
	// １点共有の時 => aIndex bIndex は共有する節点の index
	// ２点共有の時 => aIndex は Edge の index 、bIndex は Face の index
	static kmb::ElementRelation::relationType
		getTriangleTetrahedronRelation
			(int a0,int a1,int a2,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);

	// Relation すべてを求めずに接しているかどうかのみを判定する簡易版
	static bool isAdjacent(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );

	// 複製
	static bool clone( const kmb::ElementBase &eIter, kmb::nodeIdType *clonedCell );
	static bool clone( const kmb::ElementBase &eIter, kmb::nodeIdType *clonedCell, std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper );
	// 向きを逆にする
	static bool reverse( const kmb::ElementBase &eIter, kmb::nodeIdType *reversedCell );
	static bool reverse( const kmb::ElementBase &eIter, kmb::nodeIdType *reversedCell, std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper );
	// 共通要素
	static kmb::elementType common( const kmb::ElementBase &eIter0, const kmb::ElementBase &eIter1, kmb::nodeIdType *commonCell );
	static kmb::elementType common( kmb::Face f0, kmb::Face f1, const kmb::ElementContainer* elements, kmb::nodeIdType *commonCell );

protected:
	// 内部下請け
	// 第１引数の型で分類
	static kmb::ElementRelation::relationType getRelationForSegment
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );
	// common edge の時 index0 = triangle の face id
	// common node の時 index0 = triangle の節点の index
	static kmb::ElementRelation::relationType getRelationForTriangle
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );
	static kmb::ElementRelation::relationType getRelationForQuad
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );
	static kmb::ElementRelation::relationType getRelationForTetrahedron
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );
};

}
