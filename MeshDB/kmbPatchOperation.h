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

	int uniformOrientation(kmb::MeshDB* mesh,kmb::bodyIdType bodyId,kmb::elementIdType elementId=kmb::Element::nullElementId);


	kmb::bodyIdType subdivideByEdge(kmb::MeshDB* mesh,kmb::bodyIdType edgeId,double tolerance=DBL_MAX,double globalTolerance=DBL_MAX);
	kmb::bodyIdType subdivideByEdgeWithPatch(kmb::MeshDB* mesh,kmb::bodyIdType edgeId,double tolerance=DBL_MAX,double globalTolerance=DBL_MAX);


	int subdivideEdge(kmb::MeshDB* mesh,kmb::bodyIdType targetId,kmb::bodyIdType edgeId,double tolerance=DBL_MAX);
	int subdivideEdgeWithPatch(kmb::MeshDB* mesh,kmb::bodyIdType targetId,kmb::bodyIdType targetPatchId,kmb::bodyIdType edgeId,double tolerance=DBL_MAX);

	bool divideByPlane(kmb::MeshDB* mesh,const kmb::bodyIdType targetId,
				const kmb::FramedPlane &plane,kmb::bodyIdType &positiveID,kmb::bodyIdType &negativeID,
				bool capFlag=false,bool duplicateFlag=false,double tolerance=1.0e-8);
private:

	int edgeSubdivider(kmb::MeshDB* mesh,kmb::bodyIdType edgeId, std::set<kmb::NodeMatchingInfo>& matchingInfo);




	int triangleSubdivider(kmb::MeshDB* mesh,kmb::bodyIdType edgeId, kmb::bodyIdType patchId, std::set<kmb::NodeMatchingInfo>& matchingInfo);
protected:
	const Point3DContainer* points;
};

}
