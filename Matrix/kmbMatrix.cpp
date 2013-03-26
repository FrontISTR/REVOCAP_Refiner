/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matrix                                                  #
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

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbMatrix_DoubleArray.h"
#include "Matrix/kmbDiagonalMatrix.h"
#include "Matrix/kmbTriangularMatrix.h"
#include "Common/kmbCalculator.h"

#include <cfloat>
#include <climits>

bool
kmb::Matrix::zero(void)
{
	int rSize = getRowSize();
	int cSize = getColSize();
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			set(i,j,0.0);
		}
	}
	return true;
}


bool
kmb::Matrix::multiply_left( const kmb::Matrix &a, kmb::Matrix &x ) const
{
	int rSize = x.getRowSize();
	int cSize = x.getColSize();
	int nSize = this->getColSize();
	if( nSize != a.getRowSize() ||
		cSize != a.getColSize() ||
		rSize != this->getRowSize() )
	{
		return false;
	}
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			double s = 0.0;
			for(int k=0;k<nSize;++k){
				s += this->get(i,k) * a.get(k,j);
			}
			x.set(i,j,s);
		}
	}
	return true;
}

#include <cstdio>


bool
kmb::Matrix::multiply_left_mask( const kmb::Matrix &a, kmb::Matrix &x, kmb::Matrix::MASK m ) const
{
	int rSize = x.getRowSize();
	int cSize = x.getColSize();
	int nSize = this->getColSize();
	if( nSize != a.getRowSize() ||
		cSize != a.getColSize() ||
		rSize != this->getRowSize() )
	{
		return false;
	}
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			double s = 0.0;
			for(int k=0;k<nSize;++k){
				if( m(i,k) ){
					s += this->get(i,k) * a.get(k,j);
				}
			}
			x.set(i,j,s);
		}
	}
	return true;
}

bool
kmb::Matrix::multiply_vect_left( const double* a, double* x) const
{
	if( a == NULL || x == NULL ){
		return false;
	}
	int rSize = this->getRowSize();
	int nSize = this->getColSize();
	for(int i=0;i<rSize;++i){
		x[i] = 0.0;
		for(int j=0;j<nSize;++j){
			x[i] += this->get(i,j) * a[j];
		}
	}
	return true;
}

bool
kmb::Matrix::multiply_vect_left_mask(const double* a, double* x, kmb::Matrix::MASK m ) const
{
	if( a == NULL || x == NULL ){
		return false;
	}
	int rSize = this->getRowSize();
	int nSize = this->getColSize();
	for(int i=0;i<rSize;++i){
		x[i] = 0.0;
		for(int j=0;j<nSize;++j){
			if( m(i,j) ){
				x[i] += this->get(i,j) * a[j];
			}
		}
	}
	return true;
}


bool
kmb::Matrix::multiply_right( const kmb::Matrix &a, kmb::Matrix &x ) const
{
	int rSize = x.getRowSize();
	int cSize = x.getColSize();
	int nSize = this->getRowSize();
	if( nSize != a.getColSize() ||
		cSize != this->getColSize() ||
		rSize != a.getRowSize() )
	{
		return false;
	}
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			double s = 0.0;
			for(int k=0;k<nSize;++k){
				s += a.get(i,k) * this->get(k,j);
			}
			x.set(i,j,s);
		}
	}
	return true;
}

bool
kmb::Matrix::multiply_vect_right( const double* a, double* x) const
{
	if( a == NULL || x == NULL ){
		return false;
	}
	int nSize = this->getRowSize();
	int cSize = this->getColSize();
	for(int j=0;j<cSize;++j){
		x[j] = 0.0;
		for(int i=0;i<nSize;++i){
			x[j] += a[i] * this->get(i,j);
		}
	}
	return true;
}

bool
kmb::Matrix::substitute( const kmb::Matrix &a )
{
	int rSize = a.getRowSize();
	int cSize = a.getColSize();
	if( cSize != this->getColSize() ||
		rSize != this->getRowSize() )
	{
		return false;
	}
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			this->set(i,j,a.get(i,j));
		}
	}
	return true;
}


