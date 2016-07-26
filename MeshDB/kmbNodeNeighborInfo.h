/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeNeighborInfo                                        #
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
#include <map>
#include <vector>
#include <string>

namespace kmb{

class Element;
class ElementBase;
class MeshData;
class DataBindings;
class ElementContainer;
class Face;

typedef std::multimap< kmb::nodeIdType, kmb::elementIdType > NodeNeighbor;

class NodeNeighborInfo
{
public:
	NodeNeighborInfo(void);
	virtual ~NodeNeighborInfo(void);

	void clear(void);
	bool getIgnoreOrientation(void) const;
	void setIgnoreOrientation(bool f);

	// elementId vs nodeId table 
private:
	bool append( kmb::nodeIdType nodeID, kmb::elementIdType elementID );
	bool erase( kmb::nodeIdType nodeID, kmb::elementIdType elementID );
public:
	bool appendCoboundary( kmb::elementIdType elementId, const kmb::ElementBase &element );
	bool appendCoboundary( const kmb::ElementContainer* elements );
	bool appendCoboundary( const kmb::MeshData* mesh );
	bool appendCoboundary( const kmb::DataBindings* data, const kmb::ElementContainer* body );
	bool appendCoboundary( const kmb::DataBindings* data, const kmb::MeshData* mesh );
	// erase correspondence
	bool deleteCoboundary( kmb::elementIdType elementId, const kmb::ElementBase &element );

	// 近傍情報、周辺情報の取得
	
	// 要素を与えてその近傍要素を取り出し、neighbors に返す
	// neighbors は呼び出し側で element の境界の個数だけあらかじめ用意して
	// 呼び出し側でメモリの確保・解放を行うこと
	// 戻り値は近傍要素の個数
	// 注意：neighbors に入っている近傍の順番は意味はない（途中に NULL は出てこない）
	int getNeighborElements( kmb::elementIdType elementID, kmb::elementIdType *neighbors, const kmb::ElementContainer* elements ) const;
	int getNeighborElements( kmb::elementIdType elementID, kmb::elementIdType *neighbors, kmb::MeshData* mesh ) const;
	
	// 要素を与えて、その境界要素を、登録されている neighborInfo から探す
	// 見つかった個数を返す
	int getElementBoundary( const kmb::ElementBase &element, kmb::elementIdType *boundaries, const kmb::ElementContainer* elements ) const;
	// 平面要素を登録している時に立体要素 element と面番号を与えて境界要素を探す
	// element は必ずしも elements に含まれていなくてもよい
	kmb::elementIdType getElementFace( const kmb::ElementBase &element, kmb::idType faceId, const kmb::ElementContainer* elements ) const;
	// 立体要素を登録している時に平面要素を与えて、element と面番号を返す
	// element : 平面要素
	// face : element を面にもつような立体要素の Face 
	bool getFace( const kmb::ElementBase &element, kmb::Face &face, const kmb::ElementContainer* elements ) const;
	// face を与えて、それに接している Face adj を求める
	bool getAdjacentFace( const kmb::Face &face, kmb::Face &adj, const kmb::ElementContainer* elements ) const;
	bool getAdjacentFace( const kmb::Face &face, kmb::Face &adj, kmb::MeshData* mesh ) const;
public:
	// get adjacent nodes from a node
	void getNodeNeighbor( kmb::nodeIdType nodeId, std::set<kmb::nodeIdType> &neighbors, const kmb::ElementContainer* elements );
	void getNodeNeighbor( kmb::nodeIdType nodeId, std::set<kmb::nodeIdType> &neighbors, kmb::MeshData* mesh );
	// if only 2D elements are appended,
	// get sorted adjacent nodes from a node
	void getCrownNodeNeighbor
		( kmb::nodeIdType nodeID,
			std::vector<kmb::nodeIdType> &neighbors,
			const kmb::ElementContainer* elements );
	// is connected between two nodes
	bool isConnected( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, const kmb::ElementContainer* elements ) const;
	bool isConnected( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, kmb::MeshData* mesh ) const;

	// 節点を与えてその周辺要素の個数
	size_t getElementCountAroundNode(nodeIdType nodeId) const;
	// すべての近傍情報の個数
	size_t getSize(void) const;

	// 節点を与えてその近傍要素の Iterator を与える
	NodeNeighbor::iterator beginIteratorAt(kmb::nodeIdType nodeID){
		return coboundaries.lower_bound(nodeID);
	};
	NodeNeighbor::iterator endIteratorAt(kmb::nodeIdType nodeID){
		return coboundaries.upper_bound(nodeID);
	};
	NodeNeighbor::const_iterator beginIteratorAt(kmb::nodeIdType nodeID) const{
		return coboundaries.lower_bound(nodeID);
	};
	NodeNeighbor::const_iterator endIteratorAt(kmb::nodeIdType nodeID) const{
		return coboundaries.upper_bound(nodeID);
	};

	// 節点を与えてその周辺要素の集合を返す
	void getSurroundingElements(kmb::nodeIdType nodeId, std::vector<kmb::elementIdType> &elements) const;
	// given two nodes, return elements which have both nodes
	void getSurroundingElementsOf2Node(kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, std::vector<kmb::elementIdType> &elements) const;

	// 節点を与えて反時計回りに接している平面要素を返す
	// 平面要素の場合のみ
	kmb::elementIdType getNextElementAroundNode( kmb::ElementContainer* triangles, kmb::nodeIdType nodeID, kmb::elementIdType elemID, bool clockwise=false) const;

	// 線分要素の場合のみ
	// 端点を求める
	void getEndsOfEdge( const kmb::ElementContainer* edges, kmb::nodeIdType& firstID, kmb::nodeIdType& endID ) const;

	NodeNeighbor::iterator beginNodeIterator(void);
	NodeNeighbor::iterator endNodeIterator(void);
private:
	NodeNeighbor coboundaries;
	bool admitAnti;
};

}
