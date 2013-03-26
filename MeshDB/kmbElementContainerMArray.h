/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerMArray                                  #
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
#include "Common/kmbBLArray.h"

namespace kmb{

class ElementContainerMArray : public ElementContainer
{
public:
	static const char* CONTAINER_TYPE;
	ElementContainerMArray(size_t size);
	virtual ~ElementContainerMArray(void);
	virtual kmb::elementIdType addElement(kmb::Element* element);
	virtual kmb::elementIdType addElement(kmb::Element* element,const kmb::elementIdType id);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id);
	virtual bool getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const;
	virtual kmb::Element* eraseElement(kmb::elementIdType id);
	virtual bool deleteElement(const kmb::elementIdType id);
	virtual bool includeElement(const kmb::elementIdType id) const;
	virtual kmb::elementIdType getMaxId(void) const;
	virtual kmb::elementIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual void initialize(size_t size=0);

	virtual const char* getContainerType(void) const;

	class _iteratorMArray : public ElementContainer::_iterator
	{
		friend class ElementContainerMArray;
		friend class ElementContainer::iterator;
	public:
		virtual kmb::elementIdType getId(void) const;
		virtual kmb::Element* getElement(void);
		virtual const kmb::Element* getElement(void) const;
		virtual bool getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const;
		virtual kmb::elementType getType(void) const;
		virtual kmb::nodeIdType getCellId(int cellIndex) const;
		virtual bool setCellId(int cellIndex, kmb::nodeIdType nodeId);
		virtual kmb::nodeIdType operator[](const int i) const;
		virtual ElementContainer::_iterator* operator++(void);
		virtual ElementContainer::_iterator* operator++(int n);
		virtual ElementContainer::_iterator* clone(void);
	private:
		_iteratorMArray(void);
		virtual ~_iteratorMArray(void);
		ElementContainerMArray* elementContainer;
		BLArrayIndex aIndex;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::elementIdType id);
	virtual const_iterator find(kmb::elementIdType id) const;
protected:

	BLArrayPtr<kmb::Element> elementArray;
	BLArrayIndex aIndex;
	size_t count;
	kmb::Element* getElementPtr(kmb::elementIdType id) const;
};


}
