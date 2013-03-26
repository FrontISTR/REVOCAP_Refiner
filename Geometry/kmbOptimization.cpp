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
#include "Geometry/kmbOptimization.h"
#include "Common/kmbCalculator.h"
#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Matrix/kmbMatrix_DoubleArray.h"
#include "Geometry/kmbRegion.h"
#include <cmath>
#include <cfloat>

const double kmb::Optimization::tau = 0.381966011250105;

kmb::Optimization::Optimization(void)
: iterCount(0), residual(DBL_MAX), diff(DBL_MAX), iterMax(1000), epsilon(1.0e-8)
{
}

void
kmb::Optimization::initialize(void)
{
	this->iterCount = 0;
	this->residual = DBL_MAX;
	this->diff = DBL_MAX;
}

int
kmb::Optimization::getIterCount(void) const
{
	return iterCount;
}

double
kmb::Optimization::getResidual(void) const
{
	return residual;
}

double
kmb::Optimization::getDiff(void) const
{
	return diff;
}

void
kmb::Optimization::setIterMax(int max)
{
	this->iterMax = max;
}

void
kmb::Optimization::setEpsilon(double e)
{
	this->epsilon = e;
}

double
kmb::Optimization::calcMinOnGrid( OptTargetSS_0 &obj, double& opt_t, const double min_t, const double max_t, const int div )
{
	kmb::Minimizer minimizer;
	double t = min_t;
	for(int i0=0;i0<=div;++i0){
		t = ( (div-i0) * min_t + i0 * max_t ) / div;
		if( minimizer.update( obj.f( t ) ) ){
			opt_t = t;
		}
	}
	return minimizer.getMin();
}



bool
kmb::Optimization::calcMinOnGrid( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t, const int* div )
{
	if( opt_t == NULL || min_t == NULL || max_t == NULL || div == NULL ){
		return false;
	}
	int n = obj.getDomainDim();
	kmb::Minimizer minimizer;
	double* t = new double[n];
	if( n == 2 ){
		for(int i0=0;i0<=div[0];++i0){
			t[0] = ( (div[0]-i0) * min_t[0] + i0 * max_t[0] ) / div[0];
			for(int i1=0;i1<=div[1];++i1){
				t[1] = ( (div[1]-i1) * min_t[1] + i1 * max_t[1] ) / div[1];
				if( minimizer.update( obj.f( t ) ) ){
					opt_t[0] = t[0];
					opt_t[1] = t[1];
				}
			}
		}
	}else if( n == 3 ){
		for(int i0=0;i0<=div[0];++i0){
			t[0] = ( (div[0]-i0) * min_t[0] + i0 * max_t[0] ) / div[0];
			for(int i1=0;i1<=div[1];++i1){
				t[1] = ( (div[1]-i1) * min_t[1] + i1 * max_t[1] ) / div[1];
				for(int i2=0;i2<=div[2];++i2){
					t[2] = ( (div[2]-i2) * min_t[2] + i2 * max_t[2] ) / div[2];
					if( minimizer.update( obj.f( t ) ) ){
						opt_t[0] = t[0];
						opt_t[1] = t[1];
						opt_t[2] = t[2];
					}
				}
			}
		}
	}
	delete[] t;
	return true;
}

bool
kmb::Optimization::calcMinOnGridWithRegion( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t, const int* div, const kmb::Region *region )
{
	if( opt_t == NULL || min_t == NULL || max_t == NULL || div == NULL || region == NULL ){
		return false;
	}
	int n = obj.getDomainDim();
	kmb::Minimizer minimizer;
	if( n == 3 ){
		kmb::Point3D pt;
		double t[3];
		for(int i0=0;i0<=div[0];++i0){
			t[0] = ( (div[0]-i0) * min_t[0] + i0 * max_t[0] ) / div[0];
			for(int i1=0;i1<=div[1];++i1){
				t[1] = ( (div[1]-i1) * min_t[1] + i1 * max_t[1] ) / div[1];
				for(int i2=0;i2<=div[2];++i2){
					t[2] = ( (div[2]-i2) * min_t[2] + i2 * max_t[2] ) / div[2];
					pt.setCoordinate(t[0],t[1],t[2]);
					if( region->intersect(pt) == kmb::Region::INSIDE && minimizer.update( obj.f( t ) ) ){
						opt_t[0] = t[0];
						opt_t[1] = t[1];
						opt_t[2] = t[2];
					}
				}
			}
		}
	}
	return true;
}



