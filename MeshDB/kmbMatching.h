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



	kmb::bodyIdType searchMatchingEdge(kmb::MeshDB* mesh0,kmb::MeshDB* mesh1,kmb::bodyIdType edgeId,double tolerance);





	int nodeMatchingBetweenBodies(kmb::MeshDB* mesh0, kmb::bodyIdType bodyId0, kmb::MeshDB* mesh1, kmb::bodyIdType bodyId1, double tolerance, const char* coupleName);




	int nodeMatchingOnBody(kmb::MeshDB* mesh0, kmb::bodyIdType bodyId0, kmb::MeshDB* mesh1, double tolerance, const char* coupleName);
public:





	static double getDistanceEdgeToNodeWithParam
		(const kmb::Point3DContainer* points0, kmb::ElementContainer* edge0,
		 const kmb::Point3D* point1, kmb::elementIdType &elemId,double &param);




	static double getDistanceDataToData(kmb::MeshDB* mesh,const char* key0,const char* key1);














	static int matchNodeToNodeOnEdges
		(kmb::MeshDB* mesh0, kmb::MeshDB* mesh1, kmb::bodyIdType edgeId,
		 const double tolerance, std::map< kmb::nodeIdType, kmb::nodeIdType> &nodeMapper);








	static int matchNodeToNodeOnBody
		(kmb::MeshDB* mesh0, kmb::MeshDB* mesh1, kmb::bodyIdType bodyId1,
		 const double tolerance, std::map< kmb::nodeIdType, kmb::nodeIdType> &nodeMapper);



	static kmb::ElementRelation::relationType getSurfaceRelation
		( kmb::MeshDB* mesh, kmb::bodyIdType surfId0, kmb::bodyIdType surfId1 );
	static kmb::ElementRelation::relationType getFaceRelation
		( kmb::MeshDB* mesh, const char* faceId0, const char* faceId1 );
};

}

