/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Vector2WithIntBindings                                  #
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
#include "MeshDB/kmbPhysicalValue.h"

#include <map>

#include <cstdio>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

namespace kmb{

template <typename T>
class Vector2WithIntBindings : public kmb::DataBindings
{
public:
	typedef struct{
		double u;
		double v;
		long l;
	} valueType;
private:
	std::multimap<T,valueType> mapper;
public:
	Vector2WithIntBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable)
	{
		this->type = kmb::PhysicalValue::Vector2withInt;
		this->bMode = bmode;
	};
	virtual ~Vector2WithIntBindings(void){
	};
	virtual const char* getContainerType(void) const{
		return "Vector2ValueBindings";
	};
	virtual void clear(void){
		mapper.clear();
	};
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val){
		if( val && val->getType() == kmb::PhysicalValue::Vector2withInt ){
			kmb::Vector2WithInt* vec = reinterpret_cast< kmb::Vector2WithInt* >(val);
			bool res = setValue( static_cast<T>(id), vec->getValue(0), vec->getValue(1), vec->getIValue() );
			delete val;
			return res;
		}
		return false;
	};
	bool setValue(T t,double u,double v,long l){
		if( !hasId(t,l) ){
			valueType v2i={u,v,l};
			mapper.insert( std::pair<T,valueType>( t, v2i ) );
			return true;
		}
		return false;
	}

	virtual bool getPhysicalValue(kmb::idType id, double *val) const{
		typename std::multimap<T,valueType>::const_iterator i = mapper.find( static_cast<T>(id) );
		if( val && i != mapper.end() ){
			val[0] = i->second.u;
			val[1] = i->second.v;
			return true;
		}
		return false;
	}
	virtual bool getPhysicalValue(kmb::idType id, long *l) const{
		typename std::multimap<T,valueType>::const_iterator i = mapper.find( static_cast<T>(id) );
		if( l && i != mapper.end() ){
			*l = i->second.l;
			return true;
		}
		return false;
	}
	virtual bool hasId(kmb::idType id) const{
		typename std::multimap<T,valueType>::const_iterator i = mapper.find( static_cast<T>(id) );
		return ( i != mapper.end() );
	}
	virtual size_t getIdCount() const{
		return mapper.size();
	}

	bool hasId(T t,long k) const{
		if( mapper.count(t) == 0 ){
			return false;
		}else{
			std::pair< typename std::multimap<T,valueType>::const_iterator,
				typename std::multimap<T,valueType>::const_iterator > range = this->mapper.equal_range(t);
			typename std::multimap< T, valueType >::const_iterator dIter = range.first;
			while( dIter != range.second ){
				if( dIter->second.l == k ){
					return true;
				}
				++dIter;
			}
		}
		return false;
	}



	size_t replaceIds( const std::map<T,T>& iMapper ){
		size_t count = 0;
		typename std::multimap<T,valueType> temp;
		temp.swap( mapper );
		mapper.clear();
		typename std::multimap<T,valueType>::iterator tIter = temp.begin();
		while( tIter != temp.end() ){
			typename std::map<T,T>::const_iterator iter = iMapper.find( tIter->first );
			if( iter != iMapper.end() ){
				++count;
				mapper.insert( std::pair<T,valueType>( iter->second, tIter->second ) );


			}
			++tIter;
		}
		return count;
	}
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class Vector2WithIntBindings<T>;
	private:
		typename std::multimap<T,valueType>::const_iterator dataIter;
		typename std::multimap<T,valueType>::const_iterator endIter;
	public:
		_iterator(void){};
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{
			return static_cast<kmb::idType>( dataIter->first );
		};
		virtual bool getValue(double *value) const{
			value[0] = dataIter->second.u;
			value[1] = dataIter->second.v;
			return true;
		}
		virtual bool getValue(long *value) const{
			*value = dataIter->second.l;
			return true;
		}
		virtual _iterator* operator++(void){
			++dataIter;
			if( dataIter != endIter ){
				return this;
			}else{
				return NULL;
			}
		}
		virtual _iterator* operator++(int n){
			++dataIter;
			if( dataIter != endIter ){
				return this;
			}else{
				return NULL;
			}
		}
		virtual _iterator* clone(void){
			typename kmb::Vector2WithIntBindings<T>::_iterator* _it = NULL;
			_it = new typename kmb::Vector2WithIntBindings<T>::_iterator();
			_it->dataIter = this->dataIter;
			_it->endIter = this->endIter;
			return _it;
		}
	};
	virtual iterator begin(void){
		if( this->mapper.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIter = this->mapper.begin();
		_it->endIter = this->mapper.end();
		return kmb::DataBindings::iterator(_it);
	}
	virtual const_iterator begin(void) const{
		if( this->mapper.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIter = this->mapper.begin();
		_it->endIter = this->mapper.end();
		return kmb::DataBindings::const_iterator(_it);
	}


	bool isCommonIntval(T t0,T t1,long &index,double u0[2],double u1[2]) const{
		std::pair< typename std::multimap<T,valueType>::const_iterator,
			typename std::multimap<T,valueType>::const_iterator > range0 = this->mapper.equal_range(t0);
		std::pair< typename std::multimap<T,valueType>::const_iterator,
			typename std::multimap<T,valueType>::const_iterator > range1 = this->mapper.equal_range(t1);
		for( typename std::multimap<T,valueType>::const_iterator iter0 = range0.first; iter0 != range0.second; ++iter0 ){
			for( typename std::multimap<T,valueType>::const_iterator iter1 = range1.first; iter1 != range1.second; ++iter1 ){
				if( iter0->second.l == iter1->second.l ){
					u0[0] = iter0->second.u;
					u0[1] = iter0->second.v;
					u1[0] = iter1->second.u;
					u1[1] = iter1->second.v;
					index = iter0->second.l;
					return true;
				}
			}
		}
		return false;
	}

	int isCommonIntval(T t0,T t1,std::vector< valueType > &v0, std::vector< valueType > &v1) const{
		int count = 0;
		std::pair< typename std::multimap<T,valueType>::const_iterator,
			typename std::multimap<T,valueType>::const_iterator > range0 = this->mapper.equal_range(t0);
		std::pair< typename std::multimap<T,valueType>::const_iterator,
			typename std::multimap<T,valueType>::const_iterator > range1 = this->mapper.equal_range(t1);
		for( typename std::multimap<T,valueType>::const_iterator iter0 = range0.first; iter0 != range0.second; ++iter0 ){
			for( typename std::multimap<T,valueType>::const_iterator iter1 = range1.first; iter1 != range1.second; ++iter1 ){
				if( iter0->second.l == iter1->second.l ){
					v0.push_back( iter0->second );
					v1.push_back( iter1->second );
					++count;
				}
			}
		}
		return count;
	}

	size_t countId(T t) const{
		return this->mapper.count(t);
	}

	bool getValues(T t,double &u,double &v,long &l,size_t index=0) const{
		std::pair< typename std::multimap<T,valueType>::const_iterator,
			typename std::multimap<T,valueType>::const_iterator > range = this->mapper.equal_range(t);
		typename std::multimap<T,valueType>::const_iterator iter = range.first;
		if( iter == range.second ){
			return false;
		}
		for(size_t i=0;i<index;++i){
			++iter;
			if( iter == range.second ){
				return false;
			}
		}
		u = iter->second.u;
		v = iter->second.v;
		l = iter->second.l;
		return true;
	}
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
