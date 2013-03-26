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
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbTriangle.h"
#include "Common/kmbCalculator.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbMatching.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbNodeMapperBindings.h"

kmb::bodyIdType
kmb::MeshDB::mergeBody(const std::set< kmb::bodyIdType >& bodyIDs)
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	kmb::nodeIdType nodes[20];
	kmb::ElementContainer* body = NULL;
	body = new kmb::ElementContainerMap;
	kmb::bodyIdType id = static_cast<kmb::bodyIdType>( this->bodies.size() );
	this->bodies.push_back(body);
	std::set< kmb::bodyIdType >::const_iterator bIter = bodyIDs.begin();
	while( bIter != bodyIDs.end() )
	{
		kmb::bodyIdType tempId = (*bIter);
		kmb::ElementContainer* tempBody = this->getBodyPtr( tempId );
		if( tempBody != NULL ){
			kmb::ElementContainer::iterator eIter = tempBody->begin();
			while( eIter != tempBody->end() )
			{
				kmb::elementIdType elemId = eIter.getId();
				++eIter;
				if( tempBody->getElement(elemId,etype,nodes) ){
					body->addElement(etype,nodes,elemId);
				}
			}
			tempBody->clear();
		}
		++bIter;
	}
	return id;
}

kmb::bodyIdType
kmb::MeshDB::mergeBody( kmb::bodyIdType appendBodyId, kmb::bodyIdType bodyId, bool moveFlag)
{
	kmb::ElementContainer* body = NULL;
	kmb::ElementContainer* appendBody = NULL;
	if( (body=this->getBodyPtr(bodyId)) != NULL &&
		(appendBody=this->getBodyPtr(appendBodyId)) != NULL )
	{
		kmb::elementType etype = kmb::UNKNOWNTYPE;
		kmb::nodeIdType nodes[20];
		kmb::ElementContainer::iterator eIter = appendBody->begin();
		while( !eIter.isFinished() )
		{
			kmb::elementIdType elemId = eIter.getId();
			++eIter;
			if( appendBody->getElement(elemId,etype,nodes) ){
				if( moveFlag ){
					body->addElement(etype,nodes,elemId);
				}else{
					body->addElement(etype,nodes,generateElementId());
				}
			}
		}
		if( moveFlag ){
			appendBody->clear();
		}
		return bodyId;
	}
	return kmb::Body::nullBodyId;
}

kmb::bodyIdType
kmb::MeshDB::convertToLinearBody( kmb::bodyIdType bodyId )
{
	kmb::ElementContainer* body = this->getBodyPtr( bodyId );
	if( body != NULL ){
		kmb::ElementContainer* linearBody = NULL;
		linearBody = new kmb::ElementContainerMap();
		kmb::nodeIdType* nodes = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
		kmb::ElementContainer::iterator eIter = body->begin();
		kmb::elementType etype = kmb::UNKNOWNTYPE;
		while( !eIter.isFinished() ){
			eIter.getElement( etype, nodes );
			kmb::elementType linearType = kmb::Element::getLinearType( etype );
			linearBody->addElement( linearType, nodes, eIter.getId() );
			++eIter;
		}
		delete[] nodes;
		return this->replaceBody( bodyId, linearBody, false );
	}
	return kmb::Body::nullBodyId;
}















