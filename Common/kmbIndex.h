/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.7                          #
# Program Name : Index3                                                #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/10/24     #
#                                                                      #
#      Copyright (C) 2013 Advancesoft corp.                            #
#                                                                      #
----------------------------------------------------------------------*/

#pragma once

#include "Common/kmbCommon.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

namespace kmb{

enum orderType{
	kNormalOrder = 0,
	kReverseOrder = 1,
	kIJKOrder = 0,
	kKJIOrder = 1,
	kRowMajor = 0,
	kColumnMajor = 1
};

class Index3{
public:
	static orderType getOrderType(void){
		return kNormalOrder;
	}
	int i;
	int j;
	int k;
	Index3(int _i,int _j,int _k) : i(_i), j(_j), k(_k){}
	bool operator<(const Index3 &other)const{
		return
			(i < other.i) ||
			(i==other.i && j < other.j) ||
			(i==other.i && j == other.j && k < other.k);
	}
	// k j i の順に増やしていく
	// operator < の順序と同じ
	// ループは外から i j k
	bool next(int nx,int ny,int nz){
		if( k < nz-1 ){
			++k;
			return true;
		}else if( j < ny-1 ){
			k = 0;
			++j;
			return true;
		}else if( i < nx-1 ){
			k = 0;
			j = 0;
			++i;
			return true;
		}else{
			i = nx;
			j = ny;
			k = nz;
			return false;
		}
	}
	bool next(Index3 &end){
		if( k < end.k-1 ){
			++k;
			return true;
		}else if( j < end.j-1 ){
			k = 0;
			++j;
			return true;
		}else if( i < end.i-1 ){
			k = 0;
			j = 0;
			++i;
			return true;
		}else{
			i = end.i;
			j = end.j;
			k = end.k;
			return false;
		}
	}
	int operator()(int nx,int ny,int nz) const{
		return (i*(ny) + j)*(nz) + k;
	}
	void init(void){
		i = 0;
		j = 0;
		k = 0;
	}
	bool set(int n,int nx,int ny,int nz){
		if( n < 0 || n >= nx*ny*nz ){
			i = -1;
			j = -1;
			k = -1;
			return false;
		}
		i = n / (ny*nz);
		int q = n - i * ny * nz;
		j = q / nz;
		k = q - j * nz;
		return true;
	}
	bool valid(int nx,int ny,int nz) const{
		return (0 <= i) && (i < nx) && (0 <= j) && (j < ny) && (0 <= k) && (k < nz);
	}
	bool valid(Index3 &end) const{
		return (0 <= i) && (i < end.i) && (0 <= j) && (j < end.j) && (0 <= k) && (k < end.k);
	}
};

class Index3R{
public:
	static orderType getOrderType(void){
		return kReverseOrder;
	}
	int i;
	int j;
	int k;
	Index3R(int _i,int _j,int _k) : i(_i), j(_j), k(_k){}
	bool operator<(const Index3R &other)const{
		return
			(k < other.k) ||
			(k==other.k && j < other.j) ||
			(k==other.k && j == other.j && i < other.i);
	}
	// i j k の順に増やしていく
	// operator < の順序と同じ
	// ループは外から k j i
	bool next(int nx,int ny,int nz){
		if( i < nx-1 ){
			++i;
			return true;
		}else if( j < ny-1 ){
			i = 0;
			++j;
			return true;
		}else if( k < nz-1 ){
			i = 0;
			j = 0;
			++k;
			return true;
		}else{
			i = nx;
			j = ny;
			k = nz;
			return false;
		}
	}
	bool next(Index3R &end){
		if( i < end.i-1 ){
			++i;
			return true;
		}else if( j < end.j-1 ){
			i = 0;
			++j;
			return true;
		}else if( k < end.k-1 ){
			i = 0;
			j = 0;
			++k;
			return true;
		}else{
			i = end.i;
			j = end.j;
			k = end.k;
			return false;
		}
	}
	int operator()(int nx,int ny,int nz) const{
		return (k*(ny) + j)*(nx) + i;
	}
	void init(void){
		i = 0;
		j = 0;
		k = 0;
	}
	bool set(int n,int nx,int ny,int nz){
		if( n < 0 || n >= nx*ny*nz ){
			i = -1;
			j = -1;
			k = -1;
			return false;;
		}
		k = n / (nx*ny);
		int q = n - k * nx * ny;
		j = q / nx;
		i = q - j * nx;
		return true;
	}
	bool valid(int nx,int ny,int nz) const{
		return (0 <= i) && (i < nx) && (0 <= j) && (j < ny) && (0 <= k) && (k < nz);
	}
	bool valid(Index3R &end) const{
		return (0 <= i) && (i < end.i) && (0 <= j) && (j < end.j) && (0 <= k) && (k < end.k);
	}
};

class Index2{
public:
	static orderType getOrderType(void){
		return kNormalOrder;
	}
	int i;
	int j;
	Index2(int _i,int _j) : i(_i), j(_j){}
	bool operator<(const Index2 &other)const{
		return
			(i < other.i) ||
			(i==other.i && j < other.j);
	}
	// j i の順に増やしていく
	// operator < の順序と同じ
	// ループは外から i j
	bool next(int nx,int ny){
		if( j < ny-1 ){
			++j;
			return true;
		}else if( i < nx-1 ){
			j = 0;
			++i;
			return true;
		}else{
			i = nx;
			j = ny;
			return false;
		}
	}
	bool next(Index2 &end){
		if( j < end.j-1 ){
			++j;
			return true;
		}else if( i < end.i-1 ){
			j = 0;
			++i;
			return true;
		}else{
			i = end.i;
			j = end.j;
			return false;
		}
	}
	int operator()(int nx,int ny) const{
		return i*(ny) + j;
	}
	void init(void){
		i = 0;
		j = 0;
	}
	bool set(int n,int nx,int ny){
		if( n < 0 || n >= nx*ny ){
			i = -1;
			j = -1;
			return false;
		}
		i = n / ny;
		j = n - i * ny;
		return true;
	}
	bool valid(int nx,int ny) const{
		return (0 <= i) && (i < nx) && (0 <= j) && (j < ny);
	}
	bool valid(Index2 &end) const{
		return (0 <= i) && (i < end.i) && (0 <= j) && (j < end.j);
	}
};

template<typename T,orderType o=kNormalOrder>
class Array3{
protected:
	int nx,ny,nz;
	T* values;
public:
	Array3(int _nx,int _ny,int _nz)
	: nx(_nx), ny(_ny), nz(_nz), values(new T[nx*ny*nz]){
	}
	~Array3(void){
		if( values ){
			delete[] values;
		}
		values = NULL;
	}
	typedef Index3 indexType;
	void set(const indexType &ind,int k,T t){
		values[ ind(nx,ny,nz) ] = t;
	}
	void set(int i,int j,int k,T t){
		values[ indexType(i,j,k)(nx,ny,nz) ] = t;
	}
	void get(const indexType &ind,T &t) const{
		t = values[ ind(nx,ny,nz) ];
	}
	void get(int i,int j,int k,T &t) const{
		t = values[ indexType(i,j,k)(nx,ny,nz) ];
	}
	int getIndex(int i,int j,int k){
		return indexType(i,j,k)(nx,ny,nz);
	}
};

template<typename T>
class Array3<T,kmb::kReverseOrder>{
protected:
	int nx,ny,nz;
	T* values;
public:
	Array3(int _nx,int _ny,int _nz)
	: nx(_nx), ny(_ny), nz(_nz), values(new T[nx*ny*nz]){
	}
	~Array3(void){
		if( values ){
			delete[] values;
		}
		values = NULL;
	}
	void set(int i,int j,int k,T t){
		values[ indexType(i,j,k)(nx,ny,nz) ] = t;
	}
	void get(int i,int j,int k,T &t) const{
		t = values[ indexType(i,j,k)(nx,ny,nz) ];
	}
	typedef Index3R indexType;
	int getIndex(int i,int j,int k){
		return indexType(i,j,k)(nx,ny,nz);
	}
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
