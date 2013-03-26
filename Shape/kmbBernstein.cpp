/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Bernstein                                               #
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

#include "Shape/kmbBernstein.h"

kmb::Bernstein::Bernstein(void)
{
}

kmb::Bernstein::~Bernstein(void)
{
}

int
kmb::Bernstein::getCombinatorial(int n,int k)
{
	int c = 1;
	for(int i=0;i<k;++i){
		c *= (n-i);
		c /= (i+1);
	}
	return c;
}

double
kmb::Bernstein::getValue(int m,int i,double t)
{
	double s = 1.0 - t;
	double u = 1.0;
	for(int j=0;j<i;++j){
		u *= t;
	}
	for(int j=i;j<m;++j){
		u *= s;
	}
	u *= getCombinatorial( m, i );
	return u;
}
