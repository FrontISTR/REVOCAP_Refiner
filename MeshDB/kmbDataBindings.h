/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : DataBindings                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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
#pragma once

#include "MeshDB/kmbPhysicalValue.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbFace.h"
#include "Geometry/kmb_BoundingBox.h"

#include <set>
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

class MeshDB;

class DataBindings
{
public:
	enum bindingMode{
		NODEGROUP,
		ELEMENTGROUP,
		FACEGROUP,
		BODYGROUP,
		NODEVARIABLE,
		ELEMENTVARIABLE,
		FACEVARIABLE,
		BODYVARIABLE,
		GLOBAL,
		UNDEFINED,
		UNKNOWN
	};
	static bindingMode string2bindingMode(std::string str){
		if( str == "NODEGROUP" )			return DataBindings::NODEGROUP;
		else if( str == "ELEMENTGROUP" )	return DataBindings::ELEMENTGROUP;
		else if( str == "FACEGROUP" )		return DataBindings::FACEGROUP;
		else if( str == "BODYGROUP" )		return DataBindings::BODYGROUP;
		else if( str == "NODEVARIABLE" )	return DataBindings::NODEVARIABLE;
		else if( str == "ELEMENTVARIABLE" )	return DataBindings::ELEMENTVARIABLE;
		else if( str == "FACEVARIABLE" )	return DataBindings::FACEVARIABLE;
		else if( str == "BODYVARIABLE" )	return DataBindings::BODYVARIABLE;
		else if( str == "GLOBAL" )			return DataBindings::GLOBAL;
		else if( str == "UNDEFINED" )		return DataBindings::UNDEFINED;
		else								return DataBindings::UNKNOWN;
	};
	static std::string bindingMode2string(bindingMode mode)
	{
		switch(mode){
			case DataBindings::NODEGROUP:		return std::string("NODEGROUP");
			case DataBindings::ELEMENTGROUP:	return std::string("ELEMENTGROUP");
			case DataBindings::FACEGROUP:		return std::string("FACEGROUP");
			case DataBindings::BODYGROUP:		return std::string("BODYGROUP");
			case DataBindings::NODEVARIABLE:	return std::string("NODEVARIABLE");
			case DataBindings::ELEMENTVARIABLE:	return std::string("ELEMENTVARIABLE");
			case DataBindings::FACEVARIABLE:	return std::string("FACEVARIABLE");
			case DataBindings::BODYVARIABLE:	return std::string("BODYVARIABLE");
			case DataBindings::GLOBAL:			return std::string("GLOBAL");
			case DataBindings::UNDEFINED:		return std::string("UNDEFINED");
			default:							return std::string("UNKNOWN");
		}
	};
	static DataBindings* createDataBindings(bindingMode mode,kmb::PhysicalValue::valueType vType,std::string sType,kmb::bodyIdType bodyId=kmb::Body::nullBodyId);
protected:
	DataBindings(void);
public:
	virtual ~DataBindings(void){}
	virtual const char* getContainerType(void) const = 0;

protected:
	PhysicalValue::valueType type;
public:
	PhysicalValue::valueType getValueType(void) const { return type; }
	virtual int getDimension(void) const { return kmb::PhysicalValue::getDimension(type); };

protected:
	bindingMode bMode;
public:
	bindingMode getBindingMode(void) const { return bMode; };

protected:
	std::string specType;
public:
	std::string getSpecType(void) const { return specType; };
	void setSpecType(std::string sname){ specType = sname; };

protected:
	kmb::bodyIdType targetBodyId;
public:
	kmb::bodyIdType getTargetBodyId(void) const { return targetBodyId; };
	void setTargetBodyId(kmb::bodyIdType bodyId){ targetBodyId = bodyId; };

public:

	virtual void clear(void);

	virtual size_t getIdCount(void) const = 0;

	virtual bool addId(kmb::idType id);
	virtual bool addId(kmb::Face id);

	virtual bool hasId(kmb::idType id) const;
	virtual bool hasId(kmb::Face id) const;

	virtual bool deleteId(kmb::idType id);
	virtual bool deleteId(kmb::Face id);

	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual bool replaceId(kmb::Face old_id,kmb::Face new_id);
	virtual int replaceNodeId( std::map< kmb::nodeIdType, kmb::nodeIdType> &idmap );

	virtual bool setPhysicalValue(kmb::PhysicalValue* val);
	virtual bool setPhysicalValue(kmb::idType id, kmb::PhysicalValue* value);
	virtual bool setPhysicalValue(kmb::Face f, kmb::PhysicalValue* value);
	virtual bool setPhysicalValue(kmb::idType id, double *value);
	virtual bool setPhysicalValue(kmb::Face f, double *value);
	virtual bool setPhysicalValue(kmb::idType id, long *value);
	virtual bool setPhysicalValue(kmb::Face f, long *value);

	virtual bool setValue(kmb::idType id, double value,int index=0);
	virtual bool setValue(kmb::Face f, double value,int index=0);
	virtual bool setValue(kmb::idType id, long value,int index=0);
	virtual bool setValue(kmb::Face f, long value,int index=0);

	virtual kmb::PhysicalValue* getPhysicalValue(void) const;
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::idType id) const;
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::Face f) const;
	virtual bool getPhysicalValue(kmb::idType id, double *value) const;
	virtual bool getPhysicalValue(kmb::Face f, double *value) const;
	virtual bool getPhysicalValue(kmb::idType id, long *value) const;
	virtual bool getPhysicalValue(kmb::Face f, long *value) const;

