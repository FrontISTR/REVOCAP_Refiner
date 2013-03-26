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
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementRelation.h"

#include <set>
#include <map>



kmb::bodyIdType kmb::MeshDB::
cloneBody(kmb::bodyIdType bodyID)
{
	kmb::ElementContainer* orgBody = this->getBodyPtr(bodyID);
	kmb::bodyIdType id = kmb::Body::nullBodyId;
	kmb::nodeIdType* cells = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	if( orgBody != NULL && cells != NULL ){
		id = this->beginElement( orgBody->getCount() );

		kmb::ElementContainer::iterator
			eIter = orgBody->begin();
		while( eIter != orgBody->end() )
		{
			kmb::ElementRelation::clone( eIter, cells );
			this->addElement( eIter.getType(), cells );
			++eIter;
		}
		this->endElement();
		delete cells;
	}
	return id;
}

kmb::bodyIdType kmb::MeshDB::
cloneReverseBody(kmb::bodyIdType bodyID)
{
	kmb::ElementContainer* orgBody = this->getBodyPtr(bodyID);
	kmb::bodyIdType id = kmb::Body::nullBodyId;
	kmb::nodeIdType* cells = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	if( orgBody != NULL && cells != NULL ){
		id = this->beginElement( orgBody->getCount() );

		kmb::ElementContainer::iterator eIter = orgBody->begin();
		while( eIter != orgBody->end() )
		{
			kmb::ElementRelation::reverse( eIter, cells );
			this->addElement( eIter.getType(), cells );
			++eIter;
		}
		this->endElement();
		delete cells;
	}
	return id;
}

kmb::bodyIdType
kmb::MeshDB::cloneBodyWithNode(bodyIdType bodyID)
{
	kmb::ElementContainer* orgBody = this->getBodyPtr(bodyID);
	kmb::bodyIdType id = kmb::Body::nullBodyId;
	kmb::Node node;
	kmb::nodeIdType* cells = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	if( orgBody != NULL && cells != NULL ){

		std::set<kmb::nodeIdType> nodeSet;

		std::map<kmb::nodeIdType,kmb::nodeIdType> nodeMapper;
		orgBody->getNodesOfBody( nodeSet );
		std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
		while( nIter != nodeSet.end() )
		{
			kmb::nodeIdType oldID = *nIter;
			if( this->getNode( oldID, node ) ){
				kmb::nodeIdType newID = this->addNode( node );
				nodeMapper.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(oldID,newID) );
			}
			++nIter;
		}


		id = this->beginElement( orgBody->getCount() );
		kmb::ElementContainer::iterator eIter = orgBody->begin();
		while( eIter != orgBody->end() )
		{
			kmb::ElementRelation::clone( eIter, cells, nodeMapper );
			this->addElement( eIter.getType(), cells );
			++eIter;
		}
		this->endElement();
		delete cells;
	}
	return id;
}

kmb::bodyIdType
kmb::MeshDB::cloneReverseBodyWithNode(bodyIdType bodyID)
{
	kmb::ElementContainer* orgBody = this->getBodyPtr(bodyID);
	kmb::bodyIdType id = kmb::Body::nullBodyId;
	kmb::Node node;
	kmb::nodeIdType* cells = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	if( orgBody != NULL && cells != NULL ){

		std::set<kmb::nodeIdType> nodeSet;

		std::map<kmb::nodeIdType,kmb::nodeIdType> nodeMapper;
		orgBody->getNodesOfBody( nodeSet );
		std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
		while( nIter != nodeSet.end() )
		{
			kmb::nodeIdType oldID = *nIter;
			if( this->getNode( oldID, node ) ){
				kmb::nodeIdType newID = this->addNode( node );
				nodeMapper.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(oldID,newID) );
			}
			++nIter;
		}


		id = this->beginElement( orgBody->getCount() );
		kmb::ElementContainer::iterator eIter = orgBody->begin();
		while( eIter != orgBody->end() )
		{
			kmb::ElementRelation::reverse( eIter, cells, nodeMapper );
			this->addElement( eIter.getType(), cells );
			++eIter;
		}
		this->endElement();
		delete cells;
	}
	return id;
}
