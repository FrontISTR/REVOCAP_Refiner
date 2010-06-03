/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : FittingToSurface                                        #
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
#include <vector>

namespace kmb{

class MeshData;
class Surface3D;
template <typename> class Vector2WithIntBindings;

class FittingToSurface
{
protected:
	kmb::MeshData* mesh;

	kmb::Vector2WithIntBindings<kmb::nodeIdType>* fittingData;
	double threshold;

	std::vector<kmb::Surface3D*> surfaces;
public:
	FittingToSurface(void);
	virtual ~FittingToSurface(void);

	void setThreshold( double thresh );








	void clearFittingData();





	void setMesh( kmb::MeshData* mesh );
	int appendSurface( kmb::Surface3D* surface );

	size_t createFittingData( kmb::bodyIdType bodyId, const char* name="fitting" );

};

}