kmb::bodyIdType
kmb::MeshDB::importBody(kmb::MeshData& otherMesh, kmb::bodyIdType bodyId, const char* coupleName,kmb::coupleType ctype)
{
	kmb::ElementContainer* otherBody = otherMesh.getBodyPtr( bodyId );
	if( otherBody == NULL ){
		return kmb::Body::nullBodyId;
	}

	if( otherMesh.getNodeDim() == 3 ){
		if( node3Ds == NULL ){
			node3Ds = new kmb::Point3DContainerMap();
		}
	}else{
		return kmb::Body::nullBodyId;
	}

	kmb::bodyIdType myBodyId = this->beginElement( otherBody->getCount() );

	kmb::DataBindings* masterCoupleData = NULL;
	kmb::DataBindings* slaveCoupleData = NULL;
	if( coupleName != NULL ){
		if( ctype == kmb::MASTER || ctype == kmb::BOTH ){
			kmb::DataBindings* data = this->getDataBindingsPtr(coupleName);
			if( data ){
				if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
					data->getValueType() == kmb::PhysicalValue::Integer ){
					masterCoupleData = data;
				}
			}else{
				masterCoupleData = new kmb::NodeMapperBindings< kmb::nodeIdType >();
				masterCoupleData->setTargetBodyId( bodyId );
				this->setDataBindingsPtr( coupleName, masterCoupleData, "import" );
			}
		}
		if( ctype == kmb::SLAVE || ctype == kmb::BOTH ){
			kmb::DataBindings* data = otherMesh.getDataBindingsPtr(coupleName);
			if( data ){
				if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
					data->getValueType() == kmb::PhysicalValue::Integer ){
					slaveCoupleData = data;
				}
			}else{
				slaveCoupleData = new kmb::NodeMapperBindings< kmb::nodeIdType >();
				slaveCoupleData->setTargetBodyId( myBodyId );
				otherMesh.setDataBindingsPtr( coupleName, slaveCoupleData, "import" );
			}
		}
	}

	kmb::Node node;



	kmb::nodeIdType* cell = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	kmb::ElementContainer::iterator eIter = otherBody->begin();
	long l = 0;
	if( slaveCoupleData && masterCoupleData ){

		while( !eIter.isFinished() ){
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				kmb::nodeIdType yourId = eIter.getCellId(i);
				cell[i] = kmb::nullNodeId;
				if( slaveCoupleData->getPhysicalValue( yourId, &l ) ){
					cell[i] = static_cast< kmb::nodeIdType >( l );
				}else if( otherMesh.getNode( yourId, node ) ){
					cell[i] = this->addNode( node );
					l = cell[i];
					slaveCoupleData->setPhysicalValue( yourId, &l );
					l = yourId;
					masterCoupleData->setPhysicalValue( cell[i], &l );
				}
			}
			this->addElement( eIter.getType(), cell );
			++eIter;
		}
	}else if( slaveCoupleData ){

		while( !eIter.isFinished() ){
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				kmb::nodeIdType yourId = eIter.getCellId(i);
				cell[i] = kmb::nullNodeId;
				if( slaveCoupleData->getPhysicalValue( yourId, &l ) ){
					cell[i] = static_cast< kmb::nodeIdType >( l );
				}else if( otherMesh.getNode( yourId, node ) ){
					cell[i] = this->addNode( node );
					l = cell[i];
					slaveCoupleData->setPhysicalValue( yourId, &l );
				}
			}
			this->addElement( eIter.getType(), cell );
			++eIter;
		}
	}else if( masterCoupleData ){



		std::map< kmb::nodeIdType, kmb::nodeIdType > nodeMapper;
		while( !eIter.isFinished() ){
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				kmb::nodeIdType yourId = eIter.getCellId(i);
				cell[i] = kmb::nullNodeId;
				std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator nIter = nodeMapper.find( yourId );
				if( nIter != nodeMapper.end() ){
					cell[i] = nIter->second;
				}else if( otherMesh.getNode( yourId, node ) ){
					cell[i] = this->addNode( node );
					nodeMapper.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( yourId, cell[i] ) );
					l = yourId;
					masterCoupleData->setPhysicalValue( cell[i], &l );
				}
			}
			this->addElement( eIter.getType(), cell );
			++eIter;
		}
	}else{

		std::map< kmb::nodeIdType, kmb::nodeIdType > nodeMapper;
		while( !eIter.isFinished() ){
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				kmb::nodeIdType yourId = eIter.getCellId(i);
				cell[i] = kmb::nullNodeId;
				std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator nIter = nodeMapper.find( yourId );
				if( nIter != nodeMapper.end() ){
					cell[i] = nIter->second;
				}else if( otherMesh.getNode( yourId, node ) ){
					cell[i] = this->addNode( node );
					nodeMapper.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( yourId, cell[i] ) );
				}
			}
			this->addElement( eIter.getType(), cell );
			++eIter;
		}
	}
	this->endElement();
	delete[] cell;
	return myBodyId;
}

