/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DContainerMap                                     #
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
#include <climits>
#include "Geometry/kmbPoint3DContainer.h"
#include "Common/kmbCalculator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif



const char* kmb::Point3DContainerMap::CONTAINER_TYPE = "stl::map<id,Point3D*>";

kmb::Point3DContainerMap::Point3DContainerMap(void)
: minId(INT_MAX)
, maxId(-1)
, idContinuity(kmb::Point3DContainerMap::ZERO_LEADING)
{
}

kmb::Point3DContainerMap::~Point3DContainerMap(void)
{
	clear();
}

bool
kmb::Point3DContainerMap::initialize(size_t size)
{
	clear();
	return true;
}

kmb::nodeIdType kmb::Point3DContainerMap::addPoint(const double x,const double y,const double z)
{
	kmb::Point3D* point = NULL;
	point = new kmb::Point3D(x,y,z);
	if( point ){
		boundBox.update( *point );
		++maxId;
		points.insert( std::pair<kmb::nodeIdType,kmb::Point3D*>(maxId, point ) );
		if( minId > maxId ){
			minId = maxId;
		}
		return maxId;
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType kmb::Point3DContainerMap::addPoint(const kmb::Point3D& point)
{
	return this->addPoint( point.x(), point.y(), point.z() );
}

kmb::nodeIdType kmb::Point3DContainerMap::addPoint
(const double x,const double y,const double z,const kmb::nodeIdType id)
{
	if( id < 0 ){
		return kmb::nullNodeId;
	}
	if( points.find(id) == points.end() ){
		kmb::Point3D* point = NULL;
		point = new kmb::Point3D(x,y,z);
		if( point ){
			boundBox.update( *point );
			if( maxId == -1 )
			{

				if( id == 1 ){
					this->idContinuity = kmb::Point3DContainerMap::ONE_LEADING;
				}else if( id > 1 ){
					this->idContinuity = kmb::Point3DContainerMap::OTHER_LEADING;
				}
			}else if( id != this->maxId + 1 ){

				this->idContinuity = kmb::Point3DContainerMap::NOT_CONTINUOUS;
			}


			if( maxId < id ){
				maxId = id;
			}
			if( minId > id ){
				minId = id;
			}
			points.insert( std::pair<kmb::nodeIdType,kmb::Point3D*>(id, point ) );
			return id;
		}
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType kmb::Point3DContainerMap::addPoint
(const kmb::Point3D& point,const kmb::nodeIdType id)
{
	return this->addPoint( point.x(), point.y(), point.z(), id );
}

bool
kmb::Point3DContainerMap::getXYZ(nodeIdType id,double &x,double &y,double &z) const
{
	std::map< kmb::nodeIdType, kmb::Point3D* >::const_iterator it = points.find(id);
	if( it != points.end() ){
		x = it->second->x();
		y = it->second->y();
		z = it->second->z();
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point3DContainerMap::getPoint(kmb::nodeIdType id,kmb::Point3D &point) const
{
	std::map< kmb::nodeIdType, kmb::Point3D* >::const_iterator it = points.find(id);
	if( it != points.end() ){
		point.set( *it->second );
		return true;
	}else{
		return false;
	}
}


kmb::nodeIdType kmb::Point3DContainerMap::getMaxId(void) const
{
	return maxId;
}

kmb::nodeIdType kmb::Point3DContainerMap::getMinId(void) const
{
	return minId;
}

size_t
kmb::Point3DContainerMap::getCount(void) const
{
	return points.size();
}

void kmb::Point3DContainerMap::clear(void)
{
	std::map< kmb::nodeIdType, kmb::Point3D* >::iterator eIter = points.begin();
	while( eIter != points.end() )
	{
		kmb::Point3D* point = eIter->second;
		if( point )
		{
			delete point;
			point = NULL;
		}
		++eIter;
	}
	points.clear();
	minId = INT_MAX;
	maxId = -1;
	boundBox.initialize();
}

bool
kmb::Point3DContainerMap::deletePoint(nodeIdType id)
{
	std::map< kmb::nodeIdType, kmb::Point3D* >::const_iterator it = points.find(id);
	if( it != points.end() )
	{
		kmb::Point3D* point = it->second;
		delete point;
		points.erase( id );
		updateMinMaxId();
		return true;
	}
	return false;
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerMap::begin(void)
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::iterator(NULL);
	}
	kmb::Point3DContainerMap::_iteratorMap* _it = NULL;
	_it = new kmb::Point3DContainerMap::_iteratorMap();
	if( _it ){
		_it->pointIter = this->points.begin();
		_it->endIterator = this->points.end();
	}
	return kmb::Point3DContainer::iterator(_it);
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerMap::begin(void) const
{
	if( this->getCount() == 0){
		return kmb::Point3DContainer::const_iterator(NULL);
	}
	kmb::Point3DContainerMap::_iteratorMap* _it = NULL;
	_it = new kmb::Point3DContainerMap::_iteratorMap();
	if( _it ){
		_it->pointIter = this->points.begin();
		_it->endIterator = this->points.end();
	}
	return kmb::Point3DContainer::const_iterator(_it);
}

kmb::Point3DContainer::iterator
kmb::Point3DContainerMap::find(kmb::nodeIdType nodeId)
{
	std::map< kmb::nodeIdType, kmb::Point3D* >::iterator it = points.find(nodeId);
	if( it == points.end() ){
		return kmb::Point3DContainer::iterator(NULL);
	}else{
		kmb::Point3DContainerMap::_iteratorMap* _it = NULL;
		_it = new kmb::Point3DContainerMap::_iteratorMap();
		if( _it ){
			_it->pointIter = it;
			_it->endIterator = this->points.end();
		}
		return kmb::Point3DContainer::iterator(_it);
	}
}

kmb::Point3DContainer::const_iterator
kmb::Point3DContainerMap::find(kmb::nodeIdType nodeId) const
{
	std::map< kmb::nodeIdType, kmb::Point3D* >::const_iterator it = points.find(nodeId);
	if( it == points.end() ){
		return kmb::Point3DContainer::const_iterator(NULL);
	}else{
		kmb::Point3DContainerMap::_iteratorMap* _it = NULL;
		_it = new kmb::Point3DContainerMap::_iteratorMap();
		if( _it ){
			_it->pointIter = it;
			_it->endIterator = this->points.end();
		}
		return kmb::Point3DContainer::const_iterator(_it);
	}
}



bool
kmb::Point3DContainerMap::replaceId(nodeIdType oldid,nodeIdType newid)
{
	if( oldid >= 0 && newid >= 0 &&
		points.find(oldid) != points.end() &&
		points.find(newid) == points.end() )
	{
		kmb::Point3D* point = points[ oldid ];
		points[oldid] = NULL;
		points.erase(oldid);
		points.insert( std::pair<int,kmb::Point3D*>(newid,point) );
		updateMinMaxId();
		return true;
	}
	else
	{
		return false;
	}
}

bool
kmb::Point3DContainerMap::deleteId(nodeIdType id)
{
	if( points.find(id) != points.end() ){
		kmb::Point3D* point = points[id];
		if(point != NULL){
			delete point;
		}
		points.erase(id);
		return true;
	}else{
		return false;
	}
}

void
kmb::Point3DContainerMap::updateMinMaxId(void)
{
	maxId = -1;
	minId = INT_MAX;
	std::map< kmb::nodeIdType, kmb::Point3D* >::iterator pIter = points.begin();
	while( pIter != points.end() )
	{
		nodeIdType id = pIter->first;
		if( id < minId )	minId = id;
		if( id > maxId )	maxId = id;
		++pIter;
	}

	if( this->maxId - this->minId + 1 != static_cast<int>(points.size()) ){
		this->idContinuity = kmb::Point3DContainerMap::NOT_CONTINUOUS;
	}else if( this->minId == 0 ){
		this->idContinuity = kmb::Point3DContainerMap::ZERO_LEADING;
	}else if( this->minId == 1 ){
		this->idContinuity = kmb::Point3DContainerMap::ONE_LEADING;
	}else{
		this->idContinuity = kmb::Point3DContainerMap::OTHER_LEADING;
	}
}

void
kmb::Point3DContainerMap::idDefragment(nodeIdType initId, std::map<kmb::nodeIdType,kmb::nodeIdType>& idmap)
{


	if( initId == 0 && this->idContinuity == kmb::Point3DContainerMap::ZERO_LEADING){
		return;
	}
	if( initId == 1 && this->idContinuity == kmb::Point3DContainerMap::ONE_LEADING){
		return;
	}


	nodeIdType newId = initId;



	for(nodeIdType i = 0;i <= this->maxId ;++i){
		if( points.find(i) == points.end() ){
			continue;
		}

		while( points.find(newId) != points.end() ){
			++newId;
		}
		if( initId <= i && i < newId ){



		}else{
			idmap[i] = newId;
			if( i != newId ){
				kmb::Point3D* point = points[i];
				points[i] = NULL;
				points.erase(i);
				points.insert( std::pair<kmb::nodeIdType,kmb::Point3D*>(newId,point) );
			}
		}
	}

	this->minId = initId;
	this->maxId = initId + static_cast<int>(points.size()) - 1;
	if( initId == 1 ){
		this->idContinuity = kmb::Point3DContainerMap::ONE_LEADING;
	}else if( initId == 0 ){
		this->idContinuity = kmb::Point3DContainerMap::ZERO_LEADING;
	}else{
		this->idContinuity = kmb::Point3DContainerMap::OTHER_LEADING;
	}
}


kmb::Point3D*
kmb::Point3DContainerMap::erasePoint(kmb::nodeIdType id)
{
	std::map< kmb::nodeIdType, kmb::Point3D* >::iterator it = this->points.find(id);
	if( it != this->points.end() ){
		kmb::Point3D* point = it->second;
		points.erase( id );
		updateMinMaxId();
		return point;
	}else{
		return NULL;
	}
}


kmb::nodeIdType
kmb::Point3DContainerMap::_iteratorMap::getId(void) const
{
	return pointIter->first;
}

bool
kmb::Point3DContainerMap::_iteratorMap::getXYZ(double &x,double &y,double &z) const
{
	if( pointIter->second != NULL ){
		x = pointIter->second->x();
		y = pointIter->second->y();
		z = pointIter->second->z();
		return true;
	}
	return false;
}

bool
kmb::Point3DContainerMap::_iteratorMap::getPoint(kmb::Point3D &point) const
{
	if( pointIter->second != NULL ){
		point.set( *pointIter->second );
		return true;
	}
	return false;
}

double
kmb::Point3DContainerMap::_iteratorMap::x() const
{
	return pointIter->second->x();
}

double
kmb::Point3DContainerMap::_iteratorMap::y() const
{
	return pointIter->second->y();
}

double
kmb::Point3DContainerMap::_iteratorMap::z() const
{
	return pointIter->second->z();
}

bool
kmb::Point3DContainerMap::_iteratorMap::setPoint(kmb::Point3D &point) const
{
	if( pointIter->second != NULL ){
		pointIter->second->set( point );
		return true;
	}
	return false;
}

bool
kmb::Point3DContainerMap::_iteratorMap::setXYZ(double x,double y,double z) const
{
	if( pointIter->second != NULL ){
		pointIter->second->x(x);
		pointIter->second->y(y);
		pointIter->second->z(z);
		return true;
	}
	return false;
}

kmb::Point3DContainerMap::_iterator*
kmb::Point3DContainerMap::_iteratorMap::operator++(void)
{
	++pointIter;
	if( pointIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Point3DContainerMap::_iterator*
kmb::Point3DContainerMap::_iteratorMap::operator++(int n)
{
	pointIter++;
	if( pointIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Point3DContainer::_iterator*
kmb::Point3DContainerMap::_iteratorMap::clone(void)
{
	kmb::Point3DContainerMap::_iteratorMap* _itClone = NULL;
	_itClone = new kmb::Point3DContainerMap::_iteratorMap();
	if( _itClone ){
		_itClone->pointIter = this->pointIter;
		_itClone->endIterator = this->endIterator;
	}
	return _itClone;
}
