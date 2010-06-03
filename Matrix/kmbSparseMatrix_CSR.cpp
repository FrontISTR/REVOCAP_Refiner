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


#include "Matrix/kmbSparseMatrix_CSR.h"

kmb::SparseMatrix_CSR::SparseMatrix_CSR(void)
: Matrix(0,0)
, rowSize(0)
, colSize(0)
, values(NULL)
, columns(NULL)
, rowIndices(NULL)
{
}

kmb::SparseMatrix_CSR::~SparseMatrix_CSR(void)
{
}

void
kmb::SparseMatrix_CSR::clear(void)
{
	rowSize = 0;
	colSize = 0;
	if( values ){
		delete[] values;
		values = NULL;
	}
	if( columns ){
		delete[] columns;
		columns = NULL;
	}
	if( rowIndices ){
		delete[] rowIndices;
		rowIndices = NULL;
	}
}

int
kmb::SparseMatrix_CSR::getColSize(void) const
{
	return colSize;
}

int
kmb::SparseMatrix_CSR::getRowSize(void) const
{
	return rowSize;
}

int
kmb::SparseMatrix_CSR::initialize( kmb::Matrix *matrix )
{
	if( matrix == NULL ){
		return 0;
	}
	clear();
	int count = 0;
	double d = 0.0;
	colSize = matrix->getColSize();
	rowSize = matrix->getRowSize();
	for(int i=0;i<rowSize;++i){
		for(int j=0;j<colSize;++j){
			if( matrix->get(i,j) != 0.0 ){
				++count;
			}
		}
	}
	values = new double[count];
	columns = new int[count];
	rowIndices = new int[rowSize+1];
	count = 0;
	for(int i=0;i<rowSize;++i){
		rowIndices[i] = count;
		for(int j=0;j<colSize;++j){
			d = matrix->get(i,j);
			if( d != 0.0 ){
				values[count] = d;
				columns[count] = j;
				++count;
			}
		}
	}
	rowIndices[rowSize] = count;
	return count;
}

double
kmb::SparseMatrix_CSR::get(int i,int j) const
{
	if( values && columns && rowIndices ){
		for( int k = rowIndices[i]; k < rowIndices[i+1]; ++k ){
			if( columns[k] == j ){
				return values[k];
			}
		}
	}
	return 0.0;
}

bool
kmb::SparseMatrix_CSR::set(int i,int j,double val)
{
	return false;
}
