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
// 行列処理関数の基底クラス

#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100) // 使わない引数があっても警告を出さない for VC
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869) // 使わない引数があっても警告を出さない for intel
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

// 順序は Row Major
// i*colSize+j
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
	// 行列積を計算するときの成分のマスク条件
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

	// 自分を左から掛ける this * a = x
	virtual bool multiply_left( const kmb::Matrix &a, kmb::Matrix &x ) const;
	virtual bool multiply_left_mask( const kmb::Matrix &a, kmb::Matrix &x, kmb::Matrix::MASK m ) const;
	// 自分を右から掛ける a * this  = x
	virtual bool multiply_right( const kmb::Matrix &a, kmb::Matrix &x ) const;

	// ベクトル専用
	// 自分を左から掛ける this * a = x
	virtual bool multiply_vect_left(const double* a, double* x) const;
	virtual bool multiply_vect_left_mask(const double* a, double* x, kmb::Matrix::MASK m ) const;
	// 自分を右から掛ける a * this = x
	virtual bool multiply_vect_right(const double* a, double* x) const;

	// 代入 this = a
	virtual bool substitute( const kmb::Matrix &a );
	// 掛け算代入 this = a * b
	virtual bool product( const kmb::Matrix &a, const kmb::Matrix &b );
	
	// i 行と列ベクトルの内積
	virtual double rowProduct(int i, const kmb::ColumnVector &vec) const;
	virtual double rowProduct_mask(int i, const kmb::ColumnVector &vec, kmb::Matrix::MASK m) const;
	// i 列と行ベクトルの内積
	virtual double colProduct(int i, const kmb::RowVector &vec) const;

	// i0 行と i1 行の交換
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

	// i0 行を r 倍して i1 行に加える
	virtual bool row_transf(int i0,int i1,double r){
		int colSize = getColSize();
		for(int j=0;j<colSize;++j){
			if( !set( i1, j, get(i1,j) + (r*get(i0,j)) ) ){
				return false;
			}
		}
		return true;
	}

	// i0 行を r 倍
	virtual bool row_multi(int i0,double r){
		int colSize = getColSize();
		for(int j=0;j<colSize;++j){
			if( !set( i0, j, r*get(i0,j) ) ){
				return false;
			}
		}
		return true;
	}

	// j0 列と j1 列の交換
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

	// j0 列を r 倍して j1 列に加える
	virtual bool column_transf(int j0,int j1,double r){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;++i){
			if( !set(i,j1,get(i,j1)+r*get(i,j0)) ){
				return false;
			}
		}
		return true;
	}

	// j0 列を r 倍
	virtual bool column_multi(int j0,double r){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;++i){
			if( !set(i,j0,r*get(i,j0)) ){
				return false;
			}
		}
		return true;
	}

	// 行ベクトルの取得と代入
	virtual bool getRowVector(int i,kmb::RowVector& row) const;
	virtual bool setRowVector(int i,const kmb::RowVector& row);
	// 列ベクトルの取得と代入
	virtual bool getColumnVector(int j,kmb::ColumnVector& col) const;
	virtual bool setColumnVector(int j,const kmb::ColumnVector& col);

	// 列ごとに成分の絶対値の和を計算したうちでその最大のもの
	virtual double norm_1(void) const;
	// 行ごとに成分の絶対値の和を計算したうちでその最大のもの
	virtual double norm_inf(void) const;
	// 成分の２乗の和の平方根
	virtual double norm_f(void) const;

	// 成分の最大最小
	virtual double getMax(void) const;
	virtual double getMin(void) const;

	// 非零成分の個数
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

	// ベクトル専用
	// 自分を左から掛ける this * a = x
	virtual bool multiply_vect_left(const double* a, double* x) const;
	// 自分を右から掛ける a * this = x
	virtual bool multiply_vect_right(const double* a, double* x) const;

	// 2次形式
	// T = kmb::ColumnVector&, double*
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

	// this * x = b をガウス消去法で解く
	// コードは同じだけど、仮想関数なのでテンプレートが使えない
	virtual bool solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const;
	virtual bool solve(const double* b, double* x) const;

	// このクラスのインスタンスを参照して、下三角、上三角、対角成分だけを使うためのラッパの生成
	virtual const kmb::LowerTriangularMatrix* createLowerTriangular(void) const;
	virtual const kmb::UpperTriangularMatrix* createUpperTriangular(void) const;
	virtual const kmb::DiagonalMatrix* createDiagonal(void) const;

	// 対角優位であることとこの値が1より大きいことは同値
	// min |a_ii| / \sum_{i \neq j} |a_ij| 
	double diagonal_dominance(void) const;
};

// 転置行列
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
