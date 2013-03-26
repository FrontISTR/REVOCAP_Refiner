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
#include "MeshDB/kmbDataEvaluator.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbPhysicalValue.h"
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbPoint3DContainer.h"

kmb::DataEvaluator::DataEvaluator(const kmb::Point3DContainer* points)
: points(NULL)
, point2Ds(NULL)
{
	this->points = points;
}

kmb::DataEvaluator::DataEvaluator(const kmb::Point2DContainer* point2Ds)
: points(NULL)
, point2Ds(NULL)
{
	this->point2Ds = point2Ds;
}

kmb::DataEvaluator::~DataEvaluator(void)
{
}

bool
kmb::DataEvaluator::getMinMax(const kmb::DataBindings* data, kmb::BoundingBox1D &bbox) const
{
	if( data != NULL && data->getValueType() == kmb::PhysicalValue::Scalar ){
		double value = 0.0;
		kmb::DataBindings::const_iterator pIter = data->begin();
		while( !pIter.isFinished() ){
			if( pIter.getValue( &value ) ){
				bbox.update( value );
			}
			++pIter;
		}
		return true;
	}else if( data != NULL && data->getValueType() == kmb::PhysicalValue::Vector3 ){
		double value[3] = { 0.0, 0.0, 0.0 };
		kmb::DataBindings::const_iterator pIter = data->begin();
		while( !pIter.isFinished() ){
			if( pIter.getValue( value ) ){
				bbox.update( kmb::Vector3D::abs(value) );
			}
			++pIter;
		}
		return true;
	}
	return false;
}

bool
kmb::DataEvaluator::getMinMaxWithId(const kmb::DataBindings* data, kmb::MinMaxWithId<kmb::idType> &minmax,int comp) const
{
	if( data ){
		if( data->getValueType() == kmb::PhysicalValue::Scalar )
		{
			minmax.initialize();
			kmb::DataBindings::const_iterator dIter = data->begin();
			double v = 0.0;
			while( !dIter.isFinished() ){
				if( dIter.getValue( &v ) ){
					minmax.update( v, dIter.getId() );
				}
				++dIter;
			}
			return true;
		}
		else if( data->getValueType() == kmb::PhysicalValue::Vector3 )
		{
			minmax.initialize();
			kmb::DataBindings::const_iterator dIter = data->begin();
			double v[3];
			if( comp == -1 ){
				while( !dIter.isFinished() ){
					if( dIter.getValue( v ) ){
						minmax.update( kmb::Vector3D::abs( v ), dIter.getId() );
					}
					++dIter;
				}
			}else if(0 <= comp && comp < 3){
				while( !dIter.isFinished() ){
					if( dIter.getValue( v ) ){
						minmax.update( v[comp], dIter.getId() );
					}
					++dIter;
				}
			}
			return true;
		}
		else if( data->getValueType() == kmb::PhysicalValue::Tensor6 )
		{
			minmax.initialize();
			kmb::DataBindings::const_iterator dIter = data->begin();
			double v[6];
			if(0 <= comp && comp < 6){
				while( !dIter.isFinished() ){
					if( dIter.getValue( v ) ){
						minmax.update( v[comp], dIter.getId() );
					}
					++dIter;
				}
			}
			return true;
		}
		else if( data->getValueType() == kmb::PhysicalValue::Vector2 )
		{
			minmax.initialize();
			kmb::DataBindings::const_iterator dIter = data->begin();
			double v[2];
			if( comp == -1 ){
				while( !dIter.isFinished() ){
					if( dIter.getValue( v ) ){
						minmax.update( kmb::Vector2D::abs( v ), dIter.getId() );
					}
					++dIter;
				}
			}else if(0 <= comp && comp < 2){
				while( !dIter.isFinished() ){
					if( dIter.getValue( v ) ){
						minmax.update( v[comp], dIter.getId() );
					}
					++dIter;
				}
			}
			return true;
		}
		else if( data->getValueType() == kmb::PhysicalValue::Vector2withInt )
		{
			minmax.initialize();
			kmb::DataBindings::const_iterator dIter = data->begin();
			if(0 <= comp && comp < 2){
				double v[2];
				while( !dIter.isFinished() ){
					if( dIter.getValue( v ) ){
						minmax.update( v[comp], dIter.getId() );
					}
					++dIter;
				}
			}else if(comp == 2){
				long n;
				while( !dIter.isFinished() ){
					if( dIter.getValue( &n ) ){
						minmax.update( static_cast<double>(n), dIter.getId() );
					}
					++dIter;
				}
			}
			return true;
		}
	}
	return false;
}

