/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : CutFunction                                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

#include "Geometry/kmb_CutFunction.h"
#include "Geometry/kmb_Calculator.h"

kmb::CutFunction::CutFunction(void)
{
}

kmb::CutFunction::~CutFunction(void)
{
}

kmb::CutFunctionUnion::CutFunctionUnion(kmb::CutFunction* cut0,kmb::CutFunction* cut1)
: CutFunction()
, c0(cut0)
, c1(cut1)
{
}

kmb::CutFunctionUnion::~CutFunctionUnion(void)
{
}

double
kmb::CutFunctionUnion::f(double x,double y,double z) const
{
	return kmb::Maximizer::getMax( c0->f(x,y,z), c1->f(x,y,z) );
}

kmb::CutFunctionIntersection::CutFunctionIntersection(kmb::CutFunction* cut0,kmb::CutFunction* cut1)
: CutFunction()
, c0(cut0)
, c1(cut1)
{
}

kmb::CutFunctionIntersection::~CutFunctionIntersection(void)
{
}

double
kmb::CutFunctionIntersection::f(double x,double y,double z) const
{
	return kmb::Minimizer::getMin( c0->f(x,y,z), c1->f(x,y,z) );
}


kmb::CutFunctionDifference::CutFunctionDifference(kmb::CutFunction* cut0,kmb::CutFunction* cut1)
: CutFunction()
, c0(cut0)
, c1(cut1)
{
}

kmb::CutFunctionDifference::~CutFunctionDifference(void)
{
}

double
kmb::CutFunctionDifference::f(double x,double y,double z) const
{
	return kmb::Minimizer::getMin( c0->f(x,y,z), -c1->f(x,y,z) );
}

kmb::CutFunctionComplement::CutFunctionComplement(kmb::CutFunction* cut)
: CutFunction()
, c(cut)
{
}

kmb::CutFunctionComplement::~CutFunctionComplement(void)
{
}

double
kmb::CutFunctionComplement::f(double x,double y,double z) const
{
	return -c->f(x,y,z);
}
