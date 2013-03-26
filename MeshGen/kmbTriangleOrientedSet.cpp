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
#include "MeshGen/kmbTriangleOrientedSet.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbElementRelation.h"


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::TriangleOrientedSet::TriangleOrientedSet(void)
{
}

kmb::TriangleOrientedSet::~TriangleOrientedSet(void)
{
	clear();
}

const char* kmb::TriangleOrientedSet::CONTAINER_TYPE = "TriangleOrderdSet";

kmb::elementIdType
kmb::TriangleOrientedSet::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( etype == kmb::TRIANGLE && nodes != NULL ){
		appendItem( nodes[0], nodes[1], nodes[2] );
		return 0;
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::TriangleOrientedSet::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	if( etype == kmb::TRIANGLE && nodes != NULL ){
		appendItem( nodes[0], nodes[1], nodes[2] );
		return 0;
	}
	return kmb::Element::nullElementId;
}

bool
kmb::TriangleOrientedSet::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	return false;
}

bool
kmb::TriangleOrientedSet::deleteElement(const kmb::elementIdType id)
{
	return false;
}

bool
kmb::TriangleOrientedSet::includeElement(const kmb::elementIdType id) const
{
	return false;
}

kmb::elementIdType
kmb::TriangleOrientedSet::getMaxId(void) const
{
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::TriangleOrientedSet::getMinId(void) const
{
	return kmb::Element::nullElementId;
}

size_t
kmb::TriangleOrientedSet::getCount(void) const
{
	return triangles.size() / 3;
}

bool
kmb::TriangleOrientedSet::faster(kmb::Triangle& a,kmb::Triangle& b)
{
	return
		a[0] > b[0] || (
			a[0] == b[0] && (
				a[1] > b[1] || (
					a[1] == b[1] && a[2] > b[2])));
}

void
kmb::TriangleOrientedSet::clear(void)
{
	NodeTriMap::iterator tIter = triangles.begin();
	while( tIter != triangles.end() ){
		kmb::nodeIdType nodeId = tIter->first;
		kmb::Triangle* triangle = tIter->second;

		if( triangle && triangle->getCellId(0) <= nodeId && triangle->getCellId(1) <= nodeId && triangle->getCellId(2) <= nodeId ){
			delete triangle;
			triangle = NULL;
		}
		tIter->second = NULL;
		++tIter;
	}
	triangles.clear();
}

void
kmb::TriangleOrientedSet::initialize(size_t size)
{
	clear();
}

kmb::Triangle*
kmb::TriangleOrientedSet::appendItem(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2)
{
	NodeTriMap::iterator tIter = triangles.lower_bound(n0);
	NodeTriMap::iterator endIter = triangles.upper_bound(n0);
	if( tIter == triangles.end() ){
		kmb::Triangle* tri = new kmb::Triangle(n0,n1,n2);
		triangles.insert( NodeTriPair(n0,tri) );
		triangles.insert( NodeTriPair(n1,tri) );
		triangles.insert( NodeTriPair(n2,tri) );
		++(typeCounter[ kmb::TRIANGLE ]);
		return tri;
	}else{
		while( tIter != endIter ){
			kmb::Triangle* other = tIter->second;
			if( other == NULL ){
				++tIter;
				continue;
			}
			switch( kmb::Triangle::isCoincident( n0, n1, n2, other->getCellId(0), other->getCellId(1), other->getCellId(2) ) )
			{
			case 1:

				return NULL;
			case -1:
			{

				triangles.erase( tIter );

				NodeTriMap::iterator tIter1 = triangles.lower_bound(n1);
				NodeTriMap::iterator endIter1 = triangles.upper_bound(n1);
				while( tIter1 != endIter1 )
				{
					kmb::Triangle* other1 = tIter1->second;
					if( kmb::Triangle::isCoincident( n0, n1, n2, other1->getCellId(0), other1->getCellId(1), other1->getCellId(2) ) == -1 )
					{
						triangles.erase( tIter1 );
						break;
					}
					++tIter1;
				}

				NodeTriMap::iterator tIter2 = triangles.lower_bound(n2);
				NodeTriMap::iterator endIter2 = triangles.upper_bound(n2);
				while( tIter2 != endIter2 )
				{
					kmb::Triangle* other2 = tIter2->second;
					if( kmb::Triangle::isCoincident( n0, n1, n2, other2->getCellId(0), other2->getCellId(1), other2->getCellId(2) ) == -1 )
					{
						triangles.erase( tIter2 );
						break;
					}
					++tIter2;
				}
				--(typeCounter[ kmb::TRIANGLE ]);
				delete other;
				return NULL;
			}
			default:
				break;
			}
			++tIter;
		}
		kmb::Triangle* tri = new kmb::Triangle(n0,n1,n2);
		triangles.insert( NodeTriPair(n0,tri) );
		triangles.insert( NodeTriPair(n1,tri) );
		triangles.insert( NodeTriPair(n2,tri) );
		++(typeCounter[ kmb::TRIANGLE ]);
		return tri;
	}
}

kmb::Triangle*
kmb::TriangleOrientedSet::include(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2)
{
	NodeTriMap::iterator tIter = triangles.lower_bound(n0);
	NodeTriMap::iterator endIter = triangles.upper_bound(n0);
	if( tIter == triangles.end() ){
		return NULL;
	}else{
		while( tIter != endIter ){
			kmb::Triangle* other = tIter->second;
			switch( kmb::Triangle::isCoincident( n0, n1, n2, other->getCellId(0), other->getCellId(1), other->getCellId(2) ) )
			{
			case 1:
				return other;
			default:
				break;
			}
			++tIter;
		}
	}
	return NULL;
}

int
kmb::TriangleOrientedSet::getElementNeighbor( const kmb::Triangle* tri, kmb::Triangle* neighbors[3] ) const
{
	if( tri == NULL || neighbors == NULL )
		return 0;

	int count = 0;






	int index = -1;
	int otherIndex = -1;
	for(int i=0;i<3;++i){
		neighbors[i] = NULL;

		kmb::nodeIdType nodeId = tri->getBoundaryCellId(i,0);

		std::pair< NodeTriMap::const_iterator, NodeTriMap::const_iterator >
			eIterPair = triangles.equal_range( nodeId );
		NodeTriMap::const_iterator eIter = eIterPair.first;
		while( eIter != eIterPair.second && neighbors[i] == NULL )
		{

			kmb::Triangle* other = eIter->second;
			if( other && tri != other ){
				kmb::ElementRelation::relationType rel =
					kmb::ElementRelation::getRelation( *tri, index, *other, otherIndex );
				if( index == i &&
					( rel == kmb::ElementRelation::ADJACENT ||
					  rel == kmb::ElementRelation::ANTIADJACENT ) ){
					neighbors[i] = other;
					++count;
				}
			}
			++eIter;
		}
	}
	return count;
}

size_t
kmb::TriangleOrientedSet::getElementCountAroundNode(kmb::nodeIdType nodeId) const
{
	return this->triangles.count(nodeId);
}

size_t
kmb::TriangleOrientedSet::getAllEdges( std::set< std::pair< kmb::Triangle*, kmb::Triangle* > > &edges ) const
{
	int index = -1;
	int otherIndex = -1;
	for( kmb::TriangleOrientedSet::NodeTriMap::const_iterator nIter = this->triangles.begin();
		 nIter != this->triangles.end();
		 ++nIter)
	{
		kmb::nodeIdType nodeId = nIter->first;
		kmb::Triangle* tri = nIter->second;


		if( tri != NULL && (*tri)[0] == nodeId ){
			int len = tri->getNodeCount();
			for(int i=0;i<len;++i){
				std::pair< NodeTriMap::const_iterator, NodeTriMap::const_iterator > eIterPair = triangles.equal_range( (*tri)[i] );
				NodeTriMap::const_iterator eIter = eIterPair.first;
				while( eIter != eIterPair.second )
				{
					kmb::Triangle* other = eIter->second;
					if( other && tri != other ){
						kmb::ElementRelation::relationType rel =
							kmb::ElementRelation::getRelation( *tri, index, *other, otherIndex );
						if( ( rel == kmb::ElementRelation::ADJACENT || rel == kmb::ElementRelation::ANTIADJACENT ) &&
							kmb::TriangleOrientedSet::faster( *tri, *other ) )
						{
							edges.insert( std::pair< kmb::Triangle*, kmb::Triangle* >( tri, other ) );
						}
					}
					++eIter;
				}
			}
		}
	}
	return edges.size();
}

size_t
kmb::TriangleOrientedSet::getAllEdges( std::set< std::pair< kmb::nodeIdType, kmb::nodeIdType > > &edges ) const
{
	kmb::nodeIdType nodeId0 = kmb::nullNodeId;
	kmb::nodeIdType nodeId1 = kmb::nullNodeId;
	kmb::nodeIdType nodeId2 = kmb::nullNodeId;
	for( kmb::TriangleOrientedSet::NodeTriMap::const_iterator nIter = this->triangles.begin();
		 nIter != this->triangles.end();
		 ++nIter)
	{
		nodeId0 = nIter->first;
		kmb::Triangle* tri = nIter->second;


		if( tri != NULL && (*tri)[0] == nodeId0 ){
			nodeId1 = (*tri)[1];
			nodeId2 = (*tri)[2];
			if( nodeId0 < nodeId1 ){
				edges.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( nodeId0, nodeId1 ) );
			}else{
				edges.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( nodeId1, nodeId0 ) );
			}
			if( nodeId1 < nodeId2 ){
				edges.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( nodeId1, nodeId2 ) );
			}else{
				edges.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( nodeId2, nodeId1 ) );
			}
			if( nodeId2 < nodeId0 ){
				edges.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( nodeId2, nodeId0 ) );
			}else{
				edges.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( nodeId0, nodeId2 ) );
			}
		}
	}
	return edges.size();
}

