/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Point3DOctree                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
#include <cfloat>
#include <cmath>
#include "Geometry/kmb_Point3DOctree.h"
#include "Geometry/kmb_Calculator.h"

/*
kmb::Point3DOctree::Point3DOctree(void)
: thres(256)
, count(0)
, layer(0)
, nodeIds(NULL)
, children(NULL)
{
}
*/

kmb::Point3DOctree::Point3DOctree(int thresh)
: thres(256)
, count(0)
, layer(0)
, nodeIds(NULL)
, children(NULL)
{
	this->thres = thresh;
}

kmb::Point3DOctree::~Point3DOctree(void)
{
	clear();
}

void
kmb::Point3DOctree::clear(void)
{
	if( children != NULL )
	{
		for( int i=0; i<8; ++i )
		{
			if( children[i] != NULL )
			{
				delete children[i];
				children[i] = NULL;
			}
		}
		delete[] children;
		children = NULL;
	}
	count = 0;
	if( nodeIds != NULL ){
		delete[] nodeIds;
		nodeIds = NULL;
	}
	boundBox.initialize();
	center.zero();
}

int
kmb::Point3DOctree::getLocalCount(void) const
{
	return count;
}

size_t
kmb::Point3DOctree::getCount(void) const
{
	size_t sum = 0;
	if( children != NULL ){
		for(int i=0;i<8;++i){
			sum += children[i]->getCount();
		}
	}else{
		sum = count;
	}
	return sum;
}

int
kmb::Point3DOctree::getChildIndex(double x,double y,double z) const
{
	return
		( ( x > center.x() ) ? 4 : 0 ) +
		( ( y > center.y() ) ? 2 : 0 ) +
		( ( z > center.z() ) ? 1 : 0 ) ;
}

int
kmb::Point3DOctree::getChildIndex(bool bx,bool by,bool bz) const
{
	return ( ( bx ) ? 4 : 0 ) + ( ( by ) ? 2 : 0 ) + ( ( bz ) ? 1 : 0 );
}

void
kmb::Point3DOctree::addPoint(kmb::nodeIdType nodeId,kmb::Point3D& point,const kmb::Point3DContainer* points)
{

	boundBox.update( point );
	if( children != NULL )
	{

		children[ getChildIndex( point.x(), point.y(), point.z() ) ]->addPoint(nodeId,point,points);
	}
	else if( nodeIds != NULL )
	{
		nodeIds[count] = nodeId;
		++count;
		if( count == thres )
		{
			createChildren( points );
		}
	}
	else if( count == 0 )
	{
		nodeIds = NULL;
		nodeIds = new kmb::nodeIdType[thres];
		if( nodeIds ){
			nodeIds[count] = nodeId;
			++count;
		}
	}
}

void
kmb::Point3DOctree::createChildren(const kmb::Point3DContainer* points)
{

	if( children == NULL )
	{
		this->boundBox.getCenter(center);
		children = new kmb::Point3DOctree*[8];
		for(int i=0;i<8;++i)
		{
			children[i] = NULL;
			children[i] = new kmb::Point3DOctree(thres);
			if( children[i] ){
				children[i]->layer = this->layer + 1;
			}else{
				return;
			}
		}
		const kmb::Point3D maxPt = boundBox.getMax();
		const kmb::Point3D minPt = boundBox.getMin();
		children[7]->boundBox.update( maxPt.x(), maxPt.y(), maxPt.z() );
		children[7]->boundBox.update( center.x(), center.y(), center.z() );
		children[6]->boundBox.update( maxPt.x(), maxPt.y(), minPt.z() );
		children[6]->boundBox.update( center.x(), center.y(), center.z() );
		children[5]->boundBox.update( maxPt.x(), minPt.y(), maxPt.z() );
		children[5]->boundBox.update( center.x(), center.y(), center.z() );
		children[4]->boundBox.update( maxPt.x(), minPt.y(), minPt.z() );
		children[4]->boundBox.update( center.x(), center.y(), center.z() );
		children[3]->boundBox.update( minPt.x(), maxPt.y(), maxPt.z() );
		children[3]->boundBox.update( center.x(), center.y(), center.z() );
		children[2]->boundBox.update( minPt.x(), maxPt.y(), minPt.z() );
		children[2]->boundBox.update( center.x(), center.y(), center.z() );
		children[1]->boundBox.update( minPt.x(), minPt.y(), maxPt.z() );
		children[1]->boundBox.update( center.x(), center.y(), center.z() );
		children[0]->boundBox.update( minPt.x(), minPt.y(), minPt.z() );
		children[0]->boundBox.update( center.x(), center.y(), center.z() );

		kmb::Point3D point;
		for(int i=0;i<count;++i)
		{
			if( points->getPoint( nodeIds[i], point ) ){
				addPoint( nodeIds[i], point, points );
			}
		}
		this->count = -1;
		delete[] nodeIds;

		nodeIds = NULL;
	}
}

