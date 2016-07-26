/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Classification                                          #
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
#pragma once

// 分類用のユーティリティクラス
// 集合の同値関係を与えて極大同値類を返す
// もとの集合の要素の型は T






#include <set>
#include <map>

namespace kmb{

// T には不等号 < と比較 == が定義されているとする
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
	typename std::map<T,T> representTable;    // 要素 => 代表（最小要素）
	typename std::multimap<T,T> eqTable;   // 代表（最小要素） => 同値類の中身
	// x0 <= x1 が保証されているとき
	// x0 を代表元として x1 を登録する
	void addPair(T x0,T x1){
		eqTable.insert( std::pair< T, T >(x0,x1) );
		representTable.insert( std::pair< T, T >(x1,x0) );
	}
	// 代表元を小さい方に合わせる
	void convertKey(T x0,T x1){
		if( x0 == x1 ){
			return;
		}else if( x0 < x1 ){
			T y = x1;
			x1 = x0;
			x0 = y;
		}
		// x1 < x0 を仮定する
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
	// x を他とは同値でない要素として登録
	virtual bool add(T x){
		if( representTable.find(x) == representTable.end() ){
			addPair( x, x );
			++classCount;
			return true;
		}else{
			return false;
		}
	}
	// x0 と x1 を同値な要素として登録
	// 既に登録済みのときは、同値関係を追加
	virtual bool equivalent(T x0,T x1){
		if( x0 == x1 ){
			return add(x0);
		}
		if( x1 < x0 ){
			T x = x0;
			x0 = x1;
			x1 = x;
		}
		// 以後 x0 <= x1
		typename std::map<T,T>::iterator r0Iter = representTable.find(x0);
		typename std::map<T,T>::iterator r1Iter = representTable.find(x1);
		// どちらも登録されていないとき
		if( r0Iter == representTable.end() && r1Iter == representTable.end() ){
			addPair( x0, x1 );
			addPair( x0, x0 );
			++classCount;
		}else
		// x0 が登録済み
		if( r0Iter != representTable.end() && r1Iter == representTable.end() ){
			T x00 = r0Iter->second;
			addPair( x00, x1 );
		}else
		// x1 が登録済み
		if( r0Iter == representTable.end() && r1Iter != representTable.end() ){
			T x10 = r1Iter->second;
			if( x10 < x0 ){
				addPair( x10, x0 );
			}else{
				convertKey( x10, x0 );
				addPair( x0, x0 );
			}
		}else
		// 両方 が異なるクラスに登録済み
		if( r0Iter->second != r1Iter->second ){
			T x00 = r0Iter->second;
			T x10 = r1Iter->second;
			convertKey( x10, x00 );
			--classCount;
		}
		return true;
	}
	// t0 と t1 が同値かどうかを判定
	virtual bool isEquivalent(T x0,T x1) const{
		typename std::map<T,T>::const_iterator r0Iter = representTable.find(x0);
		typename std::map<T,T>::const_iterator r1Iter = representTable.find(x1);
		return ( r0Iter != representTable.end() && r1Iter != representTable.end() && r0Iter->second == r1Iter->second );
	}
	// 代表元
	virtual T getClassId(T x) const{
		typename std::map<T,T>::const_iterator rIter = representTable.find(x);
		if( rIter != representTable.end() ){
			return rIter->second;
		}else{
			return x;
		}
	}
public:
	// 全体のイテレータ
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
	// x を含む同値類のイテレータ
	// x が含まれないときは end() を返す
	// end(x) = begin( x の次の代表 ) と同じ
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
