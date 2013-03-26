/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Polygon                                                 #
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
#include "MeshDB/kmbPolygon.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbTypes.h"

#include "Geometry/kmbPolygon2D.h"
#include "Common/kmbCalculator.h"

kmb::Polygon::Polygon(void)
: edges(NULL)
, edgeDeletable(true)
{
}

kmb::Polygon::~Polygon(void)
{
	if( edges && edgeDeletable ){
		delete edges;
		edges = NULL;
	}
}

void
kmb::Polygon::setEdges(kmb::ElementContainer *edges,bool clone)
{
	if( edges == NULL || !edges->isUniqueDim(1) ){
		return;
	}
	if( clone ){
		if( this->edges != NULL ){
			this->edges->clear();
		}else{
			this->edges = new kmb::ElementContainerMap();
		}

		kmb::ElementContainer::iterator eIter = edges->begin();
		while( eIter != edges->end() )
		{
			kmb::Element* element = eIter.getElement();
			if( element != NULL ){
				this->edges->addElement( element->clone() );
			}
			++eIter;
		}
	}else{
		if( this->edges && edgeDeletable ){
			delete this->edges;
			this->edges = NULL;
		}
		this->edges = edges;
		this->edgeDeletable = false;
	}
	this->neighborInfo.clear();
	this->neighborInfo.appendCoboundary( this->edges );
}

void
kmb::Polygon::setEdges(const kmb::ElementContainer *edges)
{
	if( edges == NULL || !edges->isUniqueDim(1) ){
		return;
	}
	if( this->edges != NULL ){
		this->edges->clear();
	}else{
		this->edges = new kmb::ElementContainerMap();
	}
	kmb::ElementContainer::copy( edges, this->edges );
	this->neighborInfo.clear();
	this->neighborInfo.appendCoboundary( this->edges );
}

void
kmb::Polygon::setEdgesByFaceGroup( const kmb::DataBindings* facegroup, const kmb::ElementContainer* patches )
{
	if( patches == NULL || !patches->isUniqueDim(2) ||
		facegroup == NULL || facegroup->getBindingMode() != kmb::DataBindings::FaceGroup )
	{
		return;
	}
	if( this->edges != NULL ){
		this->edges->clear();
	}else{
		this->edges = new kmb::ElementContainerMap();
	}
	kmb::DataBindings::const_iterator fIter = facegroup->begin();
	while( !fIter.isFinished() ){
		kmb::Face f;
		if( fIter.getFace(f) ){
			kmb::ElementContainer::const_iterator eIter = patches->find( f.getElementId() );
			kmb::Segment* segment = new kmb::Segment(
				eIter.getBoundaryCellId(f.getLocalFaceId(),0),
				eIter.getBoundaryCellId(f.getLocalFaceId(),1));
			if( segment ){
				this->edges->addElement( segment );
			}
		}
		++fIter;
	}
	this->neighborInfo.clear();
	this->neighborInfo.appendCoboundary( this->edges );
}

void
kmb::Polygon::addSegment( kmb::nodeIdType n0, kmb::nodeIdType n1 )
{
	if( this->edges == NULL ){
		this->edges = new kmb::ElementContainerMap();
	}
	kmb::Segment* segment = NULL;
	segment = new kmb::Segment(n0, n1);
	if( segment != NULL )
	{
		kmb::elementIdType elemID = this->edges->addElement( segment );
		this->neighborInfo.appendCoboundary(elemID, *segment);
	}
}

bool
kmb::Polygon::deleteSegment( kmb::nodeIdType n0, kmb::nodeIdType n1 )
{
	if( edges == NULL || n0 == n1 ){
		return false;
	}
	kmb::NodeNeighbor::iterator nIter = this->neighborInfo.beginIteratorAt(n0);
	while( nIter != this->neighborInfo.endIteratorAt(n0) ){
		kmb::ElementContainer::iterator seg = edges->find( nIter->second );
		if( !seg.isFinished() && seg.includeVertex(n1) ){
			this->neighborInfo.deleteCoboundary( nIter->second, seg );
			edges->deleteElement( nIter->second );
			return true;
		}
		++nIter;
	}
	return false;
}

const kmb::ElementContainer*
kmb::Polygon::getEdges(void) const
{
	return edges;
}

size_t
kmb::Polygon::getSize(void) const
{
	if( edges ){
		return edges->getCount();
	}else{
		return 0;
	}
}

bool
kmb::Polygon::getCenter(const kmb::Point2DContainer* points,kmb::Point2D &center) const
{
	if( points == NULL ){
		return false;
	}
	center.setCoordinate(0.0,0.0);
	double x=0.0, y=0.0;
	kmb::ElementContainer::iterator eIter = edges->begin();
	while( !eIter.isFinished() ){
		kmb::nodeIdType nodeId = eIter.getCellId(0);
		points->getXY( nodeId, x, y );
		center.addCoordinate( x, y );
		++eIter;
	}
	center.scale( 1.0/getSize() );
	return true;
}

