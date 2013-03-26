/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeNeighborInfo                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbQuad.h"

kmb::NodeNeighborInfo::NodeNeighborInfo(void)
: coboundaries(), admitAnti(true)
{
}

kmb::NodeNeighborInfo::~NodeNeighborInfo(void)
{
}

void
kmb::NodeNeighborInfo::clear(void)
{
	coboundaries.clear();
}

bool
kmb::NodeNeighborInfo::getIgnoreOrientation(void) const
{
	return admitAnti;
}

void
kmb::NodeNeighborInfo::setIgnoreOrientation(bool f)
{
	admitAnti = f;
}

bool
kmb::NodeNeighborInfo::append( kmb::nodeIdType nodeID, kmb::elementIdType elementID )
{

	std::pair< NodeNeighbor::iterator, NodeNeighbor::iterator >
		bIterPair = coboundaries.equal_range( nodeID );
	NodeNeighbor::iterator nIter = bIterPair.first;
	while( nIter != bIterPair.second )
	{
		if(nIter->second == elementID){

			return false;
		}
		++nIter;
	}
	coboundaries.insert( std::pair< kmb::nodeIdType, kmb::elementIdType >(nodeID,elementID) );
	return true;
}

bool
kmb::NodeNeighborInfo::erase( kmb::nodeIdType nodeID, kmb::elementIdType elementID )
{

	std::pair< NodeNeighbor::iterator, NodeNeighbor::iterator >
		bIterPair = coboundaries.equal_range( nodeID );
	NodeNeighbor::iterator nIter = bIterPair.first;
	while( nIter != bIterPair.second )
	{
		if(nIter->second == elementID){

			coboundaries.erase( nIter );
			return true;
		}
		++nIter;
	}
	return false;
}

bool
kmb::NodeNeighborInfo::appendCoboundary( kmb::elementIdType elementId, const kmb::ElementBase &element )
{

	const unsigned int len = element.getNodeCount();
	for(unsigned int i=0;i<len;++i){
		kmb::nodeIdType nodeId = element.getCellId(i);
		this->append(nodeId,elementId);
	}
	return true;
}

bool
kmb::NodeNeighborInfo::deleteCoboundary( kmb::elementIdType elementId, const kmb::ElementBase &element )
{

	const unsigned int len = element.getNodeCount();
	for(unsigned int i=0;i<len;++i){
		kmb::nodeIdType nodeId = element.getCellId(i);
		this->erase(nodeId,elementId);
	}
	return true;
}

bool
kmb::NodeNeighborInfo::appendCoboundary( const kmb::ElementContainer* elements )
{
	if( elements == NULL )
		return false;
	kmb::ElementContainer::const_iterator eIter = elements->begin();
	while( !eIter.isFinished() )
	{
		const kmb::elementIdType id = eIter.getId();
		this->appendCoboundary( id, eIter );
		++eIter;
	}
	return true;
}

bool
kmb::NodeNeighborInfo::appendCoboundary( const kmb::MeshData* mesh )
{
	if( mesh == NULL )
		return false;
	const kmb::bodyIdType bodyCount = mesh->getBodyCount();
	for( kmb::bodyIdType id = 0; id < bodyCount; ++id){
		const kmb::ElementContainer* body = mesh->getBodyPtr( id );
		appendCoboundary( body );
	}
	return true;
}


bool
kmb::NodeNeighborInfo::appendCoboundary( const kmb::DataBindings* data, const kmb::MeshData* mesh )
{
	if( mesh == NULL || data == NULL )
		return false;
	bool retVal = false;
	kmb::bodyIdType bodyId = data->getTargetBodyId();
	switch( data->getBindingMode() )
	{
	case kmb::DataBindings::ElementVariable:
	case kmb::DataBindings::ElementGroup:
		{
			kmb::DataBindings::const_iterator eIter = data->begin();
			kmb::DataBindings::const_iterator eEnd = data->end();
			while( eIter != eEnd ){
				kmb::elementIdType elementId = static_cast< kmb::elementIdType >( eIter.getId() );
				kmb::ElementContainer::const_iterator element = mesh->findElement(elementId,bodyId);
				if( !element.isFinished() ){
					appendCoboundary( elementId, element );
				}
				++eIter;
			}
			retVal = true;
			break;
		}
	default:
		break;
	}
	return retVal;
}