size_t
kmb::TriangleOrientedSet::getTrianglePair
( const kmb::nodeIdType n0, const kmb::nodeIdType n1, kmb::Triangle* &tri0, int &index0, kmb::Triangle*& tri1, int &index1 ) const
{
	size_t count = 0;
	NodeTriMap::const_iterator tIter = triangles.lower_bound(n0);
	NodeTriMap::const_iterator endIter = triangles.upper_bound(n0);
	while( tIter != endIter ){
		kmb::Triangle* tri = tIter->second;
		if( tri ){
			int i0 = tri->indexOf( n0 );
			int i1 = tri->indexOf( n1 );
			if( i1 != -1 ){
				if( count == 0 ){
					tri0 = tri;
					index0 = tri->getFaceIndex( i0, i1 );
				}else if( count == 1 ){
					tri1 = tri;
					index1 = tri->getFaceIndex( i0, i1 );
				}
				++count;
			}
		}
		++tIter;
	}
	return count;
}



kmb::TriangleOrientedSet::NodeTriMap::iterator
kmb::TriangleOrientedSet::beginNodeIterator(void)
{
	return this->triangles.begin();
}

kmb::TriangleOrientedSet::NodeTriMap::const_iterator
kmb::TriangleOrientedSet::beginNodeIterator(void) const
{
	return this->triangles.begin();
}

