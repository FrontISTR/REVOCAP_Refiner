/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerTriangleArray                           #
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
 * 3*size 個の配列を確保して、順に入れていく
 *
 * ElementContainerNArray との違いは、OpenGL の DrawElements で
 * 使うために節点の型を unsigned int にしているところ
 *
 * また、ElementContainerNArray はクラスの外で確保された
 * 節点配列をラップして使うこともできるが
 * TriangleArray はこのクラスでメモリの確保と解放を行う
 */

#pragma once
#include "MeshDB/kmbElementContainer.h"

namespace kmb{

class ElementContainerTriangleArray : public ElementContainer
{
public:
	static const char* CONTAINER_TYPE;
	ElementContainerTriangleArray(size_t size);
	virtual ~ElementContainerTriangleArray(void);
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

	class _iteratorTA : public ElementContainer::_iterator
	{
		friend class ElementContainerTriangleArray;
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
		_iteratorTA(void);
		virtual ~_iteratorTA(void);
		size_t index;
		const ElementContainerTriangleArray* elementContainer;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::elementIdType id);
	virtual const_iterator find(kmb::elementIdType id) const;

	const unsigned int* getNodeTable(void) const { return nodeTable; }
protected:
	size_t index;
	size_t size;

	unsigned int* nodeTable;

	static unsigned int unsignedNullNodeId;
};

}
