/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FaceBucketArea                                          #
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
#include "MeshDB/kmbFace.h"

namespace kmb{

class MeshDB;
class DataBindings;
class Matrix4x4;

class FaceBucketArea : public Bucket< std::pair<kmb::Face,double> >
{
protected:
	const kmb::MeshData* mesh;
	const kmb::DataBindings* faceGroup;
	kmb::bodyIdType bodyId;
	const kmb::DataBindings* displace;
public:
	FaceBucketArea(void);
	virtual ~FaceBucketArea(void);
	void setContainer(const kmb::MeshData* mesh,const kmb::DataBindings* faceGroup,const kmb::DataBindings* displace=NULL);
	void setAutoBucketSize(void);
	int appendAll(void);
	bool getNearest(double x,double y,double z,double &dist,kmb::Face &f) const;
protected:

	bool getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,double &dist,kmb::Face &f) const;



	int appendSubBucket(int i0,int i1,int j0,int j1,int k0,int k1,const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Face &f);
};

}
