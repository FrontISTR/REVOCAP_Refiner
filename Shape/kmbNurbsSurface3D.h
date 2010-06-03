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
#pragma once

#include "Shape/kmbSurface3D.h"
#include "Shape/kmbBSpline.h"
#include "Geometry/kmb_Geometry4D.h"
#include <vector>

namespace kmb{

class NurbsSurface3D : public Surface3D
{
private:

	unsigned int uOrder, vOrder;
	kmb::BSpline uBspline, vBspline;
	std::vector< kmb::Point4D > ctrlPts;
public:
	NurbsSurface3D(void);
	virtual ~NurbsSurface3D(void);
	void clear(void);
	virtual kmb::Surface3D::surfaceType getSurfaceType(void) const;
	virtual void getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const;
	bool isDomain( double u, double v ) const;


	bool setOrder(unsigned int uOrder,unsigned int vOrder);
	bool getOrder(unsigned int &uOrder,unsigned int &vOrder) const;
	bool getKnotCount(unsigned int &uCount,unsigned int &vCount) const;
	void getDegree(unsigned int &uDegree,unsigned int &vDegree) const;
	int getCtrlPtCount(void) const;
	void appendUKnot(double k);
	void appendVKnot(double k);
	void appendCtrlPt(double x,double y,double z,double w=1.0);
	bool getCtrlPt(int i,kmb::Point4D &pt) const;
	double getUKnot(int i) const;
	double getVKnot(int i) const;
	bool valid(void) const;
	virtual bool getPoint( double u, double v, kmb::Point3D& point ) const;
	virtual int writeRNF( std::ofstream &output, std::string indent="  " ) const;
protected:

	virtual bool newtonMethod( double &u, double &v, const kmb::Point3D& point, double relax=1.0 ) const;

	virtual bool getSubDerivative( derivativeType d, double u, double v, kmb::Vector3D& tangent ) const;
	virtual bool getWeight( double u, double v, double &w ) const;

	bool shiftInsideDomain( double &u, double &v, double du, double dv ) const;
};

}
