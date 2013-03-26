/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Geometry                                                #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/


#pragma once

#include "Geometry/kmbIdTypes.h"


#define LARGER_Y(p0,p1) ( (p0).y() > (p1).y() || ( (p0).y() == (p1).y() && (p0).x() < (p1).x() ) )
#define LARGER_X(p0,p1) ( (p0).x() > (p1).x() || ( (p0).x() == (p1).x() && (p0).y() > (p1).y() ) )




#define COS2ANGLE(c) ( (c>=1.0) ? 0.0 : (c<=-1.0) ? PI : acos(c) )

#define ANGLE2(c,s) ( (c>=1.0) ? 0.0 : (c<=-1.0) ? PI : (s>0.0) ? acos(c) : 2.0*PI-acos(c) )

#define ANGLE(c,s) ( (c>=1.0) ? 0.0 : (c<=-1.0) ? PI : (s>0.0) ? acos(c) : -acos(c) )

