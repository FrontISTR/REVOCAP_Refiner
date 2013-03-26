/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshSmoother                                            #
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

#include <map>
#include <cstddef>

namespace kmb{

class MeshData;
class Point3DContainer;
class ElementContainer;
class ElementEvaluator;
class NodeNeighborInfo;
class DataBindings;

class MeshSmoother
{
protected:
	kmb::MeshData* mesh;
	kmb::Point3DContainer* shiftPoints;
	std::multimap< kmb::nodeIdType, kmb::nodeIdType > nodeConnectTable;

	double relax;
public:
	MeshSmoother(kmb::MeshData* mesh);
	virtual ~MeshSmoother(void);
	void clear(void);


	size_t appendBody(kmb::bodyIdType bodyId);
	size_t appendBody(const kmb::ElementContainer* body);


	void init(void);



	int smoothingNode(kmb::nodeIdType nodeId);
	int smoothingAllNode(void);
	int smoothingNodeGroup(const char* name,const char* stype=NULL);

	void commit(void);
private:

	bool appendConnectNode(kmb::nodeIdType n0,kmb::nodeIdType n1);
};

}
