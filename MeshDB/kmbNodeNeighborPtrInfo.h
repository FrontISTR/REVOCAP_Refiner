/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : NodeNeighborPtrInfo                                     #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/*
 * kmb::NodeNeighborInfo と同等の機能
 * ただし、elementId との対応ではなくて、kmb::Element* との対応を保存する
 */

#pragma once

#include "MeshDB/kmbTypes.h"
#include <map>

namespace kmb{

class Element;
class ElementContainer;
class MeshData;




typedef std::multimap< kmb::nodeIdType, kmb::Element* > NodeNeighborPtr;

class NodeNeighborPtrInfo
{
public:
	NodeNeighborPtrInfo(void);
	virtual ~NodeNeighborPtrInfo(void);

	void clear(void);
private:

	bool append( kmb::nodeIdType nodeId, kmb::Element* element );
	bool erase( kmb::nodeIdType nodeId, kmb::Element* element );
public:
	bool appendCoboundary( kmb::Element* element );
	bool appendCoboundary( kmb::ElementContainer* elements );
	bool appendCoboundary( kmb::MeshData* mesh );

	bool deleteCoboundary( kmb::Element* element );







	int getElementNeighbor( const kmb::Element* element, kmb::Element** neighbors ) const;
	int getElementNeighborFace( const kmb::Element* element, kmb::Element** neighbors, int *faces ) const;

	void getNodeNeighbor( kmb::nodeIdType nodeId, std::set<kmb::nodeIdType> &neighbors ) const;

	bool isConnected( kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1 ) const;

	size_t getElementCountAroundNode(kmb::nodeIdType nodeId);

	size_t getSize(void);


	NodeNeighborPtr::iterator beginIteratorAt(kmb::nodeIdType nodeId){
		return coboundaries.lower_bound(nodeId);
	};
	NodeNeighborPtr::iterator endIteratorAt(kmb::nodeIdType nodeId){
		return coboundaries.upper_bound(nodeId);
	};

	NodeNeighborPtr::iterator beginNodeIterator(void);
	NodeNeighborPtr::iterator nextNodeIterator(kmb::nodeIdType nodeId);
	NodeNeighborPtr::iterator endNodeIterator(void);
private:
	NodeNeighborPtr	coboundaries;
};

}
