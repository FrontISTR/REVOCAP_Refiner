/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : MeshDB                                                  #
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
#include "MeshDB/kmbMeshDB.h"

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"

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


	for(std::vector<kmb::ElementContainer*>::iterator p = this->bodies.begin();
		p != this->bodies.end();
		p++)
	{
		kmb::ElementContainer* body = (*p);
		if( body ){
			kmb::ElementContainer::iterator eIter = body->begin();
			while( eIter != body->end() )
			{
				int len = eIter.getNodeCount();
				for(int i=0;i<len;++i){
					nodeIdType oldId =	eIter.getCellId(i);
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
				case kmb::DataBindings::NODEGROUP:
				{
					kmb::DataBindingsGroup<kmb::nodeIdType>* data =
						static_cast<kmb::DataBindingsGroup<kmb::nodeIdType>*>(d);
					data->replaceId( idmap );
					break;
				}
				case kmb::DataBindings::NODEVARIABLE:
				{
					kmb::DataBindingsEach<kmb::nodeIdType>* data =
						static_cast<kmb::DataBindingsEach<kmb::nodeIdType>*>(d);
					data->replaceId( idmap );
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
		&& static_cast< kmb::Point3DContainerMap*>(this->node3Ds)->replaceID(oldId,newId) ){

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
				data->getBindingMode() == kmb::DataBindings::NODEVARIABLE ||
				data->getBindingMode() == kmb::DataBindings::NODEGROUP ) )
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
			kmb::nodeIdType nodeID = nIter.getId();
			++nIter;
			if( neighborInfo.getElementCountAroundNode( nodeID ) == 0 ){
				reinterpret_cast< kmb::Point3DContainerMap*>(this->node3Ds)->deleteID( nodeID );
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


bool
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId)
{
	bool ret = false;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			ret = ( eIter.replaceNodeId(oldNodeId,newNodeId) || ret );
			++eIter;
		}
	}else{
		ret = false;
	}
	return ret;
}


bool
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper)
{
	bool ret = false;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			ret = ( eIter.replaceNodeId(nodeMapper)>0 || ret );
			++eIter;
		}
	}else{
		ret = false;
	}
	return ret;
}

void
kmb::MeshDB::reverseBody(kmb::bodyIdType bodyID)
{
	kmb::ElementContainer* body = this->getBodyPtr(bodyID);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			eIter.reverse();
			++eIter;
		}
	}
}