bool
kmb::Matrix::product( const kmb::Matrix &a, const kmb::Matrix &b )
{
	if( a.getColSize() != b.getRowSize() ||
		b.getColSize() != this->getColSize() ||
		a.getRowSize() != this->getRowSize() )
	{
		return false;
	}
	int rSize = this->getRowSize();
	int cSize = this->getColSize();
	int mSize = a.getColSize();
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			double s = 0.0;
			for(int k=0;k<mSize;++k){
				s += a.get(i,k) * b.get(k,j);
			}
			this->set(i,j,s);
		}
	}
	return true;
}


double
kmb::Matrix::rowProduct(int i, const kmb::ColumnVector &vec) const
{
	int cSize = getColSize();
	if( cSize == vec.getSize() ){
		double s = 0.0;
		for(int j=0;j<cSize;++j){
			s += get(i,j) * vec[j];
		}
		return s;
	}else{
		return -DBL_MAX;
	}
}

double
kmb::Matrix::rowProduct_mask(int i, const kmb::ColumnVector &vec, kmb::Matrix::MASK m) const
{
	int cSize = getColSize();
	if( cSize == vec.getSize() ){
		double s = 0.0;
		for(int j=0;j<cSize;++j){
			if( m(i,j) ){
				s += get(i,j) * vec[j];
			}
		}
		return s;
	}else{
		return -DBL_MAX;
	}
}


double
kmb::Matrix::colProduct(int i, const kmb::RowVector &vec) const
{
	int rSize = getRowSize();
	if( rSize == vec.getSize() ){
		double s = 0.0;
		for(int j=0;j<rSize;++j){
			s += vec[j] * get(j,i);
		}
		return s;
	}else{
		return -DBL_MAX;
	}
}

bool
kmb::Matrix::getRowVector(int i,kmb::RowVector& row) const
{
	int colSize = getColSize();
	for(int j=0;j<colSize;++j){
		row.setColumn(j,get(i,j));
	}
	return true;
}

bool
kmb::Matrix::setRowVector(int i,const kmb::RowVector& row)
{
	int colSize = getColSize();
	for(int j=0;j<colSize;++j){
		set(i,j,row[j]);
	}
	return true;
}

bool
kmb::Matrix::getColumnVector(int j,kmb::ColumnVector& col) const
{
	int rowSize = getRowSize();
	for(int i=0;i<rowSize;++i){
		col.setRow(i,get(i,j));
	}
	return true;
}

bool
kmb::Matrix::setColumnVector(int j,const kmb::ColumnVector& col)
{
	int rowSize = getRowSize();
	for(int i=0;i<rowSize;++i){
		set(i,j,col[j]);
	}
	return true;
}


double
kmb::Matrix::norm_1(void) const
{
	int rowSize = getRowSize();
	int colSize = getColSize();
	kmb::Maximizer maxVal;
	for(int j=0;j<colSize;++j){
		double sum = 0.0;
		for(int i=0;i<rowSize;++i){
			sum += fabs( get(i,j) );
		}
		maxVal.update(sum);
	}
	return maxVal.getMax();
}


double
kmb::Matrix::norm_inf(void) const
{
	int rowSize = getRowSize();
	int colSize = getColSize();
	kmb::Maximizer maxVal;
	for(int i=0;i<rowSize;++i){
		double sum = 0.0;
		for(int j=0;j<colSize;++j){
			sum += fabs( get(i,j) );
		}
		maxVal.update(sum);
	}
	return maxVal.getMax();
}


double
kmb::Matrix::norm_f(void) const
{
	int rowSize = getRowSize();
	int colSize = getColSize();
	double sum=0.0;
	for(int i=0;i<rowSize;++i){
		for(int j=0;j<colSize;++j){
			double d = get(i,j);
			sum += d*d;
		}
	}
	return sqrt(sum);
}

