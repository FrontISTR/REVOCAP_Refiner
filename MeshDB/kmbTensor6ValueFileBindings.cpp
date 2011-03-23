/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Tensor6ValueFileBindings                                #
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
#include "MeshDB/kmbTensor6ValueFileBindings.h"
#include "MeshDB/kmbTensor6ValueBindings.h"
#include "MeshDB/kmbDataBindings.h"
#include <iostream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char kmb::Tensor6ValueFileBindings::HEADER_STRING[80]
	= "RevocapMesh::Tensor6ValueFileBindings                                         ";

kmb::Tensor6ValueFileBindings::Tensor6ValueFileBindings(size_t count,const char* filename,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	this->type = kmb::PhysicalValue::TENSOR6;
	this->bMode = bmode;
	initialize( filename, kmb::Tensor6ValueFileBindings::HEADER_STRING, 6*sizeof(double), static_cast<unsigned long>(count) );
}

kmb::Tensor6ValueFileBindings::Tensor6ValueFileBindings(const char* filename,const kmb::Tensor6ValueBindings* data)
: kmb::DataBindings()
, kmb::ContainerOnDisk()
{
	if( data != NULL ){
		this->type = kmb::PhysicalValue::TENSOR6;
		this->bMode = data->getBindingMode();
		initialize( filename, kmb::Tensor6ValueFileBindings::HEADER_STRING, 6*sizeof(double), static_cast<unsigned long>(data->getIdCount()) );
		setData( 0, reinterpret_cast<const char*>(data->getDoubleArray()), static_cast<unsigned long>(data->getIdCount()) );
	}else{
		this->type = kmb::PhysicalValue::TENSOR6;
		this->bMode = kmb::DataBindings::NODEVARIABLE;
		initialize( filename, kmb::Tensor6ValueFileBindings::HEADER_STRING, 6*sizeof(double), 0 );
	}
}

kmb::Tensor6ValueFileBindings::~Tensor6ValueFileBindings(void)
{
}

void
kmb::Tensor6ValueFileBindings::clear(void)
{
}

kmb::Tensor6ValueBindings*
kmb::Tensor6ValueFileBindings::createTensor6ValueBindings(void)
{
	kmb::Tensor6ValueBindings* data = new kmb::Tensor6ValueBindings( this->getSize(), this->bMode );
	getData(0,reinterpret_cast<char*>(data->getDoubleArray()),this->getSize());
	return data;
}

bool
kmb::Tensor6ValueFileBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( 0 <= id && static_cast<unsigned long>(id) < this->dataSize && val->getType() == kmb::PhysicalValue::TENSOR6 ){
		double d[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		d[0] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(0);
		d[1] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(1);
		d[2] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(2);
		d[3] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(3);
		d[4] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(4);
		d[5] = reinterpret_cast< kmb::Tensor6Value* >(val)->getValue(5);
		setData( id, reinterpret_cast<const char*>(d) );
		delete val;
		return true;
	}
	return false;
}

bool
kmb::Tensor6ValueFileBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id && static_cast<unsigned long>(id) < this->dataSize ){
		setData( id, reinterpret_cast<const char*>(val) );
		return true;
	}
	return false;
}

bool
kmb::Tensor6ValueFileBindings::setValue(kmb::idType id, double value,int index)
{
	return false;
}

kmb::PhysicalValue*
kmb::Tensor6ValueFileBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

bool
kmb::Tensor6ValueFileBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id <= getMaxIndex() ){
		double d[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		getData(id,reinterpret_cast<char*>(d));
		val[0] = d[0];
		val[1] = d[1];
		val[2] = d[2];
		val[3] = d[3];
		val[4] = d[4];
		val[5] = d[5];
		return true;
	}
	return false;
}

bool
kmb::Tensor6ValueFileBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id <= getMaxIndex() ){
		return true;
	}else{
		return false;
	}
}

bool
kmb::Tensor6ValueFileBindings::deleteId(kmb::idType id)
{
	return false;
}

bool
kmb::Tensor6ValueFileBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	return false;
}

size_t
kmb::Tensor6ValueFileBindings::getIdCount() const
{
	return getSize();
}

kmb::DataBindings::iterator
kmb::Tensor6ValueFileBindings::begin(void)
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
kmb::Tensor6ValueFileBindings::begin(void) const
{
	return kmb::DataBindings::const_iterator(NULL);
}

kmb::Tensor6ValueFileBindings::_iterator::_iterator(void)
: index( 0 )
, nodeValue( NULL )
{
}

kmb::Tensor6ValueFileBindings::_iterator::~_iterator(void)
{
}

kmb::idType
kmb::Tensor6ValueFileBindings::_iterator::getId(void) const
{
	return index;
}

bool
kmb::Tensor6ValueFileBindings::_iterator::getFace(kmb::Face &f) const
{
	return false;
}

kmb::PhysicalValue*
kmb::Tensor6ValueFileBindings::_iterator::getValue(void) const
{
	return NULL;
}

bool
kmb::Tensor6ValueFileBindings::_iterator::getValue(double *value) const
{
	double d[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
	nodeValue->getData( index, reinterpret_cast<char*>(d) );
	value[0] = d[0];
	value[1] = d[1];
	value[2] = d[2];
	value[3] = d[3];
	value[4] = d[4];
	value[5] = d[5];
	return true;
}

kmb::Tensor6ValueFileBindings::_iterator*
kmb::Tensor6ValueFileBindings::_iterator::operator++(void)
{
	++index;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Tensor6ValueFileBindings::_iterator*
kmb::Tensor6ValueFileBindings::_iterator::operator++(int n)
{
	index++;
	if( index <= nodeValue->getMaxIndex() ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Tensor6ValueFileBindings::_iterator*
kmb::Tensor6ValueFileBindings::_iterator::clone(void)
{
	kmb::Tensor6ValueFileBindings::_iterator* _itClone = NULL;
	_itClone = new kmb::Tensor6ValueFileBindings::_iterator();
	if( _itClone ){
		_itClone->nodeValue = this->nodeValue;
		_itClone->index = this->index;
	}
	return _itClone;
}
