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


#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

#ifndef NULL
 #define NULL 0
#endif

namespace kmb{

class ColumnVector;
class RowVector;
class LowerTriangularMatrix;
class UpperTriangularMatrix;
class DiagonalMatrix;



class MatrixIndex{
public:
	int rowIndex;
	int colIndex;
	MatrixIndex(int i,int j)
	: rowIndex(i)
	, colIndex(j)
	{}
	bool operator<(const MatrixIndex &other)const{
		return (rowIndex < other.rowIndex) ||
			(rowIndex==other.rowIndex && colIndex < other.colIndex);
	}
};

class Matrix
{
public:

	typedef bool (*MASK)(int,int);
	static bool upper(int i,int j){
		return (i<j);
	}
	static bool lower(int i,int j){
		return (i>j);
	}
	static bool diagonal(int i,int j){
		return (i==j);
	}
public:
	Matrix(int rowSize, int colSize){};
	virtual ~Matrix(void){};
	virtual const char* getContainerType(void) const = 0;
	virtual int init(int rowSize, int colSize) = 0;
	virtual int getColSize(void) const = 0;
	virtual int getRowSize(void) const = 0;
	virtual double get(int i,int j) const = 0;
	virtual bool set(int i,int j,double val) = 0;
	virtual bool add(int i,int j,double val) = 0;
	virtual bool zero(void);


	virtual bool multiply_left( const kmb::Matrix &a, kmb::Matrix &x ) const;
	virtual bool multiply_left_mask( const kmb::Matrix &a, kmb::Matrix &x, kmb::Matrix::MASK m ) const;

	virtual bool multiply_right( const kmb::Matrix &a, kmb::Matrix &x ) const;



	virtual bool multiply_vect_left(const double* a, double* x) const;
	virtual bool multiply_vect_left_mask(const double* a, double* x, kmb::Matrix::MASK m ) const;

	virtual bool multiply_vect_right(const double* a, double* x) const;


	virtual bool substitute( const kmb::Matrix &a );

	virtual bool product( const kmb::Matrix &a, const kmb::Matrix &b );


	virtual double rowProduct(int i, const kmb::ColumnVector &vec) const;
	virtual double rowProduct_mask(int i, const kmb::ColumnVector &vec, kmb::Matrix::MASK m) const;

	virtual double colProduct(int i, const kmb::RowVector &vec) const;


	virtual bool row_exchange(int i0,int i1){
		int colSize = getColSize();
		for(int j=0;j<colSize;++j){
			double t = get(i0,j);
			if( !set(i0,j,get(i1,j)) || !set(i1,j,t) ){
				return false;
			}
		}
		return true;
	}


	virtual bool row_transf(int i0,int i1,double r){
		int colSize = getColSize();
		for(int j=0;j<colSize;++j){
			if( !set( i1, j, get(i1,j) + (r*get(i0,j)) ) ){
				return false;
			}
		}
		return true;
	}


	virtual bool row_multi(int i0,double r){
		int colSize = getColSize();
		for(int j=0;j<colSize;++j){
			if( !set( i0, j, r*get(i0,j) ) ){
				return false;
			}
		}
		return true;
	}


	virtual bool column_exchange(int j0,int j1){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;++i){
			double t = get(i,j0);
			if( !set(i,j0,get(i,j1)) || !set(i,j1,t) ){
				return false;
			}
		}
		return true;
	}


	virtual bool column_transf(int j0,int j1,double r){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;++i){
			if( !set(i,j1,get(i,j1)+r*get(i,j0)) ){
				return false;
			}
		}
		return true;
	}


	virtual bool column_multi(int j0,double r){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;++i){
			if( !set(i,j0,r*get(i,j0)) ){
				return false;
			}
		}
		return true;
	}


	virtual bool getRowVector(int i,kmb::RowVector& row) const;
	virtual bool setRowVector(int i,const kmb::RowVector& row);

	virtual bool getColumnVector(int j,kmb::ColumnVector& col) const;
	virtual bool setColumnVector(int j,const kmb::ColumnVector& col);


	virtual double norm_1(void) const;

	virtual double norm_inf(void) const;

	virtual double norm_f(void) const;


	virtual double getMax(void) const;
	virtual double getMin(void) const;


	virtual int countNonZero(void) const;
	virtual int countNonZeroBlock(int unitBSize) const;
};

class ColumnVector;

class SquareMatrix : public Matrix
{
protected:
	int mSize;
public:
	SquareMatrix(int size);
	virtual ~SquareMatrix(void);
	virtual int getSize(void) const;
	virtual int getColSize(void) const;
	virtual int getRowSize(void) const;
	virtual bool zero(void);
	virtual bool identity(void);
	virtual bool transpose(void);
	virtual double determinant(void) const;
	virtual double trace(void) const;
	double minorDeterminant(int msize,int cIndices[],int rIndices[]) const;



	virtual bool multiply_vect_left(const double* a, double* x) const;

	virtual bool multiply_vect_right(const double* a, double* x) const;



	template <typename T> double quadratic(const T x,const T y) const
	{
		double sum = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			for(int j=0;j<size;++j){
				sum += x[i] * get(i,j) * y[j];
			}
		}
		return sum;
	}



	virtual bool solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const;
	virtual bool solve(const double* b, double* x) const;


	virtual const kmb::LowerTriangularMatrix* createLowerTriangular(void) const;
	virtual const kmb::UpperTriangularMatrix* createUpperTriangular(void) const;
	virtual const kmb::DiagonalMatrix* createDiagonal(void) const;



	double diagonal_dominance(void) const;
};


class TransposeMatrix_Wrapper : public SquareMatrix
{
private:
	const kmb::SquareMatrix* matrix;
public:
	TransposeMatrix_Wrapper(const kmb::SquareMatrix* mtx);
	virtual ~TransposeMatrix_Wrapper(void);
	virtual const char* getContainerType(void) const;
	virtual int init(int rowSize, int colSize);
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