bool
kmb::Optimization::calcMinOnCell( OptTargetSV_0 &obj, double* optmin_t, double* optmax_t, const double* min_t, const double* max_t, const int* div )
{
	if( optmin_t == NULL || optmax_t == NULL || min_t == NULL || max_t == NULL || div == NULL ){
		return false;
	}
	int n = obj.getDomainDim();
	kmb::Minimizer minimizer;
	double* t = new double[n];
	if( n == 2 ){
		for(int i0=0;i0<div[0];++i0){
			t[0] = ( (div[0]-i0-0.5) * min_t[0] + (i0+0.5) * max_t[0] ) / div[0];
			for(int i1=0;i1<div[1];++i1){
				t[1] = ( (div[1]-i1-0.5) * min_t[1] + (i1+0.5) * max_t[1] ) / div[1];
				if( minimizer.update( obj.f( t ) ) ){
					optmin_t[0] = ( (div[0]-i0) * min_t[0] + i0 * max_t[0] ) / div[0];
					optmin_t[1] = ( (div[1]-i1) * min_t[1] + i1 * max_t[1] ) / div[1];
					optmax_t[0] = ( (div[0]-i0-1) * min_t[0] + (i0+1) * max_t[0] ) / div[0];
					optmax_t[1] = ( (div[1]-i1-1) * min_t[1] + (i1+1) * max_t[1] ) / div[1];
				}
			}
		}
	}
	delete[] t;
	return true;
}

double
kmb::Optimization::calcZero_NR( FuncSS f, FuncSS df, double min_t, double max_t, double init_t )
{
	if( init_t < min_t || init_t > max_t ){
		init_t = 0.5*(min_t + max_t);
	}
	double t0 = init_t;
	double t = init_t;
	double q = 0.0;
	iterCount = 0;
	while( iterCount < iterMax ){
		q = df(t0);
		if( fabs(q) < epsilon ){
			break;
		}
		residual = f(t0);
		t = t0 - residual / q;
		if( fabs(residual) < epsilon ){
			break;
		}
		if( t < min_t || t > max_t ){
			t = 0.5*(t+t0);
		}
		t0 = t;
		++iterCount;
	}
	if( iterCount < iterMax ){
		return t;
	}else{
		return DBL_MAX;
	}
}

double
kmb::Optimization::calcZero_NR( OptTargetSS &obj, double min_t, double max_t, double init_t )
{
	if( init_t < min_t || init_t > max_t ){
		init_t = 0.5*(min_t + max_t);
	}
	double t0 = init_t;
	double t = init_t;
	double q = 0.0;
	iterCount = 0;
	while( iterCount < iterMax ){
		q = obj.df(t0);
		if( fabs(q) < epsilon ){
			break;
		}
		residual = obj.f(t0);
		t = t0 - residual / q;
		if( fabs(residual) < kmb::Optimization::epsilon ){
			break;
		}
		if( t < min_t || t > max_t ){
			t = 0.5*(t+t0);
		}
		t0 = t;
		++iterCount;
	}
	if( iterCount < iterMax ){
		return t;
	}else{
		return DBL_MAX;
	}
}

