/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ScalarValueBindings                                     #
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
#include "MeshDB/kmbScalarValueBindings.h"
#include <algorithm>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::ScalarValueBindings::ScalarValueBindings(size_t count,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(true)
{
	this->type = kmb::PhysicalValue::Scalar;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	values = new double[this->size];
	std::fill(values,values+this->size,0.0);
}

kmb::ScalarValueBindings::ScalarValueBindings(size_t count,double* values,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(false)
{
	this->type = kmb::PhysicalValue::Scalar;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	this->values = values;
}

kmb::ScalarValueBindings::~ScalarValueBindings(void)
{
	if( values && deletable ){
		delete[] values;
		values = NULL;
	}
}

void
kmb::ScalarValueBindings::clear(void)
{
}

bool
kmb::ScalarValueBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && id < size && val->getType() == kmb::PhysicalValue::Scalar ){
		values[id] = reinterpret_cast< kmb::ScalarValue* >(val)->getValue();
		delete val;
		return true;
	}
	return false;
}

bool
kmb::ScalarValueBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && id < size ){
		values[id] = val[0];
		return true;
	}
	return false;
}

bool
kmb::ScalarValueBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && id < size && index == 0){
		values[id] = value;
		return true;
	}
	return false;
}

bool
kmb::ScalarValueBindings::scalar(double r)
{
	for(int i=0;i<size;++i){
		values[i] *= r;
	}
	return true;
}

kmb::PhysicalValue*
kmb::ScalarValueBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::ScalarValueBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id < size ){
		val[0] = values[id];
		return true;
	}
	return false;
}

bool
kmb::ScalarValueBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < size ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::ScalarValueBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::ScalarValueBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		double tmp = values[new_id];
		values[new_id] = values[old_id];
		values[old_id] = tmp;
		return true;
	}
	return false;
}

size_t
kmb::ScalarValueBindings::getIdCount() const
{
	return static_cast< size_t >( size );
}

double*
kmb::ScalarValueBindings::getDoubleArray(void)
{
	return this->values;
}

const double*
kmb::ScalarValueBindings::getDoubleArray(void) const
{
	return this->values;
}

kmb::DataBindings::iterator
kmb::ScalarValueBindings::begin(void)
{
	if( values == NULL ){
		return kmb::DataBindings::iterator(NULL);
	}
	_iterator* _it = NULL;
	_it = new _iterator();
	_it->nodeValue = this;
	_it->index = 0;
	return kmb::DataBindings::iterator(_it);
}

kmb::DataBindings::const_iterator
kmb::ScalarValueBindings::begin(void) const
{
	if( values == NULL ){
		return kmb::DataBindings::const_iterator(NULL);
	}
	_iterator* _it = NULL;
	_it = new _iterator();
	_it->nodeValue = this;
	_it->index = 0;
	return kmb::DataBindings::const_iterator(_it);
}

kmb::ScalarValueBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::ScalarValueBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::ScalarValueBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::ScalarValueBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::ScalarValueBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::ScalarValueBindings::_iterator::getValue(double *value) const
{
	value[0] = nodeValue->values[ index ];
	return true;
}

kmb::ScalarValueBindings::_iterator*
kmb::ScalarValueBindings::_iterator::operator++(void)
{
	++index;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::ScalarValueBindings::_iterator*
kmb::ScalarValueBindings::_iterator::operator++(int n)
{
	index++;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::ScalarValueBindings::_iterator*
kmb::ScalarValueBindings::_iterator::clone(void)
{
	kmb::ScalarValueBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::ScalarValueBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}

