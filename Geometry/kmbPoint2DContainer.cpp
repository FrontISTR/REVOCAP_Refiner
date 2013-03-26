/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point2DContainer                                        #
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
#include <cstring>
#include "Geometry/kmbPoint2DContainer.h"
#include "Common/kmbCalculator.h"

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

bool
kmb::Point2DContainer::copy(const kmb::Point2DContainer* org, kmb::Point2DContainer* points)
{
	if( org == NULL || points == NULL ){
		return false;
	}
	bool ret = true;
	points->initialize( org->getCount() );
	kmb::Point2D point;
	kmb::Point2DContainer::const_iterator pIter = org->begin();
	while( !pIter.isFinished() && ret ){
		ret &= pIter.getPoint( point );
		if( ret ){
			points->addPoint( point, pIter.getId() );
		}
		++pIter;
	}
	return ret;
}

kmb::Point2DContainer*
kmb::Point2DContainer::createContainer( const char* containerType )
{
	if( containerType == NULL ){
		return NULL;
	}
	if(0==strcmp(containerType,kmb::Point2DContainerMap::CONTAINER_TYPE) ){
		return new kmb::Point2DContainerMap();
	}
	else if(0==strcmp(containerType,kmb::Point2DContainerVect::CONTAINER_TYPE) ){
		return new kmb::Point2DContainerVect();
	}
	else{
		return NULL;
	}
}

kmb::Point2DContainer::Point2DContainer(void)
{
}

kmb::Point2DContainer::~Point2DContainer(void)
{
}

const kmb::Point2DContainer::iterator kmb::Point2DContainer::endIterator;
const kmb::Point2DContainer::const_iterator kmb::Point2DContainer::endConstIterator;

void
kmb::Point2DContainer::copyPreservingId(Point2DContainer& container)
{
	this->clear();
	kmb::Point2D point;
	kmb::Point2DContainer::iterator pIter = container.begin();
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
kmb::Point2DContainer::copyWithoutPreservingId(Point2DContainer& container)
{
	this->clear();
	kmb::Point2D point;
	kmb::Point2DContainer::iterator pIter = container.begin();
	while( pIter != container.end() )
	{
		if( pIter.getPoint( point ) ){
			this->addPoint( point );
		}
		++pIter;
	}
}

kmb::nodeIdType
kmb::Point2DContainer::duplicatePoint(nodeIdType nodeId)
{
	double x,y;
	if( this->getXY( nodeId, x, y ) )
	{
		return this->addPoint( x, y );
	}
	else
	{
		return -1;
	}
}

void
kmb::Point2DContainer::convertAllNodes( kmb::Matrix3x3& mat )
{
	kmb::Point2DContainer::iterator pIter = this->begin();
	kmb::Point2DContainer::iterator pEnd = this->end();
	kmb::Point2D point;
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			mat.convert( point );
			pIter.setPoint( point );
		}
		++pIter;
	}
	kmb::Point2D bmax = boundBox.getMax();
	kmb::Point2D bmin = boundBox.getMin();
	mat.convert( bmax );
	mat.convert( bmin );
	boundBox.initialize();
	boundBox.update( bmax );
	boundBox.update( bmin );
}

void
kmb::Point2DContainer::translateAllNodes( double x,double y )
{
	kmb::Point2DContainer::iterator pIter = this->begin();
	kmb::Point2DContainer::iterator pEnd = this->end();
	kmb::Point2D point;
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			point.addCoordinate(x,y);
			pIter.setPoint( point );
		}
		++pIter;
	}
	kmb::Point2D bmax( boundBox.getMax() );
	kmb::Point2D bmin( boundBox.getMin() );
	bmax.addCoordinate(x,y);
	bmin.addCoordinate(x,y);
	boundBox.initialize();
	boundBox.update( bmax );
	boundBox.update( bmin );
}

void
kmb::Point2DContainer::scaleAllNodes( double r )
{
	kmb::Point2DContainer::iterator pIter = this->begin();
	kmb::Point2DContainer::iterator pEnd = this->end();
	kmb::Point2D point;
	while( pIter != pEnd ){
		if( pIter.getPoint( point ) ){
			point.scale(r);
			pIter.setPoint( point );
		}
		++pIter;
	}
	kmb::Point2D bmax( boundBox.getMax() );
	kmb::Point2D bmin( boundBox.getMin() );
	bmax.scale(r);
	bmin.scale(r);
	boundBox.initialize();
	boundBox.update( bmax );
	boundBox.update( bmin );
}

bool
kmb::Point2DContainer::updateCoordinate( kmb::nodeIdType nodeId, double x, double y )
{
	kmb::Point2DContainer::iterator pIter = this->find(nodeId);
	if( !pIter.isFinished() ){
		kmb::Point2D point(x,y);
		pIter.setPoint( point );
		return true;
	}
	return false;
}



