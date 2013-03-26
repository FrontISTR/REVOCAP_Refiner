/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : DiagonalMatrix                                          #
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

namespace kmb{

class DiagonalMatrix : public SquareMatrix
{
public:
	DiagonalMatrix(int size);
	virtual ~DiagonalMatrix(void);
	virtual bool solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const;
	virtual double determinant(void) const;

	virtual bool transpose(void);

	virtual bool row_exchange(int i0,int i1);
	virtual bool row_transf(int i0,int i1,double r);
};

class DiagonalMatrix_DoubleArray : public DiagonalMatrix
{
private:
	double *values;
public:
	DiagonalMatrix_DoubleArray(int size);
	virtual ~DiagonalMatrix_DoubleArray(void);
	virtual int init(int rowSize, int colSize);
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
protected:
	void clear(void);
};


class DiagonalMatrix_Wrapper : public DiagonalMatrix
{
private:
	const kmb::SquareMatrix* matrix;
public:
	DiagonalMatrix_Wrapper(const kmb::SquareMatrix* mtx);
	virtual ~DiagonalMatrix_Wrapper(void);
	virtual const char* getContainerType(void) const;
	virtual int init(int rowSize, int colSize);
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
};

}
