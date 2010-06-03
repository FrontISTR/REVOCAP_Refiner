/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : DataBindings                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbDataBindings.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

const kmb::DataBindings::iterator kmb::DataBindings::endIterator;
const kmb::DataBindings::const_iterator kmb::DataBindings::endConstIterator;

kmb::DataBindings::DataBindings(void)
: type(kmb::PhysicalValue::UNKNOWN)
, bMode(DataBindings::UNKNOWN)
, specType("")
, targetBodyId(kmb::Body::nullBodyId)
{
}


kmb::DataBindings*
kmb::DataBindings::createDataBindings
(bindingMode mode,kmb::PhysicalValue::valueType vType,std::string sType,kmb::bodyIdType bodyId)
{
	DataBindings* data = NULL;
	switch(mode){
		case NODEGROUP:
			data = new DataBindingsGroup<kmb::nodeIdType>(mode,vType);
			break;
		case ELEMENTGROUP:
			data = new DataBindingsGroup<kmb::elementIdType>(mode,vType);
			break;
		case FACEGROUP:
			data = new DataBindingsGroup<kmb::Face>(mode,vType);
			break;
		case BODYGROUP:
			data = new DataBindingsGroup<kmb::bodyIdType>(mode,vType);
			break;
		case NODEVARIABLE:
			data = new DataBindingsEach<kmb::nodeIdType>(mode,vType);
			break;
		case ELEMENTVARIABLE:
			data = new DataBindingsEach<kmb::elementIdType>(mode,vType);
			break;
		case FACEVARIABLE:
			data = new DataBindingsEach<kmb::Face>(mode,vType);
			break;
		case BODYVARIABLE:
			data = new DataBindingsEach<kmb::bodyIdType>(mode,vType);
			break;
		case GLOBAL:
			data = new DataBindingsGlobal(mode,vType);
			break;
		default:
			data = NULL;
			break;
	}
	if( data != NULL ){
		data->specType = sType;
		data->targetBodyId = bodyId;
	}
	return data;
}

bool
kmb::DataBindings::addId(kmb::idType id)
{
	return false;
}

bool
kmb::DataBindings::hasId(kmb::idType id) const
{
	return false;
}

bool
kmb::DataBindings::deleteId(kmb::idType id)
{
	return false;
}

void
kmb::DataBindings::clear(void)
{
}

bool
kmb::DataBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	return false;
}

bool
kmb::DataBindings::addId(kmb::Face id)
{
	return false;
}

bool
kmb::DataBindings::hasId(kmb::Face id) const
{
	return false;
}

bool
kmb::DataBindings::deleteId(kmb::Face id){
	return false;
}

bool
kmb::DataBindings::replaceId(kmb::Face old_id,kmb::Face new_id)
{
	return false;
}

bool
kmb::DataBindings::setPhysicalValue(kmb::PhysicalValue* val)
{
	return false;
}

bool
kmb::DataBindings::setPhysicalValue(kmb::idType id, kmb::PhysicalValue* val)
{
	return false;
}

bool
kmb::DataBindings::setPhysicalValue(kmb::Face f, kmb::PhysicalValue* val)
{
	return false;
}

bool
kmb::DataBindings::setPhysicalValue(kmb::idType id, double *value)
{
	return false;
}

bool
kmb::DataBindings::setPhysicalValue(kmb::Face f, double *value)
{
	return false;
}

bool
kmb::DataBindings::setPhysicalValue(kmb::idType id, long *value)
{
	return false;
}

bool
kmb::DataBindings::setPhysicalValue(kmb::Face f, long *value)
{
	return false;
}

bool
kmb::DataBindings::setValue(kmb::idType id, double value,int index)
{
	return false;
}

bool
kmb::DataBindings::setValue(kmb::Face f, double value,int index)
{
	return false;
}

bool
kmb::DataBindings::setValue(kmb::idType id, long value,int index)
{
	return false;
}

bool
kmb::DataBindings::setValue(kmb::Face f, long value,int index)
{
	return false;
}

kmb::PhysicalValue*
kmb::DataBindings::getPhysicalValue(void) const
{
	return NULL;
}

kmb::PhysicalValue*
kmb::DataBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

kmb::PhysicalValue*
kmb::DataBindings::getPhysicalValue(kmb::Face f) const
{
	return NULL;
}

bool
kmb::DataBindings::getPhysicalValue(kmb::idType id, double *value) const
{
	return false;
}

bool
kmb::DataBindings::getPhysicalValue(kmb::Face f, double *value) const
{
	return false;
}

bool
kmb::DataBindings::getPhysicalValue(kmb::idType id, long *value) const
{
	return false;
}

bool
kmb::DataBindings::getPhysicalValue(kmb::Face f, long *value) const
{
	return false;
}



kmb::DataBindings::iterator::iterator(const kmb::DataBindings::iterator& other)
: _iter(NULL)
{
	if( other._iter ){
		_iter = other._iter->clone();
	}
}

kmb::DataBindings::iterator::~iterator(void)
{
	if( _iter != NULL ){
		delete _iter;
		_iter = NULL;
	}
}

kmb::idType
kmb::DataBindings::iterator::getId(void) const
{
	if( _iter != NULL ){
		return _iter->getId();
	}else{
		return kmb::nullId;
	}
}

bool
kmb::DataBindings::iterator::getFace(kmb::Face &f) const
{
	if( _iter != NULL ){
		return _iter->getFace(f);
	}else{
		return false;
	}
}