bool
kmb::Polygon::getCenter(const kmb::Point3DContainer* points,kmb::Point3D &center) const
{
	if( points == NULL ){
		return false;
	}
	center.setCoordinate(0.0,0.0,0.0);
	double x=0.0, y=0.0, z=0.0;
	kmb::ElementContainer::iterator eIter = edges->begin();
	while( !eIter.isFinished() ){
		kmb::nodeIdType nodeId = eIter.getCellId(0);
		points->getXYZ( nodeId, x, y, z );
		center.addCoordinate( x, y, z );
		++eIter;
	}
	center.scale( 1.0/getSize() );
	return true;
}

kmb::nodeIdType
kmb::Polygon::getNearestNode(const kmb::Point2DContainer* points,kmb::Point2D &point) const
{
	if( points == NULL ){
		return kmb::nullNodeId;
	}
	kmb::nodeIdType nearestId = kmb::nullNodeId;
	kmb::Minimizer minimizer;
	double x=0.0, y=0.0;
	kmb::ElementContainer::iterator eIter = edges->begin();
	while( !eIter.isFinished() ){
		kmb::nodeIdType nodeId = eIter.getCellId(0);
		if( points->getXY(nodeId,x,y) && minimizer.update( point.distanceSq(x,y) ) ){
			nearestId = nodeId;
		}
		++eIter;
	}
	return nearestId;
}

kmb::nodeIdType
kmb::Polygon::getNearestNode(const kmb::Point3DContainer* points,kmb::Point3D &point) const
{
	if( points == NULL ){
		return kmb::nullNodeId;
	}
	kmb::nodeIdType nearestId = kmb::nullNodeId;
	kmb::Minimizer minimizer;
	double x=0.0, y=0.0, z=0.0;
	kmb::ElementContainer::iterator eIter = edges->begin();
	while( !eIter.isFinished() ){
		kmb::nodeIdType nodeId = eIter.getCellId(0);
		if( points->getXYZ(nodeId,x,y,z) && minimizer.update( point.distanceSq(x,y,z) ) ){
			nearestId = nodeId;
		}
		++eIter;
	}
	return nearestId;
}

bool
kmb::Polygon::include(kmb::nodeIdType nodeId) const
{
	return neighborInfo.getElementCountAroundNode(nodeId) > 0;
}

kmb::nodeIdType
kmb::Polygon::getNodeId(kmb::nodeIdType nodeId,int offset) const
{
	kmb::nodeIdType retNodeId = kmb::nullNodeId;
	if( edges == NULL || neighborInfo.getElementCountAroundNode(nodeId) == 0 )
		return retNodeId;

	retNodeId = nodeId;

	if( offset > 0 ){
		for(int i=0;i<offset;++i){
			retNodeId = kmb::Polygon::getNextNode( edges, neighborInfo, retNodeId, true );
		}
	}else if( offset < 0 ){
		for(int i=0;i<-offset;++i){
			retNodeId = kmb::Polygon::getNextNode( edges, neighborInfo, retNodeId, false );
		}
	}
	return retNodeId;
}

bool
kmb::Polygon::isClosed(void) const
{
	if( edges == NULL ){
		return false;
	}

	bool retVal = true;
	kmb::ElementContainer::iterator eIter = edges->begin();
	while( !eIter.isFinished() )
	{

		kmb::nodeIdType nodeId = eIter.getCellId(0);
		int innerElement = 0;
		int outerElement = 0;
		kmb::NodeNeighbor::const_iterator nIter = neighborInfo.beginIteratorAt( nodeId );
		while( nIter != neighborInfo.endIteratorAt( nodeId ) )
		{
			kmb::ElementContainer::iterator e = edges->find( nIter->second );
			switch( e.indexOf( nodeId ) )
			{
			case 0:
				++outerElement;
				break;
			case 1:
				++innerElement;
				break;
			default:
				break;
			}
			++nIter;
		}
		if( innerElement != outerElement ){
			retVal = false;
			break;
		}
		++eIter;
	}
	return retVal;
}

kmb::nodeIdType
kmb::Polygon::hasDoubleNode(void) const
{
	kmb::nodeIdType irregalNodeId = kmb::nullNodeId;
	if( edges == NULL ){
		return irregalNodeId;
	}
	kmb::ElementContainerMap::iterator eIter = edges->begin();
	while( eIter != edges->end() )
	{


		kmb::nodeIdType nodeId = eIter.getCellId(0);
		if( neighborInfo.getElementCountAroundNode( nodeId ) > 2 ){
			irregalNodeId = nodeId;
			break;
		}
		++eIter;
	}
	return irregalNodeId;
}

