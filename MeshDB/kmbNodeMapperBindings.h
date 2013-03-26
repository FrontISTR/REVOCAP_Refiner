/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeMapperBindings                                      #
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
#pragma once
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbTypes.h"
#include <map>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

namespace kmb{


template<typename T>
class NodeMapperBindings : public DataBindings
{
private:
	typename std::map<kmb::nodeIdType,T> mapper;
public:
	NodeMapperBindings(kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable)
		: kmb::DataBindings()
	{
		this->type = kmb::PhysicalValue::Integer;
		this->bMode = bmode;
	}
	virtual ~NodeMapperBindings(void)
	{
		this->clear();
	}
	virtual const char* getContainerType(void) const{
		return "NodeMapperBindings";
	};
	virtual void clear(void)
	{
		this->mapper.clear();
	}
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
	{
		if( val && val->getType() == kmb::PhysicalValue::Integer ){
			int v = reinterpret_cast<kmb::IntegerValue*>(val)->getValue();
			mapper.insert( std::pair< kmb::nodeIdType, T >
				( static_cast<kmb::nodeIdType>(id), static_cast<T>(v) ) );
			delete val;
			return true;
		}
		return false;
	}
	virtual bool setPhysicalValue(kmb::idType id, long *val)
	{
		if( val ){
			mapper.insert( std::pair< kmb::nodeIdType, T >
				( static_cast<kmb::nodeIdType>(id), static_cast<T>(val[0]) ) );
			return true;
		}
		return false;
	}
	virtual bool getPhysicalValue(kmb::idType id, long *val) const
	{
		typename std::map< kmb::nodeIdType, T>::const_iterator nIter = mapper.find( static_cast<kmb::nodeIdType>(id) );
		if( nIter == mapper.end() ){
			return false;
		}
		val[0] = static_cast<long>(nIter->second);
		return true;
	}
	virtual bool hasId(kmb::idType id) const
	{
		return (mapper.find( static_cast<kmb::nodeIdType>(id) ) != mapper.end() );
	}
	virtual bool deleteId(kmb::idType id)
	{
		typename std::map< kmb::nodeIdType, T>::iterator nIter = mapper.find( static_cast<kmb::nodeIdType>(id) );
		if( nIter == mapper.end() ){
			return false;
		}
		mapper.erase( nIter );
		return true;
	}
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id)
	{
		typename std::map< kmb::nodeIdType, T>::iterator oldIter = mapper.find( static_cast<kmb::nodeIdType>(old_id) );
		typename std::map< kmb::nodeIdType, T>::iterator newIter = mapper.find( static_cast<kmb::nodeIdType>(new_id) );
		if( oldIter != mapper.end() &&
			newIter == mapper.end() )
		{
			T v = oldIter->second;
			mapper.erase(oldIter);
			mapper.insert( std::pair<kmb::nodeIdType,T>(static_cast<kmb::nodeIdType>(new_id), v) );
			return true;
		}else{
			return false;
		}
	}
	virtual size_t getIdCount() const
	{
		return mapper.size();
	}
	const std::map<kmb::nodeIdType,T>& getNodeMapper(void) const{
		return mapper;
	}
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class NodeMapperBindings<T>;
		_iterator(void){};
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{
			return static_cast< kmb::idType >( dataIterator->first );
		}
		virtual bool getValue(long *value) const{
			value[0] = static_cast<long>( dataIterator->second );
			return true;
		}
		virtual _iterator* operator++(void)
		{
			++dataIterator;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		}
		virtual _iterator* operator++(int n)
		{
			dataIterator++;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		}
		virtual _iterator* clone(void)
		{
			typename kmb::NodeMapperBindings<T>::_iterator* _itClone = NULL;
			_itClone = new typename kmb::NodeMapperBindings<T>::_iterator();
			if( _itClone ){
				_itClone->dataIterator = this->dataIterator;
				_itClone->endIterator = this->endIterator;
			}
			return _itClone;
		}
	private:
		typename std::map<kmb::nodeIdType,T>::const_iterator dataIterator;
		typename std::map<kmb::nodeIdType,T>::const_iterator endIterator;
	};
	virtual iterator begin(void)
	{
		if( mapper.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		typename kmb::NodeMapperBindings<T>::_iterator* _it = NULL;
		_it = new typename kmb::NodeMapperBindings<T>::_iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::iterator(_it);
	}

	virtual const_iterator begin(void) const
	{
		if( mapper.size() == 0 ){
			return kmb::DataBindings::const_iterator(NULL);
		}
		typename kmb::NodeMapperBindings<T>::_iterator* _it = NULL;
		_it = new typename kmb::NodeMapperBindings<T>::_iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::const_iterator(_it);
	}
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
