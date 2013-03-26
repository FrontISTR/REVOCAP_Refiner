/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeNeighborInfo                                        #
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
#pragma once

#include "MeshDB/kmbTypes.h"
#include <map>
#include <vector>
#include <string>

namespace kmb{

class Element;
class ElementBase;
class MeshData;
class DataBindings;
class ElementContainer;
class Face;

typedef std::multimap< kmb::nodeIdType, kmb::elementIdType > NodeNeighbor;

class NodeNeighborInfo
{
public:
	NodeNeighborInfo(void);
	virtual ~NodeNeighborInfo(void);

	void clear(void);
	bool getIgnoreOrientation(void) const;
	void setIgnoreOrientation(bool f);


private:
	bool append( kmb::nodeIdType nodeID, kmb::elementIdType elementID );
	bool erase( kmb::nodeIdType nodeID, kmb::elementIdType elementID );
public:
	bool appendCoboundary( kmb::elementIdType elementId, const kmb::ElementBase &element );
	bool appendCoboundary( const kmb::ElementContainer* elements );
	bool appendCoboundary( const kmb::MeshData* mesh );
	bool appendCoboundary( const kmb::DataBindings* data, const kmb::ElementContainer* body );
	bool appendCoboundary( const kmb::DataBindings* data, const kmb::MeshData* mesh );

	bool deleteCoboundary( kmb::elementIdType elementId, const kmb::ElementBase &element );








	int getNeighborElements( kmb::elementIdType elementID, kmb::elementIdType *neighbors, const kmb::ElementContainer* elements ) const;
	int getNeighborElements( kmb::elementIdType elementID, kmb::elementIdType *neighbors, kmb::MeshData* mesh ) const;



	int getElementBoundary( const kmb::ElementBase &element, kmb::elementIdType *boundaries, const kmb::ElementContainer* elements ) const;


	kmb::elementIdType getElementFace( const kmb::ElementBase &element, kmb::idType faceId, const kmb::ElementContainer* elements ) const;



	bool getFace( const kmb::ElementBase &element, kmb::Face &face, const kmb::ElementContainer* elements ) const;

	bool getAdjacentFace( const kmb::Face &face, kmb::Face &adj, const kmb::ElementContainer* elements ) const;
	bool getAdjacentFace( const kmb::Face &face, kmb::Face &adj, kmb::MeshData* mesh ) const;
public:

	void getNodeNeighbor( kmb::nodeIdType nodeId, std::set<kmb::nodeIdType> &neighbors, const kmb::ElementContainer* elements );
	void getNodeNeighbor( kmb::nodeIdType nodeId, std::set<kmb::nodeIdType> &neighbors, kmb::MeshData* mesh );


	void getCrownNodeNeighbor
		( kmb::nodeIdType nodeID,
			std::vector<kmb::nodeIdType> &neighbors,
			const kmb::ElementContainer* elements );

	bool isConnected( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, const kmb::ElementContainer* elements ) const;
	bool isConnected( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, kmb::MeshData* mesh ) const;


	size_t getElementCountAroundNode(nodeIdType nodeId) const;

	size_t getSize(void) const;


	NodeNeighbor::iterator beginIteratorAt(kmb::nodeIdType nodeID){
		return coboundaries.lower_bound(nodeID);
	};
	NodeNeighbor::iterator endIteratorAt(kmb::nodeIdType nodeID){
		return coboundaries.upper_bound(nodeID);
	};
	NodeNeighbor::const_iterator beginIteratorAt(kmb::nodeIdType nodeID) const{
		return coboundaries.lower_bound(nodeID);
	};
	NodeNeighbor::const_iterator endIteratorAt(kmb::nodeIdType nodeID) const{
		return coboundaries.upper_bound(nodeID);
	};


	void getSurroundingElements(kmb::nodeIdType nodeId, std::vector<kmb::elementIdType> &elements) const;

	void getSurroundingElementsOf2Node(kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1, std::vector<kmb::elementIdType> &elements) const;



	kmb::elementIdType getNextElementAroundNode( kmb::ElementContainer* triangles, kmb::nodeIdType nodeID, kmb::elementIdType elemID, bool clockwise=false) const;



	void getEndsOfEdge( const kmb::ElementContainer* edges, kmb::nodeIdType& firstID, kmb::nodeIdType& endID ) const;

	NodeNeighbor::iterator beginNodeIterator(void);
	NodeNeighbor::iterator endNodeIterator(void);
private:
	NodeNeighbor coboundaries;
	bool admitAnti;
};

}
