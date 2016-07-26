/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DContainer                                        #
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

//
// Point3D に ID を付けてメモリに格納するコンテナ類
//

#include <map>
#include <vector>
#include <cfloat>
#include <cstddef>
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbBoundingBox.h"
#include "Geometry/kmbIdTypes.h"

namespace kmb{

class Matrix4x4;

class Point3DContainer
{
protected:
	kmb::BoundingBox boundBox;
public:
	Point3DContainer(void);
	virtual ~Point3DContainer(void);
	virtual bool initialize(size_t size) = 0;
	virtual nodeIdType addPoint(const double x,const double y,const double z) = 0;
	virtual nodeIdType addPoint(const kmb::Point3D& point) = 0;
	// id を指定して追加
	// 失敗したら -1 を返す
	virtual nodeIdType addPoint(const double x,const double y,const double z,const nodeIdType id) = 0;
	virtual nodeIdType addPoint(const kmb::Point3D& point,const nodeIdType id) = 0;
//	virtual kmb::Point3D* getPoint(nodeIdType id) const = 0;
	virtual bool getXYZ(nodeIdType id,double &x,double &y,double &z) const = 0;
	virtual bool getPoint(nodeIdType id,kmb::Point3D &point) const = 0;
	virtual nodeIdType getMaxId(void) const = 0;
	virtual nodeIdType getMinId(void) const = 0;
	virtual size_t getCount(void) const = 0;
	virtual void clear(void) = 0;
	virtual bool deletePoint(nodeIdType id) = 0;

	virtual const char* getContainerType(void) const = 0;

protected:
	class _iterator
	{
	public:
		virtual ~_iterator(void){};
		virtual nodeIdType getId(void) const = 0;
		virtual bool getXYZ(double &x,double &y,double &z) const = 0;
		virtual bool getPoint(kmb::Point3D &point) const = 0;
		virtual double x() const = 0;
		virtual double y() const = 0;
		virtual double z() const = 0;
		virtual bool setPoint(kmb::Point3D &point) const = 0;
		virtual bool setXYZ(double x,double y,double z) const = 0;
		virtual _iterator* operator++(void) = 0;
		virtual _iterator* operator++(int n) = 0;
		virtual _iterator* clone(void) = 0;
	};

public:
	class const_iterator;
	// iterator pointer の wrapper
	class iterator
	{
		friend class Point3DContainer;
		friend class Point3DContainer::const_iterator;
	private:
		_iterator* _it;
	public:
		iterator(_iterator* _i=NULL) : _it(_i){}
		iterator(const iterator& other);
		~iterator(void){ if( _it ) delete _it; }
		nodeIdType getId(void) const;
		bool getXYZ(double &x,double &y,double &z) const;
		bool getPoint( kmb::Point3D &point ) const;
		double x() const;
		double y() const;
		double z() const;
		bool setXYZ(double x,double y,double z) const;
		bool setPoint( kmb::Point3D &point ) const;
		iterator& operator=(const iterator& other);
		iterator& operator++(void);  // ++前置
		iterator operator++(int n);  // 後置++
		bool operator==(const iterator &other) const;
		bool operator!=(const iterator &other) const;
		// 終了判定 == end() でもいいけど、こっちの方が速い
		bool isFinished(void){ return (_it==NULL); }
	};

	class const_iterator
	{
		friend class Point3DContainer;
	private:
		_iterator* _it;
	public:
		const_iterator(_iterator* _i=NULL) : _it(_i){}
		const_iterator(const const_iterator& other);
		const_iterator(const iterator& other);
		~const_iterator(void){ if( _it ) delete _it; }
		nodeIdType getId(void) const;
		const kmb::Point3D* getPoint(void) const;
		bool getXYZ(double &x,double &y,double &z) const;
		bool getPoint( kmb::Point3D &point ) const;
		double x() const;
		double y() const;
		double z() const;
		const_iterator& operator=(const const_iterator& other);
		const_iterator& operator=(const iterator& other);
		const_iterator& operator++(void);  // ++前置
		const_iterator operator++(int n);  // 後置++
		bool operator==(const const_iterator &other) const;
		bool operator!=(const const_iterator &other) const;
		// 終了判定 == end() でもいいけど、こっちの方が速い
		bool isFinished(void){ return (_it==NULL); }
	};
private:
	static const iterator endIterator; // NULL 値
	static const const_iterator endConstIterator; // NULL 値
public:
	virtual iterator begin(void) = 0;
	virtual const_iterator begin(void) const = 0;
	virtual iterator find(kmb::nodeIdType nodeId) = 0;
	virtual const_iterator find(kmb::nodeIdType nodeId) const = 0;
	// 値返しにすると毎回コンストラクタが呼ばれるのでよくない
	const iterator& end(void){
		return endIterator;
	};
	const const_iterator& end(void) const{
		return endConstIterator;
	};

