/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TetraItem                                               #
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
//
// list 的な四面体のコンテナ
// 隣接情報も保持
// neighborFace 
// -2 : 参照しない
// -1 : 隣接する tetra がない
// 0, 1, 2, 3 : 隣接 tetra の相手側の面番号
//
#pragma once

#include "MeshDB/kmbTetrahedron.h"
#include "MeshGen/kmbListItem.h"

namespace kmb{

class Point3DContainer;

class TetraN : public Tetrahedron
{
protected:
	TetraN** neighbors;
	// 近傍情報
	// メモリ節約のための内部表現 int[4](-2〜3)
	// -1 : 隣接する tetra がない
	// -2 : 参照しない
	unsigned int face;
public:
	TetraN(nodeIdType n0,nodeIdType n1,nodeIdType n2,nodeIdType n3);
	virtual ~TetraN(void);
	// [this,index] <=> [nei,f] が接する
	void setNeighbor(int index, kmb::TetraN* nei,int f);
	// 0 1 2 3 は getNeighbor で得られる隣接する tetra からみた自分自身が接する面番号
	// [this,i] と [getNeighbor(i),getNeighborFace(i)] が隣接する
	kmb::TetraN* getNeighbor(int index) const;
	int getNeighborFace(int index) const;
	// nei が何番目の面に隣接しているか
	int getNeighborIndex(kmb::TetraN* nei) const;
};

class TetraItem : public Tetrahedron
{
protected:
	kmb::TetraItem* previousTetra;
	kmb::TetraItem* nextTetra;
	kmb::TetraItem* neighbors[4];
	// 近傍情報
	// メモリ節約のための内部表現 int[4](-2〜3)
	// -1 : 隣接する tetra がない
	// -2 : 参照しない
	unsigned int face;
public:
	TetraItem(nodeIdType n0,nodeIdType n1,nodeIdType n2,nodeIdType n3);
	virtual ~TetraItem(void);
	// list item
	kmb::TetraItem* getPrevious(void) const { return previousTetra; }
	void setPrevious(kmb::TetraItem* tetra){ previousTetra = tetra; }
	kmb::TetraItem* getNext(void) const { return nextTetra; }
	void setNext(kmb::TetraItem* tetra){ nextTetra = tetra; }

	// リストから外す
	// this->nextTetra->previousTetra を this から this->previousTetra に変える
	// this->previousTetra->nextTetra を this から this->nextTetra に変える
	// 自分の previousTetra と nextTetra は NULL にする
	void detach(void);
	void detach(kmb::TetraItem* &first,kmb::TetraItem* &last);
	// リストに追加
	void attach(kmb::TetraItem* last);
	void attach(kmb::TetraItem* &first,kmb::TetraItem* &last);

	// [this,index] <=> [nei,f] が接する
	void setNeighbor(int index, kmb::TetraItem* nei,int f);
	// 0 1 2 3 は getNeighbor で得られる隣接する tetra からみた自分自身が接する面番号
	// [this,i] と [getNeighbor(i),getNeighborFace(i)] が隣接する
	kmb::TetraItem* getNeighbor(int index) const;
	int getNeighborFace(int index) const;
	// nei が何番目の面に隣接しているか
	int getNeighborIndex(kmb::TetraItem* nei) const;

	// tetra から始まるリストについて、近傍関係を生成する
	static void generateNeighborRelations( kmb::TetraItem* tetra );

	// t0 の i0 番目の面と t1 の i1 番目の面が接しているとき
	static void setAdjacentPair(kmb::TetraItem* t0,int i0,kmb::TetraItem* t1,int i1);
	// first から始まるリストを削除
	// 削除したらそれに隣接していた tetra の neighborface は -2 にする
	static int deleteItems( kmb::TetraItem* first );

	// for debug
	static int debugItem( kmb::TetraItem* item, const char* tag=NULL  );
	static int debugItems( kmb::TetraItem* first, const char* tag=NULL );
	static bool checkVolume( kmb::TetraItem* tetra, kmb::Point3DContainer &points );
	static bool checkVolumes( kmb::TetraItem* tetra, kmb::Point3DContainer &points, const char* tag=NULL );
	static bool checkNeighbor( kmb::TetraItem* tetra );
	static bool checkNeighbors( kmb::TetraItem* tetra, const char* tag=NULL );
};

}
