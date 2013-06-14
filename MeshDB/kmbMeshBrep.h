/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.7                          #
# Class Name : MeshBrep                                                #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/03/23     #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "MeshDB/kmbTypes.h"

#include <map>
#include <vector>




namespace kmb{

class MeshData;
class NodeNeighborInfo;

class MeshBrep{
protected:
	const MeshData* mesh;


	std::multimap< kmb::bodyIdType, kmb::bodyIdType > volumes;


	std::multimap< kmb::bodyIdType, kmb::bodyIdType > surfaces;

	bool reverseOrientation;
	kmb::NodeNeighborInfo* neighborInfo;
public:
	MeshBrep(const MeshData* _mesh);
	virtual ~MeshBrep(void);

	bool getReverseOrientation(void) const;
	void setReverseOrientation(bool f);


	int createVolumeInfo(void);


	int createSurfaceInfo(void);

	size_t getChildren(kmb::bodyIdType bodyId,std::vector<kmb::bodyIdType>& children) const;
	size_t getVolumes(std::vector<kmb::bodyIdType>& bodyIds) const;
	size_t getSurfaces(std::vector<kmb::bodyIdType>& bodyIds) const;
protected:
	int appendVolume(kmb::bodyIdType parent,kmb::bodyIdType child);
	int appendSurface(kmb::bodyIdType parent,kmb::bodyIdType child);
};

}
