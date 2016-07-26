/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshSmoother                                            #
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

#include "MeshDB/kmbTypes.h"

#include <map>
#include <cstddef>

namespace kmb{

class MeshData;
class Point3DContainer;
class ElementContainer;
class ElementEvaluator;
class NodeNeighborInfo;
class DataBindings;

class MeshSmoother
{
protected:
	kmb::MeshData* mesh;
	kmb::Point3DContainer* shiftPoints;
	std::multimap< kmb::nodeIdType, kmb::nodeIdType > nodeConnectTable;
	// 緩和係数（0.0 の時に重心で置き換え、1.0の時はもとのまま）
	double relax;
public:
	MeshSmoother(kmb::MeshData* mesh);
	virtual ~MeshSmoother(void);
	void clear(void);
	// Laplacian Smoothing の対象となる要素グループの設定
	// 戻り値は nodeConnectTable に登録された個数
	size_t appendBody(kmb::bodyIdType bodyId);
	size_t appendBody(const kmb::ElementContainer* body);

	// shiftPoints の初期化
	void init(void);
	// 節点を周りの要素に従って Laplacian smoothing する
	// nodeConnectTable に登録されていなかったら移動しない
	// 戻り値は移動した節点の個数
	int smoothingNode(kmb::nodeIdType nodeId);
	int smoothingAllNode(void);
	int smoothingNodeGroup(const char* name,const char* stype=NULL);
	// smoothing の結果を points に適用
	void commit(void);
private:
	// nodeConnectTable に登録
	bool appendConnectNode(kmb::nodeIdType n0,kmb::nodeIdType n1);
};

}
