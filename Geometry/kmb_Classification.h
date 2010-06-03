/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Classification                                          #
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
#pragma once







#include <set>
#include <map>
#include <iostream>

#include "Geometry/kmb_idTypes.h"

namespace kmb{

typedef int classificationType;

template<typename T>
class Classification2
{
public:
	Classification2(void){}
	virtual ~Classification2(void){}
	std::multimap<T,T>	equivalentTable;
private:
	std::map<T,T>		representTable;
public:
	bool add(T t0){
		if( representTable.find(t0) == representTable.end() ){
			equivalentTable.insert( std::pair< T, T >(t0,t0) );
			representTable.insert( std::pair< T, T >(t0,t0) );
			return true;
		}else{
			return false;
		}
	}
	bool isEquivalent(T t0,T t1){
		typename std::map<T,T>::iterator r0Iter = representTable.find(t0);
		typename std::map<T,T>::iterator r1Iter = representTable.find(t1);
		return ( r0Iter != representTable.end() && r1Iter != representTable.end() && r0Iter == r1Iter );
	}

	bool equivalent(T t0,T t1){
		if( t0 == t1 ){
			if( representTable.find(t0) == representTable.end() ){
				equivalentTable.insert( std::pair< T, T >(t0,t0) );
				representTable.insert( std::pair< T, T >(t0,t0) );
			}
			return true;
		}
		if( t1 < t0 ){
			T t = t0;
			t0 = t1;
			t1 = t;
		}
		typename std::map<T,T>::iterator r0Iter = representTable.find(t0);
		typename std::map<T,T>::iterator r1Iter = representTable.find(t1);

		if( r0Iter == representTable.end() && r1Iter == representTable.end() ){
			equivalentTable.insert( std::pair< T, T >(t0,t0) );
			equivalentTable.insert( std::pair< T, T >(t0,t1) );
			representTable.insert( std::pair< T, T >(t0,t0) );
			representTable.insert( std::pair< T, T >(t1,t0) );

		}else

		if( r0Iter != representTable.end() && r1Iter == representTable.end() ){
			T t2 = r0Iter->second;
			equivalentTable.insert( std::pair< T, T >(t2,t1) );
			representTable.insert( std::pair< T, T >(t1,t2) );

		}else

		if( r0Iter == representTable.end() && r1Iter != representTable.end() ){
			T t2 = r1Iter->second;
			if( t2 < t0 ){
				equivalentTable.insert( std::pair< T, T >(t2,t0) );
				representTable.insert( std::pair< T, T >(t0,t2) );
			}else{
				convertKey(t2,t0);
				equivalentTable.insert( std::pair< T, T >(t0,t1) );
				representTable.insert( std::pair< T, T >(t1,t0) );
			}

		}else

		{
			T t2 = r0Iter->second;
			T t3 = r1Iter->second;

			if( t2 < t3 ){
				convertKey(t3,t2);
			}else if( t3 < t2 ){
				convertKey(t2,t3);
			}
		}

















		return true;
	}

	T getMinimalItem(T t){
		typename std::map<T,T>::iterator rIter = representTable.find(t);
		if( rIter != representTable.end() ){
			return rIter->second;
		}else{
			return t;
		}
	}
private:

	void convertKey(T t0,T t1){
		if( t1 < t0 ){
			std::pair< typename std::multimap<T,T>::iterator, typename std::multimap<T,T>::iterator >
					tIterPair = equivalentTable.equal_range( t0 );
			typename std::multimap<T,T>::iterator tIter = tIterPair.first;
			while( tIter != tIterPair.second ){
				equivalentTable.insert( std::pair< T, T >(t1,tIter->second) );
				representTable[tIter->second] = t1;
				equivalentTable.erase( tIter++ );
			}
		}
	}
public:


