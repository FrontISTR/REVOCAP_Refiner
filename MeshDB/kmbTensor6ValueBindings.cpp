/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Tensor6ValueBindings                                    #
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
#include "MeshDB/kmbTensor6ValueBindings.h"
#include "MeshDB/kmbPhysicalValue.h"
#include <algorithm>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::Tensor6ValueBindings::Tensor6ValueBindings(size_t count,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(true)
{
	this->type = kmb::PhysicalValue::Tensor6;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	values = new double[6*this->size];
	std::fill(values,values+(6*this->size),0.0);
}

kmb::Tensor6ValueBindings::Tensor6ValueBindings(size_t count,double* values,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(false)
{
	this->type = kmb::PhysicalValue::Tensor6;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	this->values = values;
}

kmb::Tensor6ValueBindings::~Tensor6ValueBindings(void)
{
	if( values && deletable ){
		delete[] values;
		values = NULL;
	}
}

void
kmb::Tensor6ValueBindings::clear(void)
{
}

bool
kmb::Tensor6ValueBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && id < size && val->getType() == kmb::PhysicalValue::Tensor6 ){
		values[6*id  ] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(0);
		values[6*id+1] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(1);
		values[6*id+2] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(2);
		values[6*id+3] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(3);
		values[6*id+4] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(4);
		values[6*id+5] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(5);
		delete val;
		return true;
	}
	return false;
}

bool
kmb::Tensor6ValueBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && id < size ){
		values[6*id  ] = val[0];
		values[6*id+1] = val[1];
		values[6*id+2] = val[2];
		values[6*id+3] = val[3];
		values[6*id+4] = val[4];
		values[6*id+5] = val[5];
		return true;
	}
	return false;
}

bool
kmb::Tensor6ValueBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && id < size && 0 <= index && index < 6){
		values[6*id+index] = value;
		return true;
	}
	return false;
}

bool
kmb::Tensor6ValueBindings::scalar(double r)
{
	for(int i=0;i<size*6;++i){
		values[i] *= r;
	}
	return true;
}

kmb::PhysicalValue*
kmb::Tensor6ValueBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::Tensor6ValueBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id < size ){
		val[0] = values[6*id  ];
		val[1] = values[6*id+1];
		val[2] = values[6*id+2];
		val[3] = values[6*id+3];
		val[4] = values[6*id+4];
		val[5] = values[6*id+5];
		return true;
	}
	return false;
}

bool
kmb::Tensor6ValueBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < size ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::Tensor6ValueBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::Tensor6ValueBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		values[6*new_id] = values[6*old_id];
		values[6*new_id+1] = values[6*old_id+1];
		values[6*new_id+2] = values[6*old_id+2];
		values[6*new_id+3] = values[6*old_id+3];
		values[6*new_id+4] = values[6*old_id+4];
		values[6*new_id+5] = values[6*old_id+5];
		return true;
	}
	return false;
}

size_t
kmb::Tensor6ValueBindings::getIdCount() const
{
	return static_cast< size_t >( size );
}

double*
kmb::Tensor6ValueBindings::getDoubleArray(void)
{
	return this->values;
}

const double*
kmb::Tensor6ValueBindings::getDoubleArray(void) const
{
	return this->values;
}

kmb::DataBindings::iterator
kmb::Tensor6ValueBindings::begin(void)
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
kmb::Tensor6ValueBindings::begin(void) const
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

kmb::Tensor6ValueBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::Tensor6ValueBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::Tensor6ValueBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::Tensor6ValueBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::Tensor6ValueBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::Tensor6ValueBindings::_iterator::getValue(double *value) const
{
	value[0] = nodeValue->values[ 6*index   ];
	value[1] = nodeValue->values[ 6*index+1 ];
	value[2] = nodeValue->values[ 6*index+2 ];
	value[3] = nodeValue->values[ 6*index+3 ];
	value[4] = nodeValue->values[ 6*index+4 ];
	value[5] = nodeValue->values[ 6*index+5 ];
	return true;
}

kmb::Tensor6ValueBindings::_iterator*
kmb::Tensor6ValueBindings::_iterator::operator++(void)
{
	++index;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Tensor6ValueBindings::_iterator*
kmb::Tensor6ValueBindings::_iterator::operator++(int n)
{
	index++;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Tensor6ValueBindings::_iterator*
kmb::Tensor6ValueBindings::_iterator::clone(void)
{
	kmb::Tensor6ValueBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::Tensor6ValueBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}