public:


	class _iterator
	{
	public:
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{ return kmb::nullId; };
		virtual bool getFace(kmb::Face &f) const{ return false; };
		virtual kmb::PhysicalValue* getValue(void) const{ return NULL; };
		virtual bool getValue(double *value) const{ return false; };
		virtual bool getValue(long *value) const{ return false; };
		virtual _iterator* operator++(void) = 0;
		virtual _iterator* operator++(int n) = 0;
		virtual _iterator* clone(void) = 0;
	};

public:

	class const_iterator;
	class iterator
	{
		friend class DataBindings;
		friend class DataBindings::const_iterator;
	private:
		_iterator* _iter;
	public:
		iterator(_iterator* _it=NULL)
			: _iter(_it){}
		iterator(const iterator& other);
		~iterator(void);
		kmb::idType getId(void) const;
		bool getFace(kmb::Face &f) const;
		kmb::PhysicalValue* getValue(void) const;
		bool getValue(double *value)  const;
		bool getValue(long *value)  const;

		iterator& operator=(const iterator& other);
		iterator& operator++(void);
		iterator  operator++(int n);
		bool operator==(const iterator &other) const;
		bool operator!=(const iterator &other) const;

		bool isFinished(void) const {	return (_iter==NULL);	}
	};

	class const_iterator
	{
		friend class DataBindings;
	private:
		_iterator* _iter;
	public:
		const_iterator(_iterator* _it=NULL)
			: _iter(_it){}
		const_iterator(const const_iterator& other);
		const_iterator(const iterator& other);
		~const_iterator(void);
		kmb::idType getId(void) const;
		bool getFace(kmb::Face &f) const;
		const kmb::PhysicalValue* getValue(void) const;
		bool getValue(double *value)  const;
		bool getValue(long *value)  const;

		const_iterator& operator=(const const_iterator& other);
		const_iterator& operator=(const iterator& other);
		const_iterator& operator++(void);
		const_iterator  operator++(int n);
		bool operator==(const const_iterator &other) const;
		bool operator!=(const const_iterator &other) const;

		bool isFinished(void) const {	return (_iter==NULL);	}
	};

	virtual iterator begin(void) = 0;
	virtual const_iterator begin(void) const = 0;


	static const iterator endIterator;
	static const const_iterator endConstIterator;
	const iterator& end(void){
		return DataBindings::endIterator;
	};
	const const_iterator& end(void) const{
		return DataBindings::endConstIterator;
	};
};

class DataBindingsGlobal : public DataBindings
{
	friend class DataBindings;
public:
	DataBindingsGlobal(DataBindings::bindingMode mode,kmb::PhysicalValue::valueType vType){
		this->type = vType;
		this->bMode = mode;
		this->value = NULL;
	}
	virtual ~DataBindingsGlobal(void){
		if( value != NULL ) delete value;
		value = NULL;
	}
	virtual const char* getContainerType(void) const{
		return "DataBindingsGlobal";
	};
protected:

	kmb::PhysicalValue* value;
public:
	virtual bool setPhysicalValue(kmb::PhysicalValue* val){
		if(val->getType() == type){
			this->value = val;
			return true;
		}else{
			return false;
		}
	};
	virtual kmb::PhysicalValue* getPhysicalValue(void) const {
		return this->value;
	};
	virtual int getDimension(void) const {
		if( value ){
			return value->getDimension();
		};
		return 0;
	}
	virtual size_t getIdCount() const {
		if( this->value ){
			return 1;
		}else{
			return 0;
		}
	};
public:
	virtual iterator begin(void){
		return kmb::DataBindings::iterator(NULL);
	}
	virtual const_iterator begin(void) const{
		return kmb::DataBindings::const_iterator(NULL);
	}
};

template<typename T>
class DataBindingsGroup : public DataBindings
{
	friend class DataBindings;
protected:
	DataBindingsGroup(DataBindings::bindingMode mode,kmb::PhysicalValue::valueType vType){
		this->type = vType;
		this->bMode = mode;
		this->value = NULL;
	}
public:
	virtual ~DataBindingsGroup(void){
		if( value != NULL ) delete value;
		value = NULL;
	}
	virtual const char* getContainerType(void) const{
		return "DataBindingsGroup";
	};
protected:
	std::set<T> group;

	kmb::PhysicalValue* value;
public:
	virtual bool setPhysicalValue(kmb::PhysicalValue* val){
		if(val != NULL && val->getType() == type){
			this->value = val;
			return true;
		}else{
			return false;
		}
	};
	virtual kmb::PhysicalValue* getPhysicalValue(void) const {
		return this->value;
	};
	virtual int getDimension(void) const {
		if( value ){
			return value->getDimension();
		};
		return 0;
	}
	virtual bool addId(kmb::idType id){
		group.insert(static_cast<T>(id));
		return true;
	};
	virtual bool hasId(kmb::idType id) const {
		return (group.find(static_cast<T>(id)) != group.end());
	};
	virtual bool deleteId(kmb::idType id){
		return ( group.erase(static_cast<T>(id)) > 0 );
	};
	virtual void clear(void){
		group.clear();
	};

	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id){
		T old_t = static_cast<T>(old_id);
		T new_t = static_cast<T>(new_id);
		if( group.find(old_t) != group.end() &&
			group.find(new_t) == group.end() )
		{
			group.erase(old_t);
			group.insert(new_t);
			return true;
		}else{
			return false;
		}
	};
	void replaceId(std::map<T,T> &idmap)
	{
		std::set<T> tempSet;
		typename std::set<T>::iterator iIter = group.begin();
		while( iIter != group.end() )
		{
			T t = (*iIter);
			if( idmap.find(t) != idmap.end() ){
				tempSet.insert( idmap[t] );
			}else{
				tempSet.insert( t );
			}
			++iIter;
		}
		group.clear();
		typename std::set<T>::iterator jIter = tempSet.begin();
		while( jIter != tempSet.end() )
		{
			T t = (*jIter);
			group.insert( t );
			++jIter;
		}
	};
	virtual size_t getIdCount(void) const{
		return group.size();
	};

