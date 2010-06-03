/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Point3DOctree                                           #
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

#include "Geometry/kmb_Point3DContainer.h"
#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_Region.h"
#include <set>

namespace kmb{

class Point3DOctree
{
public:

	Point3DOctree(int thresh=256);
	virtual ~Point3DOctree(void);
	void clear(void);




	void generateSearchCache(kmb::Point3DContainer* points);
	void generateSearchCache(kmb::Point3DContainer* points,std::set< kmb::nodeIdType > &nodeSet);
	void appendSearchCache(const kmb::Point3DContainer* points,kmb::nodeIdType nodeId);
	double getNearestPoint(double x,double y,double z, const kmb::Point3DContainer* points,kmb::nodeIdType &nearestId) const;
	void getNodesInRegion(kmb::Region* region, kmb::Point3DContainer* points, std::set<kmb::nodeIdType> &nodes) const;

	int getLocalCount(void) const;

	size_t getCount(void) const;

private:
	int thres;
	int count;
	int layer;
	kmb::nodeIdType *nodeIds;
	kmb::Point3DOctree **children;
	kmb::BoundingBox boundBox;
	kmb::Point3D center;
	int getChildIndex(double x,double y,double z) const;
	int getChildIndex(bool bx,bool by,bool bz) const;
	void createChildren(const kmb::Point3DContainer* points);
	void addPoint(kmb::nodeIdType nodeId,kmb::Point3D& point,const kmb::Point3DContainer* points);
};

}
