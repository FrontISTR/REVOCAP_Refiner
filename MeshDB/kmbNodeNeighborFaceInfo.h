/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeNeighborFaceInfo                                    #
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
// 設計メモ
//
// 節点と面（要素とその局所面番号）の間の対応を記録する
// それによって得られる以下の情報を取得できるようにする
// * 節点の周りの面の個数
// * 面に隣接している他の面
//
#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbFace.h"
#include <map>
#include <vector>
#include <string>

namespace kmb{

class Element;
class DataBindings;
class ElementContainer;

typedef std::multimap< kmb::nodeIdType, kmb::Face > NodeNeighborFace;

class NodeNeighborFaceInfo
{
public:
	NodeNeighborFaceInfo(void);
	virtual ~NodeNeighborFaceInfo(void);
	void clear(void);

	// 逆対応の登録と削除
private:
	bool append( kmb::nodeIdType nodeID, kmb::Face faceID );
	bool erase( kmb::nodeIdType nodeID, kmb::Face faceID );
public:
	bool appendCoboundary( kmb::Face face, const kmb::ElementContainer* body );
	bool appendCoboundary( kmb::Face face, const kmb::MeshData* mesh );
	bool appendCoboundary( const kmb::DataBindings* data, const kmb::ElementContainer* body );
	bool appendCoboundary( const kmb::DataBindings* data, const kmb::MeshData* mesh );
	// 逆対応の削除
	bool deleteCoboundary( kmb::Face face, const kmb::ElementContainer* body );

	// 近傍情報、周辺情報の取得
	
	// Face を与えてその近傍 Face を取り出し、neighbors に返す
	// neighbors は呼び出し側で境界の個数だけあらかじめ用意して
	// 呼び出し側でメモリの確保・解放を行うこと
	// 戻り値は近傍の個数
	// 注意：neighbors に入っている近傍の順番は意味はない（途中に NULL は出てこない）
	int getFaceNeighbor( kmb::Face face, kmb::Face *neighbors, const kmb::ElementContainer* elements ) const;

	// Face を与えてそれを１次元低い要素と考えた時の
	// 境界番号 index 側に接している Face を返す
	// 戻り値が false のときには neighborFace は変更していないことを保証する
	bool getFaceNeighborByIndex( kmb::Face face, int index, const kmb::ElementContainer* elements, kmb::Face &neighborFace ) const;

	// 節点を与えてその周辺要素の個数
	size_t getFaceCountAroundNode(nodeIdType nodeID) const;
	// すべての近傍情報の個数
	size_t getSize(void) const;

	// 節点を与えてその近傍要素の Iterator を与える
	NodeNeighborFace::iterator beginIteratorAt(kmb::nodeIdType nodeID){
		return coboundaries.lower_bound(nodeID);
	};
	NodeNeighborFace::iterator endIteratorAt(kmb::nodeIdType nodeID){
		return coboundaries.upper_bound(nodeID);
	};

private:
	NodeNeighborFace	coboundaries;
};

}
