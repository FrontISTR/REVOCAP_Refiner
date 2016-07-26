/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PatchOperation                                          #
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
//
// 三角形要素からなるパッチに対する位相処理
//

#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbPolygon.h"
#include "Geometry/kmbGeometry3D.h"
#include <cfloat>

namespace kmb{

class MeshData;
class MeshDB;
class Point3DContainer;
class ElementContainer;
class NodeMatchingInfo;
class FramedPlane;
class Polygon;

class PatchOperation
{
public:
	PatchOperation(const kmb::Point3DContainer* points);
	PatchOperation(void);
	virtual ~PatchOperation(void);
	// 向きをそろえようとする。向きを変えた要素の個数を返す。
	int uniformOrientation(kmb::MeshDB* mesh,kmb::bodyIdType bodyId,kmb::elementIdType elementId=kmb::Element::nullElementId);
	// edgeId : by its nodes all segments (and triangles) are subdivided.
	// tolerance : threshold distance between segment and nodes
	kmb::bodyIdType subdivideByEdge(kmb::MeshDB* mesh,kmb::bodyIdType edgeId,double tolerance=DBL_MAX,double globalTolerance=DBL_MAX);
	kmb::bodyIdType subdivideByEdgeWithPatch(kmb::MeshDB* mesh,kmb::bodyIdType edgeId,double tolerance=DBL_MAX,double globalTolerance=DBL_MAX);
	// targetId : its segment are subdivided by nodes of body of edge id
	// return : number of division
	int subdivideEdge(kmb::MeshDB* mesh,kmb::bodyIdType targetId,kmb::bodyIdType edgeId,double tolerance=DBL_MAX);
	int subdivideEdgeWithPatch(kmb::MeshDB* mesh,kmb::bodyIdType targetId,kmb::bodyIdType targetPatchId,kmb::bodyIdType edgeId,double tolerance=DBL_MAX);

	bool divideByPlane(kmb::MeshDB* mesh,const kmb::bodyIdType targetId,
				const kmb::FramedPlane &plane,kmb::bodyIdType &positiveID,kmb::bodyIdType &negativeID,
				bool capFlag=false,bool duplicateFlag=false,double tolerance=1.0e-8);
private:
	// subdivide edge by node matching info
	int edgeSubdivider(kmb::MeshDB* mesh,kmb::bodyIdType edgeId, std::set<kmb::NodeMatchingInfo>& matchingInfo);

	// REMARK : call this method BEFORE edgeSubdivider method for the same edgeId
	// patchId : body id of trianlges 
	// edgeId : body id of segments, used in kmb::NodeMatchingInfo::elementId
	int triangleSubdivider(kmb::MeshDB* mesh,kmb::bodyIdType edgeId, kmb::bodyIdType patchId, std::set<kmb::NodeMatchingInfo>& matchingInfo);
protected:
	const Point3DContainer* points;
};

}
