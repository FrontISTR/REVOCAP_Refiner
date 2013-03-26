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
#include "MeshGen/kmbQuadOrientedSet.h"

#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbElementRelation.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::QuadOrientedSet::QuadOrientedSet(void)
{
}

kmb::QuadOrientedSet::~QuadOrientedSet(void)
{
	clear();
}

const char* kmb::QuadOrientedSet::CONTAINER_TYPE = "QuadOrderdSet";

kmb::elementIdType
kmb::QuadOrientedSet::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( etype == kmb::QUAD && nodes != NULL ){
		appendItem( nodes[0], nodes[1], nodes[2], nodes[3] );
		return 0;
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::QuadOrientedSet::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	if( etype == kmb::QUAD && nodes != NULL ){
		appendItem( nodes[0], nodes[1], nodes[2], nodes[3] );
		return 0;
	}
	return kmb::Element::nullElementId;
}

bool
kmb::QuadOrientedSet::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	return false;
}

bool
kmb::QuadOrientedSet::deleteElement(const kmb::elementIdType id)
{
	return false;
}

bool
kmb::QuadOrientedSet::includeElement(const kmb::elementIdType id) const
{
	return false;
}

kmb::elementIdType
kmb::QuadOrientedSet::getMaxId(void) const
{
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::QuadOrientedSet::getMinId(void) const
{
	return kmb::Element::nullElementId;
}

size_t
kmb::QuadOrientedSet::getCount(void) const
{
	return quads.size() / 4;
}

void
kmb::QuadOrientedSet::clear(void)
{
	NodeQuadMap::iterator tIter = quads.begin();
	while( tIter != quads.end() ){
		kmb::nodeIdType nodeId = tIter->first;
		kmb::Quad* quad = tIter->second;

		if( quad && quad->getCellId(0) <= nodeId && quad->getCellId(1) <= nodeId && quad->getCellId(2) <= nodeId && quad->getCellId(3) <= nodeId ){
			delete quad;
			quad = NULL;
		}
		tIter->second = NULL;
		++tIter;
	}
	quads.clear();
}

void
kmb::QuadOrientedSet::initialize(size_t size)
{
	clear();
}

kmb::Quad*
kmb::QuadOrientedSet::appendItem(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3)
{
	NodeQuadMap::iterator tIter = quads.lower_bound(n0);
	NodeQuadMap::iterator endIter = quads.upper_bound(n0);

	if( tIter == quads.end() ){
		kmb::Quad* quad = new kmb::Quad(n0,n1,n2,n3);
		quads.insert( NodeQuadPair(n0,quad) );
		quads.insert( NodeQuadPair(n1,quad) );
		quads.insert( NodeQuadPair(n2,quad) );
		quads.insert( NodeQuadPair(n3,quad) );
		++(typeCounter[ kmb::QUAD ]);
		return quad;
	}else{
		while( tIter != endIter ){
			kmb::Quad* other = tIter->second;
			if( other == NULL ){
				++tIter;
				continue;
			}
			switch( kmb::Quad::isCoincident( n0, n1, n2, n3,
				other->getCellId(0), other->getCellId(1), other->getCellId(2), other->getCellId(3) ) )
			{
			case 1:

				return NULL;
			case -1:
			{

				quads.erase( tIter );

				NodeQuadMap::iterator tIter1 = quads.lower_bound(n1);
				NodeQuadMap::iterator endIter1 = quads.upper_bound(n1);
				while( tIter1 != endIter1 )
				{
					kmb::Quad* other1 = tIter1->second;
					if( kmb::Quad::isCoincident( n0, n1, n2, n3,
						other1->getCellId(0), other1->getCellId(1), other1->getCellId(2), other1->getCellId(3) ) == -1 )
					{
						quads.erase( tIter1 );
						break;
					}
					++tIter1;
				}

				NodeQuadMap::iterator tIter2 = quads.lower_bound(n2);
				NodeQuadMap::iterator endIter2 = quads.upper_bound(n2);
				while( tIter2 != endIter2 )
				{
					kmb::Quad* other2 = tIter2->second;
					if( kmb::Quad::isCoincident( n0, n1, n2, n3,
						other2->getCellId(0), other2->getCellId(1), other2->getCellId(2), other2->getCellId(3) ) == -1 )
					{
						quads.erase( tIter2 );
						break;
					}
					++tIter2;
				}

				NodeQuadMap::iterator tIter3 = quads.lower_bound(n3);
				NodeQuadMap::iterator endIter3 = quads.upper_bound(n3);
				while( tIter3 != endIter3 )
				{
					kmb::Quad* other3 = tIter3->second;
					if( kmb::Quad::isCoincident( n0, n1, n2, n3,
						other3->getCellId(0), other3->getCellId(1), other3->getCellId(2), other3->getCellId(3) ) == -1 )
					{
						quads.erase( tIter3 );
						break;
					}
					++tIter3;
				}
				--(typeCounter[ kmb::QUAD ]);
				delete other;
				return NULL;
			}
			default:
				break;
			}
			++tIter;
		}
		kmb::Quad* quad = new kmb::Quad(n0,n1,n2,n3);
		quads.insert( NodeQuadPair(n0,quad) );
		quads.insert( NodeQuadPair(n1,quad) );
		quads.insert( NodeQuadPair(n2,quad) );
		quads.insert( NodeQuadPair(n3,quad) );
		++(typeCounter[ kmb::QUAD ]);
		return quad;
	}
}

kmb::Quad*
kmb::QuadOrientedSet::include(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3) const
{
	NodeQuadMap::const_iterator tIter = quads.lower_bound(n0);
	NodeQuadMap::const_iterator endIter = quads.upper_bound(n0);
	if( tIter == quads.end() ){
		return NULL;
	}else{
		while( tIter != endIter ){
			kmb::Quad* other = tIter->second;
			switch( kmb::Quad::isCoincident( n0, n1, n2, n3,
				other->getCellId(0), other->getCellId(1), other->getCellId(2), other->getCellId(3) ) )
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

bool
kmb::QuadOrientedSet::removeItem(kmb::Quad* quad)
{
	if( quad == NULL ){
		return false;
	}
	kmb::nodeIdType n0 = quad->getCellId(0);
	kmb::nodeIdType n1 = quad->getCellId(1);
	kmb::nodeIdType n2 = quad->getCellId(2);
	kmb::nodeIdType n3 = quad->getCellId(3);
	NodeQuadMap::iterator tIter = quads.lower_bound(n0);
	NodeQuadMap::iterator endIter = quads.upper_bound(n0);
	while( tIter != endIter ){
		kmb::Quad* other = tIter->second;
		if( other == NULL ){
			++tIter;
			continue;
		}
		switch( kmb::Quad::isCoincident( n0, n1, n2, n3,
			other->getCellId(0), other->getCellId(1), other->getCellId(2), other->getCellId(3) ) )
		{
		case 1:{

			quads.erase( tIter );

			NodeQuadMap::iterator tIter1 = quads.lower_bound(n1);
			NodeQuadMap::iterator endIter1 = quads.upper_bound(n1);
			while( tIter1 != endIter1 )
			{
				kmb::Quad* other1 = tIter1->second;
				if( kmb::Quad::isCoincident( n0, n1, n2, n3,
					other1->getCellId(0), other1->getCellId(1), other1->getCellId(2), other1->getCellId(3) ) == 1 )
				{
					quads.erase( tIter1 );
					break;
				}
				++tIter1;
			}

			NodeQuadMap::iterator tIter2 = quads.lower_bound(n2);
			NodeQuadMap::iterator endIter2 = quads.upper_bound(n2);
			while( tIter2 != endIter2 )
			{
				kmb::Quad* other2 = tIter2->second;
				if( kmb::Quad::isCoincident( n0, n1, n2, n3,
					other2->getCellId(0), other2->getCellId(1), other2->getCellId(2), other2->getCellId(3) ) == 1 )
				{
					quads.erase( tIter2 );
					break;
				}
				++tIter2;
			}

			NodeQuadMap::iterator tIter3 = quads.lower_bound(n3);
			NodeQuadMap::iterator endIter3 = quads.upper_bound(n3);
			while( tIter3 != endIter3 )
			{
				kmb::Quad* other3 = tIter3->second;
				if( kmb::Quad::isCoincident( n0, n1, n2, n3,
					other3->getCellId(0), other3->getCellId(1), other3->getCellId(2), other3->getCellId(3) ) == 1 )
				{
					quads.erase( tIter3 );
					break;
				}
				++tIter3;
			}
			--(typeCounter[ kmb::QUAD ]);
			delete other;
			return true;
		}
		default:
			break;
		}
		++tIter;
	}
	return false;
}

int
kmb::QuadOrientedSet::getElementNeighbor( const kmb::Quad* quad, kmb::Quad* neighbors[4] ) const
{
	if( quad == NULL || neighbors == NULL )
		return 0;

	int count = 0;






	int index = -1;
	int otherIndex = -1;
	for(int i=0;i<4;++i){
		neighbors[i] = NULL;

		kmb::nodeIdType nodeId = quad->getBoundaryCellId(i,0);

		std::pair< NodeQuadMap::const_iterator, NodeQuadMap::const_iterator >
			eIterPair = quads.equal_range( nodeId );
		NodeQuadMap::const_iterator eIter = eIterPair.first;
		while( eIter != eIterPair.second && neighbors[i] == NULL )
		{

			kmb::Quad* other = eIter->second;

			if( other && quad != other ){
				kmb::ElementRelation::relationType rel =
					kmb::ElementRelation::getRelation( *quad, index, *other, otherIndex );
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



kmb::Quad*
kmb::QuadOrientedSet::getAdjacent( const kmb::Quad* quad, const int edgeNum, int &adjEdge ) const
{
	if( quad == NULL )	return NULL;

	kmb::nodeIdType nodeId0 = quad->getBoundaryCellId(edgeNum,0);
	kmb::nodeIdType nodeId1 = quad->getBoundaryCellId(edgeNum,1);

	std::pair< NodeQuadMap::const_iterator, NodeQuadMap::const_iterator > eIterPair = quads.equal_range( nodeId0 );
	NodeQuadMap::const_iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{

		kmb::Quad* other = eIter->second;

		if( other && quad != other ){



			int index0 = other->indexOf( nodeId0 );

			int index1 = (index0+3)%4;
			if( other->getCellId( index1 ) == nodeId1 ){

				adjEdge = index1;
				return other;
			}
		}
		++eIter;
	}
	return NULL;
}

kmb::Quad*
kmb::QuadOrientedSet::getSharedThreeNodes(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, int& restIndex) const
{
	kmb::Quad* quad = NULL;
	NodeQuadMap::const_iterator tIter = quads.lower_bound(n0);
	NodeQuadMap::const_iterator endIter = quads.upper_bound(n0);
	while( tIter != endIter ){
		kmb::Quad* other = tIter->second;

		int indexes[4] = {-1,-1,-1,-1};
		int i1 = -1;
		int i2 = -1;
		if( 0 <= (i1=other->indexOf(n1)) && 0 <= (i2=other->indexOf(n2)) ){
			indexes[i1] = n1;
			indexes[i2] = n2;
			indexes[ other->indexOf(n0) ] = n0;
			for(int i=0;i<4;++i){
				if(indexes[i] == -1){
					restIndex = i;
				}
			}
			quad = other;
		}
		++tIter;
	}
	return quad;
}

size_t
kmb::QuadOrientedSet::getElementCountAroundNode(kmb::nodeIdType nodeId) const
{
	return this->quads.count(nodeId);
}

kmb::QuadOrientedSet::NodeQuadMap::iterator
kmb::QuadOrientedSet::beginNodeIterator(void)
{
	return this->quads.begin();
}

kmb::QuadOrientedSet::NodeQuadMap::const_iterator
kmb::QuadOrientedSet::beginNodeIterator(void) const
{
	return this->quads.begin();
}

kmb::QuadOrientedSet::NodeQuadMap::iterator
kmb::QuadOrientedSet::findNodeIterator(kmb::nodeIdType nodeId)
{
	return this->quads.find(nodeId);
}

kmb::QuadOrientedSet::NodeQuadMap::const_iterator
kmb::QuadOrientedSet::findNodeIterator(kmb::nodeIdType nodeId) const
{
	return this->quads.find(nodeId);
}

kmb::QuadOrientedSet::NodeQuadMap::iterator
kmb::QuadOrientedSet::nextNodeIterator(kmb::nodeIdType nodeId)
{
	return this->quads.upper_bound(nodeId);
}

kmb::QuadOrientedSet::NodeQuadMap::const_iterator
kmb::QuadOrientedSet::nextNodeIterator(kmb::nodeIdType nodeId) const
{
	return this->quads.upper_bound(nodeId);
}

kmb::QuadOrientedSet::NodeQuadMap::iterator
kmb::QuadOrientedSet::endNodeIterator(void)
{
	return this->quads.end();
}

kmb::QuadOrientedSet::NodeQuadMap::const_iterator
kmb::QuadOrientedSet::endNodeIterator(void) const
{
	return this->quads.end();
}

size_t
kmb::QuadOrientedSet::getNodeCount(void) const
{
	size_t nCount = 0;
	NodeQuadMap::const_iterator nIter = beginNodeIterator();
	NodeQuadMap::const_iterator endIter = endNodeIterator();
	while( nIter != endIter ){
		++nCount;
		nIter = nextNodeIterator( nIter->first );
	}
	return nCount;
}

kmb::ElementContainer::iterator
kmb::QuadOrientedSet::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::QuadOrientedSet::_iterator* _it = NULL;
	_it = new kmb::QuadOrientedSet::_iterator();;
	_it->tIter = this->quads.begin();
	_it->endIter = this->quads.end();
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::QuadOrientedSet::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::QuadOrientedSet::_iterator* _it = NULL;
	_it = new kmb::QuadOrientedSet::_iterator();;
	_it->tIter = this->quads.begin();
	_it->endIter = this->quads.end();
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::QuadOrientedSet::find(kmb::elementIdType id)
{
	return kmb::ElementContainer::iterator(NULL);
}

kmb::ElementContainer::const_iterator
kmb::QuadOrientedSet::find(kmb::elementIdType id) const
{
	return kmb::ElementContainer::const_iterator(NULL);
}

kmb::QuadOrientedSet::_iterator::_iterator(void)
: localId(0)
{
}

kmb::QuadOrientedSet::_iterator::~_iterator(void)
{
}

kmb::elementIdType
kmb::QuadOrientedSet::_iterator::getId(void) const
{
	return localId;
}

bool
kmb::QuadOrientedSet::_iterator::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	kmb::Quad* quad = tIter->second;
	if( quad ){
		etype = kmb::QUAD;
		for(int i=0;i<4;++i){
			nodes[i] = quad->getCellId(i);
		}
		return true;
	}else{
		etype = kmb::UNKNOWNTYPE;
		return false;
	}
}

kmb::Element*
kmb::QuadOrientedSet::_iterator::getElement(void)
{
	return tIter->second;
}

const kmb::Element*
kmb::QuadOrientedSet::_iterator::getElement(void) const
{
	return tIter->second;
}

kmb::elementType
kmb::QuadOrientedSet::_iterator::getType(void) const
{
	if( tIter->second ){
		return kmb::QUAD;
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

kmb::nodeIdType
kmb::QuadOrientedSet::_iterator::getCellId(int cellIndex) const
{
	kmb::Quad* quad = tIter->second;
	if( quad ){
		return quad->getCellId(cellIndex);
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::QuadOrientedSet::_iterator::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	kmb::Quad* quad = tIter->second;
	if( quad ){
		return quad->setCellId(cellIndex,nodeId);
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::QuadOrientedSet::_iterator::operator[](const int cellIndex) const
{
	kmb::Quad* quad = tIter->second;
	if( quad ){
		return (*quad)[cellIndex];
	}else{
		return kmb::nullNodeId;
	}
}

kmb::ElementContainer::_iterator*
kmb::QuadOrientedSet::_iterator::operator++(void)
{

	tIter++;
	while( tIter != endIter ){
		kmb::nodeIdType nodeId = tIter->first;
		kmb::Quad* quad = tIter->second;
		if( quad && quad->getCellId(0) >= nodeId && quad->getCellId(1) >= nodeId && quad->getCellId(2) >= nodeId && quad->getCellId(3) >= nodeId ){
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
kmb::QuadOrientedSet::_iterator::operator++(int n)
{
	++tIter;
	while( tIter != endIter ){
		kmb::nodeIdType nodeId = tIter->first;
		kmb::Quad* quad = tIter->second;
		if( quad && quad->getCellId(0) >= nodeId && quad->getCellId(1) >= nodeId && quad->getCellId(2) >= nodeId && quad->getCellId(3) >= nodeId ){
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
kmb::QuadOrientedSet::_iterator::clone(void)
{
	kmb::QuadOrientedSet::_iterator* obj = NULL;
	obj = new kmb::QuadOrientedSet::_iterator();
	obj->tIter = this->tIter;
	obj->endIter = this->endIter;
	return obj;
}
