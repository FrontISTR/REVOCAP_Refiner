/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeMatchingInfo                                        #
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

namespace kmb{

class NodeMatchingInfo
{
public:
	NodeMatchingInfo(kmb::elementIdType elementId,kmb::nodeIdType nodeId,double t);
	~NodeMatchingInfo(void);

	bool operator<(const NodeMatchingInfo &other)const{
		return (elementId < other.elementId) ||
			(elementId == other.elementId && t < other.t);
	}
	bool operator==(const NodeMatchingInfo &other)const{
		return (elementId == other.elementId && nodeId == other.nodeId);
	}
	bool operator!=(const NodeMatchingInfo &other)const{
		return (elementId != other.elementId || nodeId != other.nodeId);
	}


	kmb::elementIdType elementId;
	kmb::nodeIdType nodeId;

	double t;
};

}
