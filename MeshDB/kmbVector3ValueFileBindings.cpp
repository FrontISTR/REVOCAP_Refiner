/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Vector3ValueFileBindings                                #
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

#include "MeshDB/kmbVector3ValueFileBindings.h"
#include "MeshDB/kmbVector3ValueBindings.h"
#include "MeshDB/kmbDataBindings.h"
#include <iostream>

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

const char kmb::Vector3ValueFileBindings::HEADER_STRING[80]
	= "RevocapMesh::Vector3ValueFileBindings                                         ";

kmb::Vector3ValueFileBindings::Vector3ValueFileBindings(size_t count,const char* filename,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	this->type = kmb::PhysicalValue::VECTOR3;
	this->bMode = bmode;
	initialize( filename, kmb::Vector3ValueFileBindings::HEADER_STRING, 3*sizeof(double), static_cast<unsigned long>(count) );
}

kmb::Vector3ValueFileBindings::Vector3ValueFileBindings(const char* filename,const kmb::Vector3ValueBindings* data)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	if( data != NULL ){
		this->type = kmb::PhysicalValue::VECTOR3;
		this->bMode = data->getBindingMode();
		initialize( filename, kmb::Vector3ValueFileBindings::HEADER_STRING, 3*sizeof(double), static_cast<unsigned long>(data->getIdCount()) );
		setData( 0, reinterpret_cast<const char*>(data->getDoubleArray()), static_cast<unsigned long>(data->getIdCount()) );
	}else{
		this->type = kmb::PhysicalValue::VECTOR3;
		this->bMode = kmb::DataBindings::NODEVARIABLE;
		initialize( filename, kmb::Vector3ValueFileBindings::HEADER_STRING, 3*sizeof(double), 0 );
	}
}

kmb::Vector3ValueFileBindings::~Vector3ValueFileBindings(void)
{
}

void
kmb::Vector3ValueFileBindings::clear(void)
{
}

kmb::Vector3ValueBindings*
kmb::Vector3ValueFileBindings::createVector3ValueBindings(void)
{
	kmb::Vector3ValueBindings* data = new kmb::Vector3ValueBindings( this->getSize(), this->bMode );
	getData(0,reinterpret_cast<char*>(data->getDoubleArray()),this->getSize());
	return data;
}

bool
kmb::Vector3ValueFileBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( 0 <= id && static_cast<unsigned long>(id) < this->dataSize && val->getType() == kmb::PhysicalValue::VECTOR3 ){
		double d[3] = { 0.0, 0.0, 0.0 };
		d[0] = reinterpret_cast< kmb::Vector3Value* >(val)->getValue(0);
		d[1] = reinterpret_cast< kmb::Vector3Value* >(val)->getValue(1);
		d[2] = reinterpret_cast< kmb::Vector3Value* >(val)->getValue(2);
		setData( id, reinterpret_cast<const char*>(d) );
		delete val;
		return true;
	}
	return false;
}

bool
kmb::Vector3ValueFileBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && static_cast<unsigned long>(id) < this->dataSize ){
		setData( id, reinterpret_cast<const char*>(val) );
		return true;
	}
	return false;
}

bool
kmb::Vector3ValueFileBindings::setValue(kmb::idType id, double value,int index)
{
	return false;
}

kmb::PhysicalValue*
kmb::Vector3ValueFileBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::Vector3ValueFileBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id <= getMaxIndex() ){
		double d[3] = { 0.0, 0.0, 0.0 };
		getData(id,reinterpret_cast<char*>(d));
		val[0] = d[0];
		val[1] = d[1];
		val[2] = d[2];
		return true;
	}
	return false;
}

bool
kmb::Vector3ValueFileBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id <= getMaxIndex() ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::Vector3ValueFileBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::Vector3ValueFileBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	return false;
}

size_t
kmb::Vector3ValueFileBindings::getIdCount() const
{
	return getSize();
}

kmb::DataBindings::iterator
kmb::Vector3ValueFileBindings::begin(void)
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
kmb::Vector3ValueFileBindings::begin(void) const
{
	return kmb::DataBindings::const_iterator(NULL);
}

kmb::Vector3ValueFileBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::Vector3ValueFileBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::Vector3ValueFileBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::Vector3ValueFileBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::Vector3ValueFileBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::Vector3ValueFileBindings::_iterator::getValue(double *value) const
{
	double d[3] = {0.0,0.0,0.0};
	nodeValue->getData( index, reinterpret_cast<char*>(d) );
	value[0] = d[0];
	value[1] = d[1];
	value[2] = d[2];
	return true;
}

kmb::Vector3ValueFileBindings::_iterator*
kmb::Vector3ValueFileBindings::_iterator::operator++(void)
{
	++index;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector3ValueFileBindings::_iterator*
kmb::Vector3ValueFileBindings::_iterator::operator++(int n)
{
	index++;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Vector3ValueFileBindings::_iterator*
kmb::Vector3ValueFileBindings::_iterator::clone(void)
{
	kmb::Vector3ValueFileBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::Vector3ValueFileBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}
