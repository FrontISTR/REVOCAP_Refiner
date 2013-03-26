/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriInterface                                            #
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
#include "MeshDB/kmbTriInterface.h"
/*

    2
   / \
  4---3
 / \ / \
0---5---1

    2
   / \
  /   \
 /     \
0-------1

 */

/********************************************************************************
=begin

=== 1次三角形インターフェイス要素 (TRIANGLE_INTERFACE)

接続行列

	{ 0, 1, 0,-1},
	{-1, 0, 1, 0},
	{ 0,-1, 0, 1},
	{ 1, 0,-1, 0}

面

	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 3, 0},

=end

*********************************************************************************/
const int kmb::TriInterface::nodeCount = 9;

const int kmb::TriInterface::connectionTable[6][6] =
{
	{ 0, 1, 1, 0, 1, 1},
	{ 1, 0, 1, 1, 0, 1},
	{ 1, 1, 0, 1, 1, 0},
	{ 0, 1, 1, 0, 1, 1},
	{ 1, 0, 1, 1, 0, 1},
	{ 1, 1, 0, 1, 1, 0},
};

const int kmb::TriInterface::faceTable[5][3] =
{
	{ 2, 1, 0},
	{ 0, 5, 4},
	{ 1, 3, 5},
	{ 2, 4, 3},
	{ 3, 4, 5}
};

const int kmb::TriInterface::edgeTable[12][2] =
{
	{ 1, 2},
	{ 0, 2},
	{ 0, 1},

	{ 0, 5},
	{ 1, 5},
	{ 1, 3},
	{ 2, 3},
	{ 2, 4},
	{ 0, 4},

	{ 4, 5},
	{ 3, 5},
	{ 3, 4},
};

const int kmb::TriInterface::faceIndexTable[6][6] =
{
	{ -1,  2,  1, -1,  8,  3},
	{  2, -1,  0,  5, -1,  4},
	{  1,  0, -1,  6,  7, -1},
	{ -1,  5,  6, -1, 11, 10},
	{  8, -1,  7, 11, -1,  9},
	{  3,  4, -1, 10,  9, -1}
};

kmb::TriInterface::TriInterface(void)
: kmb::Element(kmb::TRIANGLE_INTERFACE)
{
	cell = new kmb::nodeIdType[6];
}

kmb::TriInterface::TriInterface(kmb::nodeIdType *ary)
: kmb::Element(kmb::TRIANGLE_INTERFACE)
{
	cell = ary;
}

kmb::TriInterface::~TriInterface(void)
{
}

