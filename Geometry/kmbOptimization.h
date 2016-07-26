/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Optimization                                            #
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
/**
 *
 * 非線形関数の最適化を行うためのアルゴリズム集
 *
 */
#pragma once

#include "Common/kmbCommon.h"

#include <cfloat>

namespace kmb{

class Matrix;
class ColumnVector;
class Region;

// scalar => scalar
// 微分なし
class OptTargetSS_0{
public:
	virtual ~OptTargetSS_0(void){};
	virtual double f(double t) = 0;
};

// vector => scalar
// 微分なし
class OptTargetSV_0{
public:
	virtual ~OptTargetSV_0(void){};
	virtual int getDomainDim(void) const = 0;
	virtual double f(const double* t) = 0;
};

// vector => vector
// 微分なし
class OptTargetVV_0{
public:
	virtual ~OptTargetVV_0(void){};
	virtual int getDomainDim(void) const = 0;
	virtual int getRangeDim(void) const = 0;
	virtual bool f(const ColumnVector &t,ColumnVector &val) = 0;
};

// scalar => scalar
class OptTargetSS : public OptTargetSS_0{
public:
	virtual ~OptTargetSS(void){};
	virtual double df(double t) = 0;
};

// vector => scalar
class OptTargetSV : public OptTargetSV_0{
public:
	virtual ~OptTargetSV(void){};
	virtual bool df(const double* t,double* deriv) = 0;
};

// vector => vector
class OptTargetVV : public OptTargetVV_0{
public:
	virtual ~OptTargetVV(void){};
	virtual bool df(const ColumnVector &t,Matrix &jac) = 0;
};

class Optimization
{
public:
	typedef double (*FuncSS)(double);
private:
	int iterCount;
	// f(x) = 0 を求める場合に、f(x) の値
	// または f(x) の最小値を求めるときに df(x) の値
	double residual;
	// f(x) の最小値を求める場合に、x_n - x_(n-1)
	double diff;
	int iterMax;
	double epsilon;
public:
	Optimization(void);
	~Optimization(void){}
	void initialize(void);
	int getIterCount(void) const;
	double getResidual(void) const;
	double getDiff(void) const;
	void setIterMax(int max);
	void setEpsilon(double e);
	

	// min_t, max_t の領域を div で分割した grid の頂点で最小のところを探す
	// 戻り値は grid の頂点の座標
	// 1次元の区間上の等間隔の点の中から最小を探す
	double calcMinOnGrid( OptTargetSS_0 &obj, double& opt_t, const double min_t, const double max_t, const int div );
	// 2,3次元空間の格子状の点の中から最小を探す
	bool calcMinOnGrid( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t, const int* div );
	// 3次元空間の領域制限つきで最小を探す
	bool calcMinOnGridWithRegion( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t, const int* div, const kmb::Region *region );
	// min_t, max_t の領域を div で分割した grid の中心で最小のところを探す
	bool calcMinOnCell( OptTargetSV_0 &obj, double* optmin_t, double* optmax_t, const double* min_t, const double* max_t, const int* div );


	// Newton-Raphson 法
	double calcZero_NR( FuncSS f, FuncSS df, double min_t, double max_t, double init_t=-DBL_MAX );
	double calcZero_NR( OptTargetSS &obj, double min_t, double max_t, double init_t=-DBL_MAX );
	bool calcZero_NR( OptTargetVV &obj, double* opt_t, const double* min_t, const double* max_t, const double* init_t=NULL );

	// Damped Newton Method 減速ニュートン法
	double calcZero_DN( OptTargetSS &obj, double init_t );
	bool calcZero_DN( OptTargetVV &obj, double* opt_t, const double* init_t );
	
	// セカント法
	// init_t0 と init_t1 の間に解がある
	double calcZero_secant( OptTargetSS_0 &obj, double init_t0, double init_t1 );

	// Golden Section 黄金分割法
	// min_t と max_t の間に最小値が1つだけあること（単峰性）を仮定
	double calcMin_GS( FuncSS f, double min_t, double max_t );
	double calcMin_GS( OptTargetSS_0 &obj, double min_t, double max_t );
	bool calcMin_GS( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t );

	// Conjugate Gradient 共役勾配法
	bool calcMin_CG( OptTargetSV &obj, double* opt_t, const double* min_t, const double* max_t, const double* init_t=NULL );

	// 準ニュートン(BFGS)法
	bool calcMin_BFGS( OptTargetSV &obj, double* opt_t, double* init_t );
private:
	// 黄金比 rho = 1 + (1-tau)
	static const double tau;
};

}
