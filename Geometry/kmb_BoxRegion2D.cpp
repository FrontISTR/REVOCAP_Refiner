/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : BoxRegion2D                                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
#include "Geometry/kmb_BoxRegion2D.h"

#include <cmath>

kmb::BoxRegion2D::BoxRegion2D(kmb::Point2D &l,kmb::Point2D &u)
{
	kmb::BoxRegion2D(l.x(), l.y(), u.x(), u.y() );
}

kmb::BoxRegion2D::
BoxRegion2D(double x0,double y0,double x1,double y1)
{
	if(x0 < x1){
		this->minPoint.x( x0 );
		this->maxPoint.x( x1 );
	}else{
		this->minPoint.x( x1 );
		this->maxPoint.x( x0 );
	}

	if(y0 < y1){
		this->minPoint.y( y0 );
		this->maxPoint.y( y1 );
	}else{
		this->minPoint.y( y1 );
		this->maxPoint.y( y0 );
	}
}

kmb::BoxRegion2D::BoxRegion2D(const kmb::BoxRegion2D &other)
: maxPoint(other.maxPoint)
, minPoint(other.minPoint)
{
}

kmb::BoxRegion2D::~BoxRegion2D(void)
{
}

kmb::BoxRegion2D&
kmb::BoxRegion2D::operator=(const kmb::BoxRegion2D &other)
{
	maxPoint = other.maxPoint;
	minPoint = other.minPoint;
	return *this;
}

double
kmb::BoxRegion2D::minX(void) const
{
	return this->minPoint.x();
}

double
kmb::BoxRegion2D::minY(void) const
{
	return this->minPoint.y();
}

double
kmb::BoxRegion2D::maxX(void) const
{
	return this->maxPoint.x();
}

double
kmb::BoxRegion2D::maxY(void) const
{
	return this->maxPoint.y();
}

double
kmb::BoxRegion2D::centerX(void) const
{
	return 0.5*(this->maxPoint.x()+this->minPoint.x());
}

double
kmb::BoxRegion2D::centerY(void) const
{
	return 0.5*(this->maxPoint.y()+this->minPoint.y());
}

double
kmb::BoxRegion2D::rangeX(void) const
{
	return this->maxPoint.x() - this->minPoint.x();
}

double
kmb::BoxRegion2D::rangeY(void) const
{
	return this->maxPoint.y() - this->minPoint.y();
}

double
kmb::BoxRegion2D::diameter(void) const
{
	return maxPoint.distance( minPoint );
}

const kmb::Point2D&
kmb::BoxRegion2D::getMin(void) const
{
	return this->minPoint;
}

const kmb::Point2D&
kmb::BoxRegion2D::getMax(void) const
{
	return this->maxPoint;
}

void
kmb::BoxRegion2D::getCenter(Point2D& center) const
{
	center.setCoordinate(
		( maxPoint.x() + minPoint.x() ) * 0.5,
		( maxPoint.y() + minPoint.y() ) * 0.5 );
}

double
kmb::BoxRegion2D::getArea(void) const
{
	return rangeX() * rangeY();
}

kmb::Region2D::locationType
kmb::BoxRegion2D::intersect(const kmb::Point2D &point) const
{
	kmb::Region2D::locationType x,y;
	if( this->minPoint.x() < point.x() && point.x() < this->maxPoint.x() ){
		x = kmb::Region2D::INSIDE;
	}else if( this->minPoint.x() == point.x() || point.x() == this->maxPoint.x() ){
		x = kmb::Region2D::ON_BOUNDARY;
	}else{
		x = kmb::Region2D::OUTSIDE;
	}

	if( this->minPoint.y() < point.y() && point.y() < this->maxPoint.y() ){
		y = kmb::Region2D::INSIDE;
	}else if( this->minPoint.y() == point.y() || point.y() == this->maxPoint.y() ){
		y = kmb::Region2D::ON_BOUNDARY;
	}else{
		y = kmb::Region2D::OUTSIDE;
	}

	if( x == kmb::Region2D::OUTSIDE ||
		y == kmb::Region2D::OUTSIDE )
	{
		return kmb::Region2D::OUTSIDE;
	}else if(
		x == kmb::Region2D::ON_BOUNDARY ||
		y == kmb::Region2D::ON_BOUNDARY )
	{
		return kmb::Region2D::ON_BOUNDARY;
	}else{
		return kmb::Region2D::INSIDE;
	}
}

