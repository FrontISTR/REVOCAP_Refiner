/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Polygon2D                                               #
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
#include <cmath>
#include "Geometry/kmbPolygon2D.h"

kmb::Polygon2D::Polygon2D(void)
{
}

kmb::Polygon2D::~Polygon2D(void)
{
}

void
kmb::Polygon2D::clear(void)
{
	this->nodeArray.clear();
}

void
kmb::Polygon2D::setPointContainer(const kmb::Point2DContainer* points)
{
	this->points = points;
}

int
kmb::Polygon2D::getSize(void) const
{
	return static_cast<int>( this->nodeArray.size() );
}

int
kmb::Polygon2D::getNodeIndex( kmb::nodeIdType nodeId ) const
{
	int ind = -1;
	const int len = getSize();
	for(int i=0;i<len;++i){
		if( nodeId == nodeArray[i] ){
			ind = i;
			break;
		}
	}
	return ind;
}

kmb::nodeIdType
kmb::Polygon2D::getNodeId( int index ) const
{
	const int len = getSize();
	if( len == 0 ){
		return kmb::nullNodeId;
	}
	int i = index % len;
	if( i < 0 ){
		i += len;
	}
	return nodeArray[i];
}

kmb::nodeIdType
kmb::Polygon2D::getNode( kmb::nodeIdType nodeId, int offset ) const
{
	int ind = getNodeIndex( nodeId );
	return getNodeId( ind+offset );
}

kmb::Polygon2D::orientation
kmb::Polygon2D::getOrientation(void) const
{
	if( this->nodeArray.empty() ){
		return kmb::Polygon2D::UNKNOWN;
	}
	double angle = 0.0;
	const int len = getSize();
	kmb::Point2D p0,p1,p2;
	for(int i=0;i<len;++i){
		const int prv = (i==0)? len-1 : i-1;
		const int nxt = (i==len-1)? 0 : i+1;
		if( points->getPoint( nodeArray[prv], p0 ) &&
			points->getPoint( nodeArray[i],   p1 ) &&
			points->getPoint( nodeArray[nxt], p2 ) )
		{
			angle += kmb::Point2D::angle( p0, p1, p2 );
		}
	}
	if( fabs(angle-2*PI) < 0.1 ){
		return kmb::Polygon2D::CLOCKWISE;
	}else if( fabs(angle+2*PI) < 0.1 ){
		return kmb::Polygon2D::UNTICLOCKWISE;
	}
	return kmb::Polygon2D::UNKNOWN;
}

kmb::Region::locationType
kmb::Polygon2D::intersect( kmb::Point2D &point ) const
{
	if( this->nodeArray.empty() ){
		return kmb::Region::UNKNOWN;
	}
	kmb::Point2D p0,p1;
	double angle = 0.0;
	const int len = getSize();
	for(int i=0;i<len;++i){
		const int nxt = (i==len-1)? 0 : i+1;
		if( points->getPoint( nodeArray[i],   p0 ) &&
			points->getPoint( nodeArray[nxt], p1 ) )
		{
			angle += kmb::Point2D::angle( p0, point, p1 );
		}
	}

	if( fabs(angle+2*PI) < 0.1 ){
		return kmb::Region::INSIDE;
	}else{
		return kmb::Region::OUTSIDE;
	}
}

void
kmb::Polygon2D::appendNodeId( kmb::nodeIdType nodeId )
{
	this->nodeArray.push_back( nodeId );
}

void
kmb::Polygon2D::dividePolygon(kmb::nodeIdType n0,kmb::nodeIdType n1,Polygon2D* &p0,Polygon2D* &p1)
{

	int i0 = getNodeIndex( n0 );
	int i1 = getNodeIndex( n1 );
	const int len = this->getSize();
	if( i0>=0 && i1>=0 && i0!=i1){
		p0 = new kmb::Polygon2D();
		if( p0 ){
			p0->setPointContainer( points );
			for(int i=i0;i!=i1;i=(i+1)%len){
				p0->appendNodeId( nodeArray[i] );
			}
			p0->appendNodeId( nodeArray[i1] );
		}
		p1 = new kmb::Polygon2D();
		if( p1 ){
			p1->setPointContainer( points );
			for(int i=i1;i!=i0;i=(i+1)%len){
				p1->appendNodeId( nodeArray[i] );
			}
			p1->appendNodeId( nodeArray[i0] );
		}
	}else{
		p0 = NULL;
		p1 = NULL;
	}
}

void
kmb::Polygon2D::branchPolygon(kmb::nodeIdType n0,kmb::nodeIdType n1,Polygon2D &negative,Polygon2D* &cancel)
{

	int i0 = this->getNodeIndex( n0 );
	int i1 = negative.getNodeIndex( n1 );
	const int len = this->getSize();
	const int lenNeg = negative.getSize();





	if( i0>=0 && i1>=0 && i0!=i1){
		cancel = new kmb::Polygon2D();
		cancel->setPointContainer( points );
		cancel->appendNodeId( n0 );
		for(int i=(i0+1)%len;i!=i0;i=(i+1)%len){
			cancel->appendNodeId( nodeArray[i] );
		}
		cancel->appendNodeId( n0 );
		cancel->appendNodeId( n1 );
		for(int i=(i1+1)%lenNeg;i!=i1;i=(i+1)%lenNeg){
			cancel->appendNodeId( negative.nodeArray[i] );
		}
		cancel->appendNodeId( n1 );
	}else{
		cancel = NULL;
	}
}