kmb::bodyIdType
kmb::MeshDB::importBodyWithNodeMatching(kmb::MeshData& otherMesh, kmb::bodyIdType bodyId, double tolerance, const char* coupleName)
{
	kmb::ElementContainer* otherBody = otherMesh.getBodyPtr( bodyId );
	if( otherBody == NULL ){
		return kmb::Body::nullBodyId;
	}

	if( otherMesh.getNodeDim() == 3 ){
		if( node3Ds == NULL ){
			node3Ds = new kmb::Point3DContainerMap();
		}
	}else{
		return kmb::Body::nullBodyId;
	}

	kmb::DataBindings* coupleData = NULL;
	if( coupleName != NULL ){
		kmb::DataBindings* data = otherMesh.getDataBindingsPtr(coupleName);
		if( data ){
			if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
			    data->getValueType() == kmb::PhysicalValue::Integer ){
				coupleData = data;
			}
		}else{
			coupleData = new kmb::NodeMapperBindings< kmb::nodeIdType >();
			coupleData->setTargetBodyId( bodyId );
			otherMesh.setDataBindingsPtr( coupleName, coupleData, "import" );
		}
	}

	kmb::Node node;
	long l = 0;



	kmb::bodyIdType myBodyId = this->beginElement( otherBody->getCount() );
	kmb::nodeIdType* cell = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	kmb::nodeIdType nearestNodeId = kmb::nullNodeId;
	kmb::ElementContainer::iterator eIter = otherBody->begin();
	if( coupleData ){

		while( !eIter.isFinished() ){
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				kmb::nodeIdType yourId = eIter.getCellId(i);
				cell[i] = kmb::nullNodeId;
				if( coupleData->getPhysicalValue( yourId, &l ) ){
					cell[i] = static_cast< kmb::nodeIdType >(l);
				}else if( otherMesh.getNode( yourId, node ) ){
					if( this->getNearestNode( node.x(), node.y(), node.z(), nearestNodeId ) < tolerance ){
						cell[i] = nearestNodeId;
					}else{
						cell[i] = this->addNode( node );
					}
					l = cell[i];
					coupleData->setPhysicalValue( yourId, &l );
				}
			}
			this->addElement( eIter.getType(), cell );
			++eIter;
		}
	}else{

		std::map< kmb::nodeIdType, kmb::nodeIdType > nodeMapper;
		while( !eIter.isFinished() ){
			const int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				kmb::nodeIdType yourId = eIter.getCellId(i);
				cell[i] = kmb::nullNodeId;
				std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator nIter = nodeMapper.find( yourId );
				if( nIter != nodeMapper.end() ){
					cell[i] = nIter->second;
				}else if( otherMesh.getNode( yourId, node ) ){
					if( this->getNearestNode( node.x(), node.y(), node.z(), nearestNodeId ) < tolerance ){
						cell[i] = nearestNodeId;
					}else{
						cell[i] = this->addNode( node );
					}
					nodeMapper.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >( yourId, cell[i] ) );
				}
			}
			this->addElement( eIter.getType(), cell );
			++eIter;
		}
	}
	this->endElement();
	delete[] cell;
	return myBodyId;
}