double
kmb::BoxRegion2D::distanceSq(const kmb::Point2D& point) const
{
	double xdiff=0;
	double ydiff=0;

	if( point.x() < minPoint.x() ){
		xdiff = point.x() - minPoint.x();
	}
	if( point.x() < maxPoint.x() ){
		xdiff = 0.0;
	}else{
		xdiff = point.x() - maxPoint.x();
	}

	if( point.y() < minPoint.y() ){
		ydiff = point.y() - minPoint.y();
	}
	if( point.y() < maxPoint.y() ){
		ydiff = 0.0;
	}else{
		ydiff = point.y() - maxPoint.y();
	}
	return xdiff*xdiff + ydiff*ydiff;
}

double
kmb::BoxRegion2D::distanceSq(const double x,const double y) const
{
	return this->distanceSq( kmb::Point2D(x,y) );
}

bool
kmb::BoxRegion2D::intersect(const kmb::BoxRegion2D &box) const
{
	double lowX = ( minX() < box.minX() ? box.minX() : minX() );
	double lowY = ( minY() < box.minY() ? box.minY() : minY() );
	double highX = ( maxX() > box.maxX() ? box.maxX() : maxX() );
	double highY = ( maxY() > box.maxY() ? box.maxY() : maxY() );
	if( lowX < highX && lowY < highY ){
		return true;
	}else{
		return false;
	}
/*

	if( ( box.maxPoint.x() < minPoint.x() && box.maxPoint.y() < minPoint.y() ) ||
		( box.minPoint.x() < maxPoint.x() && box.minPoint.y() < maxPoint.y() ) ){
			return false;
	}


	bool lowx = ( minPoint.x() < box.minPoint.x() && box.minPoint.x() < maxPoint.x() );
	bool lowy = ( minPoint.y() < box.minPoint.y() && box.minPoint.y() < maxPoint.y() );
	bool highx = ( minPoint.x() < box.maxPoint.x() && box.maxPoint.x() < maxPoint.x() );
	bool highy = ( minPoint.y() < box.maxPoint.y() && box.maxPoint.y() < maxPoint.y() );
	return
		( lowx && lowy ) ||
		( lowx && highy ) ||
		( highx && lowy ) ||
		( highx && highy );
*/
}

double
kmb::BoxRegion2D::distanceSq(const kmb::BoxRegion2D& box) const
{
	return this->maxPoint.distanceSq( box.maxPoint ) + this->minPoint.distanceSq( box.minPoint );
}

double
kmb::BoxRegion2D::intersectArea(const BoxRegion2D& box) const
{

	double lowX = ( minX() < box.minX() ? box.minX() : minX() );
	double lowY = ( minY() < box.minY() ? box.minY() : minY() );
	double highX = ( maxX() > box.maxX() ? box.maxX() : maxX() );
	double highY = ( maxY() > box.maxY() ? box.maxY() : maxY() );
	if( lowX < highX && lowY < highY ){
		return (highX - lowX) * (highY - lowY);
	}else{
		return 0.0;
	}
}

void
kmb::BoxRegion2D::expand(double x,double y)
{
	kmb::Vector2D diff(x,y);
	this->maxPoint += diff;
	this->minPoint -= diff;
}

void
kmb::BoxRegion2D::expand(double ratio)
{
	if( ratio > 0.0 && ratio != 1.0)
	{
		double maxx = maxPoint.x();
		double minx = minPoint.x();
		maxPoint.x( 0.5*(1+ratio)*maxx + 0.5*(1-ratio)*minx );
		minPoint.x( 0.5*(1-ratio)*maxx + 0.5*(1+ratio)*minx );
		double maxy = maxPoint.y();
		double miny = minPoint.y();
		maxPoint.y( 0.5*(1+ratio)*maxy + 0.5*(1-ratio)*miny );
		minPoint.y( 0.5*(1-ratio)*maxy + 0.5*(1+ratio)*miny );
	}
}