	typename std::multimap<T,T>::iterator getClassificationSet( T t, std::set< T > &divIDs)
	{

		std::pair< typename std::multimap<T,T>::iterator, typename std::multimap<T,T>::iterator >
				tIterPair = equivalentTable.equal_range( t );
		typename std::multimap<T,T>::iterator tIter = tIterPair.first;
		typename std::multimap<T,T>::iterator next_t = tIterPair.second;

		while( tIter != tIterPair.second ){

			divIDs.insert( tIter->second );
			++tIter;
		}
		return next_t;
	}
};

/*
template<typename T>
class Classification0
{
public:
	Classification0(void)
	{
		this->divisionID = 0;
	}
	~Classification0(void)
	{
		typename std::map<classificationType,std::set<T>*>::iterator p;
		for(p = this->divisionTable.begin();
			p != this->divisionTable.end();
			++p)
		{
			std::set<T>* eq = p->second;
			delete eq;
			p->second = NULL;
		}
		this->divisionTable.clear();
	}

	void			equivalent(T x,T y)
	{
		classificationType idx = getEquivalentID(x);
		classificationType idy = getEquivalentID(y);
		if(idx == -1 && idy == -1){
			std::set<T>* eq = NULL;
			eq = new std::set<T>;
			if( eq ){
				eq->insert( x );
				eq->insert( y );
				this->divisionTable[this->divisionID] = eq;
				++(this->divisionID);
			}
		}else if(idx == idy){

			return;
		}else if(idx == -1){
			this->divisionTable[idy]->insert(x);
		}else if(idy == -1){
			this->divisionTable[idx]->insert(y);
		}else{
			mergeEquivalentClass(idx,idy);
		}
	}


	classificationType	add(T x)
	{
		classificationType id = getEquivalentID(x);
		if(id==-1){
			std::set<T>* eq = NULL;
			eq = new std::set<T>;
			if( eq ){
				eq->insert( x );
				this->divisionTable[this->divisionID] = eq;
				id = this->divisionID;
				++(this->divisionID);
			}
		}
		return id;
	}







	std::map<classificationType,std::set<T>*>	divisionTable;
private:
	classificationType	divisionID;
	classificationType	getEquivalentID(T x)
	{
		typename std::map<classificationType,std::set<T>*>::iterator p;
		for( p = this->divisionTable.begin();
			 p != this->divisionTable.end();
			 ++p)
		{
			typename std::set<T>* eq = p->second;
			if( eq->find(x) != eq->end() ){

				return p->first;
			}
		}
		return -1;
	}

	void			mergeEquivalentClass(classificationType i0,classificationType i1)
	{
		if(i0==i1 || i0<0 || i1<0 || i0 >= divisionID || i1 >= divisionID){
			return;
		}else if(i0>i1){
			classificationType temp = i1;
			i1 = i0;
			i0 = temp;
		}
		if( !this->divisionTable[i0] ){

			return;
		}

		typename std::set<T>::iterator p;
		for(p = this->divisionTable[i1]->begin();
			p != this->divisionTable[i1]->end();
			++p)
		{
			T n = (*p);
			this->divisionTable[i0]->insert(n);
		}

		this->divisionTable.erase(i1);
	}
};



template<typename T>
class Classification1
{
public:
	std::map<T,classificationType>	divisionTable;
private:
	classificationType	divisionID;
public:
	Classification1(void){	this->divisionID = 0;	}
	~Classification1(void){}

	void equivalent(T x,T y)
	{
		classificationType idx = getEquivalentID(x);
		classificationType idy = getEquivalentID(y);
		if(idx == -1 && idy == -1){
			this->divisionTable[x] = this->divisionID;
			this->divisionTable[y] = this->divisionID;
			++this->divisionID;
		}else if(idx == idy){

			return;
		}else if(idx == -1){
			this->divisionTable[x] = idy;
		}else if(idy == -1){
			this->divisionTable[y] = idx;
		}else{
			mergeEquivalentClass(idx,idy);
		}
	}
	bool isEquivalent(T x,T y)
	{
		return (getEquivalentID(x) == getEquivalentID(y));
	}


	classificationType add(T x)
	{
		if( divisionTable.find(x) == divisionTable.end() ){
			divisionTable[x] = divisionID;
			++this->divisionID;
		}
		return divisionTable[x];
	}
	classificationType getEquivalentID(T x)
	{
		if( divisionTable.find(x) == divisionTable.end() ){
			return -1;
		}else{
			return divisionTable[x];
		}
	}

	void getClassificationSet( std::set< kmb::classificationType > &divIDs)
	{
		typename std::map<T,classificationType>::iterator cIter = divisionTable.begin();
		while( cIter != divisionTable.end() )
		{
			divIDs.insert( cIter->second );
		}
	}
private:

	void mergeEquivalentClass(classificationType i0,classificationType i1)
	{
		if(i0==i1 || i0<0 || i1<0 || i0 >= divisionID || i1 >= divisionID){
			return;
		}else if(i0>i1){
			classificationType temp = i1;
			i1 = i0;
			i0 = temp;
		}


		typename std::map<T,classificationType>::iterator p;
		for( p = divisionTable.begin(); p != divisionTable.end(); ++p)
		{
			if( p->second == i1 ){
				divisionTable[p->first] = i0;
			}
		}
	}
};
*/


template<typename T>
class Classification
{
public:
	class iterator
		{
			friend class Classification;
		private:
			typename std::multimap<T,T>::const_iterator cIter;
			typename std::multimap<T,T>::const_iterator endIter;
		public:
			iterator(void){}
			iterator(const iterator& other){
				cIter = other.cIter;
				endIter = other.endIter;
			}
			~iterator(void){}
			T getValue(void) const{
				return cIter->second;
			}
			T getClassId(void) const{
				return cIter->first;
			}
			iterator& operator=(const iterator& other){
				cIter = other.cIter;
				endIter = other.endIter;
				return *this;
			}
			iterator& operator++(void){
				++cIter;
				return *this;
			}
			iterator operator++(int n){
				typename Classification<T>::iterator it(*this);
				cIter++;
				return it;
			}
			bool operator==(const iterator &other) const{
				return cIter == other.cIter;
			}
			bool operator!=(const iterator &other) const{
				return cIter != other.cIter;
			}
			bool isFinished(void) const{
				return ( cIter == endIter );
			}
		};
protected:
	size_t classCount;

public:
	Classification(void) : classCount(0){}
	virtual ~Classification(void){}
protected:
	typename std::map<T,T> representTable;
	typename std::multimap<T,T> eqTable;


