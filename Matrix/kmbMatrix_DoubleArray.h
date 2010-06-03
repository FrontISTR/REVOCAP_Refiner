/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Matrix_DoubleArray                                      #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"

namespace kmb{




class Matrix_DoubleArray : public Matrix{
private:
	double *m;
	int rowSize;
	int colSize;
public:
	Matrix_DoubleArray(int rowSize, int colSize);
	Matrix_DoubleArray(int rowSize, int colSize,double* ary);
	virtual ~Matrix_DoubleArray(void);
	virtual matrixType getType(void) const;
	virtual int getColSize(void) const;
	virtual int getRowSize(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool row_exchange(int i0,int i1);
	virtual bool row_transf(int i0,int i1,double r);
	virtual bool row_multi(int i0,double r);
};

class SquareMatrix_DoubleArray : public SquareMatrix{
private:
	double *m;
	int size;
public:
	SquareMatrix_DoubleArray(int size);
	SquareMatrix_DoubleArray(int size,double* ary);
	virtual ~SquareMatrix_DoubleArray(void);
	virtual int getSize(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
};

class ColumnVector_DoubleArray : public ColumnVector{
private:
	double *m;
	int size;
public:
	ColumnVector_DoubleArray(int size);
	ColumnVector_DoubleArray(int size,double* ary);
	virtual ~ColumnVector_DoubleArray(void);
	virtual int getSize(void) const;
	virtual double getRow(int i) const;
	virtual void setRow(int i,double val);
};

class RowVector_DoubleArray : public RowVector{
private:
	double *m;
	int size;
public:
	RowVector_DoubleArray(int size);
	RowVector_DoubleArray(int size,double* ary);
	virtual ~RowVector_DoubleArray(void);
	virtual int getSize(void) const;
	virtual double getColumn(int i) const;
	virtual void setColumn(int i,double val);
};

}
