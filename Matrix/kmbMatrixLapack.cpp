/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Matrix_LAPACK                                           #
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


#ifdef REVO_USE_LAPACK

#include "Matrix/kmbMatrixLapack.h"

extern "C"{
#include <f2c.h>
#include <clapack.h>
}

kmb::MatrixLapack::MatrixLapack(int rSize, int cSize)
: kmb::Matrix(rSize,cSize), m(NULL), rowSize(rSize), colSize(cSize), trans('n')
{
	m = new double[rowSize*colSize];
}

kmb::MatrixLapack::MatrixLapack( const Matrix &mtrx )
: kmb::Matrix(mtrx.getRowSize(),mtrx.getColSize())
, m(NULL)
, rowSize(mtrx.getRowSize())
, colSize(mtrx.getColSize())
, trans('n')
{
	m = new double[rowSize*colSize];
	for(int i=0;i<rowSize;++i){
		for(int j=0;j<colSize;++j){
			m[i+j*rowSize] = mtrx.get(i,j);
		}
	}
}

kmb::MatrixLapack::~MatrixLapack(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}

kmb::Matrix::matrixType
kmb::MatrixLapack::getType(void) const
{
	return kmb::Matrix::LAPACK;
}

int
kmb::MatrixLapack::getColSize(void) const
{
	return colSize;
}

int
kmb::MatrixLapack::getRowSize(void) const
{
	return rowSize;
}

char
kmb::MatrixLapack::getTrans(void) const
{
	return trans;
}

double
kmb::MatrixLapack::get(int i,int j) const
{
	return m[i+j*rowSize];
}

bool
kmb::MatrixLapack::set(int i,int j,double val)
{
	m[i+j*rowSize] = val;
	return true;
}

bool
kmb::MatrixLapack::multiply( const kmb::Matrix* a, const kmb::Matrix* b, kmb::Matrix* c )
{
	if( a->getType() == kmb::Matrix::LAPACK &&
		b->getType() == kmb::Matrix::LAPACK &&
		c->getType() == kmb::Matrix::LAPACK &&
		a->getColSize() == b->getRowSize() &&
		b->getColSize() == c->getColSize() &&
		a->getRowSize() == c->getRowSize() )
	{
		const kmb::MatrixLapack* la = reinterpret_cast<const kmb::MatrixLapack*>(a);
		const kmb::MatrixLapack* lb = reinterpret_cast<const kmb::MatrixLapack*>(b);
		kmb::MatrixLapack* lc = reinterpret_cast<kmb::MatrixLapack*>(c);
		double alpha = 1.0;
		double beta = 0.0;
		long m = la->getRowSize();
		long n = lc->getColSize();
		long k = la->getColSize();
		long lda = la->getRowSize();
		long ldb = lb->getRowSize();
		long ldc = lc->getRowSize();
		char transa = la->trans;
		char transb = lb->trans;
		dgemm_( &(transa), &(transb), &m, &n, &k,
			&alpha, la->m, &lda, lb->m, &ldb, &beta, lc->m, &ldc );
		return true;
	}
	return false;
}


bool
kmb::MatrixLapack::multiply_left( const kmb::Matrix &a, kmb::Matrix &x ) const
{
	if( a.getType() == kmb::Matrix::LAPACK && x.getType() == kmb::Matrix::LAPACK ){
		return kmb::MatrixLapack::multiply( this, &a, &x );
	}
	return false;
}

bool
kmb::MatrixLapack::multiply_right( const kmb::Matrix &a, kmb::Matrix &x ) const
{
	if( a.getType() == kmb::Matrix::LAPACK && x.getType() == kmb::Matrix::LAPACK ){
		return kmb::MatrixLapack::multiply( &a, this, &x );
	}
	return false;
}


bool
kmb::MatrixLapack::multiply_substitute( const kmb::Matrix &a, const kmb::Matrix &b)
{
	if( a.getType() == kmb::Matrix::LAPACK && b.getType() == kmb::Matrix::LAPACK ){
		return kmb::MatrixLapack::multiply( &a, &b, this );
	}
	return false;
}


#endif

