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


#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifndef NULL
 #define NULL 0
#endif

namespace kmb{

class Matrix
{
public:
	enum matrixType{
		UNKNOWNTYPE,
		DOUBLE_ARRAY,
		LAPACK,
	};
public:
	Matrix(int rowSize, int colSize){};
	virtual ~Matrix(void){};
	virtual matrixType getType(void) const { return kmb::Matrix::UNKNOWNTYPE; }
	virtual int getColSize(void) const = 0;
	virtual int getRowSize(void) const = 0;
	virtual double get(int i,int j) const = 0;
	virtual bool set(int i,int j,double val) = 0;


	virtual bool multiply_left( const kmb::Matrix &a, kmb::Matrix &x ) const;

	virtual bool multiply_right( const kmb::Matrix &a, kmb::Matrix &x ) const;

	virtual bool multiply_substitute( const kmb::Matrix &a, const kmb::Matrix &b );


	virtual bool row_exchange(int i0,int i1){
		int colSize = getColSize();
		for(int j=0;j<colSize;j++){
			double t = get(i0,j);
			set(i0,j,get(i1,j));
			set(i1,j,t);
		}
		return true;
	}


	virtual bool row_transf(int i0,int i1,double r){
		int colSize = getColSize();
		for(int j=0;j<colSize;j++){
			set(i1,j,get(i1,j)+r*get(i0,j));
		}
		return true;
	}


	virtual bool row_multi(int i0,double r){
		int colSize = getColSize();
		for(int j=0;j<colSize;j++){
			set(i0,j,r*get(i0,j));
		}
		return true;
	}


	virtual bool column_exchange(int j0,int j1){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;i++){
			double t = get(i,j0);
			set(i,j0,get(i,j1));
			set(i,j1,t);
		}
		return true;
	}


	virtual bool column_transf(int j0,int j1,double r){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;i++){
			set(i,j1,get(i,j1)+r*get(i,j0));
		}
		return true;
	}


	virtual bool column_multi(int j0,double r){
		int rowSize = getRowSize();
		for(int i=0;i<rowSize;i++){
			set(i,j0,r*get(i,j0));
		}
		return true;
	}
};

class SquareMatrix : public Matrix
{
public:
	SquareMatrix(int size) : Matrix(size,size){};
	virtual ~SquareMatrix(void){};
	virtual int getSize(void) const = 0;
	virtual int getColSize(void) const{ return getSize(); };
	virtual int getRowSize(void) const{ return getSize(); };
	virtual bool identity(void);
	virtual bool zero(void);
	virtual bool transpose(void);
	virtual double determinant(void) const;
	virtual double trace(void) const;
};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
