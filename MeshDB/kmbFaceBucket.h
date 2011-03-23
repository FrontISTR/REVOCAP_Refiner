/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Bucket                                                  #
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
#include "MeshDB/kmbFace.h"

namespace kmb{

class MeshDB;
class DataBindings;

class FaceBucket : public Bucket<kmb::Face>
{
protected:
	const kmb::MeshData* mesh;
	kmb::bodyIdType bodyId;
public:
	FaceBucket(void);
	virtual ~FaceBucket(void);
	void setup(const kmb::MeshData* mesh,const kmb::DataBindings* faceGroup);
	bool getNearest(double x,double y,double z,double &dist,kmb::Face &f) const;
protected:

	bool getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,double &dist,kmb::Face &f) const;
};

}
