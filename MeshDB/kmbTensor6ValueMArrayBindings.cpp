/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Tensor6ValueMArrayBindings                              #
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
#include "MeshDB/kmbTensor6ValueMArrayBindings.h"

kmb::Tensor6ValueMArrayBindings::Tensor6ValueMArrayBindings(size_t size,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, vArray(DBL_MAX)
, aIndex()
, count(0)
{
	this->type = kmb::PhysicalValue::Tensor6;
	this->bMode = bmode;
	vArray.initialize(size);
	aIndex = vArray.getBLArrayIndex(0);
}

kmb::Tensor6ValueMArrayBindings::~Tensor6ValueMArrayBindings(void)
{
	clear();
}

void
kmb::Tensor6ValueMArrayBindings::clear(void)
{
	vArray.clearData();
	aIndex = vArray.getBLArrayIndex(0);
	count = 0;
}

bool
kmb::Tensor6ValueMArrayBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && val->getType() == kmb::PhysicalValue::Tensor6 ){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		double q[6];
		reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(q);
		if( vArray.set( ind, q ) ){
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
kmb::Tensor6ValueMArrayBindings::setPhysicalValue(kmb::idType id,double* val)
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
kmb::Tensor6ValueMArrayBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && 0 <= index && index < 6 ){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		if( vArray.setComponent( ind, index, &value) ){
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
kmb::Tensor6ValueMArrayBindings::scalar(double r)
{
	kmb::BLArrayIndex index;
	vArray.first(index);
	double d[6];
	while( vArray.valid(index) ){
		if( vArray.get(index,d)){
			for(int i=0;i<6;++i){
				d[i] *= r;
			}
			vArray.set(index,d);
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
kmb::Tensor6ValueMArrayBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

bool
kmb::Tensor6ValueMArrayBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.get( static_cast<size_t>(id), val ) ){
			return true;
		}
	}
	return false;
}

bool
kmb::Tensor6ValueMArrayBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.has( static_cast<size_t>(id) ) ){
			return true;
		}
	}
	return false;
}

bool
kmb::Tensor6ValueMArrayBindings::deleteId(kmb::idType id)
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
kmb::Tensor6ValueMArrayBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	kmb::idType size = static_cast<kmb::idType>(aIndex.getIndex());
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		double tmp_old[6], tmp_new[6];
		if( vArray.get( static_cast<size_t>(old_id), tmp_old ) &&
			vArray.get( static_cast<size_t>(new_id), tmp_new ) &&
			vArray.set( static_cast<size_t>(new_id), tmp_old ) &&
			vArray.set( static_cast<size_t>(old_id), tmp_new ) )
		{
			return true;
		}
	}
	return false;
}

size_t
kmb::Tensor6ValueMArrayBindings::getIdCount() const
{
	return count;
}

kmb::DataBindings::iterator
kmb::Tensor6ValueMArrayBindings::begin(void)
{
	if( this->getIdCount() == 0 ){
		return kmb::DataBindings::iterator(NULL);
	}
	kmb::Tensor6ValueMArrayBindings::_iteratorMArray* _it = NULL;
	_it = new kmb::Tensor6ValueMArrayBindings::_iteratorMArray();
	if( _it ){
		_it->values = this;
		this->vArray.first( _it->aIndex );
	}
	return kmb::DataBindings::iterator(_it);
}

kmb::DataBindings::const_iterator
kmb::Tensor6ValueMArrayBindings::begin(void) const
{
	if( this->getIdCount() == 0 ){
		return kmb::DataBindings::const_iterator(NULL);
	}
	kmb::Tensor6ValueMArrayBindings::_iteratorMArray* _it = NULL;
	_it = new kmb::Tensor6ValueMArrayBindings::_iteratorMArray();
	if( _it ){
		_it->values = this;
		this->vArray.first( _it->aIndex );
	}
	return kmb::DataBindings::const_iterator(_it);
}

kmb::Tensor6ValueMArrayBindings::_iteratorMArray::_iteratorMArray(void)
: kmb::DataBindings::_iterator()
, values( NULL )
, aIndex()
{
}

kmb::Tensor6ValueMArrayBindings::_iteratorMArray::~_iteratorMArray(void)
{
}

kmb::idType
kmb::Tensor6ValueMArrayBindings::_iteratorMArray::getId(void) const
{
	return static_cast<kmb::idType>(aIndex.getIndex());
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

bool
kmb::Tensor6ValueMArrayBindings::_iteratorMArray::getFace(kmb::Face &f) const
{
	return false;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

kmb::PhysicalValue*
kmb::Tensor6ValueMArrayBindings::_iteratorMArray::getValue(void) const
{
	return NULL;
}

bool
kmb::Tensor6ValueMArrayBindings::_iteratorMArray::getValue(double *value) const
{
	if( values->vArray.get( aIndex, value ) ){
		return true;
	}
	return false;
}

kmb::DataBindings::_iterator*
kmb::Tensor6ValueMArrayBindings::_iteratorMArray::operator++(void)
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
kmb::Tensor6ValueMArrayBindings::_iteratorMArray::operator++(int n)
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
kmb::Tensor6ValueMArrayBindings::_iteratorMArray::clone(void)
{
	kmb::Tensor6ValueMArrayBindings::_iteratorMArray* _itClone = NULL;
	_itClone = new kmb::Tensor6ValueMArrayBindings::_iteratorMArray();
	if( _itClone ){
		_itClone->values = this->values;
		_itClone->aIndex = this->aIndex;
	}
	return _itClone;
}

