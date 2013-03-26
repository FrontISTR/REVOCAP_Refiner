/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : IntegerMapBindings                                      #
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
#include "MeshDB/kmbIntegerMapBindings.h"

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::IntegerMapBindings::IntegerMapBindings(kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
{
	this->type = kmb::PhysicalValue::Integer;
	this->bMode = bmode;
}

const char*
kmb::IntegerMapBindings::getContainerType(void) const
{
	return "IntegerMapBindings";
}

kmb::IntegerMapBindings::~IntegerMapBindings(void)
{
	clear();
}

void
kmb::IntegerMapBindings::clear(void)
{
	mapValues.clear();
}

size_t
kmb::IntegerMapBindings::getIdCount(void) const
{
	return mapValues.size();
}

bool
kmb::IntegerMapBindings::hasId(kmb::idType id) const
{
	return (mapValues.find(id) != mapValues.end());
}

bool
kmb::IntegerMapBindings::deleteId(kmb::idType id)
{
	std::map< kmb::idType, int >::iterator pIter = mapValues.find(id);
	if( pIter != mapValues.end() ){
		mapValues.erase( pIter );
		return true;
	}
	return false;
}

bool
kmb::IntegerMapBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	std::map< kmb::idType, int >::iterator pIter = mapValues.find(old_id);
	if( pIter != mapValues.end() && mapValues.find(new_id) == mapValues.end() ){
		int n = pIter->second;
		mapValues.erase(pIter);
		mapValues.insert( std::pair<kmb::idType,int>(new_id,n) );
		return true;
	}
	return false;
}

bool
kmb::IntegerMapBindings::setPhysicalValue(kmb::idType id, kmb::PhysicalValue* val)
{
	if( val && 0 <= id && val->getType() == kmb::PhysicalValue::Integer ){
		mapValues.insert( std::pair<kmb::idType,int>(id,
			reinterpret_cast< kmb::IntegerValue* >(val)->getValue() ) );
		delete val;
		return true;
	}
	return false;
}

bool
kmb::IntegerMapBindings::setPhysicalValue(kmb::idType id, long *value)
{
	if( value && 0 <= id ){
		mapValues.insert( std::pair<kmb::idType,int>(id,static_cast<int>(*value)) );
		return true;
	}
	return false;
}

bool
kmb::IntegerMapBindings::setValue(kmb::idType id, long value,int index)
{
	if( index == 0 ){
		mapValues.insert( std::pair<kmb::idType,int>(id,static_cast<int>(value)) );
		return true;
	}
	return false;
}


bool
kmb::IntegerMapBindings::getPhysicalValue(kmb::idType id, long *value) const
{
	std::map< kmb::idType, int >::const_iterator pIter = mapValues.find(id);
	if( value && pIter != mapValues.end() ){
		value[0] = static_cast<long>(pIter->second);
		return true;
	}
	return false;
}

kmb::DataBindings::iterator
kmb::IntegerMapBindings::begin(void)
{
	if( mapValues.size() == 0 ){
		return kmb::DataBindings::iterator(NULL);
	}
	_iterator* _it = NULL;
	_it = new _iterator();
	_it->dataIterator = this->mapValues.begin();
	_it->endIterator = this->mapValues.end();
	return kmb::DataBindings::iterator(_it);
}

kmb::DataBindings::const_iterator
kmb::IntegerMapBindings::begin(void) const
{
	if( mapValues.size() == 0 ){
		return kmb::DataBindings::const_iterator(NULL);
	}
	_iterator* _it = NULL;
	_it = new _iterator();
	_it->dataIterator = this->mapValues.begin();
	_it->endIterator = this->mapValues.end();
	return kmb::DataBindings::const_iterator(_it);
}

kmb::IntegerMapBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::IntegerMapBindings::_iterator::getId(void) const
{
	return static_cast< kmb::idType >( dataIterator->first );
}

bool
kmb::IntegerMapBindings::_iterator::getValue(long *value) const
{
	value[0] = static_cast<long>( dataIterator->second );
	return true;
}

kmb::IntegerMapBindings::_iterator*
kmb::IntegerMapBindings::_iterator::operator++(void)
{
	++dataIterator;
	if( dataIterator != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::IntegerMapBindings::_iterator*
kmb::IntegerMapBindings::_iterator::operator++(int n)
{
	dataIterator++;
	if( dataIterator != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::IntegerMapBindings::_iterator*
kmb::IntegerMapBindings::_iterator::clone(void)
{
	kmb::IntegerMapBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::IntegerMapBindings::_iterator();
	if( _itClone ){
		_itClone->dataIterator = this->dataIterator;
		_itClone->endIterator = this->endIterator;
	}
	return _itClone;
}

