/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BoundingBox                                             #
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
#include "Geometry/kmbBoundingBox.h"
#include "Common/kmbCalculator.h"
#include <cfloat>

kmb::BoundingBox::BoundingBox(void)
: kmb::BoxRegion(DBL_MAX,DBL_MAX,DBL_MAX,-DBL_MAX,-DBL_MAX,-DBL_MAX)
{
	initialize();
}

kmb::BoundingBox::~BoundingBox(void)
{
}

void
kmb::BoundingBox::initialize(void)
{
	minPoint.setCoordinate(  DBL_MAX,  DBL_MAX,  DBL_MAX );
	maxPoint.setCoordinate( -DBL_MAX, -DBL_MAX, -DBL_MAX );
}

bool
kmb::BoundingBox::update(const Point3D& point)
{
	return update( point.x(), point.y(), point.z() );
}

bool
kmb::BoundingBox::update(const double x, const double y,const double z)
{
	bool flag = false;
	if( x < minPoint.x() ){
		minPoint.x( x );
		flag = true;
	}
	if( y < minPoint.y() ){
		minPoint.y( y );
		flag = true;
	}
	if( z < minPoint.z() ){
		minPoint.z( z );
		flag = true;
	}
	if( x > maxPoint.x() ){
		maxPoint.x( x );
		flag = true;
	}
	if( y > maxPoint.y() ){
		maxPoint.y( y );
		flag = true;
	}
	if( z > maxPoint.z() ){
		maxPoint.z( z );
		flag = true;
	}
	return flag;
}


bool
kmb::BoundingBox::update(const BoxRegion& box)
{
	bool flag = false;
	if( update( box.getMax() ) ){
		flag = true;
	}
	if( update( box.getMin() ) ){
		flag = true;
	}
	return flag;
}

bool
kmb::BoundingBox::valid(void) const
{
	return ( minPoint.x() <= maxPoint.x() && minPoint.y() <= maxPoint.y() && minPoint.z() <= maxPoint.z() );
}

kmb::BoundingBox2D::BoundingBox2D(void)
: kmb::BoxRegion2D(DBL_MAX,DBL_MAX,-DBL_MAX,-DBL_MAX)
{
	initialize();
}

kmb::BoundingBox2D::~BoundingBox2D(void)
{
}

void
kmb::BoundingBox2D::initialize(void)
{
	minPoint.setCoordinate(  DBL_MAX,  DBL_MAX );
	maxPoint.setCoordinate( -DBL_MAX, -DBL_MAX );
}

bool
kmb::BoundingBox2D::update(const Point2D& point)
{
	return update( point.x(), point.y() );
}

bool
kmb::BoundingBox2D::update(const double x, const double y)
{
	bool flag = false;
	if( x < minPoint.x() ){
		minPoint.x(x);
		flag = true;
	}
	if( y < minPoint.y() ){
		minPoint.y(y);
		flag = true;
	}
	if( x > maxPoint.x() ){
		maxPoint.x(x);
		flag = true;
	}
	if( y > maxPoint.y() ){
		maxPoint.y(y);
		flag = true;
	}
	return flag;
}

bool
kmb::BoundingBox2D::valid(void) const
{
	return ( minPoint.x() <= maxPoint.x() && minPoint.y() <= maxPoint.y() );
}

kmb::BoundingBox1D::BoundingBox1D(void)
{
	initialize();
}

kmb::BoundingBox1D::~BoundingBox1D(void)
{
}

void kmb::BoundingBox1D::initialize(void)
{
	this->minValue = DBL_MAX;
	this->maxValue = -DBL_MAX;
}

bool
kmb::BoundingBox1D::update(double value)
{
	bool flag = false;
	if( value > this->maxValue ){
		this->maxValue = value;
		flag = true;
	}
	if( value < this->minValue ){
		this->minValue = value;
		flag = true;
	}
	return flag;
}

double
kmb::BoundingBox1D::center(void) const
{
	return 0.5 * (maxValue - minValue);
}

double
kmb::BoundingBox1D::range(void) const
{
	return maxValue - minValue;
}

bool
kmb::BoundingBox1D::valid(void) const
{
	return ( minValue <= maxValue );
}

bool
kmb::BoundingBox1D::intersect(const kmb::BoundingBox1D& box) const
{
	double low = kmb::Maximizer::getMax( getMin(), box.getMin() );
	double high = kmb::Minimizer::getMin( getMax(), box.getMax() );
	if( low < high ){
		return true;
	}else{
		return false;
	}
}

double
kmb::BoundingBox1D::intersectLength(const kmb::BoundingBox1D& box) const
{
	double low = kmb::Maximizer::getMax( getMin(), box.getMin() );
	double high = kmb::Minimizer::getMin( getMax(), box.getMax() );
	if( low < high ){
		return high - low;
	}else{
		return 0.0;
	}
}

double
kmb::BoundingBox1D::distanceSq(const kmb::BoundingBox1D& box) const
{
	return
		( this->maxValue - box.getMax() ) * ( this->maxValue - box.getMax() ) +
		( this->minValue - box.getMin() ) * ( this->minValue - box.getMin() );
}