	// BoundingBox
	void getBoundingBox(kmb::BoundingBox& bound) const;
	const kmb::BoundingBox* getBoundingBoxPtr(void) const {	return &boundBox;	};
	const kmb::BoundingBox& getBoundingBox(void) const {	return boundBox;	};
	// 最近点の検索
	double getNearest(const kmb::Point3D* target,kmb::Point3D& result,nodeIdType &nearestId) const;
	double getNearest(const double x,const double y,const double z,kmb::Point3D& result,nodeIdType &nearestId) const;
	// 線分に対して最近点を探す
	// param*t0 + (1-param)*t1 が最近点
	// d は初期距離：距離がそれ以下のを探す
	double getNearestToSegment(const Point3D& t0, const Point3D& t1,kmb::Point3D& result,nodeIdType &nearestId,double &param,double d=DBL_MAX) const;

	// コピーメソッド
	void copyPreservingId(Point3DContainer& container);
	void copyWithoutPreservingId(Point3DContainer& container);
	// 節点二重化
	kmb::nodeIdType duplicatePoint(nodeIdType nodeId);
	// 変換系
	void convertAllNodes( kmb::Matrix4x4& mat );
	void translateAllNodes( double x,double y,double z );
	void scaleAllNodes( double r );
	// 更新系
	bool updateCoordinate( kmb::nodeIdType nodeId, double x, double y, double z );
	// 法線ベクトル
	kmb::Vector3D calcNormalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2 ) const;
	kmb::Vector3D calcNormalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3 ) const;
	// 直交ベクトル（正規化しない）
	kmb::Vector3D calcVerticalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2 ) const;
	kmb::Vector3D calcVerticalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3 ) const;
	double distanceSq( kmb::nodeIdType n0, kmb::nodeIdType n1 ) const;
	double cos( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2 ) const;

public:
	static bool copy(const kmb::Point3DContainer* org, kmb::Point3DContainer* points);
	static kmb::Point3DContainer* createContainer( const char* containerType );
};

class Point3DContainerMap : public Point3DContainer
{
public:
	enum idContinuityType{
		UNKNOWN = -1,		// よくわからない
		ZERO_LEADING = 0,	// 0 から始まる
		ONE_LEADING = 1,	// 1 から始まる
		OTHER_LEADING = 2,  // それ以外の数字から連続している
		NOT_CONTINUOUS = 3	// 連続していない
	};
private:
	nodeIdType minId;
	nodeIdType maxId;
	std::map< kmb::nodeIdType, kmb::Point3D* > points;
	idContinuityType idContinuity;
public:
	static const char* CONTAINER_TYPE;
	Point3DContainerMap(void);
	virtual ~Point3DContainerMap(void);
	virtual bool initialize(size_t size);
	virtual nodeIdType addPoint(const double x,const double y,const double z);
	virtual nodeIdType addPoint(const kmb::Point3D& point);
	virtual nodeIdType addPoint(const double x,const double y,const double z,const nodeIdType id);
	virtual nodeIdType addPoint(const kmb::Point3D& point,const nodeIdType id);
//	virtual kmb::Point3D* getPoint(nodeIdType id) const;
	virtual bool getXYZ(nodeIdType id,double &x,double &y,double &z) const;
	virtual bool getPoint(nodeIdType id,kmb::Point3D &point) const;
	virtual nodeIdType getMaxId(void) const;
	virtual nodeIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual bool deletePoint(nodeIdType id);

	virtual const char* getContainerType(void) const{
		return kmb::Point3DContainerMap::CONTAINER_TYPE;
	}
	virtual kmb::Point3D* erasePoint(nodeIdType id);

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::nodeIdType nodeId);
	virtual const_iterator find(kmb::nodeIdType nodeId) const;

// 以下は Map モードの時のみ定義された関数
	bool replaceId(nodeIdType oldid,nodeIdType newid);
	bool deleteId(nodeIdType id);
	void updateMinMaxId(void);


	idContinuityType getIdContinuity(void) const{ return idContinuity; }

	void idDefragment(nodeIdType initId, std::map< nodeIdType, nodeIdType >& idmap);
protected:
	class _iteratorMap : public Point3DContainer::_iterator
	{
		friend class Point3DContainerMap;
	public:
		virtual nodeIdType getId(void) const;
		virtual bool getXYZ(double &x,double &y,double &z) const;
		virtual bool getPoint(kmb::Point3D &point) const;
		virtual double x() const;
		virtual double y() const;
		virtual double z() const;
		virtual bool setXYZ(double x,double y,double z) const;
		virtual bool setPoint(kmb::Point3D &point) const;
		virtual Point3DContainer::_iterator* operator++(void);
		virtual Point3DContainer::_iterator* operator++(int n);
		virtual Point3DContainer::_iterator* clone(void);
	private:
		_iteratorMap(void){};
		virtual ~_iteratorMap(void){};
		std::map< kmb::nodeIdType, kmb::Point3D* >::const_iterator pointIter;
		std::map< kmb::nodeIdType, kmb::Point3D* >::const_iterator endIterator;
	};
};

// direct access
class Point3DContainerDirectAccessable : public Point3DContainer{
public:
	virtual double operator()(kmb::nodeIdType nodeId,int index) const = 0;
	virtual double& operator()(kmb::nodeIdType nodeId,int index) = 0;
	// nodeId までの値の設定を行って、iterator で参照できるようにする
	// 注：size は nodeId + 1 である
	virtual void commit(kmb::nodeIdType nodeId) = 0;
};

}

