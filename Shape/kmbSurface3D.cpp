/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Surface3D                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "Shape/kmbSurface3D.h"
#include "Geometry/kmb_Calculator.h"

int kmb::Surface3D::iterMax = 1000;
double kmb::Surface3D::thres = 1.0e-8;

kmb::Surface3D::Surface3D(void)
: surfaceId(-1)
{
}

kmb::Surface3D::~Surface3D(void)
{
}

long
kmb::Surface3D::getSurfaceId(void) const
{
	return this->surfaceId;
}

void
kmb::Surface3D::setSurfaceId(long surf_id)
{
	this->surfaceId = surf_id;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

bool
kmb::Surface3D::getDerivative( derivativeType d, double u, double v, kmb::Vector3D& tangent ) const
{
	return false;
}

bool
kmb::Surface3D::getBoundingBox( kmb::BoundingBox &bbox ) const
{
	bbox = this->bbox;
	return true;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

double
kmb::Surface3D::getNearestOnGrid( const kmb::Point3D& point, unsigned int ugrid, unsigned int vgrid, double &u, double &v ) const
{
	REVOCAP_Debug_3("getNearestOnGrid init => %f %f\n",u,v);
	double min_u, max_u, min_v, max_v;
	kmb::Point3D pt;
	getDomain(min_u,max_u,min_v,max_v);
	kmb::Minimizer minimizer;
	for(unsigned int i = 0; i<=ugrid; ++i ){
		double u0 = min_u + i * (max_u - min_u) / ugrid;
		for(unsigned int j = 0; j<=vgrid; ++j ){
			double v0 = min_v + j * (max_v - min_v) / vgrid;
			getPoint(u0,v0,pt);
			if( minimizer.update( pt.distanceSq( point ) ) ){
				u = u0;
				v = v0;
			}
		}
	}
	REVOCAP_Debug_3("getNearestOnGrid update => %f %f\n",u,v);
	return minimizer.getMin();
}











bool
kmb::Surface3D::getNearest( const kmb::Point3D& point, double &u, double &v ) const
{
	REVOCAP_Debug_3("getNearest init point = ( %f, %f, %f), (u,v) = ( %f %f )\n",point.x(), point.y(), point.z(), u,v);
	double u0 = u;
	double v0 = v;
	double thresSq = kmb::Surface3D::thres * kmb::Surface3D::thres;
	double distSqOnGrid = getNearestOnGrid(point,10,10,u0,v0);
	if( distSqOnGrid <= thresSq ){
		u = u0;
		v = v0;
		return true;
	}
	REVOCAP_Debug_3("getNearest init point on grid (u0,v0) = ( %f, %f ) (u,v) = ( %f, %f )\n", u0, v0, u, v);


	if( !isDomain(u,v) ){
		u = u0;
		v = v0;
	}
	int count = kmb::Surface3D::iterMax;
	kmb::Point3D pt;
	bool res = true;
	while( count > 0 ){
		res = newtonMethod( u0, v0, point );
		REVOCAP_Debug_3("getNearest Newton res = %d, domain = %d\n", res, isDomain(u0,v0), u0, v0);
		REVOCAP_Debug_3("%d : (%f,%f) => (%f,%f)\n", count, u, v, u0, v0);
		if( res== false || !isDomain(u0,v0) ){

			u0 = 0.75 * u + 0.25 * u0;
			v0 = 0.75 * v + 0.25 * v0;







		}else{
			double diffSq = (u-u0)*(u-u0) + (v-v0)*(v-v0);
			if( diffSq < thresSq ){

				getPoint( u0, v0, pt );
				double distSq = point.distanceSq( pt );

				if( distSq <= distSqOnGrid + thresSq ){

					u = u0;
					v = v0;
					REVOCAP_Debug_3("getNearest %d output = ( %f, %f )\n",count,u,v);
					REVOCAP_Debug_3("point = ( %f, %f, %f )\n", point.x(), point.y(), point.z());
					REVOCAP_Debug_3("pt = ( %f, %f, %f ), distSq = %f\n", pt.x(), pt.y(), pt.z(), distSq);
					break;







				}
			}
			u = u0;
			v = v0;
		}
		--count;
	}
	return ( count > 0 );
}


bool
kmb::Surface3D::newtonMethod( double &u, double &v, const kmb::Point3D& point, double relax ) const
{
	kmb::Point3D pt;
	kmb::Vector3D uVec, vVec, uuVec, uvVec, vvVec;
	if( !getPoint(u,v,pt) ||
		!getDerivative( kmb::Surface3D::DER_U, u, v, uVec) ||
		!getDerivative( kmb::Surface3D::DER_V, u, v, vVec) ||
		!getDerivative( kmb::Surface3D::DER_UU, u, v, uuVec) ||
		!getDerivative( kmb::Surface3D::DER_UV, u, v, uvVec) ||
		!getDerivative( kmb::Surface3D::DER_VV, u, v, vvVec) )
	{
		return false;
	}
	kmb::Vector3D d(pt,point);
	kmb::Vector2D r0( d*uVec, d*vVec );
	kmb::Vector2D r1;
	kmb::Matrix2x2 mat(
		uVec*uVec+d*uuVec, uVec*vVec+d*uvVec,
		vVec*uVec+d*uvVec, vVec*vVec+d*vvVec
	);
	if( mat.solve( r0, r1 ) ){
		u -= relax * r1.x();
		v -= relax * r1.y();
		return true;
	}
	return false;
}