	void addPair(T x0,T x1){
		eqTable.insert( std::pair< T, T >(x0,x1) );
		representTable.insert( std::pair< T, T >(x1,x0) );
	}

	void convertKey(T x0,T x1){
		if( x0 == x1 ){
			return;
		}else if( x0 < x1 ){
			T y = x1;
			x1 = x0;
			x0 = y;
		}

		std::pair< typename std::multimap<T,T>::iterator, typename std::multimap<T,T>::iterator >
				tIterPair = eqTable.equal_range( x0 );
		typename std::multimap<T,T>::iterator tIter = tIterPair.first;
		while( tIter != tIterPair.second ){
			eqTable.insert( std::pair< T, T >(x1,tIter->second) );
			representTable[tIter->second] = x1;
			eqTable.erase( tIter++ );
		}
	}
public:
	virtual void clear(void){
		eqTable.clear();
		representTable.clear();
		classCount = 0;
	}

	virtual bool add(T x){
		if( representTable.find(x) == representTable.end() ){
			addPair( x, x );
			++classCount;
			return true;
		}else{
			return false;
		}
	}


	virtual bool equivalent(T x0,T x1){
		if( x0 == x1 ){
			return add(x0);
		}
		if( x1 < x0 ){
			T x = x0;
			x0 = x1;
			x1 = x;
		}

		typename std::map<T,T>::iterator r0Iter = representTable.find(x0);
		typename std::map<T,T>::iterator r1Iter = representTable.find(x1);

		if( r0Iter == representTable.end() && r1Iter == representTable.end() ){
			addPair( x0, x1 );
			addPair( x0, x0 );
			++classCount;
		}else

		if( r0Iter != representTable.end() && r1Iter == representTable.end() ){
			T x00 = r0Iter->second;
			addPair( x00, x1 );
		}else

		if( r0Iter == representTable.end() && r1Iter != representTable.end() ){
			T x10 = r1Iter->second;
			if( x10 < x0 ){
				addPair( x10, x0 );
			}else{
				convertKey( x10, x0 );
				addPair( x0, x0 );
			}
		}else

		if( r0Iter->second != r1Iter->second ){
			T x00 = r0Iter->second;
			T x10 = r1Iter->second;
			convertKey( x10, x00 );
			--classCount;
		}
		return true;
	}

	virtual bool isEquivalent(T x0,T x1) const{
		typename std::map<T,T>::const_iterator r0Iter = representTable.find(x0);
		typename std::map<T,T>::const_iterator r1Iter = representTable.find(x1);
		return ( r0Iter != representTable.end() && r1Iter != representTable.end() && r0Iter->second == r1Iter->second );
	}

	virtual T getClassId(T x) const{
		typename std::map<T,T>::const_iterator rIter = representTable.find(x);
		if( rIter != representTable.end() ){
			return rIter->second;
		}else{
			return x;
		}
	}
public:

	virtual iterator begin(void) const{
		typename Classification<T>::iterator it;
		it.cIter = this->eqTable.begin();
		it.endIter = this->eqTable.end();
		return it;
	}
	const iterator& end(void) const{
		typename Classification<T>::iterator it;
		it.cIter = this->eqTable.end();
		it.endIter = this->eqTable.end();
		return it;
	}



	virtual iterator begin(T x) const{
		T x0 = getClassId(x);
		typename Classification<T>::iterator it;
		it.cIter = this->eqTable.lower_bound(x0);
		it.endIter = this->eqTable.end();
		return it;
	}
	virtual iterator end(T x) const{
		T x0 = getClassId(x);
		typename Classification<T>::iterator it;
		it.cIter = this->eqTable.upper_bound(x0);
		it.endIter = this->eqTable.end();
		return it;
	}

	virtual bool first(T &x) const{
		typename std::multimap<T,T>::const_iterator
			cIter = this->eqTable.begin();
		if( cIter != this->eqTable.end() ){
			x = cIter->first;
			return true;
		}else{
			return false;
		}
	}
	virtual bool next(T &x) const{
		typename std::multimap<T,T>::const_iterator cIter = eqTable.upper_bound(x);
		if( cIter != eqTable.end() ){
			x = cIter->first;
			return true;
		}else{
			return false;
		}
	}
	virtual bool last(T &x) const{
		typename std::multimap<T,T>::const_reverse_iterator cIter = eqTable.rbegin();
		if( cIter != eqTable.rend() ){
			x = cIter->first;
			return true;
		}else{
			return false;
		}
	}
	virtual size_t getClassCount(void) const{
		return classCount;
	}
	virtual size_t getCount(T x) const{
		return static_cast<size_t>( eqTable.count(x) );
	}
};

}
