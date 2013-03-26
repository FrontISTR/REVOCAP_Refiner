/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NurbsCurve2D                                            #
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

#include "Shape/kmbCurve2D.h"
#include "Shape/kmbBSpline.h"
#include "Geometry/kmbGeometry3D.h"
#include <vector>

namespace kmb{

class NurbsCurve2D : public Curve2D
{
private:
	std::vector< kmb::Point3D > ctrlPts;
	kmb::BSpline bspline;
public:
	NurbsCurve2D(void);
	virtual ~NurbsCurve2D(void);
	void clear(void);
	int getKnotCount(void) const;
	int getCtrlPtCount(void) const;
	int getOrder(void) const;
	int getDegree(void) const;
	void appendKnot(double k);
	void appendCtrlPt(double x,double y,double w=1.0);
	bool getCtrlPt(int i,kmb::Point3D &pt) const;
	double getKnot(int i) const;
	bool isDomain( double t ) const;
	virtual bool getPoint( double t, kmb::Point2D& point ) const;
	virtual void getDomain( double &min_t, double &max_t ) const;
	virtual bool getNearest( const kmb::Point2D& point, double& t ) const;
protected:

	virtual bool getSubDerivative( double t, kmb::Vector2D& tangent ) const;
	virtual bool getSubSecondDerivative( double t, kmb::Vector2D& tangent ) const;
	virtual bool getWeight( double t, double &w ) const;
};

}
