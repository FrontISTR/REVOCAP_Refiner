/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ColumnVector, RowVector                                 #
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
#pragma once

#include "Matrix/kmbMatrix.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

#include <cfloat>
#include <cmath>

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
	virtual void addRow(int i,double val) = 0;
	virtual double get(int i,int j) const{ return getRow(i); }
	virtual bool set(int i,int j,double v){ setRow(i,v); return true; }
	virtual bool add(int i,int j,double v){ addRow(i,v); return true; }
	virtual bool zero(void){
		int size = getSize();
		for(int i=0;i<size;++i){
			setRow(i,0.0);
		}
		return true;
	}
	virtual void scale(const double r){
		int size = getSize();
		for(int i=0;i<size;++i){
			setRow(i,r*getRow(i));
		}
	}


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


	virtual double& operator[](const int i) = 0;
	virtual double operator[](const int i) const = 0;


	ColumnVector& operator=(const ColumnVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			this->setRow(i,other.getRow(i));
		}
		return *this;
	}
	void set(const ColumnVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			(*this)[i] = other[i];
		}
	}

	bool setSum(const ColumnVector& v0,const ColumnVector& v1){
		int size = getSize();
		for(int i=0;i<size;++i){
			setRow(i,v0.getRow(i)+v1.getRow(i));
		}
		return true;
	}
	ColumnVector& operator+=(const ColumnVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			this->setRow(i,this->getRow(i)+other.getRow(i));
		}
		return *this;
	}
	bool add(const ColumnVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			this->setRow(i,getRow(i)+v.getRow(i));
		}
		return true;
	}

	bool setDifference(const ColumnVector& v0,const ColumnVector& v1){
		int size = getSize();
		for(int i=0;i<size;++i){
			setRow(i,v0.getRow(i)-v1.getRow(i));
		}
		return true;
	}
	ColumnVector& operator-=(const ColumnVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			this->setRow(i,this->getRow(i)-other.getRow(i));
		}
		return *this;
	}
	bool substract(const ColumnVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			this->setRow(i,getRow(i)-v.getRow(i));
		}
		return true;
	}

	bool setScalar(const double s,const ColumnVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			setRow(i,s*v.getRow(i));
		}
		return true;
	}

	bool addScalar(const double s,const ColumnVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			setRow(i,getRow(i)+s*v.getRow(i));
		}
		return true;
	}

	double distanceSq(const ColumnVector &other) const{
		double lenSq = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			lenSq += (this->getRow(i) - other.getRow(i))*(this->getRow(i) - other.getRow(i));
		}
		return lenSq;
	}

	double operator%(const kmb::ColumnVector &other) const{
		double sum = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			sum += this->getRow(i) * other.getRow(i);
		}
		return sum;
	}
	static double inner(const double* x,const double* y,int size){
		double sum = 0.0;
		for(int i=0;i<size;++i){
			sum += x[i] * y[i];
		}
		return sum;
	}

	double normSq(void) const{
		double lenSq = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			lenSq += this->getRow(i)*this->getRow(i);
		}
		return lenSq;
	}
	static double normSq(const double* x,int size){
		double sum = 0.0;
		for(int i=0;i<size;++i){
			sum += x[i] * x[i];
		}
		return sum;
	}

	double norm_1(void) const{
		double norm = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			norm += fabs(this->getRow(i));
		}
		return norm;
	}

	double norm_inf(void) const{
		double m = -DBL_MAX;
		int size = getSize();
		for(int i=0;i<size;++i){
			double d = fabs(this->getRow(i));
			if( m < d ){
				m = d;
			}
		}
		return m;
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
	virtual void addColumn(int i,double val) = 0;
	virtual double get(int i,int j) const{ return getColumn(j); }
	virtual bool set(int i,int j,double v){ setColumn(j,v); return true; }
	virtual bool add(int i,int j,double v){ addColumn(j,v); return true; }
	virtual bool zero(void){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,0.0);
		}
		return true;
	}
	virtual void scale(const double r){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,r*getColumn(i));
		}
	}


	virtual bool column_exchange(int i0,int i1){
		double t = getColumn(i0);
		setColumn(i0,getColumn(i1));
		setColumn(i1,t);
		return true;
	}


	virtual bool column_transf(int i0,int i1,double r){
		setColumn(i1,getColumn(i1)+r*getColumn(i0));
		return true;
	}


	virtual bool column_multi(int i0,double r){
		setColumn(i0,r*getColumn(i0));
		return true;
	}


	virtual double& operator[](const int i) = 0;
	virtual double operator[](const int i) const = 0;


	RowVector& operator=(const RowVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,other.getColumn(i));
		}
		return *this;
	}
	void set(const RowVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			(*this)[i] = other[i];
		}
	}

	bool setSum(const RowVector& v0,const RowVector& v1){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,v0.getColumn(i)+v1.getColumn(i));
		}
		return true;
	}
	RowVector& operator+=(const RowVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,this->getColumn(i)+other.getColumn(i));
		}
		return *this;
	}
	bool add(const RowVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			this->setColumn(i,getColumn(i)+v.getColumn(i));
		}
		return true;
	}

	bool setDifference(const RowVector& v0,const RowVector& v1){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,v0.getColumn(i)-v1.getColumn(i));
		}
		return true;
	}
	RowVector& operator-=(const RowVector& other){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,this->getColumn(i)-other.getColumn(i));
		}
		return *this;
	}
	bool substract(const RowVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			this->setColumn(i,getColumn(i)+v.getColumn(i));
		}
		return true;
	}

	bool setScalar(const double s,const RowVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,s*v.getColumn(i));
		}
		return true;
	}

	bool addScalar(const double s,const RowVector& v){
		int size = getSize();
		for(int i=0;i<size;++i){
			setColumn(i,getColumn(i)+s*v.getColumn(i));
		}
		return true;
	}

	double distanceSq(const RowVector &other) const{
		double lenSq = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			lenSq += (this->getColumn(i) - other.getColumn(i))*(this->getColumn(i) - other.getColumn(i));
		}
		return lenSq;
	}

	double operator%(const kmb::RowVector &other) const{
		double sum = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			sum += this->getColumn(i) * other.getColumn(i);
		}
		return sum;
	}
	static double inner(const double* x,const double* y,int size){
		double sum = 0.0;
		for(int i=0;i<size;++i){
			sum += x[i] * y[i];
		}
		return sum;
	}

	double normSq(void) const{
		double lenSq = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			lenSq += this->getColumn(i)*this->getColumn(i);
		}
		return lenSq;
	}
	static double normSq(const double* x,int size){
		double sum = 0.0;
		for(int i=0;i<size;++i){
			sum += x[i] * x[i];
		}
		return sum;
	}

	double norm_1(void) const{
		double norm = 0.0;
		int size = getSize();
		for(int i=0;i<size;++i){
			norm += fabs(this->getColumn(i));
		}
		return norm;
	}

	double norm_inf(void) const{
		double m = -DBL_MAX;
		int size = getSize();
		for(int i=0;i<size;++i){
			double d = fabs(this->getColumn(i));
			if( m < d ){
				m = d;
			}
		}
		return m;
	}
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
