/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshDB                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbMeshDB.h"

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementEvaluator.h"

int
kmb::MeshDB::getInterpolatedValueInBody(const char* name,kmb::bodyIdType bodyId,double x,double y,double z,double *values,double tolerance,const char* stype) const
{
	if( this->evaluator == NULL ){
		return -1;
	}
	int dim = -1;
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data == NULL || data->getBindingMode() != kmb::DataBindings::NodeVariable || (dim=data->getDimension()) == 0){
		return -1;
	}
	kmb::elementIdType elementId = this->searchElement(bodyId,x,y,z,tolerance);
	kmb::ElementContainer::const_iterator nearestElement = this->findElement( elementId, bodyId );
	if( nearestElement.isFinished() ){
		return -1;
	}
	double coeff[20];
	if( this->evaluator->getWeightElement( nearestElement, x, y, z, coeff ) >= tolerance ){
		for(int j=0;j<dim;++j){
			values[j] = 0.0;
		}
		double* nVal = new double[dim];
		int len = nearestElement.getNodeCount();

		for(int i=0;i<len;++i){
			data->getPhysicalValue( nearestElement[i], nVal );

			for(int j=0;j<dim;++j){
				values[j] += coeff[i] * nVal[j];
			}
		}
		delete[] nVal;
		return dim;
	}
	return 0;
}

int
kmb::MeshDB::getInterpolatedValueInData(const char* name,const char* target,double x,double y,double z,double *values,double tolerance,const char* stype) const
{
	if( this->evaluator == NULL ){
		return -1;
	}
	int dim = -1;
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data == NULL || data->getBindingMode() != kmb::DataBindings::NodeVariable || (dim=data->getDimension()) == 0){
		return -1;
	}
	const kmb::DataBindings* targetGroup = this->getDataBindingsPtr(target);
	if( targetGroup == NULL ){
		return -1;
	}
	kmb::bodyIdType bodyId = targetGroup->getTargetBodyId();

	kmb::elementIdType elementId = this->searchElementInData(target,x,y,z,tolerance);
	kmb::ElementContainer::const_iterator nearestElement = this->findElement( elementId, bodyId );
	if( nearestElement.isFinished() ){
		return -1;
	}
	double coeff[20];
	if( this->evaluator->getWeightElement( nearestElement, x, y, z, coeff ) >= tolerance){
		for(int j=0;j<dim;++j){
			values[j] = 0.0;
		}
		double* nVal = new double[dim];
		int len = nearestElement.getNodeCount();

		for(int i=0;i<len;++i){
			data->getPhysicalValue( nearestElement[i], nVal );

			for(int j=0;j<dim;++j){
				values[j] += coeff[i] * nVal[j];
			}
		}
		delete[] nVal;
		return dim;
	}
	return 0;
}

int
kmb::MeshDB::getInterpolatedValue(const char* name,double x,double y,double z,double *values,double tolerance,const char* stype) const
{
	if( this->evaluator == NULL ){
		return -1;
	}
	int dim = -1;
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data == NULL ||
		data->getBindingMode() != kmb::DataBindings::NodeVariable ||
		(dim=data->getDimension()) <= 0 )
	{
		return -2;
	}
	for(int j=0;j<dim;++j){
		values[j] = 0.0;
	}
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	kmb::bodyIdType bodyCount = this->getBodyCount();
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
		elementId = this->searchElement(bodyId,x,y,z,tolerance);
		kmb::ElementContainer::const_iterator nearestElement = this->findElement( elementId, bodyId );
		if( !nearestElement.isFinished() ){
			double coeff[20];
			if( this->evaluator->getWeightElement( nearestElement, x, y, z, coeff ) >= tolerance ){
				double* nVal = new double[dim];
				int len = nearestElement.getNodeCount();

				for(int i=0;i<len;++i){
					data->getPhysicalValue( nearestElement[i], nVal );

					for(int j=0;j<dim;++j){
						values[j] += coeff[i] * nVal[j];
					}
				}
				delete[] nVal;
				return dim;
			}
		}
	}
	return 0;
}

