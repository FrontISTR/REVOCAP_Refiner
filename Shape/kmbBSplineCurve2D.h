/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BSplineCurve2D                                          #
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
#include <vector>

namespace kmb{

class BSplineCurve2D : public Curve2D
{
private:
	std::vector< kmb::Point2D > ctrlPts;
	kmb::BSpline bspline;
public:
	BSplineCurve2D(void);
	virtual ~BSplineCurve2D(void);
	void clear(void);
	int getKnotCount(void) const;
	int getCtrlPtCount(void) const;
	int getOrder(void) const;
	int getDegree(void) const;
	void appendKnot(double k);
	void appendCtrlPt(double x,double y);
	bool getCtrlPt(int i,kmb::Point2D &pt) const;
	double getKnot(int i) const;
	bool isDomain( double t ) const;
	virtual bool getPoint( double t, kmb::Point2D& point ) const;
	virtual void getDomain( double &min_t, double &max_t ) const;
	bool getDerivative( double t, kmb::Vector2D& tangent ) const;
	bool getSecondDerivative( double t, kmb::Vector2D& tangent ) const;
};

}
