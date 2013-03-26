/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MiddleNodeManager                                       #
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

#include "MeshGen/kmbMiddleNodeManager.h"

#include "Geometry/kmbPoint3DContainer.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbPyramid2.h"
#include "MeshGen/kmbMeshSmoother.h"

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
kmb::MiddleNodeManager::isDivided(kmb::nodeIdType a,kmb::nodeIdType b) const
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
	std::map< NodePair, kmb::nodeIdType >::const_iterator nIter = middlePoints.find( pair );
	if( nIter != middlePoints.end() ){
		return nIter->second;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode(kmb::nodeIdType n0, kmb::nodeIdType n1)
{
	kmb::Node p0, p1;
	if( points && points->getPoint(n0,p0) && points->getPoint(n1,p1) ){
		return points->addPoint( 0.5*(p0.x()+p1.x()), 0.5*(p0.y()+p1.y()), 0.5*(p0.z()+p1.z()) );
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode3(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2)
{
	kmb::Node p0, p1, p2;
	if( points && points->getPoint(n0,p0) && points->getPoint(n1,p1) && points->getPoint(n2,p2) ){
		return points->addPoint(
			(p0.x()+p1.x()+p2.x())/3.0,
			(p0.y()+p1.y()+p2.y())/3.0,
			(p0.z()+p1.z()+p2.z())/3.0);
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode3s(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2)
{
	kmb::Node p0, p1, p2;
	if( points && points->getPoint(n0,p0) && points->getPoint(n1,p1) ){
		if( points->getPoint(n2,p2) ){
			return points->addPoint(
				0.375*p0.x()+0.75*p1.x()-0.125*p2.x(),
				0.375*p0.y()+0.75*p1.y()-0.125*p2.y(),
				0.375*p0.z()+0.75*p1.z()-0.125*p2.z()
			);
		}else{
			return points->addPoint( 0.5*(p0.x()+p1.x()), 0.5*(p0.y()+p1.y()), 0.5*(p0.z()+p1.z()) );
		}
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode4(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3)
{
	kmb::Node p0, p1, p2, p3;
	if( points &&
		points->getPoint(n0,p0) &&
		points->getPoint(n1,p1) &&
		points->getPoint(n2,p2) &&
		points->getPoint(n3,p3) )
	{
		return points->addPoint(
			0.25*(p0.x()+p1.x()+p2.x()+p3.x()),
			0.25*(p0.y()+p1.y()+p2.y()+p3.y()),
			0.25*(p0.z()+p1.z()+p2.z()+p3.z()));
	}
	return kmb::nullNodeId;
}


kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode5(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3, kmb::nodeIdType n4)
{
	kmb::Node p0, p1, p2, p3, p4;
	if( points && points->getPoint(n0,p0) && points->getPoint(n1,p1) ){
		if( points->getPoint(n2,p2) && points->getPoint(n3,p3) && points->getPoint(n4,p4) ){
			return points->addPoint(
				0.5*p0.x()+0.5*p1.x()-0.125*p2.x()-0.125*p3.x()+0.25*p4.x(),
				0.5*p0.y()+0.5*p1.y()-0.125*p2.y()-0.125*p3.y()+0.25*p4.y(),
				0.5*p0.z()+0.5*p1.z()-0.125*p2.z()-0.125*p3.z()+0.25*p4.z()
			);
		}else{
			return points->addPoint( 0.5*(p0.x()+p1.x()), 0.5*(p0.y()+p1.y()), 0.5*(p0.z()+p1.z()) );
		}
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode6(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3,kmb::nodeIdType n4, kmb::nodeIdType n5)
{
	kmb::Node p0, p1, p2, p3, p4, p5;
	if( points &&
		points->getPoint(n0,p0) &&
		points->getPoint(n1,p1) &&
		points->getPoint(n2,p2) &&
		points->getPoint(n3,p3) &&
		points->getPoint(n4,p4) &&
		points->getPoint(n5,p5) )
	{
		return points->addPoint(
			(p0.x()+p1.x()+p2.x()+p3.x()+p4.x()+p5.x())/6.0,
			(p0.y()+p1.y()+p2.y()+p3.y()+p4.y()+p5.y())/6.0,
			(p0.z()+p1.z()+p2.z()+p3.z()+p4.z()+p5.z())/6.0);
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode8(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3,kmb::nodeIdType n4, kmb::nodeIdType n5,kmb::nodeIdType n6, kmb::nodeIdType n7)
{
	kmb::Node p0, p1, p2, p3, p4, p5, p6, p7;
	if( points &&
		points->getPoint(n0,p0) &&
		points->getPoint(n1,p1) &&
		points->getPoint(n2,p2) &&
		points->getPoint(n3,p3) &&
		points->getPoint(n4,p4) &&
		points->getPoint(n5,p5) &&
		points->getPoint(n6,p6) &&
		points->getPoint(n7,p7) )
	{
		return points->addPoint(
			0.125*(p0.x()+p1.x()+p2.x()+p3.x()+p4.x()+p5.x()+p6.x()+p7.x()),
			0.125*(p0.y()+p1.y()+p2.y()+p3.y()+p4.y()+p5.y()+p6.y()+p7.y()),
			0.125*(p0.z()+p1.z()+p2.z()+p3.z()+p4.z()+p5.z()+p6.z()+p7.z()));
	}
	return kmb::nullNodeId;
}


kmb::nodeIdType
kmb::MiddleNodeManager::createMiddleNode8s(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3, kmb::nodeIdType n4, kmb::nodeIdType n5,kmb::nodeIdType n6, kmb::nodeIdType n7)
{
	kmb::Node p0, p1, p2, p3, p4, p5, p6, p7;
	if( points &&
		points->getPoint(n0,p0) &&
		points->getPoint(n1,p1) &&
		points->getPoint(n2,p2) &&
		points->getPoint(n3,p3) &&
		points->getPoint(n4,p4) &&
		points->getPoint(n5,p5) &&
		points->getPoint(n6,p6) &&
		points->getPoint(n7,p7) )
	{
		return points->addPoint(
			-0.25*(p0.x()+p1.x()+p2.x()+p3.x())+0.5*(p4.x()+p5.x()+p6.x()+p7.x()),
			-0.25*(p0.y()+p1.y()+p2.y()+p3.y())+0.5*(p4.y()+p5.y()+p6.y()+p7.y()),
			-0.25*(p0.z()+p1.z()+p2.z()+p3.z())+0.5*(p4.z()+p5.z()+p6.z()+p7.z()));
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
	if( middleNodeId != kmb::nullNodeId ){
		return middleNodeId;
	}
	middleNodeId = createMiddleNode(a,b);
	if( middleNodeId != kmb::nullNodeId ){
		appendMiddleNode( middleNodeId, a, b, elemId );
		return middleNodeId;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::getDividedNode3(kmb::nodeIdType a,kmb::nodeIdType b,kmb::nodeIdType c,kmb::elementIdType elemId)
{
	kmb::nodeIdType middleNodeId = this->isDivided(a,b);
	if( middleNodeId != kmb::nullNodeId ){
		return middleNodeId;
	}
	middleNodeId = createMiddleNode3s(a,b,c);
	if( middleNodeId != kmb::nullNodeId ){
		if( a < b ){
			appendMiddleNode( middleNodeId, a, b, elemId );
		}else{
			appendMiddleNode( middleNodeId, b, a, elemId );
		}
		return middleNodeId;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::getDividedNode5(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3,kmb::nodeIdType n4,kmb::elementIdType elemId)
{
	kmb::nodeIdType middleNodeId = this->isDivided(n0,n1);
	if( middleNodeId != kmb::nullNodeId ){
		return middleNodeId;
	}
	middleNodeId = createMiddleNode5(n0,n1,n2,n3,n4);
	if( middleNodeId != kmb::nullNodeId ){
		if( n0 < n1 ){
			appendMiddleNode( middleNodeId, n0, n1, elemId );
		}else{
			appendMiddleNode( middleNodeId, n1, n0, elemId );
		}
		return middleNodeId;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::getDividedNodePyrmid2c(int index,kmb::nodeIdType center,kmb::ElementBase &elem,kmb::elementIdType elemId)
{
	if( points == NULL ){
		return kmb::nullNodeId;
	}
	kmb::nodeIdType n = elem[index];
	kmb::nodeIdType middleNodeId = this->isDivided(n,center);
	if( middleNodeId != kmb::nullNodeId ){
		return middleNodeId;
	}
	double coeff[13];
	double x(0.0),y(0.0),z(0.0);
	kmb::Point3D pt;
	kmb::Pyramid2::shapeFunction(-0.25,-0.25,-0.5,coeff);
	for(int i=0;i<13;++i){
		if( points->getPoint(elem[i],pt ) ){
			x += coeff[i]*pt.x();
			y += coeff[i]*pt.y();
			z += coeff[i]*pt.z();
		}
	}
	middleNodeId = points->addPoint( x, y, z );
	if( middleNodeId != kmb::nullNodeId ){
		if( n < center ){
			appendMiddleNode( middleNodeId, n, center, elemId );
		}else{
			appendMiddleNode( middleNodeId, center, n, elemId );
		}
		return middleNodeId;
	}
	return kmb::nullNodeId;
}

void
kmb::MiddleNodeManager::appendMiddleNode( kmb::nodeIdType middle, kmb::nodeIdType org0, kmb::nodeIdType org1,kmb::elementIdType elemId)
{

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
	switch( elem.getType() )
	{
	case kmb::SEGMENT:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = elem[1]; break;
		case 1: b = elem[0]; break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	case kmb::QUAD:
	case kmb::QUAD2:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = elem[2]; break;
		case 1: b = elem[3]; break;
		case 2: b = elem[0]; break;
		case 3: b = elem[1]; break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode4(elem[0],elem[1],elem[2],elem[3]);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	case kmb::HEXAHEDRON:
	case kmb::HEXAHEDRON2:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = elem[6]; break;
		case 1: b = elem[7]; break;
		case 2: b = elem[4]; break;
		case 3: b = elem[5]; break;
		case 4: b = elem[2]; break;
		case 5: b = elem[3]; break;
		case 6: b = elem[0]; break;
		case 7: b = elem[1]; break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode8(elem[0],elem[1],elem[2],elem[3],elem[4],elem[5],elem[6],elem[7]);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	case kmb::TRIANGLE:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = getDividedNode(elem[1],elem[2]); break;
		case 1: b = getDividedNode(elem[2],elem[0]); break;
		case 2: b = getDividedNode(elem[0],elem[1]); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode3(elem[0],elem[1],elem[2]);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	case kmb::TETRAHEDRON:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = getCenterNode(elem,0,elementId); break;
		case 1: b = getCenterNode(elem,1,elementId); break;
		case 2: b = getCenterNode(elem,2,elementId); break;
		case 3: b = getCenterNode(elem,3,elementId); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode4(elem[0],elem[1],elem[2],elem[3]);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	case kmb::WEDGE:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = getDividedNode(elem[4],elem[5]); break;
		case 1: b = getDividedNode(elem[5],elem[3]); break;
		case 2: b = getDividedNode(elem[3],elem[4]); break;
		case 3: b = getDividedNode(elem[1],elem[2]); break;
		case 4: b = getDividedNode(elem[2],elem[0]); break;
		case 5: b = getDividedNode(elem[0],elem[1]); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode4(elem[0],elem[1],elem[2],elem[3]);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
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
	switch( elem.getBoundaryType(faceIndex) )
	{
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
	{
		int minIndex = elem.getIndexMinNodeIdOfFace(faceIndex);
		a = elem.getBoundaryCellId( faceIndex, minIndex );
		switch( minIndex )
		{
		case 0: b = elem.getBoundaryCellId(faceIndex,1); break;
		case 1: b = elem.getBoundaryCellId(faceIndex,0); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
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
		case 0: b = elem.getBoundaryCellId(faceIndex,2); break;
		case 1: b = elem.getBoundaryCellId(faceIndex,3); break;
		case 2: b = elem.getBoundaryCellId(faceIndex,0); break;
		case 3: b = elem.getBoundaryCellId(faceIndex,1); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode4(
			elem.getBoundaryCellId(faceIndex,0),
			elem.getBoundaryCellId(faceIndex,1),
			elem.getBoundaryCellId(faceIndex,2),
			elem.getBoundaryCellId(faceIndex,3));
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	case kmb::TRIANGLE:
	{
		int minIndex = elem.getIndexMinNodeIdOfFace(faceIndex);
		a = elem.getBoundaryCellId( faceIndex, minIndex );
		switch( minIndex )
		{
		case 0: b = getDividedNode(elem.getBoundaryCellId(faceIndex,1),elem.getBoundaryCellId(faceIndex,2)); break;
		case 1: b = getDividedNode(elem.getBoundaryCellId(faceIndex,2),elem.getBoundaryCellId(faceIndex,0)); break;
		case 2: b = getDividedNode(elem.getBoundaryCellId(faceIndex,0),elem.getBoundaryCellId(faceIndex,1)); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode3(
			elem.getBoundaryCellId(faceIndex,0),
			elem.getBoundaryCellId(faceIndex,1),
			elem.getBoundaryCellId(faceIndex,2));
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	default:
		break;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::getCenterNode2(kmb::ElementBase &elem,kmb::elementIdType elementId)
{
	if( points == NULL ){
		return kmb::nullNodeId;
	}


	kmb::nodeIdType a = kmb::nullNodeId;
	kmb::nodeIdType b = kmb::nullNodeId;
	switch( elem.getType() )
	{
	case kmb::QUAD2:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = elem[2]; break;
		case 1: b = elem[3]; break;
		case 2: b = elem[0]; break;
		case 3: b = elem[1]; break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode8s(elem[0],elem[1],elem[2],elem[3],elem[4],elem[5],elem[6],elem[7]);
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	case kmb::HEXAHEDRON2:
	{
		int minIndex = elem.getIndexMinNodeId();
		a = elem[minIndex];
		switch( minIndex )
		{
		case 0: b = elem[6]; break;
		case 1: b = elem[7]; break;
		case 2: b = elem[4]; break;
		case 3: b = elem[5]; break;
		case 4: b = elem[2]; break;
		case 5: b = elem[3]; break;
		case 6: b = elem[0]; break;
		case 7: b = elem[1]; break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}

		kmb::Node pt;
		double x(0.0),y(0.0),z(0.0);
		if( points ){
			for(int i=0;i<8;++i){
				if( points->getPoint(elem[i],pt) ){
					x -= pt.x();
					y -= pt.y();
					z -= pt.z();
				}
			}
			for(int i=8;i<20;++i){
				if( points->getPoint(elem[i],pt) ){
					x += pt.x();
					y += pt.y();
					z += pt.z();
				}
			}
			x *= 0.25;
			y *= 0.25;
			z *= 0.25;
			middleNodeId = points->addPoint(x,y,z);
		}
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
		}
		break;
	}
	default:
		break;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::MiddleNodeManager::getCenterNode2(kmb::ElementBase &elem,int faceIndex,kmb::elementIdType elementId)
{
	if( points == NULL ){
		return kmb::nullNodeId;
	}
	kmb::nodeIdType a = kmb::nullNodeId;
	kmb::nodeIdType b = kmb::nullNodeId;
	switch( elem.getBoundaryType(faceIndex) )
	{
	case kmb::QUAD2:
	{
		int minIndex = elem.getIndexMinNodeIdOfFace(faceIndex);
		a = elem.getBoundaryCellId( faceIndex, minIndex );
		switch( minIndex )
		{
		case 0: b = elem.getBoundaryCellId(faceIndex,2); break;
		case 1: b = elem.getBoundaryCellId(faceIndex,3); break;
		case 2: b = elem.getBoundaryCellId(faceIndex,0); break;
		case 3: b = elem.getBoundaryCellId(faceIndex,1); break;
		default: break;
		}
		kmb::nodeIdType middleNodeId = this->isDivided(a,b);
		if( middleNodeId != kmb::nullNodeId ){
			return middleNodeId;
		}
		middleNodeId = createMiddleNode8s(
			elem.getBoundaryCellId(faceIndex,0),
			elem.getBoundaryCellId(faceIndex,1),
			elem.getBoundaryCellId(faceIndex,2),
			elem.getBoundaryCellId(faceIndex,3),
			elem.getBoundaryCellId(faceIndex,4),
			elem.getBoundaryCellId(faceIndex,5),
			elem.getBoundaryCellId(faceIndex,6),
			elem.getBoundaryCellId(faceIndex,7));
		if( middleNodeId != kmb::nullNodeId ){
			appendMiddleNode( middleNodeId, a, b, elementId );
			return middleNodeId;
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

int
kmb::MiddleNodeManager::smoothingMiddleNodes(kmb::MeshSmoother* smoother)
{
	if( smoother == NULL ){
		return 0;
	}
	int count = 0;
	std::map< kmb::nodeIdType, kmb::MiddleNodeManager::originalPair >::iterator iter = originalPoints.begin();
	while( iter != originalPoints.end() ){
		kmb::nodeIdType middleNodeId = iter->first;
		count += smoother->smoothingNode(middleNodeId);
		++iter;
	}
	return count;
}
