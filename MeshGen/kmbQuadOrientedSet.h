/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : QuadOrientedSet                                         #
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
#include <map>

namespace kmb{

class Quad;

class QuadOrientedSet : public ElementContainer
{
public:
	typedef std::multimap< kmb::nodeIdType, kmb::Quad* > NodeQuadMap;
	typedef std::pair< kmb::nodeIdType, kmb::Quad* > NodeQuadPair;
	QuadOrientedSet(void);
	virtual ~QuadOrientedSet(void);
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


	kmb::Quad* appendItem(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3);
	kmb::Quad* include(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3) const;
	bool removeItem(kmb::Quad* quad);

	int getElementNeighbor( const kmb::Quad* quad, kmb::Quad* neighbors[4] ) const;
	size_t getElementCountAroundNode(kmb::nodeIdType nodeId) const;


	kmb::Quad* getAdjacent( const kmb::Quad* quad, const int edgeNum, int &adjEdge ) const;

	kmb::Quad* getSharedThreeNodes(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, int& restIndex) const;


	NodeQuadMap::iterator beginNodeIterator(void);
	NodeQuadMap::const_iterator beginNodeIterator(void) const;

	NodeQuadMap::iterator findNodeIterator(kmb::nodeIdType nodeId);
	NodeQuadMap::const_iterator findNodeIterator(kmb::nodeIdType nodeId) const;

	NodeQuadMap::iterator nextNodeIterator(kmb::nodeIdType nodeId);
	NodeQuadMap::const_iterator nextNodeIterator(kmb::nodeIdType nodeId) const;

	NodeQuadMap::iterator endNodeIterator(void);
	NodeQuadMap::const_iterator endNodeIterator(void) const;
	size_t getNodeCount(void) const;

	class _iterator : public ElementContainer::_iterator
	{
		friend class QuadOrientedSet;
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
		NodeQuadMap::const_iterator tIter;
		NodeQuadMap::const_iterator endIter;
		kmb::elementIdType localId;
	};

	virtual kmb::ElementContainer::iterator begin(void);
	virtual kmb::ElementContainer::const_iterator begin(void) const;
	virtual kmb::ElementContainer::iterator find(kmb::elementIdType id);
	virtual kmb::ElementContainer::const_iterator find(kmb::elementIdType id) const;

protected:
	NodeQuadMap quads;
};

}
