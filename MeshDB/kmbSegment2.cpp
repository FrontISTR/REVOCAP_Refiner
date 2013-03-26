/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Segment2                                                #
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
#include "MeshDB/kmbSegment2.h"
#include "Common/kmbCalculator.h"

/********************************************************************************
=begin

=== 2ŽŸü•ª—v‘f (SEGMENT2)

Ú‘±s—ñ

	{ 0, 1, 2},
	{-1, 0,-2},
	{-2, 2, 0}

=end

Œ`óŠÖ”
0 : (s-1)(2s-1)     => s = 0
1 : s(2s-1)         => s = 1
2 : 4s(1-s)         => s = 1/2

*********************************************************************************/

const int kmb::Segment2::nodeCount = 3;

const int kmb::Segment2::connectionTable[3][3] =
{
	{ 0, 1, 2},
	{-1, 0,-2},
	{-2, 2, 0}
};

kmb::Segment2::Segment2(void)
: kmb::Element(kmb::SEGMENT2)
{
	cell = new kmb::nodeIdType[3];
}

kmb::Segment2::Segment2(kmb::nodeIdType *ary)
: kmb::Element(kmb::SEGMENT2)
{
	cell = ary;
}

kmb::Segment2::~Segment2(void)
{
}

void
kmb::Segment2::shapeFunction(double s,double* coeff)
{
	coeff[0] = (2.0*s-1.0)*(s-1.0);
	coeff[1] = s*(2.0*s-1.0);
	coeff[2] = 4.0*s*(1.0-s);
}

double
kmb::Segment2::checkShapeFunctionDomain(double s)
{
	return kmb::Minimizer::getMin( 1.0-s, s );
}
