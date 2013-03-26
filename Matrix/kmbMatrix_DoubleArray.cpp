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
#include "Matrix/kmbMatrix_DoubleArray.h"

#include <algorithm>

kmb::Matrix_DoubleArray::Matrix_DoubleArray(int rSize, int cSize)
: Matrix(rSize,cSize)
, m(NULL)
, rowSize(rSize)
, colSize(cSize)
{
	m = new double[rowSize*colSize];
	std::fill(m,m+(rowSize*colSize),0.0);
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

kmb::Matrix_DoubleArray::Matrix_DoubleArray(const kmb::Matrix& mat)
: Matrix(mat.getRowSize(),mat.getColSize())
, m(NULL)
, rowSize(mat.getRowSize())
, colSize(mat.getColSize())
{
	m = new double[rowSize*colSize];
	for(int j=0;j<colSize;++j){
		for(int i=0;i<rowSize;++i){
			m[i+j*rowSize] = mat.get(i,j);
		}
	}
}

kmb::Matrix_DoubleArray::~Matrix_DoubleArray(void)
{
	clear();
}

void
kmb::Matrix_DoubleArray::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}

int
kmb::Matrix_DoubleArray::init(int rowSize, int colSize)
{
	if( rowSize > 0 && colSize > 0 ){
		if( this->rowSize != rowSize || this->colSize != colSize || m ==NULL ){
			clear();
			this->rowSize = rowSize;
			this->colSize = colSize;
			m = new double[rowSize*colSize];
		}
		std::fill(m,m+(rowSize*colSize),0.0);
		return 0;
	}else{
		return -1;
	}
}

const char*
kmb::Matrix_DoubleArray::getContainerType(void) const
{
	return "double_array";
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
kmb::Matrix_DoubleArray::add(int i,int j,double val)
{
	m[i+j*rowSize] += val;
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
{
	m = new double[size*size];
	std::fill(m,m+(size*size),0.0);
}

kmb::SquareMatrix_DoubleArray::SquareMatrix_DoubleArray(int size,double* ary)
: SquareMatrix(size)
, m(NULL)
{
	m = new double[size*size];
	for(int i=0;i<size*size;++i){
		m[i] = ary[i];
	}
}

kmb::SquareMatrix_DoubleArray::SquareMatrix_DoubleArray(const kmb::SquareMatrix& mat)
: SquareMatrix(mat.getSize())
, m(NULL)
{
	m = new double[mSize*mSize];
	for(int j=0;j<mSize;++j){
		for(int i=0;i<mSize;++i){
			m[i+j*mSize] = mat.get(i,j);
		}
	}
}

kmb::SquareMatrix_DoubleArray::~SquareMatrix_DoubleArray(void)
{
	clear();
}

void
kmb::SquareMatrix_DoubleArray::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}

int
kmb::SquareMatrix_DoubleArray::init(int rowSize, int colSize)
{
	if( colSize == rowSize && rowSize > 0 ){
		if( rowSize != this->mSize || m == NULL ){
			clear();
			this->mSize = rowSize;
			m = new double[mSize*mSize];
		}
		std::fill(m,m+(rowSize*colSize),0.0);
		return 0;
	}else{
		return -1;
	}
}

const char*
kmb::SquareMatrix_DoubleArray::getContainerType(void) const
{
	return "double_array_square";
}

double
kmb::SquareMatrix_DoubleArray::get(int i,int j) const
{
	return m[i+j*mSize];
}

bool
kmb::SquareMatrix_DoubleArray::set(int i,int j,double val)
{
	m[i+j*mSize] = val;
	return true;
}

bool
kmb::SquareMatrix_DoubleArray::add(int i,int j,double val)
{
	m[i+j*mSize] += val;
	return true;
}



kmb::ColumnVector_DoubleArray::ColumnVector_DoubleArray(int size)
: ColumnVector(size)
, m(NULL)
, size(size)
{
	m = new double[size];
	std::fill(m,m+size,0.0);
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

kmb::ColumnVector_DoubleArray::ColumnVector_DoubleArray(const kmb::ColumnVector &vec)
: ColumnVector(vec.getSize())
, m(NULL)
, size(vec.getSize())
{
	m = new double[size];
	for(int i=0;i<size;++i){
		m[i] = vec[i];
	}
}

kmb::ColumnVector_DoubleArray::~ColumnVector_DoubleArray(void)
{
	clear();
}

void
kmb::ColumnVector_DoubleArray::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}

int
kmb::ColumnVector_DoubleArray::init(int rowSize, int colSize)
{
	if( colSize == 1 && rowSize > 0 ){
		if( rowSize != size || m == NULL ){
			clear();
			this->size = rowSize;
			m = new double[size];
		}
		std::fill(m,m+size,0.0);
		return 0;
	}else{
		return -1;
	}
}

const char*
kmb::ColumnVector_DoubleArray::getContainerType(void) const
{
	return "double_array_column";
}

int
kmb::ColumnVector_DoubleArray::getSize(void) const
{
	return size;
}

int
kmb::ColumnVector_DoubleArray::getRowSize(void) const
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

void
kmb::ColumnVector_DoubleArray::addRow(int i,double val)
{
	m[i] += val;
}

double& kmb::ColumnVector_DoubleArray::operator()(int i,int j){
	return m[i];
}

const double& kmb::ColumnVector_DoubleArray::operator()(int i,int j) const{
	return m[i];
}

kmb::ColumnVector_DoubleArray&
kmb::ColumnVector_DoubleArray::operator=(const ColumnVector_DoubleArray& other)
{
	for(int i=0;i<size;++i){
		m[i] = other[i];
	}
	return *this;
}

kmb::RowVector_DoubleArray::RowVector_DoubleArray(int size)
: RowVector(size)
, m(NULL)
, size(size)
{
	m = new double[size];
	std::fill(m,m+size,0.0);
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

kmb::RowVector_DoubleArray::RowVector_DoubleArray(const kmb::RowVector &vec)
: RowVector(vec.getSize())
, m(NULL)
, size(vec.getSize())
{
	m = new double[size];
	for(int i=0;i<size;++i){
		m[i] = vec[i];
	}
}

kmb::RowVector_DoubleArray::~RowVector_DoubleArray(void)
{
	clear();
}

void
kmb::RowVector_DoubleArray::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}

int
kmb::RowVector_DoubleArray::init(int rowSize, int colSize)
{
	if( rowSize == 1 && colSize > 0 ){
		if( colSize != size || m == NULL ){
			clear();
			this->size = colSize;
			m = new double[size];
		}
		std::fill(m,m+size,0.0);
		return 0;
	}else{
		return -1;
	}
}

const char*
kmb::RowVector_DoubleArray::getContainerType(void) const
{
	return "double_array_row";
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

void
kmb::RowVector_DoubleArray::addColumn(int i,double val)
{
	m[i] += val;
}

double& kmb::RowVector_DoubleArray::operator()(int i,int j){
	return m[j];
}

const double& kmb::RowVector_DoubleArray::operator()(int i,int j) const{
	return m[j];
}

kmb::RowVector_DoubleArray&
kmb::RowVector_DoubleArray::operator=(const RowVector_DoubleArray& other)
{
	for(int i=0;i<size;++i){
		m[i] = other[i];
	}
	return *this;
}
