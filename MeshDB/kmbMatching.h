/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matching                                                #
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

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbTypes.h"

#include <map>

namespace kmb{

class MeshDB;
class Point3D;
class Point3DContainer;
class ElementContainer;
class NodeNeighborInfo;

class Matching
{
public:
	Matching(void);
	virtual ~Matching(void);

	double getDistanceEdgeToNode( kmb::MeshDB* mesh, kmb::bodyIdType bodyId, kmb::nodeIdType nodeId ) const;

	double getDistanceEdgeToEdge( kmb::MeshDB* mesh, kmb::bodyIdType edgeId0, kmb::bodyIdType edgeId1 ) const;
	double getDistanceEdgeToEdge( kmb::MeshDB* mesh0, kmb::bodyIdType edgeId0, kmb::MeshDB* mesh1, kmb::bodyIdType edgeId1 ) const;
	//
	// search matching edge (body of dim 1) of edgeId of mesh 1 in mesh0
	//
	kmb::bodyIdType searchMatchingEdge(kmb::MeshDB* mesh0,kmb::MeshDB* mesh1,kmb::bodyIdType edgeId,double tolerance);

	// nodeMatching mesh0:body0 => mesh1:body1 within tolerance
	// record mapping info to mesh0:data(coupleName)
	// return count of record pair
	// importBody の時に使う nodeMapping data の生成に用いる
	int nodeMatchingBetweenBodies(kmb::MeshDB* mesh0, kmb::bodyIdType bodyId0, kmb::MeshDB* mesh1, kmb::bodyIdType bodyId1, double tolerance, const char* coupleName);
	// nodeMatching mesh0:body0 => mesh1:all nodes within tolerance
	// record mapping info to mesh0:data(coupleName)
	// return count of record pair
	// importBody の時に使う nodeMapping data の生成に用いる
	int nodeMatchingOnBody(kmb::MeshDB* mesh0, kmb::bodyIdType bodyId0, kmb::MeshDB* mesh1, double tolerance, const char* coupleName);
public:
	//
	// calculate distance between edge0 (in points0) and point1
	// return nearest elementId and 
	// parameter of foot of a perpendicular
	//
	static double getDistanceEdgeToNodeWithParam
		(const kmb::Point3DContainer* points0, kmb::ElementContainer* edge0,
		 const kmb::Point3D* point1, kmb::elementIdType &elemId,double &param);

	//
	// return distance between bounding boxes of id groups
	//
	static double getDistanceDataToData(kmb::MeshDB* mesh,const char* key0,const char* key1);

	//
	// create matching pairs of nodes between edge of mesh1 and any edge of mesh0
	// if distance  < tolerance 
	// nodeMapper[ nodeId of mesh1 on edge1 ] = nodeId of mesh0  
	//
	static int matchNodeToNodeOnEdges
		(kmb::MeshDB* mesh0, kmb::MeshDB* mesh1, kmb::bodyIdType edgeId,
		 const double tolerance, std::map< kmb::nodeIdType, kmb::nodeIdType> &nodeMapper);
	//
	// create matching pairs of nodes between body1 of mesh1 and any body of mesh0 (of the same dimension of body1)
	// if distance  < tolerance 
	// nodeMapper[ nodeId of mesh1 on body ] = nodeId of mesh0
	//
	// mesh1 の bodyId1 の領域上の節点については、mesh0 の中の最近点を探して、距離が tolerance 以下のものは
	// それを使い、使った結果を nodeMapper に登録しておく
	// 
	static int matchNodeToNodeOnBody
		(kmb::MeshDB* mesh0, kmb::MeshDB* mesh1, kmb::bodyIdType bodyId1,
		 const double tolerance, std::map< kmb::nodeIdType, kmb::nodeIdType> &nodeMapper);
	//
	// compare topology of surfaces
	//
	static kmb::ElementRelation::relationType getSurfaceRelation
		( kmb::MeshDB* mesh, kmb::bodyIdType surfId0, kmb::bodyIdType surfId1 );
	static kmb::ElementRelation::relationType getFaceRelation
		( kmb::MeshDB* mesh, const char* faceId0, const char* faceId1 );
};

}

