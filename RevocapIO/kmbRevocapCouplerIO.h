/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapCouplerIO                                        #
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








#pragma once

#include "MeshDB/kmbElement.h"
#include <fstream>

namespace kmb{

class MeshData;

class RevocapCouplerIO
{
private:
	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;

	kmb::bodyIdType loadElement( std::ifstream &input, kmb::elementType etype, size_t elementCount, kmb::MeshData* mesh );
	size_t loadNode( std::ifstream &input, size_t nodeCount, kmb::MeshData* mesh );
	size_t loadLocalNode( std::ifstream &input, size_t nodeCount, kmb::MeshData* mesh );
public:
	RevocapCouplerIO( kmb::nodeIdType offsetNodeId=0, kmb::elementIdType offsetElementId=0 );
	virtual ~RevocapCouplerIO(void);

	int loadFromPartitionFile(const char* filename,kmb::MeshData* mesh);



	int loadLocalNodesFromPartitionFile(const char* filename,kmb::MeshData* mesh);
};

}
