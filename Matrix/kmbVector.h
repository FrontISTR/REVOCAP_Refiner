/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ColumnVector, RowVector                                 #
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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

namespace kmb{

class ColumnVector : public Matrix
{
public:
	ColumnVector(int size) : kmb::Matrix(size, 1){};
	virtual ~ColumnVector(void){};
	virtual int getSize(void) const = 0;
	virtual int getColSize(void) const{ return 1; };
	virtual int getRowSize(void) const{ return getSize(); };
	virtual double getRow(int i) const = 0;
	virtual void setRow(int i,double val) = 0;
	virtual double get(int i,int j) const{ return getRow(i); }
	virtual bool set(int i,int j,double v){ setRow(i,v); return true; }

	virtual bool row_exchange(int i0,int i1){
		double t = getRow(i0);
		setRow(i0,getRow(i1));
		setRow(i1,t);
		return true;
	}


	virtual bool row_transf(int i0,int i1,double r){
		setRow(i1,getRow(i1)+r*getRow(i0));
		return true;
	}


	virtual bool row_multi(int i0,double r){
		setRow(i0,r*getRow(i0));
		return true;
	}
};

class RowVector : public Matrix
{
public:
	RowVector(int size) : kmb::Matrix(1, size){};
	virtual ~RowVector(void){};
	virtual int getSize(void) const = 0;
	virtual int getColSize(void) const{ return getSize(); };
	virtual int getRowSize(void) const{ return 1; };
	virtual double getColumn(int i) const = 0;
	virtual void setColumn(int i,double val) = 0;
	virtual double get(int i,int j) const{ return getColumn(i); }
	virtual bool set(int i,int j,double v){ setColumn(i,v); return true; }
};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