double
kmb::Matrix::getMax(void) const
{
	kmb::Maximizer maxVal;
	int rowSize = getRowSize();
	int colSize = getColSize();
	for(int i=0;i<rowSize;++i){
		for(int j=0;j<colSize;++j){
			double v = get(i,j);
			maxVal.update(v);
		}
	}
	return maxVal.getMax();
}

double
kmb::Matrix::getMin(void) const
{
	kmb::Minimizer minVal;
	int rowSize = getRowSize();
	int colSize = getColSize();
	for(int i=0;i<rowSize;++i){
		for(int j=0;j<colSize;++j){
			double v = get(i,j);
			minVal.update(v);
		}
	}
	return minVal.getMin();
}

int
kmb::Matrix::countNonZero(void) const
{
	int count = 0;
	int rowSize = getRowSize();
	int colSize = getColSize();
	for(int i=0;i<rowSize;++i){
		for(int j=0;j<colSize;++j){
			double v = get(i,j);
			if( v != 0.0 ){
				++count;
			}
		}
	}
	return count;
}

int
kmb::Matrix::countNonZeroBlock(int unitBSize) const
{
	if( unitBSize <= 0 ||
		this->getRowSize() % unitBSize != 0 ||
		this->getColSize() % unitBSize != 0 )
	{
		return -1;
	}
	int count = 0;
	double d = 0.0;
	int colBSize = this->getColSize() / unitBSize;
	int rowBSize = this->getRowSize() / unitBSize;
	bool nonZeroFlag = false;
	for(int i=0;i<rowBSize;++i){
		for(int j=0;j<colBSize;++j){

			nonZeroFlag = false;
			for(int i0=0;i0<unitBSize;++i0){
				for(int j0=0;j0<unitBSize;++j0){
					d = this->get(i*unitBSize+i0,j*unitBSize+j0);
					if( d != 0.0 ){
						++count;
						nonZeroFlag = true;
						break;
					}
				}
				if( nonZeroFlag ){
					break;
				}
			}
		}
	}
	return count;
}

kmb::SquareMatrix::SquareMatrix(int size)
: Matrix(size,size)
, mSize(size)
{
}

kmb::SquareMatrix::~SquareMatrix(void)
{
}

int
kmb::SquareMatrix::getSize(void) const
{
	return mSize;
}

int
kmb::SquareMatrix::getColSize(void) const
{
	return getSize();
}

int
kmb::SquareMatrix::getRowSize(void) const
{
	return getSize();
}

bool
kmb::SquareMatrix::identity(void)
{
	int size = getSize();
	for(int i=0;i<size;++i){
		for(int j=0;j<size;++j){
			if(i==j){
				set(i,j,1.0);
			}else{
				set(i,j,0.0);
			}
		}
	}
	return true;
}

bool
kmb::SquareMatrix::zero(void)
{
	int size = getSize();
	for(int i=0;i<size;++i){
		for(int j=0;j<size;++j){
			set(i,j,0.0);
		}
	}
	return true;
}

bool
kmb::SquareMatrix::transpose(void)
{
	bool res = true;
	int size = getSize();
	for(int i=0;i<size;++i){
		for(int j=i+1;j<size;++j){
			double v = get(i,j);
			res &= set(i,j,get(j,i));
			res &= set(j,i,v);
		}
		if( res == false ){
			break;
		}
	}
	return res;
}

double
kmb::SquareMatrix::determinant(void) const
{
	int size = getSize();
	int* cIndices = new int[size-1];
	int* rIndices = new int[size-1];
	for(int i=0;i<size-1;++i){
		rIndices[i] = i+1;
	}
	double sum = 0.0;
	int sign = 1;
	for(int i=0;i<size;++i){

		for(int j=0;j<size-1;++j){
			cIndices[j] = (j<i) ? j : j+1;
		}
		sum += sign * get( 0, i ) * minorDeterminant(size-1,rIndices,cIndices);
		sign *= -1;
	}
	delete[] cIndices;
	delete[] rIndices;
	return sum;
}