public:
	class _iterator : public DataBindings::_iterator
	{
		friend class DataBindingsGroup<T>;
	private:
		typename std::set<T>::const_iterator dataIterator;
		typename std::set<T>::const_iterator endIterator;
	public:
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{
			return static_cast< kmb::idType >( *dataIterator );
		};
		virtual bool getFace(kmb::Face &f) const{
			return false;
		};
		virtual kmb::PhysicalValue* getValue(void) const{
			return NULL;
		};
		virtual bool getValue(double *value) const{
			return false;
		};
		virtual _iterator* operator++(void){
			++dataIterator;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* operator++(int n){
			dataIterator++;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* clone(void){
			_iterator* _it = NULL;
			_it = new _iterator();
			_it->dataIterator = this->dataIterator;
			_it->endIterator = this->endIterator;
			return _it;
		};
	};
	virtual iterator begin(void){
		if( group.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIterator = this->group.begin();
		_it->endIterator = this->group.end();
		return kmb::DataBindings::iterator(_it);
	}
	virtual const_iterator begin(void) const{
		if( group.size() == 0 ){
			return kmb::DataBindings::const_iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIterator = this->group.begin();
		_it->endIterator = this->group.end();
		return kmb::DataBindings::const_iterator(_it);
	}
};

template<>
class DataBindingsGroup<kmb::Face> : public DataBindings
{
	friend class DataBindings;
protected:
	DataBindingsGroup(DataBindings::bindingMode mode,kmb::PhysicalValue::valueType vType){
		this->type = vType;
		this->bMode = mode;
		this->value = NULL;
	}
public:
	virtual ~DataBindingsGroup(void){
		if( value != NULL ) delete value;
		value = NULL;
	}
	virtual const char* getContainerType(void) const{
		return "DataBindingsGroup<kmb::Face>";
	};
protected:
	std::set<kmb::Face> group;

	kmb::PhysicalValue* value;
public:
	virtual bool setPhysicalValue(kmb::PhysicalValue* val){
		if(val != NULL && val->getType() == type){
			this->value = val;
			return true;
		}else{
			return false;
		}
	};
	virtual kmb::PhysicalValue* getPhysicalValue(void) const{
		return this->value;
	};
	virtual int getDimension(void) const {
		if( value ){
			return value->getDimension();
		};
		return 0;
	}
	virtual size_t getIdCount(void) const{
		return group.size();
	};
	virtual bool addId(kmb::Face id){
		group.insert(id);
		return true;
	};
	virtual bool hasId(kmb::Face id) const{
		return (group.find(id) != group.end());
	};
	virtual bool deleteId(kmb::Face id){
		return ( group.erase(id) > 0 );
	};
	virtual void clear(void){
		group.clear();
	};
	virtual bool replaceId(kmb::Face old_id,kmb::Face new_id){
		if( group.find(old_id) != group.end() &&
			group.find(new_id) == group.end() )
		{
			group.erase(old_id);
			group.insert(new_id);
			return true;
		}else{
			return false;
		}
	};
	void replaceId(std::map<kmb::Face,kmb::Face> &idmap)
	{
		std::set<kmb::Face> tempSet;
		std::set<kmb::Face>::iterator iIter = group.begin();
		while( iIter != group.end() )
		{
			kmb::Face t = (*iIter);
			if( idmap.find(t) != idmap.end() ){
				tempSet.insert( idmap[t] );
			}else{
				tempSet.insert( t );
			}
			++iIter;
		}
		group.clear();
		std::set<kmb::Face>::iterator jIter = tempSet.begin();
		while( jIter != tempSet.end() )
		{
			kmb::Face t = (*jIter);
			group.insert( t );
			++jIter;
		}
	};

public:
	class _iterator : public DataBindings::_iterator
	{
		friend class DataBindingsGroup<kmb::Face>;
	private:
		std::set<kmb::Face>::const_iterator dataIterator;
		std::set<kmb::Face>::const_iterator endIterator;
	public:
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{
			return kmb::nullId;
		};
		virtual bool getFace(kmb::Face &f) const{
			f.setId( dataIterator->getElementId(), dataIterator->getLocalFaceId() );
			return true;
		};
		virtual kmb::PhysicalValue* getValue(void) const{
			return NULL;
		};
		virtual bool getValue(double *value) const{
			return false;
		};
		virtual _iterator* operator++(void){
			++dataIterator;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* operator++(int n){
			dataIterator++;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* clone(void){
			kmb::DataBindingsGroup<kmb::Face>::_iterator* _it = NULL;
			_it = new kmb::DataBindingsGroup<kmb::Face>::_iterator();
			_it->dataIterator = this->dataIterator;
			_it->endIterator = this->endIterator;
			return _it;
		};
	};
	virtual iterator begin(void){
		if( group.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		kmb::DataBindingsGroup<kmb::Face>::_iterator* _it = NULL;
		_it = new kmb::DataBindingsGroup<kmb::Face>::_iterator();
		_it->dataIterator = this->group.begin();
		_it->endIterator = this->group.end();
		return kmb::DataBindings::iterator(_it);
	}
	virtual const_iterator begin(void) const{
		if( group.size() == 0 ){
			return kmb::DataBindings::const_iterator(NULL);
		}
		kmb::DataBindingsGroup<kmb::Face>::_iterator* _it = NULL;
		_it = new kmb::DataBindingsGroup<kmb::Face>::_iterator();
		_it->dataIterator = this->group.begin();
		_it->endIterator = this->group.end();
		return kmb::DataBindings::const_iterator(_it);
	}
};



template<typename T,typename V>
class DataVariable : public DataBindings
{
	friend class DataBindings;
protected:
	DataVariable(DataBindings::bindingMode mode){
		this->type = V::vType;
		this->bMode = mode;
	}
public:
	virtual ~DataVariable(void){
		clear();
	}
	virtual const char* getContainerType(void) const{
		return "DataVariable";
	};
	virtual void clear(void){
		typename std::map<T,kmb::PhysicalValue*>::iterator pIter = mapper.begin();
		while( pIter != mapper.end() ){
			kmb::PhysicalValue* v = pIter->second;
			if( v != NULL ){
				delete v;
			}
			++pIter;
		}
		mapper.clear();
	};
private:
	std::map<T,kmb::PhysicalValue*> mapper;
public:
	virtual bool setPhysicalValue(T t,kmb::PhysicalValue* val){
		if(val != NULL && val->getType() == type){
			typename std::map<T,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				delete dIter->second;
				mapper.erase( dIter );
			}
			mapper.insert( std::pair<T,kmb::PhysicalValue*>(t,val) );
			return true;
		}else{
			return false;
		}
	};
	virtual bool setPhysicalValue(T t,double* value){
		if( value != NULL ){
			typename std::map<T,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				dIter->second->setValue(value);
			}else{

				kmb::PhysicalValue* v = new V();
				v->setValue(value);
				mapper.insert( std::pair<T,kmb::PhysicalValue*>(t,v) );
			}
			return true;
		}
		return false;
	};
	virtual bool setPhysicalValue(T t,long* value){
		if( value != NULL ){
			typename std::map<T,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				dIter->second->setValue(value);
			}else{

				kmb::PhysicalValue* v = new V();
				v->setValue(value);
				mapper.insert( std::pair<T,kmb::PhysicalValue*>(t,v) );
			}
			return true;
		}
		return false;
	};
	virtual bool setValue(T t, double value,int index=0){
		typename std::map<T,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
		if( dIter != mapper.end() ){

			dIter->second->setValue( value, index );
			return true;
		}else{

			kmb::PhysicalValue* v = new V();
			v->setValue( value, index );
			mapper.insert( std::pair<T,kmb::PhysicalValue*>(t,v) );
			return true;
		}
	}
	virtual bool setValue(T t, long value,int index=0){
		typename std::map<T,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
		if( dIter != mapper.end() ){

			dIter->second->setValue( value, index );
			return true;
		}else{

			kmb::PhysicalValue* v = new V();
			v->setValue( value, index );
			mapper.insert( std::pair<T,kmb::PhysicalValue*>(t,v) );
			return true;
		}
	}
	virtual kmb::PhysicalValue* getPhysicalValue(T t) const{
		typename std::map<T,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(t);
		if( pIter != this->mapper.end()){
			return pIter->second;
		}else{
			return NULL;
		}
	};
	virtual bool getPhysicalValue(T t, double *value) const{
		typename std::map<T,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(t);
		if( pIter == this->mapper.end()){
			return false;
		}else{
			return pIter->second->getValue(value);
		}
	}
	virtual bool getPhysicalValue(T t, long *value) const{
		typename std::map<T,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(t);
		if( pIter == this->mapper.end()){
			return false;
		}else{
			return pIter->second->getValue(value);
		}
	}
	virtual int getDimension(void) const {
		return kmb::PhysicalValue::getDimension( V::vType );
	}
	virtual bool hasId(T t) const{
		return (mapper.find(t) != mapper.end());
	};
	virtual bool deleteId(T t){
		typename std::map<T,kmb::PhysicalValue*>::iterator pIter = mapper.find(t);
		if( pIter != mapper.end() )
		{
			kmb::PhysicalValue* val = pIter->second;
			if( val ){
				delete val;
			}
			mapper.erase(pIter);
			return true;
		}else{
			return false;
		}
	};
	virtual bool replaceId(T old_t,T new_t){
		typename std::map<T,kmb::PhysicalValue*>::iterator pIter = mapper.find(old_t);
		if( pIter != mapper.end() &&
			mapper.find(new_t) == mapper.end() )
		{
			kmb::PhysicalValue* val = pIter->second;
			mapper.erase(pIter);
			if( val ){
				mapper.insert( std::pair<T,kmb::PhysicalValue*>(new_t,val) );
			}
			return true;
		}else{
			return false;
		}
	};
	void replaceId(std::map<T,T> &idmap)
	{
		typename std::map<T,kmb::PhysicalValue*> tempMap;
		typename std::map<T,kmb::PhysicalValue*>::iterator pIter = mapper.begin();
		while( pIter != mapper.end() )
		{
			T t = pIter->first;
			if( pIter->second != NULL ){
				typename std::map<T,T>::iterator tIter = idmap.find(t);
				if( tIter != idmap.end() ){
					tempMap.insert( std::pair<T,kmb::PhysicalValue*>( tIter->second, pIter->second ) );
				}else{
					tempMap.insert( std::pair<T,kmb::PhysicalValue*>( t, pIter->second ) );
				}
			}
			++pIter;
		}

		mapper.clear();
		typename std::map<T,kmb::PhysicalValue*>::iterator qIter = tempMap.begin();
		while( qIter != tempMap.end() )
		{
			T t = qIter->first;
			if( qIter->second != NULL ){
				mapper.insert( std::pair<T,kmb::PhysicalValue*>( t, qIter->second ) );
			}
			++qIter;
		}
	};
	virtual int replaceNodeId( std::map< kmb::nodeIdType, kmb::nodeIdType> &idmap )
	{
		int count = 0;
		if( this->bMode == kmb::DataBindings::NODEVARIABLE ){
			typename std::map<T,kmb::PhysicalValue*> tempMap;
			typename std::map<T,kmb::PhysicalValue*>::iterator pIter = mapper.begin();
			while( pIter != mapper.end() )
			{
				T t = pIter->first;
				if( pIter->second != NULL ){
					typename std::map<T,T>::iterator tIter = idmap.find(t);
					if( tIter != idmap.end() ){
						++count;
						tempMap.insert( std::pair<T,kmb::PhysicalValue*>( tIter->second, pIter->second ) );
					}else{
						tempMap.insert( std::pair<T,kmb::PhysicalValue*>( t, pIter->second ) );
					}
				}
				++pIter;
			}

			mapper.clear();
			typename std::map<T,kmb::PhysicalValue*>::iterator qIter = tempMap.begin();
			while( qIter != tempMap.end() )
			{
				T t = qIter->first;
				if( qIter->second != NULL ){
					mapper.insert( std::pair<T,kmb::PhysicalValue*>( t, qIter->second ) );
				}
				++qIter;
			}
		}
		return count;
	};
	virtual size_t getIdCount() const{
		return mapper.size();
	};

public:
	class _iterator : public DataBindings::_iterator
	{
		friend class DataVariable<T,V>;
	private:
		typename std::map<T,kmb::PhysicalValue*>::const_iterator dataIterator;
		typename std::map<T,kmb::PhysicalValue*>::const_iterator endIterator;
	public:
		_iterator(void)
		: DataBindings::_iterator()
		{}
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{
			return static_cast< kmb::idType >( dataIterator->first );
		};
		virtual bool getFace(kmb::Face &f) const{
			return false;
		};
		virtual kmb::PhysicalValue* getValue(void) const{
			return dataIterator->second;
		};
		virtual bool getValue(double *value) const{
			return dataIterator->second->getValue(value);
		};
		virtual bool getValue(long *value) const{
			return dataIterator->second->getValue(value);
		};
		virtual _iterator* operator++(void){
			++dataIterator;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* operator++(int n){
			dataIterator++;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* clone(void){
			_iterator* _it = NULL;
			_it = new _iterator();
			_it->dataIterator = this->dataIterator;
			_it->endIterator = this->endIterator;
			return _it;
		};
	};
	virtual iterator begin(void){
		if( mapper.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::iterator(_it);
	}
	virtual const_iterator begin(void) const{
		if( mapper.size() == 0 ){
			return kmb::DataBindings::const_iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::const_iterator(_it);
	}
};

template<typename V>
class DataVariable<kmb::Face,V> : public DataBindings
{
	friend class DataBindings;
protected:
	DataVariable(DataBindings::bindingMode mode){
		this->type = V::vType;
		this->bMode = mode;
	}
public:
	virtual ~DataVariable(void){
		clear();
	}
	virtual const char* getContainerType(void) const{
		return "DataVariableFace";
	};
	virtual void clear(void){
		typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.begin();
		while( pIter != mapper.end() ){
			kmb::PhysicalValue* v = pIter->second;
			if( v != NULL ){
				delete v;
			}
			++pIter;
		}
		mapper.clear();
	};
private:
	std::map<kmb::Face,kmb::PhysicalValue*> mapper;
public:
	virtual bool setPhysicalValue(kmb::Face t,kmb::PhysicalValue* val){
		if(val != NULL && val->getType() == type){
			typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				delete dIter->second;
				mapper.erase( dIter );
			}
			mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,val) );
			return true;
		}else{
			return false;
		}
	};
	virtual bool setPhysicalValue(kmb::Face t,double* value){
		if( value != NULL ){
			typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				dIter->second->setValue(value);
			}else{

				kmb::PhysicalValue* v = new V();
				v->setValue(value);
				mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
			}
			return true;
		}
		return false;
	};
	virtual bool setPhysicalValue(kmb::Face t,long* value){
		if( value != NULL ){
			typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				dIter->second->setValue(value);
			}else{

				kmb::PhysicalValue* v = new V();
				v->setValue(value);
				mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
			}
			return true;
		}
		return false;
	};
	virtual bool setValue(kmb::Face t, double value,int index=0){
		typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
		if( dIter != mapper.end() ){

			dIter->second->setValue( value, index );
			return true;
		}else{

			kmb::PhysicalValue* v = new V();
			v->setValue( value, index );
			mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
			return true;
		}
	}
	virtual bool setValue(kmb::Face t, long value,int index=0){
		typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
		if( dIter != mapper.end() ){

			dIter->second->setValue( value, index );
			return true;
		}else{

			kmb::PhysicalValue* v = new V();
			v->setValue( value, index );
			mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
			return true;
		}
	}
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::Face t) const{
		typename std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(t);
		if( pIter != this->mapper.end()){
			return pIter->second;
		}else{
			return NULL;
		}
	};
	virtual bool getPhysicalValue(kmb::Face t, double *value) const{
		typename std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(t);
		if( pIter == this->mapper.end()){
			return false;
		}else{
			return pIter->second->getValue(value);
		}
	}
	virtual bool getPhysicalValue(kmb::Face t, long *value) const{
		typename std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(t);
		if( pIter == this->mapper.end()){
			return false;
		}else{
			return pIter->second->getValue(value);
		}
	}
	virtual int getDimension(void) const {
		return kmb::PhysicalValue::getDimension( V::vType );
	}
	virtual bool hasId(kmb::Face t) const{
		return (mapper.find(t) != mapper.end());
	};
	virtual bool deleteId(kmb::Face t){
		typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.find(t);
		if( pIter != mapper.end() )
		{
			kmb::PhysicalValue* val = pIter->second;
			if( val ){
				delete val;
			}
			mapper.erase(pIter);
			return true;
		}else{
			return false;
		}
	};
	virtual bool replaceId(kmb::Face old_t,kmb::Face new_t){
		typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.find(old_t);
		if( pIter != mapper.end() &&
			mapper.find(new_t) == mapper.end() )
		{
			kmb::PhysicalValue* val = pIter->second;
			mapper.erase(pIter);
			if( val ){
				mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(new_t,val) );
			}
			return true;
		}else{
			return false;
		}
	};
	void replaceId(std::map<kmb::Face,kmb::Face> &idmap)
	{
		typename std::map<kmb::Face,kmb::PhysicalValue*> tempMap;
		typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.begin();
		while( pIter != mapper.end() )
		{
			kmb::Face t = pIter->first;
			if( pIter->second != NULL ){
				typename std::map<kmb::Face,kmb::Face>::iterator tIter = idmap.find(t);
				if( tIter != idmap.end() ){
					tempMap.insert( std::pair<kmb::Face,kmb::PhysicalValue*>( tIter->second, pIter->second ) );
				}else{
					tempMap.insert( std::pair<kmb::Face,kmb::PhysicalValue*>( t, pIter->second ) );
				}
			}
			++pIter;
		}

		mapper.clear();
		typename std::map<kmb::Face,kmb::PhysicalValue*>::iterator qIter = tempMap.begin();
		while( qIter != tempMap.end() )
		{
			kmb::Face t = qIter->first;
			if( qIter->second != NULL ){
				mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>( t, qIter->second ) );
			}
			++qIter;
		}
	};
	virtual size_t getIdCount() const{
		return mapper.size();
	};

