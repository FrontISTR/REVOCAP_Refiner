/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Vector2ValueBindings                                    #
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
#include "MeshDB/kmbVector2ValueBindings.h"
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

kmb::Vector2ValueBindings::Vector2ValueBindings(size_t count,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(true)
{
	this->type = kmb::PhysicalValue::Vector2;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	values = new double[2*this->size];
	std::fill(values,values+(2*this->size),0.0);
}

kmb::Vector2ValueBindings::Vector2ValueBindings(size_t count,double* values,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(false)
{
	this->type = kmb::PhysicalValue::Vector2;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	this->values = values;
}

kmb::Vector2ValueBindings::~Vector2ValueBindings(void)
{
	if( values && deletable ){
		delete[] values;
		values = NULL;
	}
}

void
kmb::Vector2ValueBindings::clear(void)
{
}

bool
kmb::Vector2ValueBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && id < size && val->getType() == kmb::PhysicalValue::Vector2 ){
		values[2*id  ] = reinterpret_cast< kmb::Vector2Value* >(val)->getValue(0);
		values[2*id+1] = reinterpret_cast< kmb::Vector2Value* >(val)->getValue(1);
		delete val;
		return true;
	}
	return false;
}

bool
kmb::Vector2ValueBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && id < size ){
		values[2*id  ] = val[0];
		values[2*id+1] = val[1];
		return true;
	}
	return false;
}

bool
kmb::Vector2ValueBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && id < size && 0 <= index && index < 2 ){
		values[2*id+index] = value;
		return true;
	}
	return false;
}

bool
kmb::Vector2ValueBindings::scalar(double r)
{
	for(int i=0;i<size*2;++i){
		values[i] *= r;
	}
	return true;
}

kmb::PhysicalValue*
kmb::Vector2ValueBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::Vector2ValueBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id < size ){
		val[0] = values[2*id  ];
		val[1] = values[2*id+1];
		return true;
	}
	return false;
}

bool
kmb::Vector2ValueBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < size ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::Vector2ValueBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::Vector2ValueBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		values[2*new_id] = values[2*old_id];
		values[2*new_id+1] = values[2*old_id+1];
		return true;
	}
	return false;
}

size_t
kmb::Vector2ValueBindings::getIdCount() const
{
	return static_cast< size_t >( size );
}

double*
kmb::Vector2ValueBindings::getDoubleArray(void)
{
	return this->values;
}

const double*
kmb::Vector2ValueBindings::getDoubleArray(void) const
{
	return this->values;
}

kmb::DataBindings::iterator
kmb::Vector2ValueBindings::begin(void)
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
kmb::Vector2ValueBindings::begin(void) const
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

kmb::Vector2ValueBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::Vector2ValueBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::Vector2ValueBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::Vector2ValueBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::Vector2ValueBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::Vector2ValueBindings::_iterator::getValue(double *value) const
{
	value[0] = nodeValue->values[ 2*index   ];
	value[1] = nodeValue->values[ 2*index+1 ];
	return true;
}

kmb::Vector2ValueBindings::_iterator*
kmb::Vector2ValueBindings::_iterator::operator++(void)
{
	++index;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector2ValueBindings::_iterator*
kmb::Vector2ValueBindings::_iterator::operator++(int n)
{
	index++;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector2ValueBindings::_iterator*
kmb::Vector2ValueBindings::_iterator::clone(void)
{
	kmb::Vector2ValueBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::Vector2ValueBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}

