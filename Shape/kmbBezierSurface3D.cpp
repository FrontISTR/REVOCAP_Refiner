/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BezierSurfae3D                                          #
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

#include "Shape/kmbBezierSurface3D.h"
#include "Shape/kmbBernstein.h"

kmb::BezierSurface3D::BezierSurface3D(void)
: kmb::Surface3D()
, uOrder(0)
, vOrder(0)
{
}

kmb::BezierSurface3D::~BezierSurface3D(void)
{
}

void
kmb::BezierSurface3D::clear(void)
{
	ctrlPts.clear();
	this->uOrder = 0;
	this->vOrder = 0;
}

kmb::Surface3D::surfaceType
kmb::BezierSurface3D::getSurfaceType(void) const
{
	return kmb::Surface3D::BEZIER;
}

bool
kmb::BezierSurface3D::isDomain( double u, double v) const
{
	return 0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0;
}

bool
kmb::BezierSurface3D::isUDomain( double u ) const
{
	return 0.0 <= u && u <= 1.0;
}

bool
kmb::BezierSurface3D::isVDomain( double v ) const
{
	return 0.0 <= v && v <= 1.0;
}

void
kmb::BezierSurface3D::getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const
{
	min_u = 0.0;
	max_u = 1.0;
	min_v = 0.0;
	max_v = 1.0;
}

bool
kmb::BezierSurface3D::setOrder(unsigned int uOrder,unsigned int vOrder)
{
	this->uOrder = uOrder;
	this->vOrder = vOrder;
	return ( uOrder*vOrder == ctrlPts.size() );
}

bool
kmb::BezierSurface3D::getOrder(unsigned int &uOrder,unsigned int &vOrder) const
{
	uOrder = this->uOrder;
	vOrder = this->vOrder;
	return ( uOrder*vOrder == ctrlPts.size() );
}

void
kmb::BezierSurface3D::getDegree(unsigned int &uDegree,unsigned int &vDegree) const
{
	uDegree = this->uOrder-1;
	vDegree = this->vOrder-1;
}

void
kmb::BezierSurface3D::appendCtrlPt(double x,double y,double z)
{
	this->ctrlPts.push_back( kmb::Point3D(x,y,z) );
	this->bbox.update(x,y,z);
}

