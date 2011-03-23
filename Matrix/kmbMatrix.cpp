/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Matrix                                                  #
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

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbMatrix_DoubleArray.h"

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
kmb::Matrix::rowProduct(int i, const kmb::ColumnVector &vec)
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
kmb::Matrix::colProduct(int i, const kmb::RowVector &vec)
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
kmb::Matrix::getRowVector(int i,kmb::RowVector& row)
{
	int colSize = getColSize();
	for(int j=0;j<colSize;++j){
		row.setColumn(j,get(i,j));
	}
	return true;
}

bool
kmb::Matrix::getColumnVector(int j,kmb::ColumnVector& col)
{
	int rowSize = getRowSize();
	for(int i=0;i<rowSize;++i){
		col.setRow(i,get(i,j));
	}
	return true;
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
	return mSize;
}

int
kmb::SquareMatrix::getRowSize(void) const
{
	return mSize;
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
		cIndices[i] = i+1;
	}
	double sum = 0.0;
	int sign = 1;
	for(int i=0;i<size;++i){
		for(int j=0;j<size-1;++j){
			rIndices[j] = (j<i) ? j : j+1;
		}
		sum += sign * get( 0, i ) * minorDeterminant(size-1,cIndices,rIndices);
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
	for(int i=0;i<size;++i){
		x[i] = 0.0;
		for(int j=0;j<size;++j){
			x[i] += a[j] * get(j,i);
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
			x[0] = ( get(1,1)*b[0] - get(1,0)*b[1]) / d;
			x[1] = (-get(0,1)*b[0] + get(0,0)*b[1]) / d;
			return true;
		}else{
			return false;
		}
	}else if(size == 3 ){

		double coef[12] = {
			get(0,0),get(1,0),get(2,0),
			get(0,1),get(1,1),get(2,1),
			get(0,2),get(1,2),get(2,2),
			b[0],b[1],b[2]
		};

		kmb::Matrix_DoubleArray mat(3,4,coef);


		if( mat.get(0,0) == 0.0 ){
			if( mat.get(1,0) != 0.0 ){
				mat.row_exchange(0,1);
			}else if( mat.get(2,0) != 0.0 ){
				mat.row_exchange(0,2);
			}else{
				return false;
			}
		}
		mat.row_multi(0, 1.0/mat.get(0,0));
		mat.row_transf(0, 1, -mat.get(1,0));
		mat.row_transf(0, 2, -mat.get(2,0));


		if( mat.get(1,1) == 0.0 ){
			if( mat.get(2,1) != 0.0 ){
				mat.row_exchange(1,2);
			}else{
				return false;
			}
		}
		mat.row_multi(1, 1.0/mat.get(1,1));
		mat.row_transf(1, 2, -mat.get(2,1));
		mat.row_transf(1, 0, -mat.get(0,1));


		if( mat.get(2,2) == 0.0 ){
			return false;
		}
		mat.row_multi(2, 1.0/mat.get(2,2));
		mat.row_transf(2, 0, -mat.get(0,2));
		mat.row_transf(2, 1, -mat.get(1,2));

		x[0] = mat.get(0,3);
		x[1] = mat.get(1,3);
		x[2] = mat.get(2,3);
		return true;
	}
	return false;
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
			x[0] = ( get(1,1)*b[0] - get(1,0)*b[1]) / d;
			x[1] = (-get(0,1)*b[0] + get(0,0)*b[1]) / d;
			return true;
		}else{
			return false;
		}
	}else if(size == 3 ){

		double coef[12] = {
			get(0,0),get(1,0),get(2,0),
			get(0,1),get(1,1),get(2,1),
			get(0,2),get(1,2),get(2,2),
			b[0],b[1],b[2]
		};

		kmb::Matrix_DoubleArray mat(3,4,coef);


		if( mat.get(0,0) == 0.0 ){
			if( mat.get(1,0) != 0.0 ){
				mat.row_exchange(0,1);
			}else if( mat.get(2,0) != 0.0 ){
				mat.row_exchange(0,2);
			}else{
				return false;
			}
		}
		mat.row_multi(0, 1.0/mat.get(0,0));
		mat.row_transf(0, 1, -mat.get(1,0));
		mat.row_transf(0, 2, -mat.get(2,0));


		if( mat.get(1,1) == 0.0 ){
			if( mat.get(2,1) != 0.0 ){
				mat.row_exchange(1,2);
			}else{
				return false;
			}
		}
		mat.row_multi(1, 1.0/mat.get(1,1));
		mat.row_transf(1, 2, -mat.get(2,1));
		mat.row_transf(1, 0, -mat.get(0,1));


		if( mat.get(2,2) == 0.0 ){
			return false;
		}
		mat.row_multi(2, 1.0/mat.get(2,2));
		mat.row_transf(2, 0, -mat.get(0,2));
		mat.row_transf(2, 1, -mat.get(1,2));

		x[0] = mat.get(0,3);
		x[1] = mat.get(1,3);
		x[2] = mat.get(2,3);
		return true;
	}
	return false;
}

double
kmb::SquareMatrix::minorDeterminant(int msize,int cIndices[],int rIndices[]) const
{
	if( msize == 1 ){
		return get( cIndices[0], rIndices[0] );
	}else if( msize == 2 ){
		return
			+ get( cIndices[0], rIndices[0] ) * get( cIndices[1], rIndices[1] )
			- get( cIndices[0], rIndices[1] ) * get( cIndices[0], rIndices[1] );
	}else if( msize == 3 ){
		return
			+ get( cIndices[0], rIndices[0] ) * get( cIndices[1], rIndices[1] ) * get( cIndices[2], rIndices[2] )
			+ get( cIndices[0], rIndices[1] ) * get( cIndices[1], rIndices[2] ) * get( cIndices[2], rIndices[0] )
			+ get( cIndices[0], rIndices[2] ) * get( cIndices[1], rIndices[0] ) * get( cIndices[2], rIndices[1] )
			- get( cIndices[0], rIndices[0] ) * get( cIndices[1], rIndices[2] ) * get( cIndices[2], rIndices[1] )
			- get( cIndices[0], rIndices[2] ) * get( cIndices[1], rIndices[1] ) * get( cIndices[2], rIndices[0] )
			- get( cIndices[0], rIndices[1] ) * get( cIndices[1], rIndices[0] ) * get( cIndices[2], rIndices[2] );
	}else{
		int* rsubIndices = new int[msize-1];
		double sum = 0.0;
		int sign = 1;
		for(int i=0;i<msize;++i){
			for(int j=0;j<msize-1;++j){
				rsubIndices[j] = (j<i) ? rIndices[j] : rIndices[j+1];
			}
			sum += sign * get( cIndices[0], rIndices[i] ) * minorDeterminant(msize-1,&cIndices[1],rsubIndices);
			sign *= -1;
		}
		delete[] rsubIndices;
		return sum;
	}
}
