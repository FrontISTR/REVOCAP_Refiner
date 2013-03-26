/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriangularMatrix_DoubleArray                            #
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
#pragma once

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbTriangularMatrix.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

namespace kmb{

class UpperTriangularMatrix_DoubleArray : public UpperTriangularMatrix
{
private:
	double *m;
public:
	UpperTriangularMatrix_DoubleArray(int size);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
protected:
	void clear(void);
};

class LowerTriangularMatrix_DoubleArray : public LowerTriangularMatrix
{
private:
	double *m;
public:
	LowerTriangularMatrix_DoubleArray(int size);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
protected:
	void clear(void);
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