double
kmb::SquareMatrix::trace(void) const
{
	double tr = 1.0;
	int size = getSize();
	for(int i=0;i<size;++i){
		tr += get(i,i);
	}
	return tr;
}

bool
kmb::SquareMatrix::multiply_vect_left(const double* a, double* x) const
{
	int size = getSize();
	for(int i=0;i<size;++i){
		x[i] = 0.0;
		for(int j=0;j<size;++j){
			x[i] += get(i,j) * a[j];
		}
	}
	return true;
}

bool
kmb::SquareMatrix::multiply_vect_right(const double* a, double* x) const
{
	int size = getSize();
	for(int j=0;j<size;++j){
		x[j] = 0.0;
		for(int i=0;i<size;++i){
			x[j] += a[i] * get(i,j);
		}
	}
	return true;
}


bool
kmb::SquareMatrix::solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const
{
	int size = getSize();
	if(size == 1){
		double d = get(0,0);
		if( d != 0.0 ){
			x[0] = b[0] / d;
			return true;
		}else{
			return false;
		}
	}else if(size == 2 ){
		double d = this->determinant();
		if( d != 0.0 ){
			x[0] = ( get(1,1)*b[0] - get(0,1)*b[1]) / d;
			x[1] = (-get(1,0)*b[0] + get(0,0)*b[1]) / d;
			return true;
		}else{
			return false;
		}
	}else{

		kmb::Matrix_DoubleArray mat(size,size+1);
		for(int j=0;j<size;++j){
			for(int i=0;i<size;++i){
				mat.set(i,j,get(i,j));
			}
		}
		for(int i=0;i<size;++i){
			mat.set(i,size,b[i]);
		}

		for(int j=0;j<size;++j){

			int ind = -1;
			double m = 0.0;
			for(int i=j;i<size;++i){
				double d = fabs(mat.get(i,j));
				if( d > m ){
					ind = i;
					m = d;
				}
			}
			if( m == 0.0 ){
				return false;
			}else if(ind != j){
				mat.row_exchange(j,ind);
			}
			mat.row_multi(j, 1.0/mat.get(j,j));
			for(int i=0;i<size;++i){
				if( i != j){
					mat.row_transf(j, i, -mat.get(i,j));
				}
			}
		}
		for(int i=0;i<size;++i){
			x[i] = mat.get(i,size);
		}
		return true;
	}
}

bool
kmb::SquareMatrix::solve(const double* b, double* x) const
{
	int size = getSize();
	if(size == 1){
		double d = get(0,0);
		if( d != 0.0 ){
			x[0] = b[0] / d;
			return true;
		}else{
			return false;
		}
	}else if(size == 2 ){
		double d = this->determinant();
		if( d != 0.0 ){
			x[0] = ( get(1,1)*b[0] - get(0,1)*b[1]) / d;
			x[1] = (-get(1,0)*b[0] + get(0,0)*b[1]) / d;
			return true;
		}else{
			return false;
		}
	}else{

		kmb::Matrix_DoubleArray mat(size,size+1);
		for(int j=0;j<size;++j){
			for(int i=0;i<size;++i){
				mat.set(i,j,get(i,j));
			}
		}
		for(int i=0;i<size;++i){
			mat.set(i,size,b[i]);
		}

		for(int j=0;j<size;++j){

			int ind = -1;
			double m = 0.0;
			for(int i=j;i<size;++i){
				double d = fabs(mat.get(i,j));
				if( d > m ){
					ind = i;
					m = d;
				}
			}
			if( m == 0.0 ){
				return false;
			}else if(ind != j){
				mat.row_exchange(j,ind);
			}
			mat.row_multi(j, 1.0/mat.get(j,j));
			for(int i=0;i<size;++i){
				if( i != j ){
					mat.row_transf(j, i, -mat.get(i,j));
				}
			}
		}
		for(int i=0;i<size;++i){
			x[i] = mat.get(i,size);
		}
		return true;
	}
}