bool
kmb::DataEvaluator::getAbsMinMax(const kmb::DataBindings* data, kmb::BoundingBox1D &bbox) const
{
	if( data != NULL && data->getValueType() == kmb::PhysicalValue::Vector3 ){
		double value[3] = { 0.0, 0.0, 0.0 };
		kmb::DataBindings::const_iterator pIter = data->begin();
		while( !pIter.isFinished() ){
			if( pIter.getValue( value ) ){
				bbox.update( kmb::Vector3D::abs(value) );
			}
			++pIter;
		}
		return true;
	}
	return false;
}

bool
kmb::DataEvaluator::getMinMax(const kmb::DataBindings* data, kmb::BoundingBox &bbox) const
{
	if( data != NULL && data->getValueType() == kmb::PhysicalValue::Vector3 ){
		double value[3] = { 0.0, 0.0, 0.0 };
		kmb::DataBindings::const_iterator pIter = data->begin();
		while( !pIter.isFinished() ){
			if( pIter.getValue( value ) ){
				bbox.update( value[0], value[1], value[2] );
			}
			++pIter;
		}
		return true;
	}
	return false;
}

bool
kmb::DataEvaluator::getMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox& bbox) const
{
	if( data != NULL && data->getValueType() == kmb::PhysicalValue::Vector3 && elements != NULL ){
		double value[3] = {0.0, 0.0, 0.0};
		if( data->getBindingMode() == kmb::DataBindings::NodeVariable ){
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				const int nodeCount = eIter.getNodeCount();
				for(int i=0;i<nodeCount;++i){
					if( data->getPhysicalValue( eIter.getCellId(i), value ) ){
						bbox.update( value[0], value[1], value[2] );
					}
				}
				++eIter;
			}
			return true;
		}else if( data->getBindingMode() == kmb::DataBindings::ElementVariable ){
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				if( data->getPhysicalValue( eIter.getId(), value ) ){
					bbox.update( value[0], value[1], value[2] );
				}
				++eIter;
			}
			return true;
		}
	}
	return false;
}

bool
kmb::DataEvaluator::getAbsMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox1D& bbox) const
{
	if( data != NULL && data->getValueType() == kmb::PhysicalValue::Vector3 && elements != NULL ){
		double value[3] = {0.0, 0.0, 0.0};
		if( data->getBindingMode() == kmb::DataBindings::NodeVariable ){
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				const int nodeCount = eIter.getNodeCount();
				for(int i=0;i<nodeCount;++i){
					if( data->getPhysicalValue( eIter.getCellId(i), value ) ){
						bbox.update( kmb::Vector3D::abs(value) );
					}
				}
				++eIter;
			}
			return true;
		}else if( data->getBindingMode() == kmb::DataBindings::ElementVariable ){
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				if( data->getPhysicalValue( eIter.getId(), value ) ){
					bbox.update( kmb::Vector3D::abs(value) );
				}
				++eIter;
			}
			return true;
		}
	}
	return false;
}

bool
kmb::DataEvaluator::getMinMaxOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, kmb::BoundingBox1D& bbox) const
{
	if( data != NULL && data->getValueType() == kmb::PhysicalValue::Scalar && elements != NULL ){
		double value = 0.0;
		if( data->getBindingMode() == kmb::DataBindings::NodeVariable ){
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				const int nodeCount = eIter.getNodeCount();
				for(int i=0;i<nodeCount;++i){
					if( data->getPhysicalValue( eIter.getCellId(i), &value ) ){
						bbox.update( value );
					}
				}
				++eIter;
			}
			return true;
		}else if( data->getBindingMode() == kmb::DataBindings::ElementVariable ){
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				if( data->getPhysicalValue( eIter.getId(), &value ) ){
					bbox.update( value );
				}
				++eIter;
			}
			return true;
		}
	}
	return false;
}

