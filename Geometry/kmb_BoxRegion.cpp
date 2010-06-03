/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : BoxRegion                                               #
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
#include "Geometry/kmb_BoxRegion.h"
#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_Plane.h"
#include <cmath>

double kmb::BoxRegion::thres = 1.0e-20;

kmb::BoxRegion::BoxRegion(void)
{
	kmb::BoxRegion( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
}

kmb::BoxRegion::BoxRegion(kmb::Point3D &l,kmb::Point3D &u)
{
	kmb::BoxRegion(l.x(), l.y(), l.z(), u.x(), u.y(), u.z());
}

kmb::BoxRegion::
BoxRegion(double x0,double y0,double z0,double x1,double y1,double z1)
{
	setMinMax(x0,y0,z0,x1,y1,z1);
}

kmb::BoxRegion::BoxRegion(const kmb::BoxRegion &other)
: maxPoint(other.maxPoint)
, minPoint(other.minPoint)
{
}

kmb::BoxRegion::~BoxRegion(void)
{
}

kmb::BoxRegion&
kmb::BoxRegion::operator=(const kmb::BoxRegion &other)
{
	maxPoint = other.maxPoint;
	minPoint = other.minPoint;
	return *this;
}

void
kmb::BoxRegion::setMinMax(double x0,double y0,double z0,double x1,double y1,double z1)
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

	if(z0 < z1){
		this->minPoint.z( z0 );
		this->maxPoint.z( z1 );
	}else{
		this->minPoint.z( z1 );
		this->maxPoint.z( z0 );
	}
}

void
kmb::BoxRegion::getCenter(kmb::Point3D& center) const
{
	center.setCoordinate(
		( maxPoint.x() + minPoint.x() ) * 0.5,
		( maxPoint.y() + minPoint.y() ) * 0.5,
		( maxPoint.z() + minPoint.z() ) * 0.5 );
}

double
kmb::BoxRegion::minX(void) const
{
	return this->minPoint.x();
}

double
kmb::BoxRegion::minY(void) const
{
	return this->minPoint.y();
}

double
kmb::BoxRegion::minZ(void) const
{
	return this->minPoint.z();
}

double
kmb::BoxRegion::maxX(void) const
{
	return this->maxPoint.x();
}

double
kmb::BoxRegion::maxY(void) const
{
	return this->maxPoint.y();
}

double
kmb::BoxRegion::maxZ(void) const
{
	return this->maxPoint.z();
}

double
kmb::BoxRegion::centerX(void) const
{
	return 0.5*(this->maxPoint.x()+this->minPoint.x());
}

double
kmb::BoxRegion::centerY(void) const
{
	return 0.5*(this->maxPoint.y()+this->minPoint.y());
}

double
kmb::BoxRegion::centerZ(void) const
{
	return 0.5*(this->maxPoint.z()+this->minPoint.z());
}

double
kmb::BoxRegion::rangeX(void) const
{
	return this->maxPoint.x() - this->minPoint.x();
}

double
kmb::BoxRegion::rangeY(void) const
{
	return this->maxPoint.y() - this->minPoint.y();
}

double
kmb::BoxRegion::rangeZ(void) const
{
	return this->maxPoint.z() - this->minPoint.z();
}

double
kmb::BoxRegion::diameter(void) const
{
	return maxPoint.distance( minPoint );
}

const kmb::Point3D&
kmb::BoxRegion::getMin(void) const
{
	return this->minPoint;
}

const kmb::Point3D&
kmb::BoxRegion::getMax(void) const
{
	return this->maxPoint;
}

double
kmb::BoxRegion::getVolume(void) const
{
	return rangeX() * rangeY() * rangeZ();
}

kmb::Region::locationType
kmb::BoxRegion::intersect(const kmb::Point3D &point) const
{
	return intersect( point.x(), point.y(), point.z() );
}

