/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ScalarValueFileBindings                                 #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/




#include "MeshDB/kmbScalarValueFileBindings.h"
#include "MeshDB/kmbScalarValueBindings.h"
#include "MeshDB/kmbDataBindings.h"
#include <iostream>

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

const char kmb::ScalarValueFileBindings::HEADER_STRING[80]
	= "RevocapMesh::ScalarValueFileBindings                                          ";

kmb::ScalarValueFileBindings::ScalarValueFileBindings(size_t count,const char* filename,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	this->type = kmb::PhysicalValue::SCALAR;
	this->bMode = bmode;
	initialize( filename, kmb::ScalarValueFileBindings::HEADER_STRING, sizeof(double), static_cast<unsigned long>( count ) );
}

kmb::ScalarValueFileBindings::ScalarValueFileBindings(const char* filename,const kmb::ScalarValueBindings* data)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	if( data != NULL ){
		this->type = kmb::PhysicalValue::SCALAR;
		this->bMode = data->getBindingMode();
		initialize( filename, kmb::ScalarValueFileBindings::HEADER_STRING, sizeof(double), static_cast<unsigned long>(data->getIdCount()) );
		setData( 0, reinterpret_cast<const char*>(data->getDoubleArray()), static_cast<unsigned long>(data->getIdCount()) );
	}else{
		this->type = kmb::PhysicalValue::SCALAR;
		this->bMode = kmb::DataBindings::NODEVARIABLE;
		initialize( filename, kmb::ScalarValueFileBindings::HEADER_STRING, sizeof(double), 0 );
	}
}

kmb::ScalarValueFileBindings::~ScalarValueFileBindings(void)
{
}

void
kmb::ScalarValueFileBindings::clear(void)
{
}


kmb::ScalarValueBindings*
kmb::ScalarValueFileBindings::createScalarValueBindings(void)
{
	kmb::ScalarValueBindings* data = new kmb::ScalarValueBindings( this->getSize(), this->bMode );
	getData(0,reinterpret_cast<char*>(data->getDoubleArray()),this->getSize());
	return data;
}

bool
kmb::ScalarValueFileBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( 0 <= id && static_cast<unsigned long>(id) < this->dataSize && val->getType() == kmb::PhysicalValue::SCALAR ){
		double d = reinterpret_cast< kmb::ScalarValue* >(val)->getValue();
		setData( id, reinterpret_cast<const char*>(&d) );
		delete val;
		return true;
	}
	return false;
}

bool
kmb::ScalarValueFileBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && static_cast<unsigned long>(id) < this->dataSize ){
		setData( id, reinterpret_cast<const char*>(val) );
		return true;
	}
	return false;
}

bool
kmb::ScalarValueFileBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && static_cast<unsigned long>(id) < this->dataSize && index == 0){
		setData( id, reinterpret_cast<const char*>(&value) );
		return true;
	}
	return false;
}

kmb::PhysicalValue*
kmb::ScalarValueFileBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::ScalarValueFileBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id <= this->getMaxIndex() ){
		double d = 0.0;
		getData(id,reinterpret_cast<char*>(&d));
		*val = d;
		return true;
	}
	return false;
}

bool
kmb::ScalarValueFileBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id <= this->getMaxIndex() ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::ScalarValueFileBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::ScalarValueFileBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	return false;
}

size_t
kmb::ScalarValueFileBindings::getIdCount() const
{
	return getSize();
}

kmb::DataBindings::iterator
kmb::ScalarValueFileBindings::begin(void)
{
	if( !this->good() ){
		return kmb::DataBindings::iterator(NULL);
	}
	_iterator* _it = NULL;
	_it = new _iterator();
	_it->nodeValue = this;
	_it->index = 0;
	return kmb::DataBindings::iterator(_it);
}

kmb::DataBindings::const_iterator
kmb::ScalarValueFileBindings::begin(void) const
{
	return kmb::DataBindings::const_iterator(NULL);
}

kmb::ScalarValueFileBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::ScalarValueFileBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::ScalarValueFileBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::ScalarValueFileBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::ScalarValueFileBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::ScalarValueFileBindings::_iterator::getValue(double *value) const
{
	double d=0.0;
	nodeValue->getData( index, reinterpret_cast<char*>(&d) );
	*value = d;
	return true;
}

kmb::ScalarValueFileBindings::_iterator*
kmb::ScalarValueFileBindings::_iterator::operator++(void)
{
	++index;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::ScalarValueFileBindings::_iterator*
kmb::ScalarValueFileBindings::_iterator::operator++(int n)
{
	index++;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::ScalarValueFileBindings::_iterator*
kmb::ScalarValueFileBindings::_iterator::clone(void)
{
	kmb::ScalarValueFileBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::ScalarValueFileBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}

