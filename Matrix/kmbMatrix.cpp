/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Matrix                                                  #
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

#include "Matrix/kmbMatrix.h"


bool
kmb::Matrix::multiply_left( const kmb::Matrix &a, kmb::Matrix &x ) const
{
	if( this->getColSize() != a.getRowSize() ||
		a.getColSize() != x.getColSize() ||
		this->getRowSize() != x.getRowSize() )
	{
		return false;
	}
	int rSize = x.getRowSize();
	int cSize = x.getColSize();
	int mSize = this->getColSize();
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			double s = 0.0;
			for(int k=0;k<mSize;++k){
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
	if( a.getColSize() != this->getRowSize() ||
		this->getColSize() != x.getColSize() ||
		a.getRowSize() != x.getRowSize() )
	{
		return false;
	}
	int rSize = x.getRowSize();
	int cSize = x.getColSize();
	int mSize = this->getRowSize();
	for(int i=0;i<rSize;++i){
		for(int j=0;j<cSize;++j){
			double s = 0.0;
			for(int k=0;k<mSize;++k){
				s += a.get(i,k) * this->get(k,j);
			}
			x.set(i,j,s);
		}
	}
	return true;
}


bool
kmb::Matrix::multiply_substitute( const kmb::Matrix &a, const kmb::Matrix &b )
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
	return 0.0;
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

