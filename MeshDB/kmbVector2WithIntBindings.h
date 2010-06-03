/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Vector2WithIntBindings                                  #
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






#pragma once
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbPhysicalValue.h"

#include <map>

#include <cstdio>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

namespace kmb{

template <typename T>
class Vector2WithIntBindings : public kmb::DataBindings
{
private:
	typedef struct{
		double u;
		double v;
		long l;
	} vec2int;
	std::multimap<T,vec2int> mapper;
public:
	Vector2WithIntBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NODEVARIABLE)
	{
		this->type = kmb::PhysicalValue::VECTOR2WITHINT;
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
		if( val && val->getType() == kmb::PhysicalValue::VECTOR2WITHINT ){
			kmb::Vector2WithInt* vec = reinterpret_cast< kmb::Vector2WithInt* >(val);
			bool res = setValue( static_cast<T>(id), vec->getValue(0), vec->getValue(1), vec->getIValue() );
			delete val;
			return res;
		}
		return false;
	};
	bool setValue(T t,double u,double v,long l){
		if( !hasId(t,l) ){
			vec2int v2i={u,v,l};
			mapper.insert( std::pair<T,vec2int>( t, v2i ) );
			return true;
		}
		return false;
	}

	virtual bool getPhysicalValue(kmb::idType id, double *val) const{
		typename std::multimap<T,vec2int>::const_iterator i = mapper.find( static_cast<T>(id) );
		if( val && i != mapper.end() ){
			val[0] = i->second.u;
			val[1] = i->second.v;
			return true;
		}
		return false;
	}
	virtual bool getPhysicalValue(kmb::idType id, long *l) const{
		typename std::multimap<T,vec2int>::const_iterator i = mapper.find( static_cast<T>(id) );
		if( l && i != mapper.end() ){
			*l = i->second.l;
			return true;
		}
		return false;
	}
	virtual bool hasId(kmb::idType id) const{
		typename std::multimap<T,vec2int>::const_iterator i = mapper.find( static_cast<T>(id) );
		return ( i != mapper.end() );
	}
	virtual size_t getIdCount() const{
		return mapper.size();
	}

	bool hasId(T t,long k) const{
		if( mapper.count(t) == 0 ){
			return false;
		}else{
			typename std::multimap< T, vec2int >::const_iterator dataIter = mapper.lower_bound(t);
			typename std::multimap< T, vec2int >::const_iterator endIter = mapper.upper_bound(t);
			while( dataIter != endIter ){
				if( dataIter->second.l == k ){
					return true;
				}
				++dataIter;
			}
		}
		return false;
	}
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class Vector2WithIntBindings<T>;
	private:
		typename std::multimap<T,vec2int>::const_iterator dataIter;
		typename std::multimap<T,vec2int>::const_iterator endIter;
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
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIter = this->mapper.begin();
		_it->endIter = this->mapper.end();
		return kmb::DataBindings::iterator(_it);
	}
	virtual const_iterator begin(void) const{
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIter = this->mapper.begin();
		_it->endIter = this->mapper.end();
		return kmb::DataBindings::const_iterator(_it);
	}
};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
