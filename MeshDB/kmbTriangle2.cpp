/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Triangle2                                               #
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
#include "MeshDB/kmbTriangle2.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbSegment2.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElementRelation.h"
#include "Geometry/kmb_Circle.h"

/********************************************************************************
=begin

=== 2次三角形要素 (TRIANGLE2)

接続行列

	{  0,  1, -1,  0, -2,  2},
	{ -1,  0,  1,  2,  0, -2},
	{  1, -1,  0, -2,  2,  0},
	{  0, -2,  2,  0,  0,  0},
	{  2,  0, -2,  0,  0,  0},
	{ -2,  2,  0,  0,  0,  0},

辺

	{ 1, 2, 3},
	{ 2, 0, 4},
	{ 0, 1, 5},

=end

形状関数
0 : (1-s-t)*(1-2*s-2*t) => (s,t) = (  0,   0)
1 : s*(2*s-1)           => (s,t) = (  1,   0)
2 : t*(2*t-1)           => (s,t) = (  0,   1)
3 : 4*s*t               => (s,t) = (0.5, 0.5)
4 : 4*t*(1-s-t)         => (s,t) = (  0, 0.5)
5 : 4*s*(1-s-t)         => (s,t) = (0.5,   0)

*********************************************************************************/
const int kmb::Triangle2::nodeCount = 6;

const int kmb::Triangle2::connectionTable[6][6] =
{
	{  0,  1, -1,  0, -2,  2},
	{ -1,  0,  1,  2,  0, -2},
	{  1, -1,  0, -2,  2,  0},
	{  0, -2,  2,  0,  0,  0},
	{  2,  0, -2,  0,  0,  0},
	{ -2,  2,  0,  0,  0,  0},
};

const int kmb::Triangle2::faceTable[3][3] =
{
	{ 1, 2, 3},
	{ 2, 0, 4},
	{ 0, 1, 5},
};

kmb::Triangle2::Triangle2(void)
: kmb::Element(kmb::TRIANGLE2)
{
	cell = new kmb::nodeIdType[6];
}

kmb::Triangle2::Triangle2(kmb::nodeIdType *ary)
: kmb::Element(kmb::TRIANGLE2)
{
	cell = ary;
}

kmb::Triangle2::~Triangle2(void)
{
}

void
kmb::Triangle2::shapeFunction(double s,double t,double* coeff)
{
	coeff[0] = (1.0-s-t)*(1.0-2.0*s-2.0*t);
	coeff[1] = s*(2.0*s-1.0);
	coeff[2] = t*(2.0*t-1.0);
	coeff[3] = 4.0*s*t;
	coeff[4] = 4.0*t*(1.0-s-t);
	coeff[5] = 4.0*s*(1.0-s-t);
}















































































































































































































































































































































































































































