bool
kmb::Polygon::isAdjacent(kmb::nodeIdType n0,kmb::nodeIdType n1) const
{
	bool findFlag = false;

	kmb::NodeNeighbor::const_iterator
		eIter = neighborInfo.beginIteratorAt( n0 );
	while( eIter != neighborInfo.endIteratorAt( n0 ) )
	{
		kmb::elementIdType elemID = eIter->second;
		kmb::ElementContainer::iterator elem = edges->find( elemID );
		if( elem.includeVertex( n1 ) )
		{
			findFlag = true;
			break;
		}
	}
	return findFlag;
}

kmb::nodeIdType
kmb::Polygon::getNextNode
( const kmb::ElementContainer* edges, const kmb::NodeNeighborInfo& neighborInfo, kmb::nodeIdType nodeID, bool order )
{
	kmb::nodeIdType nextID = kmb::nullNodeId;
	if( edges == NULL || !edges->isUniqueDim(1) )
		return nextID;

	int count = 0;
	kmb::NodeNeighbor::const_iterator
		eIter = neighborInfo.beginIteratorAt( nodeID );
	while( eIter != neighborInfo.endIteratorAt( nodeID ) )
	{
		kmb::elementIdType elemID = eIter->second;
		kmb::ElementContainer::const_iterator elem = edges->find( elemID );
		if( !elem.isFinished() && elem.getCellId( (order) ? 0 : 1 ) == nodeID )
		{
			++count;
			nextID = elem.getCellId( (order) ? 1 : 0 );
		}
		++eIter;
	}
	if( count == 1 ){
		return nextID;
	}else{
		return kmb::nullNodeId;
	}
}

kmb::elementIdType
kmb::Polygon::getNextElement
( const kmb::ElementContainer* edges, const kmb::NodeNeighborInfo& neighborInfo, kmb::elementIdType elementID, bool order)
{
	kmb::elementIdType nextID = kmb::Element::nullElementId;
	if( edges == NULL || !edges->isUniqueDim(1) )
		return nextID;


	kmb::ElementContainer::const_iterator nowElement = edges->find( elementID );
	if( nowElement.isFinished() )
		return nextID;

	kmb::nodeIdType nodeID = nowElement.getCellId( (order) ? 1 : 0 );

	int count = 0;
	kmb::NodeNeighbor::const_iterator
		eIter = neighborInfo.beginIteratorAt( nodeID );
	while( eIter != neighborInfo.endIteratorAt( nodeID ) )
	{
		kmb::elementIdType elemID = eIter->second;
		kmb::ElementContainer::const_iterator elem = edges->find( elemID );
		if( !elem.isFinished() && elem.getCellId( (order) ? 0 : 1 ) == nodeID )
		{
			++count;
			nextID = elemID;
		}
		++eIter;
	}
	if( count == 1 ){
		return nextID;
	}else{
		return kmb::Element::nullElementId;
	}
}

kmb::elementIdType
kmb::Polygon::getElementByNode
( kmb::ElementContainer* edges, kmb::NodeNeighborInfo& neighborInfo, kmb::nodeIdType nodeID, bool order)
{
	kmb::elementIdType nextID = kmb::Element::nullElementId;
	if( edges == NULL || !edges->isUniqueDim(1) )
		return nextID;

	int count = 0;
	kmb::NodeNeighbor::iterator
		eIter = neighborInfo.beginIteratorAt( nodeID );
	while( eIter != neighborInfo.endIteratorAt( nodeID ) )
	{
		kmb::elementIdType elemID = eIter->second;
		kmb::ElementContainer::iterator elem = edges->find( elemID );
		if( !elem.isFinished() && elem.getCellId( (order) ? 0 : 1 ) == nodeID )
		{
			++count;
			nextID = elemID;
		}
		++eIter;
	}
	if( count == 1 ){
		return nextID;
	}else{
		return kmb::Element::nullElementId;
	}

}

