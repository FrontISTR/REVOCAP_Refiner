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
#include <cfloat>
#include <cmath>
#include "Shape/kmbBSpline.h"

double kmb::BSpline::precision = 1.0e-10;

kmb::BSpline::BSpline(void)
{
}

kmb::BSpline::~BSpline(void)
{
}

void
kmb::BSpline::clear(void)
{
	knots.clear();
}

bool
kmb::BSpline::appendKnot(double k)
{
	if( knots.size() == 0 ){
		knots.push_back(k);
		return true;
	}else if( knots.back() <= k ){
		knots.push_back(k);
		return true;
	}else{
		return false;
	}
}

double
kmb::BSpline::getKnot(int index) const
{
	return knots[index];
}


void
kmb::BSpline::setKnots(int num,double* k)
{
	knots.clear();
	double tmp=0.0;
	for(int i=0;i<num;++i){
		if( i == 0 ){
			tmp = k[i];
			knots.push_back( tmp );
		}else if( tmp <= k[i] ){
			tmp = k[i];
			knots.push_back( tmp );
		}else{
			return;
		}
	}
}

int
kmb::BSpline::getKnotCount(void) const
{
	return static_cast<int>( knots.size() );
}

bool
kmb::BSpline::isDomain(double t) const
{
	return ( knots.front() <= t && t <= knots.back() );
}

void
kmb::BSpline::getDomain(double &min_t,double &max_t) const
{
	min_t = knots.front();
	max_t = knots.back();
}

void
kmb::BSpline::getDomainOnFrame( double origin, double unit, double &min_t,double &max_t) const
{
	if( fabs(unit) < kmb::BSpline::precision ){
		min_t = -DBL_MAX;
		max_t = DBL_MAX;
	}else if( unit > 0.0 ){
		max_t = (knots.back() - origin) / unit;
		min_t = (knots.front() - origin) / unit;
	}else{
		max_t = (knots.front() - origin) / unit;
		min_t = (knots.back() - origin) / unit;
	}
}




int
kmb::BSpline::getInterval(double t) const
{
	int index = -1;
	int i = -1;
	double org = -DBL_MAX;
	std::vector< double >::const_iterator kIter = knots.begin();
	while( kIter != knots.end() ){
		if( *kIter < t ){
			index = i+1;
		}else if( org <= t && org < *kIter ){
			index = i;
		}else if( t < *kIter ){
			break;
		}
		org = *kIter;
		++i;
		++kIter;
	}
	return index;
}

double
kmb::BSpline::getValue(int index,int degree,double u) const
{
	if( getKnotCount() <= 1 + index + degree || degree < 0 || index < 0 ){
		return 0.0;
	}
	int i = getInterval(u);
	if( i < 0 || index + degree < i ){
		return 0.0;
	}
	if( degree == 0 ){
		if( index == i ){
			return 1.0;
		}else{
			return 0.0;
		}
	}else{
		double retVal =  0.0;

		double* n = new double[degree+1];


		for(int d=0;d<degree+1;++d){
			n[d] = (index+d==i)? 1.0 : 0.0;
		}

		for(int d=1;d<degree+1;++d){
			for(int j=0;j<degree+1-d;++j){
				double left = knots[index+j+d] - knots[index+j];
				double right = knots[index+j+d+1] - knots[index+j+1];
				double tmp = 0.0;
				if( left != 0.0 ){
					tmp += (u-knots[index+j]) / left * n[j];
				}
				if( right != 0.0 ){
					tmp += (knots[index+j+d+1]-u) / right * n[j+1];
				}
				n[j] = tmp;
			}
		}
		retVal = n[0];
		delete[] n;
		return retVal;
	}
}

double
kmb::BSpline::getDerivative(int index,int degree,double u) const
{
	if( getKnotCount() <= 1 + index + degree || degree < 0 || index < 0 ){
		return 0.0;
	}
	int i = getInterval(u);
	if( i < 0 || index + degree < i ){
		return 0.0;
	}
	if( degree <= 0 ){
		return 0.0;
	}else{
		double retVal =  0.0;

		double* n = new double[degree+1];


		for(int d=0;d<degree+1;++d){
			n[d] = (index+d==i)? 1.0 : 0.0;
		}

		for(int d=1;d<degree;++d){
			for(int j=0;j<degree+1-d;++j){
				double left = knots[index+j+d] - knots[index+j];
				double right = knots[index+j+d+1] - knots[index+j+1];
				double tmp = 0.0;
				if( left != 0.0 ){
					tmp += (u-knots[index+j]) / left * n[j];
				}
				if( right != 0.0 ){
					tmp += (knots[index+j+d+1]-u) / right * n[j+1];
				}
				n[j] = tmp;
			}
		}
		retVal = 0.0;
		double left = knots[index+degree] - knots[index];
		double right = knots[index+degree+1] - knots[index+1];
		if( left != 0.0 ){
			retVal += n[0] / left;
		}
		if( right != 0.0 ){
			retVal -= n[1] / right;
		}
		retVal *= degree;
		delete[] n;
		return retVal;
	}
}

double
kmb::BSpline::getSecondDerivative(int index,int degree,double u) const
{
	if( getKnotCount() <= 1 + index + degree || degree < 0 || index < 0 ){
		return 0.0;
	}
	int i = getInterval(u);
	if( i < 0 || index + degree < i ){
		return 0.0;
	}
	if( degree <= 1 ){
		return 0.0;
	}else{
		double retVal =  0.0;

		double* n = new double[degree+1];


		for(int d=0;d<degree+1;++d){
			n[d] = (index+d==i)? 1.0 : 0.0;
		}

		for(int d=1;d<degree;++d){
			for(int j=0;j<degree+1-d;++j){
				double left = knots[index+j+d] - knots[index+j];
				double right = knots[index+j+d+1] - knots[index+j+1];
				double tmp = 0.0;
				if( left != 0.0 ){
					tmp += (u-knots[index+j]) / left * n[j];
				}
				if( right != 0.0 ){
					tmp += (knots[index+j+d+1]-u) / right * n[j+1];
				}
				n[j] = tmp;
			}
		}
		retVal = 0.0;
		double d0  = (knots[index+degree] - knots[index])*(knots[index+degree-1] - knots[index]);
		double d11 = (knots[index+degree] - knots[index+1])*(knots[index+degree] - knots[index]);
		double d12 = (knots[index+degree] - knots[index+1])*(knots[index+degree+1] - knots[index+1]);
		double d2  = (knots[index+degree+1] - knots[index+1])*(knots[index+degree+1] - knots[index+2]);
		if( d0 != 0.0 ){
			retVal += n[0] / d0;
		}
		if( d11 != 0.0 ){
			retVal -= n[1] / d11;
		}
		if( d12 != 0.0 ){
			retVal -= n[1] / d12;
		}
		if( d2 != 0.0 ){
			retVal += n[2] / d2;
		}
		retVal *= (degree*(degree-1));
		delete[] n;
		return retVal;
	}
}
