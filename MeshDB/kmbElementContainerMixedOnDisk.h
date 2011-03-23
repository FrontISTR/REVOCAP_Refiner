/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : ElementContainerMixedOnDisk                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/*
 * バイナリファイル上にデータを格納してストリームでデータを走査する
 *
 * バイナリファイルは3種類用意する
 * nodeTable  要素の節点番号を順に並べた配列
 * etypeTable 要素の種類を並べた配列
 * indexTable 要素の区切りの配列の添え字を並べた配列
 */
#pragma once
#include "MeshDB/kmbElementContainer.h"
#include "Geometry/kmb_ContainerOnDisk.h"

namespace kmb{

class ElementContainerMixedOnDisk : public kmb::ElementContainer
{
public:
	static const char* CONTAINER_TYPE;
private:
	unsigned long size;
	kmb::ContainerOnDisk nodeTable;
	kmb::ContainerOnDisk etypeTable;
	kmb::ContainerOnDisk indexTable;
	size_t index;
	size_t nindex;
	size_t count;
	static const char HEADER_STRING[80];
public:
	ElementContainerMixedOnDisk(unsigned long size,const char* filename=NULL);
	virtual ~ElementContainerMixedOnDisk(void);
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
	virtual const char* getContainerType(void) const;

	class _iteratorMOD : public ElementContainer::_iterator
	{
		friend class ElementContainerMixedOnDisk;
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
		_iteratorMOD(void);
		virtual ~_iteratorMOD(void);
		size_t index;
		const ElementContainerMixedOnDisk* elementContainer;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::elementIdType id);
	virtual const_iterator find(kmb::elementIdType id) const;
};

}