bool
kmb::NodeNeighborInfo::appendCoboundary( const kmb::DataBindings* data, const kmb::ElementContainer* body )
{
	if( body == NULL || data == NULL )
		return false;
	bool retVal = false;
	switch( data->getBindingMode() )
	{
	case kmb::DataBindings::ElementVariable:
	case kmb::DataBindings::ElementGroup:
		{
			kmb::DataBindings::const_iterator eIter = data->begin();
			kmb::DataBindings::const_iterator eEnd = data->end();
			while( eIter != eEnd ){
				kmb::elementIdType elementId = static_cast< kmb::elementIdType >( eIter.getId() );
				kmb::ElementContainer::const_iterator element = body->find(elementId);
				if( !element.isFinished() ){
					appendCoboundary( elementId, element );
				}
				++eIter;
			}
			retVal = true;
			break;
		}
	default:
		break;
	}
	return retVal;
}

int
kmb::NodeNeighborInfo::getNeighborElements( const kmb::elementIdType elementID, kmb::elementIdType* neighbors, const kmb::ElementContainer* elements ) const
{
	if( elements == NULL )
		return 0;

	kmb::ElementContainer::const_iterator element = elements->find(elementID);

	int count = 0;
	if( element == NULL )
		return count;





	const int boundaryNum = element.getBoundaryCount();


	if( element.getDimension() == 1 )
	{
		for(int i=0;i<2;++i)
		{
			neighbors[i] = kmb::Element::nullElementId;
			const kmb::nodeIdType nodeID = element.getCellId(i);

			std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighbor::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count] == kmb::Element::nullElementId )
			{
				kmb::elementIdType coElemId = eIter->second;
				++eIter;

				if( elementID != coElemId ){
					neighbors[count] = coElemId;
					++count;
				}
			}
		}
	}else if( element.getDimension() == 2 ){
		int index = -1;
		int otherIndex = -1;
		for(int i=0;i<boundaryNum;++i){
			neighbors[i] = kmb::Element::nullElementId;

			kmb::nodeIdType nodeID = element.getBoundaryCellId(i,0);

			std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighbor::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count] == kmb::Element::nullElementId )
			{
				kmb::elementIdType coElemId = eIter->second;
				++eIter;

				if( elementID != coElemId ){
					kmb::ElementContainer::const_iterator coElement = elements->find(coElemId);
					if( !coElement.isFinished() ){
						kmb::ElementRelation::relationType rel =
							kmb::ElementRelation::getRelation( element, index, coElement, otherIndex );
						if( index == i &&
							( rel == kmb::ElementRelation::ADJACENT ||
							  (admitAnti && rel == kmb::ElementRelation::ANTIADJACENT) ) ){
							neighbors[count] = coElemId;
							++count;
						}
					}
				}
			}
		}
	}else if( element.getDimension() == 3 ){
		int index = -1;
		int otherIndex = -1;
		for(int i=0;i<boundaryNum;++i){
			neighbors[i] = kmb::Element::nullElementId;

			kmb::nodeIdType nodeID = element.getBoundaryCellId(i,0);

			std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighbor::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count] == kmb::Element::nullElementId )
			{
				kmb::elementIdType coElemId = eIter->second;
				++eIter;

				if( elementID != coElemId ){
					kmb::ElementContainer::const_iterator coElement = elements->find(coElemId);
					if( !coElement.isFinished() ){
						kmb::ElementRelation::relationType rel =
							kmb::ElementRelation::getRelation( element, index, coElement, otherIndex );
						if( index == i &&
							( rel == kmb::ElementRelation::ADJACENT ||
							  (admitAnti && rel == kmb::ElementRelation::ANTIADJACENT) ) ){
							neighbors[count] = coElemId;
							++count;
						}
					}
				}
			}
		}
	}
	return count;
}

