/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : BLMGenerator                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
public:
	BLMGenerator(void);
	BLMGenerator(MeshDB* meshDB);
	BLMGenerator(MeshDB* meshDB,int layerNum,double* layerThick);
	virtual ~BLMGenerator(void);







	virtual bool generate(kmb::bodyIdType bodyID,kmb::bodyIdType &layerID,kmb::bodyIdType &boundaryID);






	void		setLayer(int layerNum,double* thicks);

	void		setMeshDB(MeshDB* meshDB);
protected:


	int			layerNum;
	double*		layerThick;
	MeshDB* meshDB;






	kmb::elementType appendElement2D( kmb::bodyIdType bodyID,std::vector< kmb::nodeIdType >* nodes );




	bool appendLayerElements( kmb::bodyIdType bodyID, kmb::ElementBase &upper, kmb::ElementBase &lower);



	bool appendLayerElements( kmb::bodyIdType bodyID, kmb::ElementBase &upper, kmb::nodeIdType lowerID);



	bool appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID, kmb::ElementBase &lower);






	bool appendLayerElements( kmb::bodyIdType bodyID, kmb::ElementBase &upper, kmb::nodeIdType lowerID0, kmb::nodeIdType lowerID1);






	bool appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID0, kmb::nodeIdType upperID1, kmb::ElementBase &lower);





	bool appendLayerElements( kmb::bodyIdType bodyID, std::vector<kmb::nodeIdType>& upperID0, std::vector<kmb::nodeIdType>& upperID1, kmb::nodeIdType lowerID0, kmb::nodeIdType lowerID1);





	bool appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID0, kmb::nodeIdType upperID1, std::vector<kmb::nodeIdType>& lowerID0, std::vector<kmb::nodeIdType>& lowerID1);
















	kmb::nodeIdType		getLayerNodeId(kmb::nodeIdType outerNodeID, kmb::nodeIdType innerNodeID, int layerIndex );
private:
	std::map< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType* >	layerNodes;
	void				clearLayerNodes(void);
};

}
