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
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbPoint3DContainerArray.h"
#include "Geometry/kmbPoint3DContainerMArray.h"
#include "Geometry/kmbPoint3DContainerVect.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbGeometry4D.h"
#include <cmath>
#include <cstring>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

bool
kmb::Point3DContainer::copy(const kmb::Point3DContainer* org, kmb::Point3DContainer* points)
{
	if( org == NULL || points == NULL ){
		return false;
	}
	bool ret = true;
	points->initialize( org->getCount() );
	kmb::Point3D point;
	kmb::Point3DContainer::const_iterator pIter = org->begin();
	while( !pIter.isFinished() && ret ){
		ret &= pIter.getPoint( point );
		if( ret ){
			points->addPoint( point, pIter.getId() );
		}
		++pIter;
	}
	return ret;
}

kmb::Point3DContainer*
kmb::Point3DContainer::createContainer( const char* containerType )
{
	if( containerType == NULL ){
		return NULL;
	}
	if(0==strcmp(containerType,kmb::Point3DContainerMap::CONTAINER_TYPE) ){
		return new kmb::Point3DContainerMap();
	}
	else if(0==strcmp(containerType,kmb::Point3DContainerArray::CONTAINER_TYPE) ){
		return new kmb::Point3DContainerArray();
	}
	else if(0==strcmp(containerType,kmb::Point3DContainerVect::CONTAINER_TYPE) ){
		return new kmb::Point3DContainerVect();
	}
	else if(0==strcmp(containerType,kmb::Point3DContainerMArray::CONTAINER_TYPE) ){
		return new kmb::Point3DContainerMArray();
	}
	else if(0==strcmp(containerType,kmb::Point3DContainerXYZArray::CONTAINER_TYPE) ){
		return new kmb::Point3DContainerXYZArray();
	}
	else{
		return NULL;
	}
}

kmb::Point3DContainer::Point3DContainer(void)
{
}

kmb::Point3DContainer::~Point3DContainer(void)
{
}

const kmb::Point3DContainer::iterator kmb::Point3DContainer::endIterator;
const kmb::Point3DContainer::const_iterator kmb::Point3DContainer::endConstIterator;

void
kmb::Point3DContainer::copyPreservingId(Point3DContainer& container)
{
	this->clear();
	kmb::Point3D point;
	kmb::Point3DContainer::iterator pIter = container.begin();
	while( pIter != container.end() )
	{
		nodeIdType id = pIter.getId();
		if( pIter.getPoint( point ) ){
			this->addPoint( point , id );
		}
		++pIter;
	}
}

void
kmb::Point3DContainer::copyWithoutPreservingId(Point3DContainer& container)
{
	this->clear();
	kmb::Point3D point;
	kmb::Point3DContainer::iterator pIter = container.begin();
	while( pIter != container.end() )
	{
		if( pIter.getPoint( point ) ){
			this->addPoint( point );
		}
		++pIter;
	}
}

kmb::nodeIdType
kmb::Point3DContainer::duplicatePoint(nodeIdType nodeId)
{
	kmb::Point3D point;
	if( this->getPoint( nodeId, point ) ){
		return this->addPoint( point.x(), point.y(), point.z() );
	}else{
		return -1;
	}
}

void
kmb::Point3DContainer::convertAllNodes( kmb::Matrix4x4& mat )
{
	kmb::Point3DContainer::iterator pIter = this->begin();
	kmb::Point3DContainer::iterator pEnd = this->end();
	kmb::Point3D point;
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			mat.convert( point );
			pIter.setPoint( point );
		}
		++pIter;
	}
	kmb::Point3D bmax = boundBox.getMax();
	kmb::Point3D bmin = boundBox.getMin();
	mat.convert( bmax );
	mat.convert( bmin );
	boundBox.initialize();
	boundBox.update( bmax );
	boundBox.update( bmin );
}

void
kmb::Point3DContainer::translateAllNodes( double x,double y,double z )
{
	kmb::Point3DContainer::iterator pIter = this->begin();
	kmb::Point3DContainer::iterator pEnd = this->end();
	kmb::Point3D point;
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			point.addCoordinate(x,y,z);
			pIter.setPoint( point );
		}
		++pIter;
	}
	kmb::Point3D bmax = boundBox.getMax();
	kmb::Point3D bmin = boundBox.getMin();
	bmax.addCoordinate(x,y,z);
	bmin.addCoordinate(x,y,z);
	boundBox.initialize();
	boundBox.update( bmax );
	boundBox.update( bmin );
}

