/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Optimization                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/**
 *
 * ����`�֐��̍œK�����s�����߂̃A���S���Y���W
 * static ���\�b�h�Ƃ��Ď�������
 *
 */
#pragma once

#include <cfloat>
#include "Geometry/kmb_Common.h"

namespace kmb{

class Matrix;
class ColumnVector;
class Region;



class OptTargetSS_0{
public:
	virtual ~OptTargetSS_0(void){};
	virtual double f(double t) = 0;
};



class OptTargetSV_0{
public:
	virtual ~OptTargetSV_0(void){};
	virtual int getDomainDim(void) const = 0;
	virtual double f(const double* t) = 0;
};



class OptTargetVV_0{
public:
	virtual ~OptTargetVV_0(void){};
	virtual int getDomainDim(void) const = 0;
	virtual int getRangeDim(void) const = 0;
	virtual bool f(const ColumnVector &t,ColumnVector &val) = 0;
};


class OptTargetSS : public OptTargetSS_0{
public:
	virtual ~OptTargetSS(void){};
	virtual double df(double t) = 0;
};


class OptTargetSV : public OptTargetSV_0{
public:
	virtual ~OptTargetSV(void){};
	virtual bool df(const double* t,double* deriv) = 0;
};


class OptTargetVV : public OptTargetVV_0{
public:
	virtual ~OptTargetVV(void){};
	virtual bool df(const ColumnVector &t,Matrix &jac) = 0;
};

class Optimization
{
public:
	static int maxIter;
	static double epsilon;
	typedef double (*FuncSS)(double);
private:
	int iterCount;
	double residual;
public:
	Optimization(void) : iterCount(0), residual(DBL_MAX){}
	~Optimization(void){}
	void initialize(void);
	int getIterCount(void);
	double getResidual(void);





	double calcMinOnGrid( OptTargetSS_0 &obj, double& opt_t, const double min_t, const double max_t, const int div );

	bool calcMinOnGrid( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t, const int* div );

	bool calcMinOnGridWithRegion( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t, const int* div, const kmb::Region *region );

	bool calcMinOnCell( OptTargetSV_0 &obj, double* optmin_t, double* optmax_t, const double* min_t, const double* max_t, const int* div );



	double calcZero_NR( FuncSS f, FuncSS df, double min_t, double max_t, double init_t=-DBL_MAX );
	double calcZero_NR( OptTargetSS &obj, double min_t, double max_t, double init_t=-DBL_MAX );
	bool calcZero_NR( OptTargetVV &obj, double* opt_t, const double* min_t, const double* max_t, const double* init_t=NULL );


	double calcZero_DN( OptTargetSS &obj, double init_t );
	bool calcZero_DN( OptTargetVV &obj, double* opt_t, const double* init_t );



	double calcZero_secant( OptTargetSS_0 &obj, double init_t0, double init_t1 );



	double calcMin_GS( FuncSS f, double min_t, double max_t );
	double calcMin_GS( OptTargetSS_0 &obj, double min_t, double max_t );
	bool calcMin_GS( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t );


	static bool getMin_CG( OptTargetSV &obj, double* opt_t, const double* min_t, const double* max_t, const double* init_t=NULL );
	bool calcMin_CG( OptTargetSV &obj, double* opt_t, const double* min_t, const double* max_t, const double* init_t=NULL );


	static bool getMin_BFGS( OptTargetSV &obj, double* opt_t, double* init_t );
	bool calcMin_BFGS( OptTargetSV &obj, double* opt_t, double* init_t );
private:

	static const double tau;
};

}