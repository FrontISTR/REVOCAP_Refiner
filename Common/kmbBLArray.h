/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BLArray                                                 #
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
/*
 * 大きさ可変の配列を仮想的な二次元配列で実現したもの
 * Bi-Layerd Array
 *
 * SubArray は大きさが 2 のべきにする
 * subSize = 1<<bitlength = 2^bitlength
 *
 * n >= 1 なら要素が n 成分あると考える
 * 座標値を格納するときは BLArray<double,3> のようにする
 *
 */

#pragma once

#include <cstddef>

namespace kmb{

class BLArrayIndex;

class BLArrayBase
{
protected:
	static unsigned int MAX_BITLENGTH;
	unsigned int bitlength;
	unsigned int localbitmask;
	size_t topSize;
	size_t subSize;
public:
	BLArrayBase(void)
	: bitlength(0U)
	, localbitmask(0U)
	, topSize(0U)
	, subSize(0U)
	{
	}
	virtual ~BLArrayBase(void){
	}
	size_t getSize(void) const;
	size_t getTopSize(void) const;
	size_t getSubSize(void) const;
	BLArrayIndex getBLArrayIndex(size_t index) const;
	bool valid(const BLArrayIndex &index) const;
protected:
	BLArrayIndex getBLArrayIndex(size_t tIndex,size_t sIndex) const;
};

/**
 * BLArray の index
 * topIndex と subIndex を管理する
 */
class BLArrayIndex
{

	friend class BLArrayBase;
private:
	size_t topIndex;
	size_t subSize;
	size_t subIndex;
public:
	BLArrayIndex(void);
	BLArrayIndex(const BLArrayIndex &other);
	virtual ~BLArrayIndex(void);
	BLArrayIndex& operator=(const BLArrayIndex &other);
	bool operator<(const BLArrayIndex &other) const;
	bool operator<=(const BLArrayIndex &other) const;
	void clear(void);
	BLArrayIndex& operator++(void);
	BLArrayIndex operator++(int n);
	BLArrayIndex& operator--(void);
	BLArrayIndex operator--(int n);
	bool valid(void) const;
	size_t getIndex(void) const;
	size_t getSubIndex(void) const;
	size_t getTopIndex(void) const;
};

/* n 個の T を並べて格納するとき */
template<typename T,int n=1>
class BLArray : public BLArrayBase
{
protected:
	T** ary;
	T defval;
public:
	BLArray(T defval)
	: BLArrayBase()
	, ary(NULL)
	{
		this->defval = defval;
	}
	virtual ~BLArray(void){
		clear();
	}
	void clear(void){
		if( ary ){
			for(size_t i=0;i<topSize;++i){
				if( ary[i] != NULL ){
					delete[] ary[i];
					ary[i] = NULL;
				}
			}
			delete[] ary;
			ary = NULL;
			topSize = 0U;
			subSize = 0U;
			bitlength = 0U;
			localbitmask = 0U;
		}
	}
	void clearData(void){
		if( ary ){
			for(size_t i = 0;i<topSize;++i){
				if( ary[i] ){
					for(size_t j = 0;j<n*subSize;++j){
						ary[i][j] = defval;
					}
				}
			}
		}
	}





