/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : PermutationMatrix                                       #
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

#include "Matrix/kmbPermutationMatrix.h"
#include "Matrix/kmbVector.h"



kmb::PermutationMatrix::PermutationMatrix(int size)
: SquareMatrix(size)
, ary(NULL)
, size(0)
{
	this->ary = new int[size];
	this->size = size;

	for(int i=0;i<size;++i){
		this->ary[i] = i;
	}
}

kmb::PermutationMatrix::~PermutationMatrix(void)
{
	if( ary != NULL ){
		delete[] ary;
	}
}

int
kmb::PermutationMatrix::getSize(void) const
{
	return this->size;
}

double
kmb::PermutationMatrix::get(int i,int j) const
{
	if( ary[i] == j ){
		return 1.0;
	}else{
		return 0.0;
	}
}

bool
kmb::PermutationMatrix::set(int i,int j,double val)
{
	return false;
}

bool
kmb::PermutationMatrix::row_exchange(int i0,int i1)
{
	int k = ary[i0];
	ary[i0] = ary[i1];
	ary[i1] = k;
	return true;
}

bool
kmb::PermutationMatrix::column_exchange(int i0,int i1)
{
	for(int i=0;i<size;++i){
	  if( ary[i] == i0 ){
	    ary[i] = i1;
	  }else if( ary[i] == i1 ){
	    ary[i] = i0;
	  }
	}
	return true;
}

bool
kmb::PermutationMatrix::identity(void)
{
	for(int i=0;i<size;++i){
		this->ary[i] = i;
	}
	return true;
}

bool
kmb::PermutationMatrix::transpose(void)
{
	int *temp = new int[size];
	for(int i=0;i<size;++i){
		temp[i] = this->ary[i];
	}
	for(int i=0;i<size;++i){
		this->ary[ temp[i] ] = i;
	}
	delete[] temp;
	return true;
}

double
kmb::PermutationMatrix::determinant(void) const
{

	double d = 1.0;
	for(int i=0;i<size;++i){
		for(int j=i+1;j<size;++j){
			if( ary[i] > ary[j] ){
				d *= -1;
			}
		}
	}
	return d;
}

double
kmb::PermutationMatrix::trace(void) const
{
	double tr = 0.0;
	for(int i=0;i<size;++i){
		if( this->ary[i] == i){
			tr += 1.0;
		}
	}
	return tr;
}

bool
kmb::PermutationMatrix::solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const
{
	for(int i=0;i<size;++i){
		x.setRow( i, b.getRow( ary[i] ) );
	}
	return true;
}


double
kmb::PermutationMatrix::getMultiRight( const kmb::Matrix& a, int i,int j) const
{
	for(int k=0;k<size;++k){
		if( this->ary[k] == j){
			return a.get(i,k);
		}
	}
	return 0.0;
}


double
kmb::PermutationMatrix::getMultiLeft( const kmb::Matrix& a, int i,int j) const
{
	return a.get( ary[i], j );
}