int
kmb::NodeNeighborInfo::getNeighborElements( kmb::elementIdType elementID, kmb::elementIdType *neighbors, kmb::MeshData* mesh ) const
{
	if( mesh == NULL )
		return 0;




	kmb::ElementContainer::const_iterator element = mesh->findElement(elementID);

	int count = 0;
	if( element.isFinished() )
		return count;





	const int boundaryNum = element.getBoundaryCount();


	if( element.getDimension() == 1 )
	{
		for(int i=0;i<2;++i)
		{
			neighbors[i] = kmb::Element::nullElementId;
			const kmb::nodeIdType nodeID = element.getCellId(i);

			std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighbor::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count] == kmb::Element::nullElementId )
			{
				kmb::elementIdType coElemId = eIter->second;
				++eIter;

				if( elementID != coElemId ){
					neighbors[count] = coElemId;
					++count;
				}
			}
		}
	}else if( element.getDimension() == 2 ){
		int index = -1;
		int otherIndex = -1;
		for(int i=0;i<boundaryNum;++i){
			neighbors[i] = kmb::Element::nullElementId;

			kmb::nodeIdType nodeID = element.getBoundaryCellId(i,0);

			std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighbor::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count] == kmb::Element::nullElementId )
			{
				kmb::elementIdType coElemId = eIter->second;
				++eIter;

				if( elementID != coElemId ){
					kmb::ElementContainer::const_iterator coElement = mesh->findElement(coElemId);
					if( coElement != NULL ){
						kmb::ElementRelation::relationType rel =
							kmb::ElementRelation::getRelation( element, index, coElement, otherIndex );
						if( index == i &&
							( rel == kmb::ElementRelation::ADJACENT ||
							  (admitAnti && rel == kmb::ElementRelation::ANTIADJACENT) ) ){
							neighbors[count] = coElemId;
							++count;
						}
					}
				}
			}
		}
	}else if( element.getDimension() == 3 ){
		int index = -1;
		int otherIndex = -1;
		for(int i=0;i<boundaryNum;++i){
			neighbors[i] = kmb::Element::nullElementId;

			kmb::nodeIdType nodeID = element.getBoundaryCellId(i,0);

			std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighbor::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count] == kmb::Element::nullElementId )
			{
				kmb::elementIdType coElemId = eIter->second;
				++eIter;

				if( elementID != coElemId ){
					kmb::ElementContainer::const_iterator coElement = mesh->findElement(coElemId);
					if( !coElement.isFinished() ){
						kmb::ElementRelation::relationType rel =
							kmb::ElementRelation::getRelation( element, index, coElement, otherIndex );
						if( index == i &&
							( rel == kmb::ElementRelation::ADJACENT ||
							  (admitAnti && rel == kmb::ElementRelation::ANTIADJACENT) ) ){
							neighbors[count] = coElemId;
							++count;
						}
					}
				}
			}
		}
	}
	return count;
}

void
kmb::NodeNeighborInfo::getNodeNeighbor
( kmb::nodeIdType nodeID, std::set<kmb::nodeIdType> &neighbors, const kmb::ElementContainer* elements )
{
	if( elements == NULL )
		return;

	std::pair< NodeNeighbor::iterator, NodeNeighbor::iterator >
		eIterPair = coboundaries.equal_range( nodeID );
	NodeNeighbor::iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{
		kmb::elementIdType elementID = eIter->second;
		kmb::ElementContainer::const_iterator element = elements->find(elementID);
		if( !element.isFinished() ){
			const int len = element.getNodeCount();

			int nodeIndex = element.indexOf(nodeID);
			for(int i=0;i<len;++i){
				if( i != nodeIndex &&
					element.isConnected(i,nodeIndex) != 0 )
				{
					neighbors.insert(element.getCellId(i));
				}
			}
		}
		++eIter;
	}
}

void
kmb::NodeNeighborInfo::getNodeNeighbor
( kmb::nodeIdType nodeID, std::set<kmb::nodeIdType> &neighbors, kmb::MeshData* mesh )
{
	if( mesh == NULL )
		return;


	std::pair< NodeNeighbor::iterator, NodeNeighbor::iterator >
		eIterPair = coboundaries.equal_range( nodeID );
	NodeNeighbor::iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{
		kmb::elementIdType elementID = eIter->second;
		kmb::ElementContainer::const_iterator element = mesh->findElement(elementID);
		if( !element.isFinished() ){
			const int len = element.getNodeCount();

			int nodeIndex = element.indexOf(nodeID);
			for(int i=0;i<len;++i){
				if( i != nodeIndex &&
					element.isConnected(i,nodeIndex) != 0 )
				{
					neighbors.insert(element.getCellId(i));
				}
			}
		}
		++eIter;
	}
}

