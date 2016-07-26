/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.7                          #
# Class Name : MeshOperation                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/05/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "MeshDB/kmbTypes.h"
#include <map>
#include <cstddef>

namespace kmb{

class MeshData;
class NodeNeighborInfo;
class ElementBase;
class ElementContainer;
class DataBindings;

class MeshOperation
{
protected:
	kmb::MeshData* mesh;
public:
	MeshOperation(kmb::MeshData* _mesh);
	virtual ~MeshOperation(void);
	// 領域ごとに節点番号が重複しないようにする
	// 構造解析用のモデルの場合、異材料間に力が伝わらなくなるので注意する。
	size_t nodeSplitByVolumes(void);
	// body 上の節点番号を複製
	// coupleName が NULL ではないときは、orgNodeId => newNodeId の対応を Data として記録する
	// 戻り値は置き換えた節点の個数を返す
	int duplicateNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName=NULL);
	// 距離が thresh 以下の節点を一つにまとめる、関連する要素も書き換える
	// ただし、同じ要素内に近い節点がある場合もチェックしないので、不正な要素ができる可能性がある
	size_t uniteNodes(double thresh);
	// 三角形分割
	int triangulation(kmb::bodyIdType bodyId);

	// static 関数
	// 親子関係
	static bool isParent(const kmb::ElementContainer* child,const kmb::ElementContainer* parent,const kmb::NodeNeighborInfo &neighbors);
	// surface から facegroup への変換
	static int surface2FaceGroup(const kmb::ElementContainer* child,const kmb::ElementContainer* parent,const kmb::NodeNeighborInfo &neighborInfo,kmb::DataBindings* faceGroup);
protected:
	// 親要素の検索
	static kmb::elementIdType getParentElement(const kmb::ElementBase &elem,const kmb::ElementContainer* parent,const kmb::NodeNeighborInfo &neighbors);
};

}