int
kmb::MeshDB::getValueOnNearestNode(const char* name,double x,double y,double z,double *values,const char* stype) const
{
	int dim = -1;
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data == NULL ||
		data->getBindingMode() != kmb::DataBindings::NodeVariable ||
		(dim=data->getDimension()) <= 0 )
	{
		return -1;
	}
	for(int j=0;j<dim;++j){
		values[j] = 0.0;
	}
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	this->getNearestNode(x,y,z,nodeId);
	data->getPhysicalValue( nodeId, values );
	return 0;
}

int
kmb::MeshDB::getInterpolatedValuesInBody(kmb::bodyIdType bodyId,double x,double y,double z,double *values,double tolerance) const
{
	int dim = this->getTargetDataDim();
	if( this->evaluator == NULL || dim <= 0 ){
		return -1;
	}
	kmb::elementIdType elementId = this->searchElement(bodyId,x,y,z,tolerance);
	kmb::ElementContainer::const_iterator nearestElement = this->findElement( elementId, bodyId );
	if( nearestElement.isFinished() ){
		return -1;
	}
	for(int j=0;j<dim;++j){
		values[j] = 0.0;
	}
	double coeff[20];
	if( this->evaluator->getWeightElement( nearestElement, x, y, z, coeff ) >= tolerance ){
		double* nVal = new double[dim];
		int len = nearestElement.getNodeCount();

		for(int i=0;i<len;++i){
			this->getMultiPhysicalValues( nearestElement[i], nVal );

			for(int j=0;j<dim;++j){
				values[j] += coeff[i] * nVal[j];
			}
		}
		delete[] nVal;
		return dim;
	}
	return 0;
}

int
kmb::MeshDB::getInterpolatedValuesInData(const char* target,double x,double y,double z,double *values,double tolerance) const
{
	int dim = this->getTargetDataDim();
	if( this->evaluator == NULL || dim <= 0 ){
		return -1;
	}
	const kmb::DataBindings* targetGroup = this->getDataBindingsPtr(target);
	if( targetGroup == NULL ){
		return -1;
	}
	kmb::bodyIdType bodyId = targetGroup->getTargetBodyId();

	kmb::elementIdType elementId = this->searchElementInData(target,x,y,z,tolerance);
	kmb::ElementContainer::const_iterator nearestElement = this->findElement( elementId, bodyId );
	if( nearestElement.isFinished() ){
		return -1;
	}
	for(int j=0;j<dim;++j){
		values[j] = 0.0;
	}
	double coeff[20];
	if( this->evaluator->getWeightElement( nearestElement, x, y, z, coeff ) >= tolerance ){
		double* nVal = new double[dim];
		int len = nearestElement.getNodeCount();

		for(int i=0;i<len;++i){
			this->getMultiPhysicalValues( nearestElement[i], nVal );

			for(int j=0;j<dim;++j){
				values[j] += coeff[i] * nVal[j];
			}
		}
		delete[] nVal;
		return dim;
	}
	return 0;
}

int
kmb::MeshDB::getInterpolatedValues(double x,double y,double z,double *values,double tolerance) const
{
	int dim = this->getTargetDataDim();
	if( this->evaluator == NULL || dim <= 0 ){
		return -1;
	}
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	kmb::bodyIdType bodyCount = this->getBodyCount();
	double coeff[20];
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
		elementId = this->searchElement(bodyId,x,y,z,tolerance);
		kmb::ElementContainer::const_iterator nearestElement = this->findElement( elementId, bodyId );
		if( !nearestElement.isFinished() ){
			if( this->evaluator->getWeightElement( nearestElement, x, y, z, coeff ) >= tolerance ){
				for(int j=0;j<dim;++j){
					values[j] = 0.0;
				}
				double* nVal = new double[dim];
				int len = nearestElement.getNodeCount();

				for(int i=0;i<len;++i){
					this->getMultiPhysicalValues( nearestElement[i], nVal );

					for(int j=0;j<dim;++j){
						values[j] += coeff[i] * nVal[j];
					}
				}
				delete[] nVal;
				return dim;
			}
		}
	}
	return 0;
}
