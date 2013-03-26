/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : DataBindings                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
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

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const kmb::DataBindings::iterator kmb::DataBindings::endIterator;
const kmb::DataBindings::const_iterator kmb::DataBindings::endConstIterator;

kmb::DataBindings::DataBindings(void)
: type(kmb::PhysicalValue::Unknown)
, bMode(DataBindings::Unknown)
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
		case NodeGroup:
			data = new DataBindingsGroup<kmb::nodeIdType>(mode,vType);
			break;
		case ElementGroup:
			data = new DataBindingsGroup<kmb::elementIdType>(mode,vType);
			break;
		case FaceGroup:
			data = new DataBindingsGroup<kmb::Face>(mode,vType);
			break;
		case BodyGroup:
			data = new DataBindingsGroup<kmb::bodyIdType>(mode,vType);
			break;
		case NodeVariable:
			switch( vType ){
			case kmb::PhysicalValue::Scalar:
				data = new DataVariable<kmb::nodeIdType,kmb::ScalarValue>(mode);
				break;
			case kmb::PhysicalValue::Vector2:
				data = new DataVariable<kmb::nodeIdType,kmb::Vector2Value>(mode);
				break;
			case kmb::PhysicalValue::Vector3:
				data = new DataVariable<kmb::nodeIdType,kmb::Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Vector4:
				data = new DataVariable<kmb::nodeIdType,kmb::Vector4Value>(mode);
				break;
			case kmb::PhysicalValue::Tensor6:
				data = new DataVariable<kmb::nodeIdType,kmb::Tensor6Value>(mode);
				break;
			case kmb::PhysicalValue::Point3Vector3:
				data = new DataVariable<kmb::nodeIdType,kmb::Point3Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Integer:
				data = new DataVariable<kmb::nodeIdType,kmb::IntegerValue>(mode);
				break;
			case kmb::PhysicalValue::Array:
				data = new DataVariable<kmb::nodeIdType,kmb::ArrayValue>(mode);
				break;
			case kmb::PhysicalValue::Hash:
				data = new DataVariable<kmb::nodeIdType,kmb::HashValue>(mode);
				break;
			case kmb::PhysicalValue::IntHash:
				data = new DataVariable<kmb::nodeIdType,kmb::IntHashValue>(mode);
				break;
			case kmb::PhysicalValue::String:
				data = new DataVariable<kmb::nodeIdType,kmb::TextValue>(mode);
				break;
			case kmb::PhysicalValue::Boolean:
				data = new DataVariable<kmb::nodeIdType,kmb::BooleanValue>(mode);
				break;
			default:
				break;
			}
			break;
		case ElementVariable:
			switch( vType ){
			case kmb::PhysicalValue::Scalar:
				data = new DataVariable<kmb::elementIdType,kmb::ScalarValue>(mode);
				break;
			case kmb::PhysicalValue::Vector2:
				data = new DataVariable<kmb::elementIdType,kmb::Vector2Value>(mode);
				break;
			case kmb::PhysicalValue::Vector3:
				data = new DataVariable<kmb::elementIdType,kmb::Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Vector4:
				data = new DataVariable<kmb::elementIdType,kmb::Vector4Value>(mode);
				break;
			case kmb::PhysicalValue::Tensor6:
				data = new DataVariable<kmb::elementIdType,kmb::Tensor6Value>(mode);
				break;
			case kmb::PhysicalValue::Point3Vector3:
				data = new DataVariable<kmb::elementIdType,kmb::Point3Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Integer:
				data = new DataVariable<kmb::elementIdType,kmb::IntegerValue>(mode);
				break;
			case kmb::PhysicalValue::Array:
				data = new DataVariable<kmb::elementIdType,kmb::ArrayValue>(mode);
				break;
			case kmb::PhysicalValue::Hash:
				data = new DataVariable<kmb::elementIdType,kmb::HashValue>(mode);
				break;
			case kmb::PhysicalValue::IntHash:
				data = new DataVariable<kmb::elementIdType,kmb::IntHashValue>(mode);
				break;
			case kmb::PhysicalValue::String:
				data = new DataVariable<kmb::elementIdType,kmb::TextValue>(mode);
				break;
			case kmb::PhysicalValue::Boolean:
				data = new DataVariable<kmb::elementIdType,kmb::BooleanValue>(mode);
				break;
			default:
				break;
			}
			break;
		case BodyVariable:
			switch( vType ){
			case kmb::PhysicalValue::Scalar:
				data = new DataVariable<kmb::bodyIdType,kmb::ScalarValue>(mode);
				break;
			case kmb::PhysicalValue::Vector2:
				data = new DataVariable<kmb::bodyIdType,kmb::Vector2Value>(mode);
				break;
			case kmb::PhysicalValue::Vector3:
				data = new DataVariable<kmb::bodyIdType,kmb::Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Vector4:
				data = new DataVariable<kmb::bodyIdType,kmb::Vector4Value>(mode);
				break;
			case kmb::PhysicalValue::Tensor6:
				data = new DataVariable<kmb::bodyIdType,kmb::Tensor6Value>(mode);
				break;
			case kmb::PhysicalValue::Point3Vector3:
				data = new DataVariable<kmb::bodyIdType,kmb::Point3Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Integer:
				data = new DataVariable<kmb::bodyIdType,kmb::IntegerValue>(mode);
				break;
			case kmb::PhysicalValue::Array:
				data = new DataVariable<kmb::bodyIdType,kmb::ArrayValue>(mode);
				break;
			case kmb::PhysicalValue::Hash:
				data = new DataVariable<kmb::bodyIdType,kmb::HashValue>(mode);
				break;
			case kmb::PhysicalValue::IntHash:
				data = new DataVariable<kmb::bodyIdType,kmb::IntHashValue>(mode);
				break;
			case kmb::PhysicalValue::String:
				data = new DataVariable<kmb::bodyIdType,kmb::TextValue>(mode);
				break;
			case kmb::PhysicalValue::Boolean:
				data = new DataVariable<kmb::bodyIdType,kmb::BooleanValue>(mode);
				break;
			default:
				break;
			}
			break;
		case FaceVariable:
			switch( vType ){
			case kmb::PhysicalValue::Scalar:
				data = new DataVariable<kmb::Face,kmb::ScalarValue>(mode);
				break;
			case kmb::PhysicalValue::Vector2:
				data = new DataVariable<kmb::Face,kmb::Vector2Value>(mode);
				break;
			case kmb::PhysicalValue::Vector3:
				data = new DataVariable<kmb::Face,kmb::Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Vector4:
				data = new DataVariable<kmb::Face,kmb::Vector4Value>(mode);
				break;
			case kmb::PhysicalValue::Tensor6:
				data = new DataVariable<kmb::Face,kmb::Tensor6Value>(mode);
				break;
			case kmb::PhysicalValue::Point3Vector3:
				data = new DataVariable<kmb::Face,kmb::Point3Vector3Value>(mode);
				break;
			case kmb::PhysicalValue::Integer:
				data = new DataVariable<kmb::Face,kmb::IntegerValue>(mode);
				break;
			case kmb::PhysicalValue::Array:
				data = new DataVariable<kmb::Face,kmb::ArrayValue>(mode);
				break;
			case kmb::PhysicalValue::Hash:
				data = new DataVariable<kmb::Face,kmb::HashValue>(mode);
				break;
			case kmb::PhysicalValue::IntHash:
				data = new DataVariable<kmb::Face,kmb::IntHashValue>(mode);
				break;
			case kmb::PhysicalValue::String:
				data = new DataVariable<kmb::Face,kmb::TextValue>(mode);
				break;
			case kmb::PhysicalValue::Boolean:
				data = new DataVariable<kmb::Face,kmb::BooleanValue>(mode);
				break;
			default:
				break;
			}
			break;
		case Global:
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

