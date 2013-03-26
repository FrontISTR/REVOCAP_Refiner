/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : DataEvaluator                                           #
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

#include "Geometry/kmbBoundingBox.h"
#include "Common/kmbCalculator.h"

namespace kmb{

class Point3DContainer;
class Point2DContainer;
class DataBindings;
class PhysicalValue;
class ElementContainer;

class DataEvaluator
{
public:
	DataEvaluator(const kmb::Point3DContainer* points);
	DataEvaluator(const kmb::Point2DContainer* point2Ds);
	virtual ~DataEvaluator(void);
	bool getMinMax(const kmb::DataBindings* data, kmb::BoundingBox &bbox) const;
	bool getMinMaxWithId(const kmb::DataBindings* data, kmb::MinMaxWithId<kmb::idType> &minmax,int comp=-1) const;
	bool getAbsMinMax(const kmb::DataBindings* data, kmb::BoundingBox1D &bbox) const;
	bool getMinMax(const kmb::DataBindings* data, kmb::BoundingBox1D &bbox) const;
	bool getMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox& bbox) const;
	bool getAbsMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox1D& bbox) const;
	bool getMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox1D& bbox) const;

	bool getAverageOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements,double* average) const;
private:
	const Point3DContainer* points;
	const Point2DContainer* point2Ds;
};

}
