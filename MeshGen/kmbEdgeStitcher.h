/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : PatchOperation                                          #
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
#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbPolygon.h"

namespace kmb{

class MeshData;
class MeshDB;
class Point3DContainer;
class ElementContainer;
class Polygon;

class EdgeStitcher
{
private:
	kmb::MeshData* mesh;
	kmb::Point3DContainer* points;
	kmb::Polygon forward;
	kmb::nodeIdType f0;
	kmb::nodeIdType fend;
	kmb::Polygon backward;
	kmb::nodeIdType b0;
	kmb::nodeIdType bend;
	kmb::Vector3D normal;
	bool normalCheck;
	void setInitNormal(void);

public:
	EdgeStitcher(void);
	virtual ~EdgeStitcher(void);


	void setMesh(kmb::MeshData* mesh);


	bool setEdges(const char* forward,const char* backward);
	bool setEdges(kmb::bodyIdType forward,kmb::bodyIdType backward);
	bool setEdgesWithinDistance(kmb::bodyIdType forward,kmb::bodyIdType backward,double distance);
	bool setEdgesWithInitNodes(kmb::bodyIdType forward,kmb::nodeIdType forwardInit,kmb::bodyIdType backward,kmb::nodeIdType backwardInit);
	kmb::bodyIdType stitchEdgesByTriangles(void);
	kmb::bodyIdType stitchEdgesByQuads(void);



	kmb::bodyIdType stitchByQuadsWithNodeCount(int fCount,int bCount);

	void setNormalCheck(bool check);
	bool getNormalCheck(void);

	bool stitchByTriangle( kmb::ElementContainer* result );
	bool stitchByQuad( kmb::ElementContainer* result );


	void setInitialNode( kmb::nodeIdType forwardInit=kmb::nullNodeId, kmb::nodeIdType backwardInit=kmb::nullNodeId );
};

}
