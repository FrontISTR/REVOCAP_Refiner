/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matrix_DoubleArrayR_Dist                                #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/12/22     #
#                                                                      #
----------------------------------------------------------------------*/
#if defined(REVOCAP_SUPPORT_MPI)

#pragma once

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"

namespace kmb{






class Matrix_DoubleArrayRDist : public Matrix{
private:
	double *m;
	int rowSize;
	int colSize;
	int paraSize;
	int localRank;
	int localRowSize;
	int localInitRow;
public:
	Matrix_DoubleArrayRDist(int _rowSize, int _colSize);
	virtual ~Matrix_DoubleArrayRDist(void);
	virtual int init(int _rowSize, int _colSize);
	virtual const char* getContainerType(void) const;
	virtual int getColSize(void) const;
	virtual int getRowSize(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);


	virtual bool row_exchange(int i0,int i1);

	virtual bool row_transf(int i0,int i1,double r);

	virtual bool row_multi(int i0,double r);


	virtual bool column_exchange(int j0,int j1);

	virtual bool column_transf(int j0,int j1,double r);

	virtual bool column_multi(int j0,double r);

	int getLocalRowSize(void) const;
	int getLocalRank(void) const;
protected:
	void clear(void);


};

class SquareMatrix_DoubleArrayRDist : public SquareMatrix{
private:
	double *m;
	int paraSize;
	int localRank;
	int localRowSize;
	int localInitRow;
public:
	SquareMatrix_DoubleArrayRDist(int _mSize);
	virtual ~SquareMatrix_DoubleArrayRDist(void);
	virtual int init(int _rowSize, int _colSize);
	virtual const char* getContainerType(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);


	virtual bool row_exchange(int i0,int i1);

	virtual bool row_transf(int i0,int i1,double r);

	virtual bool row_multi(int i0,double r);


	virtual bool column_exchange(int j0,int j1);

	virtual bool column_transf(int j0,int j1,double r);

	virtual bool column_multi(int j0,double r);

	int getLocalRowSize(void) const;
	int getLocalRank(void) const;
protected:
	void clear(void);

};

}

#endif
