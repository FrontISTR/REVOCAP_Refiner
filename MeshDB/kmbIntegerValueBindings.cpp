/*----------------------------------------------------------------------
#                                                                      #
# Software Name : Advance/REVOCAP_PrePost version 3.2                  #
# Class Name : IntegerValueBindings                                    #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/10/21     #
#                                                                      #
#      Copyright (C) 2013 Advancesoft corp.                            #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbIntegerValueBindings.h"
#include <algorithm>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::IntegerValueBindings::IntegerValueBindings(size_t count,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deallocatable(true)
{
	this->type = kmb::PhysicalValue::Integer;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	values = new int[this->size];
	std::fill(values,values+this->size,0);
}

kmb::IntegerValueBindings::IntegerValueBindings(size_t count,int* values,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deallocatable(false)
{
	this->type = kmb::PhysicalValue::Integer;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	this->values = values;
}

kmb::IntegerValueBindings::~IntegerValueBindings(void)
{
	if( values && deallocatable ){
		delete[] values;
		values = NULL;
	}
}

void
kmb::IntegerValueBindings::clear(void)
{
	clear(0);
}

void kmb::IntegerValueBindings::clear(int v)
{
	std::fill(values,values+size,v);
}

bool
kmb::IntegerValueBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && id < size && val->getType() == kmb::PhysicalValue::Integer ){
		values[id] = reinterpret_cast< kmb::IntegerValue* >(val)->getValue();
		delete val;
		return true;
	}
	return false;
}

bool
kmb::IntegerValueBindings::setPhysicalValue(kmb::idType id,long* val)
{
	if( val && 0 <= id && id < size ){
		values[id] = val[0];
		return true;
	}
	return false;
}

bool
kmb::IntegerValueBindings::setValue(kmb::idType id, long value,int index)
{
	if( 0 <= id && id < size && index == 0){
		values[id] = static_cast<int>(value);
		return true;
	}
	return false;
}

kmb::PhysicalValue*
kmb::IntegerValueBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::IntegerValueBindings::getPhysicalValue(kmb::idType id, long* val) const
{
	if( val && 0 <= id && id < size ){
		val[0] = values[id];
		return true;
	}
	return false;
}

bool
kmb::IntegerValueBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < size ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::IntegerValueBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::IntegerValueBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		int tmp = values[new_id];
		values[new_id] = values[old_id];
		values[old_id] = tmp;
		return true;
	}
	return false;
}

size_t
kmb::IntegerValueBindings::getIdCount() const
{
	return static_cast< size_t >( size );
}

int*
kmb::IntegerValueBindings::getIntArray(void)
{
	return this->values;
}

const int*
kmb::IntegerValueBindings::getIntArray(void) const
{
	return this->values;
}

kmb::DataBindings::iterator
kmb::IntegerValueBindings::begin(void)
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
kmb::IntegerValueBindings::begin(void) const
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

kmb::IntegerValueBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::IntegerValueBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::IntegerValueBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::IntegerValueBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::IntegerValueBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::IntegerValueBindings::_iterator::getValue(long *value) const
{
	value[0] = static_cast<long>(nodeValue->values[ index ]);
	return true;
}

kmb::IntegerValueBindings::_iterator*
kmb::IntegerValueBindings::_iterator::operator++(void)
{
	++index;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::IntegerValueBindings::_iterator*
kmb::IntegerValueBindings::_iterator::operator++(int n)
{
	index++;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::IntegerValueBindings::_iterator*
kmb::IntegerValueBindings::_iterator::clone(void)
{
	kmb::IntegerValueBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::IntegerValueBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}