bool
kmb::Optimization::calcZero_NR( OptTargetVV &obj, double* opt_t, const double* min_t, const double* max_t, const double* init_t )
{
	if( obj.getDomainDim() != obj.getRangeDim() ){
		return false;
	}
	int dim = obj.getDomainDim();
	kmb::ColumnVector_DoubleArray t(dim);
	kmb::ColumnVector_DoubleArray s(dim);
	kmb::ColumnVector_DoubleArray t0(dim);
	kmb::ColumnVector_DoubleArray q(dim);
	kmb::SquareMatrix_DoubleArray h(dim);
	if( init_t == NULL ){
		for(int i=0;i<dim;++i){
			t0[i] = 0.5*(min_t[i] + max_t[i]);
		}
	}else{
		for(int i=0;i<dim;++i){
			t0[i] = init_t[i];
		}
	}

	iterCount = 0;
	while( iterCount < iterMax ){
		if( !obj.f(t0,q) || !obj.df(t0,h) || fabs(h.determinant()) < epsilon ){
			break;
		}
		residual = q.normSq();
		if( residual < epsilon*epsilon ){
			break;
		}
		if( h.solve(q,s) ){
			t.setDifference(t0,s);
			for(int i=0;i<dim;++i){
				if( t.getRow(i) < min_t[i] || t.getRow(i) > max_t[i] ){
					t.setRow(i,0.5*(t.getRow(i)+t0.getRow(i)));
				}
			}
			t0.set(t);
		}
		++iterCount;
	}
	if( iterCount < iterMax ){
		for(int i=0;i<dim;++i){
			opt_t[i] = t0[i];
		}
		return true;
	}else{
		return false;
	}
}

double
kmb::Optimization::calcZero_DN( OptTargetSS &obj, double init_t )
{
	double t0 = init_t;
	double t = init_t;
	double r = 2.0;
	double d = 0.0;
	iterCount = 0;
	while( iterCount < iterMax ){
		r = 2.0;
		residual = obj.f(t0);
		if( fabs(residual) < epsilon ){
			return t0;
		}
		d = obj.df(t0);
		if( fabs(d) < epsilon ){

			break;
		}
		d = - residual / d;
		do{
			r *= 0.5;
			t = t0 + r * d;
		}while( fabs(obj.f(t)) > ( 1 - 0.5*r ) * fabs(residual) );
		t0 = t;
		++iterCount;
	}
	return DBL_MAX;
}

bool
kmb::Optimization::calcZero_DN( OptTargetVV &obj, double* opt_t, const double* init_t )
{
	if( opt_t == NULL || init_t == NULL ){
		return false;
	}
	int dim = obj.getDomainDim();
	kmb::ColumnVector_DoubleArray t(dim);
	kmb::ColumnVector_DoubleArray t0(dim);
	kmb::ColumnVector_DoubleArray y(dim);
	kmb::ColumnVector_DoubleArray y0(dim);
	kmb::ColumnVector_DoubleArray d(dim);
	kmb::SquareMatrix_DoubleArray j(dim);

	for(int i=0;i<dim;++i){
		t0[i] = init_t[i];
	}
	iterCount = 0;
	bool res = false;
	double r = 2.0;
	while( iterCount < iterMax ){
		r = 2.0;
		obj.f(t0,y0);
		residual = y0.normSq();
		if( residual < epsilon*epsilon ){
			res = true;
			for(int i=0;i<dim;++i){
				opt_t[i] = t0[i];
			}
			break;
		}
		obj.df(t0,j);
		if( !j.solve(y0,d) ){
			break;
		}
		do{
			r *= 0.5;
			for(int i=0;i<dim;++i){
				t[i] = t0[i] - r * d[i];
			}
			obj.f(t,y);
		}while( y.normSq() > ( 1 - 0.5*r ) * ( 1 - 0.5*r ) * residual );
		if( t0.distanceSq(t) < epsilon*epsilon ){

			break;
		}
		t0 = t;
		++iterCount;
	}
	return res;
}

double
kmb::Optimization::calcZero_secant( OptTargetSS_0 &obj, double init_t0, double init_t1 )
{
	double t0 = init_t0;
	double t1 = init_t1;
	double t2;

	double q0 = obj.f(t0);
	double q1 = obj.f(t1);
	iterCount = 0;
	while( iterCount < iterMax ){
		if( fabs(q1) < epsilon ){
			break;
		}
		t2 = t1 - q1 * (t1-t0) / (q1-q0);
		t0 = t1;
		t1 = t2;
		q0 = q1;
		q1 = obj.f(t1);
		residual = q1;
		if( fabs(residual) < epsilon ){
			break;
		}
		++iterCount;
	}
	if( iterCount < iterMax ){
		return t1;
	}else{
		return DBL_MAX;
	}
}

