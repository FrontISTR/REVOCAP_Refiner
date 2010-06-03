/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : AFTetraMesher                                           #
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
#include "MeshDB/kmbFace.h"

namespace kmb{

class Point3DContainer;
class ElementContainer;
class ElementEvaluator;
class MeshData;
class NodeNeighborInfo;

class PatchModifier
{
protected:
	kmb::Point3DContainer* points;
	kmb::ElementContainer* patch;
	kmb::ElementEvaluator* evaluator;
	kmb::NodeNeighborInfo* neighborInfo;
public:
	PatchModifier(kmb::Point3DContainer* points,kmb::ElementContainer* patch);
	PatchModifier(kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	virtual ~PatchModifier(void);

	int uniformOrientation(kmb::elementIdType elementId=kmb::Element::nullElementId);


	bool triangleEdgeSwap(kmb::elementIdType tri0, kmb::elementIdType tri1 );



	bool quadEdgeSwap(kmb::elementIdType quad0, kmb::elementIdType quad1, bool orientation=true);



	kmb::elementIdType quadTrisect(kmb::elementIdType quad0, kmb::elementIdType quad1, bool orientation=true );

	int getNeighborElements(kmb::elementIdType elementId,kmb::elementIdType* neighbors) const;
	kmb::elementIdType getNeighborElement(kmb::Face f) const;

	bool edgeContract(kmb::nodeIdType node0, kmb::nodeIdType node1);

	double getAngleByIndex(kmb::elementIdType elementId,int index) const;

};

}
