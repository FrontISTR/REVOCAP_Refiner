/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PatchOperation                                          #
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
#include "MeshDB/kmbPolygon.h"

namespace kmb{

class MeshData;
class MeshDB;
class Point3DContainer;
class ElementContainer;
class Polygon;

class EdgeStitcher
{
private:
	kmb::MeshData* mesh;
	kmb::Point3DContainer* points;
	kmb::Polygon forward;			// 順方向向きの多角形
	kmb::nodeIdType f0;				// forward 多角形の注目節点
	kmb::nodeIdType fend;			// forward 多角形の終了節点
	kmb::Polygon backward;			// 逆方向向きの多角形
	kmb::nodeIdType b0;				// backward 多角形の注目節点
	kmb::nodeIdType bend;			// backward 多角形の終了節点
	kmb::Vector3D normal;			// 判定用の法線ベクトル
	bool normalCheck;				// 法線チェックをするかどうか
	void setInitNormal(void);		// 初期法線ベクトルを計算する

public:
	EdgeStitcher(void);
	virtual ~EdgeStitcher(void);
	
	// for ruby interface
	void setMesh(kmb::MeshData* mesh);
	// FaceGroup で Edge を与える
	// 親 Body を必ず与えておかなければならない
	bool setEdges(const char* forward,const char* backward);
	bool setEdges(kmb::bodyIdType forward,kmb::bodyIdType backward);
	bool setEdgesWithinDistance(kmb::bodyIdType forward,kmb::bodyIdType backward,double distance);
	bool setEdgesWithInitNodes(kmb::bodyIdType forward,kmb::nodeIdType forwardInit,kmb::bodyIdType backward,kmb::nodeIdType backwardInit);
	kmb::bodyIdType stitchEdgesByTriangles(void);
	kmb::bodyIdType stitchEdgesByQuads(void);
	// 境界の要素の個数をチェックしながら縫い合わせる
	// fCount : forward を境界に持つような patch の要素数
	// bCount : backward を境界に持つような patch の要素数
	kmb::bodyIdType stitchByQuadsWithNodeCount(int fCount,int bCount);

	void setNormalCheck(bool check);
	bool getNormalCheck(void) const;

	bool stitchByTriangle( kmb::ElementContainer* result );
	bool stitchByQuad( kmb::ElementContainer* result );
	// 最初の注目節点を設定する。
	// nullNodeId を入れた場合は、一番近いところで適当に設定する。
	void setInitialNode( kmb::nodeIdType forwardInit=kmb::nullNodeId, kmb::nodeIdType backwardInit=kmb::nullNodeId );
};

}