bool
kmb::DataEvaluator::getAverageOnBody(const kmb::DataBindings* data, const kmb::ElementContainer* elements, double* average) const
{
	if( data != NULL && elements != NULL && average != NULL && this->points != NULL )
	{
		if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
			data->getValueType() == kmb::PhysicalValue::Scalar &&
			elements->isUniqueDim(2) )
		{

			kmb::ElementEvaluator evaluator( points );
			double value = 0.0;
			double area = 0.0;
			double sum = 0.0;
			double sumArea = 0.0;
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				const int nodeCount = eIter.getNodeCount();
				double dataOnElement = 0.0;
				for(int i=0;i<nodeCount;++i){
					if( data->getPhysicalValue( eIter.getCellId(i), &value ) ){
						dataOnElement += value;
					}
				}
				dataOnElement /= nodeCount;
				area = evaluator.getArea( eIter );
				sum += dataOnElement * area;
				sumArea += area;
				++eIter;
			}
			*average = sum / sumArea;
			return true;
		}
		else if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
			data->getValueType() == kmb::PhysicalValue::Vector3 &&
			elements->isUniqueDim(2) )
		{

			kmb::ElementEvaluator evaluator( points );
			double value[3] = {0.0, 0.0, 0.0};
			double area = 0.0;
			double sum[3] = {0.0, 0.0, 0.0};
			double sumArea = 0.0;
			double dataOnElement[3] = {0.0, 0.0, 0.0};
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				const int nodeCount = eIter.getNodeCount();
				dataOnElement[0] = 0.0;
				dataOnElement[1] = 0.0;
				dataOnElement[2] = 0.0;
				for(int i=0;i<nodeCount;++i){
					if( data->getPhysicalValue( eIter.getCellId(i), value ) ){
						dataOnElement[0] = value[0];
						dataOnElement[1] = value[1];
						dataOnElement[2] = value[2];
					}
				}
				dataOnElement[0] /= nodeCount;
				dataOnElement[1] /= nodeCount;
				dataOnElement[2] /= nodeCount;
				area = evaluator.getArea( eIter );
				sum[0] += dataOnElement[0] * area;
				sum[1] += dataOnElement[1] * area;
				sum[2] += dataOnElement[2] * area;
				sumArea += area;
				++eIter;
			}
			average[0] = sum[0] / sumArea;
			average[1] = sum[1] / sumArea;
			average[2] = sum[2] / sumArea;
			return true;
		}
		else if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
			data->getValueType() == kmb::PhysicalValue::Scalar &&
			elements->isUniqueDim(3) )
		{

			kmb::ElementEvaluator evaluator( points );
			double value = 0.0;
			double volume = 0.0;
			double sum = 0.0;
			double sumVolume = 0.0;
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				const int nodeCount = eIter.getNodeCount();
				double dataOnElement = 0.0;
				for(int i=0;i<nodeCount;++i){
					if( data->getPhysicalValue( eIter.getCellId(i), &value ) ){
						dataOnElement += value;
					}
				}
				dataOnElement /= nodeCount;
				volume = evaluator.getVolume( eIter );
				sum += dataOnElement * volume;
				sumVolume += volume;
				++eIter;
			}
			*average = sum / sumVolume;
			return true;
		}
		else if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
			data->getValueType() == kmb::PhysicalValue::Vector3 &&
			elements->isUniqueDim(3) )
		{

			kmb::ElementEvaluator evaluator( points );
			double value[3] = {0.0, 0.0, 0.0};
			double volume = 0.0;
			double sum[3] = {0.0, 0.0, 0.0};
			double sumVolume = 0.0;
			double dataOnElement[3] = {0.0, 0.0, 0.0};
			kmb::ElementContainer::const_iterator eIter = elements->begin();
			while( !eIter.isFinished() ){
				const int nodeCount = eIter.getNodeCount();
				dataOnElement[0] = 0.0;
				dataOnElement[1] = 0.0;
				dataOnElement[2] = 0.0;
				for(int i=0;i<nodeCount;++i){
					if( data->getPhysicalValue( eIter.getCellId(i), value ) ){
						dataOnElement[0] = value[0];
						dataOnElement[1] = value[1];
						dataOnElement[2] = value[2];
					}
				}
				dataOnElement[0] /= nodeCount;
				dataOnElement[1] /= nodeCount;
				dataOnElement[2] /= nodeCount;
				volume = evaluator.getVolume( eIter );
				sum[0] += dataOnElement[0] * volume;
				sum[1] += dataOnElement[1] * volume;
				sum[2] += dataOnElement[2] * volume;
				sumVolume += volume;
				++eIter;
			}
			average[0] = sum[0] / sumVolume;
			average[1] = sum[1] / sumVolume;
			average[2] = sum[2] / sumVolume;
			return true;
		}
	}
	return false;
}
