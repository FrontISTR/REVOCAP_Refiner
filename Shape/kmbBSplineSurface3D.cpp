/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BSplineSurface3D                                        #
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

#include "Shape/kmbBSplineSurface3D.h"

kmb::BSplineSurface3D::BSplineSurface3D(void)
: kmb::Surface3D()
, uOrder(0)
, vOrder(0)
{
}

kmb::BSplineSurface3D::~BSplineSurface3D(void)
{
}

void
kmb::BSplineSurface3D::clear(void)
{
	ctrlPts.clear();
	uBspline.clear();
	vBspline.clear();
	this->uOrder = 0;
	this->vOrder = 0;
}

kmb::Surface3D::surfaceType
kmb::BSplineSurface3D::getSurfaceType(void) const
{
	return kmb::Surface3D::BSPLINE;
}

void
kmb::BSplineSurface3D::getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const
{
	uBspline.getDomain( min_u, max_u );
	vBspline.getDomain( min_v, max_v );
}

bool
kmb::BSplineSurface3D::isDomain( double u, double v ) const
{
	return uBspline.isDomain(u) && vBspline.isDomain(v);
}

bool
kmb::BSplineSurface3D::isUDomain( double u ) const
{
	return uBspline.isDomain(u);
}

bool
kmb::BSplineSurface3D::isVDomain( double v ) const
{
	return vBspline.isDomain(v);
}

bool
kmb::BSplineSurface3D::setOrder(unsigned int uOrder,unsigned int vOrder)
{
	this->uOrder = uOrder;
	this->vOrder = vOrder;
	return valid();
}

bool
kmb::BSplineSurface3D::getOrder(unsigned int &uOrder,unsigned int &vOrder) const
{
	uOrder = this->uOrder;
	vOrder = this->vOrder;
	return valid();
}

bool
kmb::BSplineSurface3D::getKnotCount(unsigned int &uCount,unsigned int &vCount) const
{
	uCount = uBspline.getKnotCount();
	vCount = vBspline.getKnotCount();
	return valid();
}

void
kmb::BSplineSurface3D::getDegree(unsigned int &uDegree,unsigned int &vDegree) const
{
	uDegree = this->uOrder-1;
	vDegree = this->vOrder-1;
}

int
kmb::BSplineSurface3D::getCtrlPtCount(void) const
{
	return static_cast<int>( ctrlPts.size() );
}

void
kmb::BSplineSurface3D::appendUKnot(double k)
{
	uBspline.appendKnot(k);
}

void
kmb::BSplineSurface3D::appendVKnot(double k)
{
	vBspline.appendKnot(k);
}

void
kmb::BSplineSurface3D::appendCtrlPt(double x,double y,double z)
{
	this->ctrlPts.push_back( kmb::Point3D(x,y,z) );
	this->bbox.update(x,y,z);
}