kmb::Region::locationType
kmb::BoxRegion::intersect(const double x,const double y,const double z) const
{
	kmb::Region::locationType tx,ty,tz;
	if( this->minPoint.x() < x && x < this->maxPoint.x() ){
		tx = kmb::Region::INSIDE;
	}else if( this->minPoint.x() == x || x == this->maxPoint.x() ){
		tx = kmb::Region::ON_BOUNDARY;
	}else{
		tx = kmb::Region::OUTSIDE;
	}

	if( this->minPoint.y() < y && y < this->maxPoint.y() ){
		ty = kmb::Region::INSIDE;
	}else if( this->minPoint.y() == y || y == this->maxPoint.y() ){
		ty = kmb::Region::ON_BOUNDARY;
	}else{
		ty = kmb::Region::OUTSIDE;
	}

	if( this->minPoint.z() < z && z < this->maxPoint.z() ){
		tz = kmb::Region::INSIDE;
	}else if( this->minPoint.z() == z || z == this->maxPoint.z() ){
		tz = kmb::Region::ON_BOUNDARY;
	}else{
		tz = kmb::Region::OUTSIDE;
	}

	if( tx == kmb::Region::OUTSIDE ||
		ty == kmb::Region::OUTSIDE ||
		tz == kmb::Region::OUTSIDE )
	{
		return kmb::Region::OUTSIDE;
	}else if(
		tx == kmb::Region::ON_BOUNDARY ||
		ty == kmb::Region::ON_BOUNDARY ||
		tz == kmb::Region::ON_BOUNDARY )
	{
		return kmb::Region::ON_BOUNDARY;
	}else{
		return kmb::Region::INSIDE;
	}
}

double
kmb::BoxRegion::distanceSq(const kmb::Point3D& point) const
{
	double xdiff=0;
	double ydiff=0;
	double zdiff=0;

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

	if( point.z() < minPoint.z() ){
		zdiff = point.z() - minPoint.z();
	}
	if( point.z() < maxPoint.z() ){
		zdiff = 0.0;
	}else{
		zdiff = point.z() - maxPoint.z();
	}
	return xdiff*xdiff + ydiff*ydiff + zdiff*zdiff;
}

double
kmb::BoxRegion::distanceSq(const double x,const double y,const double z) const
{
	return this->distanceSq( kmb::Point3D(x,y,z) );
}

bool
kmb::BoxRegion::intersect(const kmb::BoxRegion &box) const
{
	double lowX = ( minX() < box.minX() ? box.minX() : minX() );
	double lowY = ( minY() < box.minY() ? box.minY() : minY() );
	double lowZ = ( minZ() < box.minZ() ? box.minZ() : minZ() );
	double highX = ( maxX() > box.maxX() ? box.maxX() : maxX() );
	double highY = ( maxY() > box.maxY() ? box.maxY() : maxY() );
	double highZ = ( maxZ() > box.maxZ() ? box.maxZ() : maxZ() );
	if( lowX < highX && lowY < highY && lowZ < highZ ){
		return true;
	}else{
		return false;
	}
}

double
kmb::BoxRegion::getThreshold(void) const
{
	return this->thres;
}

void
kmb::BoxRegion::setThreshold(double thres)
{
	this->thres = thres;
}

