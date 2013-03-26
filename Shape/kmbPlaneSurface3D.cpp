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

#include "Shape/kmbPlaneSurface3D.h"
#include "Shape/kmbNurbsSurface3D.h"

kmb::PlaneSurface3D::PlaneSurface3D(void)
: kmb::Surface3D()
{
}

kmb::PlaneSurface3D::~PlaneSurface3D(void)
{
}

kmb::Surface3D::surfaceType
kmb::PlaneSurface3D::getSurfaceType(void) const
{
	return kmb::Surface3D::PLANE;
}

bool
kmb::PlaneSurface3D::isDomain( double u, double v ) const
{
	return 0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0;
}

bool
kmb::PlaneSurface3D::isUDomain( double u ) const
{
	return 0.0 <= u && u <= 1.0;
}

bool
kmb::PlaneSurface3D::isVDomain( double v ) const
{
	return 0.0 <= v && v <= 1.0;
}

void
kmb::PlaneSurface3D::convertToNurbs3D( kmb::NurbsSurface3D& nurbs ) const
{
	nurbs.clear();
	nurbs.setOrder(2,2);
	nurbs.appendUKnot(0.0);
	nurbs.appendUKnot(0.0);
	nurbs.appendUKnot(1.0);
	nurbs.appendUKnot(1.0);
	nurbs.appendVKnot(0.0);
	nurbs.appendVKnot(0.0);
	nurbs.appendVKnot(1.0);
	nurbs.appendVKnot(1.0);
	nurbs.appendCtrlPt(
		basePoint.x(),
		basePoint.y(),
		basePoint.z() );
	nurbs.appendCtrlPt(
		basePoint.x() + uVector.x(),
		basePoint.y() + uVector.y(),
		basePoint.z() + uVector.z() );
	nurbs.appendCtrlPt(
		basePoint.x() + vVector.x(),
		basePoint.y() + vVector.y(),
		basePoint.z() + vVector.z() );
	nurbs.appendCtrlPt(
		basePoint.x() + vVector.x() + uVector.x(),
		basePoint.y() + vVector.y() + uVector.y(),
		basePoint.z() + vVector.z() + uVector.z() );
}

int
kmb::PlaneSurface3D::writeRNF( std::ofstream &output, std::string indent) const
{
	kmb::NurbsSurface3D nurbs;
	convertToNurbs3D(nurbs);
	nurbs.writeRNF( output, indent );
	return 0;
}

bool
kmb::PlaneSurface3D::getPoint( double u, double v, kmb::Point3D &point ) const
{
	point = basePoint + u*uVector + v*vVector;
	return true;
}

void
kmb::PlaneSurface3D::getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const
{
	min_u = 0.0;
	max_u = 1.0;
	min_v = 0.0;
	max_v = 1.0;
}

void
kmb::PlaneSurface3D::setBasePoint( const kmb::Point3D& pt )
{
	this->basePoint = pt;
}

void
kmb::PlaneSurface3D::setBasePoint( const double x, const double y, const double z )
{
	this->basePoint.setCoordinate(x,y,z);
}

void
kmb::PlaneSurface3D::setAxisVectors( const kmb::Vector3D& uVec, const kmb::Vector3D& vVec )
{
	this->uVector = uVec;
	this->vVector = vVec;
}

void
kmb::PlaneSurface3D::setAxisVectors( const double uX, const double uY, const double uZ, const double vX, const double vY, const double vZ )
{
	this->uVector.setCoordinate(uX,uY,uZ);
	this->vVector.setCoordinate(vX,vY,vZ);
}