public:
	class _iterator : public DataBindings::_iterator
	{
		friend class DataVariable<kmb::Face,V>;
	private:
		typename std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator dataIterator;
		typename std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator endIterator;
	public:
		_iterator(void)
		: DataBindings::_iterator()
		{}
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{
			return kmb::nullId;
		};
		virtual bool getFace(kmb::Face &f) const{
			f.setId( dataIterator->first.getElementId(), dataIterator->first.getLocalFaceId() );
			return true;
		};
		virtual kmb::PhysicalValue* getValue(void) const{
			return dataIterator->second;
		};
		virtual bool getValue(double *value) const{
			return dataIterator->second->getValue(value);
		};
		virtual bool getValue(long *value) const{
			return dataIterator->second->getValue(value);
		};
		virtual _iterator* operator++(void){
			++dataIterator;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* operator++(int n){
			dataIterator++;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* clone(void){
			_iterator* _it = NULL;
			_it = new _iterator();
			_it->dataIterator = this->dataIterator;
			_it->endIterator = this->endIterator;
			return _it;
		};
	};
	virtual iterator begin(void){
		if( mapper.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::iterator(_it);
	}
	virtual const_iterator begin(void) const{
		if( mapper.size() == 0 ){
			return kmb::DataBindings::const_iterator(NULL);
		}
		_iterator* _it = NULL;
		_it = new _iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::const_iterator(_it);
	}
};

/*
template<>
class DataBindingsEach<kmb::Face> : public DataBindings
{
	friend class DataBindings;
protected:
	DataBindingsEach(DataBindings::bindingMode mode,kmb::PhysicalValue::valueType vType){
		this->type = vType;
		this->bMode = mode;
	}
public:
	virtual ~DataBindingsEach(void){
		clear();
	}
	virtual const char* getContainerType(void) const{
		return "DataBindingsEach<kmb::Face>";
	};
	virtual void clear(void){
		for( std::map<kmb::Face,kmb::PhysicalValue*>::iterator p = mapper.begin();
			p != mapper.end();
			++p)
		{
			kmb::PhysicalValue* v = p->second;
			if( v != NULL ){
				delete v;
			}
		}
		mapper.clear();
	}
private:
	std::map<kmb::Face,kmb::PhysicalValue*> mapper;
public:
	virtual bool setPhysicalValue(kmb::Face t,kmb::PhysicalValue* val){
		if(val != NULL && val->getType() == type){
			std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				delete dIter->second;
				mapper.erase( dIter );
			}
			mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,val) );
			return true;
		}else{
			return false;
		}
	};
	virtual bool setPhysicalValue(kmb::Face t,double* value){
		if(value != NULL ){
			std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				delete dIter->second;
				mapper.erase( dIter );
			}
			switch( getValueType() )
			{
			case kmb::PhysicalValue::SCALAR:
				{
					kmb::ScalarValue* v = new kmb::ScalarValue( value[0] );
					mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
					return true;
				}
			case kmb::PhysicalValue::VECTOR2:
				{
					kmb::Vector2Value* v = new kmb::Vector2Value( value[0], value[1] );
					mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
					return true;
				}
			case kmb::PhysicalValue::VECTOR3:
				{
					kmb::Vector3Value* v = new kmb::Vector3Value( value[0], value[1], value[2] );
					mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
					return true;
				}
			case kmb::PhysicalValue::VECTOR4:
				{
					kmb::Vector4Value* v = new kmb::Vector4Value( value[0], value[1], value[2], value[3] );
					mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
					return true;
				}
			case kmb::PhysicalValue::TENSOR6:
				{
					kmb::Tensor6Value* v = new kmb::Tensor6Value( value[0], value[1], value[2], value[3], value[4], value[5] );
					mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
					return true;
				}
			case kmb::PhysicalValue::POINT3VECTOR3:
				{
					kmb::Point3Vector3Value* v = new kmb::Point3Vector3Value( value[0], value[1], value[2], value[3], value[4], value[5] );
					mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
					return true;
				}
			default:
				return false;
			}
		}else{
			return false;
		}
	};
	virtual bool setPhysicalValue(kmb::Face t,long* value){
		if(value != NULL ){
			std::map<kmb::Face,kmb::PhysicalValue*>::iterator dIter = mapper.find(t);
			if( dIter != mapper.end() ){
				delete dIter->second;
				mapper.erase( dIter );
			}
			switch( getValueType() )
			{
			case kmb::PhysicalValue::INTEGER:
				{
					kmb::IntegerValue* v = new kmb::IntegerValue( value[0] );
					mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(t,v) );
					return true;
				}
			default:
				return false;
			}
		}else{
			return false;
		}
	};
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::Face t){
		std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.find(t);
		if( pIter != this->mapper.end() ){
			return pIter->second;
		}else{
			return NULL;
		}
	};
	virtual bool getPhysicalValue(kmb::Face f, double *value) const{
		std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(f);
		if( pIter == this->mapper.end()){
			return false;
		}
		switch( getValueType() )
		{
		case kmb::PhysicalValue::SCALAR:
			value[0] = reinterpret_cast< kmb::ScalarValue* >( pIter->second )->getValue();
			return true;
		case kmb::PhysicalValue::VECTOR2:
			value[0] = reinterpret_cast< kmb::Vector2Value* >( pIter->second )->getValue(0);
			value[1] = reinterpret_cast< kmb::Vector2Value* >( pIter->second )->getValue(1);
			return true;
		case kmb::PhysicalValue::VECTOR3:
			value[0] = reinterpret_cast< kmb::Vector3Value* >( pIter->second )->getValue(0);
			value[1] = reinterpret_cast< kmb::Vector3Value* >( pIter->second )->getValue(1);
			value[2] = reinterpret_cast< kmb::Vector3Value* >( pIter->second )->getValue(2);
			return true;
		case kmb::PhysicalValue::VECTOR4:
			value[0] = reinterpret_cast< kmb::Vector3Value* >( pIter->second )->getValue(0);
			value[1] = reinterpret_cast< kmb::Vector3Value* >( pIter->second )->getValue(1);
			value[2] = reinterpret_cast< kmb::Vector3Value* >( pIter->second )->getValue(2);
			value[3] = reinterpret_cast< kmb::Vector4Value* >( pIter->second )->getValue(3);
			return true;
		case kmb::PhysicalValue::TENSOR6:
			value[0] = reinterpret_cast< kmb::Tensor6Value* >( pIter->second )->getValue(0);
			value[1] = reinterpret_cast< kmb::Tensor6Value* >( pIter->second )->getValue(1);
			value[2] = reinterpret_cast< kmb::Tensor6Value* >( pIter->second )->getValue(2);
			value[3] = reinterpret_cast< kmb::Tensor6Value* >( pIter->second )->getValue(3);
			value[4] = reinterpret_cast< kmb::Tensor6Value* >( pIter->second )->getValue(4);
			value[5] = reinterpret_cast< kmb::Tensor6Value* >( pIter->second )->getValue(5);
			return true;
		case kmb::PhysicalValue::POINT3VECTOR3:
			value[0] = reinterpret_cast< kmb::Point3Vector3Value* >( pIter->second )->getPoint(0);
			value[1] = reinterpret_cast< kmb::Point3Vector3Value* >( pIter->second )->getPoint(1);
			value[2] = reinterpret_cast< kmb::Point3Vector3Value* >( pIter->second )->getPoint(2);
			value[3] = reinterpret_cast< kmb::Point3Vector3Value* >( pIter->second )->getValue(0);
			value[4] = reinterpret_cast< kmb::Point3Vector3Value* >( pIter->second )->getValue(1);
			value[5] = reinterpret_cast< kmb::Point3Vector3Value* >( pIter->second )->getValue(2);
			return true;
		default:
			return false;
		}
	}
	virtual bool getPhysicalValue(kmb::Face f, long *value) const{
		std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator pIter = mapper.find(f);
		if( pIter == this->mapper.end()){
			return false;
		}
		switch( getValueType() )
		{
		case kmb::PhysicalValue::INTEGER:
			value[0] = reinterpret_cast< kmb::IntegerValue* >( pIter->second )->getValue();
			return true;
		default:
			return false;
		}
	}
	virtual int getDimension(void) const {
		return kmb::PhysicalValue::getDimension( getValueType() );
	}
	virtual bool hasId(kmb::Face id) const{
		return (mapper.find(id) != mapper.end());
	};
	virtual bool deleteId(kmb::Face id){
		std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.find(id);
		if( pIter != mapper.end() )
		{
			kmb::PhysicalValue* val = pIter->second;
			if( val ){
				delete val;
			}
			mapper.erase( pIter );
			return true;
		}else{
			return false;
		}
	};
	virtual bool replaceId(kmb::Face old_id,kmb::Face new_id){
		std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.find(old_id);
		if( pIter != mapper.end() &&
			mapper.find(new_id) == mapper.end() )
		{
			kmb::PhysicalValue* val = pIter->second;
			mapper.erase( pIter );
			mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>(new_id,val) );
			return true;
		}else{
			return false;
		}
	};
	void replaceId(std::map<kmb::Face,kmb::Face> &idmap)
	{
		std::map<kmb::Face,kmb::PhysicalValue*> tempMap;
		std::map<kmb::Face,kmb::PhysicalValue*>::iterator pIter = mapper.begin();
		while( pIter != mapper.end() )
		{
			kmb::Face t = pIter->first;
			if( pIter->second != NULL ){
				std::map<kmb::Face,kmb::Face>::iterator fIter = idmap.find(t);
				if( fIter != idmap.end() ){
					tempMap.insert( std::pair<kmb::Face,kmb::PhysicalValue*>( fIter->second, pIter->second ) );
				}else{
					tempMap.insert( std::pair<kmb::Face,kmb::PhysicalValue*>( t, pIter->second ) );
				}
			}
			++pIter;
		}

		mapper.clear();
		std::map<kmb::Face,kmb::PhysicalValue*>::iterator qIter = tempMap.begin();
		while( qIter != tempMap.end() )
		{
			kmb::Face t = qIter->first;
			if( qIter->second != NULL ){
				mapper.insert( std::pair<kmb::Face,kmb::PhysicalValue*>( t, qIter->second ) );
			}
			++qIter;
		}
	};
	virtual size_t getIdCount() const{
		return mapper.size();
	};

public:
	class _iterator : public DataBindings::_iterator
	{
		friend class DataBindingsEach<kmb::Face>;
	private:
		std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator dataIterator;
		std::map<kmb::Face,kmb::PhysicalValue*>::const_iterator endIterator;
	public:
		virtual ~_iterator(void){};
		virtual kmb::idType getId(void) const{
			return kmb::nullId;
		};
		virtual bool getFace(kmb::Face &f) const{
			f.setId( dataIterator->first.getElementId(), dataIterator->first.getLocalFaceId() );
			return true;
		};
		virtual kmb::PhysicalValue* getValue(void) const{
			return dataIterator->second;
		};
		virtual bool getValue(double *value) const{
			switch( dataIterator->second->getType() )
			{
			case kmb::PhysicalValue::SCALAR:
				value[0] = reinterpret_cast< kmb::ScalarValue* >( dataIterator->second )->getValue();
				return true;
			case kmb::PhysicalValue::VECTOR2:
				value[0] = reinterpret_cast< kmb::Vector2Value* >( dataIterator->second )->getValue(0);
				value[1] = reinterpret_cast< kmb::Vector2Value* >( dataIterator->second )->getValue(1);
				return true;
			case kmb::PhysicalValue::VECTOR3:
				value[0] = reinterpret_cast< kmb::Vector3Value* >( dataIterator->second )->getValue(0);
				value[1] = reinterpret_cast< kmb::Vector3Value* >( dataIterator->second )->getValue(1);
				value[2] = reinterpret_cast< kmb::Vector3Value* >( dataIterator->second )->getValue(2);
				return true;
			case kmb::PhysicalValue::VECTOR4:
				value[0] = reinterpret_cast< kmb::Vector4Value* >( dataIterator->second )->getValue(0);
				value[1] = reinterpret_cast< kmb::Vector4Value* >( dataIterator->second )->getValue(1);
				value[2] = reinterpret_cast< kmb::Vector4Value* >( dataIterator->second )->getValue(2);
				value[3] = reinterpret_cast< kmb::Vector4Value* >( dataIterator->second )->getValue(3);
				return true;
			case kmb::PhysicalValue::POINT3VECTOR3:
				value[0] = reinterpret_cast< kmb::Point3Vector3Value* >( dataIterator->second )->getPoint(0);
				value[1] = reinterpret_cast< kmb::Point3Vector3Value* >( dataIterator->second )->getPoint(1);
				value[2] = reinterpret_cast< kmb::Point3Vector3Value* >( dataIterator->second )->getPoint(2);
				value[3] = reinterpret_cast< kmb::Point3Vector3Value* >( dataIterator->second )->getValue(0);
				value[4] = reinterpret_cast< kmb::Point3Vector3Value* >( dataIterator->second )->getValue(1);
				value[5] = reinterpret_cast< kmb::Point3Vector3Value* >( dataIterator->second )->getValue(2);
				return true;
			case kmb::PhysicalValue::TENSOR6:
				value[0] = reinterpret_cast< kmb::Tensor6Value* >( dataIterator->second )->getValue(0);
				value[1] = reinterpret_cast< kmb::Tensor6Value* >( dataIterator->second )->getValue(1);
				value[2] = reinterpret_cast< kmb::Tensor6Value* >( dataIterator->second )->getValue(2);
				value[3] = reinterpret_cast< kmb::Tensor6Value* >( dataIterator->second )->getValue(3);
				value[4] = reinterpret_cast< kmb::Tensor6Value* >( dataIterator->second )->getValue(4);
				value[5] = reinterpret_cast< kmb::Tensor6Value* >( dataIterator->second )->getValue(5);
				return true;
			default:
				return false;
			}
		};
		virtual bool getValue(long *value) const{
			switch( dataIterator->second->getType() )
			{
			case kmb::PhysicalValue::INTEGER:
				value[0] = reinterpret_cast< kmb::IntegerValue* >( dataIterator->second )->getValue();
				return true;
			default:
				return false;
			}
		};
		virtual _iterator* operator++(void){
			++dataIterator;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* operator++(int n){
			dataIterator++;
			if( dataIterator != endIterator ){
				return this;
			}else{
				return NULL;
			}
		};
		virtual _iterator* clone(void){
			kmb::DataBindingsEach<kmb::Face>::_iterator* _it = NULL;
			_it = new kmb::DataBindingsEach<kmb::Face>::_iterator();
			_it->dataIterator = this->dataIterator;
			_it->endIterator = this->endIterator;
			return _it;
		};
	};
	virtual iterator begin(void){
		if( mapper.size() == 0 ){
			return kmb::DataBindings::iterator(NULL);
		}
		kmb::DataBindingsEach<kmb::Face>::_iterator* _it = NULL;
		_it = new kmb::DataBindingsEach<kmb::Face>::_iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::iterator(_it);
	}
	virtual const_iterator begin(void) const{
		if( mapper.size() == 0 ){
			return kmb::DataBindings::const_iterator(NULL);
		}
		kmb::DataBindingsEach<kmb::Face>::_iterator* _it = NULL;
		_it = new kmb::DataBindingsEach<kmb::Face>::_iterator();
		_it->dataIterator = this->mapper.begin();
		_it->endIterator = this->mapper.end();
		return kmb::DataBindings::const_iterator(_it);
	}
};
*/
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

