/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Bucket                                                  #
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
//
// 空間を格子状に分割して、それぞれの領域に含まれている節点・要素の情報を取得する
// 保存するのは Point3DContainer または ElementContainer で保存されているものの ID
// ある節点または要素が複数の格子の領域に含まれていることは許すために multimap を使う。
//
// T は nodeIdType elementIdType など
//
// Bucket<T>::iterator で T の型のイテレータが得られる
//

#pragma once

#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbBoxRegion.h"

#include <map>
#include <vector>
#include <cmath>
#include <cstddef>

namespace kmb{

// (i,j,k) をまとめて扱うためのクラス
// 大小関係は i,j,k の順に優先
// 大小関係で並べたループは
//	for(i){
//		for(j){
//			for(k){
//			}
//		}
//	}
//	と同じ
class BucketIndex{
public:
	int i,j,k;
	BucketIndex() : i(0), j(0), k(0) {}
	BucketIndex(int i0,int j0,int k0) : i(i0), j(j0), k(k0) {}
	void set(int i0,int j0,int k0){
		this->i = i0;
		this->j = j0;
		this->k = k0;
	}
	bool operator<(const BucketIndex &other)const{
		return
			(i < other.i) ||
			(i==other.i && j < other.j) ||
			(i==other.i && j == other.j && k < other.k);
	}
	bool operator==(const BucketIndex &other)const{
		return i==other.i && j == other.j && k == other.k;
	}
	BucketIndex& operator=(const BucketIndex& other){
		this->i = other.i;
		this->j = other.j;
		this->k = other.k;
		return *this;
	}
	// next したときに最初の index になるようにする
	void init(void){
		set(0,0,-1);
	}
	void init(const BucketIndex& minIndex){
		set(minIndex.i,minIndex.j,minIndex.k-1);
	}
	void init_kji(void){
		set(-1,0,0);
	}
	void init_kji(const BucketIndex& minIndex){
		set(minIndex.i-1,minIndex.j,minIndex.k);
	}
	// ループの外側から i j k
	// k から順に増やす operator < の順と同じ
	bool next(const BucketIndex& maxIndex){
		if( k < maxIndex.k-1 ){
			++k;
			return true;
		}else if( j < maxIndex.j-1 ){
			k = 0;
			++j;
			return true;
		}else if( i < maxIndex.i-1 ){
			k = 0;
			j = 0;
			++i;
			return true;
		}else{
			++k;
			return false;
		}
	}
	// ループの外側から i j k
	// k から順に増やす operator < の順と同じ
	bool next(const BucketIndex& maxIndex,const BucketIndex& minIndex){
		if( k < maxIndex.k-1 ){
			++k;
			return true;
		}else if( j < maxIndex.j-1 ){
			k = minIndex.k;
			++j;
			return true;
		}else if( i < maxIndex.i-1 ){
			k = minIndex.k;
			j = minIndex.j;
			++i;
			return true;
		}else{
			++k;
			return false;
		}
	}
	// ループの外側から k j i 
	// i から順に増やす operator < の順と違う
	bool next_kji(const BucketIndex& maxIndex){
		if( i < maxIndex.i-1 ){
			++i;
			return true;
		}else if( j < maxIndex.j-1 ){
			i = 0;
			++j;
			return true;
		}else if( k < maxIndex.k-1 ){
			i = 0;
			j = 0;
			++k;
			return true;
		}else{
			++i;
			return false;
		}
	}
	// ループの外側から k j i 
	// i から順に増やす operator < の順と違う
	bool next_kji(const BucketIndex& maxIndex,const BucketIndex& minIndex){
		if( i < maxIndex.i-1 ){
			++i;
			return true;
		}else if( j < maxIndex.j-1 ){
			i = minIndex.i;
			++j;
			return true;
		}else if( k < maxIndex.k-1 ){
			i = minIndex.i;
			j = minIndex.j;
			++k;
			return true;
		}else{
			++i;
			return false;
		}
	}
	bool valid(const BucketIndex& maxIndex){
		return
			( 0 <= k && k < maxIndex.k ) &&
			( 0 <= j && j < maxIndex.j ) &&
			( 0 <= i && i < maxIndex.i );
	}
	bool valid(const BucketIndex& maxIndex,const BucketIndex& minIndex){
		return
			( minIndex.k <= k && k < maxIndex.k ) &&
			( minIndex.j <= j && j < maxIndex.j ) &&
			( minIndex.i <= i && i < maxIndex.i );
	}
};

template <typename T,typename indexType=int>
class Bucket
{
protected:
	std::multimap<indexType,T> regions;
	kmb::BoxRegion bucketRegion;
	int xnum,ynum,znum;
public:
	Bucket(void): xnum(0), ynum(0), znum(0){};
	Bucket(const kmb::BoxRegion &box,int xnum,int ynum,int znum)
	 : bucketRegion(box), xnum(xnum), ynum(ynum), znum(znum){};
	virtual ~Bucket(void){};

