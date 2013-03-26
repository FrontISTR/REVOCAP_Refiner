/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DContainerMArray                                  #
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
#include <cfloat>
#include "Geometry/kmbPoint3DContainerMArray.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::Point3DContainerMArray::CONTAINER_TYPE = "double_marray";

kmb::Point3DContainerMArray::Point3DContainerMArray(void)
: kmb::Point3DContainer()
, pointArray(DBL_MAX)
, aIndex()
, count(0)
{
}

kmb::Point3DContainerMArray::~Point3DContainerMArray(void)
{
	clear();
}

void
kmb::Point3DContainerMArray::clear(void)
{
	pointArray.clearData();
	aIndex = pointArray.getBLArrayIndex(0);
	count = 0;
	this->boundBox.initialize();
}

bool
kmb::Point3DContainerMArray::initialize(size_t size)
{
	clear();
	if( size <= 0 ){
		size = 0x1000U;
	}else if( size > 0x100000U ){
		size = 0x100000U;
	}
	pointArray.initialize(size);
	aIndex = pointArray.getBLArrayIndex(0);
	return true;
}

kmb::nodeIdType
kmb::Point3DContainerMArray::addPoint(const double x,const double y,const double z)
{
	double q[3] = {x,y,z};
	pointArray.set(aIndex,q);
	boundBox.update(x,y,z);
	++aIndex;
	++count;
	return static_cast<kmb::nodeIdType>(aIndex.getIndex()-1);
}

kmb::nodeIdType
kmb::Point3DContainerMArray::addPoint(const kmb::Point3D& point)
{
	return addPoint( point.x(), point.y(), point.z() );
}

