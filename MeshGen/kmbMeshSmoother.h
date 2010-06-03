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

namespace kmb{

class MeshData;
class Point3DContainer;
class ElementContainer;
class ElementEvaluator;
class NodeNeighborInfo;
class DataBindings;

class MeshSmoother
{
protected:
	kmb::Point3DContainer* points;
	kmb::ElementContainer* elements;
	kmb::ElementContainer* boundary;
	kmb::ElementEvaluator* evaluator;
	kmb::NodeNeighborInfo* neighborInfo;
	kmb::NodeNeighborInfo* neighborInfoSurf;
	kmb::Point3DContainer* shiftPoints;
public:
	MeshSmoother(kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	virtual ~MeshSmoother(void);

	void init(void);
	void smoothingAtNode(kmb::nodeIdType nodeId);
	void smoothingOnFaceAtNode(kmb::nodeIdType nodeId);

	void commit(void);
};

}
