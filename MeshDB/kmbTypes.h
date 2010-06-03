/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Types                                                   #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
/*
 * プロジェクト全体で共有したい typedef や enum を記述する
 * クラスで局所的に用いる enum はクラスの内部で定義する
 *
 * ここで int32_t 等を使うと、swing が native 型と判断できなくなってしまう
 */

#pragma once

#include <set>
#include <map>

#include "Geometry/kmb_Common.h"       /* SIZE NULL PI */
#include "Geometry/kmb_idTypes.h"      /* nodeIdType */
#include "Geometry/kmb_Geometry3D.h"   /* for Point3D */

namespace kmb{

typedef int elementIdType;
typedef int bodyIdType;
typedef Point3D Node;

typedef std::set<elementIdType> ElementNeighbor;

}
