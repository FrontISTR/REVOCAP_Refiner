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

#pragma once

#include "Matrix/kmbMatrix.h"

namespace kmb{

class MatrixLapack : public Matrix
{
private:
	double *m;
	int rowSize;
	int colSize;
	char trans;
public:
	MatrixLapack(int rowSize, int colSize);
	MatrixLapack( const Matrix &mtrx );
	virtual ~MatrixLapack(void);
	virtual matrixType getType(void) const;
	virtual int getColSize(void) const;
	virtual int getRowSize(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);

	virtual bool multiply_left( const kmb::Matrix &a, kmb::Matrix &x ) const;

	virtual bool multiply_right( const kmb::Matrix &a, kmb::Matrix &x ) const;

	virtual bool multiply_substitute( const kmb::Matrix &a, const kmb::Matrix &b );

protected:

	static bool multiply( const kmb::Matrix* a, const kmb::Matrix* b, kmb::Matrix* c );
	char getTrans(void) const;
};

}
#endif