void
kmb::Point3DContainer::scaleAllNodes( double r )
{
	kmb::Point3DContainer::iterator pIter = this->begin();
	kmb::Point3DContainer::iterator pEnd = this->end();
	kmb::Point3D point;
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			point.scale(r);
			pIter.setPoint( point );
		}
		++pIter;
	}
	kmb::Point3D bmax = boundBox.getMax();
	kmb::Point3D bmin = boundBox.getMin();
	bmax.scale(r);
	bmin.scale(r);
	boundBox.initialize();
	boundBox.update( bmax );
	boundBox.update( bmin );
}

bool
kmb::Point3DContainer::updateCoordinate( kmb::nodeIdType nodeId, double x, double y, double z )
{
	kmb::Point3DContainer::iterator pIter = this->find(nodeId);
	if( !pIter.isFinished() ){
		kmb::Point3D point(x,y,z);
		pIter.setPoint( point );
		return true;
	}
	return false;
}

void
kmb::Point3DContainer::getBoundingBox(kmb::BoundingBox& bound) const
{
	bound.update( boundBox.getMax() );
	bound.update( boundBox.getMin() );
}

double
kmb::Point3DContainer::getNearest
(const kmb::Point3D* target,kmb::Point3D& result,nodeIdType &nearestId) const
{
	return getNearest( target->x(), target->y(), target->z(), result, nearestId );
}

double
kmb::Point3DContainer::getNearest
(const double x,const double y,const double z,kmb::Point3D& result,nodeIdType &nearestId) const
{
	kmb::Minimizer min;
	kmb::Point3D point;
	kmb::Point3DContainer::const_iterator pIter = this->begin();
	kmb::Point3DContainer::const_iterator pEnd = this->end();
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			if( min.update( point.distanceSq( x, y, z ) ) ){
				nearestId = pIter.getId();
				result.set( point );
			}
		}
		++pIter;
	}
	return sqrt( min.getMin() );
}

double
kmb::Point3DContainer::getNearestToSegment
(const kmb::Point3D& t0,const kmb::Point3D& t1,kmb::Point3D& result,nodeIdType &nearestId,double &param,double d) const
{
	kmb::Minimizer min;
	kmb::Point3D point;
	min.update(d);
	nearestId = -1;
	double t;

	kmb::Point3DContainer::const_iterator pIter = this->begin();
	kmb::Point3DContainer::const_iterator pEnd = this->end();
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			if( min.update( point.distanceSqToSegment(t0,t1,t) ) )
			{
				nearestId = pIter.getId();
				result.set( point );
				param = t;
			}
		}
		++pIter;
	}
	return sqrt( min.getMin() );
}

kmb::Vector3D
kmb::Point3DContainer::calcNormalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2 ) const
{
	double x0, y0, z0, x1, y1, z1, x2, y2, z2;
	if( getXYZ(n0,x0,y0,z0) &&
		getXYZ(n1,x1,y1,z1) &&
		getXYZ(n2,x2,y2,z2) )
	{
		x1 -= x0;
		y1 -= y0;
		z1 -= z0;
		x2 -= x0;
		y2 -= y0;
		z2 -= z0;

		x0 = y1*z2 - y2*z1;
		y0 = z1*x2 - z2*x1;
		z0 = x1*y2 - x2*y1;
		double r = sqrt(x0*x0+y0*y0+z0*z0);
		if( r != 0.0 ){
			return kmb::Vector3D(x0/r,y0/r,z0/r);
		}
	}
	return kmb::Vector3D();
}

