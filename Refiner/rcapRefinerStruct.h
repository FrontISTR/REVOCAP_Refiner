/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Program Name : rcapRefinerStruct                                     #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#ifndef REVOCAP_REFINER_STRUCT_HEADER
#define REVOCAP_REFINER_STRUCT_HEADER

#include "rcapRefiner.h"
#include "Geometry/kmbIdTypes.h"

#include <vector>

namespace kmb{
	class MeshData;
	class MeshRefiner;
	class MiddleNodeManager;
	class Surface3D;
	class MeshSmoother;
}

struct rcapRefinerInfo
{
	kmb::MeshData* mesh;
	kmb::MeshRefiner* refiner;
	kmb::MeshRefiner* boundaryRefiner;
	kmb::MiddleNodeManager* middleMan;
	kmb::MeshSmoother* smoother;
	std::vector< kmb::Surface3D* > surfaces;
	int32_t nodeOffset;
	int32_t elementOffset;
	int32_t maxElementId;
	int32_t maxRefinedElementId;
};

#endif /* REVOCAP_REFINER_STRUCT_HEADER */