bool
kmb::BoxRegion::intersect(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c) const
{

	if( intersect( a ) || intersect( b ) || intersect(c) ){
		return true;
	}
	double d = 0.0;
	kmb::Matrix3x3 mat;
	kmb::Vector3D v;
	kmb::Vector3D t;


	d = a.x() - b.x();
	if( fabs(d) > thres ){
		mat.zero();
		mat.set(1,0,rangeY());
		mat.set(2,1,rangeZ());


		mat.set(0,2,a.x()-b.x());
		mat.set(1,2,a.y()-b.y());
		mat.set(2,2,a.z()-b.z());
		v.zero();
		v.x( a.x()-minPoint.x() );
		v.y( a.y()-minPoint.y() );
		v.z( a.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.x( a.x()-maxPoint.x() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}


		mat.set(0,2,b.x()-c.x());
		mat.set(1,2,b.y()-c.y());
		mat.set(2,2,b.z()-c.z());
		v.zero();
		v.x( b.x()-minPoint.x() );
		v.y( b.y()-minPoint.y() );
		v.z( b.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.x( b.x()-maxPoint.x() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}


		mat.set(0,2,c.x()-a.x());
		mat.set(1,2,c.y()-a.y());
		mat.set(2,2,c.z()-a.z());
		v.zero();
		v.x( c.x()-minPoint.x() );
		v.y( c.y()-minPoint.y() );
		v.z( c.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.x( c.x()-maxPoint.x() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
	}

	d = a.y() - b.y();
	if( fabs(d) > thres ){
		mat.zero();
		mat.set(2,0,rangeZ());
		mat.set(0,1,rangeX());


		mat.set(0,2,a.x()-b.x());
		mat.set(1,2,a.y()-b.y());
		mat.set(2,2,a.z()-b.z());
		v.zero();
		v.x( a.x()-minPoint.x() );
		v.y( a.y()-minPoint.y() );
		v.z( a.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.y( a.y()-maxPoint.y() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}


		mat.set(0,2,b.x()-c.x());
		mat.set(1,2,b.y()-c.y());
		mat.set(2,2,b.z()-c.z());
		v.zero();
		v.x( b.x()-minPoint.x() );
		v.y( b.y()-minPoint.y() );
		v.z( b.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.y( b.y()-maxPoint.y() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}


		mat.set(0,2,c.x()-a.x());
		mat.set(1,2,c.y()-a.y());
		mat.set(2,2,c.z()-a.z());
		v.zero();
		v.x( c.x()-minPoint.x() );
		v.y( c.y()-minPoint.y() );
		v.z( c.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.y( c.y()-maxPoint.y() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
	}

	d = a.z() - b.z();
	if( fabs(d) > thres ){
		mat.zero();
		mat.set(0,0,rangeX());
		mat.set(1,1,rangeY());


		mat.set(0,2,a.x()-b.x());
		mat.set(1,2,a.y()-b.y());
		mat.set(2,2,a.z()-b.z());
		v.zero();
		v.x( a.x()-minPoint.x() );
		v.y( a.y()-minPoint.y() );
		v.z( a.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.z( a.z()-maxPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}


		mat.set(0,2,b.x()-c.x());
		mat.set(1,2,b.y()-c.y());
		mat.set(2,2,b.z()-c.z());
		v.zero();
		v.x( b.x()-minPoint.x() );
		v.y( b.y()-minPoint.y() );
		v.z( b.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.z( b.z()-maxPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}


		mat.set(0,2,c.x()-a.x());
		mat.set(1,2,c.y()-a.y());
		mat.set(2,2,c.z()-a.z());
		v.zero();
		v.x( c.x()-minPoint.x() );
		v.y( c.y()-minPoint.y() );
		v.z( c.z()-minPoint.z() );
		if( mat.solve( v, t ) &&
			-thres <= t.x() && t.x() <= 1.0 + thres &&
			-thres <= t.y() && t.y() <= 1.0 + thres &&
			-thres <= t.z() && t.z() <= 1.0 + thres )
		{
			return true;
		}
		v.z( c.z()-maxPoint.z() );
		if( mat.solve( v, t ) &&
			0.0 <= t.x() && t.x() <= 1.0 && 0.0 <= t.y() && t.y() <= 1.0 && 0.0 <= t.z() && t.z() <= 1.0 )
		{
			return true;
		}
	}

	mat.zero();
	mat.set(0,0,a.x()-b.x());
	mat.set(1,0,a.y()-b.y());
	mat.set(2,0,a.z()-b.z());
	mat.set(0,1,a.x()-c.x());
	mat.set(1,1,a.y()-c.y());
	mat.set(2,1,a.z()-c.z());
	mat.set(0,2,rangeX());
	v.zero();
	v.x( a.x()-minPoint.x() );
	v.y( a.y()-minPoint.y() );
	v.z( a.z()-minPoint.z() );
	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}

	v.y( a.y()-maxPoint.y() );

	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}


	v.z( a.z()-maxPoint.z() );
	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}

	v.y( a.y()-minPoint.y() );

	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}

	mat.set(0,2,0.0);
	mat.set(1,2,rangeY());
	v.zero();
	v.x( a.x()-minPoint.x() );
	v.y( a.y()-minPoint.y() );
	v.z( a.z()-minPoint.z() );
	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}
	v.x( a.x()-maxPoint.x() );


	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}


	v.z( a.z()-maxPoint.z() );
	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}
	v.x( a.x()-minPoint.x() );


	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}

	mat.set(1,2,0.0);
	mat.set(2,2,rangeZ());
	v.zero();
	v.x( a.x()-minPoint.x() );
	v.y( a.y()-minPoint.y() );
	v.z( a.z()-minPoint.z() );
	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}
	v.x( a.x()-maxPoint.x() );


	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}

	v.y( a.y()-maxPoint.y() );

	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}
	v.x( a.x()-minPoint.x() );


	if( mat.solve( v, t ) &&
		-thres <= t.x() && -thres <= t.y() && t.x() + t.y() <= 1.0 + thres &&
		-thres <= t.z() && t.z() <= 1.0 + thres )
	{
		return true;
	}
	return false;
}