	// 格納されているデータのクリア
	// 背景格子の情報は残る
	void clear(void){
		regions.clear();
	}

	void setRegion(const kmb::BoxRegion &box){
		this->bucketRegion = box;
	};

	void getRegion(kmb::BoxRegion &box) const{
		box = this->bucketRegion;
	};

	void setGridSize(int numX, int numY, int numZ){
		this->xnum = numX;
		this->ynum = numY;
		this->znum = numZ;
	};

	void getGridSize(int& numX, int& numY, int& numZ) const{
		numX = this->xnum;
		numY = this->ynum;
		numZ = this->znum;
	};

	// 全体が num 程度になるように x y z を配分して Grid を作る
	void setRegionGrid(const kmb::BoxRegion &box, size_t num){
		if( num == 0 ){ num = 1; }
		this->bucketRegion = box;
		double unit = pow( bucketRegion.getVolume() / num, 1.0/3.0);
		this->xnum = static_cast<int>( bucketRegion.rangeX() / unit );
		this->ynum = static_cast<int>( bucketRegion.rangeY() / unit );
		this->znum = static_cast<int>( bucketRegion.rangeZ() / unit );
		if( this->xnum <= 0 ){ this->xnum = 1; }
		if( this->ynum <= 0 ){ this->ynum = 1; }
		if( this->znum <= 0 ){ this->znum = 1; }
	};

	int getSize(void) const{
		return this->xnum * this->ynum * this->znum;
	};

	size_t getCount(void) const{
		return regions.size();
	};

	size_t getCount(int i,int j,int k) const{
		return regions.count(getIndex(i,j,k));
	};

	bool insert(int i,int j,int k,const T &t){
		regions.insert( std::pair<indexType,T>( getIndex(i,j,k), t ) );
		return true;
	}

	// (i,j,k) 番目の格子を取得する
	void getSubRegion(int i,int j,int k,kmb::BoxRegion& box) const{
		box.setMinMax(
			bucketRegion.minX() + (i * bucketRegion.rangeX()) / xnum,
			bucketRegion.minY() + (j * bucketRegion.rangeY()) / ynum,
			bucketRegion.minZ() + (k * bucketRegion.rangeZ()) / znum,
			bucketRegion.minX() + ((i+1) * bucketRegion.rangeX()) / xnum,
			bucketRegion.minY() + ((j+1) * bucketRegion.rangeY()) / ynum,
			bucketRegion.minZ() + ((k+1) * bucketRegion.rangeZ()) / znum);
	};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100) // 使わない引数があっても警告を出さない for VC
#endif
#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869) // 使わない引数があっても警告を出さない for intel
#endif
	double getSubRegionMinX(int i,int j,int k) const{
		return bucketRegion.minX() + (i * bucketRegion.rangeX()) / xnum;
	};
	double getSubRegionMinY(int i,int j,int k) const{
		return bucketRegion.minY() + (j * bucketRegion.rangeY()) / ynum;
	};
	double getSubRegionMinZ(int i,int j,int k) const{
		return bucketRegion.minZ() + (k * bucketRegion.rangeZ()) / znum;
	};
	double getSubRegionMaxX(int i,int j,int k) const{
		return bucketRegion.minX() + ((i+1) * bucketRegion.rangeX()) / xnum;
	};
	double getSubRegionMaxY(int i,int j,int k) const{
		return bucketRegion.minY() + ((j+1) * bucketRegion.rangeY()) / ynum;
	};
	double getSubRegionMaxZ(int i,int j,int k) const{
		return bucketRegion.minZ() + ((k+1) * bucketRegion.rangeZ()) / znum;
	};
#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

