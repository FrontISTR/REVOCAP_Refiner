/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Element                                                 #
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
/*
 * 1次元要素 connectionTable
 * 2次元要素 connectionTable faceTable
 * 3次元要素 connectionTable faceTable edgeTable
 *
 * 2次要素について面、辺の1次元要素は SEGMENT2
 * 2次元要素の faceTable の第3成分は index 順にソートされている
 * 3次元要素の edgeTable の第3成分は index 順にソートされている
 *
 */

#pragma once

#include <map>
#include <string>

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElementType.h"

namespace kmb{

class MeshData;
class Tetrahedron;




class ElementBase
{
public:
	ElementBase(void){}
	virtual ~ElementBase(void){}


	static int getNodeCount(kmb::elementType type);


	static int getVertexCount(kmb::elementType type);
	static int getBoundaryCount(kmb::elementType type);
	static int getEdgeCount(kmb::elementType type);
	static kmb::elementType getBoundaryType(kmb::elementType type,int index);
	static int getBoundaryNodeCount(kmb::elementType type,int index);
	static int getBoundaryVertexCount(kmb::elementType type,int index);
	static kmb::elementType getEdgeType(kmb::elementType type,int index);
	static int getEdgeNodeCount(kmb::elementType type,int index);
	static int getEdgeVertexCount(kmb::elementType type,int index);
	static int getDimension(kmb::elementType type);
	static int getDegree(kmb::elementType type);
	static const std::string getTypeString(kmb::elementType type);


	static kmb::elementType getType(std::string str);
	static kmb::elementType getLinearType(kmb::elementType type);
	static kmb::elementType getSecondType(kmb::elementType type);
	static int getDiagIndex(kmb::elementType type,int index);

	virtual kmb::elementType getType(void) const = 0;
	virtual kmb::nodeIdType getCellId(int cellIndex) const = 0;
	virtual bool setCellId(int cellIndex, kmb::nodeIdType nodeId) = 0;
	virtual kmb::nodeIdType operator[](const int i) const = 0;


	int getNodeCount(void) const;
	int getVertexCount(void) const;
	int getBoundaryCount(void) const;
	int getEdgeCount(void) const;
	kmb::elementType getBoundaryType(int index) const;
	kmb::elementType getEdgeType(int index) const;
	int getBoundaryNodeCount(int index) const;
	int getBoundaryVertexCount(int index) const;
	int getEdgeNodeCount(int index) const;
	int getEdgeVertexCount(int index) const;
	kmb::nodeIdType getBoundaryCellId(int index,int i) const;
	kmb::nodeIdType getEdgeCellId(int index,int i) const;
	int getDimension(void) const;
	int getDegree(void) const;
	const std::string getTypeString(void) const;
	kmb::elementType getLinearType(void) const;
	kmb::elementType getSecondType(void) const;
	int getDiagIndex(int index) const;

	bool include(kmb::nodeIdType nodeId) const;
	bool includeVertex(kmb::nodeIdType nodeId) const;

	int indexOf(kmb::nodeIdType nodeId) const;
	int countCommonNode(kmb::ElementBase& other) const;
	bool replaceNodeId(kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId);
	int replaceNodeId(std::map<nodeIdType,nodeIdType> &idmap);

	kmb::elementType getBoundaryElement(int index,kmb::nodeIdType* cell) const;
	bool getBoundaryElement(int index,kmb::ElementBase& elem) const;
	kmb::elementType getEdgeElement(int index,kmb::nodeIdType* cell) const;
	bool getEdgeElement(int index,kmb::ElementBase& elem) const;





	int isConnected(int index0,int index1) const;






	int isFace(int index0,int index1,int &faceIndex) const;
	int isFace(int index0,int index1,int index2,int &faceIndex) const;
	int isFace(int index0,int index1,int index2,int index3,int &faceIndex) const;
	int isFace(const kmb::ElementBase &element, int &faceIndex) const;

	int getFaceIndex(int index0,int index1) const;

	bool reverse(void);
	static bool reverse( kmb::elementType etype, kmb::nodeIdType* nodes);

	int divideIntoTriangles(kmb::nodeIdType triangles[][3]) const;

	int divideIntoTetrahedrons(kmb::nodeIdType tetrahedrons[][4]) const;


	int getIndexMinNodeIdOfFace(int index) const;
	int getIndexMinNodeId(void) const;

	kmb::nodeIdType nextNode(kmb::nodeIdType nodeId) const;
	kmb::nodeIdType previousNode(kmb::nodeIdType nodeId) const;
};

class Element : public ElementBase
{
public:

	static kmb::elementIdType nullElementId;



	static const int MAX_NODE_COUNT;
	static const int MAX_NODE_COUNT_DIM2;

	Element(kmb::elementType t = UNKNOWNTYPE);
	virtual ~Element(void);


	Element* clone() const;
	Element* reverseClone() const;

	Element* clone( std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper ) const;
	Element* reverseClone( std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper ) const;




	static Element* create(kmb::elementType type,kmb::nodeIdType *ary=NULL);

	virtual kmb::elementType getType() const;
public:

	virtual bool setCellId(int index,kmb::nodeIdType nodeId);
	virtual kmb::nodeIdType getCellId(int index) const;
	virtual kmb::nodeIdType operator[](const int i) const;
	virtual kmb::nodeIdType& operator[](const int i);
protected:
	elementType type;
	nodeIdType *cell;
};


}
