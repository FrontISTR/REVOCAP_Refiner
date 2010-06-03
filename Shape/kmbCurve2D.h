/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Curve2D                                                 #
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

#include "Geometry/kmb_Geometry2D.h"

namespace kmb{

class Curve2D
{
protected:

	static int iterMax;

	static double thres;
public:
	Curve2D(void);
	virtual ~Curve2D(void);

	virtual bool getPoint( double t, kmb::Point2D& point ) const = 0;
	virtual void getDomain( double &min_t, double &max_t ) const = 0;
	virtual bool isDomain( double t ) const = 0;
	virtual bool getDerivative( double t, kmb::Vector2D& tangent ) const;
	virtual bool getSecondDerivative( double t, kmb::Vector2D& tangent ) const;



	virtual bool getNearest( kmb::Point2D& point, double& t ) const;


	bool getNearestOnInnerGrid( kmb::Point2D& point, unsigned int tgrid, double &t ) const;
protected:

	virtual bool newtonMethod( double &t, kmb::Point2D& point, double relax=1.0 ) const;
};

}