void
kmb::NodeNeighborInfo::getCrownNodeNeighbor
( kmb::nodeIdType nodeID, std::vector<kmb::nodeIdType> &neighbors, const kmb::ElementContainer* elements )
{
	if( elements == NULL ||
			!elements->isUniqueDim(2) ){
		return;
	}

	kmb::ElementContainer::const_iterator element;
	kmb::nodeIdType nextNodeId = kmb::nullNodeId;
	{
		kmb::NodeNeighbor::iterator eIter = beginIteratorAt(nodeID);
		kmb::NodeNeighbor::iterator end = endIteratorAt(nodeID);
		if( eIter == end ){
			return;
		}
		element = elements->find( eIter->second );
		nextNodeId = element.nextNode(nodeID);
		neighbors.push_back( nextNodeId );
	}

	while( !element.isFinished() ){
		element = elements->end();
		kmb::NodeNeighbor::iterator eIter = beginIteratorAt(nodeID);
		kmb::NodeNeighbor::iterator end = endIteratorAt(nodeID);
		while( eIter != end ){
			kmb::ElementContainer::const_iterator nextElement = elements->find( eIter->second );
			++eIter;
			if( !nextElement.isFinished() &&
					nextNodeId == nextElement.previousNode(nodeID) ){
				element = nextElement;
				nextNodeId = element.nextNode(nodeID);
				break;
			}
		}
		if( !element.isFinished() ){

			for(unsigned int i=0;i<neighbors.size();++i){
				if( nextNodeId == neighbors[i] ){
					nextNodeId = kmb::nullNodeId;
					if( i > 0 ){

						for(unsigned int j=0;j<neighbors.size()-i;++j){
							neighbors[j] = neighbors[j+i];
						}
						for(unsigned int j=0;j<i;++j){
							neighbors.pop_back();
						}
					}
					break;
				}
			}
			if( nextNodeId != kmb::nullNodeId ){
				neighbors.push_back( nextNodeId );
			}else{
				element = elements->end();
				break;
			}
		}
	}
}

bool
kmb::NodeNeighborInfo::isConnected
( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, const kmb::ElementContainer* elements ) const
{
	bool retVal = false;
	if( elements == NULL )
		return retVal;


	std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
		eIterPair = coboundaries.equal_range( nodeId0 );
	NodeNeighbor::const_iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{
		kmb::elementIdType elementId = eIter->second;
		kmb::ElementContainer::const_iterator element = elements->find(elementId);
		if( !element.isFinished() )
		{
			int ind0 = element.indexOf(nodeId0);
			int ind1 = element.indexOf(nodeId1);
			if( element.isConnected(ind0,ind1) ){
				retVal = true;
				break;
			}
		}
		++eIter;
	}
	return retVal;
}

bool
kmb::NodeNeighborInfo::isConnected
( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, kmb::MeshData* mesh ) const
{
	bool retVal = false;
	if( mesh == NULL )
		return retVal;


	std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
		eIterPair = coboundaries.equal_range( nodeId0 );
	NodeNeighbor::const_iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{
		kmb::elementIdType elementId = eIter->second;
		kmb::ElementContainer::const_iterator element = mesh->findElement(elementId);
		if( !element.isFinished() )
		{
			int ind0 = element.indexOf(nodeId0);
			int ind1 = element.indexOf(nodeId1);
			if( element.isConnected(ind0,ind1) ){
				retVal = true;
				break;
			}
		}
		++eIter;
	}
	return retVal;
}

size_t
kmb::NodeNeighborInfo::getElementCountAroundNode(nodeIdType nodeId) const
{
	return this->coboundaries.count(nodeId);
}

size_t
kmb::NodeNeighborInfo::getSize(void) const
{
	return this->coboundaries.size();
}

kmb::NodeNeighbor::iterator
kmb::NodeNeighborInfo::beginNodeIterator(void)
{
	return this->coboundaries.begin();
}

kmb::NodeNeighbor::iterator
kmb::NodeNeighborInfo::endNodeIterator(void)
{
	return this->coboundaries.end();
}

void
kmb::NodeNeighborInfo::getSurroundingElements(kmb::nodeIdType nodeId, std::vector<kmb::elementIdType> &elements) const
{

	std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator > eIterPair = coboundaries.equal_range( nodeId );
	NodeNeighbor::const_iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{
		elements.push_back( eIter->second );
		++eIter;
	}
}

void
kmb::NodeNeighborInfo::getSurroundingElementsOf2Node(kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, std::vector<kmb::elementIdType> &elements) const
{
	std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator > eIterPair0 = coboundaries.equal_range( nodeId0 );
	std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator > eIterPair1 = coboundaries.equal_range( nodeId1 );
	NodeNeighbor::const_iterator eIter0 = eIterPair0.first;
	while( eIter0 != eIterPair0.second )
	{
		NodeNeighbor::const_iterator eIter1 = eIterPair1.first;
		while( eIter1 != eIterPair1.second )
		{
			if( eIter0->second == eIter1->second ){
				elements.push_back( eIter0->second );
			}
			++eIter1;
		}
		++eIter0;
	}
}

