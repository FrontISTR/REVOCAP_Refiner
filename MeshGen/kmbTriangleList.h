/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriangleList                                            #
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
#pragma once

#include "MeshDB/kmbElementContainer.h"

namespace kmb{

class TriangleItem;

class TriangleList
	: public ElementContainer
{
public:
	TriangleList(void);
	virtual ~TriangleList(void);
	static const char* CONTAINER_TYPE;
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

	virtual const char* getContainerType(void) const{
		return CONTAINER_TYPE;
	};


	bool deleteItem(TriangleItem* tri);
	bool appendItem(TriangleItem* tri);
	size_t appendChain(TriangleItem* head);


	TriangleItem* eraseItem(TriangleItem* tri);

	class _iterator : public ElementContainer::_iterator
	{
		friend class TriangleList;
	public:
		virtual kmb::elementIdType getId(void) const;
		virtual bool getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const;
		virtual kmb::Element* getElement(void);
		virtual const kmb::Element* getElement(void) const;
		virtual kmb::elementType getType(void) const;
		virtual kmb::nodeIdType getCellId(int cellIndex) const;
		virtual bool setCellId(int cellIndex, kmb::nodeIdType nodeId);
		virtual kmb::nodeIdType operator[](const int cellIndex) const;
		virtual ElementContainer::_iterator* operator++(void);
		virtual ElementContainer::_iterator* operator++(int n);
		virtual ElementContainer::_iterator* clone(void);
	private:
		_iterator(void);
		virtual ~_iterator(void);
		TriangleItem* tri;
		kmb::elementIdType localId;
	};

	virtual kmb::ElementContainer::iterator begin(void);
	virtual kmb::ElementContainer::const_iterator begin(void) const;
	virtual kmb::ElementContainer::iterator find(kmb::elementIdType id);
	virtual kmb::ElementContainer::const_iterator find(kmb::elementIdType id) const;

protected:
	TriangleItem* first;
	TriangleItem* last;
	size_t elementCount;
};

}
