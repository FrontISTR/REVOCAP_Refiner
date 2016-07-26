/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matching                                                #
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
/*
 * 目的
 * Surface と FaceGroup の順番と向きまで込めたマッチング
 * Surface で足りないところはダミーの面を追加することも可能
 * 入力
 * Surface の bodyId
 * FaceGroup
 * 出力
 * FaceGroup の face と Surface の elementId の間の対応
 */

#pragma once

#include <string>
#include <map>
#include <set>
#include "MeshDB/kmbTypes.h"

namespace kmb{

class MeshData;
class ElementContainer;
class DataBindings;
class NodeNeighborFaceInfo;
class Face;
class Collision;
class Matrix;
class Permutation;

class SurfaceMatching
{
private:
	kmb::MeshData* mesh;
	kmb::bodyIdType masterId;
	kmb::ElementContainer* masterSurf;
	std::string slaveName;
	kmb::DataBindings* slaveFaceGroup;
	kmb::ElementContainer* slaveElements;
	// FaceGroup の近傍情報
	kmb::NodeNeighborFaceInfo* neighborInfo;
	kmb::Matrix* distanceMatrix;
	kmb::elementIdType* elementIds;
	kmb::Face* faces;
	kmb::Collision* collision;
	kmb::DataBindings* insertedNodes;
	struct rotatedElement{
		kmb::elementIdType elementId;
		int index;
	};
	std::map< kmb::Face, rotatedElement > mapping;
	// 配列 elementIds の index による接続関係
	std::set< std::pair< int, int > > connectionTable;
public:
	SurfaceMatching(void);
	virtual ~SurfaceMatching(void);
	void setMesh(kmb::MeshData* mesh,const char* insNodes=NULL);
	// facegroup => surface のマッチングを行う
	void setPair(kmb::bodyIdType bodyId,const char* faceGroup);
	kmb::elementIdType getMatchingElementId(kmb::Face f,int &index);
	// すべての Face に対して対応する要素があるように、なければダミーを追加する。
	// MatchingElement を検索して、なければ appendDummyElement を呼び出す。
	// 追加した element の個数を返す
	size_t constructDummyElements(void);
private:
	void clear(void);
	void clearPairInfo(void);
	// 距離が最小になるような組み合わせを探す
	bool calcMapping(void);
	// f に match する要素が存在しないときに、境界の近傍の節点を使って masterSurf にダミー要素の追加
	// 追加したときは節点座標は２重化されているので、あとから節点をずらすこと
	// 追加した要素の向きは f と合うようにする、すなわち getMatchingElementId で得られる index = 0
	kmb::elementIdType appendDummyElement(kmb::Face f);
	// nodeId n0 を複製して n1 を作ってそれを返す
	// insertedNodes に登録
	kmb::nodeIdType duplicateNode(kmb::nodeIdType n0);
	// 位相が合うような対応かどうかをチェックする
	// 合わない場合は位相が合わなかった perm の index のうちの大きい方を返す
	// 位相が合う場合は -1 を返す
	// それ以外のエラーは -2 を返す
	int checkTopologicalMapping( kmb::Permutation& perm ) const;
};

}
