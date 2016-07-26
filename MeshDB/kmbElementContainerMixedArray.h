/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerMixedArray                              #
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
 * 複数種の要素を節点配列に格納する
 *
 * nodeTable  要素の節点番号を順に並べた配列
 * etypeTable 要素の種類を並べた配列（char で確保していることに注意）
 * indexTable 要素の区切りの配列の添え字を並べた配列
 */
#pragma once
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElement.h"

namespace kmb{

class ElementContainerMixedArray : public ElementContainerDirectAccessable
{
public:
	static const char* CONTAINER_TYPE;
protected:
	size_t size; // 要素の配列の大きさ
	kmb::nodeIdType *nodeTable; // 節点配列
	char *etypeTable; // 要素の種類の配列
	size_t *indexTable; // 要素の区切りの index の配列
	// i 番目の要素は
	// 要素の種類 etype = etypeTable[i] 
	// 配列の区切り j = indexTable[i]
	// len = kmb::Element::getNodeCount( etype )
	// nodeTable[j] ... nodeTable[j+len-1]
	bool nodeTableDeletable;  // 節点配列をこのクラスで Delete してもよいかどうか
	bool etypeTableDeletable;  // 要素種類配列をこのクラスで Delete してもよいかどうか
	bool indexTableDeletable;  // 要素区切り配列をこのクラスで Delete してもよいかどうか
	kmb::nodeIdType nodeOffset; // 節点番号をずらして登録する場合
	size_t index; // 次に登録する要素種類配列の添え字（登録済みの個数とは違う）
	size_t nindex; // 次に登録する節点配列の添え字（登録済みの個数とは違う）
	size_t count; // 既に登録済みの個数
public:
	ElementContainerMixedArray(size_t typeCounter[kmb::ELEMENT_TYPE_NUM]);
	// 余裕を持って eSize * MAX_NODE_COUNT で初期化する
	ElementContainerMixedArray(size_t eSize);
	// writable = true は確保した配列だけ渡して、要素を格納するのはこのクラスで行う
	// writable = false はすでに要素を格納した配列を渡して、ラッパとしてこのクラスを使う
	ElementContainerMixedArray(size_t eSize,char* etype, kmb::nodeIdType *nodeTable, bool writable=false, kmb::nodeIdType offset=0 );
	// 要素種類配列が与えられているとき
	ElementContainerMixedArray(size_t eSize,char *etype,kmb::nodeIdType offset=0);
	virtual ~ElementContainerMixedArray(void);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id);
	virtual bool getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const;
	virtual bool deleteElement(const kmb::elementIdType id);
	virtual bool includeElement(const kmb::elementIdType id) const;
	virtual kmb::elementIdType getMaxId(void) const;
	virtual kmb::elementIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void initialize(size_t eSize=0);
	void initialize(size_t typeCounter[kmb::ELEMENT_TYPE_NUM]);
	virtual void clear(void);
	virtual const char* getContainerType(void) const;

	class _iteratorMA : public ElementContainer::_iterator
	{
		friend class ElementContainerMixedArray;
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
		_iteratorMA(void);
		virtual ~_iteratorMA(void);
		size_t index;
		const ElementContainerMixedArray* elementContainer;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::elementIdType id);
	virtual const_iterator find(kmb::elementIdType id) const;

	size_t getNodeTableSize(void) const;
	kmb::elementType getElementType(kmb::elementIdType id) const;
	// direct access
	// nodeOffset は考慮されないので注意
	// elementOffset は考慮される
	// 要素種類配列が事前に与えられている時だけ正しく動作する
	kmb::nodeIdType operator()(kmb::elementIdType elementId,kmb::idType localId) const;
	kmb::nodeIdType& operator()(kmb::elementIdType elementId,kmb::idType localId);
	void commit(kmb::elementIdType elementId);
private:
	static size_t getRequiredNodeTableSize(const size_t typeCounter[kmb::ELEMENT_TYPE_NUM]);
	static size_t getRequiredElementSize(const size_t typeCounter[kmb::ELEMENT_TYPE_NUM]);
};

}