	bool getSubRegion(int index,kmb::BoxRegion& box) const{
		if( index < 0 || index >= xnum*ynum*znum ){
			return false;
		}
		int i = index / (ynum*znum);
		int j = (index - i * (ynum*znum)) / znum;
		int k = index - i * (ynum*znum) - j * znum;
		getSubRegion(i,j,k,box);
		return true;
	}

	indexType getIndex(double x,double y,double z) const
	{
		int i,j,k;
		if( getSubIndices(x,y,z,i,j,k) ){
			return ( i * ynum + j ) * znum + k;
		}else{
			return -1;
		}
/*
		if( bucketRegion.intersect(x,y,z) == kmb::BoxRegion::OUTSIDE ){
			return -1;
		}
		indexType i = static_cast<indexType>( (x-bucketRegion.minX()) / bucketRegion.rangeX() * xnum );
		indexType j = static_cast<indexType>( (y-bucketRegion.minY()) / bucketRegion.rangeY() * ynum );
		indexType k = static_cast<indexType>( (z-bucketRegion.minZ()) / bucketRegion.rangeZ() * znum );
		// maxX maxY maxZ のところだけ特別対応
		if( x == bucketRegion.maxX() ){
			i = xnum-1;
		}
		if( y == bucketRegion.maxY() ){
			j = ynum-1;
		}
		if( z == bucketRegion.maxZ() ){
			k = znum-1;
		}
		if( 0 <= i && i < xnum && 0 <= j && j < ynum && 0 <= k && k < znum ){
			return ( i * ynum + j ) * znum + k;
		}else{
			return -1;
		}
*/
	};

	indexType getIndex(int i,int j,int k) const{
		return ( static_cast<indexType>(i) * static_cast<indexType>(ynum) + static_cast<indexType>(j) ) * static_cast<indexType>(znum) + static_cast<indexType>(k);
	};

	class iterator
	{
		friend class Bucket<T>;
	protected:
		typename std::multimap<int,T>::iterator it;
	public:
		iterator(void){};
		virtual ~iterator(void){};

		// 格納している T 型の参照を返す
		const T& get(void) const{ return it->second; };

		int getIndex() const{ return it->first; };

		void getIndices(int &i,int &j,int &k) const{
			i = it->first / (ynum*znum);
			j = (it->first - i*ynum*znum) / znum;
			k = it->first - i*ynum*znum - j*znum;
		};

		iterator& operator++(void){ ++it; return *this; };

		iterator  operator++(int n){
			typename kmb::Bucket<T>::iterator itClone;
			itClone.it = it;
			it++;
			return itClone;
		};

		bool operator==(const iterator& other) const{
			return this->it == other.it;
		};

		bool operator!=(const iterator& other) const{
			return this->it != other.it;
		};

		iterator& operator=(const iterator& other){
			it = other.it;
			return *this;
		};

	};

	class const_iterator
	{
		friend class Bucket<T>;
	protected:
		typename std::multimap<int,T>::const_iterator it;
	public:
		const_iterator(void){}
		virtual ~const_iterator(void){}

		const T& get(void) const{ return it->second; };

		int getIndex() const{ return it->first; };

		void getIndices(int &i,int &j,int &k) const{
			i = it->first / (ynum*znum);
			j = (it->first - i*ynum*znum) / znum;
			k = it->first - i*ynum*znum - j*znum;
		};

		const_iterator& operator++(void){ ++it; return *this; };

		const_iterator operator++(int n){
			typename kmb::Bucket<T>::iterator itClone;
			itClone.it = it;
			it++;
			return itClone;
		};

		bool operator==(const const_iterator& other) const{
			return this->it == other.it;
		};

		bool operator!=(const const_iterator& other) const{
			return this->it != other.it;
		};

		const_iterator& operator=(const const_iterator& other){
			it = other.it;
			return *this;
		};

		const_iterator& operator=(const iterator& other){
			it = other.it;
			return *this;
		};
	};

	iterator begin(int i,int j,int k){
		typename kmb::Bucket<T>::iterator itClone;
		itClone.it = regions.lower_bound(getIndex(i,j,k));
		return itClone;
	};

