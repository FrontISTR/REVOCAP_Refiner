/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : NurbsSurface3D                                          #
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

#include "Shape/kmbNurbsSurface3D.h"
#include <cmath>

kmb::NurbsSurface3D::NurbsSurface3D(void)
: uOrder(0)
, vOrder(0)
{
}

kmb::NurbsSurface3D::~NurbsSurface3D(void)
{
}

void
kmb::NurbsSurface3D::clear(void)
{
	ctrlPts.clear();
	uBspline.clear();
	vBspline.clear();
	bbox.initialize();
	this->uOrder = 0;
	this->vOrder = 0;
}

kmb::Surface3D::surfaceType
kmb::NurbsSurface3D::getSurfaceType(void) const
{
	return kmb::Surface3D::NURBS;
}

void
kmb::NurbsSurface3D::getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const
{
	uBspline.getDomain( min_u, max_u );
	vBspline.getDomain( min_v, max_v );
}

bool
kmb::NurbsSurface3D::isDomain( double u, double v ) const
{
	return uBspline.isDomain(u) && vBspline.isDomain(v);
}


bool
kmb::NurbsSurface3D::shiftInsideDomain( double &u, double &v, double du, double dv ) const
{
	double max_u, min_u, max_v, min_v;
	uBspline.getDomainOnFrame( u, du, min_u, max_u);
	vBspline.getDomainOnFrame( v, dv, min_v, max_v);
	REVOCAP_Debug_3("domain u => (%f, %f), v => (%f, %f)\n", min_u, max_u, min_v, max_v);
	double abs_du = fabs(du);
	double abs_dv = fabs(dv);
	if( abs_du < kmb::Surface3D::thres ){
		if( abs_dv < kmb::Surface3D::thres ){
			return false;
		}

		if( 1.0 < max_v ){
			v += dv;
		}else if( dv > 0.0 ){
			vBspline.getDomain( dv, v );
		}else{
			vBspline.getDomain( v, dv );
		}
		return true;
	}else if( abs_dv < kmb::Surface3D::thres ){

		if( 1.0 < max_u ){
			u += du;
		}else if( du > 0.0 ){
			uBspline.getDomain( du, u );
		}else{
			uBspline.getDomain( u, du );
		}
		return true;
	}else if( 1.0 < max_u && 1.0 < max_v ){

		u += du;
		v += dv;
		return true;
	}else if( 0.0 < max_u && max_u < max_v ){




		if( du > 0.0 ){
			uBspline.getDomain( du, u );
		}else{
			uBspline.getDomain( u, du );
		}
		v += max_u * dv;
		return true;
	}else if( 0.0 < max_v && max_v < max_u ){




		u += max_v * du;
		if( dv > 0.0 ){
			vBspline.getDomain( dv, v );
		}else{
			vBspline.getDomain( v, dv );
		}
		return true;
	}
	return false;
}

bool
kmb::NurbsSurface3D::setOrder(unsigned int uOrder,unsigned int vOrder)
{
	this->uOrder = uOrder;
	this->vOrder = vOrder;
	return valid();
}

bool
kmb::NurbsSurface3D::getOrder(unsigned int &uOrder,unsigned int &vOrder) const
{
	uOrder = this->uOrder;
	vOrder = this->vOrder;
	return valid();
}

bool
kmb::NurbsSurface3D::getKnotCount(unsigned int &uCount,unsigned int &vCount) const
{
	uCount = uBspline.getKnotCount();
	vCount = vBspline.getKnotCount();
	return valid();
}

void
kmb::NurbsSurface3D::getDegree(unsigned int &uDegree,unsigned int &vDegree) const
{
	uDegree = this->uOrder-1;
	vDegree = this->vOrder-1;
}

int
kmb::NurbsSurface3D::getCtrlPtCount(void) const
{
	return static_cast<int>( ctrlPts.size() );
}

void
kmb::NurbsSurface3D::appendUKnot(double k)
{
	uBspline.appendKnot(k);
}

void
kmb::NurbsSurface3D::appendVKnot(double k)
{
	vBspline.appendKnot(k);
}

void
kmb::NurbsSurface3D::appendCtrlPt(double x,double y,double z,double w)
{
	this->ctrlPts.push_back( kmb::Point4D(x,y,z,w) );
	this->bbox.update(x,y,z);
}

bool
kmb::NurbsSurface3D::getCtrlPt(int i,kmb::Point4D &pt) const
{
	pt.set( ctrlPts[i] );
	return true;
}

double
kmb::NurbsSurface3D::getUKnot(int i) const
{
	return uBspline.getKnot(i);
}

double
kmb::NurbsSurface3D::getVKnot(int i) const
{
	return vBspline.getKnot(i);
}

