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




#pragma once

#include "Matrix/kmbMatrix.h"

namespace kmb{

class ColumnVector;

class PermutationMatrix : public SquareMatrix
{
private:
	int *ary;
	int size;
public:
	PermutationMatrix(int size);
	virtual ~PermutationMatrix(void);
	virtual int getSize(void) const;
	virtual bool identity(void);
	virtual bool transpose(void);
	virtual double determinant(void) const;
	virtual double trace(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);

	virtual bool row_exchange(int i0,int i1);

	virtual bool column_exchange(int i0,int i1);
	virtual bool solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const;


	double getMultiRight( const kmb::Matrix& a, int i,int j) const;


	double getMultiLeft( const kmb::Matrix& a, int i,int j) const;
};

}

