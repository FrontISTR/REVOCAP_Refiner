/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : DataEvaluator                                           #
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

#include "Geometry/kmb_BoundingBox.h"
#include "Geometry/kmb_Calculator.h"

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
	bool getMinMax(const kmb::DataBindings* data, kmb::BoundingBox &bbox);
	bool getMinMaxWithId(const kmb::DataBindings* data, kmb::MinMaxWithId &minmax,int comp=-1);
	bool getAbsMinMax(const kmb::DataBindings* data, kmb::BoundingBox1D &bbox);
	bool getMinMax(const kmb::DataBindings* data, kmb::BoundingBox1D &bbox);
	bool getMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox& bbox);
	bool getAbsMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox1D& bbox);
	bool getMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox1D& bbox);

	bool getAverageOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements,double* average);
private:
	const Point3DContainer* points;
	const Point2DContainer* point2Ds;
};

}