kmb::nodeIdType
kmb::Point3DContainerMArray::addPoint(const double x,const double y,const double z,const nodeIdType id)
{
	if( 0 <= id ){
		double q[3] = {x,y,z};
		kmb::BLArrayIndex ind = pointArray.getBLArrayIndex( static_cast<size_t>(id) );
		if( pointArray.set( ind, q ) ){
			if( aIndex <= ind ){
				aIndex = ind;
				++aIndex;
			}
			boundBox.update( x, y, z );
			++count;
			return id;
		}
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::Point3DContainerMArray::addPoint(const kmb::Point3D& point,const nodeIdType id)
{
	return addPoint( point.x(), point.y(), point.z(), id );
}

bool
kmb::Point3DContainerMArray::getXYZ(nodeIdType id,double &x,double &y,double &z) const
{
	if( 0 <= id && id < static_cast<kmb::nodeIdType>(aIndex.getIndex()) ){
		size_t i = static_cast<size_t>(id);
		double q[3] = {0.0,0.0,0.0};
		if( pointArray.get( i, q ) ){
			x = q[0];
			y = q[1];
			z = q[2];
			return true;
		}
	}
	return false;
}

bool
kmb::Point3DContainerMArray::getPoint(nodeIdType id,kmb::Point3D &point) const
{
	if( 0 <= id && id < static_cast<kmb::nodeIdType>(aIndex.getIndex()) ){
		size_t i = static_cast<size_t>(id);
		double q[3] = {0.0,0.0,0.0};
		if( pointArray.get( i, q ) ){
			point.setCoordinate( q[0], q[1], q[2] );
			return true;
		}
	}
	return false;
}

kmb::nodeIdType
kmb::Point3DContainerMArray::getMaxId(void) const
{
	return static_cast<kmb::nodeIdType>(aIndex.getIndex()-1);
}

kmb::nodeIdType
kmb::Point3DContainerMArray::getMinId(void) const
{
	return 0;
}

size_t
kmb::Point3DContainerMArray::getCount(void) const
{
	return count;
}

bool
kmb::Point3DContainerMArray::deletePoint(nodeIdType id)
{
	if( 0 <= id ){
		size_t i = static_cast<size_t>(id);
		double q[3] = {DBL_MAX,DBL_MAX,DBL_MAX};
		if( pointArray.set( i, q ) ){
			--count;
			return true;
		}
	}
	return false;
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerMArray::begin(void)
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerMArray::_iteratorMArray* _it = NULL;
	_it = new kmb::Point3DContainerMArray::_iteratorMArray();
	if( _it ){
		_it->points = this;
		this->pointArray.first( _it->aIndex );

	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerMArray::begin(void) const
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerMArray::_iteratorMArray* _it = NULL;
	_it = new kmb::Point3DContainerMArray::_iteratorMArray();
	if( _it ){
		_it->points = const_cast<kmb::Point3DContainerMArray*>(this);
		this->pointArray.first( _it->aIndex );

	}
	return kmb::Point3DContainer::const_iterator(_it);
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerMArray::find(kmb::nodeIdType nodeId)
{
	if( this->getCount() == 0 || nodeId < 0 ){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::BLArrayIndex ind = this->pointArray.getBLArrayIndex( static_cast<size_t>(nodeId) );
	if( this->pointArray.has(ind) ){
		kmb::Point3DContainerMArray::_iteratorMArray* _it = NULL;
		_it = new kmb::Point3DContainerMArray::_iteratorMArray();
		if( _it ){
			_it->points = this;
			_it->aIndex = ind;
		}
		return kmb::Point3DContainer::iterator(_it);
	}else{
		return kmb::Point3DContainer::iterator(NULL);
	}
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerMArray::find(kmb::nodeIdType nodeId) const
{
	if( this->getCount() == 0 || nodeId < 0 ){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::BLArrayIndex ind = this->pointArray.getBLArrayIndex( static_cast<size_t>(nodeId) );
	if( this->pointArray.has(ind) ){
		kmb::Point3DContainerMArray::_iteratorMArray* _it = NULL;
		_it = new kmb::Point3DContainerMArray::_iteratorMArray();
		if( _it ){
			_it->points = const_cast<kmb::Point3DContainerMArray*>(this);
			_it->aIndex = ind;
		}
		return kmb::Point3DContainer::const_iterator(_it);
	}else{
		return kmb::Point3DContainer::const_iterator(NULL);
	}
}

kmb::Point3DContainerMArray::_iteratorMArray::_iteratorMArray(void)
: kmb::Point3DContainer::_iterator()
, points(NULL)
, aIndex()
{
}

kmb::Point3DContainerMArray::_iteratorMArray::~_iteratorMArray(void)
{
}

kmb::nodeIdType
kmb::Point3DContainerMArray::_iteratorMArray::getId(void) const
{
	return static_cast<kmb::nodeIdType>(aIndex.getIndex());
}

bool
kmb::Point3DContainerMArray::_iteratorMArray::getXYZ(double &x,double &y,double &z) const
{
	double q[3] = {0.0,0.0,0.0};
	if( points->pointArray.get( aIndex, q ) ){
		x = q[0];
		y = q[1];
		z = q[2];
		return true;
	}
	return false;
}

bool
kmb::Point3DContainerMArray::_iteratorMArray::getPoint(kmb::Point3D &point) const
{
	double q[3] = {0.0,0.0,0.0};
	if( points->pointArray.get( aIndex, q ) ){
		point.setCoordinate( q[0], q[1], q[2] );
		return true;
	}
	return false;
}

double
kmb::Point3DContainerMArray::_iteratorMArray::x() const
{
	double q[3] = {0.0,0.0,0.0};
	points->pointArray.get( aIndex, q );
	return q[0];
}

double
kmb::Point3DContainerMArray::_iteratorMArray::y() const
{
	double q[3] = {0.0,0.0,0.0};
	points->pointArray.get( aIndex, q );
	return q[1];
}

double
kmb::Point3DContainerMArray::_iteratorMArray::z() const
{
	double q[3] = {0.0,0.0,0.0};
	points->pointArray.get( aIndex, q );
	return q[2];
}

bool
kmb::Point3DContainerMArray::_iteratorMArray::setXYZ(double x,double y,double z) const
{
	double q[3] = {x,y,z};
	if( points->pointArray.set( aIndex.getIndex(), q ) ){
		return true;
	}
	return false;
}

bool
kmb::Point3DContainerMArray::_iteratorMArray::setPoint(kmb::Point3D &point) const
{
	double q[3] = {point.x(),point.y(),point.z()};
	if( points->pointArray.set( aIndex.getIndex(), q ) ){
		return true;
	}
	return false;
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerMArray::_iteratorMArray::operator++(void)
{
	do{
		++aIndex;
		if( points->pointArray.has(aIndex) ){
			return this;
		}
	}while( points->pointArray.valid(aIndex) );
	return NULL;
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerMArray::_iteratorMArray::operator++(int n)
{
	do{
		aIndex++;
		if( points->pointArray.has(aIndex) ){
			return this;
		}
	}while( points->pointArray.valid(aIndex) );
	return NULL;
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerMArray::_iteratorMArray::clone(void)
{
	kmb::Point3DContainerMArray::_iteratorMArray* _itClone = NULL;
	_itClone = new kmb::Point3DContainerMArray::_iteratorMArray();
	if( _itClone ){
		_itClone->points = this->points;
		_itClone->aIndex = this->aIndex;
	}
	return _itClone;
}
