/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : AFTetraMesher                                           #
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
#include "MeshDB/kmbFace.h"

namespace kmb{

class Point3DContainer;
class ElementContainer;
class ElementEvaluator;
class MeshData;
class NodeNeighborInfo;

class PatchModifier
{
protected:
	kmb::Point3DContainer* points;
	kmb::ElementContainer* patch;
	kmb::ElementEvaluator* evaluator;
	kmb::NodeNeighborInfo* neighborInfo;
public:
	PatchModifier(kmb::Point3DContainer* points,kmb::ElementContainer* patch);
	PatchModifier(kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	virtual ~PatchModifier(void);
	// 向きをそろえる。向きを変えた要素の個数を返す。
	int uniformOrientation(kmb::elementIdType elementId=kmb::Element::nullElementId);
	// 隣接する三角形の辺を交換する
	// [0,1,3] + [1,2,3] => [0,1,2] + [2,3,0]
	bool triangleEdgeSwap(kmb::elementIdType tri0, kmb::elementIdType tri1 );
	// 隣接する四角形の辺を交換する
	// [0,1,2,3] + [0,3,4,5] => [1,2,3,4] + [0,1,4,5]  (orientation=true)
	//                       => [0,1,2,5] + [2,3,4,5]  (orientation=false)
	bool quadEdgeSwap(kmb::elementIdType quad0, kmb::elementIdType quad1, bool orientation=true);
	// 隣接する四角形に１点を加えて３つの四角形にする
	// できるパターンは２通り
	// orientation true => quad0 の 0 番目から辺を出す
	kmb::elementIdType quadTrisect(kmb::elementIdType quad0, kmb::elementIdType quad1, bool orientation=true );
	// 隣接している要素番号を返す
	int getNeighborElements(kmb::elementIdType elementId,kmb::elementIdType* neighbors) const;
	kmb::elementIdType getNeighborElement(kmb::Face f) const;
	// 辺の縮退
	bool edgeContract(kmb::nodeIdType node0, kmb::nodeIdType node1);
	// 頂点のなす角度を返す
	double getAngleByIndex(kmb::elementIdType elementId,int index) const;
	// TODO : 凹な四角形があったときに、修正する
};

}
