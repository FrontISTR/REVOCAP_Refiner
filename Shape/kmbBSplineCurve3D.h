/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BSplineCurve3D                                          #
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

class BSplineCurve3D : public Curve3D
{
private:
	std::vector< kmb::Point3D > ctrlPts;
	kmb::BSpline bspline;
public:
	BSplineCurve3D(void);
	virtual ~BSplineCurve3D(void);
	void clear(void);
	int getKnotCount(void) const;
	int getCtrlPtCount(void) const;
	int getOrder(void) const;
	int getDegree(void) const;
	void appendKnot(double k);
	void appendCtrlPt(double x,double y,double z);
	bool getCtrlPt(int i,kmb::Point3D &pt) const;
	double getKnot(int i) const;
	virtual bool isDomain( double t ) const;
	virtual void getDomain( double &min_t, double &max_t ) const;
	virtual bool getPoint( double t, kmb::Point3D& point ) const;
	bool getDerivative( double t, kmb::Vector3D& tangent ) const;
	bool getSecondDerivative( double t, kmb::Vector3D& tangent ) const;
};

}
