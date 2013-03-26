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
#include <cmath>
#include "Geometry/kmbPoint3DContainerVect.h"
#include "Common/kmbCalculator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::Point3DContainerVect::CONTAINER_TYPE = "stl::vector<Point3D*>";

kmb::Point3DContainerVect::Point3DContainerVect(void)
{
}

kmb::Point3DContainerVect::~Point3DContainerVect(void)
{
	clear();
}

bool
kmb::Point3DContainerVect::initialize(size_t size)
{
	clear();
	points.reserve(size);
	return true;
}

kmb::nodeIdType
kmb::Point3DContainerVect::addPoint(const double x,const double y,const double z)
{
	kmb::Point3D* point = NULL;
	point = new kmb::Point3D(x,y,z);
	if( point ){
		boundBox.update( *point );
		points.push_back( point );
		return (int)(this->points.size() - 1);
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::Point3DContainerVect::addPoint(const kmb::Point3D& point)
{
	kmb::Point3D* point2 = NULL;
	point2 = new kmb::Point3D( point );
	if( point2 ){
		boundBox.update( *point2 );
		points.push_back( point2 );
		return (int)(this->points.size() - 1);
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::Point3DContainerVect::addPoint(const double x,const double y,const double z,const nodeIdType id)
{
	if( id < 0 ){
		return -1;
	}else if( 0 <= id && id < static_cast<int>(this->points.size()) ){

		kmb::Point3D* pt = this->points[id];
		if( pt != NULL )
		{
			return kmb::nullNodeId;
		}
		kmb::Point3D* point = NULL;
		point = new kmb::Point3D(x,y,z);
		if( point ){
			boundBox.update( *point );
			this->points[id] = point;
			return id;
		}else{
			return kmb::nullNodeId;
		}
	}else{
		while( static_cast<int>(points.size()) < id ){
			this->points.push_back( NULL );
		}
		return addPoint(x,y,z);
	}
}

kmb::nodeIdType
kmb::Point3DContainerVect::addPoint(const kmb::Point3D& point,const nodeIdType id)
{
	if( id < 0 ){
		return -1;
	}else if( 0 <= id && id < static_cast<int>(this->points.size()) ){

		kmb::Point3D* pt = this->points[id];
		if( pt != NULL )
		{
			return kmb::nullNodeId;
		}
		kmb::Point3D* point2 = NULL;
		point2 = new kmb::Point3D( point );
		if( point2 ){
			boundBox.update( *point2 );
			this->points[id] = point2;
			return id;
		}else{
			return kmb::nullNodeId;
		}
	}else{
		while( static_cast<int>(points.size()) < id ){
			this->points.push_back( NULL );
		}
		return addPoint(point);
	}
}

bool
kmb::Point3DContainerVect::getXYZ(nodeIdType id,double &x,double &y,double &z) const
{
	if( 0 <= id && id < static_cast<int>(this->points.size()) ){
		x = this->points[ id ]->x();
		y = this->points[ id ]->y();
		z = this->points[ id ]->z();
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point3DContainerVect::getPoint(nodeIdType id,kmb::Point3D &point) const
{
	if( 0 <= id && id < static_cast<int>(this->points.size()) ){
		point.set( *this->points[ id ] );
		return true;
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::Point3DContainerVect::getMaxId(void) const
{
	return (int)(this->points.size() - 1);
}

kmb::nodeIdType
kmb::Point3DContainerVect::getMinId(void) const
{
	return 0;
}

size_t
kmb::Point3DContainerVect::getCount(void) const
{
	return this->points.size();
}

void
kmb::Point3DContainerVect::clear(void)
{
	std::vector< kmb::Point3D* >::iterator pIter = this->points.begin();
	while( pIter != this->points.end() )
	{
		kmb::Point3D* pt = (*pIter);
		if( pt != NULL ){
			delete pt;
			pt = NULL;
		}
		++pIter;
	}
	this->points.clear();
	this->boundBox.initialize();
}

bool
kmb::Point3DContainerVect::deletePoint(nodeIdType id)
{
	if( 0 <= id && id < static_cast<int>(this->points.size()) ){
		kmb::Point3D* point = this->points[ id ];
		if( point != NULL )
		{
			delete point;
			this->points[id] = NULL;
			return true;
		}
	}
	return false;
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerVect::begin(void)
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point3DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = 0;
	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerVect::begin(void) const
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point3DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = 0;
	}
	return kmb::Point3DContainer::const_iterator(_it);
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerVect::find(kmb::nodeIdType nodeId)
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->points.size()) ){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point3DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerVect::find(kmb::nodeIdType nodeId) const
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->points.size()) ){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point3DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point3DContainer::const_iterator(_it);
}



kmb::nodeIdType
kmb::Point3DContainerVect::_iteratorVect::getId
(void) const
{
	return this->index;
}

bool
kmb::Point3DContainerVect::_iteratorVect::getXYZ(double &x,double &y,double &z) const
{
	if( this->vect != NULL ){
		x = this->vect->points[ index ]->x();
		y = this->vect->points[ index ]->y();
		z = this->vect->points[ index ]->z();
		return true;
	}
	return false;
}

bool
kmb::Point3DContainerVect::_iteratorVect::getPoint(kmb::Point3D &point) const
{
	if( this->vect != NULL ){
		point.set( *this->vect->points[ index ] );
		return true;
	}
	return false;
}

double
kmb::Point3DContainerVect::_iteratorVect::x() const
{
	return this->vect->points[ index ]->x();
}

double
kmb::Point3DContainerVect::_iteratorVect::y() const
{
	return this->vect->points[ index ]->y();
}

double
kmb::Point3DContainerVect::_iteratorVect::z() const
{
	return this->vect->points[ index ]->z();
}


bool
kmb::Point3DContainerVect::_iteratorVect::setXYZ(double x,double y,double z) const
{
	if( this->vect != NULL ){
		this->vect->points[ index ]->x(x);
		this->vect->points[ index ]->y(y);
		this->vect->points[ index ]->z(z);
		return true;
	}
	return false;
}

bool
kmb::Point3DContainerVect::_iteratorVect::setPoint(kmb::Point3D &point) const
{
	if( this->vect != NULL ){
		this->vect->points[ index ]->set( point );
		return true;
	}
	return false;
}

kmb::Point3DContainerVect::_iterator*
kmb::Point3DContainerVect::_iteratorVect::operator++(void)
{
	++(this->index);
	if( this->index < static_cast<int>( this->vect->points.size() ) ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point3DContainerVect::_iterator*
kmb::Point3DContainerVect::_iteratorVect::operator++(int n)
{
	this->index++;
	if( this->index < static_cast<int>( this->vect->points.size() ) ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerVect::_iteratorVect::clone(void)
{
	kmb::Point3DContainerVect::_iteratorVect* _itClone = NULL;
	_itClone = new kmb::Point3DContainerVect::_iteratorVect();
	if( _itClone ){
		_itClone->vect = this->vect;
		_itClone->index = this->index;
	}
	return _itClone;
}