	bool initialize(size_t size,size_t tSize=1)
	{
		clear();
		if( tSize == 0 ){
			return false;
		}
		bitlength = 0U;
		while( size > (1U<<bitlength)*tSize && bitlength < MAX_BITLENGTH ){
			++bitlength;
		}
		subSize = 1U << bitlength;
		topSize = (size>>bitlength)+1;
		localbitmask = static_cast<unsigned int>(subSize-1);
		ary = new T*[ topSize ];
		for(size_t i = 0;i<topSize;++i){
			ary[i] = new T[ n * subSize ];
			for(size_t j = 0;j<n*subSize;++j){
				ary[i][j] = defval;
			}
		}
		return true;
	}
	bool set(size_t i,T *t)
	{
		return set(i>>bitlength,i&localbitmask,t);
	}
	bool set(const kmb::BLArrayIndex &index,T *t)
	{
		return set(index.getTopIndex(),index.getSubIndex(),t);
	}
	const T& operator()(const kmb::BLArrayIndex &index,int j=0){
		const size_t& tIndex = index.getTopIndex();
		const size_t& sIndex = index.getSubIndex();
		if( tIndex >= topSize ){
			return defval;
		}
		if( ary[tIndex] == NULL ){
			return defval;
		}
		return ary[tIndex][n*sIndex+j];
	}
	const T& operator()(size_t i,int j=0){
		const size_t& tIndex = i>>bitlength;
		const size_t& sIndex = i&localbitmask;
		if( tIndex >= topSize ){
			return defval;
		}
		if( ary[tIndex] == NULL ){
			return defval;
		}
		return ary[tIndex][n*sIndex+j];
	}
	bool get(size_t i,T *t) const
	{
		return get(i>>bitlength,i&localbitmask,t);
	}
	bool get(const kmb::BLArrayIndex &index,T *t) const
	{
		return get(index.getTopIndex(),index.getSubIndex(),t);
	}
	bool setComponent(size_t i,int j,T *t)
	{
		return setComponent(i>>bitlength,i&localbitmask,j,t);
	}
	bool setComponent(const kmb::BLArrayIndex &index,int j,T *t)
	{
		return setComponent(index.getTopIndex(),index.getSubIndex(),j,t);
	}
	bool getComponent(size_t i,int j,T *t) const
	{
		return getComponent(i>>bitlength,i&localbitmask,j,t);
	}
	bool getComponent(const kmb::BLArrayIndex &index,int j,T *t) const
	{
		return getComponent(index.getTopIndex(),index.getSubIndex(),j,t);
	}
	bool erase(size_t i)
	{
		return erase(i>>bitlength,i&localbitmask);
	}
	bool erase(const kmb::BLArrayIndex &index)
	{
		return erase(index.getTopIndex(),index.getSubIndex());
	}
	bool has(const BLArrayIndex &index) const
	{
		size_t tIndex = index.getTopIndex();
		size_t sIndex = index.getSubIndex();
		return
			tIndex < this->topSize &&
			sIndex < this->subSize &&
			ary[tIndex] != NULL &&
			ary[tIndex][n*sIndex] != defval;
	}
	bool has(size_t i) const
	{
		size_t tIndex = i>>bitlength;
		size_t sIndex = i&localbitmask;
		return
			tIndex < this->topSize &&
			sIndex < this->subSize &&
			ary[tIndex] != NULL &&
			ary[tIndex][n*sIndex] != defval;
	}

	bool first(BLArrayIndex &index) const
	{
		for(size_t i = 0;i<topSize;++i){
			if( ary[i] != NULL ){
				for(size_t j = 0;j<subSize;++j){

					bool flag = true;
					for(int k=0;k<n;++k){
						flag &= ( ary[i][n*j+k] != defval );
					}
					if( flag ){
						index = getBLArrayIndex(i,j);
						return true;
					}
				}
			}
		}
		return false;
	}
protected:
	bool set(size_t tIndex,size_t sIndex,T *t)
	{
		if( tIndex >= topSize ){
			increaseSubArray( tIndex + 1 );
		}
		if( ary[tIndex] == NULL ){
			initializeSubArray(tIndex);
		}
		for(size_t j=0;j<n;++j){
			ary[tIndex][n*sIndex+j] = t[j];
		}
		return true;
	}
	bool get(size_t tIndex,size_t sIndex,T *t) const
	{
		if( tIndex >= topSize ){
			return false;
		}
		if( ary[tIndex] == NULL ){
			return false;
		}
		for(size_t j=0;j<n;++j){
			t[j] = ary[tIndex][n*sIndex+j];
		}
		return true;
	}
	bool setComponent(size_t tIndex,size_t sIndex,int index,T *t)
	{
		if( tIndex >= topSize ){
			increaseSubArray( tIndex + 1 );
		}
		if( ary[tIndex] == NULL ){
			initializeSubArray(tIndex);
		}
		ary[tIndex][n*sIndex+index] = *t;
		return true;
	}
	bool getComponent(size_t tIndex,size_t sIndex,int index,T *t) const
	{
		if( tIndex >= topSize ){
			return false;
		}
		if( ary[tIndex] == NULL ){
			return false;
		}
		*t = ary[tIndex][n*sIndex+index];
		return true;
	}
	bool erase(size_t tIndex,size_t sIndex)
	{
		if( tIndex >= topSize ){
			return false;
		}
		if( ary[tIndex] == NULL ){
			return false;
		}
		for(size_t j=0;j<n;++j){
			ary[tIndex][n*sIndex+j] = defval;
		}
		return true;
	}

