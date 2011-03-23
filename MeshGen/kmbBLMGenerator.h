/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : BLMGenerator                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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
#include "MeshDB/kmbElement.h"

#include <vector>

namespace kmb{

class MeshDB;
class Quad;

class BLMGenerator
{
protected:


	int layerNum;
	double* layerThick;
	MeshDB* meshDB;
public:
	BLMGenerator(void);
	BLMGenerator(MeshDB* meshDB);
	BLMGenerator(MeshDB* meshDB,int layerNum,double* layerThick);
	virtual ~BLMGenerator(void);







	virtual bool generate(kmb::bodyIdType bodyId,kmb::bodyIdType &layerId,kmb::bodyIdType &boundaryId);





	virtual bool generateFromData(const char* faceGroup,kmb::bodyIdType &layerId);






	void setLayer(int layerNum,double* thicks);

	void setMeshDB(MeshDB* meshDB);
protected:



	bool appendLayerElements( kmb::bodyIdType bodyId, kmb::ElementBase &upper, kmb::ElementBase &lower);



	bool appendLayerElements( kmb::bodyIdType bodyId, kmb::ElementBase &upper, kmb::nodeIdType lowerId);



	bool appendLayerElements( kmb::bodyIdType bodyId, kmb::nodeIdType upperId, kmb::ElementBase &lower);






	bool appendLayerElements( kmb::bodyIdType bodyId, kmb::ElementBase &upper, kmb::nodeIdType lowerId0, kmb::nodeIdType lowerId1);






	bool appendLayerElements( kmb::bodyIdType bodyId, kmb::nodeIdType upperId0, kmb::nodeIdType upperId1, kmb::ElementBase &lower);

/*




	bool appendLayerElements( kmb::bodyIdType bodyID, std::vector<kmb::nodeIdType>& upperID0, std::vector<kmb::nodeIdType>& upperID1, kmb::nodeIdType lowerID0, kmb::nodeIdType lowerID1);





	bool appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID0, kmb::nodeIdType upperID1, std::vector<kmb::nodeIdType>& lowerID0, std::vector<kmb::nodeIdType>& lowerID1);
*/















	kmb::nodeIdType getLayerNodeId(kmb::nodeIdType outerNodeID, kmb::nodeIdType innerNodeID, int layerIndex );
private:
	std::map< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType* > layerNodes;
	void clearLayerNodes(void);
};

}
