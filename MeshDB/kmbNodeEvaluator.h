/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeEvaluator                                           #
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

#include "MeshDB/kmbTypes.h"

namespace kmb{

class MeshData;

class NodeEvaluator
{
protected:
	kmb::MeshData* mesh;
public:
	NodeEvaluator(void);
	virtual ~NodeEvaluator(void);
	void setMesh(kmb::MeshData* mesh);



	int calcCurvature(const char* name,const char* stype=NULL);
};

}
