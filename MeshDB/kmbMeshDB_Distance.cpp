/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : MeshDB                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbTriangle.h"
#include "Geometry/kmb_Calculator.h"
#include "Geometry/kmb_Geometry3D.h"
#include "MeshDB/kmbElementContainer.h"

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
kmb::MeshDB::getNearestNodeInBody(double x,double y,double z,kmb::bodyIdType bodyID,kmb::nodeIdType& nearestId)
{
	kmb::Minimizer minimizer;
	kmb::Node node;
	kmb::ElementContainer* body = this->getBodyPtr(bodyID);
	if( body != NULL && this->node3Ds != NULL ){
		ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				const kmb::nodeIdType nodeID = eIter.getCellId(i);
				if( this->getNode( nodeID, node ) ){
					if( minimizer.update( node.distanceSq(x,y,z) ) ){
						nearestId = nodeID;
					}
				}
			}
			++eIter;
		}
	}
	return sqrt( minimizer.getMin() );
}

double
kmb::MeshDB::getNearestNodeInBody(const kmb::Point3D& point, kmb::bodyIdType bodyId, kmb::nodeIdType& nearestId)
{
	kmb::Minimizer minimizer;
	kmb::Node node;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body != NULL && this->node3Ds != NULL ){
		ElementContainer::iterator eIter = body->begin();
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
		kmb::Point3DContainer::iterator nIter = this->node3Ds->begin();
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
kmb::MeshDB::getNearestNodeInBody(const kmb::nodeIdType id,kmb::bodyIdType bodyID,kmb::nodeIdType& nearestId)
{
	kmb::Minimizer minimizer;
	kmb::Node node,targetNode;
	kmb::ElementContainer* body = this->getBodyPtr(bodyID);
	if( body != NULL && this->node3Ds != NULL && this->getNode(id,node) ){
		ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				const kmb::nodeIdType target_nodeID = eIter.getCellId(i);
				if( this->getNode( target_nodeID, targetNode ) && id != target_nodeID ){
					if( minimizer.update( targetNode.distanceSq( node ) ) ){
						nearestId = target_nodeID;
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
