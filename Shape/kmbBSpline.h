/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BSpline                                                 #
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
// B-Spline 基底関数の基礎知識
// 0 次は knots の区間の step 関数
// p 次は （区分的）p 次多項式
// knots を k_0,k_1,...,k_m-1 とする
// i 番目の 0 次基底関数は [k_i,k_i+1) を台に持つ
// i 番目の p 次基底関数は [k_i,k_i+p+1) を台に持つ
// p 次の基底関数の個数：m - (p+1)
// n 個の制御点から p 次の曲線を作るには n+p+1 個の knot が必要

#pragma once

#include <vector>

namespace kmb{

class BSpline
{
private:
	std::vector< double > knots;
	// 曲線パラメータの精度
	static double precision;
public:
	BSpline(void);
	~BSpline(void);
	void clear(void);
	bool appendKnot(double k);
	double getKnot(int index) const;
	void setKnots(int num,double* knots);
	int getKnotCount(void) const;
	// knotsCount = m のとき、index は 0,...,m-deg-2 まで
	double getValue(int index,int degree,double u) const;
	// knotsCount = m のとき、index は 0,...,m-deg-3 まで
	double getDerivative(int index,int degree,double u) const;
	// knotsCount = m のとき、index は 0,...,m-deg-4 まで
	double getSecondDerivative(int index,int degree,double u) const;
	// 定義域に含まれるかどうか = knots の最初と最後の間にあるかどうか
	bool isDomain(double t) const;
	void getDomain(double &min_t,double &max_t) const;
	// origin を原点に unit を単位とした座標での最大と最小を求める
	void getDomainOnFrame( double origin, double unit, double &min_t,double &max_t) const;
private:
	// t が knot の i 番目以上 i+1 番目より小さい
	// t < knots[0] の場合は -1
	int getInterval(double t) const;
};

}