kmb::Point2DContainer::iterator::iterator(const iterator &other)
: _it(NULL)
{
	if( other._it ){
		_it = other._it->clone();
	}
}

kmb::nodeIdType
kmb::Point2DContainer::iterator::getId(void) const
{
	if( _it != NULL )
		return _it->getId();
	else
		return -1;
}

bool
kmb::Point2DContainer::iterator::getXY(double &x,double &y) const
{
	if( _it != NULL )
		return _it->getXY(x,y);
	else
		return false;
}

bool
kmb::Point2DContainer::iterator::getPoint(kmb::Point2D &point) const
{
	if( _it != NULL )
		return _it->getPoint( point );
	else
		return false;
}


double
kmb::Point2DContainer::iterator::x(void) const
{
	return _it->x();
}

double
kmb::Point2DContainer::iterator::y(void) const
{
	return _it->y();
}

bool
kmb::Point2DContainer::iterator::setXY(double x,double y) const
{
	if( _it != NULL )
		return _it->setXY(x,y);
	else
		return false;
}

bool
kmb::Point2DContainer::iterator::setPoint(kmb::Point2D &point) const
{
	if( _it != NULL )
		return _it->setPoint( point );
	else
		return false;
}

kmb::Point2DContainer::iterator&
kmb::Point2DContainer::iterator::operator=(const kmb::Point2DContainer::iterator& other)
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


kmb::Point2DContainer::iterator&
kmb::Point2DContainer::iterator::operator++(void)
{
	if( _it != NULL ){
		kmb::Point2DContainer::_iterator* retVal = ++(*_it);
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
	}
	return *this;
}


kmb::Point2DContainer::iterator
kmb::Point2DContainer::iterator::operator++(int n)
{
	if( _it != NULL ){
		kmb::Point2DContainer::_iterator* _itClone = _it->clone();
		kmb::Point2DContainer::_iterator* retVal = (*_it)++;
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
		return kmb::Point2DContainer::iterator(_itClone);
	}else{
		return kmb::Point2DContainer::iterator(NULL);
	}
}

bool
kmb::Point2DContainer::iterator::operator==(const iterator &other) const
{
	return ( _it == other._it );
}

bool
kmb::Point2DContainer::iterator::operator!=(const iterator &other) const
{
	return ( _it != other._it );
}



kmb::Point2DContainer::const_iterator::const_iterator(const kmb::Point2DContainer::const_iterator &other)
: _it(NULL)
{
	if( other._it ){
		_it = other._it->clone();
	}
}

kmb::Point2DContainer::const_iterator::const_iterator(const kmb::Point2DContainer::iterator& other)
: _it(NULL)
{
	if( other._it ){
		_it = other._it->clone();
	}
}

kmb::nodeIdType
kmb::Point2DContainer::const_iterator::getId(void) const
{
	if( _it != NULL )
		return _it->getId();
	else
		return -1;
}

bool
kmb::Point2DContainer::const_iterator::getXY(double &x,double &y) const
{
	if( _it != NULL )
		return _it->getXY(x,y);
	else
		return false;
}

bool
kmb::Point2DContainer::const_iterator::getPoint(kmb::Point2D &point) const
{
	if( _it != NULL )
		return _it->getPoint( point );
	else
		return false;
}


double
kmb::Point2DContainer::const_iterator::x(void) const
{
	return _it->x();
}

double
kmb::Point2DContainer::const_iterator::y(void) const
{
	return _it->y();
}

kmb::Point2DContainer::const_iterator&
kmb::Point2DContainer::const_iterator::operator=(const kmb::Point2DContainer::const_iterator& other)
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

kmb::Point2DContainer::const_iterator&
kmb::Point2DContainer::const_iterator::operator=(const kmb::Point2DContainer::iterator& other)
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


kmb::Point2DContainer::const_iterator&
kmb::Point2DContainer::const_iterator::operator++(void)
{
	if( _it != NULL ){
		kmb::Point2DContainer::_iterator* retVal = ++(*_it);
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
	}
	return *this;
}


kmb::Point2DContainer::const_iterator
kmb::Point2DContainer::const_iterator::operator++(int n)
{
	if( _it != NULL ){
		kmb::Point2DContainer::_iterator* _itClone = _it->clone();
		kmb::Point2DContainer::_iterator* retVal = (*_it)++;
		if( retVal == NULL ){
			if( _it ) delete _it;
			_it = NULL;
		}
		return kmb::Point2DContainer::const_iterator(_itClone);
	}else{
		return kmb::Point2DContainer::const_iterator(NULL);
	}
}

