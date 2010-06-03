/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : BodyOperation                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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



	size_t getEdgesOfBody(
		const kmb::ElementContainer* body,
		kmb::ElementContainer* result=NULL);



	size_t getEdgesIntersectPlane(
		const kmb::ElementContainer* body, const kmb::Plane* plane,
		kmb::ElementContainer* result=NULL);
private:
	const Point3DContainer* points;
};

}
