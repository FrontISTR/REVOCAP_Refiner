/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Integration                                             #
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

namespace kmb{

class Integration
{
public:
	static double Gauss_Point2[2];
	static double Gauss_Weight2[2];
	static double Gauss_Point3[3];
	static double Gauss_Weight3[3];
	static double Gauss_Point4[4];
	static double Gauss_Weight4[4];
};

}
