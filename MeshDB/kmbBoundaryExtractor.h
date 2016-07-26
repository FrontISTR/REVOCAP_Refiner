/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BoundaryExtractor                                       #
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

#include <map>
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbElementRelation.h"

namespace kmb{

class MeshData;
class Face;
class ElementContainer;
class DataBindings;

class BoundaryExtractor
{
protected:
	kmb::MeshData* mesh;
	// 裏返っている要素を許すかどうか
	bool reverseMode;
	// 節点を含むような面の対応テーブル
	std::multimap< kmb::nodeIdType, kmb::Face > facemap;
	// すべての Face について facemap に登録する
	// elements == NULL のときは関係は登録済みの mesh のすべての要素を調べる
	void appendElement(kmb::elementIdType elementId,kmb::ElementBase &element,const kmb::ElementContainer* elements=NULL);
	// facemap に登録する
	void appendFaceMap(kmb::elementIdType elementId,kmb::ElementBase &element,kmb::idType faceId,const kmb::ElementContainer* elements=NULL);
	// facemap に登録されていたら削除する
	// ここでの引数 reverse はメンバ変数の reverseMode とは別。reverseMode = false を前提とする。
	// reverse = false ならば、同じ向きに登録されているものを削除（登録済みの要素で削除する場合）
	// reverse = true ならば、逆向きに登録されているものを削除（接している要素で削除する場合）
	void eraseFaceMap(kmb::ElementBase &element,kmb::idType faceId,bool reverse=false,const kmb::ElementContainer* elements=NULL);
	// ポインタで呼ばれる
	void appendElementContainer(const kmb::ElementContainer* body);
	// 内部呼び出し用ルーチン
	kmb::ElementRelation::relationType getFaceRelation(kmb::ElementBase &e0,kmb::idType f0,kmb::ElementBase &e1,kmb::idType f1) const;
public:
	BoundaryExtractor(void);
	virtual ~BoundaryExtractor(void);
	void setMesh(kmb::MeshData* mesh);
	void appendBody(kmb::bodyIdType bodyId);
	// 境界の探索対象を自分自身のみとする
	void appendUnitBody(const kmb::ElementContainer* body);
	// 要素グループなら、その要素の面を登録
	// 面グループなら、その面をそのまま facemap に登録
	void appendData(const char* name);
	void appendData(const kmb::DataBindings* data);
	void eraseData(const char* name,bool reverse=false);
	// 境界を抽出して、mesh に登録し、bodyId を返す
	// MeshData の bindingsStack に束縛されたデータが登録されていたら、それを境界にも登録する
	// bodyId に属する Face を facemap から取り出して抽出
	kmb::bodyIdType getBoundary(kmb::bodyIdType bodyId) const;
	// facemap に含まれる Face をすべて取り出して抽出
	kmb::bodyIdType getBoundary(void) const;
	// 他の BoundaryExtractor に登録されている Face を取り出して抽出
	kmb::bodyIdType getBoundary(kmb::BoundaryExtractor& other) const;

	// 境界をあらかじめ確保したコンテナに格納する (mesh == NULL のときでも使える)
	size_t getBoundary(const kmb::ElementContainer* parent,kmb::ElementContainer* boundary) const;
	// 境界をあらかじめ確保した節点グループに格納する (mesh == NULL のときでも使える)
	// 境界情報を要素でなく、節点グループでほしいときに使う
	size_t getBoundaryNodeGroup(const kmb::ElementContainer* parent,kmb::DataBindings* nodeGroup) const;
	// 境界を成分に分割して返す
	int getBoundaryComponents(kmb::bodyIdType bodyId,kmb::bodyIdType* &boundaryIds) const;
	// 登録してある境界情報から bodyId に含まれるものを FaceGroup として
	// name をつけて取り出す。このとき、stype = "Brep" にする。
	bool getBoundaryFace(kmb::bodyIdType bodyId,const char* name) const;
	// 登録してある境界情報から Element と一致する Face を取り出す
	bool getBoundaryFace(const kmb::ElementBase& elem,kmb::Face &f) const;

	// すでに登録されている境界面と接するような面を抽出
	// bodyId = 0 と bodyId = 1 のインターフェイス面を取り出すには
	// appendBody(0) してから getInterfaceBoundary(1) する
	// 格納されるのは bodyId = 1 の面
	kmb::bodyIdType getInterfaceBoundary(kmb::bodyIdType bodyId) const;
	bool getInterfaceBoundaryFace(kmb::bodyIdType bodyId,const char* name) const;

	// boundary がない状態 = closed かどうかの判定
	// 判定のためにはあらかじめ appendBody しておくことが必要
	// 境界面で body に属しているものもがあるかをチェックする
	bool isClosed(kmb::bodyIdType bodyId) const;
	// 端に境界面の集合が空かどうかをチェックする
	bool isClosed(void) const;
	// 初期状態に戻す
	void clear(void);
	void eraseData(const kmb::DataBindings* data,bool reverse);
	void setReverseMode(bool mode);
	bool getReverseMode(void) const;
};

}
