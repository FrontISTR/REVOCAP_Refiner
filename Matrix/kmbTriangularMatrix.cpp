/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : UpperTriangularMatrix, LowerTriangularMatrix            #
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

#include "Matrix/kmbTriangularMatrix.h"
#include "Matrix/kmbVector.h"
#include "Matrix/kmbMatrix_DoubleArray.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

bool
kmb::UpperTriangularMatrix::transpose(void)
{
	return false;
}

bool
kmb::UpperTriangularMatrix::row_exchange(int i0,int i1)
{
	return false;
}

bool
kmb::UpperTriangularMatrix::row_transf(int i0,int i1,double r)
{
	return false;
}

double
kmb::UpperTriangularMatrix::rowProduct_upper(int i, const kmb::ColumnVector &vec) const
{
	int cSize = getColSize();
	if( cSize == vec.getSize() ){
		double s = 0.0;
		for(int j=i+1;j<cSize;++j){
			s += get(i,j) * vec[j];
		}
		return s;
	}else{
		return -DBL_MAX;
	}
}

double kmb::UpperTriangularMatrix::getDiagonal(int i) const
{
	return get(i,i);
}

double
kmb::UpperTriangularMatrix::determinant(void) const
{
	double det = 1.0;
	int size = getSize();
	for(int i=0;i<size;++i){
		det *= getDiagonal(i);
	}
	return det;
}

bool
kmb::UpperTriangularMatrix::solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const
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
		s -= this->rowProduct_upper(i,x);
		double d = this->getDiagonal(i);
		if( d == 0.0 ){
			return false;
		}else{
			x.setRow(i,s/d);
		}
	}
	return true;
}

double kmb::UpperTriangularDiagonalUnitMatrix::getDiagonal(int i) const
{
	return 1.0;
}

bool
kmb::UpperTriangularDiagonalUnitMatrix::solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const
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
		s -= this->rowProduct_upper(i,x);
		x.setRow(i,s);
	}
	return true;
}

kmb::UpperTriangularMatrix_Wrapper::UpperTriangularMatrix_Wrapper(const kmb::SquareMatrix* mtx)
: kmb::UpperTriangularMatrix(mtx->getSize())
{
	this->matrix = mtx;
}

kmb::UpperTriangularMatrix_Wrapper::~UpperTriangularMatrix_Wrapper(void)
{
}

const char*
kmb::UpperTriangularMatrix_Wrapper::getContainerType(void) const
{
	return "upper_triangular_wrapper";
}

int
kmb::UpperTriangularMatrix_Wrapper::init(int rowSize, int colSize)
{
	return -1;
}

double
kmb::UpperTriangularMatrix_Wrapper::get(int i,int j) const
{
	if( 0 <= i && i <= j && j < this->mSize ){
		return this->matrix->get(i,j);
	}else{
		return 0.0;
	}
}

bool
kmb::UpperTriangularMatrix_Wrapper::set(int i,int j,double val)
{
	return false;
}

bool
kmb::UpperTriangularMatrix_Wrapper::add(int i,int j,double val)
{
	return false;
}

kmb::UpperTriangularDiagonalUnitMatrix_Wrapper::UpperTriangularDiagonalUnitMatrix_Wrapper(const kmb::SquareMatrix* mtx)
: kmb::UpperTriangularDiagonalUnitMatrix(mtx->getSize())
{
	this->matrix = mtx;
}

kmb::UpperTriangularDiagonalUnitMatrix_Wrapper::~UpperTriangularDiagonalUnitMatrix_Wrapper(void)
{
}

const char*
kmb::UpperTriangularDiagonalUnitMatrix_Wrapper::getContainerType(void) const
{
	return "upper_triangular_diagonal_unit_wrapper";
}

int
kmb::UpperTriangularDiagonalUnitMatrix_Wrapper::init(int rowSize, int colSize)
{
	return -1;
}

double
kmb::UpperTriangularDiagonalUnitMatrix_Wrapper::get(int i,int j) const
{
	if( 0 <= i && j < this->mSize ){
		if( i < j ){
			return this->matrix->get(i,j);
		}else if( i == j ){
			return 1.0;
		}
	}
	return 0.0;
}

bool
kmb::UpperTriangularDiagonalUnitMatrix_Wrapper::set(int i,int j,double val)
{
	return false;
}

bool
kmb::UpperTriangularDiagonalUnitMatrix_Wrapper::add(int i,int j,double val)
{
	return false;
}

bool
kmb::LowerTriangularMatrix::transpose(void)
{
	return false;
}

bool
kmb::LowerTriangularMatrix::row_exchange(int i0,int i1)
{
	return false;
}

bool
kmb::LowerTriangularMatrix::row_transf(int i0,int i1,double r)
{
	return false;
}

double
kmb::LowerTriangularMatrix::rowProduct_lower(int i, const kmb::ColumnVector &vec) const
{
	int cSize = getColSize();
	if( cSize == vec.getSize() ){
		double s = 0.0;
		for(int j=0;j<i;++j){
			s += get(i,j) * vec[j];
		}
		return s;
	}else{
		return -DBL_MAX;
	}
}

double kmb::LowerTriangularMatrix::getDiagonal(int i) const
{
	return get(i,i);
}

double
kmb::LowerTriangularMatrix::determinant(void) const
{
	double det = 1.0;
	int size = getSize();
	for(int i=0;i<size;++i){
		det *= getDiagonal(i);
	}
	return det;
}

bool
kmb::LowerTriangularMatrix::solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const
{
	int cSize = this->getSize();
	if( cSize != b.getSize() ||
		cSize != x.getSize() )
	{
		return false;
	}
	for(int i=0;i<cSize;++i)
	{
		double s = b.getRow(i);
		s -= this->rowProduct_lower(i,x);
		double d = this->getDiagonal(i);
		if( d == 0.0 ){
			return false;
		}else{
			x.setRow(i,s/d);
		}
	}
	return true;
}

kmb::LowerTriangularMatrix_Wrapper::LowerTriangularMatrix_Wrapper(const kmb::SquareMatrix* mtx)
: kmb::LowerTriangularMatrix(mtx->getSize())
{
	this->matrix = mtx;
}

kmb::LowerTriangularMatrix_Wrapper::~LowerTriangularMatrix_Wrapper(void)
{
}

const char*
kmb::LowerTriangularMatrix_Wrapper::getContainerType(void) const
{
	return "lower_triangular_wrapper";
}


int
kmb::LowerTriangularMatrix_Wrapper::init(int rowSize, int colSize)
{
	return -1;
}

double
kmb::LowerTriangularMatrix_Wrapper::get(int i,int j) const
{
	if( 0 <= j && j <= i && i < this->mSize ){
		return this->matrix->get(i,j);
	}else{
		return 0.0;
	}
}

bool
kmb::LowerTriangularMatrix_Wrapper::set(int i,int j,double val)
{
	return false;
}

bool
kmb::LowerTriangularMatrix_Wrapper::add(int i,int j,double val)
{
	return false;
}
