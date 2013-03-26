/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Segment                                                 #
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
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbMeshData.h"
#include "Common/kmbCalculator.h"

/********************************************************************************
=begin

=== 1ŽŸü•ª—v‘f (SEGMENT)

Ú‘±s—ñ

	{ 0, 1},
	{-1, 0}

=end

Œ`óŠÖ”
0 : 1-s   => s = 0
1 : s     => s = 1

*********************************************************************************/

const int kmb::Segment::nodeCount = 2;

const int kmb::Segment::connectionTable[2][2] =
{
	{ 0, 1},
	{-1, 0}
};

kmb::Segment::Segment(void)
: kmb::Element(kmb::SEGMENT)
{
	cell = new kmb::nodeIdType[2];
}

kmb::Segment::Segment(kmb::nodeIdType *ary)
: kmb::Element(kmb::SEGMENT)
{
	cell = ary;
}

kmb::Segment::Segment(kmb::nodeIdType a,kmb::nodeIdType b)
: kmb::Element(kmb::SEGMENT)
{
	cell = new kmb::nodeIdType[2];
	cell[0] = a;
	cell[1] = b;
}

kmb::Segment::~Segment(void)
{
}

void
kmb::Segment::shapeFunction(double s,double* coeff)
{
	coeff[0] = 1.0-s;
	coeff[1] = s;
}

double
kmb::Segment::checkShapeFunctionDomain(double s)
{
	return kmb::Minimizer::getMin( 1.0-s, s );
}
