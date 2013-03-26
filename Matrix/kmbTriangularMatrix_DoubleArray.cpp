/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TriangularMatrix_DoubleArray                            #
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
#include "Matrix/kmbTriangularMatrix_DoubleArray.h"

#include <algorithm>












kmb::UpperTriangularMatrix_DoubleArray::UpperTriangularMatrix_DoubleArray(int size)
: UpperTriangularMatrix(size)
{
	m = new double[size*(size+1)/2];
	std::fill(m,m+(size*(size+1)/2),0.0);
}

int
kmb::UpperTriangularMatrix_DoubleArray::init(int rowSize, int colSize)
{
	if( colSize == rowSize && rowSize > 0 ){
		if( rowSize != this->mSize || m ==NULL ){
			clear();
			this->mSize = rowSize;
			m = new double[mSize*(mSize+1)/2];
		}
		std::fill(m,m+(mSize*(mSize+1)/2),0.0);
		return 0;
	}else{
		return -1;
	}
}

const char*
kmb::UpperTriangularMatrix_DoubleArray::getContainerType(void) const
{
	return "double_array_upper_triangle";
}

double
kmb::UpperTriangularMatrix_DoubleArray::get(int i,int j) const
{
	if( i <= j ){
		return m[j*(j+1)/2 + i];
	}else{
		return 0.0;
	}
}

bool
kmb::UpperTriangularMatrix_DoubleArray::set(int i,int j,double val)
{
	if( i <= j ){
		m[j*(j+1)/2 + i] = val;
		return true;
	}else{
		return false;
	}
}

bool
kmb::UpperTriangularMatrix_DoubleArray::add(int i,int j,double val)
{
	if( i <= j ){
		m[j*(j+1)/2 + i] += val;
		return true;
	}else{
		return false;
	}
}

void
kmb::UpperTriangularMatrix_DoubleArray::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}












kmb::LowerTriangularMatrix_DoubleArray::LowerTriangularMatrix_DoubleArray(int size)
: LowerTriangularMatrix(size)
{
	m = new double[size*(size+1)/2];
	std::fill(m,m+(size*(size+1)/2),0.0);
}

int
kmb::LowerTriangularMatrix_DoubleArray::init(int rowSize, int colSize)
{
	if( colSize == rowSize && rowSize > 0 ){
		if( rowSize != this->mSize || m ==NULL ){
			clear();
			this->mSize = rowSize;
			m = new double[mSize*(mSize+1)/2];
		}
		std::fill(m,m+(mSize*(mSize+1)/2),0.0);
		return 0;
	}else{
		return -1;
	}
}

const char*
kmb::LowerTriangularMatrix_DoubleArray::getContainerType(void) const
{
	return "double_array_lower_triangle";
}

double
kmb::LowerTriangularMatrix_DoubleArray::get(int i,int j) const
{
	if( j <= i ){
		return m[i*(i+1)/2 + j];
	}else{
		return 0.0;
	}
}

bool
kmb::LowerTriangularMatrix_DoubleArray::set(int i,int j,double val)
{
	if( j <= i ){
		m[i*(i+1)/2 + j] = val;
		return true;
	}else{
		return false;
	}
}

bool
kmb::LowerTriangularMatrix_DoubleArray::add(int i,int j,double val)
{
	if( j <= i ){
		m[i*(i+1)/2 + j] += val;
		return true;
	}else{
		return false;
	}
}

void
kmb::LowerTriangularMatrix_DoubleArray::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
}
