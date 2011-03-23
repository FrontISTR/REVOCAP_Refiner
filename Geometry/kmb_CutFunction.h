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
#pragma once

namespace kmb{

class CutFunction
{
public:
	CutFunction(void);
	virtual ~CutFunction(void);
	virtual double f(double x,double y,double z) const = 0;
};

class CutFunctionUnion : public CutFunction
{
private:
	kmb::CutFunction* c0;
	kmb::CutFunction* c1;
public:
	CutFunctionUnion(kmb::CutFunction* cut0,kmb::CutFunction* cut1);
	virtual ~CutFunctionUnion(void);
	virtual double f(double x,double y,double z) const;
};

class CutFunctionIntersection : public CutFunction
{
private:
	kmb::CutFunction* c0;
	kmb::CutFunction* c1;
public:
	CutFunctionIntersection(kmb::CutFunction* cut0,kmb::CutFunction* cut1);
	virtual ~CutFunctionIntersection(void);
	virtual double f(double x,double y,double z) const;
};

class CutFunctionDifference : public CutFunction
{
private:
	kmb::CutFunction* c0;
	kmb::CutFunction* c1;
public:
	CutFunctionDifference(kmb::CutFunction* cut0,kmb::CutFunction* cut1);
	virtual ~CutFunctionDifference(void);
	virtual double f(double x,double y,double z) const;
};

class CutFunctionComplement : public CutFunction
{
private:
	kmb::CutFunction* c;
public:
	CutFunctionComplement(kmb::CutFunction* c);
	virtual ~CutFunctionComplement(void);
	virtual double f(double x,double y,double z) const;
};

}
