/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : SparseMatrix_CSR                                        #
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

class SparseMatrix_CSR : public Matrix
{
private:
	int rowSize;
	int colSize;
	double *values;
	int *columns;
	int *rowIndices;
public:
	SparseMatrix_CSR(void);
	virtual ~SparseMatrix_CSR(void);


	int initialize( kmb::Matrix *matrix );
	virtual int getColSize(void) const;
	virtual int getRowSize(void) const;
	virtual double get(int i,int j) const;

	virtual bool set(int i,int j,double val);
protected:
	void clear(void);
};

}
