/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeNeighborPtrInfo                                     #
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
 * kmb::NodeNeighborInfo と同等の機能
 * ただし、elementId との対応ではなくて、kmb::Element* との対応を保存する
 */

#pragma once

#include "MeshDB/kmbTypes.h"
#include <map>
#include <cstddef>

namespace kmb{

class Element;
class ElementContainer;
class MeshData;

// Delaunay や Advancing Front の初期近傍情報の生成に用いるため
// 保存するポインタは const ではない

typedef std::multimap< kmb::nodeIdType, kmb::Element* > NodeNeighborPtr;

class NodeNeighborPtrInfo
{
public:
	NodeNeighborPtrInfo(void);
	virtual ~NodeNeighborPtrInfo(void);

	void clear(void);
	bool getIgnoreOrientation(void) const;
	void setIgnoreOrientation(bool f);
private:
	// nodeId vs Element* table
	bool append( kmb::nodeIdType nodeId, kmb::Element* element );
	bool erase( kmb::nodeIdType nodeId, kmb::Element* element );
public:
	bool appendCoboundary( kmb::Element* element );
	bool appendCoboundary( kmb::ElementContainer* elements );
	bool appendCoboundary( kmb::MeshData* mesh );
	// erase correspondence
	bool deleteCoboundary( kmb::Element* element );

	// 近傍情報、周辺情報の取得
	
	// 要素を与えてその近傍要素を取り出し、neighbors に返す
	// neighbors は呼び出し側で element の境界の個数だけあらかじめ用意して
	// 呼び出し側でメモリの確保・解放を行うこと
	// 戻り値は近傍要素の個数
	int getElementNeighbor( const kmb::Element* element, kmb::Element** neighbors ) const;
	int getElementNeighborFace( const kmb::Element* element, kmb::Element** neighbors, int *faces ) const;
	// get adjacent nodes from a node
	void getNodeNeighbor( kmb::nodeIdType nodeId, std::set<kmb::nodeIdType> &neighbors ) const;
	// is connected between two nodes
	bool isConnected( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1 ) const;
	// get the number of elements around the given node
	size_t getElementCountAroundNode(kmb::nodeIdType nodeId) const;
	// get the number of correspondence
	size_t getSize(void) const;

	// get the iterator of elements around the given node
	NodeNeighborPtr::iterator beginIteratorAt(kmb::nodeIdType nodeId){
		return coboundaries.lower_bound(nodeId);
	};
	NodeNeighborPtr::iterator endIteratorAt(kmb::nodeIdType nodeId){
		return coboundaries.upper_bound(nodeId);
	};

	NodeNeighborPtr::iterator beginNodeIterator(void);
	NodeNeighborPtr::iterator nextNodeIterator(kmb::nodeIdType nodeId);
	NodeNeighborPtr::iterator endNodeIterator(void);
private:
	NodeNeighborPtr	coboundaries;
	bool admitAnti;
};

}
