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
#include "Matrix/kmbMatrix_DoubleArray.h"

kmb::Matrix_DoubleArray::Matrix_DoubleArray(int rSize, int cSize)
: Matrix(rSize,cSize)
, m(NULL)
, rowSize(rSize)
, colSize(cSize)
{
	m = new double[rowSize*colSize];
}

kmb::Matrix_DoubleArray::Matrix_DoubleArray(int rSize, int cSize,double* ary)
: Matrix(rSize,cSize)
, m(NULL)
, rowSize(rSize)
, colSize(cSize)
{
	m = new double[rowSize*colSize];
	for(int i=0;i<rowSize*colSize;++i){
		m[i] = ary[i];
	}
}

kmb::Matrix_DoubleArray::~Matrix_DoubleArray(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}

kmb::Matrix::matrixType
kmb::Matrix_DoubleArray::getType(void) const
{
	return kmb::Matrix::DOUBLE_ARRAY;
}

int
kmb::Matrix_DoubleArray::getColSize(void) const
{
	return colSize;
}

int
kmb::Matrix_DoubleArray::getRowSize(void) const
{
	return rowSize;
}

double
kmb::Matrix_DoubleArray::get(int i,int j) const
{
	return m[i+j*rowSize];
}

bool
kmb::Matrix_DoubleArray::set(int i,int j,double val)
{
	m[i+j*rowSize] = val;
	return true;
}

bool
kmb::Matrix_DoubleArray::row_exchange(int i0,int i1)
{
	for(int j=0;j<colSize;j++){
		double t = m[i0+j*rowSize];
		m[i0+j*rowSize] = m[i1+j*rowSize];
		m[i1+j*rowSize] = t;
	}
	return true;
}

bool
kmb::Matrix_DoubleArray::row_transf(int i0,int i1,double r)
{
	for(int j=0;j<colSize;j++){
		m[i1+j*rowSize] += r * m[i0+j*rowSize];
	}
	return true;
}

bool
kmb::Matrix_DoubleArray::row_multi(int i0,double r)
{
	for(int j=0;j<colSize;j++){
		m[i0+j*rowSize] *= r;
	}
	return true;
}

kmb::SquareMatrix_DoubleArray::SquareMatrix_DoubleArray(int size)
: SquareMatrix(size)
, m(NULL)
, size(size)
{
	m = new double[size*size];
}

kmb::SquareMatrix_DoubleArray::SquareMatrix_DoubleArray(int size,double* ary)
: SquareMatrix(size)
, m(NULL)
, size(size)
{
	m = new double[size*size];
	for(int i=0;i<size*size;++i){
		m[i] = ary[i];
	}
}

kmb::SquareMatrix_DoubleArray::~SquareMatrix_DoubleArray(void)
{
	if( m ){
		delete[] m;
	}
}

int
kmb::SquareMatrix_DoubleArray::getSize(void) const
{
	return size;
}

double
kmb::SquareMatrix_DoubleArray::get(int i,int j) const
{
	return m[i+j*size];
}

bool
kmb::SquareMatrix_DoubleArray::set(int i,int j,double val)
{
	m[i+j*size] = val;
	return true;
}

kmb::ColumnVector_DoubleArray::ColumnVector_DoubleArray(int size)
: ColumnVector(size)
, m(NULL)
, size(size)
{
	m = new double[size];
}

kmb::ColumnVector_DoubleArray::ColumnVector_DoubleArray(int size,double* ary)
: ColumnVector(size)
, m(NULL)
, size(size)
{
	m = new double[size];
	for(int i=0;i<size;++i){
		m[i] = ary[i];
	}
}

kmb::ColumnVector_DoubleArray::~ColumnVector_DoubleArray(void)
{
	if( m ){
		delete[] m;
	}
}

int
kmb::ColumnVector_DoubleArray::getSize(void) const
{
	return size;
}

double
kmb::ColumnVector_DoubleArray::getRow(int i) const
{
	return m[i];
}

void
kmb::ColumnVector_DoubleArray::setRow(int i,double val)
{
	m[i] = val;
}

kmb::RowVector_DoubleArray::RowVector_DoubleArray(int size)
: RowVector(size)
, m(NULL)
, size(size)
{
	m = new double[size];
}

kmb::RowVector_DoubleArray::RowVector_DoubleArray(int size,double* ary)
: RowVector(size)
, m(NULL)
, size(size)
{
	m = new double[size];
	for(int i=0;i<size;++i){
		m[i] = ary[i];
	}
}

kmb::RowVector_DoubleArray::~RowVector_DoubleArray(void)
{
	if( m ){
		delete[] m;
	}
}

int
kmb::RowVector_DoubleArray::getSize(void) const
{
	return size;
}

double
kmb::RowVector_DoubleArray::getColumn(int i) const
{
	return m[i];
}

void
kmb::RowVector_DoubleArray::setColumn(int i,double val)
{
	m[i] = val;
}
