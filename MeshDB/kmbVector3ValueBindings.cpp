/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Vector3ValueBindings                                    #
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
#include "MeshDB/kmbVector3ValueBindings.h"
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

kmb::Vector3ValueBindings::Vector3ValueBindings(size_t count,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(true)
{
	this->type = kmb::PhysicalValue::Vector3;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	values = new double[3*this->size];
	std::fill(values,values+(3*this->size),0.0);
}

kmb::Vector3ValueBindings::Vector3ValueBindings(size_t count,double* values,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, size(0)
, values(NULL)
, deletable(false)
{
	this->type = kmb::PhysicalValue::Vector3;
	this->bMode = bmode;
	this->size = static_cast<int>( count );
	this->values = values;
}

kmb::Vector3ValueBindings::~Vector3ValueBindings(void)
{
	if( values && deletable){
		delete[] values;
		values = NULL;
	}
}

void
kmb::Vector3ValueBindings::clear(void)
{
}

bool
kmb::Vector3ValueBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && id < size && val->getType() == kmb::PhysicalValue::Vector3 ){
		values[3*id  ] = reinterpret_cast< kmb::Vector3Value* >(val)->getValue(0);
		values[3*id+1] = reinterpret_cast< kmb::Vector3Value* >(val)->getValue(1);
		values[3*id+2] = reinterpret_cast< kmb::Vector3Value* >(val)->getValue(2);
		delete val;
		return true;
	}
	return false;
}

bool
kmb::Vector3ValueBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && id < size ){
		values[3*id  ] = val[0];
		values[3*id+1] = val[1];
		values[3*id+2] = val[2];
		return true;
	}
	return false;
}

bool
kmb::Vector3ValueBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && id < size && 0 <= index && index < 3 ){
		values[3*id+index] = value;
		return true;
	}
	return false;
}

bool
kmb::Vector3ValueBindings::scalar(double r)
{
	for(int i=0;i<size*3;++i){
		values[i] *= r;
	}
	return true;
}

kmb::PhysicalValue*
kmb::Vector3ValueBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::Vector3ValueBindings::getPhysicalValue(kmb::idType id, double *val) const
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
kmb::Vector3ValueBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < size ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::Vector3ValueBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::Vector3ValueBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
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
kmb::Vector3ValueBindings::getIdCount() const
{
	return static_cast< size_t >( size );
}

double*
kmb::Vector3ValueBindings::getDoubleArray(void)
{
	return this->values;
}

const double*
kmb::Vector3ValueBindings::getDoubleArray(void) const
{
	return this->values;
}

kmb::DataBindings::iterator
kmb::Vector3ValueBindings::begin(void)
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
kmb::Vector3ValueBindings::begin(void) const
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

kmb::Vector3ValueBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::Vector3ValueBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::Vector3ValueBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::Vector3ValueBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::Vector3ValueBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::Vector3ValueBindings::_iterator::getValue(double *value) const
{
	value[0] = nodeValue->values[ 3*index   ];
	value[1] = nodeValue->values[ 3*index+1 ];
	value[2] = nodeValue->values[ 3*index+2 ];
	return true;
}

kmb::Vector3ValueBindings::_iterator*
kmb::Vector3ValueBindings::_iterator::operator++(void)
{
	++index;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector3ValueBindings::_iterator*
kmb::Vector3ValueBindings::_iterator::operator++(int n)
{
	index++;
	if( index < nodeValue->size ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector3ValueBindings::_iterator*
kmb::Vector3ValueBindings::_iterator::clone(void)
{
	kmb::Vector3ValueBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::Vector3ValueBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}