int
kmb::DataBindings::replaceNodeId(std::map<kmb::nodeIdType,kmb::nodeIdType> &idmap)
{
	return 0;
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

bool
kmb::DataBindings::scalar(double r)
{
	int len = this->getDimension();
	if( len <= 0 ){
		return false;
	}
	double* value = new double[len];
	kmb::DataBindings::iterator dIter = this->begin();
	while( !dIter.isFinished() ){
		if( dIter.getValue(value) ){
			for(int i=0;i<len;++i){
				value[i] *= r;
			}
			dIter.setValue(value);
		}
		++dIter;
	}
	delete[] value;
	return true;
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
kmb::DataBindings::iterator::getValue(double *value) const
{
	if( _iter != NULL ){
		return _iter->getValue(value);
	}else{
		return false;
	}
}

bool
kmb::DataBindings::iterator::getValue(long *value) const
{
	if( _iter != NULL ){
		return _iter->getValue(value);
	}else{
		return false;
	}
}

bool
kmb::DataBindings::iterator::setValue(double *value)
{
	if( _iter != NULL ){
		return _iter->setValue(value);
	}else{
		return false;
	}
}

bool
kmb::DataBindings::iterator::setValue(long *value)
{
	if( _iter != NULL ){
		return _iter->setValue(value);
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

kmb::idType
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

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
