/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PolygonPartitioner                                      #
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


#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbFramedPlane.h"

#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbPolygon.h"

#include <set>

namespace kmb{

class MeshDB;

class PolygonPartitioner
{
public:

	enum vertexType{
		UNKNOWN,
		START,
		SPLIT,
		END,
		MERGE,
		REGULAR
	};

	PolygonPartitioner(void);
	virtual ~PolygonPartitioner(void);
	void clear(void);
	void setPoints(const kmb::Point2DContainer* targetPoints);
	void setInitialPolygon(kmb::ElementContainer* edges);
	size_t getCount(void) const;


	bool partition( kmb::ElementContainer &body );


	kmb::bodyIdType partitionToTriangles(kmb::MeshDB* mesh,kmb::bodyIdType edgeId);

private:



	kmb::elementIdType getNearestSegmentWithSameLevel(kmb::nodeIdType nodeId, bool left=true) const;



	kmb::nodeIdType getHelperNode( kmb::nodeIdType nodeID, kmb::elementIdType leftID, kmb::elementIdType rightID, vertexType vType) const;


	bool triangulateMonotonePolygon(kmb::Polygon* polygon, kmb::ElementContainer &body );

	const kmb::Point2DContainer* points;
	kmb::Polygon* initialPolygon;

	vertexType	getVertexType(kmb::Polygon* polygon,kmb::nodeIdType nodeId) const;
	static vertexType	getVertexType(const kmb::Point2D& previousPoint,const kmb::Point2D& targetPoint,const kmb::Point2D& nextPoint);
};

}