bool
kmb::Optimization::calcMin_BFGS( OptTargetSV &obj, double* opt_t, double* init_t )
{
	if( opt_t == NULL || init_t == NULL ){
		return false;
	}
	int dim = obj.getDomainDim();
	double* t0 = new double[dim];
	double* t  = new double[dim];
	double* d  = new double[dim];
	double alpha = 0.0;
	double* u  = new double[dim];
	kmb::SquareMatrix_DoubleArray b(dim);
	bool res = false;

	b.identity();
	for(int i=0;i<dim;++i){
		t0[i] = init_t[i];
	}
	iterCount = 0;
	while( iterCount < iterMax ){

		obj.df( t0, u );
		for(int i=0;i<dim;++i){
			u[i] *= -1.0;
		}
		if( !b.solve( u, d ) ){
			break;
		}


		alpha = 1.0 / 0.8;
		double v = - kmb::ColumnVector::inner( d, u, dim );
		double y = obj.f(t0);
		do{
			alpha *= 0.8;
			for(int i=0;i<dim;++i){
				t[i] = t0[i] + alpha * d[i];
			}
		}while( obj.f(t) > y + 0.5 * alpha * v && alpha > DBL_MIN );

		obj.df( t, d );
		residual = kmb::ColumnVector::normSq( d, dim );
		if( residual < epsilon*epsilon ){
			for(int i=0;i<dim;++i){
				opt_t[i] = t[i];
			}
			res = true;
			break;
		}

		for(int i=0;i<dim;++i){
			u[i] += d[i];
			d[i] = t[i] - t0[i];
			t0[i] = t[i];
		}




		b.multiply_vect_left(d,t);
		double q0 = b.quadratic<double*>(d,d);
		double q1 = kmb::ColumnVector::inner( d, u, dim );
		for(int i=0;i<dim;++i){
			for(int j=0;j<dim;++j){
				double c = b.get(i,j) - t[i]*t[j] / q0 + u[i]*u[j] / q1;
				b.set(i,j,c);
			}
		}
		++iterCount;
	}
	delete[] t0;
	delete[] t;
	delete[] d;
	delete[] u;
	return res;
}

double
kmb::Optimization::calcMin_GS( FuncSS f, double min_t, double max_t )
{
	double t0 = min_t;
	double t1 = max_t;
	iterCount = 0;
	diff = t1 - t0;
	while( iterCount < iterMax ){
		diff = t1 - t0;
		if( diff < epsilon ){
			break;
		}
		double s0 = (1.0-tau) * t0 + tau * t1;
		double s1 = tau * t0 + (1.0-tau) * t1;
		double v0 = f(s0);
		double v1 = f(s1);
		if( v0 < v1 ){
			t1 = s1;
		}else{
			t0 = s0;
		}
		++iterCount;
	}
	if( iterCount < iterMax ){
		return 0.5*(t0 + t1);
	}else{
		return DBL_MAX;
	}
}

double
kmb::Optimization::calcMin_GS( OptTargetSS_0 &obj, double min_t, double max_t )
{
	double t0 = min_t;
	double t1 = max_t;
	iterCount = 0;
	diff = t1 - t0;
	while( iterCount < iterMax ){
		diff = t1 - t0;
		if( diff < epsilon ){
			break;
		}
		double s0 = (1.0-tau) * t0 + tau * t1;
		double s1 = tau * t0 + (1.0-tau) * t1;
		double v0 = obj.f(s0);
		double v1 = obj.f(s1);
		if( v0 < v1 ){
			t1 = s1;
		}else{
			t0 = s0;
		}
		++iterCount;
	}
	if( iterCount < iterMax ){
		return 0.5*(t0 + t1);
	}else{
		return DBL_MAX;
	}
}


