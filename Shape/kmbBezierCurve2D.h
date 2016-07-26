/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BezierCurve2D                                           #
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
 * Bezier 曲線
 * n 個の制御点を与えると、n-1 次の曲線が得られる
 * order = n
 * degree = n-1
 *
 * 定義域は 0 <= t <= 1 とする
 */
#pragma once

#include "Shape/kmbCurve2D.h"
#include <vector>

namespace kmb{

class BezierCurve2D : public Curve2D
{
private:
	std::vector< kmb::Point2D > ctrlPts;
public:
	BezierCurve2D(void);
	virtual ~BezierCurve2D(void);
	void clear(void);
	virtual bool isDomain( double t ) const;
	// 定義域は [0.0, 1.0]
	virtual void getDomain( double &min_t, double &max_t ) const;
	virtual bool getPoint( double t, kmb::Point2D& point ) const;
	bool getDerivative( double t, kmb::Vector2D& tangent ) const;
	bool getSecondDerivative( double t, kmb::Vector2D& tangent ) const;
	int getOrder(void) const;
	int getDegree(void) const;
	void appendCtrlPt(double x,double y);
	bool getCtrlPt(int i,kmb::Point2D &pt) const;
};

}
