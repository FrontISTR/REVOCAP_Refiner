/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerNArray                                  #
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
/**
 * 単一種の要素を節点配列に格納する
 * 上書き可能かどうかは 0 番目と 1 番目の節点番号が同じかどうかで判断する
 * Fortran での節点配列を C でそのまま使うための nodeOffset
 * set系 : + nodeOffset
 * get系 : - nodeOffset
 *
 * 既に節点配列が使われているかは 0 番目と 1 番目の節点番号が異なる場合と判定する
 */

#pragma once
#include "MeshDB/kmbElementContainer.h"

namespace kmb{

class ElementContainerNArray : public ElementContainerDirectAccessable
{
protected:
	size_t index; // 0 から順に埋めていく場合の添え字
	size_t size;  // 配列の大きさ
	kmb::elementType etype; // 要素の種類
	size_t ncount; // 要素ごとの節点の個数
	kmb::nodeIdType *nodeTable;  // 節点配列
	bool nodeTableDeletable;  // 節点配列をこのクラスで Delete してもよいかどうか
	kmb::nodeIdType nodeOffset; // 節点番号をずらして登録する場合
public:
	static const char* CONTAINER_TYPE;
	ElementContainerNArray(kmb::elementType etype, size_t size );
	// writable = true は確保した配列だけ渡して、要素を格納するのはこのクラスで行う
	// writable = false はすでに要素を格納した配列を渡して、ラッパとしてこのクラスを使う
	ElementContainerNArray(kmb::elementType etype, size_t size, kmb::nodeIdType *nodeTable, bool writable=false, kmb::nodeIdType offset=0 );
	virtual ~ElementContainerNArray(void);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id);
	virtual bool getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const;
	virtual bool deleteElement(const kmb::elementIdType id);
	virtual bool includeElement(const kmb::elementIdType id) const;
	virtual kmb::elementIdType getMaxId(void) const;
	virtual kmb::elementIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual void initialize(size_t size=0);

	virtual const char* getContainerType(void) const;

	class _iteratorNA : public ElementContainer::_iterator
	{
		friend class ElementContainerNArray;
	public:
		virtual kmb::elementIdType getId(void) const;
		virtual bool getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const;
		virtual kmb::elementType getType(void) const;
		virtual kmb::nodeIdType getCellId(int cellIndex) const;
		virtual bool setCellId(int cellIndex, kmb::nodeIdType nodeId);
		virtual kmb::nodeIdType operator[](const int i) const;
		virtual ElementContainer::_iterator* operator++(void);
		virtual ElementContainer::_iterator* operator++(int n);
		virtual ElementContainer::_iterator* clone(void);
	private:
		_iteratorNA(void);
		virtual ~_iteratorNA(void);
		size_t index;
		const ElementContainerNArray* elementContainer;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::elementIdType id);
	virtual const_iterator find(kmb::elementIdType id) const;

	kmb::elementType getElementType(kmb::elementIdType id) const;
	// direct access
	// nodeOffset は考慮されないので注意
	// elementOffset は考慮される
	kmb::nodeIdType operator()(kmb::elementIdType elementId,kmb::idType localId) const;
	kmb::nodeIdType& operator()(kmb::elementIdType elementId,kmb::idType localId);
	void commit(kmb::elementIdType elementId);
};

}
