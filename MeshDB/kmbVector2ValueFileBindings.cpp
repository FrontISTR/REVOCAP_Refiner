/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Vector2ValueFileBindings                                #
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




#include "MeshDB/kmbVector2ValueFileBindings.h"
#include "MeshDB/kmbVector2ValueBindings.h"
#include "MeshDB/kmbDataBindings.h"
#include <iostream>

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

const char kmb::Vector2ValueFileBindings::HEADER_STRING[80]
	= "RevocapMesh::Vector2ValueFileBindings                                         ";

kmb::Vector2ValueFileBindings::Vector2ValueFileBindings(size_t count,const char* filename,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	this->type = kmb::PhysicalValue::VECTOR2;
	this->bMode = bmode;
	initialize( filename, kmb::Vector2ValueFileBindings::HEADER_STRING, 2*sizeof(double), static_cast<long>( count ) );
}

kmb::Vector2ValueFileBindings::Vector2ValueFileBindings(const char* filename,const kmb::Vector2ValueBindings* data)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	if( data != NULL ){
		this->type = kmb::PhysicalValue::VECTOR2;
		this->bMode = data->getBindingMode();
		initialize( filename, kmb::Vector2ValueFileBindings::HEADER_STRING, 2*sizeof(double), static_cast<unsigned long>(data->getIdCount()) );
		setData( 0, reinterpret_cast<const char*>(data->getDoubleArray()), static_cast<unsigned long>(data->getIdCount()) );
	}else{
		this->type = kmb::PhysicalValue::VECTOR2;
		this->bMode = kmb::DataBindings::NODEVARIABLE;
		initialize( filename, kmb::Vector2ValueFileBindings::HEADER_STRING, 2*sizeof(double), 0 );
	}
}

kmb::Vector2ValueFileBindings::~Vector2ValueFileBindings(void)
{
}

void
kmb::Vector2ValueFileBindings::clear(void)
{
}

kmb::Vector2ValueBindings*
kmb::Vector2ValueFileBindings::createVector2ValueBindings(void)
{
	kmb::Vector2ValueBindings* data = new kmb::Vector2ValueBindings( this->getSize(), this->bMode );
	getData( 0, reinterpret_cast<char*>(data->getDoubleArray()), this->getSize() );
	return data;
}

bool
kmb::Vector2ValueFileBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( 0 <= id && static_cast<unsigned long>(id) < this->dataSize && val->getType() == kmb::PhysicalValue::VECTOR2 ){
		double d[2] = { 0.0, 0.0 };
		d[0] = reinterpret_cast< kmb::Vector2Value* >(val)->getValue(0);
		d[1] = reinterpret_cast< kmb::Vector2Value* >(val)->getValue(1);
		setData( id, reinterpret_cast<const char*>(d) );
		delete val;
		return true;
	}
	return false;
}

bool
kmb::Vector2ValueFileBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && static_cast<unsigned long>(id) < this->dataSize ){
		setData( id, reinterpret_cast<const char*>(val) );
		return true;
	}
	return false;
}

bool
kmb::Vector2ValueFileBindings::setValue(kmb::idType id, double value,int index)
{
	return false;
}

kmb::PhysicalValue*
kmb::Vector2ValueFileBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::Vector2ValueFileBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id <= getMaxIndex() ){
		double d[2] = { 0.0, 0.0 };
		getData(id,reinterpret_cast<char*>(d));
		val[0] = d[0];
		val[1] = d[1];
		return true;
	}
	return false;
}

bool
kmb::Vector2ValueFileBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id <= getMaxIndex() ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::Vector2ValueFileBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::Vector2ValueFileBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	return false;
}

size_t
kmb::Vector2ValueFileBindings::getIdCount() const
{
	return getSize();
}

kmb::DataBindings::iterator
kmb::Vector2ValueFileBindings::begin(void)
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
kmb::Vector2ValueFileBindings::begin(void) const
{
	return kmb::DataBindings::const_iterator(NULL);
}

kmb::Vector2ValueFileBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::Vector2ValueFileBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::Vector2ValueFileBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::Vector2ValueFileBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::Vector2ValueFileBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::Vector2ValueFileBindings::_iterator::getValue(double *value) const
{
	double d[2] = {0.0,0.0};
	nodeValue->getData( index, reinterpret_cast<char*>(d) );
	value[0] = d[0];
	value[1] = d[1];
	return true;
}

kmb::Vector2ValueFileBindings::_iterator*
kmb::Vector2ValueFileBindings::_iterator::operator++(void)
{
	++index;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector2ValueFileBindings::_iterator*
kmb::Vector2ValueFileBindings::_iterator::operator++(int n)
{
	index++;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector2ValueFileBindings::_iterator*
kmb::Vector2ValueFileBindings::_iterator::clone(void)
{
	kmb::Vector2ValueFileBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::Vector2ValueFileBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}