kmb::TriangleOrientedSet::NodeTriMap::iterator
kmb::TriangleOrientedSet::nextNodeIterator(kmb::nodeIdType nodeId)
{
	return this->triangles.upper_bound(nodeId);
}

kmb::TriangleOrientedSet::NodeTriMap::const_iterator
kmb::TriangleOrientedSet::nextNodeIterator(kmb::nodeIdType nodeId) const
{
	return this->triangles.upper_bound(nodeId);
}

kmb::TriangleOrientedSet::NodeTriMap::iterator
kmb::TriangleOrientedSet::endNodeIterator(void)
{
	return this->triangles.end();
}

kmb::TriangleOrientedSet::NodeTriMap::const_iterator
kmb::TriangleOrientedSet::endNodeIterator(void) const
{
	return this->triangles.end();
}

size_t
kmb::TriangleOrientedSet::getNodeCount(void) const
{
	size_t nCount = 0;
	NodeTriMap::const_iterator nIter = beginNodeIterator();
	NodeTriMap::const_iterator endIter = endNodeIterator();
	while( nIter != endIter ){
		++nCount;
		nIter = nextNodeIterator( nIter->first );
	}
	return nCount;
}



kmb::ElementContainer::iterator
kmb::TriangleOrientedSet::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::TriangleOrientedSet::_iterator* _it = NULL;
	_it = new kmb::TriangleOrientedSet::_iterator();;
	_it->tIter = this->triangles.begin();
	_it->endIter = this->triangles.end();
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::TriangleOrientedSet::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::TriangleOrientedSet::_iterator* _it = NULL;
	_it = new kmb::TriangleOrientedSet::_iterator();;
	_it->tIter = this->triangles.begin();
	_it->endIter = this->triangles.end();
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::TriangleOrientedSet::find(kmb::elementIdType id)
{
	return kmb::ElementContainer::iterator(NULL);
}

