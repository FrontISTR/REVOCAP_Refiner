/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Region                                                  #
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
#include "Geometry/kmbRegion.h"

/*
 * 形状関数の定義域などに現れる標準的な単体
 * (x,y,z)  0 < x, 0 < y, 0 < z, x+y+z < d
 *
 */

namespace kmb{

class SimplexRegion : public Region
{
private:
	double dist;
public:
	SimplexRegion(double d=1.0);
	virtual ~SimplexRegion(void);

	virtual locationType intersect(const Point3D &point) const;
	virtual bool intersect(const BoxRegion& box) const;

	virtual double distanceSq(const Point3D &point) const;
};

}