kmb::elementIdType
kmb::NodeNeighborInfo::getNextElementAroundNode( kmb::ElementContainer* triangles, kmb::nodeIdType nodeId, kmb::elementIdType elemId, bool clockwise) const
{
	kmb::elementIdType nextElementId = kmb::Element::nullElementId;
	if( triangles == NULL || nodeId == kmb::nullNodeId || elemId == kmb::Element::nullElementId ||
		!triangles->isUniqueDim(2) ){
		return nextElementId;
	}
	kmb::ElementContainer::iterator element = triangles->find( elemId );


	kmb::NodeNeighbor::const_iterator eIter = beginIteratorAt(nodeId);
	kmb::NodeNeighbor::const_iterator end = endIteratorAt(nodeId);
	while( eIter != end && nextElementId == kmb::Element::nullElementId ){
		kmb::ElementContainer::iterator neighborElement = triangles->find( eIter->second );
		if( !neighborElement.isFinished() ){
			int index = -1;
			int otherIndex = -1;
			kmb::ElementRelation::relationType rel =
				kmb::ElementRelation::getRelation( element, index, neighborElement, otherIndex );
			if( rel == kmb::ElementRelation::ADJACENT ){

				if( clockwise && element.getType() == kmb::TRIANGLE &&
					element.getCellId( kmb::Triangle::faceTable[index][0] ) == nodeId )
				{
					nextElementId = eIter->second;
				}
				else if( !clockwise && element.getType() == kmb::TRIANGLE &&
					element.getCellId( kmb::Triangle::faceTable[index][1] ) == nodeId )
				{
					nextElementId = eIter->second;
				}
				else if( clockwise && element.getType() == kmb::QUAD &&
					element.getCellId( kmb::Quad::faceTable[index][0] ) == nodeId )
				{
					nextElementId = eIter->second;
				}
				else if( !clockwise && element.getType() == kmb::QUAD &&
					element.getCellId( kmb::Quad::faceTable[index][1] ) == nodeId )
				{
					nextElementId = eIter->second;
				}
			}
		}
		++eIter;
	}
	return nextElementId;
}

void
kmb::NodeNeighborInfo::getEndsOfEdge( const kmb::ElementContainer* edges, kmb::nodeIdType& firstID, kmb::nodeIdType& endID ) const
{
	if( edges == NULL || !edges->isUniqueType( kmb::SEGMENT ) )
	{
		firstID = kmb::nullNodeId;
		endID = kmb::nullNodeId;
	}

	int count = 0;


	kmb::ElementContainer::const_iterator eIter = edges->begin();
	while( !eIter.isFinished() ){
		kmb::nodeIdType node0 = eIter.getCellId(0);
		kmb::nodeIdType node1 = eIter.getCellId(1);
		if( getElementCountAroundNode(node0) == 1 ){
			++count;
			firstID = node0;
		}
		if( getElementCountAroundNode(node1) == 1 ){
			++count;
			endID = node1;
		}
		++eIter;
	}

	if( count != 2 )
	{
		firstID = kmb::nullNodeId;
		endID = kmb::nullNodeId;
	}
}

int
kmb::NodeNeighborInfo::getElementBoundary( const kmb::ElementBase &element, kmb::elementIdType *boundaries, const kmb::ElementContainer* elements ) const
{
	int count = 0;
	if( elements == NULL ){
		return count;
	}
	const int boundaryNum = element.getBoundaryCount();
	for(int i=0;i<boundaryNum;++i){
		boundaries[i] = getElementFace( element, i, elements );
		if( boundaries[i] != kmb::Element::nullElementId ){
			++count;
		}
	}
	return count;
}

