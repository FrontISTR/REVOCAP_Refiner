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
 * �v���W�F�N�g�S�̂ŋ��L������ typedef �� enum ���L�q����
 * �N���X�ŋǏ��I�ɗp���� enum �̓N���X�̓����Œ�`����
 *
 * ������ int32_t �����g���ƁAswing �� native �^�Ɣ��f�ł��Ȃ��Ȃ��Ă��܂�
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