void
kmb::Point3DOctree::generateSearchCache(kmb::Point3DContainer* points)
{
	if( points != NULL )
	{
		kmb::Point3D point;
		points->getBoundingBox( boundBox );
		kmb::Point3DContainer::iterator pIter = points->begin();
		while( pIter != points->end() )
		{
			kmb::nodeIdType nodeId = pIter.getId();
			if( pIter.getPoint( point ) ){
				addPoint( nodeId, point, points);
			}
			++pIter;
		}
	}
}

void
kmb::Point3DOctree::generateSearchCache(kmb::Point3DContainer* points,std::set< kmb::nodeIdType > &nodeSet)
{
	if( points != NULL )
	{
		kmb::Point3D point;
		std::set< kmb::nodeIdType >::iterator nIter = nodeSet.begin();
		while( nIter != nodeSet.end() )
		{
			kmb::nodeIdType nodeId = *nIter;
			if( points->getPoint( nodeId, point ) ){
				addPoint( nodeId, point, points );
			}
			++nIter;
		}
	}
}

void
kmb::Point3DOctree::appendSearchCache(const kmb::Point3DContainer* points,kmb::nodeIdType nodeId)
{
	if( points != NULL )
	{
		kmb::Point3D point;
		if( points->getPoint( nodeId, point ) ){
			addPoint( nodeId, point, points );
		}
	}
}

double
kmb::Point3DOctree::getNearestPoint(double x,double y,double z, const kmb::Point3DContainer* points,kmb::nodeIdType &nearestId) const
{
	kmb::Minimizer min;
	nearestId = kmb::nullNodeId;
	if( points == NULL ){
		return min.getMin();
	}
	if( children != NULL )
	{
		kmb::nodeIdType tmpId = kmb::nullNodeId;
		bool bx = ( x > center.x() );
		bool by = ( y > center.y() );
		bool bz = ( z > center.z() );
		min.update( children[ getChildIndex(bx,by,bz) ]->getNearestPoint(x,y,z,points,nearestId) );

		if( fabs(x-center.x()) < min.getMin() ){
			if( min.update( children[ getChildIndex(!bx,by,bz) ]->getNearestPoint(x,y,z,points,tmpId) ) ){
				nearestId = tmpId;
			}
			if( fabs(y-center.y()) < min.getMin() ){
				if( min.update( children[ getChildIndex(!bx,!by,bz) ]->getNearestPoint(x,y,z,points,tmpId) ) ){
					nearestId = tmpId;
				}
				if( fabs(z-center.z()) < min.getMin() ){
					if( min.update( children[ getChildIndex(!bx,!by,!bz) ]->getNearestPoint(x,y,z,points,tmpId) ) ){
						nearestId = tmpId;
					}
				}
			}
			if( fabs(z-center.z()) < min.getMin() ){
				if( min.update( children[ getChildIndex(!bx,by,!bz) ]->getNearestPoint(x,y,z,points,tmpId) ) ){
					nearestId = tmpId;
				}
			}
		}
		if( fabs(y-center.y()) < min.getMin() ){
			if( min.update( children[ getChildIndex(bx,!by,bz) ]->getNearestPoint(x,y,z,points,tmpId) ) ){
				nearestId = tmpId;
			}
			if( fabs(z-center.z()) < min.getMin() ){
				if( min.update( children[ getChildIndex(bx,!by,!bz) ]->getNearestPoint(x,y,z,points,tmpId) ) ){
					nearestId = tmpId;
				}
			}
		}
		if( fabs(z-center.z()) < min.getMin() ){
			if( min.update( children[ getChildIndex(bx,by,!bz) ]->getNearestPoint(x,y,z,points,tmpId) ) ){
				nearestId = tmpId;
			}
		}
	}
	else if( nodeIds != NULL )
	{
		kmb::Point3D point;
		for(int i=0;i<count;++i)
		{
			if( points->getPoint( nodeIds[i], point ) ){
				if( min.update( point.distanceSq(x,y,z) ) ){
					nearestId = nodeIds[i];
				}
			}
		}
	}
	return min.getMin();
}

void
kmb::Point3DOctree::getNodesInRegion(kmb::Region* region, kmb::Point3DContainer* points, std::set<kmb::nodeIdType> &nodes) const
{

	if( points == NULL || region == NULL ){
		return;
	}
	if( children != NULL )
	{
		for(int i=0;i<8;++i){
			if( region->intersect( children[i]->boundBox ) ){
				children[i]->getNodesInRegion(region,points,nodes);
			}
		}
	}
	else if( nodeIds != NULL )
	{
		kmb::Point3D point;
		for(int i=0;i<count;++i)
		{
			if( points->getPoint( nodeIds[i], point ) ){
				if( region->intersect( point ) != kmb::Region::OUTSIDE ){
					nodes.insert( nodeIds[i] );
				}
			}
		}
	}
}