	bool increaseSubArray(size_t tSize){
		if( tSize > topSize ){

			T** temp = ary;

			ary = new T*[ tSize ];
			for(size_t i = 0;i<topSize;++i){
				ary[i] = temp[i];
			}
			for(size_t i = topSize;i<tSize;++i){
				ary[i] = NULL;
			}






			delete[] temp;
			topSize = tSize;
			return true;
		}else{
			return false;
		}
	}
	bool initializeSubArray(size_t tIndex){
		if( ary[tIndex] == NULL ){
			ary[tIndex] = new T[ n * subSize ];
			for(size_t j = 0;j<n*subSize;++j){
				ary[tIndex][j] = defval;
			}
			return true;
		}
		return false;
	}
};

/* T のポインタを保存する時 */
template<typename T>
class BLArrayPtr : public BLArrayBase
{
protected:
	T*** ary;
public:
	BLArrayPtr(void)
	: BLArrayBase()
	, ary(NULL)
	{
	}
	virtual ~BLArrayPtr(void){
		clear();
	}
	void clear(void){
		if( ary ){
			for(size_t i=0;i<topSize;++i){
				if( ary[i] != NULL ){
					for(size_t j=0;j<subSize;++j){
						if( ary[i][j] != NULL ){
							delete ary[i][j];
							ary[i][j] = NULL;
						}
					}
					delete[] ary[i];
					ary[i] = NULL;
				}
			}
			delete[] ary;
			ary = NULL;
			topSize = 0U;
			subSize = 0U;
			bitlength = 0U;
			localbitmask = 0U;
		}
	}


	void clearData(void){
		if( ary ){
			for(size_t i = 0;i<topSize;++i){
				if( ary[i] ){
					for(size_t j = 0;j<subSize;++j){
						ary[i][j] = NULL;
					}
				}
			}
		}
	}
	bool initialize(size_t size,size_t tSize=1)
	{
		clear();
		if( tSize == 0 ){
			return false;
		}
		bitlength = 0U;
		while( size > (1U<<bitlength)*tSize && bitlength < MAX_BITLENGTH ){
			++bitlength;
		}
		subSize = 1U << bitlength;
		topSize = (size>>bitlength)+1;
		localbitmask = static_cast<unsigned int>(subSize-1);
		ary = new T**[ topSize ];
		for(size_t i = 0;i<topSize;++i){
			ary[i] = new T*[ subSize ];
			for(size_t j = 0;j<subSize;++j){
				ary[i][j] = NULL;
			}
		}
		return true;
	}
	bool set(size_t i,T *t)
	{
		return set(i>>bitlength,i&localbitmask,t);
	}
	bool set(kmb::BLArrayIndex &index,T *t)
	{
		return set(index.getTopIndex(),index.getSubIndex(),t);
	}
	T* get(size_t i) const
	{
		return get(i>>bitlength,i&localbitmask);
	}
	T* get(const kmb::BLArrayIndex &index) const
	{
		return get(index.getTopIndex(),index.getSubIndex());
	}
	bool has(const BLArrayIndex &index) const
	{
		size_t tIndex = index.getTopIndex();
		size_t sIndex = index.getSubIndex();
		return
			tIndex < this->topSize &&
			sIndex < this->subSize &&
			ary[tIndex] != NULL &&
			ary[tIndex][sIndex] != NULL;
	}

	bool first(BLArrayIndex &index) const
	{
		for(size_t i = 0;i<topSize;++i){
			if( ary[i] != NULL ){
				for(size_t j = 0;j<subSize;++j){
					if( ary[i][j] != NULL ){
						index = getBLArrayIndex(i,j);
						return true;
					}
				}
			}
		}
		return false;
	}
protected:
	bool set(size_t tIndex,size_t sIndex,T* t)
	{
		if( tIndex >= topSize ){
			increaseSubArray( tIndex + 1 );
		}
		if( ary[tIndex] == NULL ){
			initializeSubArray(tIndex);
		}
		ary[tIndex][sIndex] = t;
		return true;
	}
	T* get(size_t tIndex,size_t sIndex) const
	{
		if( tIndex >= topSize ){
			return NULL;
		}
		if( ary[tIndex] == NULL ){
			return NULL;
		}
		return ary[tIndex][sIndex];
	}

	bool increaseSubArray(size_t tSize){
		if( tSize > topSize ){
			T*** temp = ary;
			ary = new T**[ tSize ];
			for(size_t i = 0;i<topSize;++i){
				ary[i] = temp[i];
			}
			for(size_t i = topSize;i<tSize;++i){
				ary[i] = NULL;
			}






			delete[] temp;
			topSize = tSize;
			return true;
		}else{
			return false;
		}
	}
	bool initializeSubArray(size_t tIndex){
		if( ary[tIndex] == NULL ){
			ary[tIndex] = new T*[ subSize ];
			for(size_t j = 0;j<subSize;++j){
				ary[tIndex][j] = NULL;
			}
			return true;
		}
		return false;
	}
};

}
