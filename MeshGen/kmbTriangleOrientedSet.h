/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriangleOrientedSet                                     #
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
#include <set>

namespace kmb{

class Triangle;

class TriangleOrientedSet : public ElementContainer
{
public:
	typedef std::multimap< kmb::nodeIdType, kmb::Triangle* > NodeTriMap;
	typedef std::pair< kmb::nodeIdType, kmb::Triangle* > NodeTriPair;

	TriangleOrientedSet(void);
	virtual ~TriangleOrientedSet(void);
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


	kmb::Triangle* appendItem(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2);
	kmb::Triangle* include(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2);

	int getElementNeighbor( const kmb::Triangle* tri, kmb::Triangle* neighbors[3] ) const;
	size_t getElementCountAroundNode(kmb::nodeIdType nodeId) const;

	size_t getTrianglePair( const kmb::nodeIdType n0, const kmb::nodeIdType n1, kmb::Triangle* &tri0, int &index0, kmb::Triangle*& tri1, int &index1 ) const;


	NodeTriMap::iterator beginNodeIterator(void);
	NodeTriMap::const_iterator beginNodeIterator(void) const;

	NodeTriMap::iterator nextNodeIterator(kmb::nodeIdType nodeId);
	NodeTriMap::const_iterator nextNodeIterator(kmb::nodeIdType nodeId) const;
	NodeTriMap::iterator endNodeIterator(void);
	NodeTriMap::const_iterator endNodeIterator(void) const;
	size_t getNodeCount(void) const;


	size_t getAllEdges( std::set< std::pair< kmb::Triangle*, kmb::Triangle* > > &edges ) const;

	size_t getAllEdges( std::set< std::pair< kmb::nodeIdType, kmb::nodeIdType > > &edges ) const;

	class _iterator : public ElementContainer::_iterator
	{
		friend class TriangleOrientedSet;
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
		NodeTriMap::const_iterator tIter;
		NodeTriMap::const_iterator endIter;
		kmb::elementIdType localId;
	};

	virtual kmb::ElementContainer::iterator begin(void);
	virtual kmb::ElementContainer::const_iterator begin(void) const;
	virtual kmb::ElementContainer::iterator find(kmb::elementIdType id);
	virtual kmb::ElementContainer::const_iterator find(kmb::elementIdType id) const;

protected:
	NodeTriMap triangles;

	static bool faster(kmb::Triangle& a,kmb::Triangle& b);
};

}