bool
kmb::BSplineSurface3D::getCtrlPt(int i,kmb::Point3D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

double
kmb::BSplineSurface3D::getUKnot(int i) const
{
	return uBspline.getKnot(i);
}

double
kmb::BSplineSurface3D::getVKnot(int i) const
{
	return vBspline.getKnot(i);
}

bool
kmb::BSplineSurface3D::valid(void) const
{
	return ( (uBspline.getKnotCount()-uOrder)*(vBspline.getKnotCount()-vOrder) == ctrlPts.size() );
}

bool
kmb::BSplineSurface3D::getPoint( double u, double v, kmb::Point3D& point ) const
{
	point.zero();
	if( !valid() ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;
	for(int j=0;j<vNum;++j){
		double v0 = vBspline.getValue(j,vOrder-1,v);
		for(int i=0;i<uNum;++i){
			double u0 = uBspline.getValue(i,uOrder-1,u);
			point.addCoordinate(
				u0*v0*ctrlPts[i+j*uNum].x(),
				u0*v0*ctrlPts[i+j*uNum].y(),
				u0*v0*ctrlPts[i+j*uNum].z() );
		}
	}
	return true;
}

bool
kmb::BSplineSurface3D::getDerivative( kmb::Surface3D::derivativeType d, double u, double v, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	unsigned int uDeg=0, vDeg=0;
	getDegree( uDeg, vDeg );
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;
	switch( d )
	{
	case kmb::Surface3D::DER_U:
		if( uDeg == 0 ){
			return true;
		}
		for(int j=0;j<vNum;++j){
			double v0 = vBspline.getValue(j,vDeg,v);
			for(int i=0;i<uNum;++i){
				double u0 = uBspline.getDerivative(i,uDeg,u);
				tangent.addCoordinate(
					u0*v0*ctrlPts[i+j*uNum].x(),
					u0*v0*ctrlPts[i+j*uNum].y(),
					u0*v0*ctrlPts[i+j*uNum].z() );
			}
		}
		return true;
	case kmb::Surface3D::DER_V:
		if( vDeg == 0 ){
			return true;
		}
		for(int j=0;j<vNum;++j){
			double v0 = vBspline.getDerivative(j,vDeg,v);
			for(int i=0;i<uNum;++i){
				double u0 = uBspline.getValue(i,uDeg,u);
				tangent.addCoordinate(
					u0*v0*ctrlPts[i+j*uNum].x(),
					u0*v0*ctrlPts[i+j*uNum].y(),
					u0*v0*ctrlPts[i+j*uNum].z() );
			}
		}
		return true;
	case kmb::Surface3D::DER_UU:
		if( uDeg < 2 ){
			return true;
		}
		for(int j=0;j<vNum;++j){
			double v0 = vBspline.getValue(j,vDeg,v);
			for(int i=0;i<uNum;++i){
				double u0 = uBspline.getSecondDerivative(i,uDeg,u);
				tangent.addCoordinate(
					u0*v0*ctrlPts[i+j*uNum].x(),
					u0*v0*ctrlPts[i+j*uNum].y(),
					u0*v0*ctrlPts[i+j*uNum].z() );
			}
		}
		return true;
	case kmb::Surface3D::DER_UV:
		if( uDeg == 0 || vDeg == 0 ){
			return true;
		}
		for(int j=0;j<vNum;++j){
			double v0 = vBspline.getDerivative(j,vDeg,v);
			for(int i=0;i<uNum;++i){
				double u0 = uBspline.getDerivative(i,uDeg,u);
				tangent.addCoordinate(
					u0*v0*ctrlPts[i+j*uNum].x(),
					u0*v0*ctrlPts[i+j*uNum].y(),
					u0*v0*ctrlPts[i+j*uNum].z() );
			}
		}
		return true;
	case kmb::Surface3D::DER_VV:
		if( vDeg < 2 ){
			return true;
		}
		for(int j=0;j<vNum;++j){
			double v0 = vBspline.getSecondDerivative(j,vDeg,v);
			for(int i=0;i<uNum;++i){
				double u0 = uBspline.getValue(i,uDeg,u);
				tangent.addCoordinate(
					u0*v0*ctrlPts[i+j*uNum].x(),
					u0*v0*ctrlPts[i+j*uNum].y(),
					u0*v0*ctrlPts[i+j*uNum].z() );
			}
		}
		return true;
	default:
		return false;
	}
}

int
kmb::BSplineSurface3D::writeRNF( std::ofstream &output, std::string indent ) const
{
	output << indent << "uknots:" << std::endl;
	output << indent << "  order: " << uOrder << std::endl;
	output << indent << "  vector: [ ";
	int uKnotCount = uBspline.getKnotCount();
	for(int i=0;i<uKnotCount;++i){
		if( i != 0 ){
			output << ", ";
		}
		output << std::fixed << uBspline.getKnot(i);
	}
	output << " ]" << std::endl;
	output << indent << "vknots:" << std::endl;
	output << indent << "  order: " << vOrder << std::endl;
	output << indent << "  vector: [ ";
	int vKnotCount = vBspline.getKnotCount();
	for(int i=0;i<vKnotCount;++i){
		if( i != 0 ){
			output << ", ";
		}
		output << std::fixed << vBspline.getKnot(i);
	}
	output << " ]" << std::endl;
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
