/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Curve2D                                                 #
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

#include "Geometry/kmbGeometry2D.h"

namespace kmb{

class Curve2D
{
protected:
	// getNearest の処理における閾値
	double epsilon;
	int iterMax;
public:
	Curve2D(void);
	virtual ~Curve2D(void);
	// 定義域の外ならば false を返す
	virtual bool getPoint( double t, kmb::Point2D& point ) const = 0;
	virtual void getDomain( double &min_t, double &max_t ) const = 0;
	virtual bool isDomain( double t ) const = 0;
	virtual bool getDerivative( double t, kmb::Vector2D& tangent ) const;
	virtual bool getSecondDerivative( double t, kmb::Vector2D& tangent ) const;
	// point にもっとも近い 曲線上の点を求める
	// newton 法が収束しなかったときは false を返す
	virtual bool getNearest( const kmb::Point2D& point, double& t ) const;

	double getEpsilon(void) const;
	void setEpsilon(double e);
	int getIterMax(void) const;
	void setIterMax(int m);
};

}
