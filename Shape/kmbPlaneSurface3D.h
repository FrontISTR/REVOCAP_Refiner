/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PlaneSurface3D                                          #
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
/**
 * •½–Ê
 * ’è‹`ˆæ‚Í 0 <= u,v <= 1 ‚Æ‚·‚é
 */
#pragma once
#include "Shape/kmbSurface3D.h"
#include "Geometry/kmbGeometry3D.h"

namespace kmb{

class NurbsSurface3D;

class PlaneSurface3D : public Surface3D
{
private:
	kmb::Point3D basePoint;
	kmb::Vector3D uVector;
	kmb::Vector3D vVector;
public:
	PlaneSurface3D(void);
	virtual ~PlaneSurface3D(void);
	virtual surfaceType getSurfaceType(void) const;
	virtual int writeRNF( std::ofstream &output, std::string indent="  ") const;
	virtual bool getPoint( double u, double v, kmb::Point3D &point ) const;
	virtual void getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const;
	virtual bool isDomain( double u, double v ) const;
	virtual bool isUDomain( double u ) const;
	virtual bool isVDomain( double v ) const;

	void setBasePoint( const kmb::Point3D& pt );
	void setBasePoint( const double x, const double y, const double z );
	void setAxisVectors( const kmb::Vector3D& uVec, const kmb::Vector3D& vVec );
	void setAxisVectors( const double uX, const double uY, const double uZ, const double vX, const double vY, const double vZ );

	void convertToNurbs3D( kmb::NurbsSurface3D& nurbs ) const;
};

}
