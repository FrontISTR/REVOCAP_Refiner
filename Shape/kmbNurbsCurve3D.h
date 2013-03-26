/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NurbsCurve3D                                            #
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

#include "Shape/kmbCurve3D.h"
#include "Shape/kmbBSpline.h"
#include <vector>

namespace kmb{

class Point4D;

class NurbsCurve3D : public Curve3D
{
private:
	std::vector< kmb::Point4D > ctrlPts;
	kmb::BSpline bspline;
public:
	NurbsCurve3D(void);
	virtual ~NurbsCurve3D(void);
	void clear(void);
	int getKnotCount(void) const;
	int getCtrlPtCount(void) const;
	int getOrder(void) const;
	int getDegree(void) const;
	void appendKnot(double k);
	void appendCtrlPt(double x,double y,double z,double w=1.0);
	bool getCtrlPt(int i,kmb::Point4D &pt) const;
	double getKnot(int i) const;
	bool isDomain( double t ) const;
	virtual bool getPoint( double t, kmb::Point3D& point ) const;
	virtual void getDomain( double &min_t, double &max_t ) const;
	virtual bool getNearest( const kmb::Point3D& point, double& t ) const;
protected:

	virtual bool getSubDerivative( double t, kmb::Vector3D& tangent ) const;
	virtual bool getSubSecondDerivative( double t, kmb::Vector3D& tangent ) const;
	virtual bool getWeight( double t, double &w ) const;
};

}
