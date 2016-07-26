/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BodyOperation                                           #
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

class Point3DContainer;
class ElementContainer;
class Plane;

class BodyOperation
{
public:
	BodyOperation(const Point3DContainer* nodes);
	virtual ~BodyOperation(void);
	// 2次元要素グループ body に含まれる要素の辺を
	// SEGMENT 要素として result に登録する
	// result == NULL だと個数だけ返す
	size_t getEdgesOfBody(const kmb::ElementContainer* body,kmb::ElementContainer* result=NULL) const;
	// body に含まれる要素の辺のうち、plane と交差するものを
	// SEGMENT 要素として result に登録する
	// result == NULL だと個数だけ返す
	size_t getEdgesIntersectPlane(const kmb::ElementContainer* body, const kmb::Plane* plane,kmb::ElementContainer* result=NULL) const;
private:
	const Point3DContainer* points;
};

}
