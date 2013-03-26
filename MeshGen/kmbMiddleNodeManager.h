/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MiddleNodeManager                                       #
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
#include "MeshDB/kmbElement.h"
#include <map>

namespace kmb{

class Point3DContainer;
class ElementBase;
class MeshSmoother;

class MiddleNodeManager
{
protected:
	struct originalPair{
		kmb::nodeIdType n0;
		kmb::nodeIdType n1;

		kmb::elementIdType elementId;
	};

	typedef std::pair< kmb::nodeIdType, kmb::nodeIdType > NodePair;
	Point3DContainer* points;

	std::map< NodePair, kmb::nodeIdType > middlePoints;

	std::map< kmb::nodeIdType, kmb::MiddleNodeManager::originalPair > originalPoints;
protected:

	void appendMiddleNode( kmb::nodeIdType middle, kmb::nodeIdType org0, kmb::nodeIdType org1, kmb::elementIdType elemId=kmb::Element::nullElementId);

	virtual kmb::nodeIdType createMiddleNode(kmb::nodeIdType n0, kmb::nodeIdType n1);
	virtual kmb::nodeIdType createMiddleNode3(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2);
	virtual kmb::nodeIdType createMiddleNode4(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3);
	virtual kmb::nodeIdType createMiddleNode6
		(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2,
		 kmb::nodeIdType n3, kmb::nodeIdType n4,kmb::nodeIdType n5);

	virtual kmb::nodeIdType createMiddleNode8
		(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3,
		 kmb::nodeIdType n4, kmb::nodeIdType n5,kmb::nodeIdType n6, kmb::nodeIdType n7);

	virtual kmb::nodeIdType createMiddleNode3s(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2);

	virtual kmb::nodeIdType createMiddleNode5(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3, kmb::nodeIdType n4);

	virtual kmb::nodeIdType createMiddleNode8s
		(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3,
		 kmb::nodeIdType n4, kmb::nodeIdType n5,kmb::nodeIdType n6, kmb::nodeIdType n7);
public:
	MiddleNodeManager(void);
	virtual ~MiddleNodeManager(void);
	void clear(void);

	void setNodeContainer(Point3DContainer* points);



	kmb::nodeIdType getDividedNode(kmb::nodeIdType a,kmb::nodeIdType b,kmb::elementIdType elemId=kmb::Element::nullElementId);



	kmb::nodeIdType getCenterNode(kmb::ElementBase &elem,kmb::elementIdType elementId);
	kmb::nodeIdType getCenterNode(kmb::ElementBase &elem,int faceIndex,kmb::elementIdType elementId);

	kmb::nodeIdType getCenterNode2(kmb::ElementBase &elem,kmb::elementIdType elementId);
	kmb::nodeIdType getCenterNode2(kmb::ElementBase &elem,int faceIndex,kmb::elementIdType elementId);


	kmb::nodeIdType isDivided(kmb::nodeIdType a,kmb::nodeIdType b) const;

	void setDividedNode(kmb::nodeIdType middle, kmb::nodeIdType a,kmb::nodeIdType b);

	kmb::elementIdType getOriginalNode(kmb::nodeIdType centerId,kmb::nodeIdType &a,kmb::nodeIdType &b) const;





	kmb::nodeIdType getDividedNode3(kmb::nodeIdType a,kmb::nodeIdType b,kmb::nodeIdType c,kmb::elementIdType elemId=kmb::Element::nullElementId);



	kmb::nodeIdType getDividedNode5(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3,kmb::nodeIdType n4,kmb::elementIdType elemId=kmb::Element::nullElementId);



	kmb::nodeIdType getDividedNodePyrmid2c(int index,kmb::nodeIdType center,kmb::ElementBase &elem,kmb::elementIdType elemId=kmb::Element::nullElementId);






	virtual int smoothingMiddleNodes(kmb::MeshSmoother* smoother);
};

}
