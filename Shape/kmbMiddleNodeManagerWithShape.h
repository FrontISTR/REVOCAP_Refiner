/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : FittingToSurface                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "MeshGen/kmbMiddleNodeManager.h"
#include <vector>

namespace kmb{

template <typename T> class Vector2WithIntBindings;
template <typename T> class NodeMapperBindings;
class Surface3D;

class MiddleNodeManagerWithShape : public kmb::MiddleNodeManager
{
protected:
	kmb::Vector2WithIntBindings<kmb::nodeIdType>* mappingToSurface;
	const std::vector< kmb::Surface3D* >* surfaces;

	bool nearestFlag;
public:
	MiddleNodeManagerWithShape(void);
	virtual ~MiddleNodeManagerWithShape(void);
	void setMappingData(kmb::Vector2WithIntBindings<kmb::nodeIdType>* mapping);
	void setSurfaces(const std::vector< kmb::Surface3D* >* surfaces);
	void setNearestFlag(bool b);


	size_t replaceNodeIds( const std::map<kmb::nodeIdType,kmb::nodeIdType> &nodeMapper );
protected:

	virtual kmb::nodeIdType createMiddleNode(kmb::nodeIdType n0, kmb::nodeIdType n1);
	virtual kmb::nodeIdType createMiddleNode4(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3);
};

}
