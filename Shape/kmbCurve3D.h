/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Curve3D                                                 #
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

#include "Geometry/kmb_Geometry3D.h"

namespace kmb{

class Curve3D
{
protected:

	static int iterMax;

	static double thres;
public:
	Curve3D(void);
	virtual ~Curve3D(void);

	virtual bool getPoint( double t, kmb::Point3D& point ) const = 0;
	virtual void getDomain( double &min_t, double &max_t ) const = 0;
	virtual bool isDomain( double t ) const = 0;
	virtual bool getDerivative( double t, kmb::Vector3D& tangent ) const;
	virtual bool getSecondDerivative( double t, kmb::Vector3D& tangent ) const;



	virtual bool getNearest( kmb::Point3D& point, double& t ) const;
	bool getNearestOnInnerGrid( kmb::Point3D& point, unsigned int tgrid, double &t ) const;
protected:
	virtual bool newtonMethod( double &t, kmb::Point3D& point, double relax=1.0 ) const;
};

}
