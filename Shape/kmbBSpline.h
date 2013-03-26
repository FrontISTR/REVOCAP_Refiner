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









#pragma once

#include <vector>

namespace kmb{

class BSpline
{
private:
	std::vector< double > knots;

	static double precision;
public:
	BSpline(void);
	~BSpline(void);
	void clear(void);
	bool appendKnot(double k);
	double getKnot(int index) const;
	void setKnots(int num,double* knots);
	int getKnotCount(void) const;

	double getValue(int index,int degree,double u) const;

	double getDerivative(int index,int degree,double u) const;

	double getSecondDerivative(int index,int degree,double u) const;

	bool isDomain(double t) const;
	void getDomain(double &min_t,double &max_t) const;

	void getDomainOnFrame( double origin, double unit, double &min_t,double &max_t) const;
private:


	int getInterval(double t) const;
};

}
