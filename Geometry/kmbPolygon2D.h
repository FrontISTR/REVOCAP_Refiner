/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Polygon2D                                               #
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

#include <vector>
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbRegion.h"

namespace kmb{

class Polygon2D
{
	enum orientation{
		UNKNOWN = -1,
		CLOCKWISE,
		UNTICLOCKWISE
	};
protected:
	std::vector< kmb::nodeIdType > nodeArray;
	const kmb::Point2DContainer* points;
public:
	Polygon2D(void);
	virtual ~Polygon2D(void);
	void clear(void);
	void setPointContainer(const kmb::Point2DContainer* points);
	void appendNodeId( kmb::nodeIdType nodeId );
	int getSize(void) const;
	int getNodeIndex( kmb::nodeIdType nodeId ) const;

	kmb::nodeIdType getNode( kmb::nodeIdType nodeId, int offset=0 ) const;
	kmb::nodeIdType getNodeId( int index ) const;

	Polygon2D::orientation getOrientation(void) const;
	kmb::Region::locationType intersect( kmb::Point2D &point ) const;


	void dividePolygon(kmb::nodeIdType n0,kmb::nodeIdType n1,Polygon2D* &p0,Polygon2D* &p1);



	void branchPolygon(kmb::nodeIdType n0,kmb::nodeIdType n1,Polygon2D &negative,Polygon2D* &cancel);
};

}
