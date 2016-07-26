/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementBucket                                           #
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
class ElementEvaluator;
class Matrix4x4;
class DataBindings;

class ElementBucket : public Bucket<kmb::elementIdType>
{
private:
	const kmb::Point3DContainer* points;
	const kmb::ElementContainer* elements;
	const kmb::Matrix4x4* coordMatrix;
	kmb::ElementEvaluator* evaluator;
	double margin;
public:
	ElementBucket(void);
	ElementBucket(const kmb::BoxRegion &box,int xnum,int ynum,int znum);
	virtual ~ElementBucket(void);
	void setContainer(const kmb::Point3DContainer* points,const kmb::ElementContainer* elements,const kmb::Matrix4x4* coordMatrix=NULL);

	int append(kmb::elementIdType elementId);
	int appendAll(void);

	// 登録されているコンテナを取り替えて、
	// このコンテナに格納されている要素をすべて登録する
	// ただし既存の登録済みの要素はそのまま
	int appendAllElement(const kmb::ElementContainer* body);

	// 登録されているコンテナから探す
	// 与えられた点を内部に含むような要素を返す
	// 与えられた点と要素の面からなる錐体の体積が tolerance の値よりも小さい場合を除く
	kmb::elementIdType searchElement(double x,double y,double z,double tolerance=0.0) const;

	// body を与えてその中から探す
	kmb::elementIdType searchElementInBody(const kmb::ElementContainer* body,double x,double y,double z,double tolerance=0.0) const;
	// element group または face group  を与えてその中から探す
	kmb::elementIdType searchElementInData(const kmb::DataBindings* data,const kmb::ElementContainer* body,double x,double y,double z,double tolerance=0.0) const;
};

}