bool
kmb::NurbsSurface3D::valid(void) const
{
	return ( (uBspline.getKnotCount()-uOrder)*(vBspline.getKnotCount()-vOrder) == ctrlPts.size() );
}

bool
kmb::NurbsSurface3D::getPoint( double u, double v, kmb::Point3D& point ) const
{
	point.zero();
	if( !valid() ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;
	double weight = 0.0;
	for(int j=0;j<vNum;++j){
		double vj0 = vBspline.getValue(j,vOrder-1,v);
		for(int i=0;i<uNum;++i){
			double ui0 = uBspline.getValue(i,uOrder-1,u);
			point.addCoordinate(
				ui0*vj0*ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w(),
				ui0*vj0*ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w(),
				ui0*vj0*ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w() );
			weight += ui0*vj0*ctrlPts[i+j*uNum].w();
		}
	}
	if( weight != 0.0 ){
		point.scale( 1.0/weight );
	}
	return true;
}


bool
kmb::NurbsSurface3D::newtonMethod( double &u, double &v, const kmb::Point3D& point, double relax ) const
{
	kmb::Point3D pt;
	kmb::Vector3D uVec, vVec, uuVec, uvVec, vvVec;
	double weight = 0;
	if( !getPoint(u,v,pt) ||
		!getSubDerivative( kmb::Surface3D::DER_U, u, v, uVec) ||
		!getSubDerivative( kmb::Surface3D::DER_V, u, v, vVec) ||
		!getSubDerivative( kmb::Surface3D::DER_UU, u, v, uuVec) ||
		!getSubDerivative( kmb::Surface3D::DER_UV, u, v, uvVec) ||
		!getSubDerivative( kmb::Surface3D::DER_VV, u, v, vvVec) ||
		!getWeight( u, v, weight ) ||
		weight == 0.0 )
	{
		REVOCAP_Debug_1("newtonMethod getPoint or getDerivative error %f %f\n",u,v);
		return false;
	}
	REVOCAP_Debug_3("newtonMethod (u,v) = (%f, %f)\n",u,v);
	REVOCAP_Debug_3("newtonMethod point = (%f, %f, %f)\n", pt.x(), pt.y(), pt.z());
	REVOCAP_Debug_3("newtonMethod uVec  = (%f, %f, %f)\n", uVec.x(), uVec.y(), uVec.z());
	REVOCAP_Debug_3("newtonMethod vVec  = (%f, %f, %f)\n", vVec.x(), vVec.y(), vVec.z());
	REVOCAP_Debug_3("newtonMethod uuVec = (%f, %f, %f)\n", uuVec.x(), uuVec.y(), uuVec.z());
	REVOCAP_Debug_3("newtonMethod uvVec = (%f, %f, %f)\n", uvVec.x(), uvVec.y(), uvVec.z());
	REVOCAP_Debug_3("newtonMethod vvVec = (%f, %f, %f)\n", vvVec.x(), vvVec.y(), vvVec.z());
	REVOCAP_Debug_3("newtonMethod weight = %f\n", weight);
	double sqWeight = weight * weight;
	kmb::Vector3D d(pt,point);
	kmb::Vector2D r0( d*uVec, d*vVec );
	kmb::Vector2D r1;
	kmb::Matrix2x2 mat(
		uVec*uVec/sqWeight+d*uuVec, uVec*vVec/sqWeight+d*uvVec,
		vVec*uVec/sqWeight+d*uvVec, vVec*vVec/sqWeight+d*vvVec
	);
	REVOCAP_Debug_3("newtonMethod d = (%f, %f, %f)\n", d.x(), d.y(), d.z());
	REVOCAP_Debug_3("newtonMethod r0 = (%f, %f)\n", r0.x(), r0.y());
	REVOCAP_Debug_3("newtonMethod [ %f, %f]\n", mat.get(0,0), mat.get(0,1) );
	REVOCAP_Debug_3("newtonMethod [ %f, %f]\n", mat.get(1,0), mat.get(1,1) );
	if( mat.solve( r0, r1 ) ){
		REVOCAP_Debug_3("newtonMethod r1 = (%f, %f)\n", r1.x(), r1.y());
		shiftInsideDomain( u, v, -relax*r1.x(), -relax*r1.y() );
		REVOCAP_Debug_3("newtonMethod update (%f, %f)\n",u,v);
		return true;
	}
	REVOCAP_Debug_1("newtonMethod can't solve matrix%f %f\n",u,v);
	return false;
}

#undef _DEBUG_
#include "Geometry/kmb_Debug.h"

bool
kmb::NurbsSurface3D::getSubDerivative( kmb::Surface3D::derivativeType d, double u, double v, kmb::Vector3D& tangent ) const
{
	tangent.zero();
	if( !valid() ){
		return false;
	}
	kmb::Point3D pt;
	double weight = 0.0;
	if( !getPoint(u,v,pt) || !getWeight(u,v,weight) ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;
	switch( d )
	{
	case kmb::Surface3D::DER_U:
		for(int j=0;j<vNum;++j){
			double vj0 = vBspline.getValue(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui1 = uBspline.getDerivative(i,uOrder-1,u);
				tangent.addCoordinate(
					ui1*vj0*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.x()),
					ui1*vj0*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.y()),
					ui1*vj0*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		return true;
	case kmb::Surface3D::DER_V:
		for(int j=0;j<vNum;++j){
			double vj1 = vBspline.getDerivative(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui0 = uBspline.getValue(i,uOrder-1,u);
				tangent.addCoordinate(
					ui0*vj1*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.x()),
					ui0*vj1*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.y()),
					ui0*vj1*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		return true;
	case kmb::Surface3D::DER_UU:
		for(int j=0;j<vNum;++j){
			double vj0 = vBspline.getValue(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui2 = uBspline.getSecondDerivative(i,uOrder-1,u);
				tangent.addCoordinate(
					ui2*vj0*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.x()),
					ui2*vj0*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.y()),
					ui2*vj0*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		return true;
	case kmb::Surface3D::DER_UV:
		{
			kmb::Vector4D uder;
			kmb::Vector4D vder;
			for(int j=0;j<vNum;++j){
				double vj0 = vBspline.getValue(j,vOrder-1,v);
				double vj1 = vBspline.getDerivative(j,vOrder-1,v);
				for(int i=0;i<uNum;++i){
					double ui0 = uBspline.getValue(i,uOrder-1,u);
					double ui1 = uBspline.getDerivative(i,uOrder-1,u);
					tangent.addCoordinate(
						ui1*vj1*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.x()),
						ui1*vj1*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.y()),
						ui1*vj1*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.z()));
					uder.addCoordinate(
						ui1 * vj0 * ctrlPts[i+j*uNum].x(),
						ui1 * vj0 * ctrlPts[i+j*uNum].y(),
						ui1 * vj0 * ctrlPts[i+j*uNum].z(),
						ui1 * vj0 * ctrlPts[i+j*uNum].w());
					vder.addCoordinate(
						ui0 * vj1 * ctrlPts[i+j*uNum].x(),
						ui0 * vj1 * ctrlPts[i+j*uNum].y(),
						ui0 * vj1 * ctrlPts[i+j*uNum].z(),
						ui0 * vj1 * ctrlPts[i+j*uNum].w());
				}
			}
			tangent.addCoordinate(
				uder.x() * vder.w() - vder.x() * uder.w(),
				uder.y() * vder.w() - vder.y() * uder.w(),
				uder.z() * vder.w() - vder.z() * uder.w());
		}
		return true;
	case kmb::Surface3D::DER_VV:
		for(int j=0;j<vNum;++j){
			double vj2 = vBspline.getSecondDerivative(j,vOrder-1,v);
			for(int i=0;i<uNum;++i){
				double ui0 = uBspline.getValue(i,uOrder-1,u);
				tangent.addCoordinate(
					ui0*vj2*(ctrlPts[i+j*uNum].x()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.x()),
					ui0*vj2*(ctrlPts[i+j*uNum].y()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.y()),
					ui0*vj2*(ctrlPts[i+j*uNum].z()*ctrlPts[i+j*uNum].w()*weight - ctrlPts[i+j*uNum].w()*pt.z()));
			}
		}
		return true;
	default:
		break;
	}
	return false;
}

bool
kmb::NurbsSurface3D::getWeight( double u, double v, double &w ) const
{
	if( !valid() ){
		return false;
	}
	int uNum = uBspline.getKnotCount()-uOrder;
	int vNum = vBspline.getKnotCount()-vOrder;
	double weight = 0.0;
	for(int j=0;j<vNum;++j){
		double v0 = vBspline.getValue(j,vOrder-1,v);
		for(int i=0;i<uNum;++i){
			double u0 = uBspline.getValue(i,uOrder-1,u);
			weight += u0*v0*ctrlPts[i+j*uNum].w();
		}
	}
	w = weight;
	return true;
}

int
kmb::NurbsSurface3D::writeRNF( std::ofstream &output, std::string indent) const
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
	for(std::vector<kmb::Point4D>::const_iterator pIter = ctrlPts.begin();
		pIter != ctrlPts.end(); ++pIter )
	{
		output << indent << std::fixed
			<< "  - [ "
			<< pIter->x() << ", "
			<< pIter->y() << ", "
			<< pIter->z() << ", "
			<< pIter->w() << " ]" << std::endl;
	}
	return 0;
}