void
kmb::Polygon::dividePolygonsByDiagonals(
	const kmb::Point2DContainer* points,
	std::vector< std::pair<kmb::nodeIdType, kmb::nodeIdType> > &diagonals,
	std::vector< kmb::Polygon* > &polygons)
{
	if( points == NULL ){
		return;
	}


	std::multimap< kmb::nodeIdType, kmb::nodeIdType > nodePairs;

	kmb::ElementContainer::iterator eIter = edges->begin();
	while( !eIter.isFinished() )
	{
		nodePairs.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >
			( eIter.getCellId(0), eIter.getCellId(1) ) );
		++eIter;
	}

	std::vector< std::pair<kmb::nodeIdType, kmb::nodeIdType> >::iterator dIter = diagonals.begin();
	while( dIter != diagonals.end() )
	{

		std::multimap< kmb::nodeIdType, kmb::nodeIdType >::iterator f0Iter = nodePairs.lower_bound(dIter->first);
		std::multimap< kmb::nodeIdType, kmb::nodeIdType >::iterator f1Iter = nodePairs.upper_bound(dIter->first);
		bool flag = false;
		while( f0Iter != f1Iter ){
			if( f0Iter->second == dIter->second ){
				flag = true;
			}
			++f0Iter;
		}
		if( !flag ){
			nodePairs.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >(dIter->first,dIter->second) );
			nodePairs.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >(dIter->second,dIter->first) );
		}
		++dIter;
	}


	while( true ){
		std::multimap< kmb::nodeIdType, kmb::nodeIdType >::iterator
			nIter = nodePairs.begin();
		if( nIter == nodePairs.end() ){

			break;
		}else{
			kmb::nodeIdType prevId = nIter->first;
			kmb::nodeIdType nodeId = nIter->second;
			kmb::Polygon* polygon = NULL;
			polygon = new kmb::Polygon();
			kmb::nodeIdType startId = nodeId;
			while( nodeId != kmb::nullNodeId ){

				kmb::nodeIdType nextId =
					kmb::Polygon::getNextNode(prevId,nodeId,points,nodePairs,true);
















				if( nextId != kmb::nullNodeId ){





					polygon->addSegment( nodeId, nextId );
				}
				prevId = nodeId;
				nodeId = nextId;
				if( nodeId == startId ){
					break;
				}
			}
			if( polygon->getSize() >= 3 && polygon->isClosed() ){
				polygons.push_back( polygon );
			}else{

				delete polygon;
			}
		}
	}
}

kmb::nodeIdType
kmb::Polygon::getNextNode(
	kmb::nodeIdType prevId,
	kmb::nodeIdType nodeId,
	const kmb::Point2DContainer* points,
	std::multimap< kmb::nodeIdType, kmb::nodeIdType > &nodePairs,
	bool deleteflag )
{
	kmb::nodeIdType nextId = kmb::nullNodeId;
	if( points == NULL ){
		return nextId;
	}
	kmb::Point2D prevPoint,nowPoint;
	if( !points->getPoint( prevId, prevPoint ) ||
		!points->getPoint( nodeId, nowPoint ) )
	{
		return nextId;
	}

	if( nodePairs.count(nodeId) == 1){
		std::multimap< kmb::nodeIdType, kmb::nodeIdType >::iterator nextIter = nodePairs.find(nodeId);
		nextId = nextIter->second;
		if( deleteflag ){
			nodePairs.erase( nextIter );
		}
	}

	else{
		kmb::Maximizer maximizer;
		std::multimap< kmb::nodeIdType, kmb::nodeIdType >::iterator
			nIter = nodePairs.lower_bound( nodeId );
		std::multimap< kmb::nodeIdType, kmb::nodeIdType >::iterator
			endIter = nodePairs.upper_bound( nodeId );
		std::multimap< kmb::nodeIdType, kmb::nodeIdType >::iterator nextIter = endIter;
		while( nIter != endIter )
		{
			kmb::Point2D nextPoint;
			if( points->getPoint( nIter->second, nextPoint ) && nIter->second != prevId ){
				double ang = Point2D::angle2( prevPoint, nowPoint, nextPoint );
				if( maximizer.update( ang ) ){
					nextIter = nIter;
				}
			}
			++nIter;
		}
		if( nextIter != endIter ){
			nextId = nextIter->second;
			if( deleteflag ){
				nodePairs.erase( nextIter );
			}
		}
	}
	return nextId;
}

bool
kmb::Polygon::getEndPoints( kmb::nodeIdType &initial, kmb::nodeIdType &end ) const
{
	int count = 0;

	initial = kmb::nullNodeId;
	end = kmb::nullNodeId;


	kmb::ElementContainer::iterator eIter = edges->begin();
	while( !eIter.isFinished() ){
		kmb::nodeIdType node0 = eIter.getCellId(0);
		kmb::nodeIdType node1 = eIter.getCellId(1);
		if( neighborInfo.getElementCountAroundNode(node0) == 1 ){
			++count;
			initial = node0;
		}
		if( neighborInfo.getElementCountAroundNode(node1) == 1 ){
			++count;
			end = node1;
		}
		++eIter;
	}

	if( count > 2 )
	{
		initial = kmb::nullNodeId;
		end = kmb::nullNodeId;
		return false;
	}

	return true;
}
