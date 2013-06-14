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
#include "MeshDB/kmbMeshDB.h"

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbNodeMapperBindings.h"
#include "MeshDB/kmbMeshOperation.h"

#include <map>
#include <cstring>







void
kmb::MeshDB::nodeIdDefragmentation(nodeIdType initId)
{
	if( node3Ds == NULL ||
		strcmp( node3Ds->getContainerType(), kmb::Point3DContainerMap::CONTAINER_TYPE ) != 0 )
		return;


	this->neighborInfo.clear();



	std::map<kmb::nodeIdType,kmb::nodeIdType> idmap;

	static_cast< kmb::Point3DContainerMap*>(this->node3Ds)->idDefragment(initId,idmap);


	if( idmap.size() == 0 ){
		return;
	}


	for(std::vector<kmb::ElementContainer*>::iterator cIter = this->bodies.begin();
		cIter != this->bodies.end();
		++cIter)
	{
		kmb::ElementContainer* body = (*cIter);
		if( body ){
			kmb::ElementContainer::iterator eIter = body->begin();
			while( eIter != body->end() ){
				int len = eIter.getNodeCount();
				for(int i=0;i<len;++i){
					nodeIdType oldId = eIter.getCellId(i);
					if( idmap.find(oldId) != idmap.end() ){
						eIter.setCellId(i,idmap[oldId]);
					}
				}
				++eIter;
			}
		}
	}


	std::multimap< std::string, kmb::DataBindings*>::iterator bIter = this->bindings.begin();
	while( bIter != this->bindings.end() )
	{
		kmb::DataBindings* d = bIter->second;
		if( d != NULL ){
			switch(d->getBindingMode()){
				case kmb::DataBindings::NodeGroup:
				{
					if( strcmp( "DataBindingsGroup", d->getContainerType() ) == 0 ){
						reinterpret_cast<kmb::DataBindingsGroup<kmb::nodeIdType>*>(d)->replaceId( idmap );
					}
					break;
				}
				case kmb::DataBindings::NodeVariable:
				{
					if( strcmp( "DataVariable", d->getContainerType() ) == 0 ){
						d->replaceNodeId( idmap );
					}
					break;
				}
				default:
					break;
			}
		}
		++bIter;
	}
}

bool
kmb::MeshDB::replaceNodeId(kmb::nodeIdType oldId, kmb::nodeIdType newId)
{
	if( this->node3Ds
		&& strcmp( node3Ds->getContainerType(), kmb::Point3DContainerMap::CONTAINER_TYPE ) == 0
		&& static_cast< kmb::Point3DContainerMap*>(this->node3Ds)->replaceId(oldId,newId) ){

		std::vector<elementIdType> coboundary;

		getSurroundingElements(oldId,coboundary,false);

		std::vector<elementIdType>::iterator eIter = coboundary.begin();
		while ( eIter != coboundary.end() )
		{
			kmb::elementIdType eid = (*eIter);
			kmb::ElementContainer::iterator e = this->findElement(eid);
			if( !e.isFinished() )
			{
				e.replaceNodeId( oldId, newId );
			}
			++eIter;
		}

		std::multimap< std::string, kmb::DataBindings*>::iterator iterBind = this->bindings.begin();
		while( iterBind != this->bindings.end() )
		{
			kmb::DataBindings* data = iterBind->second;
			if( data != NULL && (
				data->getBindingMode() == kmb::DataBindings::NodeVariable ||
				data->getBindingMode() == kmb::DataBindings::NodeGroup ) )
			{
				data->replaceId( oldId, newId );
			}
			++iterBind;
		}
		return true;
	}else{
		return false;
	}
}



int
kmb::MeshDB::deleteUselessNodes(void)
{
	int count = 0;
	kmb::NodeNeighborInfo neighborInfo;

	neighborInfo.appendCoboundary( this );

	if( this->node3Ds
		&& strcmp( node3Ds->getContainerType(), kmb::Point3DContainerMap::CONTAINER_TYPE ) == 0 )
	{
		kmb::Point3DContainer::iterator nIter = this->node3Ds->begin();
		while( nIter != this->node3Ds->end() )
		{
			kmb::nodeIdType nodeId = nIter.getId();
			++nIter;
			if( neighborInfo.getElementCountAroundNode( nodeId ) == 0 ){
				reinterpret_cast< kmb::Point3DContainerMap*>(this->node3Ds)->deleteId( nodeId );
				++count;
			}
		}
		reinterpret_cast< kmb::Point3DContainerMap*>(this->node3Ds)->updateMinMaxId();
	}
	return count;
}

bool
kmb::MeshDB::replaceNodeIdOfElement
(kmb::bodyIdType bodyId,kmb::elementIdType elementId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId)
{
	kmb::ElementContainer::iterator element = this->findElement(elementId,bodyId);
	if( !element.isFinished() ){
		return element.replaceNodeId(oldNodeId,newNodeId);
	}
	return false;
}

bool
kmb::MeshDB::reverseElement(kmb::elementIdType elementID,kmb::bodyIdType bodyID)
{
	kmb::ElementContainer::iterator elem = this->findElement(elementID,bodyID);
	return elem.reverse();
}

int
kmb::MeshDB::duplicateNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName)
{
	if( meshOperation == NULL ){
		meshOperation = new kmb::MeshOperation(this);
	}
	return meshOperation->duplicateNodeIdOfBody(bodyId,coupleName);









































}


int
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId)
{
	int count = 0;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			if( eIter.replaceNodeId(oldNodeId,newNodeId) ){
				++count;
			}
			++eIter;
		}
	}
	return count;
}


int
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper)
{
	int count = 0;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			count += eIter.replaceNodeId(nodeMapper);
			++eIter;
		}
	}
	return count;
}

int
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName)
{
	int count = 0;
	kmb::nodeIdType orgNodeId = kmb::nullNodeId;
	kmb::nodeIdType newNodeId = kmb::nullNodeId;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	kmb::DataBindings* data = this->getDataBindingsPtr(coupleName);
	long l = 0L;
	if( body && data &&
		strcmp("NodeMapperBindings",data->getContainerType()) == 0 )
	{
		kmb::ElementContainer::iterator eIter = body->begin();
		while( !eIter.isFinished() ){
			int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				orgNodeId = eIter[i];
				if( data->hasId( orgNodeId ) && data->getPhysicalValue( orgNodeId, &l ) ){
					newNodeId = static_cast<kmb::nodeIdType>(l);
					eIter.setCellId(i,newNodeId);
				}
			}
			++eIter;
		}
	}
	return count;
}

void
kmb::MeshDB::reverseBody(kmb::bodyIdType bodyId)
{
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			eIter.reverse();
			++eIter;
		}
	}
}
