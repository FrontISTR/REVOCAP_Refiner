/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Quad                                                    #
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
#include "MeshDB/kmbQuad.h"

/********************************************************************************
=begin

=== 1次四角形要素 (QUAD)

接続行列

	{ 0, 1, 0,-1},
	{-1, 0, 1, 0},
	{ 0,-1, 0, 1},
	{ 1, 0,-1, 0}

辺

	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 3, 0},

=end

形状関数
0 : 1/4(1-s)(1-t) => (s,t) = (-1,-1)
1 : 1/4(1+s)(1-t) => (s,t) = ( 1,-1)
2 : 1/4(1+s)(1+t) => (s,t) = ( 1, 1)
3 : 1/4(1-s)(1+t) => (s,t) = (-1, 1)

*********************************************************************************/
const int kmb::Quad::nodeCount = 4;

const int kmb::Quad::connectionTable[4][4] =
{
	{ 0, 1, 0,-1},
	{-1, 0, 1, 0},
	{ 0,-1, 0, 1},
	{ 1, 0,-1, 0}
};

const int kmb::Quad::faceTable[4][2] =
{
	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 3, 0},
};

kmb::Quad::Quad(void)
: kmb::Element(kmb::QUAD)
{
	cell = new kmb::nodeIdType[4];
}

kmb::Quad::Quad(kmb::nodeIdType *ary)
: kmb::Element(kmb::QUAD)
{
	cell = ary;
}

kmb::Quad::Quad(kmb::nodeIdType i0,kmb::nodeIdType i1,kmb::nodeIdType i2,kmb::nodeIdType i3)
: kmb::Element(kmb::QUAD)
{
	cell = new kmb::nodeIdType[4];
	cell[0] = i0;
	cell[1] = i1;
	cell[2] = i2;
	cell[3] = i3;
}

kmb::Quad::~Quad(void)
{
}

void
kmb::Quad::shapeFunction(double s,double t,double* coeff)
{
	coeff[0] = 0.25*(1.0-s)*(1.0-t);
	coeff[1] = 0.25*(1.0+s)*(1.0-t);
	coeff[2] = 0.25*(1.0+s)*(1.0+t);
	coeff[3] = 0.25*(1.0-s)*(1.0+t);
}

bool
kmb::Quad::edgeSwap(kmb::ElementBase &quad0,kmb::ElementBase &quad1,bool orientation)
{
	bool swapped = false;
	if( quad0.getType() == kmb::QUAD && quad1.getType() == kmb::QUAD )
	{

		unsigned int rel = 0;
		unsigned int flag = 0x0001;
		for(int i=0;i<4;++i){
			for(int j=0;j<4;++j){
				if( quad0.getCellId(i) == quad1.getCellId(j) )	rel |= flag;
				flag = flag << 1;
			}
		}
		kmb::nodeIdType nodes[6]
			= {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};

		switch( rel ){
		case 0x0081:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0012:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0024:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0048:
			nodes[0] = quad0.getCellId(0);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(1);
			nodes[4] = quad0.getCellId(2);
			nodes[5] = quad0.getCellId(3);
			swapped = true;
			break;
		case 0x0810:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x0120:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x0240:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x0480:
			nodes[0] = quad0.getCellId(1);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(2);
			nodes[4] = quad0.getCellId(3);
			nodes[5] = quad0.getCellId(0);
			swapped = true;
			break;
		case 0x8100:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x1200:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x2400:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x4800:
			nodes[0] = quad0.getCellId(2);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(3);
			nodes[4] = quad0.getCellId(0);
			nodes[5] = quad0.getCellId(1);
			swapped = true;
			break;
		case 0x1008:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(1);
			nodes[2] = quad1.getCellId(2);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		case 0x2001:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(2);
			nodes[2] = quad1.getCellId(3);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		case 0x4002:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(3);
			nodes[2] = quad1.getCellId(0);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		case 0x8004:
			nodes[0] = quad0.getCellId(3);
			nodes[1] = quad1.getCellId(0);
			nodes[2] = quad1.getCellId(1);
			nodes[3] = quad0.getCellId(0);
			nodes[4] = quad0.getCellId(1);
			nodes[5] = quad0.getCellId(2);
			swapped = true;
			break;
		default:
			break;
		}
		if( swapped ){
			if( orientation ){

				quad0.setCellId(0, nodes[1]);
				quad0.setCellId(1, nodes[2]);
				quad0.setCellId(2, nodes[3]);
				quad0.setCellId(3, nodes[4]);
				quad1.setCellId(0, nodes[0]);
				quad1.setCellId(1, nodes[1]);
				quad1.setCellId(2, nodes[4]);
				quad1.setCellId(3, nodes[5]);
			}else{

				quad0.setCellId(0, nodes[0]);
				quad0.setCellId(1, nodes[1]);
				quad0.setCellId(2, nodes[2]);
				quad0.setCellId(3, nodes[5]);
				quad1.setCellId(0, nodes[2]);
				quad1.setCellId(1, nodes[3]);
				quad1.setCellId(2, nodes[4]);
				quad1.setCellId(3, nodes[5]);
			}
		}
	}
	return swapped;
}

int
kmb::Quad::isCoincident(kmb::nodeIdType t00,kmb::nodeIdType t01,kmb::nodeIdType t02,kmb::nodeIdType t03,
						kmb::nodeIdType t10,kmb::nodeIdType t11,kmb::nodeIdType t12,kmb::nodeIdType t13)
{
	unsigned int rel = 0;
	if( t00 == t10 )	rel |= 0x1000;
	if( t00 == t11 )	rel |= 0x2000;
	if( t00 == t12 )	rel |= 0x4000;
	if( t00 == t13 )	rel |= 0x8000;
	if( t01 == t10 )	rel |= 0x0100;
	if( t01 == t11 )	rel |= 0x0200;
	if( t01 == t12 )	rel |= 0x0400;
	if( t01 == t13 )	rel |= 0x0800;
	if( t02 == t10 )	rel |= 0x0010;
	if( t02 == t11 )	rel |= 0x0020;
	if( t02 == t12 )	rel |= 0x0040;
	if( t02 == t13 )	rel |= 0x0080;
	if( t03 == t10 )	rel |= 0x0001;
	if( t03 == t11 )	rel |= 0x0002;
	if( t03 == t12 )	rel |= 0x0004;
	if( t03 == t13 )	rel |= 0x0008;
	switch( rel ){

	case 0x1248:
	case 0x2481:
	case 0x4812:
	case 0x8124:
		return 1;

	case 0x8421:
	case 0x4218:
	case 0x2184:
	case 0x1842:
		return -1;
	default:
		return 0;
	}
}