double
kmb::BoxRegion::intersectArea(const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c) const
{
	return intersectArea_minx(a,b,c);
}

double
kmb::BoxRegion::intersectArea_minx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const
{
	kmb::Point3D p3, p4;
	switch( kmb::PlaneYZ::getIntersectionTriangle( minX(), p0, p1, p2, p3, p4 ) )
	{
	case -2: return 0.0;
	case -1: return kmb::BoxRegion::intersectArea_maxx(p0,p1,p2);
	case 0: return kmb::BoxRegion::intersectArea_maxx(p0,p3,p4);
	case 1: return kmb::BoxRegion::intersectArea_maxx(p1,p3,p4);
	case 2: return kmb::BoxRegion::intersectArea_maxx(p2,p3,p4);
	case 3: return kmb::BoxRegion::intersectArea_maxx(p1,p2,p4) + kmb::BoxRegion::intersectArea_maxx(p1,p4,p3);
	case 4: return kmb::BoxRegion::intersectArea_maxx(p2,p0,p4) + kmb::BoxRegion::intersectArea_maxx(p2,p4,p3);
	case 5: return kmb::BoxRegion::intersectArea_maxx(p0,p1,p4) + kmb::BoxRegion::intersectArea_maxx(p0,p4,p3);
	case 6: return kmb::BoxRegion::intersectArea_maxx(p0,p1,p3);
	case 7: return kmb::BoxRegion::intersectArea_maxx(p1,p2,p3);
	case 8: return kmb::BoxRegion::intersectArea_maxx(p2,p0,p3);
	case 9: return kmb::BoxRegion::intersectArea_maxx(p0,p3,p2);
	case 10: return kmb::BoxRegion::intersectArea_maxx(p1,p3,p0);
	case 11: return kmb::BoxRegion::intersectArea_maxx(p2,p3,p1);
	default: return 0.0;
	}
}

