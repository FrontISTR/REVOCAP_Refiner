/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.7                          #
# Class Name : MeshOperation                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/05/23     #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "MeshDB/kmbTypes.h"
#include <map>

namespace kmb{

class MeshData;

class MeshOperation
{
protected:
	kmb::MeshData* mesh;
public:
	MeshOperation(kmb::MeshData* _mesh);
	virtual ~MeshOperation(void);



	size_t nodeSplitByVolumes(void);



	int duplicateNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName=NULL);


	size_t uniteNodes(double thresh);
protected:


	size_t replaceNodeIdOfBody(kmb::bodyIdType bodyId,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper);
};

}
