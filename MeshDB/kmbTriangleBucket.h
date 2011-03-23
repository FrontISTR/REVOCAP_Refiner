/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : TriangleBucket                                          #
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

#include "Geometry/kmb_Bucket.h"
#include "MeshDB/kmbTypes.h"

namespace kmb{

class ElementContainer;
class Point3DContainer;
class Matrix4x4;

class TriangleBucket : public Bucket<kmb::elementIdType>
{
private:
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
protected:



	int appendSubBucket(int i0,int i1,int j0,int j1,int k0,int k1,kmb::Point3D &a,kmb::Point3D &b,kmb::Point3D &c,kmb::elementIdType elemId);
};

}