double
kmb::BoxRegion::intersectArea_maxx(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const
{
	kmb::Point3D p3, p4;
	switch( kmb::PlaneYZ::getIntersectionTriangle( maxX(), p0, p1, p2, p3, p4 ) )
	{
	case -1: return 0.0;
	case -2: return kmb::BoxRegion::intersectArea_miny(p0,p1,p2);
	case 3: return kmb::BoxRegion::intersectArea_miny(p0,p3,p4);
	case 4: return kmb::BoxRegion::intersectArea_miny(p1,p3,p4);
	case 5: return kmb::BoxRegion::intersectArea_miny(p2,p3,p4);
	case 0: return kmb::BoxRegion::intersectArea_miny(p1,p2,p4) + kmb::BoxRegion::intersectArea_miny(p1,p4,p3);
	case 1: return kmb::BoxRegion::intersectArea_miny(p2,p0,p4) + kmb::BoxRegion::intersectArea_miny(p2,p4,p3);
	case 2: return kmb::BoxRegion::intersectArea_miny(p0,p1,p4) + kmb::BoxRegion::intersectArea_miny(p0,p4,p3);
	case 9: return kmb::BoxRegion::intersectArea_miny(p0,p1,p3);
	case 10: return kmb::BoxRegion::intersectArea_miny(p1,p2,p3);
	case 11: return kmb::BoxRegion::intersectArea_miny(p2,p0,p3);
	case 6: return kmb::BoxRegion::intersectArea_miny(p0,p3,p2);
	case 7: return kmb::BoxRegion::intersectArea_miny(p1,p3,p0);
	case 8: return kmb::BoxRegion::intersectArea_miny(p2,p3,p1);
	default: return 0.0;
	}
}

double
kmb::BoxRegion::intersectArea_miny(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const
{
	kmb::Point3D p3, p4;
	switch( kmb::PlaneZX::getIntersectionTriangle( minY(), p0, p1, p2, p3, p4 ) )
	{
	case -2: return 0.0;
	case -1: return kmb::BoxRegion::intersectArea_maxy(p0,p1,p2);
	case 0: return kmb::BoxRegion::intersectArea_maxy(p0,p3,p4);
	case 1: return kmb::BoxRegion::intersectArea_maxy(p1,p3,p4);
	case 2: return kmb::BoxRegion::intersectArea_maxy(p2,p3,p4);
	case 3: return kmb::BoxRegion::intersectArea_maxy(p1,p2,p4) + kmb::BoxRegion::intersectArea_maxy(p1,p4,p3);
	case 4: return kmb::BoxRegion::intersectArea_maxy(p2,p0,p4) + kmb::BoxRegion::intersectArea_maxy(p2,p4,p3);
	case 5: return kmb::BoxRegion::intersectArea_maxy(p0,p1,p4) + kmb::BoxRegion::intersectArea_maxy(p0,p4,p3);
	case 6: return kmb::BoxRegion::intersectArea_maxy(p0,p1,p3);
	case 7: return kmb::BoxRegion::intersectArea_maxy(p1,p2,p3);
	case 8: return kmb::BoxRegion::intersectArea_maxy(p2,p0,p3);
	case 9: return kmb::BoxRegion::intersectArea_maxy(p0,p3,p2);
	case 10: return kmb::BoxRegion::intersectArea_maxy(p1,p3,p0);
	case 11: return kmb::BoxRegion::intersectArea_maxy(p2,p3,p1);
	default: return 0.0;
	}
}

double
kmb::BoxRegion::intersectArea_maxy(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const
{
	kmb::Point3D p3, p4;
	switch( kmb::PlaneZX::getIntersectionTriangle( maxY(), p0, p1, p2, p3, p4 ) )
	{
	case -1: return 0.0;
	case -2: return kmb::BoxRegion::intersectArea_minz(p0,p1,p2);
	case 3: return kmb::BoxRegion::intersectArea_minz(p0,p3,p4);
	case 4: return kmb::BoxRegion::intersectArea_minz(p1,p3,p4);
	case 5: return kmb::BoxRegion::intersectArea_minz(p2,p3,p4);
	case 0: return kmb::BoxRegion::intersectArea_minz(p1,p2,p4) + kmb::BoxRegion::intersectArea_minz(p1,p4,p3);
	case 1: return kmb::BoxRegion::intersectArea_minz(p2,p0,p4) + kmb::BoxRegion::intersectArea_minz(p2,p4,p3);
	case 2: return kmb::BoxRegion::intersectArea_minz(p0,p1,p4) + kmb::BoxRegion::intersectArea_minz(p0,p4,p3);
	case 9: return kmb::BoxRegion::intersectArea_minz(p0,p1,p3);
	case 10: return kmb::BoxRegion::intersectArea_minz(p1,p2,p3);
	case 11: return kmb::BoxRegion::intersectArea_minz(p2,p0,p3);
	case 6: return kmb::BoxRegion::intersectArea_minz(p0,p3,p2);
	case 7: return kmb::BoxRegion::intersectArea_minz(p1,p3,p0);
	case 8: return kmb::BoxRegion::intersectArea_minz(p2,p3,p1);
	default: return 0.0;
	}
}

double
kmb::BoxRegion::intersectArea_minz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const
{
	kmb::Point3D p3, p4;
	switch( kmb::PlaneXY::getIntersectionTriangle( minZ(), p0, p1, p2, p3, p4 ) )
	{
	case -2: return 0.0;
	case -1: return kmb::BoxRegion::intersectArea_maxz(p0,p1,p2);
	case 0: return kmb::BoxRegion::intersectArea_maxz(p0,p3,p4);
	case 1: return kmb::BoxRegion::intersectArea_maxz(p1,p3,p4);
	case 2: return kmb::BoxRegion::intersectArea_maxz(p2,p3,p4);
	case 3: return kmb::BoxRegion::intersectArea_maxz(p1,p2,p4) + kmb::BoxRegion::intersectArea_maxz(p1,p4,p3);
	case 4: return kmb::BoxRegion::intersectArea_maxz(p2,p0,p4) + kmb::BoxRegion::intersectArea_maxz(p2,p4,p3);
	case 5: return kmb::BoxRegion::intersectArea_maxz(p0,p1,p4) + kmb::BoxRegion::intersectArea_maxz(p0,p4,p3);
	case 6: return kmb::BoxRegion::intersectArea_maxz(p0,p1,p3);
	case 7: return kmb::BoxRegion::intersectArea_maxz(p1,p2,p3);
	case 8: return kmb::BoxRegion::intersectArea_maxz(p2,p0,p3);
	case 9: return kmb::BoxRegion::intersectArea_maxz(p0,p3,p2);
	case 10: return kmb::BoxRegion::intersectArea_maxz(p1,p3,p0);
	case 11: return kmb::BoxRegion::intersectArea_maxz(p2,p3,p1);
	default: return 0.0;
	}
}

double
kmb::BoxRegion::intersectArea_maxz(const kmb::Point3D &p0,const kmb::Point3D &p1,const kmb::Point3D &p2) const
{
	kmb::Point3D p3, p4;
	switch( kmb::PlaneXY::getIntersectionTriangle( maxZ(), p0, p1, p2, p3, p4 ) )
	{
	case -1: return 0.0;
	case -2: return kmb::Point3D::area(p0,p1,p2);
	case 3: return kmb::Point3D::area(p0,p3,p4);
	case 4: return kmb::Point3D::area(p1,p3,p4);
	case 5: return kmb::Point3D::area(p2,p3,p4);
	case 0: return kmb::Point3D::area(p1,p2,p4) + kmb::Point3D::area(p1,p4,p3);
	case 1: return kmb::Point3D::area(p2,p0,p4) + kmb::Point3D::area(p2,p4,p3);
	case 2: return kmb::Point3D::area(p0,p1,p4) + kmb::Point3D::area(p0,p4,p3);
	case 9: return kmb::Point3D::area(p0,p1,p3);
	case 10: return kmb::Point3D::area(p1,p2,p3);
	case 11: return kmb::Point3D::area(p2,p0,p3);
	case 6: return kmb::Point3D::area(p0,p3,p2);
	case 7: return kmb::Point3D::area(p1,p3,p0);
	case 8: return kmb::Point3D::area(p2,p3,p1);
	default: return 0.0;
	}
}

double
kmb::BoxRegion::distanceSq(const kmb::BoxRegion& box) const
{
	return this->maxPoint.distanceSq( box.maxPoint ) + this->minPoint.distanceSq( box.minPoint );
}

double
kmb::BoxRegion::intersectVolume(const BoxRegion& box) const
{
	double lowX = ( minX() < box.minX() ? box.minX() : minX() );
	double lowY = ( minY() < box.minY() ? box.minY() : minY() );
	double lowZ = ( minZ() < box.minZ() ? box.minZ() : minZ() );
	double highX = ( maxX() > box.maxX() ? box.maxX() : maxX() );
	double highY = ( maxY() > box.maxY() ? box.maxY() : maxY() );
	double highZ = ( maxZ() > box.maxZ() ? box.maxZ() : maxZ() );
	if( lowX < highX && lowY < highY && lowZ < highZ ){
		return (highX - lowX) * (highY - lowY) * (highZ - lowZ);
	}else{
		return 0.0;
	}
}

void
kmb::BoxRegion::expand(double x,double y,double z)
{
	kmb::Vector3D diff(x,y,z);
	this->maxPoint += diff;
	this->minPoint -= diff;
}



void
kmb::BoxRegion::expand(double ratio)
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
		double maxz = maxPoint.z();
		double minz = minPoint.z();
		maxPoint.z( 0.5*(1+ratio)*maxz + 0.5*(1-ratio)*minz );
		minPoint.z( 0.5*(1-ratio)*maxz + 0.5*(1+ratio)*minz );
	}
}
