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
#include "Matrix/kmbDiagonalMatrix.h"
#include "Matrix/kmbVector.h"
#include <algorithm>

kmb::DiagonalMatrix::DiagonalMatrix(int size)
: kmb::SquareMatrix(size)
{
}

kmb::DiagonalMatrix::~DiagonalMatrix(void)
{
}

bool
kmb::DiagonalMatrix::transpose(void)
{
	return true;
}

bool
kmb::DiagonalMatrix::row_exchange(int i0,int i1)
{
	return false;
}

bool
kmb::DiagonalMatrix::row_transf(int i0,int i1,double r)
{
	return false;
}

bool
kmb::DiagonalMatrix::solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const
{
	int cSize = this->getSize();
	if( cSize != b.getSize() ||
		cSize != x.getSize () )
	{
		return false;
	}
	for(int i=cSize-1;i>=0;--i)
	{
		double s = b.getRow(i);
		double d = this->get(i,i);
		if( d == 0.0 ){
			return false;
		}else{
			x.setRow(i,s/d);
		}
	}
	return true;
}

double
kmb::DiagonalMatrix::determinant(void) const
{
	double det = 1.0;
	int size = getSize();
	for(int i=0;i<size;++i){
		det *= get(i,i);
	}
	return det;
}

kmb::DiagonalMatrix_DoubleArray::DiagonalMatrix_DoubleArray(int size)
: kmb::DiagonalMatrix(size)
, values(NULL)
{
	values = new double[size];
	std::fill(values,values+size,0.0);
}

kmb::DiagonalMatrix_DoubleArray::~DiagonalMatrix_DoubleArray(void)
{
	clear();
}

void
kmb::DiagonalMatrix_DoubleArray::clear(void)
{
	if( values ){
		delete[] values;
		values = NULL;
	}
}

int
kmb::DiagonalMatrix_DoubleArray::init(int rowSize, int colSize)
{
	if( rowSize == colSize && rowSize > 0 ){
		clear();
		this->mSize = rowSize;
		values = new double[this->mSize];
		std::fill_n(values,this->mSize,0.0);
		return 0;
	}else{
		return -1;
	}
}

double
kmb::DiagonalMatrix_DoubleArray::get(int i,int j) const
{
	if( values && 0 <= i && i < this->mSize && i == j){
		return values[i];
	}else{
		return 0.0;
	}
}

bool
kmb::DiagonalMatrix_DoubleArray::set(int i,int j,double val)
{
	if( values && 0 <= i && i < this->mSize && i == j){
		values[i] = val;
		return true;
	}else{
		return false;
	}
}

kmb::DiagonalMatrix_Wrapper::DiagonalMatrix_Wrapper(const kmb::SquareMatrix* mtx)
: kmb::DiagonalMatrix(mtx->getSize())
, matrix(mtx)
{
}

kmb::DiagonalMatrix_Wrapper::~DiagonalMatrix_Wrapper(void)
{
}

const char*
kmb::DiagonalMatrix_Wrapper::getContainerType(void) const
{
	return "diagonal_wrapper";
}

int
kmb::DiagonalMatrix_Wrapper::init(int rowSize, int colSize)
{
	return -1;
}

double
kmb::DiagonalMatrix_Wrapper::get(int i,int j) const
{
	if( 0 <= i && i < this->mSize && i == j){
		return this->matrix->get(i,j);
	}else{
		return 0.0;
	}
}

bool
kmb::DiagonalMatrix_Wrapper::set(int i,int j,double val)
{
	return false;
}

bool
kmb::DiagonalMatrix_Wrapper::add(int i,int j,double val)
{
	return false;
}