kmb::ElementContainer::const_iterator
kmb::TriangleOrientedSet::find(kmb::elementIdType id) const
{
	return kmb::ElementContainer::const_iterator(NULL);
}

kmb::TriangleOrientedSet::_iterator::_iterator(void)
: localId(0)
{
}

kmb::TriangleOrientedSet::_iterator::~_iterator(void)
{
}

kmb::elementIdType
kmb::TriangleOrientedSet::_iterator::getId(void) const
{
	return localId;
}

bool
kmb::TriangleOrientedSet::_iterator::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	kmb::Triangle* tri = tIter->second;
	if( tri ){
		etype = kmb::TRIANGLE;
		for(int i=0;i<3;++i){
			nodes[i] = tri->getCellId(i);
		}
		return true;
	}else{
		etype = kmb::UNKNOWNTYPE;
		return false;
	}
}

kmb::Element*
kmb::TriangleOrientedSet::_iterator::getElement(void)
{
	return tIter->second;
}

const kmb::Element*
kmb::TriangleOrientedSet::_iterator::getElement(void) const
{
	return tIter->second;
}

kmb::elementType
kmb::TriangleOrientedSet::_iterator::getType(void) const
{
	if( tIter->second ){
		return kmb::TRIANGLE;
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::TriangleOrientedSet::_iterator::getCellId(int cellIndex) const
{
	kmb::Triangle* tri = tIter->second;
	if( tri ){
		return tri->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::TriangleOrientedSet::_iterator::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	kmb::Triangle* tri = tIter->second;
	if( tri ){
		return tri->setCellId(cellIndex,nodeId);
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::TriangleOrientedSet::_iterator::operator[](const int cellIndex) const
{
	kmb::Triangle* tri = tIter->second;
	if( tri ){
		return (*tri)[cellIndex];
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::_iterator*
kmb::TriangleOrientedSet::_iterator::operator++(void)
{


	tIter++;
	while( tIter != endIter ){
		kmb::nodeIdType nodeId = tIter->first;
		kmb::Triangle* tri = tIter->second;
		if( tri && tri->getCellId(0) >= nodeId && tri->getCellId(1) >= nodeId && tri->getCellId(2) >= nodeId ){
			break;
		}
		tIter++;
	}
	if( tIter!=endIter ){
		localId++;
		return this;
	}else{
		return NULL;
	}
}

kmb::ElementContainer::_iterator*
kmb::TriangleOrientedSet::_iterator::operator++(int n)
{
	++tIter;
	while( tIter != endIter ){
		kmb::nodeIdType nodeId = tIter->first;
		kmb::Triangle* tri = tIter->second;
		if( tri && tri->getCellId(0) >= nodeId && tri->getCellId(1) >= nodeId && tri->getCellId(2) >= nodeId ){
			break;
		}
		++tIter;
	}
	if( tIter!=endIter ){
		++localId;
		return this;
	}else{
		return NULL;
	}
}

kmb::ElementContainer::_iterator*
kmb::TriangleOrientedSet::_iterator::clone(void)
{
	kmb::TriangleOrientedSet::_iterator* obj = NULL;
	obj = new kmb::TriangleOrientedSet::_iterator();
	obj->tIter = this->tIter;
	obj->endIter = this->endIter;
	return obj;
}
