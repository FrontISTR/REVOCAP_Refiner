/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Surface3D                                               #
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
#pragma once

#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbBoundingBox.h"
#include <string>
#include <fstream>

namespace kmb{

class BoxRegion2D;

class Surface3D
{
protected:
	double epsilon;
	int iterMax;
	kmb::BoundingBox bbox;
public:
	enum surfaceType{
		PLANE,
		BEZIER,
		BSPLINE,
		NURBS
	};
	enum derivativeType{
		DER_U,
		DER_V,
		DER_UU,
		DER_UV,
		DER_VV
	};
	Surface3D(void);
	virtual ~Surface3D(void);
	long getSurfaceId(void) const;
	void setSurfaceId(long id);
	virtual surfaceType getSurfaceType(void) const = 0;
	virtual int writeRNF( std::ofstream &output, std::string indent="  ") const = 0;

	virtual bool getPoint( double u, double v, kmb::Point3D &point ) const = 0;
	virtual void getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const = 0;
	virtual bool isDomain( double u, double v ) const = 0;
	virtual bool isUDomain( double u ) const = 0;
	virtual bool isVDomain( double v ) const = 0;
	virtual bool getDerivative( derivativeType d, double u, double v, kmb::Vector3D& tangent ) const;
	virtual bool getBoundingBox( kmb::BoundingBox &bbox ) const;

	virtual bool getMiddlePoint( double u0, double v0, double u1, double v1, double &u, double &v, kmb::Point3D &point ) const;
	virtual bool getMiddlePointByNearest( double u0, double v0, double u1, double v1, double &u, double &v, kmb::Point3D &point ) const;



	virtual bool getNearest( const kmb::Point3D& point, double &u, double &v ) const;

	double getEpsilon(void) const;
	void setEpsilon(double e);
	int getIterMax(void) const;
	void setIterMax(int m);
protected:
	long surfaceId;
};

}
