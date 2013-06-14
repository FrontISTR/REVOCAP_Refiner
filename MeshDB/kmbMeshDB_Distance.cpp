/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshDB                                                  #
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
#include <cfloat>

#include "MeshDB/kmbMeshDB.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbBucket.h"
#include "Geometry/kmbBoundingBox.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbMeshOperation.h"

double
kmb::MeshDB::getDistance(kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1) const
{
	if( this->node3Ds ){
		kmb::Point3D p0, p1;
		double d = this->node3Ds->distanceSq(nodeId0,nodeId1);
		if( d >= 0.0 ){
			return sqrt(d);
		}
	}
	return DBL_MAX;
}



double
kmb::MeshDB::getNearestNodeInBody(double x,double y,double z,kmb::bodyIdType bodyID,kmb::nodeIdType& nearestId) const
{
	kmb::Minimizer minimizer;
	kmb::Node node;
	const kmb::ElementContainer* body = this->getBodyPtr(bodyID);
	if( body != NULL && this->node3Ds != NULL ){
		ElementContainer::const_iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				const kmb::nodeIdType nodeId = eIter.getCellId(i);
				if( this->getNode( nodeId, node ) ){
					if( minimizer.update( node.distanceSq(x,y,z) ) ){
						nearestId = nodeId;
					}
				}
			}
			++eIter;
		}
	}
	return sqrt( minimizer.getMin() );
}

double
kmb::MeshDB::getNearestNodeInBody(const kmb::Point3D& point, kmb::bodyIdType bodyId, kmb::nodeIdType& nearestId) const
{
	kmb::Minimizer minimizer;
	kmb::Node node;
	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body != NULL && this->node3Ds != NULL ){
		ElementContainer::const_iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				const kmb::nodeIdType nodeId = eIter.getCellId(i);
				if( this->getNode( nodeId, node ) ){
					if( minimizer.update( node.distanceSq( point ) ) ){
						nearestId = nodeId;
					}
				}
			}
			++eIter;
		}
	}
	return sqrt( minimizer.getMin() );
}


double
kmb::MeshDB::getNearestNode(const kmb::nodeIdType id,kmb::nodeIdType& nearestId) const
{
	kmb::Minimizer minimizer;
	kmb::Node node,target;
	if(this->node3Ds != NULL && this->getNode(id,node)){
		kmb::Point3DContainer::const_iterator nIter = this->node3Ds->begin();
		while( nIter != this->node3Ds->end() )
		{
			kmb::nodeIdType target_id = nIter.getId();
			if( target_id != id && nIter.getPoint(target)){
				if( minimizer.update( target.distanceSq( node ) ) ){
					nearestId = nIter.getId();
				}
			}
			++nIter;
		}
	}
	return sqrt( minimizer.getMin() );
}

double
kmb::MeshDB::getNearestNodeInBody(const kmb::nodeIdType id,kmb::bodyIdType bodyID,kmb::nodeIdType& nearestId) const
{
	kmb::Minimizer minimizer;
	kmb::Node node,targetNode;
	const kmb::ElementContainer* body = this->getBodyPtr(bodyID);
	if( body != NULL && this->node3Ds != NULL && this->getNode(id,node) ){
		ElementContainer::const_iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				const kmb::nodeIdType target_nodeId = eIter.getCellId(i);
				if( this->getNode( target_nodeId, targetNode ) && id != target_nodeId ){
					if( minimizer.update( targetNode.distanceSq( node ) ) ){
						nearestId = target_nodeId;
					}
				}
			}
			++eIter;
		}
	}
	return sqrt( minimizer.getMin() );
}

double
kmb::MeshDB::getDistanceToElement(double x,double y,double z,kmb::elementIdType elementId,kmb::bodyIdType bodyId) const
{
	kmb::ElementContainer::const_iterator eIter = this->findElement(elementId,bodyId);
	return sqrt( this->evaluator->getDistanceSq(eIter,x,y,z) );
}

size_t kmb::MeshDB::uniteNodes(double thresh)
{
	if( meshOperation == NULL ){
		meshOperation = new kmb::MeshOperation(this);
	}
	return meshOperation->uniteNodes(thresh);
}
