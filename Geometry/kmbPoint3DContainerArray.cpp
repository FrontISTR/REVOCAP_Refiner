/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DContainerArray                                   #
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
#include "Geometry/kmbPoint3DContainerArray.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::Point3DContainerArray::CONTAINER_TYPE = "double_array";

kmb::Point3DContainerArray::Point3DContainerArray(void)
: pointArray(NULL)
, index(0)
, size(0)
{
}

kmb::Point3DContainerArray::~Point3DContainerArray(void)
{
	clear();
}

bool kmb::Point3DContainerArray::initialize(size_t size)
{
	clear();
	this->pointArray = new double[3*size];
	this->size = size;
	return true;
}

kmb::nodeIdType
kmb::Point3DContainerArray::addPoint(const double x,const double y,const double z)
{
	if( pointArray && index < size ){
		pointArray[3*index]   = x;
		pointArray[3*index+1] = y;
		pointArray[3*index+2] = z;
		boundBox.update(x,y,z);
		++index;
		return static_cast<kmb::nodeIdType>(index-1);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType
kmb::Point3DContainerArray::addPoint(const kmb::Point3D& point)
{
	return addPoint( point.x(), point.y(), point.z() );
}

kmb::nodeIdType
kmb::Point3DContainerArray::addPoint(const double x,const double y,const double z,const nodeIdType id)
{
	if( pointArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		pointArray[3*id]   = x;
		pointArray[3*id+1] = y;
		pointArray[3*id+2] = z;
		boundBox.update(x,y,z);
		return id;
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType
kmb::Point3DContainerArray::addPoint(const kmb::Point3D& point,const nodeIdType id)
{
	if( pointArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		pointArray[3*id]   = point.x();
		pointArray[3*id+1] = point.y();
		pointArray[3*id+2] = point.z();
		boundBox.update(point);
		return id;
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::Point3DContainerArray::getXYZ(nodeIdType id,double &x,double &y,double &z) const
{
	if( pointArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		x = pointArray[3*id];
		y = pointArray[3*id+1];
		z = pointArray[3*id+2];
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point3DContainerArray::getPoint(nodeIdType id,kmb::Point3D &point) const
{
	if( pointArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		point.setCoordinate(pointArray[3*id], pointArray[3*id+1], pointArray[3*id+2]);
		return true;
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::Point3DContainerArray::getMaxId(void) const
{
	return static_cast<kmb::nodeIdType>(size-1);
}

kmb::nodeIdType
kmb::Point3DContainerArray::getMinId(void) const
{
	return 0;
}

size_t
kmb::Point3DContainerArray::getCount(void) const
{
	return size;
}

void
kmb::Point3DContainerArray::clear(void)
{
	if( pointArray ){
		delete[] pointArray;
		pointArray = NULL;
		size = 0;
		index = 0;
	}
	this->boundBox.initialize();
}

bool
kmb::Point3DContainerArray::deletePoint(nodeIdType id)
{
	if( pointArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		pointArray[3*id] = 0.0;
		pointArray[3*id+1] = 0.0;
		pointArray[3*id+2] = 0.0;
		return true;
	}else{
		return false;
	}
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerArray::begin(void)
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerArray::_iteratorArray* _it = NULL;
	_it = new kmb::Point3DContainerArray::_iteratorArray();
	if( _it ){
		_it->points = this;
		_it->index = 0;
	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerArray::begin(void) const
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerArray::_iteratorArray* _it = NULL;
	_it = new kmb::Point3DContainerArray::_iteratorArray();
	if( _it ){
		_it->points = this;
		_it->index = 0;
	}
	return kmb::Point3DContainer::const_iterator(_it);
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerArray::find(kmb::nodeIdType nodeId)
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->size) ){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerArray::_iteratorArray* _it = NULL;
	_it = new kmb::Point3DContainerArray::_iteratorArray();
	if( _it ){
		_it->points = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerArray::find(kmb::nodeIdType nodeId) const
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->size) ){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerArray::_iteratorArray* _it = NULL;
	_it = new kmb::Point3DContainerArray::_iteratorArray();
	if( _it ){
		_it->points = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point3DContainer::const_iterator(_it);
}



kmb::nodeIdType
kmb::Point3DContainerArray::_iteratorArray::getId(void) const
{
	return static_cast<kmb::nodeIdType>(this->index);
}

bool
kmb::Point3DContainerArray::_iteratorArray::getXYZ(double &x,double &y,double &z) const
{
	x = points->pointArray[3*index];
	y = points->pointArray[3*index+1];
	z = points->pointArray[3*index+2];
	return true;
}

bool
kmb::Point3DContainerArray::_iteratorArray::getPoint(kmb::Point3D &point) const
{
	point.setCoordinate(
		points->pointArray[3*index],
		points->pointArray[3*index+1],
		points->pointArray[3*index+2]);
	return true;
}

double
kmb::Point3DContainerArray::_iteratorArray::x() const
{
	return points->pointArray[3*index];
}

double
kmb::Point3DContainerArray::_iteratorArray::y() const
{
	return points->pointArray[3*index+1];
}

double
kmb::Point3DContainerArray::_iteratorArray::z() const
{
	return points->pointArray[3*index+2];
}

bool
kmb::Point3DContainerArray::_iteratorArray::setPoint(kmb::Point3D &point) const
{
	points->pointArray[3*index  ] = point.x();
	points->pointArray[3*index+1] = point.y();
	points->pointArray[3*index+2] = point.z();
	return true;
}

bool
kmb::Point3DContainerArray::_iteratorArray::setXYZ(double x,double y,double z) const
{
	points->pointArray[3*index  ] = x;
	points->pointArray[3*index+1] = y;
	points->pointArray[3*index+2] = z;
	return true;
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerArray::_iteratorArray::operator++(void)
{
	++(this->index);
	if( this->index < points->getCount() ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerArray::_iteratorArray::operator++(int n)
{
	this->index++;
	if( this->index < points->getCount() ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerArray::_iteratorArray::clone(void)
{
	kmb::Point3DContainerArray::_iteratorArray* _itClone = NULL;
	_itClone = new kmb::Point3DContainerArray::_iteratorArray();
	if( _itClone ){
		_itClone->points = this->points;
		_itClone->index = this->index;
	}
	return _itClone;
}

const char* kmb::Point3DContainerXYZArray::CONTAINER_TYPE = "double_xyzarray";

kmb::Point3DContainerXYZArray::Point3DContainerXYZArray(void)
: xArray(NULL)
, yArray(NULL)
, zArray(NULL)
, index(0)
, size(0)
{
}

kmb::Point3DContainerXYZArray::~Point3DContainerXYZArray(void)
{
	clear();
}

bool kmb::Point3DContainerXYZArray::initialize(size_t size)
{
	clear();
	this->xArray = new double[size];
	this->yArray = new double[size];
	this->zArray = new double[size];
	this->size = size;
	return true;
}

kmb::nodeIdType
kmb::Point3DContainerXYZArray::addPoint(const double x,const double y,const double z)
{
	if( xArray && yArray && zArray && index < size ){
		xArray[index] = x;
		yArray[index] = y;
		zArray[index] = z;
		boundBox.update(x,y,z);
		++index;
		return static_cast<kmb::nodeIdType>(index-1);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType
kmb::Point3DContainerXYZArray::addPoint(const kmb::Point3D& point)
{
	if( xArray && yArray && zArray && index < size ){
		xArray[index] = point.x();
		yArray[index] = point.y();
		zArray[index] = point.z();
		boundBox.update( point.x(), point.y(), point.z() );
		++index;
		return static_cast<kmb::nodeIdType>(index-1);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType
kmb::Point3DContainerXYZArray::addPoint(const double x,const double y,const double z,const nodeIdType id)
{
	if( xArray && yArray && zArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		xArray[id] = x;
		yArray[id] = y;
		zArray[id] = z;
		boundBox.update(x,y,z);
		return id;
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType
kmb::Point3DContainerXYZArray::addPoint(const kmb::Point3D& point,const nodeIdType id)
{
	if( xArray && yArray && zArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		xArray[id] = point.x();
		yArray[id] = point.y();
		zArray[id] = point.z();
		boundBox.update(point);
		return id;
	}else{
		return kmb::nullNodeId;
	}
}

bool
kmb::Point3DContainerXYZArray::getXYZ(nodeIdType id,double &x,double &y,double &z) const
{
	if( xArray && yArray && zArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		x = xArray[id];
		y = yArray[id];
		z = zArray[id];
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point3DContainerXYZArray::getPoint(nodeIdType id,kmb::Point3D &point) const
{
	if( xArray && yArray && zArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		point.setCoordinate(xArray[id], yArray[id], zArray[id]);
		return true;
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::Point3DContainerXYZArray::getMaxId(void) const
{
	return static_cast<kmb::nodeIdType>(size-1);
}

kmb::nodeIdType
kmb::Point3DContainerXYZArray::getMinId(void) const
{
	return 0;
}

size_t
kmb::Point3DContainerXYZArray::getCount(void) const
{
	return size;
}

void
kmb::Point3DContainerXYZArray::clear(void)
{
	if( xArray && yArray && zArray ){
		delete[] xArray;
		delete[] yArray;
		delete[] zArray;
		xArray = NULL;
		yArray = NULL;
		zArray = NULL;
		size = 0;
		index = 0;
	}
	this->boundBox.initialize();
}

bool
kmb::Point3DContainerXYZArray::deletePoint(nodeIdType id)
{
	if( xArray && yArray && zArray && 0 <= id && id < static_cast<kmb::nodeIdType>(size) ){
		xArray[id] = 0.0;
		yArray[id] = 0.0;
		zArray[id] = 0.0;
		return true;
	}else{
		return false;
	}
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerXYZArray::begin(void)
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerXYZArray::_iteratorXYZ* _it = NULL;
	_it = new kmb::Point3DContainerXYZArray::_iteratorXYZ();
	if( _it ){
		_it->points = this;
		_it->index = 0;
	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerXYZArray::begin(void) const
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerXYZArray::_iteratorXYZ* _it = NULL;
	_it = new kmb::Point3DContainerXYZArray::_iteratorXYZ();
	if( _it ){
		_it->points = this;
		_it->index = 0;
	}
	return kmb::Point3DContainer::const_iterator(_it);
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerXYZArray::find(kmb::nodeIdType nodeId)
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->size) ){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerXYZArray::_iteratorXYZ* _it = NULL;
	_it = new kmb::Point3DContainerXYZArray::_iteratorXYZ();
	if( _it ){
		_it->points = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerXYZArray::find(kmb::nodeIdType nodeId) const
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->size) ){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerXYZArray::_iteratorXYZ* _it = NULL;
	_it = new kmb::Point3DContainerXYZArray::_iteratorXYZ();
	if( _it ){
		_it->points = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point3DContainer::const_iterator(_it);
}



kmb::nodeIdType
kmb::Point3DContainerXYZArray::_iteratorXYZ::getId(void) const
{
	return static_cast<kmb::nodeIdType>(this->index);
}

bool
kmb::Point3DContainerXYZArray::_iteratorXYZ::getXYZ(double &x,double &y,double &z) const
{
	x = points->xArray[index];
	y = points->yArray[index];
	z = points->zArray[index];
	return true;
}

bool
kmb::Point3DContainerXYZArray::_iteratorXYZ::getPoint(kmb::Point3D &point) const
{
	point.setCoordinate(
		points->xArray[index],
		points->yArray[index],
		points->zArray[index]);
	return true;
}

double
kmb::Point3DContainerXYZArray::_iteratorXYZ::x() const
{
	return points->xArray[index];
}

double
kmb::Point3DContainerXYZArray::_iteratorXYZ::y() const
{
	return points->yArray[index];
}

double
kmb::Point3DContainerXYZArray::_iteratorXYZ::z() const
{
	return points->zArray[index];
}

bool
kmb::Point3DContainerXYZArray::_iteratorXYZ::setPoint(kmb::Point3D &point) const
{
	points->xArray[index] = point.x();
	points->yArray[index] = point.y();
	points->zArray[index] = point.z();
	return true;
}

bool
kmb::Point3DContainerXYZArray::_iteratorXYZ::setXYZ(double x,double y,double z) const
{
	points->xArray[index] = x;
	points->yArray[index] = y;
	points->zArray[index] = z;
	return true;
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerXYZArray::_iteratorXYZ::operator++(void)
{
	++(this->index);
	if( this->index < points->getCount() ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerXYZArray::_iteratorXYZ::operator++(int n)
{
	this->index++;
	if( this->index < points->getCount() ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerXYZArray::_iteratorXYZ::clone(void)
{
	kmb::Point3DContainerXYZArray::_iteratorXYZ* _itClone = NULL;
	_itClone = new kmb::Point3DContainerXYZArray::_iteratorXYZ();
	if( _itClone ){
		_itClone->points = this->points;
		_itClone->index = this->index;
	}
	return _itClone;
}