bool
kmb::BezierSurface3D::getCtrlPt(int i,kmb::Point3D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

bool
kmb::BezierSurface3D::getPoint( double u, double v, kmb::Point3D& point ) const
{
	point.zero();
	if( uOrder*vOrder != ctrlPts.size() ){
		return false;
	}

	for(unsigned int j=0;j<vOrder;++j){
		double v0 = kmb::Bernstein::getValue(vOrder-1,j,v);
		for(unsigned int i=0;i<uOrder;++i){
			double u0 = kmb::Bernstein::getValue(uOrder-1,i,u);
			point.addCoordinate(
				u0*v0*ctrlPts[i+j*uOrder].x(),
				u0*v0*ctrlPts[i+j*uOrder].y(),
				u0*v0*ctrlPts[i+j*uOrder].z() );
		}
	}
	return true;
}

bool
kmb::BezierSurface3D::getDerivative( kmb::Surface3D::derivativeType d, double u, double v, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	if( uOrder*vOrder != ctrlPts.size() ){
		return false;
	}
	switch( d )
	{
	case kmb::Surface3D::DER_U:
		for(unsigned int j=0;j<vOrder;++j){
			double v0 = kmb::Bernstein::getValue(vOrder-1,j,v);
			for(unsigned int i=0;i<uOrder-1;++i){
				double u0 = kmb::Bernstein::getValue(uOrder-2,i,u) * (uOrder-1);
				tangent.addCoordinate(
					u0*v0*(ctrlPts[(i+1)+j*uOrder].x()-ctrlPts[i+j*uOrder].x()),
					u0*v0*(ctrlPts[(i+1)+j*uOrder].y()-ctrlPts[i+j*uOrder].y()),
					u0*v0*(ctrlPts[(i+1)+j*uOrder].z()-ctrlPts[i+j*uOrder].z()) );
			}
		}
		return true;
	case kmb::Surface3D::DER_V:
		for(unsigned int j=0;j<vOrder-1;++j){
			double v0 = kmb::Bernstein::getValue(vOrder-2,j,v) * (vOrder-1);
			for(unsigned int i=0;i<uOrder;++i){
				double u0 = kmb::Bernstein::getValue(uOrder-1,i,u);
				tangent.addCoordinate(
					u0*v0*(ctrlPts[i+(j+1)*uOrder].x()-ctrlPts[i+j*uOrder].x()),
					u0*v0*(ctrlPts[i+(j+1)*uOrder].y()-ctrlPts[i+j*uOrder].y()),
					u0*v0*(ctrlPts[i+(j+1)*uOrder].z()-ctrlPts[i+j*uOrder].z()) );
			}
		}
		return true;
	case kmb::Surface3D::DER_UU:
		for(unsigned int j=0;j<vOrder;++j){
			double v0 = kmb::Bernstein::getValue(vOrder-1,j,v);
			for(unsigned int i=0;i<uOrder-2;++i){
				double u0 = kmb::Bernstein::getValue(uOrder-3,i,u) * (uOrder-1) * (uOrder-2);
				tangent.addCoordinate(
					u0*v0*(ctrlPts[i+2+j*uOrder].x()-2.0*ctrlPts[i+1+j*uOrder].x()+ctrlPts[i+j*uOrder].x()),
					u0*v0*(ctrlPts[i+2+j*uOrder].y()-2.0*ctrlPts[i+1+j*uOrder].y()+ctrlPts[i+j*uOrder].y()),
					u0*v0*(ctrlPts[i+2+j*uOrder].z()-2.0*ctrlPts[i+1+j*uOrder].z()+ctrlPts[i+j*uOrder].z()) );
			}
		}
		return true;
	case kmb::Surface3D::DER_UV:
		for(unsigned int j=0;j<vOrder-1;++j){
			double v0 = kmb::Bernstein::getValue(vOrder-2,j,v) * (vOrder-1);
			for(unsigned int i=0;i<uOrder-1;++i){
				double u0 = kmb::Bernstein::getValue(uOrder-2,i,u) * (uOrder-1);
				tangent.addCoordinate(
					u0*v0*(ctrlPts[i+1+(j+1)*uOrder].x()-ctrlPts[i+1+j*uOrder].x()-ctrlPts[i+(j+1)*uOrder].x()+ctrlPts[i+j*uOrder].x()),
					u0*v0*(ctrlPts[i+1+(j+1)*uOrder].x()-ctrlPts[i+1+j*uOrder].y()-ctrlPts[i+(j+1)*uOrder].y()+ctrlPts[i+j*uOrder].y()),
					u0*v0*(ctrlPts[i+1+(j+1)*uOrder].x()-ctrlPts[i+1+j*uOrder].z()-ctrlPts[i+(j+1)*uOrder].z()+ctrlPts[i+j*uOrder].z()) );
			}
		}
		return true;
	case kmb::Surface3D::DER_VV:
		for(unsigned int j=0;j<vOrder-2;++j){
			double v0 = kmb::Bernstein::getValue(vOrder-3,j,v) * (vOrder-1) * (vOrder-2);
			for(unsigned int i=0;i<uOrder;++i){
				double u0 = kmb::Bernstein::getValue(uOrder-1,i,u);
				tangent.addCoordinate(
					u0*v0*(ctrlPts[i+(j+2)*uOrder].x()-2.0*ctrlPts[i+(j+1)*uOrder].x()+ctrlPts[i+j*uOrder].x()),
					u0*v0*(ctrlPts[i+(j+2)*uOrder].y()-2.0*ctrlPts[i+(j+1)*uOrder].y()+ctrlPts[i+j*uOrder].y()),
					u0*v0*(ctrlPts[i+(j+2)*uOrder].z()-2.0*ctrlPts[i+(j+1)*uOrder].z()+ctrlPts[i+j*uOrder].z()) );
			}
		}
		return true;
	default:
		return false;
	}
}

int
kmb::BezierSurface3D::writeRNF( std::ofstream &output, std::string indent ) const
{
	output << indent << "uorder:" << uOrder << std::endl;
	output << indent << "vorder:" << vOrder << std::endl;
	output << indent << "ctrlpts:" << std::endl;
	for(std::vector<kmb::Point3D>::const_iterator pIter = ctrlPts.begin();
		pIter != ctrlPts.end(); ++pIter )
	{
		output << indent << std::fixed
			<< "  - [ "
			<< pIter->x() << ", "
			<< pIter->y() << ", "
			<< pIter->z() << " ]" << std::endl;
	}
	return 0;
}