double
kmb::SquareMatrix::minorDeterminant(int msize,int rIndices[],int cIndices[]) const
{
	if( msize == 1 ){
		return get( rIndices[0], cIndices[0] );
	}else if( msize == 2 ){
		return
			+ get( rIndices[0], cIndices[0] ) * get( rIndices[1], cIndices[1] )
			- get( rIndices[0], cIndices[1] ) * get( rIndices[1], cIndices[0] );
	}else if( msize == 3 ){
		return
			+ get( rIndices[0], cIndices[0] ) * get( rIndices[1], cIndices[1] ) * get( rIndices[2], cIndices[2] )
			+ get( rIndices[0], cIndices[1] ) * get( rIndices[1], cIndices[2] ) * get( rIndices[2], cIndices[0] )
			+ get( rIndices[0], cIndices[2] ) * get( rIndices[1], cIndices[0] ) * get( rIndices[2], cIndices[1] )
			- get( rIndices[0], cIndices[0] ) * get( rIndices[1], cIndices[2] ) * get( rIndices[2], cIndices[1] )
			- get( rIndices[0], cIndices[2] ) * get( rIndices[1], cIndices[1] ) * get( rIndices[2], cIndices[0] )
			- get( rIndices[0], cIndices[1] ) * get( rIndices[1], cIndices[0] ) * get( rIndices[2], cIndices[2] );
	}else{
		int* csubIndices = new int[msize-1];
		double sum = 0.0;
		int sign = 1;
		for(int i=0;i<msize;++i){
			for(int j=0;j<msize-1;++j){
				csubIndices[j] = (j<i) ? cIndices[j] : cIndices[j+1];
			}
			sum += sign * get( rIndices[0], cIndices[i] ) * minorDeterminant(msize-1,&rIndices[1],csubIndices);
			sign *= -1;
		}
		delete[] csubIndices;
		return sum;
	}
}

const kmb::LowerTriangularMatrix*
kmb::SquareMatrix::createLowerTriangular(void) const
{
	return new kmb::LowerTriangularMatrix_Wrapper(this);
}

const kmb::UpperTriangularMatrix*
kmb::SquareMatrix::createUpperTriangular(void) const
{
	return new kmb::UpperTriangularMatrix_Wrapper(this);
}

const kmb::DiagonalMatrix*
kmb::SquareMatrix::createDiagonal(void) const
{
	return new kmb::DiagonalMatrix_Wrapper(this);
}

double
kmb::SquareMatrix::diagonal_dominance(void) const
{
	kmb::Minimizer minimizer;
	for(int i=0;i<mSize;++i){
		double diagonal = fabs(get(i,i));
		double out = 0.0;
		for(int j=0;j<mSize;++j){
			if( i!= j ){
				out += fabs(get(i,j));
			}
		}
		if( out != 0.0 ){
			minimizer.update( diagonal / out );
		}
	}
	return minimizer.getMin();
}

kmb::TransposeMatrix_Wrapper::TransposeMatrix_Wrapper(const kmb::SquareMatrix* mtx)
: SquareMatrix(mtx->getSize())
, matrix(mtx)
{
}

kmb::TransposeMatrix_Wrapper::~TransposeMatrix_Wrapper(void)
{
}

const char*
kmb::TransposeMatrix_Wrapper::getContainerType(void) const
{
	return "transpose_wrapper";
}

int
kmb::TransposeMatrix_Wrapper::init(int rowSize, int colSize)
{
	return -1;
}

double
kmb::TransposeMatrix_Wrapper::get(int i,int j) const
{
	return matrix->get(j,i);
}

bool
kmb::TransposeMatrix_Wrapper::set(int i,int j,double val)
{
	return false;
}

bool
kmb::TransposeMatrix_Wrapper::add(int i,int j,double val)
{
	return false;
}

