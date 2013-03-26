/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : QuadInterface                                           #
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
#include "MeshDB/kmbQuadInterface.h"
/*

 3--6--2
 |  |  |
 7--8--5
 |  |  |
 0--4--1

 3-----2
 |     |
 |     |
 |     |
 0-----1

 */

/********************************************************************************
=begin

=== 1次四角形インターフェイス要素 (QUADRILATERAL_INTERFACE)

接続行列

	{ 0, 1, 0, 1, 1, 0, 0, 1, 0},
	{ 1, 0, 1, 0, 1, 1, 0, 0, 0},
	{ 0, 1, 0, 1, 0, 1, 1, 0, 0},
	{ 1, 0, 1, 0, 0, 0, 1, 1, 0},
	{ 1, 1, 0, 0, 0, 0, 0, 0, 1},
	{ 0, 1, 1, 0, 0, 0, 0, 0, 1},
	{ 0, 0, 1, 1, 0, 0, 0, 0, 1},
	{ 1, 0, 0, 1, 0, 0, 0, 0, 1},
	{ 0, 0, 0, 0, 1, 1, 1, 1, 0},

面

	{ 3, 2, 1, 0},
	{ 0, 4, 8, 7},
	{ 1, 5, 8, 4},
	{ 2, 6, 8, 5},
	{ 3, 7, 8, 6}

=end

*********************************************************************************/
const int kmb::QuadInterface::nodeCount = 9;

const int kmb::QuadInterface::connectionTable[9][9] =
{
	{ 0, 1, 0, 1, 1, 0, 0, 1, 0},
	{ 1, 0, 1, 0, 1, 1, 0, 0, 0},
	{ 0, 1, 0, 1, 0, 1, 1, 0, 0},
	{ 1, 0, 1, 0, 0, 0, 1, 1, 0},
	{ 1, 1, 0, 0, 0, 0, 0, 0, 1},
	{ 0, 1, 1, 0, 0, 0, 0, 0, 1},
	{ 0, 0, 1, 1, 0, 0, 0, 0, 1},
	{ 1, 0, 0, 1, 0, 0, 0, 0, 1},
	{ 0, 0, 0, 0, 1, 1, 1, 1, 0},
};

const int kmb::QuadInterface::faceTable[5][4] =
{
	{ 3, 2, 1, 0},
	{ 0, 4, 8, 7},
	{ 1, 5, 8, 4},
	{ 2, 6, 8, 5},
	{ 3, 7, 8, 6}
};

const int kmb::QuadInterface::edgeTable[16][2] =
{
	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 0, 3},
	{ 0, 4},
	{ 1, 4},
	{ 1, 5},
	{ 2, 5},
	{ 2, 6},
	{ 3, 6},
	{ 3, 7},
	{ 0, 7},
	{ 4, 8},
	{ 5, 8},
	{ 6, 8},
	{ 7, 8}
};

const int kmb::QuadInterface::faceIndexTable[9][9] =
{
	{ -1,  5,  0,  8,  9, -1, -1, 16,  1},
	{  5, -1,  6,  0, 10, 11, -1, -1,  2},
	{  0,  6, -1,  7, -1, 12, 13, -1,  3},
	{  8,  0,  7, -1, -1, -1, 14, 15,  4},
	{  9, 10, -1, -1, -1,  2, -1,  1, 17},
	{ -1, 11, 12, -1,  2, -1,  3, -1, 18},
	{ -1, -1, 13, 14, -1,  3, -1,  4, 19},
	{ 16, -1, -1, 15,  1, -1,  4, -1, 20},
	{  1,  2,  3,  4, 17, 18, 19, 20, -1}
};

kmb::QuadInterface::QuadInterface(void)
: kmb::Element(kmb::QUADRILATERAL_INTERFACE)
{
	cell = new kmb::nodeIdType[9];
}

kmb::QuadInterface::QuadInterface(kmb::nodeIdType *ary)
: kmb::Element(kmb::QUADRILATERAL_INTERFACE)
{
	cell = ary;
}

kmb::QuadInterface::~QuadInterface(void)
{
}

