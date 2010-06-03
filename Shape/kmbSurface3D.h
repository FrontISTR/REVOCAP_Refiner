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
#pragma once

#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_Geometry2D.h"
#include "Geometry/kmb_BoundingBox.h"
#include <string>
#include <fstream>

namespace kmb{

class Surface3D
{
protected:

	static int iterMax;

	static double thres;
	kmb::BoundingBox bbox;
public:
	enum surfaceType{
		BEZIER,
		BSPLINE,
		NURBS,
	};
	enum derivativeType{
		DER_U,
		DER_V,
		DER_UU,
		DER_UV,
		DER_VV,
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
	virtual bool getDerivative( derivativeType d, double u, double v, kmb::Vector3D& tangent ) const;
	virtual bool getBoundingBox( kmb::BoundingBox &bbox ) const;



	virtual bool getNearest( const kmb::Point3D& point, double &u, double &v ) const;



	double getNearestOnGrid( const kmb::Point3D& point, unsigned int ugrid, unsigned int vgrid, double &u, double &v ) const;
protected:
	long surfaceId;


	virtual bool newtonMethod( double &u, double &v, const kmb::Point3D& point, double relax=1.0 ) const;
};

}
