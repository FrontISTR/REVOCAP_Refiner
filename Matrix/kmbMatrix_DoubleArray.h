/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matrix_DoubleArray                                      #
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
	Matrix_DoubleArray(const kmb::Matrix& mat);
	virtual ~Matrix_DoubleArray(void);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual int getColSize(void) const;
	virtual int getRowSize(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
	virtual double& operator()(int i,int j){
		return m[i+j*rowSize];
	}
	virtual const double& operator()(int i,int j) const{
		return m[i+j*rowSize];
	}
	virtual bool row_exchange(int i0,int i1);

	virtual bool row_transf(int i0,int i1,double r);
	virtual bool row_multi(int i0,double r);
protected:
	void clear(void);
};

class SquareMatrix_DoubleArray : public SquareMatrix{
private:
	double *m;
public:
	SquareMatrix_DoubleArray(int size);

	SquareMatrix_DoubleArray(int size,double* ary);
	SquareMatrix_DoubleArray(const kmb::SquareMatrix& mat);
	virtual ~SquareMatrix_DoubleArray(void);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
	virtual double& operator()(int i,int j){
		return m[i+j*mSize];
	}
	virtual const double& operator()(int i,int j) const{
		return m[i+j*mSize];
	}
protected:
	void clear(void);
};

class ColumnVector_DoubleArray : public ColumnVector{
private:
	double *m;
	int size;
public:
	ColumnVector_DoubleArray(int size);

	ColumnVector_DoubleArray(int size,double* ary);
	ColumnVector_DoubleArray(const kmb::ColumnVector &vec);
	virtual ~ColumnVector_DoubleArray(void);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual int getSize(void) const;
	virtual int getRowSize(void) const;
	virtual double getRow(int i) const;
	virtual void setRow(int i,double val);
	virtual void addRow(int i,double val);
	virtual double& operator[](const int i){ return m[i]; };
	virtual double operator[](const int i) const{ return m[i]; };
	virtual double& operator()(int i,int j);
	virtual const double& operator()(int i,int j) const;
	virtual ColumnVector_DoubleArray& operator=(const ColumnVector_DoubleArray& other);
protected:
	void clear(void);
};

class RowVector_DoubleArray : public RowVector{
private:
	double *m;
	int size;
public:
	RowVector_DoubleArray(int size);

	RowVector_DoubleArray(int size,double* ary);
	RowVector_DoubleArray(const kmb::RowVector &vec);
	virtual ~RowVector_DoubleArray(void);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual int getSize(void) const;
	virtual double getColumn(int i) const;
	virtual void setColumn(int i,double val);
	virtual void addColumn(int i,double val);
	virtual double& operator[](const int i){ return m[i]; };
	virtual double operator[](const int i) const{ return m[i]; };
	virtual double& operator()(int i,int j);
	virtual const double& operator()(int i,int j) const;
	virtual RowVector_DoubleArray& operator=(const RowVector_DoubleArray& other);
protected:
	void clear(void);
};

}
