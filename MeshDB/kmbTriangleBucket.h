/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriangleBucket                                          #
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

#include "Geometry/kmbBucket.h"
#include "MeshDB/kmbTypes.h"

namespace kmb{

class ElementContainer;
class Point3DContainer;
class Matrix4x4;

class TriangleBucket : public Bucket<kmb::elementIdType>
{
protected:
	const kmb::Point3DContainer* points;
	const kmb::ElementContainer* elements;
	const kmb::Matrix4x4* coordMatrix;
public:
	TriangleBucket(void);
	TriangleBucket(const kmb::BoxRegion &box,int xnum,int ynum,int znum);
	virtual ~TriangleBucket(void);
	void setContainer(const kmb::Point3DContainer* points,const kmb::ElementContainer* elements,const kmb::Matrix4x4* coordMatrix=NULL);

	int append(kmb::elementIdType elementId);
	int appendAll(void);

	bool getNearest(double x,double y,double z,double &dist,kmb::elementIdType &tri) const;
protected:




	bool getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,double &dist,kmb::elementIdType &tri) const;
};

}