bool
kmb::Optimization::calcMin_GS( OptTargetSV_0 &obj, double* opt_t, const double* min_t, const double* max_t )
{
	if( opt_t == NULL || min_t == NULL || max_t == NULL ){
		return false;
	}
	int n = obj.getDomainDim();
	if( n > 3 ){
		return false;
	}
	int m = 1;
	double* t0 = new double[n];
	double* t1 = new double[n];
	for(int i=0;i<n;++i){
		t0[i] = min_t[i];
		t1[i] = max_t[i];
		m *= 2;
	}
	double* s0 = new double[n];
	double* s1 = new double[n];
	double* s = new double[n];
	kmb::Maximizer maximizer;
	iterCount = 0;
	while( iterCount < iterMax ){
		diff = 0.0;
		int maxIndex = -1;
		maximizer.initialize();
		for(int i=0;i<n;++i){
			diff += fabs(t0[i]-t1[i]);
		}
		if( diff < n*kmb::Optimization::epsilon ){
			break;
		}

		if( n == 2 ){
			for(int i=0;i<n;++i){
				s0[i] = (1.0-tau) * t0[i] + tau * t1[i];
				s1[i] = tau * t0[i] + (1.0-tau) * t1[i];
			}
			s[0] = s0[0]; s[1] = s0[1];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 0;
			}
			s[0] = s1[0]; s[1] = s0[1];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 1;
			}
			s[0] = s0[0]; s[1] = s1[1];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 2;
			}
			s[0] = s1[0]; s[1] = s1[1];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 3;
			}
			switch(maxIndex){
			case 0:
				t0[0] = s0[0]; t0[1] = s0[1];
				break;
			case 1:
				t0[1] = s0[1];
				t1[0] = s1[0];
				break;
			case 2:
				t0[0] = s0[0];
				t1[1] = s1[1];
				break;
			case 3:
				t1[0] = s1[0]; t1[1] = s1[1];
				break;
			default:
				break;
			}
		}else if( n == 3 ){
			for(int i=0;i<n;++i){
				s0[i] = (1.0-tau) * t0[i] + tau * t1[i];
				s1[i] = tau * t0[i] + (1.0-tau) * t1[i];
			}
			s[0] = s0[0]; s[1] = s0[1]; s[2] = s0[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 0;
			}
			s[0] = s1[0]; s[1] = s0[1]; s[2] = s0[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 1;
			}
			s[0] = s0[0]; s[1] = s1[1]; s[2] = s0[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 2;
			}
			s[0] = s1[0]; s[1] = s1[1]; s[2] = s0[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 3;
			}
			s[0] = s0[0]; s[1] = s0[1]; s[2] = s1[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 4;
			}
			s[0] = s1[0]; s[1] = s0[1]; s[2] = s1[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 5;
			}
			s[0] = s0[0]; s[1] = s1[1]; s[2] = s1[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 6;
			}
			s[0] = s1[0]; s[1] = s1[1]; s[2] = s1[2];
			if( maximizer.update( obj.f(s) ) ){
				maxIndex = 7;
			}
			switch(maxIndex){
			case 0:
				t0[0] = s0[0]; t0[1] = s0[1]; t0[2] = s0[2];
				break;
			case 1:
				t1[0] = s1[0]; t0[1] = s0[1]; t0[2] = s0[2];
				break;
			case 2:
				t0[0] = s0[0]; t1[1] = s1[1]; t0[2] = s0[2];
				break;
			case 3:
				t1[0] = s1[0]; t1[1] = s1[1]; t0[2] = s0[2];
				break;
			case 4:
				t0[0] = s0[0]; t0[1] = s0[1]; t1[2] = s1[2];
				break;
			case 5:
				t1[0] = s1[0]; t0[1] = s0[1]; t1[2] = s1[2];
				break;
			case 6:
				t0[0] = s0[0]; t1[1] = s1[1]; t1[2] = s1[2];
				break;
			case 7:
				t1[0] = s1[0]; t1[1] = s1[1]; t1[2] = s1[2];
				break;
			default:
				break;
			}
		}
		++iterCount;
	}
	delete[] s0;
	delete[] s1;
	delete[] s;
	if( iterCount < iterMax ){
		for(int i=0;i<n;++i){
			opt_t[i] = 0.5*(t0[i] + t1[i]);
		}
		delete[] t0;
		delete[] t1;
		return true;
	}else{
		delete[] t0;
		delete[] t1;
		return false;
	}
}