kmb::bodyIdType
kmb::MeshDB::importBody(kmb::MeshData& otherMesh,kmb::bodyIdType bodyID,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper)
{
	kmb::ElementContainer* otherBody = otherMesh.getBodyPtr( bodyID );
	if( otherBody == NULL ){
		return kmb::Body::nullBodyId;
	}
	std::set< kmb::nodeIdType > nodeSet;
	otherBody->getNodesOfBody( nodeSet );
	if( node3Ds == NULL && otherMesh.getNodeDim() == 3 ){
		node3Ds = new kmb::Point3DContainerMap();
	}


	std::set< kmb::nodeIdType >::iterator nIter = nodeSet.begin();
	while( nIter != nodeSet.end() ){
		kmb::nodeIdType yourID = (*nIter);

		if( nodeMapper.find( yourID ) == nodeMapper.end() ){
			kmb::Node node;
			if( otherMesh.getNode( yourID, node ) ){

				kmb::nodeIdType myID = this->addNode( node );
				nodeMapper.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >(yourID,myID) );
			}else{
				nodeMapper.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >(yourID,kmb::nullNodeId) );
			}
		}
		++nIter;
	}
	nodeSet.clear();

	kmb::ElementContainerMap* body = new kmb::ElementContainerMap();
	kmb::nodeIdType* cell = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	kmb::ElementContainer::iterator eIter = otherBody->begin();
	while( eIter != otherBody->end() ){
		kmb::ElementRelation::clone( eIter, cell, nodeMapper );
		body->addElement( eIter.getType(), cell );
		++eIter;
	}
	delete[] cell;
	kmb::bodyIdType myBodyID = this->appendBody( body );
	return myBodyID;
}

kmb::nodeIdType
kmb::MeshDB::importNode(kmb::MeshDB& otherMesh)
{
	if( this->getNodeDim() == -1 ){
		this->beginNode( otherMesh.getNodeCount() );
		this->endNode();
	}
	if( this->getNodeDim() == otherMesh.getNodeDim() ){
		if( node3Ds ){
			double x=0.0,y=0.0,z=0.0;
			kmb::nodeIdType offsetNodeId = node3Ds->getMaxId() + 1;
			kmb::Point3DContainer::const_iterator pIter = otherMesh.getNodes()->begin();
			kmb::Point3DContainer::const_iterator endIter = otherMesh.getNodes()->end();
			while( pIter != endIter ){
				pIter.getXYZ(x,y,z);
				node3Ds->addPoint( x, y, z, offsetNodeId+pIter.getId() );
				++pIter;
			}
			return offsetNodeId;
		}else if( node2Ds ){
			double x=0.0,y=0.0;
			kmb::nodeIdType offsetNodeId = node2Ds->getMaxId() + 1;
			kmb::Point2DContainer::const_iterator pIter = otherMesh.getNode2Ds()->begin();
			kmb::Point2DContainer::const_iterator endIter = otherMesh.getNode2Ds()->end();
			while( pIter != endIter ){
				pIter.getXY(x,y);
				node2Ds->addPoint( x, y, offsetNodeId+pIter.getId() );
				++pIter;
			}
			return offsetNodeId;
		}
	}
	return kmb::nullNodeId;
}

size_t
kmb::MeshDB::importAllBody(const kmb::MeshData& otherMesh)
{
	if( otherMesh.getNodes() == NULL ){
		return 0;
	}
	size_t appendElementCount = 0;
	if( node3Ds == NULL ){
		node3Ds = new kmb::Point3DContainerMap();
	}

	kmb::nodeIdType offsetNodeId = this->getMaxNodeId() + 1;
	kmb::Point3D pt;
	kmb::Point3DContainer::const_iterator pIter = otherMesh.getNodes()->begin();
	while( !pIter.isFinished() ){
		if( pIter.getPoint( pt ) ){
			this->addNodeWithId( pt, pIter.getId() + offsetNodeId );
		}
		++pIter;
	}

	kmb::bodyIdType bCount = otherMesh.getBodyCount();
	kmb::nodeIdType nodeTable[20];
	for(kmb::bodyIdType i=0;i<bCount;++i){
		const kmb::ElementContainer* body = otherMesh.getBodyPtr(i);
		if( body ){
			appendElementCount += body->getCount();
			kmb::bodyIdType bodyId = this->beginElement( body->getCount() );
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				int len = eIter.getNodeCount();
				for(int j=0;j<len;++j){
					nodeTable[j] = eIter[j] + offsetNodeId;
				}
				this->addElement( eIter.getType(), nodeTable );
				++eIter;
			}
			this->endElement();
			this->setBodyName(bodyId,body->getBodyName());
		}
	}
	return appendElementCount;
}
