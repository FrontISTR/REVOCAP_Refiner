/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Quad2                                                   #
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
#include "MeshDB/kmbQuad2.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbTriangle2.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbElementRelation.h"

/********************************************************************************
=begin

=== 2次四角形要素 (QUAD2)

接続行列

	{ 0, 1, 0,-1, 2, 0, 0,-2},
	{-1, 0, 1, 0,-2, 2, 0, 0},
	{ 0,-1, 0, 1, 0,-2, 2, 0},
	{ 1, 0,-1, 0, 0, 0,-2, 2},
	{-2, 2, 0, 0, 0, 0, 0, 0},
	{ 0,-2, 2, 0, 0, 0, 0, 0},
	{ 0, 0,-2, 2, 0, 0, 0, 0},
	{ 2, 0, 0,-2, 0, 0, 0, 0},

辺

	{ 0, 1, 4},
	{ 1, 2, 5},
	{ 2, 3, 6},
	{ 3, 0, 7},

=end

形状関数 serendipity
0 : 1/4(1-s)(1-t)(-1-s-t) => (s,t) = (-1,-1)
1 : 1/4(1+s)(1-t)(-1+s-t) => (s,t) = ( 1,-1)
2 : 1/4(1+s)(1+t)(-1+s+t) => (s,t) = ( 1, 1)
3 : 1/4(1-s)(1+t)(-1-s+t) => (s,t) = (-1, 1)
4 : 1/2(1-s*s)(1-t)       => (s,t) = ( 0,-1)
5 : 1/2(1+s)(1-t*t)       => (s,t) = ( 1, 0)
6 : 1/2(1-s*s)(1+t)       => (s,t) = ( 0, 1)
7 : 1/2(1-s)(1-t*t)       => (s,t) = (-1, 0)

*********************************************************************************/
const int kmb::Quad2::nodeCount = 8;

const int kmb::Quad2::connectionTable[8][8] =
{
	{ 0, 1, 0,-1, 2, 0, 0,-2},
	{-1, 0, 1, 0,-2, 2, 0, 0},
	{ 0,-1, 0, 1, 0,-2, 2, 0},
	{ 1, 0,-1, 0, 0, 0,-2, 2},
	{-2, 2, 0, 0, 0, 0, 0, 0},
	{ 0,-2, 2, 0, 0, 0, 0, 0},
	{ 0, 0,-2, 2, 0, 0, 0, 0},
	{ 2, 0, 0,-2, 0, 0, 0, 0},
};

const int kmb::Quad2::faceTable[4][3] =
{
	{ 0, 1, 4},
	{ 1, 2, 5},
	{ 2, 3, 6},
	{ 3, 0, 7},
};

kmb::Quad2::Quad2(void)
: Element(kmb::QUAD2)
{
	cell = new kmb::nodeIdType[8];
}

kmb::Quad2::Quad2(kmb::nodeIdType *ary)
: Element(kmb::QUAD2)
{
	cell = ary;
}

kmb::Quad2::~Quad2(void)
{
}

void
kmb::Quad2::shapeFunction(double s,double t,double* coeff)
{
	coeff[0] = 0.25*(1.0-s)*(1.0-t)*(-1.0-s-t);
	coeff[1] = 0.25*(1.0+s)*(1.0-t)*(-1.0+s-t);
	coeff[2] = 0.25*(1.0+s)*(1.0+t)*(-1.0+s+t);
	coeff[3] = 0.25*(1.0-s)*(1.0+t)*(-1.0-s+t);
	coeff[4] = 0.5*(1.0-s*s)*(1.0-t);
	coeff[5] = 0.5*(1.0+s)*(1.0-t*t);
	coeff[6] = 0.5*(1.0-s*s)*(1.0+t);
	coeff[7] = 0.5*(1.0-s)*(1.0-t*t);
}






































































































































































































































































































































































































































