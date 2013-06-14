/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ScalarValueMArrayBindings                               #
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
#include <cfloat>
#include "MeshDB/kmbScalarValueMArrayBindings.h"

kmb::ScalarValueMArrayBindings::ScalarValueMArrayBindings(size_t size,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, vArray(DBL_MAX)
, aIndex()
, count(0)
{
	this->type = kmb::PhysicalValue::Scalar;
	this->bMode = bmode;
	vArray.initialize(size);
	aIndex = vArray.getBLArrayIndex(0);
}

kmb::ScalarValueMArrayBindings::~ScalarValueMArrayBindings(void)
{
	clear();
}

void
kmb::ScalarValueMArrayBindings::clear(void)
{
	vArray.clearData();
	aIndex = vArray.getBLArrayIndex(0);
	count = 0;
}

bool
kmb::ScalarValueMArrayBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && val->getType() == kmb::PhysicalValue::Scalar ){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		double q = reinterpret_cast< kmb::ScalarValue* >(val)->getValue();
		if( vArray.set( ind, &q ) ){
			if( aIndex <= ind ){
				aIndex = ind;
				++aIndex;
			}
			++count;
			return true;
		}
	}
	return false;
}

bool
kmb::ScalarValueMArrayBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id ){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		if( vArray.set( ind, val ) ){
			if( aIndex <= ind ){
				aIndex = ind;
				++aIndex;
			}
			++count;
			return true;
		}
	}
	return false;
}

bool
kmb::ScalarValueMArrayBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && index == 0){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		if( vArray.set( ind, &value ) ){
			if( aIndex <= ind ){
				aIndex = ind;
				++aIndex;
			}
			++count;
			return true;
		}
	}
	return false;
}

bool
kmb::ScalarValueMArrayBindings::scalar(double r)
{
	kmb::BLArrayIndex index;
	vArray.first(index);
	while( vArray.valid(index) ){
		double d(0.0);
		if( vArray.get(index,&d)){
			d *= r;
			vArray.set(index,&d);
		}
		++index;
	}
	return true;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::PhysicalValue*
kmb::ScalarValueMArrayBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

bool
kmb::ScalarValueMArrayBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.get( static_cast<size_t>(id), val ) ){
			return true;
		}
	}
	return false;
}

bool
kmb::ScalarValueMArrayBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.has( static_cast<size_t>(id) ) ){
			return true;
		}
	}
	return false;
}

bool
kmb::ScalarValueMArrayBindings::deleteId(kmb::idType id)
{
	if( 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.erase( static_cast<size_t>(id)) ){
			--count;
			return true;
		}
	}
	return false;
}

bool
kmb::ScalarValueMArrayBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	kmb::idType size = static_cast<kmb::idType>(aIndex.getIndex());
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		double tmp_old=0.0, tmp_new=0.0;
		if( vArray.get( static_cast<size_t>(old_id), &tmp_old ) &&
			vArray.get( static_cast<size_t>(new_id), &tmp_new ) &&
			vArray.set( static_cast<size_t>(new_id), &tmp_old ) &&
			vArray.set( static_cast<size_t>(old_id), &tmp_new ) )
		{
			return true;
		}
	}
	return false;
}

size_t
kmb::ScalarValueMArrayBindings::getIdCount() const
{
	return count;
}

kmb::DataBindings::iterator
kmb::ScalarValueMArrayBindings::begin(void)
{
	if( this->getIdCount() == 0 ){
		return kmb::DataBindings::iterator(NULL);
	}
	kmb::ScalarValueMArrayBindings::_iteratorMArray* _it = NULL;
	_it = new kmb::ScalarValueMArrayBindings::_iteratorMArray();
	if( _it ){
		_it->values = this;
		this->vArray.first( _it->aIndex );
	}
	return kmb::DataBindings::iterator(_it);
}

kmb::DataBindings::const_iterator
kmb::ScalarValueMArrayBindings::begin(void) const
{
	if( this->getIdCount() == 0 ){
		return kmb::DataBindings::const_iterator(NULL);
	}
	kmb::ScalarValueMArrayBindings::_iteratorMArray* _it = NULL;
	_it = new kmb::ScalarValueMArrayBindings::_iteratorMArray();
	if( _it ){
		_it->values = this;
		this->vArray.first( _it->aIndex );
	}
	return kmb::DataBindings::const_iterator(_it);
}

kmb::ScalarValueMArrayBindings::_iteratorMArray::_iteratorMArray(void)
: kmb::DataBindings::_iterator()
, values( NULL )
, aIndex()
{
}

kmb::ScalarValueMArrayBindings::_iteratorMArray::~_iteratorMArray(void)
{
}

kmb::idType
kmb::ScalarValueMArrayBindings::_iteratorMArray::getId(void) const
{
	return static_cast<kmb::idType>(aIndex.getIndex());
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

bool
kmb::ScalarValueMArrayBindings::_iteratorMArray::getFace(kmb::Face &f) const
{
	return false;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

kmb::PhysicalValue*
kmb::ScalarValueMArrayBindings::_iteratorMArray::getValue(void) const
{
	return NULL;
}

bool
kmb::ScalarValueMArrayBindings::_iteratorMArray::getValue(double *value) const
{
	if( values->vArray.get( aIndex, value ) ){
		return true;
	}
	return false;
}

kmb::DataBindings::_iterator*
kmb::ScalarValueMArrayBindings::_iteratorMArray::operator++(void)
{
	do{
		++aIndex;
		if( values->vArray.has(aIndex) ){
			return this;
		}
	}while( values->vArray.valid(aIndex) );
	return NULL;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::DataBindings::_iterator*
kmb::ScalarValueMArrayBindings::_iteratorMArray::operator++(int n)
{
	do{
		aIndex++;
		if( values->vArray.has(aIndex) ){
			return this;
		}
	}while( values->vArray.valid(aIndex) );
	return NULL;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

kmb::DataBindings::_iterator*
kmb::ScalarValueMArrayBindings::_iteratorMArray::clone(void)
{
	kmb::ScalarValueMArrayBindings::_iteratorMArray* _itClone = NULL;
	_itClone = new kmb::ScalarValueMArrayBindings::_iteratorMArray();
	if( _itClone ){
		_itClone->values = this->values;
		_itClone->aIndex = this->aIndex;
	}
	return _itClone;
}

