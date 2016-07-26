/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FaceBucket                                              #
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
//
// MPC 情報生成のための事前処理クラス
//

#pragma once

#include "Geometry/kmbBucket.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbFace.h"

namespace kmb{

class MeshDB;
class DataBindings;

class FaceBucket : public Bucket<kmb::Face>
{
protected:
	const kmb::MeshData* mesh;
	const kmb::DataBindings* faceGroup;
	kmb::bodyIdType bodyId;
public:
	FaceBucket(void);
	virtual ~FaceBucket(void);

	void setContainer(const kmb::MeshData* mesh,const kmb::DataBindings* faceGroup);
	void setAutoBucketSize(void);
	int appendAll(void);

	bool getNearest(double x,double y,double z,double &dist,kmb::Face &f) const;
protected:
	// 距離は２乗で返す
	// (i,j,k) の bucket の中での最小値
	bool getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,double &dist,kmb::Face &f) const;
};

}