kmb::Vector3D
kmb::Point3DContainer::calcNormalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3 ) const
{
	double x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3;
	if( getXYZ(n0,x0,y0,z0) &&
		getXYZ(n1,x1,y1,z1) &&
		getXYZ(n2,x2,y2,z2) &&
		getXYZ(n3,x3,y3,z3) )
	{
		x1 -= x0;
		y1 -= y0;
		z1 -= z0;
		x2 -= x0;
		y2 -= y0;
		z2 -= z0;
		x3 -= x0;
		y3 -= y0;
		z3 -= z0;

		kmb::Vector3D n012(y1*z2 - y2*z1,z1*x2 - z2*x1,x1*y2 - x2*y1);
		kmb::Vector3D n013(y1*z3 - y3*z1,z1*x3 - z3*x1,x1*y3 - x3*y1);
		kmb::Vector3D normal = n012 + n013;
		normal.normalize();
		return normal;
	}
	return kmb::Vector3D();
}

kmb::Vector3D
kmb::Point3DContainer::calcVerticalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2 ) const
{
	double x0, y0, z0, x1, y1, z1, x2, y2, z2;
	if( getXYZ(n0,x0,y0,z0) &&
		getXYZ(n1,x1,y1,z1) &&
		getXYZ(n2,x2,y2,z2) )
	{
		x1 -= x0;
		y1 -= y0;
		z1 -= z0;
		x2 -= x0;
		y2 -= y0;
		z2 -= z0;

		x0 = y1*z2 - y2*z1;
		y0 = z1*x2 - z2*x1;
		z0 = x1*y2 - x2*y1;
		return kmb::Vector3D(x0,y0,z0);
	}
	return kmb::Vector3D();
}

kmb::Vector3D
kmb::Point3DContainer::calcVerticalVector( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3 ) const
{
	double x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3;
	if( getXYZ(n0,x0,y0,z0) &&
		getXYZ(n1,x1,y1,z1) &&
		getXYZ(n2,x2,y2,z2) &&
		getXYZ(n3,x3,y3,z3) )
	{
		x1 -= x0;
		y1 -= y0;
		z1 -= z0;
		x2 -= x0;
		y2 -= y0;
		z2 -= z0;
		x3 -= x0;
		y3 -= y0;
		z3 -= z0;

		kmb::Vector3D n012(y1*z2 - y2*z1,z1*x2 - z2*x1,x1*y2 - x2*y1);
		kmb::Vector3D n013(y1*z3 - y3*z1,z1*x3 - z3*x1,x1*y3 - x3*y1);
		kmb::Vector3D normal = n012 + n013;
		return normal;
	}
	return kmb::Vector3D();
}


double
kmb::Point3DContainer::distanceSq( kmb::nodeIdType n0, kmb::nodeIdType n1 ) const
{
	kmb::Point3D p0, p1;
	if( this->getPoint(n0,p0) && this->getPoint(n1,p1) )
	{
		return p0.distanceSq(p1);
	}
	return -1.0;
}

double
kmb::Point3DContainer::cos( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2 ) const
{
	kmb::Point3D p0,p1,p2;
	if( getPoint(n0,p0) && getPoint(n1,p1) && getPoint(n2,p2) ){
		return kmb::Point3D::cos( p0, p1 ,p2 );
	}
	return -2.0;
}





kmb::Point3DContainer::iterator::iterator(const kmb::Point3DContainer::iterator &other)
: _it(NULL)
{
	if( other._it ){
		_it = other._it->clone();
	}
}

kmb::nodeIdType
kmb::Point3DContainer::iterator::getId(void) const
{
	if( _it != NULL )
		return _it->getId();
	else
		return -1;
}

bool
kmb::Point3DContainer::iterator::getXYZ(double &x,double &y,double &z) const
{
	if( _it != NULL ){
		return _it->getXYZ(x,y,z);
	}else{
		return false;
	}
}

bool
kmb::Point3DContainer::iterator::getPoint(kmb::Point3D &point) const
{
	if( _it != NULL ){
		return _it->getPoint( point );
	}else{
		return false;
	}
}


double
kmb::Point3DContainer::iterator::x(void) const
{
	return _it->x();
}

double
kmb::Point3DContainer::iterator::y(void) const
{
	return _it->y();
}

double
kmb::Point3DContainer::iterator::z(void) const
{
	return _it->z();
}

bool
kmb::Point3DContainer::iterator::setPoint(kmb::Point3D &point) const
{
	if( _it != NULL ){
		return _it->setPoint( point );
	}else{
		return false;
	}
}

