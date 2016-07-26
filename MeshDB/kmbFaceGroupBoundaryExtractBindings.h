/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FaceGroupBoundaryExtractBindings                        #
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
// 表面抽出のための面データを管理するためのデータ構造
// 結果だけが欲しいときは BoundaryExtractor でよい
// 内部の面のペアとなっている面の情報が欲しい時に使う
#pragma once

#include "MeshDB/kmbDataBindings.h"
#include <map>

namespace kmb{

class MeshData;

class FaceGroupBoundaryExtractBindings : public kmb::DataBindings
{
protected:
	kmb::MeshData* mesh;
	std::multimap< kmb::nodeIdType, kmb::Face > facemap;
public:
	FaceGroupBoundaryExtractBindings(void);
	virtual ~FaceGroupBoundaryExtractBindings(void);
	virtual const char* getContainerType(void) const;
	virtual void clear(void);
	virtual size_t getIdCount(void) const;
	virtual bool addId(kmb::Face f);
	virtual bool hasId(kmb::Face f) const;
	virtual bool deleteId(kmb::Face f);
	virtual bool replaceId(kmb::Face old_f,kmb::Face new_f);

	class _iteratorFGBE : public DataBindings::_iterator
	{
		friend class FaceGroupBoundaryExtractBindings;
	private:
		std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator fIter;
		std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator endIter;
	public:
		_iteratorFGBE(void) : DataBindings::_iterator(){};
		virtual ~_iteratorFGBE(void){};
		virtual bool getFace(kmb::Face &f) const;
		virtual DataBindings::_iterator* operator++(void);
		virtual DataBindings::_iterator* operator++(int n);
		virtual DataBindings::_iterator* clone(void);
	};
	virtual DataBindings::iterator begin(void);
	virtual DataBindings::const_iterator begin(void) const;

	// 独自メソッド
	void setMesh(kmb::MeshData* mesh);
	size_t appendBody(kmb::bodyIdType bodyId);
	// f と重なっているもう一方の Face を返す
	// 裏返っていてもよい場合は reverse = true
	// 注：向きが同じ要素が登録されている場合は、通常面は向きが逆になる
	// reverse=true の時は親の要素の向きが異なる場合、面は同じ向きになる
	kmb::Face getNeighbor(kmb::Face f,bool reverse=false) const;
};

}
