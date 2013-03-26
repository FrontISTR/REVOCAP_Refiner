/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point2DContainerMap                                     #
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
#include "Geometry/kmbPoint2DContainer.h"
#include "Common/kmbCalculator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif



const char* kmb::Point2DContainerMap::CONTAINER_TYPE = "stl::map<id,Point2D*>";

kmb::Point2DContainerMap::Point2DContainerMap(void)
: minId(INT_MAX)
, maxId(-1)
, idContinuity(kmb::Point2DContainerMap::ZERO_LEADING)
{
}

kmb::Point2DContainerMap::~Point2DContainerMap(void)
{
	clear();
}

bool
kmb::Point2DContainerMap::initialize(size_t size)
{
	this->clear();
	return true;
}

kmb::nodeIdType kmb::Point2DContainerMap::addPoint(const double x,const double y)
{
	kmb::Point2D* point = NULL;
	point = new kmb::Point2D(x,y);
	if( point ){
		boundBox.update( *point );
		++maxId;
		points.insert( std::pair<kmb::nodeIdType,kmb::Point2D*>(maxId, point ) );
		if( minId > maxId ){
			minId = maxId;
		}
		return maxId;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType kmb::Point2DContainerMap::addPoint(const kmb::Point2D& point)
{
	return this->addPoint( point.x(), point.y() );
}

kmb::nodeIdType kmb::Point2DContainerMap::addPoint
(const double x,const double y,const kmb::nodeIdType id)
{
	if( id < 0 ){
		return -1;
	}
	if( points.find(id) == points.end() ){
		kmb::Point2D* point = NULL;
		point = new kmb::Point2D(x,y);
		if( point ){
			boundBox.update( *point );
			if( maxId == -1 )
			{

				if( id == 1 ){
					this->idContinuity = kmb::Point2DContainerMap::ONE_LEADING;
				}else if( id > 1 ){
					this->idContinuity = kmb::Point2DContainerMap::OTHER_LEADING;
				}
			}else if( id != this->maxId + 1 ){

				this->idContinuity = kmb::Point2DContainerMap::NOT_CONTINUOUS;
			}


			if( maxId < id ){
				maxId = id;
			}
			if( minId > id ){
				minId = id;
			}
			points.insert( std::pair<kmb::nodeIdType,kmb::Point2D*>(id, point ) );
			return id;
		}
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType kmb::Point2DContainerMap::addPoint
(const kmb::Point2D& point,const kmb::nodeIdType id)
{
	return this->addPoint( point.x(), point.y(), id );
}

bool
kmb::Point2DContainerMap::getXY(nodeIdType id,double &x,double &y) const
{
	std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator it = points.find(id);
	if( it != points.end() ){
		x = it->second->x();
		y = it->second->y();
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point2DContainerMap::getPoint(kmb::nodeIdType id,kmb::Point2D &point) const
{
	std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator it = points.find(id);
	if( it != points.end() ){
		point.set( *it->second );
		return true;
	}else{
		return false;
	}
}

kmb::nodeIdType kmb::Point2DContainerMap::getMaxId(void) const
{
	return maxId;
}

kmb::nodeIdType kmb::Point2DContainerMap::getMinId(void) const
{
	return minId;
}

size_t
kmb::Point2DContainerMap::getCount(void) const
{
	return points.size();
}

void kmb::Point2DContainerMap::clear(void)
{
	std::map< kmb::nodeIdType, kmb::Point2D* >::iterator eIter = points.begin();
	while( eIter != points.end() )
	{
		kmb::Point2D* point = eIter->second;
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
kmb::Point2DContainerMap::deletePoint(nodeIdType id)
{
	std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator it = points.find(id);
	if( it != points.end() )
	{
		kmb::Point2D* point = it->second;
		delete point;
		points.erase( id );
		updateMinMaxId();
		return true;
	}
	return false;
}

void kmb::Point2DContainerMap::getBoundingBox(kmb::BoundingBox2D& bound) const
{
	bound.update( boundBox.getMax() );
	bound.update( boundBox.getMin() );
}

double
kmb::Point2DContainerMap::getNearest
(const kmb::Point2D* target,kmb::Point2D*& result,nodeIdType &nearestId) const
{
	kmb::Minimizer min;
	std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator eIter = points.begin();
	while( eIter != points.end() )
	{
		kmb::Point2D* point = eIter->second;
		if( point )
		{
			if( min.update( target->distanceSq( *point ) ) )
			{
				nearestId = eIter->first;
				result = point;
			}
		}
		++eIter;
	}
	return sqrt( min.getMin() );
}

double
kmb::Point2DContainerMap::getNearest
(const double x,const double y,kmb::Point2D*& result,nodeIdType &nearestId) const
{
	kmb::Minimizer min;
	std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator eIter = points.begin();
	while( eIter != points.end() )
	{
		kmb::Point2D* point = eIter->second;
		if( point )
		{
			if( min.update( point->distanceSq(x,y) ) )
			{
				nearestId = eIter->first;
				result = point;
			}
		}
		++eIter;
	}
	return sqrt( min.getMin() );
}

double
kmb::Point2DContainerMap::getNearestToSegment
(const kmb::Point2D& t0,const kmb::Point2D& t1,kmb::Point2D*& result,nodeIdType &nearestId,double &param,double d) const
{
	kmb::Minimizer min;
	min.update(d);
	nearestId = -1;
	double t;

	std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator eIter = points.begin();
	while( eIter != points.end() )
	{
		kmb::Point2D* point = eIter->second;
		if( point )
		{
			if( min.update( point->distanceSqToSegment(t0,t1,t) ) )
			{
				nearestId = eIter->first;
				result = point;
				param = t;
			}
		}
		++eIter;
	}
	return sqrt( min.getMin() );
}

kmb::Point2DContainer::iterator
kmb::Point2DContainerMap::begin(void)
{
	if( this->getCount() == 0){
		return kmb::Point2DContainer::iterator(NULL);
	}
	kmb::Point2DContainerMap::_iteratorMap* _it = NULL;
	_it = new kmb::Point2DContainerMap::_iteratorMap();
	if( _it ){
		_it->pointIter = this->points.begin();
		_it->endIterator = this->points.end();
	}
	return kmb::Point2DContainer::iterator(_it);
}

kmb::Point2DContainer::const_iterator
kmb::Point2DContainerMap::begin(void) const
{
	if( this->getCount() == 0){
		return kmb::Point2DContainer::const_iterator(NULL);
	}
	kmb::Point2DContainerMap::_iteratorMap* _it = NULL;
	_it = new kmb::Point2DContainerMap::_iteratorMap();
	if( _it ){
		_it->pointIter = this->points.begin();
		_it->endIterator = this->points.end();
	}
	return kmb::Point2DContainer::const_iterator(_it);
}

kmb::Point2DContainer::iterator
kmb::Point2DContainerMap::find(kmb::nodeIdType nodeId)
{
	std::map< kmb::nodeIdType, kmb::Point2D* >::iterator it = points.find(nodeId);
	if( it == points.end() ){
		return kmb::Point2DContainer::iterator(NULL);
	}else{
		kmb::Point2DContainerMap::_iteratorMap* _it = NULL;
		_it = new kmb::Point2DContainerMap::_iteratorMap();
		if( _it ){
			_it->pointIter = it;
			_it->endIterator = this->points.end();
		}
		return kmb::Point2DContainer::iterator(_it);
	}
}

kmb::Point2DContainer::const_iterator
kmb::Point2DContainerMap::find(kmb::nodeIdType nodeId) const
{
	std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator it = points.find(nodeId);
	if( it == points.end() ){
		return kmb::Point2DContainer::const_iterator(NULL);
	}else{
		kmb::Point2DContainerMap::_iteratorMap* _it = NULL;
		_it = new kmb::Point2DContainerMap::_iteratorMap();
		if( _it ){
			_it->pointIter = it;
			_it->endIterator = this->points.end();
		}
		return kmb::Point2DContainer::const_iterator(_it);
	}
}


bool
kmb::Point2DContainerMap::replaceId(nodeIdType oldid,nodeIdType newid)
{
	if( oldid >= 0 && newid >= 0 &&
		points.find(oldid) != points.end() &&
		points.find(newid) == points.end() )
	{
		kmb::Point2D* point = points[ oldid ];
		points[oldid] = NULL;
		points.erase(oldid);
		points.insert( std::pair<int,kmb::Point2D*>(newid,point) );
		updateMinMaxId();
		return true;
	}
	else
	{
		return false;
	}
}

bool
kmb::Point2DContainerMap::deleteId(nodeIdType id)
{
	if( points.find(id) != points.end() ){
		kmb::Point2D* point = points[id];
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
kmb::Point2DContainerMap::updateMinMaxId(void)
{
	maxId = -1;
	minId = INT_MAX;
	std::map< kmb::nodeIdType, kmb::Point2D* >::iterator pIter = points.begin();
	while( pIter != points.end() )
	{
		nodeIdType id = pIter->first;
		if( id < minId )	minId = id;
		if( id > maxId )	maxId = id;
		++pIter;
	}

	if( this->maxId - this->minId + 1 != static_cast<int>(points.size()) ){
		this->idContinuity = kmb::Point2DContainerMap::NOT_CONTINUOUS;
	}else if( this->minId == 0 ){
		this->idContinuity = kmb::Point2DContainerMap::ZERO_LEADING;
	}else if( this->minId == 1 ){
		this->idContinuity = kmb::Point2DContainerMap::ONE_LEADING;
	}else{
		this->idContinuity = kmb::Point2DContainerMap::OTHER_LEADING;
	}
}

void
kmb::Point2DContainerMap::idDefragment(nodeIdType initId, std::map<kmb::nodeIdType,kmb::nodeIdType>& idmap)
{


	if( initId == 0 && this->idContinuity == kmb::Point2DContainerMap::ZERO_LEADING){
		return;
	}
	if( initId == 1 && this->idContinuity == kmb::Point2DContainerMap::ONE_LEADING){
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
				kmb::Point2D* point = points[i];
				points[i] = NULL;
				points.erase(i);
				points.insert( std::pair<kmb::nodeIdType,kmb::Point2D*>(newId,point) );
			}
		}
	}

	this->minId = initId;
	this->maxId = initId + static_cast<int>(points.size()) - 1;
	if( initId == 1 ){
		this->idContinuity = kmb::Point2DContainerMap::ONE_LEADING;
	}else if( initId == 0 ){
		this->idContinuity = kmb::Point2DContainerMap::ZERO_LEADING;
	}else{
		this->idContinuity = kmb::Point2DContainerMap::OTHER_LEADING;
	}
}


kmb::Point2D*
kmb::Point2DContainerMap::erasePoint(kmb::nodeIdType id)
{
	std::map< kmb::nodeIdType, kmb::Point2D* >::iterator it = this->points.find(id);
	if( it != this->points.end() ){
		kmb::Point2D* point = it->second;
		points.erase( id );
		updateMinMaxId();
		return point;
	}else{
		return NULL;
	}
}


kmb::nodeIdType
kmb::Point2DContainerMap::_iteratorMap::getId(void) const
{
	return pointIter->first;
}

bool
kmb::Point2DContainerMap::_iteratorMap::getXY(double &x,double &y) const
{
	if( pointIter->second ){
		x = pointIter->second->x();
		y = pointIter->second->y();
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point2DContainerMap::_iteratorMap::setXY(double x,double y) const
{
	if( pointIter->second ){
		pointIter->second->x(x);
		pointIter->second->y(y);
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point2DContainerMap::_iteratorMap::getPoint(kmb::Point2D &point) const
{
	if( pointIter->second ){
		point.set( *pointIter->second );
		return true;
	}else{
		return false;
	}
}

double
kmb::Point2DContainerMap::_iteratorMap::x(void) const
{
	return  pointIter->second->x();
}

double
kmb::Point2DContainerMap::_iteratorMap::y(void) const
{
	return  pointIter->second->y();
}

bool
kmb::Point2DContainerMap::_iteratorMap::setPoint(kmb::Point2D &point) const
{
	if( pointIter->second ){
		pointIter->second->set( point );
		return true;
	}else{
		return false;
	}
}

kmb::Point2DContainerMap::_iterator*
kmb::Point2DContainerMap::_iteratorMap::operator++(void)
{
	++pointIter;
	if( pointIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Point2DContainerMap::_iterator*
kmb::Point2DContainerMap::_iteratorMap::operator++(int n)
{
	pointIter++;
	if( pointIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::Point2DContainer::_iterator*
kmb::Point2DContainerMap::_iteratorMap::clone(void)
{
	kmb::Point2DContainerMap::_iteratorMap* _itClone = NULL;
	_itClone = new kmb::Point2DContainerMap::_iteratorMap();
	if( _itClone ){
		_itClone->pointIter = this->pointIter;
		_itClone->endIterator = this->endIterator;
	}
	return _itClone;
}