bool
kmb::Optimization::calcMin_CG( OptTargetSV &obj, double* opt_t, const double* min_t, const double* max_t, const double* init_t )
{
	if( opt_t == NULL || min_t == NULL || max_t == NULL ){
		return false;
	}
	bool res = false;
	int n = obj.getDomainDim();
	double* t = new double[n];
	double* grad = new double[n];
	double* grad_prev = new double[n];
	double* conj = new double[n];

	if( init_t != NULL ){
		for(int i=0;i<n;++i){
			t[i] = init_t[i];
		}
	}else{
		for(int i=0;i<n;++i){
			t[i] = 0.5*(min_t[i] + max_t[i]);
		}
	}

	class cg_local : public kmb::OptTargetSS_0 {
	private:
		int n;
		double* local_t;
	public:
		double* t;
		double* conj;
		OptTargetSV* org;
		cg_local(kmb::OptTargetSV& obj) : n(0), local_t(NULL), t(NULL), conj(NULL), org(NULL){
			n = obj.getDomainDim();
			org = &obj;
			local_t = new double[n];
		}
		~cg_local(void){
			delete[] local_t;
		}

		double f( double s ){
			for(int i=0;i<n;++i){
				local_t[i] = t[i] + s * conj[i];
			}
			return org->f( local_t );
		}
	};
	cg_local sub_obj(obj);
	sub_obj.t = t;
	sub_obj.conj = conj;
	double epsilonSq = epsilon * epsilon;
	iterCount = 0;
	double lenSq = 0.0;

	double beta = 0.0;
	double sub_opt = 0.0;
	double sub_min = 0.0;
	double sub_max = 0.0;
	kmb::Minimizer minimizer;
	kmb::Maximizer maximizer;
	while( iterCount < iterMax ){
		obj.df( t, grad );
		lenSq = 0.0;
		for(int i=0;i<n;++i){
			lenSq += grad[i] * grad[i];
		}
		diff = lenSq;
		if( diff < epsilonSq ){
			for(int i=0;i<n;++i){
				opt_t[i] = t[i];
			}
			break;
		}
		if( beta == 0.0 ){
			for(int i=0;i<n;++i){
				conj[i] = - grad[i];
			}
		}else{
			double temp = 0.0;
			for(int i=0;i<n;++i){
				temp += grad[i] * (grad[i] - grad_prev[i]);
			}
			beta *= temp;
			for(int i=0;i<n;++i){
				conj[i] = - grad[i] + beta * conj[i];
			}
		}
		beta = 1.0 / lenSq;


		minimizer.initialize();
		maximizer.initialize();
		for(int i=0;i<n;++i){
			if( conj[i] != 0.0 ){
				double s0 = (min_t[i] - t[i]) / conj[i];
				double s1 = (max_t[i] - t[i]) / conj[i];
				if( s0 < s1 ){
					maximizer.update( s0 );
					minimizer.update( s1 );
				}else{
					maximizer.update( s1 );
					minimizer.update( s0 );
				}
			}
		}
		sub_min = maximizer.getMax();
		sub_max = minimizer.getMin();
		kmb::Optimization sub;
		sub_opt = sub.calcMin_GS( sub_obj, sub_min, sub_max );
		for(int i=0;i<n;++i){
			t[i] += sub_opt * conj[i];
		}
		double* ptr = grad;
		grad = grad_prev;
		grad_prev = ptr;
		++iterCount;
	}
	if( iterCount < iterMax ){
		for(int i=0;i<n;++i){
			opt_t[i] = t[i];
		}
		res = true;
	}
	sub_obj.t = NULL;
	sub_obj.conj = NULL;
	sub_obj.org = NULL;
	delete[] conj;
	delete[] grad_prev;
	delete[] grad;
	delete[] t;
	return res;
}

