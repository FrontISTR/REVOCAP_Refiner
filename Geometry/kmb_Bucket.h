/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Bucket                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/










#pragma once

#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_BoxRegion.h"

#include <map>
#include <cmath>

namespace kmb{

template <typename T>
class Bucket
{
protected:
	std::multimap<int,T> regions;
	kmb::BoxRegion bbox;
	int xnum,ynum,znum;
public:
	Bucket(void): xnum(0), ynum(0), znum(0){};
	Bucket(const kmb::BoxRegion &box,int xnum,int ynum,int znum)
	 : bbox(box), xnum(xnum), ynum(ynum), znum(znum){};
	virtual ~Bucket(void){};



	void clear(void){
		regions.clear();
	}

	void setRegion(const kmb::BoxRegion &box){
		this->bbox = box;
	};

	void getRegion(kmb::BoxRegion &box) const{
		box = this->bbox;
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


	void setRegionGrid(const kmb::BoxRegion &box, size_t num){
		if( num == 0 ){ num = 1; }
		this->bbox = box;
		double unit = pow( bbox.getVolume() / num, 1.0/3.0);
		this->xnum = static_cast<int>( bbox.rangeX() /unit );
		this->ynum = static_cast<int>( bbox.rangeY() /unit );
		this->znum = static_cast<int>( bbox.rangeZ() /unit );
		if( this->xnum <= 0 ){ this->xnum = 1; }
		if( this->ynum <= 0 ){ this->ynum = 1; }
		if( this->znum <= 0 ){ this->znum = 1; }
	};

	void getSize(void) const{
		return this->xnum * this->ynum * this->znum;
	};

	size_t getCount(void) const{
		return regions.size();
	};

	size_t getCount(int i,int j,int k) const{
		return regions.count(getIndex(i,j,k));
	};

	bool insert(int i,int j,int k,const T &t){
		regions.insert( std::pair<int,T>( getIndex(i,j,k), t ) );
		return true;
	}


	void getSubRegion(int i,int j,int k,kmb::BoxRegion& box) const{
		box.setMinMax(
			bbox.minX() + (i * bbox.rangeX()) / xnum,
			bbox.minY() + (j * bbox.rangeY()) / ynum,
			bbox.minZ() + (k * bbox.rangeZ()) / znum,
			bbox.minX() + ((i+1) * bbox.rangeX()) / xnum,
			bbox.minY() + ((j+1) * bbox.rangeY()) / ynum,
			bbox.minZ() + ((k+1) * bbox.rangeZ()) / znum);
	};

	double getSubRegionMinX(int i,int j,int k) const{
		return bbox.minX() + (i * bbox.rangeX()) / xnum;
	};
	double getSubRegionMinY(int i,int j,int k) const{
		return bbox.minY() + (j * bbox.rangeY()) / ynum;
	};
	double getSubRegionMinZ(int i,int j,int k) const{
		return bbox.minZ() + (k * bbox.rangeZ()) / znum;
	};
	double getSubRegionMaxX(int i,int j,int k) const{
		return bbox.minX() + ((i+1) * bbox.rangeX()) / xnum;
	};
	double getSubRegionMaxY(int i,int j,int k) const{
		return bbox.minY() + ((j+1) * bbox.rangeY()) / ynum;
	};
	double getSubRegionMaxZ(int i,int j,int k) const{
		return bbox.minZ() + ((k+1) * bbox.rangeZ()) / znum;
	};

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

	int getIndex(double x,double y,double z) const
	{
		if( bbox.intersect(x,y,z) == kmb::BoxRegion::OUTSIDE ){
			return -1;
		}
		int i = static_cast<int>( (x-bbox.minX()) / bbox.rangeX() * xnum );
		int j = static_cast<int>( (y-bbox.minY()) / bbox.rangeY() * ynum );
		int k = static_cast<int>( (z-bbox.minZ()) / bbox.rangeZ() * znum );
		if( 0 <= i && i < xnum && 0 <= j && j < ynum && 0 <= k && k < znum ){
			return ( i * ynum + j ) * znum + k;
		}else{
			return -1;
		}
	};

	int getIndex(int i,int j,int k) const{
		return ( i * ynum + j ) * znum + k;
	};

	class iterator
	{
		friend class Bucket<T>;
	protected:
		typename std::multimap<int,T>::iterator it;
	public:
		iterator(void){};
		virtual ~iterator(void){};

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




	bool getSubIndices(double x,double y,double z,int& i,int& j,int& k) const
	{
		bool res = true;
		i = static_cast<int>( (x-bbox.minX()) / bbox.rangeX() * xnum );
		j = static_cast<int>( (y-bbox.minY()) / bbox.rangeY() * ynum );
		k = static_cast<int>( (z-bbox.minZ()) / bbox.rangeZ() * znum );
		if( i < 0 ){ i = 0; res = false; }
		if( j < 0 ){ j = 0; res = false; }
		if( k < 0 ){ k = 0; res = false; }
		if( i >= xnum ){ i = xnum-1; res = false; }
		if( j >= ynum ){ j = ynum-1; res = false; }
		if( k >= znum ){ k = znum-1; res = false; }
		return res;
	};
};



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


	double getNearest(const double x,const double y,const double z,kmb::nodeIdType &nearestId) const;
protected:


	double getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,kmb::nodeIdType &nearestId) const;


	double getNearestInBucket2(const kmb::Point3D& pt,
		int imin,int jmin,int kmin,int imax,int jmax,int kmax,
		int imin_ex,int jmin_ex,int kmin_ex,int imax_ex,int jmax_ex,int kmax_ex,
		kmb::nodeIdType &nearestId) const;
};

}