kmb::elementIdType
kmb::NodeNeighborInfo::getElementFace( const kmb::ElementBase &element, kmb::idType faceId, const kmb::ElementContainer* elements ) const
{
	if( elements == NULL ){
		return kmb::Element::nullElementId;
	}
	int index = -1;
	int otherIndex = -1;

	kmb::nodeIdType nodeID = element.getBoundaryCellId(faceId,0);

	std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator > eIterPair = coboundaries.equal_range( nodeID );
	NodeNeighbor::const_iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{
		kmb::elementIdType coElemId = eIter->second;
		++eIter;

		kmb::ElementContainer::const_iterator coElement = elements->find(coElemId);
		if( !coElement.isFinished() ){
			kmb::ElementRelation::relationType rel =
				kmb::ElementRelation::getRelation( element, index, coElement, otherIndex );
			if( index == faceId &&
				( rel == kmb::ElementRelation::COBOUNDARY ||
				  (admitAnti && rel == kmb::ElementRelation::ANTICOBOUNDARY) ) ){
				return coElemId;
			}
		}
	}
	return kmb::Element::nullElementId;
}

bool
kmb::NodeNeighborInfo::getFace( const kmb::ElementBase &element, kmb::Face &face, const kmb::ElementContainer* elements ) const
{
	if( elements == NULL ){
		return false;
	}
	int index = -1;
	int otherIndex = -1;

	kmb::nodeIdType nodeID = element.getCellId(0);

	std::pair< NodeNeighbor::const_iterator, NodeNeighbor::const_iterator >
		eIterPair = coboundaries.equal_range( nodeID );
	NodeNeighbor::const_iterator eIter = eIterPair.first;
	while( eIter != eIterPair.second )
	{
		kmb::elementIdType coElemId = eIter->second;
		++eIter;

		kmb::ElementContainer::const_iterator coElement = elements->find(coElemId);
		if( !coElement.isFinished() ){
			kmb::ElementRelation::relationType rel =
				kmb::ElementRelation::getRelation( coElement, index, element, otherIndex );
			if( rel == kmb::ElementRelation::COBOUNDARY ){
				face.setId( coElemId, index );
				return true;
			}
		}
	}
	return false;
}

bool
kmb::NodeNeighborInfo::getAdjacentFace( const kmb::Face &face, kmb::Face &adj, const kmb::ElementContainer* elements ) const
{
	if( elements == NULL ){
		return false;
	}
	kmb::ElementContainer::const_iterator element = elements->find( face.getElementId() );
	if( !element.isFinished() ){
		int index = -1;
		int otherIndex = -1;

		kmb::nodeIdType nodeId = element.getBoundaryCellId( face.getLocalFaceId(), 0 );

		std::pair< kmb::NodeNeighbor::const_iterator, kmb::NodeNeighbor::const_iterator >
			eIterPair = coboundaries.equal_range( nodeId );
		kmb::NodeNeighbor::const_iterator eIter = eIterPair.first;
		while( eIter != eIterPair.second )
		{
			kmb::elementIdType coElemId = eIter->second;
			++eIter;

			kmb::ElementContainer::const_iterator coElement = elements->find(coElemId);
			if( !coElement.isFinished() ){
				kmb::ElementRelation::relationType rel =
					kmb::ElementRelation::getRelation( element, index, coElement, otherIndex );
				if( index == face.getLocalFaceId() && rel == kmb::ElementRelation::ADJACENT ){
					adj.setId( coElemId, otherIndex );
					return true;
				}
			}
		}
	}
	return false;
}

bool
kmb::NodeNeighborInfo::getAdjacentFace( const kmb::Face &face, kmb::Face &adj, kmb::MeshData* mesh ) const
{
	if( mesh == NULL ){
		return false;
	}

	kmb::ElementContainer::const_iterator element = mesh->findElement( face.getElementId() );
	if( !element.isFinished() ){
		int index = -1;
		int otherIndex = -1;

		kmb::nodeIdType nodeId = element.getBoundaryCellId( face.getLocalFaceId(), 0 );

		std::pair< kmb::NodeNeighbor::const_iterator, kmb::NodeNeighbor::const_iterator >
			eIterPair = coboundaries.equal_range( nodeId );
		kmb::NodeNeighbor::const_iterator eIter = eIterPair.first;
		while( eIter != eIterPair.second )
		{
			kmb::elementIdType coElemId = eIter->second;
			++eIter;

			kmb::ElementContainer::const_iterator coElement = mesh->findElement(coElemId);
			if( !coElement.isFinished() ){
				kmb::ElementRelation::relationType rel =
					kmb::ElementRelation::getRelation( element, index, coElement, otherIndex );
				if( index == face.getLocalFaceId() && rel == kmb::ElementRelation::ADJACENT ){
					adj.setId( coElemId, otherIndex );
					return true;
				}
			}
		}
	}
	return false;
}
