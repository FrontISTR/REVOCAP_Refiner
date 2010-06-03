/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Element                                                 #
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
/*
 * ÇPéüå≥óvëf connectionTable
 * ÇQéüå≥óvëf connectionTable faceTable
 * ÇRéüå≥óvëf connectionTable faceTable edgeTable
 *
 * ÇQéüóvëfÇÃ faceTable ÇÃëÊÇRê¨ï™ÇÕ index èáÇ…É\Å[ÉgÇ≥ÇÍÇƒÇ¢ÇÈ
 * ÇRéüóvëfÇÃ edgeTable ÇÃëÊÇRê¨ï™ÇÕ index èáÇ…É\Å[ÉgÇ≥ÇÍÇƒÇ¢ÇÈ
 *
 */

#pragma once

#include <map>
#include <string>

#include "MeshDB/kmbTypes.h"

namespace kmb{

enum elementType{
	UNKNOWNTYPE = -1,
	SEGMENT,
	SEGMENT2,
	TRIANGLE,
	TRIANGLE2,
	QUAD,
	QUAD2,
	TETRAHEDRON,
	TETRAHEDRON2,
	WEDGE,
	WEDGE2,
	PYRAMID,
	PYRAMID2,
	HEXAHEDRON,
	HEXAHEDRON2,
	ELEMENT_TYPE_NUM,
	VERTEX,
	POLYGON,
	TWO_DIM_ONLY,
	THREE_DIM_ONLY,
	MIXED,
	OTHERTYPE
};

class MeshDB;
class Tetrahedron;




class ElementBase
{
public:
	ElementBase(void){}
	virtual ~ElementBase(void){}


	static const int getNodeCount(kmb::elementType type);


	static const int getVertexCount(kmb::elementType type);
	static const int getBoundaryCount(kmb::elementType type);
	static const int getEdgeCount(kmb::elementType type);
	static const kmb::elementType getBoundaryType(kmb::elementType type,int index);
	static const int getBoundaryNodeCount(kmb::elementType type,int index);
	static const int getBoundaryVertexCount(kmb::elementType type,int index);
	static const kmb::elementType getEdgeType(kmb::elementType type,int index);
	static const int getEdgeNodeCount(kmb::elementType type,int index);
	static const int getEdgeVertexCount(kmb::elementType type,int index);
	static const int getDimension(kmb::elementType type);
	static const int getDegree(kmb::elementType type);
	static const std::string getTypeString(kmb::elementType type);

	static const kmb::elementType getType(std::string str);
	static const kmb::elementType getLinearType(kmb::elementType type);
	static const kmb::elementType getSecondType(kmb::elementType type);
	static const int getDiagIndex(kmb::elementType type,int index);

	virtual const kmb::elementType getType(void) const = 0;
	virtual const kmb::nodeIdType getCellId(int cellIndex) const = 0;
	virtual bool setCellId(int cellIndex, kmb::nodeIdType nodeId) = 0;
	virtual kmb::nodeIdType operator[](const int i) const = 0;


	const int getNodeCount(void) const;
	const int getVertexCount(void) const;
	const int getBoundaryCount(void) const;
	const int getEdgeCount(void) const;
	const kmb::elementType getBoundaryType(int index) const;
	const kmb::elementType getEdgeType(int index) const;
	const int getBoundaryNodeCount(int index) const;
	const int getBoundaryVertexCount(int index) const;
	const int getEdgeNodeCount(int index) const;
	const int getEdgeVertexCount(int index) const;
	const kmb::nodeIdType getBoundaryCellId(int index,int i) const;
	const kmb::nodeIdType getEdgeCellId(int index,int i) const;
	const int getDimension(void) const;
	const int getDegree(void) const;
	const std::string getTypeString(void) const;
	const kmb::elementType getLinearType(void) const;
	const kmb::elementType getSecondType(void) const;
	const int getDiagIndex(int index) const;

	const bool include(kmb::nodeIdType nodeId) const;
	const bool includeVertex(kmb::nodeIdType nodeId) const;

	const int indexOf(kmb::nodeIdType nodeId) const;
	const int countCommonNode(kmb::ElementBase& other) const;
	bool replaceNodeId(kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId);
	int replaceNodeId(std::map<nodeIdType,nodeIdType> &idmap);

	const elementType getBoundaryElement(int index,kmb::nodeIdType* cell) const;
	bool getBoundaryElement(int index,kmb::ElementBase& elem) const;
	const elementType getEdgeElement(int index,kmb::nodeIdType* cell) const;
	bool getEdgeElement(int index,kmb::ElementBase& elem) const;





	const int isConnected(int index0,int index1) const;






	const int isFace(int index0,int index1,int &faceIndex) const;
	const int isFace(int index0,int index1,int index2,int &faceIndex) const;
	const int isFace(int index0,int index1,int index2,int index3,int &faceIndex) const;
	const int isFace(const kmb::ElementBase &element, int &faceIndex) const;

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

	virtual const kmb::elementType getType() const;
public:

	virtual bool setCellId(int index,kmb::nodeIdType nodeId);
	virtual const kmb::nodeIdType getCellId(int index) const;
	virtual kmb::nodeIdType operator[](const int i) const;
protected:
	elementType type;
	nodeIdType *cell;
};


}