kmb::PhysicalValue*
kmb::DataBindings::iterator::getValue(void) const
{
	if( _iter != NULL ){
		return _iter->getValue();
	}else{
		return NULL;
	}
}

bool
kmb::DataBindings::iterator::getValue(double *value)  const
{
	if( _iter != NULL ){
		return _iter->getValue(value);
	}else{
		return false;
	}
}

bool
kmb::DataBindings::iterator::getValue(long *value)  const
{
	if( _iter != NULL ){
		return _iter->getValue(value);
	}else{
		return false;
	}
}

kmb::DataBindings::iterator&
kmb::DataBindings::iterator::operator=(const kmb::DataBindings::iterator& other)
{
	if( _iter != NULL && _iter != other._iter )
	{
		delete _iter;
		_iter = NULL;
	}
	if( other._iter ){
		_iter = other._iter->clone();
	}
	return *this;
}

kmb::DataBindings::iterator&
kmb::DataBindings::iterator::operator++(void)
{
	if( _iter != NULL ){
		kmb::DataBindings::_iterator* retVal = ++(*_iter);
		if( retVal == NULL ){
			if( _iter ){
				delete _iter;
				_iter = NULL;
			}
		}
	}
	return *this;
}

kmb::DataBindings::iterator
kmb::DataBindings::iterator::operator++(int n)
{
	if( _iter != NULL ){
		kmb::DataBindings::_iterator* _itClone = _iter->clone();
		kmb::DataBindings::_iterator* retVal = (*_iter)++;
		if( retVal == NULL ){
			if( _iter ) delete _iter;
			_iter = NULL;
		}
		return kmb::DataBindings::iterator(_itClone);
	}else{
		return kmb::DataBindings::iterator(NULL);
	}
}

bool
kmb::DataBindings::iterator::operator==(const iterator &other) const
{
	return ( _iter == other._iter );
}

bool
kmb::DataBindings::iterator::operator!=(const iterator &other) const
{
	return ( _iter != other._iter );
}

kmb::DataBindings::const_iterator::const_iterator(const kmb::DataBindings::const_iterator& other)
: _iter(NULL)
{
	if( other._iter ){
		_iter = other._iter->clone();
	}
}

kmb::DataBindings::const_iterator::const_iterator(const kmb::DataBindings::iterator& other)
: _iter(NULL)
{
	if( other._iter ){
		_iter = other._iter->clone();
	}
}

kmb::DataBindings::const_iterator::~const_iterator(void)
{
	if( _iter != NULL ){
		delete _iter;
		_iter = NULL;
	}
}

const kmb::idType
kmb::DataBindings::const_iterator::getId(void) const
{
	if( _iter != NULL ){
		return _iter->getId();
	}else{
		return kmb::nullId;
	}
}

bool
kmb::DataBindings::const_iterator::getFace(kmb::Face &f) const
{
	if( _iter != NULL ){
		return _iter->getFace(f);
	}else{
		return false;
	}
}

const kmb::PhysicalValue*
kmb::DataBindings::const_iterator::getValue(void) const
{
	if( _iter != NULL ){
		return _iter->getValue();
	}else{
		return NULL;
	}
}

bool
kmb::DataBindings::const_iterator::getValue(double *value)  const
{
	if( _iter != NULL ){
		return _iter->getValue(value);
	}else{
		return false;
	}
}

bool
kmb::DataBindings::const_iterator::getValue(long *value)  const
{
	if( _iter != NULL ){
		return _iter->getValue(value);
	}else{
		return false;
	}
}

kmb::DataBindings::const_iterator&
kmb::DataBindings::const_iterator::operator=(const kmb::DataBindings::const_iterator& other)
{
	if( _iter != NULL && _iter != other._iter )
	{
		delete _iter;
		_iter = NULL;
	}
	if( other._iter ){
		_iter = other._iter->clone();
	}
	return *this;
}

kmb::DataBindings::const_iterator&
kmb::DataBindings::const_iterator::operator=(const kmb::DataBindings::iterator& other)
{
	if( _iter != NULL && _iter != other._iter )
	{
		delete _iter;
		_iter = NULL;
	}
	if( other._iter ){
		_iter = other._iter->clone();
	}
	return *this;
}

kmb::DataBindings::const_iterator&
kmb::DataBindings::const_iterator::operator++(void)
{
	if( _iter != NULL ){
		kmb::DataBindings::_iterator* retVal = ++(*_iter);
		if( retVal == NULL ){
			if( _iter ){
				delete _iter;
				_iter = NULL;
			}
		}
	}
	return *this;
}

kmb::DataBindings::const_iterator
kmb::DataBindings::const_iterator::operator++(int n)
{
	if( _iter != NULL ){
		kmb::DataBindings::_iterator* _itClone = _iter->clone();
		kmb::DataBindings::_iterator* retVal = (*_iter)++;
		if( retVal == NULL ){
			if( _iter ) delete _iter;
			_iter = NULL;
		}
		return kmb::DataBindings::const_iterator(_itClone);
	}else{
		return kmb::DataBindings::const_iterator(NULL);
	}
}

bool
kmb::DataBindings::const_iterator::operator==(const kmb::DataBindings::const_iterator &other) const
{
	return ( _iter == other._iter );
}

bool
kmb::DataBindings::const_iterator::operator!=(const kmb::DataBindings::const_iterator &other) const
{
	return ( _iter != other._iter );
}

#ifdef WIN32
#pragma warning(pop)
#endif

