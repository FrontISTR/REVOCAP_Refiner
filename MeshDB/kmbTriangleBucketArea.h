/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : TriangleBucketArea                                      #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/




#pragma once

#include "MeshDB/kmbTypes.h"
#include "Geometry/kmb_Bucket.h"
#include "Geometry/kmb_Geometry3D.h"

#include <vector>

namespace kmb{

class ElementContainer;
class Point3DContainer;
class Matrix4x4;

class TriangleBucketArea : public Bucket< std::pair<kmb::elementIdType,double> >
{
private:
	const kmb::Point3DContainer* points;
	const kmb::ElementContainer* elements;
	const kmb::Matrix4x4* coordMatrix;
public:
	TriangleBucketArea(void);
	TriangleBucketArea(const kmb::BoxRegion &box,int xnum,int ynum,int znum);
	virtual ~TriangleBucketArea(void);

	void setContainer(const kmb::Point3DContainer* points,const kmb::ElementContainer* elements,const kmb::Matrix4x4* coordMatrix=NULL);

	int append(kmb::elementIdType elementId);
	int appendAll(void);
};

}
