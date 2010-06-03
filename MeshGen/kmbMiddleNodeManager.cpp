/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : MiddleNodeManager                                       #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

#include "MeshGen/kmbMiddleNodeManager.h"

#include "Geometry/kmb_Point3DContainer.h"
#include "MeshDB/kmbElement.h"

kmb::MiddleNodeManager::MiddleNodeManager(void)
: points(NULL)
{
}

kmb::MiddleNodeManager::~MiddleNodeManager(void)
{
}

void
kmb::MiddleNodeManager::clear(void)
{
	middlePoints.clear();
	originalPoints.clear();
}

void
kmb::MiddleNodeManager::setNodeContainer(kmb::Point3DContainer* points)
{
	this->points = points;
}

kmb::nodeIdType
kmb::MiddleNodeManager::isDivided(kmb::nodeIdType a,kmb::nodeIdType b)
{
	if( a == kmb::nullNodeId || b == kmb::nullNodeId ){
		return kmb::nullNodeId;
	}
	if( a > b ){
		kmb::nodeIdType tmp = a;
		a = b;
		b = tmp;
	}
	kmb::MiddleNodeManager::NodePair pair(a,b);
	std::map< NodePair, kmb::nodeIdType >::iterator nIter = middlePoints.find( pair );
	if( nIter != middlePoints.end() ){
		return nIter->second;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::getDividedNode(kmb::nodeIdType a,kmb::nodeIdType b,kmb::elementIdType elemId)
{
	if( a > b ){
		kmb::nodeIdType tmp = a;
		a = b;
		b = tmp;
	}
	kmb::nodeIdType middleNodeId = this->isDivided(a,b);
	REVOCAP_Debug_3("getDividedNode [%d, %d] => existing node : %d\n", a, b, middleNodeId);
	if( middleNodeId != kmb::nullNodeId ){
		return middleNodeId;
	}
	kmb::Node na, nb;
	if( points && points->getPoint(a,na) && points->getPoint(b,nb) ){
		REVOCAP_Debug_3("[%f, %f, %f] [%f, %f, %f]\n", na.x(), na.y(), na.z(), nb.x(), nb.y(), nb.z() );
		kmb::nodeIdType nodeId = points->addPoint( 0.5*(na.x()+nb.x()), 0.5*(na.y()+nb.y()), 0.5*(na.z()+nb.z()) );
		REVOCAP_Debug_3("getDividedNode new node : %d\n", nodeId);
		appendMiddleNode( nodeId, a, b, elemId );
		return nodeId;
	}
	return kmb::nullNodeId;
}

void
kmb::MiddleNodeManager::appendMiddleNode( kmb::nodeIdType middle, kmb::nodeIdType org0, kmb::nodeIdType org1,kmb::elementIdType elemId)
{
	REVOCAP_Debug_3("appendMiddleNode [%d, %d] (%d) => %d\n",org0,org1,elemId,middle);

	kmb::MiddleNodeManager::NodePair pair(org0,org1);
	middlePoints.insert( std::pair<NodePair, kmb::nodeIdType>(pair,middle) );

	kmb::MiddleNodeManager::originalPair orgPair = {org0,org1,elemId};
	originalPoints.insert( std::pair<kmb::nodeIdType,kmb::MiddleNodeManager::originalPair>(middle,orgPair) );
}

kmb::nodeIdType
kmb::MiddleNodeManager::getCenterNode(kmb::ElementBase &elem,kmb::elementIdType elementId)
{
	if( points == NULL ){
		return kmb::nullNodeId;
	}


	kmb::nodeIdType a = kmb::nullNodeId;
	kmb::nodeIdType b = kmb::nullNodeId;
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	switch( elem.getType() )
	{
	case kmb::SEGMENT:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem.getCellId( minIndex );
		switch( minIndex )
		{
		case 0: b = elem.getCellId(1); break;
		case 1: b = elem.getCellId(0); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		kmb::Node na, nb;
		if( points && points->getPoint(a,na) && points->getPoint(b,nb) ){
			nodeId = points->addPoint( 0.5*(na.x()+nb.x()), 0.5*(na.y()+nb.y()), 0.5*(na.z()+nb.z()) );
			appendMiddleNode( nodeId, a, b, elementId );
			return nodeId;
		}
		break;
	}
	case kmb::QUAD:
	case kmb::QUAD2:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem.getCellId( minIndex );
		switch( minIndex )
		{
		case 0:	b = elem.getCellId(2);	break;
		case 1:	b = elem.getCellId(3);	break;
		case 2:	b = elem.getCellId(0);	break;
		case 3:	b = elem.getCellId(1);	break;
		default:	break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		kmb::Node n0, n1, n2, n3;
		if( points &&
			points->getPoint(elem.getCellId(0), n0) &&
			points->getPoint(elem.getCellId(1), n1) &&
			points->getPoint(elem.getCellId(2), n2) &&
			points->getPoint(elem.getCellId(3), n3) )
		{
			nodeId = points->addPoint(
				0.25*(n0.x()+n1.x()+n2.x()+n3.x()),
				0.25*(n0.y()+n1.y()+n2.y()+n3.y()),
				0.25*(n0.z()+n1.z()+n2.z()+n3.z())
			);
			appendMiddleNode( nodeId, a, b, elementId );
			return nodeId;
		}
		break;
	}
	case kmb::HEXAHEDRON:
	case kmb::HEXAHEDRON2:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem.getCellId( minIndex );
		switch( minIndex )
		{
		case 0:	b = elem.getCellId(6);	break;
		case 1:	b = elem.getCellId(7);	break;
		case 2:	b = elem.getCellId(4);	break;
		case 3:	b = elem.getCellId(5);	break;
		case 4:	b = elem.getCellId(2);	break;
		case 5:	b = elem.getCellId(3);	break;
		case 6:	b = elem.getCellId(0);	break;
		case 7:	b = elem.getCellId(1);	break;
		default:	break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		kmb::Node n0, n1, n2, n3, n4, n5, n6, n7;
		if( points &&
			points->getPoint(elem.getCellId(0), n0) &&
			points->getPoint(elem.getCellId(1), n1) &&
			points->getPoint(elem.getCellId(2), n2) &&
			points->getPoint(elem.getCellId(3), n3) &&
			points->getPoint(elem.getCellId(4), n4) &&
			points->getPoint(elem.getCellId(5), n5) &&
			points->getPoint(elem.getCellId(6), n6) &&
			points->getPoint(elem.getCellId(7), n7) )
		{
			nodeId = points->addPoint(
				0.125*(n0.x()+n1.x()+n2.x()+n3.x()+n4.x()+n5.x()+n6.x()+n7.x()),
				0.125*(n0.y()+n1.y()+n2.y()+n3.y()+n4.y()+n5.y()+n6.y()+n7.y()),
				0.125*(n0.z()+n1.z()+n2.z()+n3.z()+n4.z()+n5.z()+n6.z()+n7.z())
			);
			appendMiddleNode( nodeId, a, b, elementId );
			return nodeId;
		}
		break;
	}
	default:
		break;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::getCenterNode(kmb::ElementBase &elem,int faceIndex, kmb::elementIdType elementId)
{
	if( points == NULL ){
		return kmb::nullNodeId;
	}
	kmb::nodeIdType a = kmb::nullNodeId;
	kmb::nodeIdType b = kmb::nullNodeId;
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	switch( elem.getBoundaryType(faceIndex) )
	{
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
	{
		int minIndex = elem.getIndexMinNodeIdOfFace(faceIndex);
		a = elem.getBoundaryCellId( faceIndex, minIndex );
		switch( minIndex )
		{
		case 0:	b = elem.getBoundaryCellId(faceIndex,1);	break;
		case 1:	b = elem.getBoundaryCellId(faceIndex,0);	break;
		default:	break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		kmb::Node na, nb;
		if( points && points->getPoint(a,na) && points->getPoint(b,nb) ){
			nodeId = points->addPoint( 0.5*(na.x()+nb.x()), 0.5*(na.y()+nb.y()), 0.5*(na.z()+nb.z()) );
			appendMiddleNode( nodeId, a, b, elementId );
			return nodeId;
		}
		break;
	}
	case kmb::QUAD:
	case kmb::QUAD2:
	{
		int minIndex = elem.getIndexMinNodeIdOfFace(faceIndex);
		a = elem.getBoundaryCellId( faceIndex, minIndex );
		switch( minIndex )
		{
		case 0:	b = elem.getBoundaryCellId(faceIndex,2);	break;
		case 1:	b = elem.getBoundaryCellId(faceIndex,3);	break;
		case 2:	b = elem.getBoundaryCellId(faceIndex,0);	break;
		case 3:	b = elem.getBoundaryCellId(faceIndex,1);	break;
		default:	break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		kmb::Node n0, n1, n2, n3;
		if( points &&
			points->getPoint(elem.getBoundaryCellId(faceIndex,0), n0) &&
			points->getPoint(elem.getBoundaryCellId(faceIndex,1), n1) &&
			points->getPoint(elem.getBoundaryCellId(faceIndex,2), n2) &&
			points->getPoint(elem.getBoundaryCellId(faceIndex,3), n3) )
		{
			nodeId = points->addPoint(
				0.25*(n0.x()+n1.x()+n2.x()+n3.x()),
				0.25*(n0.y()+n1.y()+n2.y()+n3.y()),
				0.25*(n0.z()+n1.z()+n2.z()+n3.z())
			);
			appendMiddleNode( nodeId, a, b, elementId );
			return nodeId;
		}
		break;
	}
	default:
		break;
	}
	return kmb::nullNodeId;
}

void
kmb::MiddleNodeManager::setDividedNode(kmb::nodeIdType middle, kmb::nodeIdType a,kmb::nodeIdType b)
{
	if( a > b ){
		kmb::nodeIdType tmp = a;
		a = b;
		b = tmp;
	}
	appendMiddleNode( middle, a, b );
}

kmb::elementIdType
kmb::MiddleNodeManager::getOriginalNode(kmb::nodeIdType centerId,kmb::nodeIdType &a,kmb::nodeIdType &b) const
{
	std::map< kmb::nodeIdType, kmb::MiddleNodeManager::originalPair >::const_iterator cIter = originalPoints.find(centerId);
	if( cIter != originalPoints.end() ){
		a = cIter->second.n0;
		b = cIter->second.n1;
		return cIter->second.elementId;
	}
	a = kmb::nullNodeId;
	b = kmb::nullNodeId;
	return kmb::Element::nullElementId;
}