	const_iterator begin(int i,int j,int k) const{
		typename kmb::Bucket<T>::const_iterator itClone;
		itClone.it = regions.lower_bound(getIndex(i,j,k));
		return itClone;
	};

	iterator end(int i,int j,int k){
		typename kmb::Bucket<T>::iterator itClone;
		itClone.it = regions.upper_bound(getIndex(i,j,k));
		return itClone;
	};

	const_iterator end(int i,int j,int k) const{
		typename kmb::Bucket<T>::const_iterator itClone;
		itClone.it = regions.upper_bound(getIndex(i,j,k));
		return itClone;
	};

	iterator begin(void){
		typename kmb::Bucket<T>::iterator itClone;
		itClone.it = regions.begin();
		return itClone;
	};

	const_iterator begin(void) const{
		typename kmb::Bucket<T>::const_iterator itClone;
		itClone.it = regions.begin();
		return itClone;
	};

	iterator end(void){
		typename kmb::Bucket<T>::iterator itClone;
		itClone.it = regions.end();
		return itClone;
	};

	const_iterator end(void) const{
		typename kmb::Bucket<T>::const_iterator itClone;
		itClone.it = regions.end();
		return itClone;
	};

protected:
	// 座標から index を取得する
	// 親 box の中にあれば true を返す
	// 親 box の外にあっても i j k は境界の値を入れて false を返す
	// x, y, z が無限大の場合は割り算で数値誤差になるので注意
	bool getSubIndices(double x,double y,double z,int& i,int& j,int& k) const
	{
		bool res = true;
		i = static_cast<int>( (x-bucketRegion.minX()) / bucketRegion.rangeX() * xnum );
		j = static_cast<int>( (y-bucketRegion.minY()) / bucketRegion.rangeY() * ynum );
		k = static_cast<int>( (z-bucketRegion.minZ()) / bucketRegion.rangeZ() * znum );
		// maxX maxY maxZ のところだけ特別対応
		if( x == bucketRegion.maxX() ){
			i = xnum-1;
		}
		if( y == bucketRegion.maxY() ){
			j = ynum-1;
		}
		if( z == bucketRegion.maxZ() ){
			k = znum-1;
		}
		if( i < 0 ){ i = 0; res = false; }
		if( j < 0 ){ j = 0; res = false; }
		if( k < 0 ){ k = 0; res = false; }
		if( i >= xnum ){ i = xnum-1; res = false; }
		if( j >= ynum ){ j = ynum-1; res = false; }
		if( k >= znum ){ k = znum-1; res = false; }
		return res;
	};
};

// Point3D についてのリファレンス実装

class Point3DContainer;

class Bucket3DGrid : public Bucket<kmb::nodeIdType>
{
private:
	const kmb::Point3DContainer* points;
public:
	Bucket3DGrid(void);
	Bucket3DGrid(const kmb::BoxRegion &box,int xnum,int ynum,int znum);
	virtual ~Bucket3DGrid(void);
	void setContainer(const kmb::Point3DContainer* points);

	int append(kmb::nodeIdType nodeId);
	int appendAll(void);

	// 距離を返す
	double getNearest(const double x,const double y,const double z,kmb::nodeIdType &nearestId) const;
	// 節点番号を与えて、thresh 以下の他の点があるかどうか
	size_t getNearPoints(kmb::nodeIdType nodeId,double thresh,std::vector<kmb::nodeIdType>& nodeIds) const;
protected:
	// (i,j,k) のバケットの中での最小値を探索
	// 距離を返す
	double getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,kmb::nodeIdType &nearestId) const;
	// (imin,jmin,kmin) から (imax,jmax,kmax) のバケットの中での最小値を探索
	// ただし (imin_ex,jmin_ex,kmin_ex) から (imax_ex,jmax_ex,kmax_ex) を除く
	double getNearestInBucket2(const kmb::Point3D& pt,
		int imin,int jmin,int kmin,int imax,int jmax,int kmax,
		int imin_ex,int jmin_ex,int kmin_ex,int imax_ex,int jmax_ex,int kmax_ex,
		kmb::nodeIdType &nearestId) const;
};

}
