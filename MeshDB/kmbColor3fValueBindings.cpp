/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Color3fValueBindings                                    #
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
#include "MeshDB/kmbColor3fValueBindings.h"
#include "MeshDB/kmbPhysicalValue.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::Color3fValueBindings::Color3fValueBindings(size_t count,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
{
	this->type = kmb::PhysicalValue::VECTOR3;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	values = new float[3*this->size];
}

kmb::Color3fValueBindings::~Color3fValueBindings(void)
{
	if( values ){
		delete[] values;
		values = NULL;
	}
}

void
kmb::Color3fValueBindings::clear(void)
{
}

bool
kmb::Color3fValueBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && id < size && val->getType() == kmb::PhysicalValue::VECTOR3 ){
		values[3*id  ] = static_cast<float>( reinterpret_cast< kmb::Vector3Value* >(val)->getValue(0) );
		values[3*id+1] = static_cast<float>( reinterpret_cast< kmb::Vector3Value* >(val)->getValue(1) );
		values[3*id+2] = static_cast<float>( reinterpret_cast< kmb::Vector3Value* >(val)->getValue(2) );
		delete val;
		return true;
	}
	return false;
}

bool
kmb::Color3fValueBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && id < size ){
		values[3*id  ] = static_cast<float>( val[0] );
		values[3*id+1] = static_cast<float>( val[1] );
		values[3*id+2] = static_cast<float>( val[2] );
		return true;
	}
	return false;
}

bool
kmb::Color3fValueBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && id < size && 0 <= index && index < 3 ){
		values[3*id+index] = static_cast<float>( value );
		return true;
	}
	return false;
}

bool
kmb::Color3fValueBindings::setColorValue(kmb::idType id, float *rgb)
{
	if( rgb && 0 <= id && id < size ){
		values[3*id  ] = rgb[0];
		values[3*id+1] = rgb[1];
		values[3*id+2] = rgb[2];
		return true;
	}
	return false;
}

kmb::PhysicalValue*
kmb::Color3fValueBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::Color3fValueBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id < size ){
		val[0] = values[3*id  ];
		val[1] = values[3*id+1];
		val[2] = values[3*id+2];
		return true;
	}
	return false;
}

bool
kmb::Color3fValueBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < size ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::Color3fValueBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::Color3fValueBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		values[3*new_id] = values[3*old_id];
		values[3*new_id+1] = values[3*old_id+1];
		values[3*new_id+2] = values[3*old_id+2];
		return true;
	}
	return false;
}

size_t
kmb::Color3fValueBindings::getIdCount() const
{
	return static_cast< size_t >( size );
}

float*
kmb::Color3fValueBindings::getFloatArray(void)
{
	return this->values;
}

const float*
kmb::Color3fValueBindings::getFloatArray(void) const
{
	return this->values;
}

kmb::DataBindings::iterator
kmb::Color3fValueBindings::begin(void)
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
kmb::Color3fValueBindings::begin(void) const
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

kmb::Color3fValueBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::Color3fValueBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::Color3fValueBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::Color3fValueBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::Color3fValueBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::Color3fValueBindings::_iterator::getValue(double *value) const
{
	value[0] = nodeValue->values[ 3*index   ];
	value[1] = nodeValue->values[ 3*index+1 ];
	value[2] = nodeValue->values[ 3*index+2 ];
	return true;
}

kmb::Color3fValueBindings::_iterator*
kmb::Color3fValueBindings::_iterator::operator++(void)
{
	++index;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Color3fValueBindings::_iterator*
kmb::Color3fValueBindings::_iterator::operator++(int n)
{
	index++;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Color3fValueBindings::_iterator*
kmb::Color3fValueBindings::_iterator::clone(void)
{
	kmb::Color3fValueBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::Color3fValueBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}