bool
kmb::Point2DContainer::const_iterator::operator==(const const_iterator &other) const
{
	return ( _it == other._it );
}

bool
kmb::Point2DContainer::const_iterator::operator!=(const const_iterator &other) const
{
	return ( _it != other._it );
}



const char* kmb::Point2DContainerVect::CONTAINER_TYPE = "stl::vector<Point2D*>";

kmb::Point2DContainerVect::Point2DContainerVect(void)
{
}

kmb::Point2DContainerVect::~Point2DContainerVect(void)
{
	clear();
}

bool
kmb::Point2DContainerVect::initialize(size_t size)
{
	this->clear();
	this->points.reserve(size);
	return true;
}

kmb::nodeIdType
kmb::Point2DContainerVect::addPoint(const double x,const double y)
{
	kmb::Point2D* point = NULL;
	point = new kmb::Point2D(x,y);
	if( point ){
		boundBox.update( *point );
		points.push_back( point );
		return (int)(this->points.size() - 1);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType
kmb::Point2DContainerVect::addPoint(const kmb::Point2D& point)
{
	kmb::Point2D* point2 = NULL;
	point2 = new kmb::Point2D( point );
	if( point2 ){
		boundBox.update( *point2 );
		points.push_back( point2 );
		return (int)(this->points.size() - 1);
	}else{
		return kmb::nullNodeId;
	}
}

kmb::nodeIdType
kmb::Point2DContainerVect::addPoint(const double x,const double y,const nodeIdType id)
{
	if( id < 0 ){
		return -1;
	}else if( 0 <= id && id < static_cast<int>(this->points.size()) ){

		kmb::Point2D* pt = this->points[id];
		if( pt != NULL )
		{
			return kmb::nullNodeId;
		}
		kmb::Point2D* point = NULL;
		point =	new kmb::Point2D(x,y);
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
		return addPoint(x,y);
	}
}

kmb::nodeIdType
kmb::Point2DContainerVect::addPoint(const kmb::Point2D& point,const nodeIdType id)
{
	if( id < 0 ){
		return -1;
	}else if( 0 <= id && id < static_cast<int>(this->points.size()) ){

		kmb::Point2D* pt = this->points[id];
		if( pt != NULL )
		{
			return kmb::nullNodeId;
		}
		kmb::Point2D* point2 = NULL;
		point2 = new kmb::Point2D(point);
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
kmb::Point2DContainerVect::getXY(nodeIdType id,double &x,double &y) const
{
	if( 0 <= id && id < static_cast<int>(this->points.size()) ){
		x = this->points[ id ]->x();
		y = this->points[ id ]->y();
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point2DContainerVect::getPoint(nodeIdType id,kmb::Point2D &point) const
{
	if( 0 <= id && id < static_cast<int>(this->points.size()) ){
		point.set( *this->points[ id ] );
		return true;
	}else{
		return false;
	}
}

kmb::nodeIdType
kmb::Point2DContainerVect::getMaxId(void) const
{
	return (int)(this->points.size() - 1);
}

kmb::nodeIdType
kmb::Point2DContainerVect::getMinId(void) const
{
	return 0;
}

size_t
kmb::Point2DContainerVect::getCount(void) const
{
	return this->points.size();
}

void
kmb::Point2DContainerVect::clear(void)
{
	std::vector< kmb::Point2D* >::iterator pIter = this->points.begin();
	while( pIter != this->points.end() )
	{
		kmb::Point2D* pt = (*pIter);
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
kmb::Point2DContainerVect::deletePoint(nodeIdType id)
{
	if( 0 <= id && id < static_cast<int>(this->points.size()) ){
		kmb::Point2D* point = this->points[ id ];
		if( point != NULL )
		{
			delete point;
			this->points[id] = NULL;
			return true;
		}
	}
	return false;
}

void
kmb::Point2DContainerVect::getBoundingBox(kmb::BoundingBox2D& bound) const
{
	bound.update( boundBox.getMax() );
	bound.update( boundBox.getMin() );
}

double
kmb::Point2DContainerVect::getNearest
(const kmb::Point2D* target,kmb::Point2D*& result,nodeIdType &nearestId) const
{
	kmb::Minimizer min;
	const int len = static_cast<int>(points.size());
	for(nodeIdType i=0;i<len;++i){
		kmb::Point2D* point = points[i];
		if( point )
		{
			if( min.update( target->distanceSq( *point ) ) )
			{
				nearestId = i;
				result = point;
			}
		}
	}
	return sqrt( min.getMin() );
}

double
kmb::Point2DContainerVect::getNearest
(const double x,const double y,kmb::Point2D*& result,nodeIdType &nearestId) const
{
	kmb::Minimizer min;
	const int len = static_cast<int>(points.size());
	for(nodeIdType i=0;i<len;++i){
		kmb::Point2D* point = points[i];
		if( point )
		{
			if( min.update( point->distanceSq( x, y ) ) )
			{
				nearestId = i;
				result = point;
			}
		}
	}
	return sqrt( min.getMin() );
}

double
kmb::Point2DContainerVect::getNearestToSegment
(const kmb::Point2D& t0,const kmb::Point2D& t1,kmb::Point2D*& result,nodeIdType &nearestId,double &param,double d) const
{
	kmb::Minimizer min;
	min.update(d);
	nearestId = -1;
	double t;
	const int len = static_cast<int>(points.size());
	for(nodeIdType i=0;i<len;++i){
		kmb::Point2D* point = points[i];
		if( point )
		{
			if( min.update( point->distanceSqToSegment(t0,t1,t) ) )
			{
				param = t;
				nearestId = i;
				result = point;
			}
		}
	}
	return sqrt( min.getMin() );
}

kmb::Point2DContainer::iterator
kmb::Point2DContainerVect::begin(void)
{
	if( this->getCount() == 0){
		return kmb::Point2DContainer::iterator(NULL);
	}
	kmb::Point2DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point2DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = 0;
	}
	return kmb::Point2DContainer::iterator(_it);
}

kmb::Point2DContainer::const_iterator
kmb::Point2DContainerVect::begin(void) const
{
	if( this->getCount() == 0){
		return kmb::Point2DContainer::const_iterator(NULL);
	}
	kmb::Point2DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point2DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = 0;
	}
	return kmb::Point2DContainer::const_iterator(_it);
}

kmb::Point2DContainer::iterator
kmb::Point2DContainerVect::find(kmb::nodeIdType nodeId)
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->points.size()) ){
		return kmb::Point2DContainer::iterator(NULL);
	}
	kmb::Point2DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point2DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point2DContainer::iterator(_it);
}

kmb::Point2DContainer::const_iterator
kmb::Point2DContainerVect::find(kmb::nodeIdType nodeId) const
{
	if( this->getCount() == 0 || nodeId < 0 || nodeId >= static_cast<int>(this->points.size()) ){
		return kmb::Point2DContainer::const_iterator(NULL);
	}
	kmb::Point2DContainerVect::_iteratorVect* _it = NULL;
	_it = new kmb::Point2DContainerVect::_iteratorVect();
	if( _it ){
		_it->vect = this;
		_it->index = static_cast<int>(nodeId);
	}
	return kmb::Point2DContainer::const_iterator(_it);
}



kmb::nodeIdType
kmb::Point2DContainerVect::_iteratorVect::getId
(void) const
{
	return this->index;
}

bool
kmb::Point2DContainerVect::_iteratorVect::getPoint(kmb::Point2D &point) const
{
	if( this->vect != NULL ){
		return this->vect->getPoint( index, point );
	}else{
		return false;
	}
}

double
kmb::Point2DContainerVect::_iteratorVect::x(void) const
{
	return this->vect->points[index]->x();
}

double
kmb::Point2DContainerVect::_iteratorVect::y(void) const
{
	return this->vect->points[index]->y();
}

bool
kmb::Point2DContainerVect::_iteratorVect::setPoint(kmb::Point2D &point) const
{
	if( this->vect != NULL ){
		this->vect->points[ index ]->set( point );
		return true;
	}else{
		return false;
	}
}

bool
kmb::Point2DContainerVect::_iteratorVect::getXY(double &x,double &y) const
{
	if( this->vect != NULL ){
		return this->vect->getXY( index, x, y );
	}else{
		return false;
	}
}

bool
kmb::Point2DContainerVect::_iteratorVect::setXY(double x,double y) const
{
	if( this->vect != NULL ){
		this->vect->points[ index ]->x(x);
		this->vect->points[ index ]->y(y);
		return true;
	}else{
		return false;
	}
}

kmb::Point2DContainerVect::_iterator*
kmb::Point2DContainerVect::_iteratorVect::operator++(void)
{
	++(this->index);
	if( this->index < static_cast<int>( this->vect->points.size() ) ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point2DContainerVect::_iterator*
kmb::Point2DContainerVect::_iteratorVect::operator++(int n)
{
	this->index++;
	if( this->index < static_cast<int>( this->vect->points.size() ) ){
		return this;
	}else{


		return NULL;
	}
}

kmb::Point2DContainer::_iterator*
kmb::Point2DContainerVect::_iteratorVect::clone(void)
{
	kmb::Point2DContainerVect::_iteratorVect* _itClone = NULL;
	_itClone = new kmb::Point2DContainerVect::_iteratorVect();
	if( _itClone ){
		_itClone->vect = this->vect;
		_itClone->index = this->index;
	}
	return _itClone;
}