kmb::Point3DContainer::iterator&
kmb::Point3DContainer::iterator::operator=(const kmb::Point3DContainer::iterator& other)
{
	if( _it != NULL && _it != other._it )
	{
		delete _it;
		_it = NULL;
	}


	if( other._it ){
		_it = other._it->clone();
	}
	return *this;
}


kmb::Point3DContainer::iterator&
kmb::Point3DContainer::iterator::operator++(void)
{
	if( _it != NULL ){
		kmb::Point3DContainer::_iterator* retVal = ++(*_it);
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
	}
	return *this;
}


kmb::Point3DContainer::iterator
kmb::Point3DContainer::iterator::operator++(int n)
{
	if( _it != NULL ){
		kmb::Point3DContainer::_iterator* _itClone = _it->clone();
		kmb::Point3DContainer::_iterator* retVal = (*_it)++;
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
		return kmb::Point3DContainer::iterator(_itClone);
	}else{
		return kmb::Point3DContainer::iterator(NULL);
	}
}

bool
kmb::Point3DContainer::iterator::operator==(const iterator &other) const
{
	return ( _it == other._it );
}

bool
kmb::Point3DContainer::iterator::operator!=(const iterator &other) const
{
	return ( _it != other._it );
}



kmb::Point3DContainer::const_iterator::const_iterator(const kmb::Point3DContainer::const_iterator &other)
: _it(NULL)
{
	if( other._it ){
		_it = other._it->clone();
	}
}

kmb::Point3DContainer::const_iterator::const_iterator(const kmb::Point3DContainer::iterator &other)
: _it(NULL)
{
	if( other._it ){
		_it = other._it->clone();
	}
}

kmb::nodeIdType
kmb::Point3DContainer::const_iterator::getId(void) const
{
	if( _it != NULL )
		return _it->getId();
	else
		return -1;
}

bool
kmb::Point3DContainer::const_iterator::getXYZ(double &x,double &y,double &z) const
{
	if( _it != NULL ){
		return _it->getXYZ(x,y,z);
	}else{
		return false;
	}
}

bool
kmb::Point3DContainer::const_iterator::getPoint(kmb::Point3D &point) const
{
	if( _it != NULL ){
		return _it->getPoint( point );
	}else{
		return false;
	}
}


double
kmb::Point3DContainer::const_iterator::x(void) const
{
	return _it->x();
}

double
kmb::Point3DContainer::const_iterator::y(void) const
{
	return _it->y();
}

double
kmb::Point3DContainer::const_iterator::z(void) const
{
	return _it->z();
}

kmb::Point3DContainer::const_iterator&
kmb::Point3DContainer::const_iterator::operator=(const kmb::Point3DContainer::const_iterator& other)
{
	if( _it != NULL && _it != other._it )
	{
		delete _it;
		_it = NULL;
	}


	if( other._it ){
		_it = other._it->clone();
	}
	return *this;
}

kmb::Point3DContainer::const_iterator&
kmb::Point3DContainer::const_iterator::operator=(const kmb::Point3DContainer::iterator& other)
{
	if( _it != NULL && _it != other._it )
	{
		delete _it;
		_it = NULL;
	}


	if( other._it ){
		_it = other._it->clone();
	}
	return *this;
}


kmb::Point3DContainer::const_iterator&
kmb::Point3DContainer::const_iterator::operator++(void)
{
	if( _it != NULL ){
		kmb::Point3DContainer::_iterator* retVal = ++(*_it);
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
	}
	return *this;
}


kmb::Point3DContainer::const_iterator
kmb::Point3DContainer::const_iterator::operator++(int n)
{
	if( _it != NULL ){
		kmb::Point3DContainer::_iterator* _itClone = _it->clone();
		kmb::Point3DContainer::_iterator* retVal = (*_it)++;
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
		return kmb::Point3DContainer::const_iterator(_itClone);
	}else{
		return kmb::Point3DContainer::const_iterator(NULL);
	}
}

bool
kmb::Point3DContainer::const_iterator::operator==(const const_iterator &other) const
{
	return ( _it == other._it );
}

bool
kmb::Point3DContainer::const_iterator::operator!=(const const_iterator &other) const
{
	return ( _it != other._it );
}
