/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BLMGenerator                                            #
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

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"

#include <vector>

namespace kmb{

class MeshData;
class ElementContainer;
class Face;

class BLMGenerator
{
protected:
	// 層の個数と厚さ
	// layerThick は小さい順に並んでいること
	int layerNum;
	double* layerThick;
	MeshData* mesh;
public:
	BLMGenerator(void);
	BLMGenerator(MeshData* mesh);
	BLMGenerator(MeshData* mesh,int layerNum,double* layerThick);
	virtual ~BLMGenerator(void);

	// extrude => 押し出し
	//   表面メッシュの外側に層メッシュを追加していく
	//   入力の表面メッシュは移動させずに節点番号も変えない
	//   出力の外側には新しい節点番号の表面メッシュができる
	// intrude => 押し込み（表面拡張）
	//   表面メッシュを膨らませて内側に層メッシュを追加していく
	//   出力の外側には入力と同じ節点番号の表面メッシュが現れる
	//   節点番号は変えずに座標値だけを変えている
	//   内側のメッシュ parentId は表面の節点番号が置き換えられる
	// intrudeB => 押し込み（表面固定）
	//   表面メッシュを固定して内側に層メッシュを追加していく
	//   出力の外側には入力と同じ節点番号の表面メッシュが現れる
	//   もともとのメッシュをひずませて層メッシュを挿入している
	//   内側のメッシュ parentId は表面の節点番号が置き換えられる
	// boundalyId   対象となる表面メッシュの Id（入出力兼用）
	// parentId     押し込み対象となるボリュームメッシュの Id
	// 戻り値       生成した層メッシュの Id
	kmb::bodyIdType extrude(kmb::bodyIdType &boundaryId);
	kmb::bodyIdType intrude(kmb::bodyIdType parentId,kmb::bodyIdType &boundaryId);
	kmb::bodyIdType intrudeB(kmb::bodyIdType parentId,kmb::bodyIdType &boundaryId);

	// 面データを与えて
	// extrude => 押し出し
	// intrude => 押し込み
	// 戻り値  生成した層メッシュの Id
	// 与えた面データは層の外側の面になるように更新される
	kmb::bodyIdType extrudeFromData(const char* faceGroup);
	kmb::bodyIdType intrudeFromData(const char* faceGroup);
	kmb::bodyIdType intrudeBFromData(const char* faceGroup);

	// 登録した MeshData に BLM を生成する（入力面の内部）
	// 与えられた faceGroup は新しい境界面に更新される（targetBodyIdも）
	// それ以外の境界面は変化しないことを保証する
	// faceGroup の表面領域の内部にだけ BLM を作ることに注意する
	bool generateInnerFromData(const char* faceGroup,kmb::bodyIdType &layerId);

	// 登録した MeshData に BLM を生成する（入力面の内部）
	// 与えられた bodyId は新しい境界面に更新される
	// それ以外の境界面は変化しないことを保証する
	// 入力表面の内部にだけ BLM を作ることに注意する
	bool generateInner(kmb::bodyIdType bodyId,kmb::bodyIdType parentId,kmb::bodyIdType &layerId);

	// layer は積算で与える
	// 例えば thicks = [0.1, 0.2, 0.3, 0.5, 0.8]
	// setLayer(5,thicks)
	// のように
	// thicks のメモリ管理は呼び出し側が責任を持つ
	void setLayer(int layerNum,double* thicks);

	void setMesh(MeshData* mesh);
protected:
	// getLayerNodeId( outerNodeId, innerNodeId, layerIndex );
	// 0 <= layerIndex <= layerNum
	// layerIndex = 0             : innerNodeId
	// layerIndex = layerNum      : outerNodeId
	// 0 < layerIndex < layerNum  : innerNode と outerNode を layerThick の間隔で
	//                              分割した節点の nodeId を返す
	//                              まだ節点がなければ作り、既にあればキャッシュした
	//                              データから返す
	// キャッシュの内容は
	// layers = layerNodes[ std::pair(innerNodeId,outerNodeId) ];
	// layers[0] : layerIndex = 1
	// layers[1] : layerIndex = 2
	// layers[2] : layerIndex = 3
	// .....
	// layers[layerNum-2] : layerIndex = layerNum-1
	kmb::nodeIdType getLayerNodeId(kmb::nodeIdType outerNodeId, kmb::nodeIdType innerNodeId, int layerIndex ) const;


	// 要素の型 (etype, oldNodeId) に対して nodeMapper で与えられる対応があれば
	// 新節点と旧節点の間に要素を登録されている mesh に addElement で追加
	// このメソッドを beginElement と endElement に挟んで使うこと
	// oldNodeId が外側から見えるとき outer = true
	kmb::Face appendLayerElements( kmb::elementType etype, kmb::nodeIdType* oldNodeId, bool outer=true);
private:
	mutable std::map< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType* > layerNodes;
	void clearLayerNodes(void);
	// 二重化した節点の対応
	// original => duplicated
	std::map< kmb::nodeIdType, kmb::nodeIdType > nodeMapper;
	kmb::nodeIdType getDuplicatedNodeId(kmb::nodeIdType nodeId);
};

}
