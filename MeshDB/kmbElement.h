/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Element                                                 #
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
/*
 * 1次元要素 connectionTable
 * 2次元要素 connectionTable faceTable
 * 3次元要素 connectionTable faceTable edgeTable
 *
 * 2次要素について面、辺の1次元要素は SEGMENT2
 * 2次元要素の faceTable の第3成分は index 順にソートされている
 * 3次元要素の edgeTable の第3成分は index 順にソートされている
 *
 */

#pragma once

#include <map>
#include <string>

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElementType.h"

namespace kmb{

class MeshData;
class Tetrahedron;

// 要素に関する処理で、node table のみで処理できるものについては Base クラスにメソッドを追加する
// Point3D もしくは Point2D に依存するものについては ElementEvaluator に実装する

class ElementBase
{
public:
	ElementBase(void){}
	virtual ~ElementBase(void){}
	// static 版
	// size of node table array
	static int getNodeCount(kmb::elementType type);
	// size of vertex ( which equals node count in case an element is 1st degree )
	// vertex count of 2nd element = vertex count of 1st element = node count of 1st element
	static int getVertexCount(kmb::elementType type);
	static int getBoundaryCount(kmb::elementType type);
	static int getEdgeCount(kmb::elementType type);
	static kmb::elementType getBoundaryType(kmb::elementType type,int index);
	static int getBoundaryNodeCount(kmb::elementType type,int index);
	static int getBoundaryVertexCount(kmb::elementType type,int index);
	static kmb::elementType getEdgeType(kmb::elementType type,int index);
	static int getEdgeNodeCount(kmb::elementType type,int index);
	static int getEdgeVertexCount(kmb::elementType type,int index);
	static int getDimension(kmb::elementType type);
	static int getDegree(kmb::elementType type);
	static const std::string getTypeString(kmb::elementType type);

	// remark :call this method by kmb::ElementBase::getType
	static kmb::elementType getType(std::string str);
	static kmb::elementType getLinearType(kmb::elementType type);
	static kmb::elementType getSecondType(kmb::elementType type);
	static int getDiagIndex(kmb::elementType type,int index);
	// basic virtual methods
	virtual kmb::elementType getType(void) const = 0;
	virtual kmb::nodeIdType getCellId(int cellIndex) const = 0;
	virtual bool setCellId(int cellIndex, kmb::nodeIdType nodeId) = 0;
	virtual kmb::nodeIdType operator[](const int i) const = 0;

	// instance 版
	int getNodeCount(void) const;
	int getVertexCount(void) const;
	int getBoundaryCount(void) const;
	int getEdgeCount(void) const;
	kmb::elementType getBoundaryType(int index) const;
	kmb::elementType getEdgeType(int index) const;
	int getBoundaryNodeCount(int index) const;
	int getBoundaryVertexCount(int index) const;
	int getEdgeNodeCount(int index) const;
	int getEdgeVertexCount(int index) const;
	kmb::nodeIdType getBoundaryCellId(int index,int i) const;
	kmb::nodeIdType getEdgeCellId(int index,int i) const;
	int getDimension(void) const;
	int getDegree(void) const;
	const std::string getTypeString(void) const;
	kmb::elementType getLinearType(void) const;
	kmb::elementType getSecondType(void) const;
	int getDiagIndex(int index) const;
	// 便利関数系
	bool include(kmb::nodeIdType nodeId) const;
	bool includeVertex(kmb::nodeIdType nodeId) const;
	// 含まれなかったら -1 を返す
	int indexOf(kmb::nodeIdType nodeId) const;
	int countCommonNode(kmb::ElementBase& other) const;
	bool replaceNodeId(kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId);
	int replaceNodeId(std::map<nodeIdType,nodeIdType> &idmap);
	// 面要素、辺要素の生成
	kmb::elementType getBoundaryElement(int index,kmb::nodeIdType* cell) const;
	bool getBoundaryElement(int index,kmb::ElementBase& elem) const;
	kmb::elementType getEdgeElement(int index,kmb::nodeIdType* cell) const;
	bool getEdgeElement(int index,kmb::ElementBase& elem) const;
	// index0 と index1 が辺でつながっているかどうか 
	// 1/-1 は1次要素とみた時につながっているか
	// 符号は平面要素のときのみ意味があり、正の向きか負の向きかを示す
	// 2 2次要素とみた時につながっている
	// 0 つながっていない
	int isConnected(int index0,int index1) const;
	//
	// 面を構成する節点の添え字を与えて、何番目の面かを判定する
	// 1 順向きの面
	// -1 逆向きの面
	// 0 それ以外
	//
	int isFace(int index0,int index1,int &faceIndex) const;
	int isFace(int index0,int index1,int index2,int &faceIndex) const;
	int isFace(int index0,int index1,int index2,int index3,int &faceIndex) const;
	int isFace(const kmb::ElementBase &element, int &faceIndex) const;
	// 2点から決まる面、辺を特定する
	int getFaceIndex(int index0,int index1) const;
	// 裏返す
	bool reverse(void);
	static bool reverse( kmb::elementType etype, kmb::nodeIdType* nodes);
	// ３角形分割
	int divideIntoTriangles(kmb::nodeIdType triangles[][3]) const;
	// ４面体分割
	int divideIntoTetrahedrons(kmb::nodeIdType tetrahedrons[][4]) const;
	// 節点番号が最小の頂点（中間節点は含めない）を調べて、その節点配列の添え字番号を求める
	// index 番目の Face で節点番号が最小の節点添え字番号を求める
	int getIndexMinNodeIdOfFace(int index) const;
	int getIndexMinNodeId(void) const;
	// 平面要素について。中間節点の場合は nullNodeId を返す
	kmb::nodeIdType nextNode(kmb::nodeIdType nodeId) const;
	kmb::nodeIdType previousNode(kmb::nodeIdType nodeId) const;
};

class Element : public ElementBase
{
public:
	// element id of null element
	static kmb::elementIdType nullElementId;
	// maximum size of node table array of elements
	// MAX_NODE_COUNT = sizeof( Hexahedron2.cell ) = 20
	// MAX_NODE_COUNT_DIM2 = sizeof( Quad2.cell ) = 8
	static const int MAX_NODE_COUNT;
	static const int MAX_NODE_COUNT_DIM2;

	Element(kmb::elementType t = UNKNOWNTYPE);
	virtual ~Element(void);

	// clone
	Element* clone() const;
	Element* reverseClone() const;
	// clone with replacing node ids
	Element* clone( std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper ) const;
	Element* reverseClone( std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper ) const;
	// 生成メソッド
	// ary は配列でなく new で確保されたポインタを代入すること
	// そのポインタを保持するので、呼び出し側では delete しないこと！
	// ary = NULL の場合はこのメソッドの中で new する。
	static Element* create(kmb::elementType type,kmb::nodeIdType *ary=NULL);
	// get element type
	virtual kmb::elementType getType() const;
public:
	// set/get node table
	virtual bool setCellId(int index,kmb::nodeIdType nodeId);
	virtual kmb::nodeIdType getCellId(int index) const;
	virtual kmb::nodeIdType operator[](const int i) const;
	virtual kmb::nodeIdType& operator[](const int i);
protected:
	elementType type;
	nodeIdType *cell;
};


}
