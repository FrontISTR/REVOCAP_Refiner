/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Calculator                                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include "Common/kmbCalculator.h"
#include <cfloat>
#include <climits>

kmb::AverageCalculator::AverageCalculator(void)
: sum(0.0)
, counter(0)
{
	initialize();
}

kmb::AverageCalculator::~AverageCalculator(void)
{
}

void
kmb::AverageCalculator::initialize(void)
{
	this->sum = 0.0;
	this->counter = 0;
}

void
kmb::AverageCalculator::add(double value)
{
	this->sum += value;
	++(this->counter);
}

double
kmb::AverageCalculator::getAverage(void) const
{
	if( counter == 0 )
		return DBL_MAX;
	else
		return sum / static_cast<double>(counter);
}

kmb::Minimizer::Minimizer(void)
: minValue(DBL_MAX)
{
	initialize();
}

kmb::Minimizer::~Minimizer(void)
{
}

void
kmb::Minimizer::initialize(void)
{
	this->minValue = DBL_MAX;
}

bool
kmb::Minimizer::update(double value)
{
	if( value < this->minValue ){
		this->minValue = value;
		return true;
	}else{
		return false;
	}
}

double
kmb::Minimizer::getMin(double x,double y)
{
	return (x>y)? y : x;
}

double
kmb::Minimizer::getMin(double x,double y,double z)
{
	return (x>y)?
		( (y>z)? z : y ):
		( (x>z)? z : x );
}

kmb::Maximizer::Maximizer(void)
: maxValue(-DBL_MAX)
{
	initialize();
}

kmb::Maximizer::~Maximizer(void)
{
}

void
kmb::Maximizer::initialize(void)
{
	this->maxValue = -DBL_MAX;
}

bool
kmb::Maximizer::update(double value)
{
	if( value > this->maxValue ){
		this->maxValue = value;
		return true;
	}else{
		return false;
	}
}

double
kmb::Maximizer::getMax(double x,double y)
{
	return (x<y)? y : x;
}

double
kmb::Maximizer::getMax(double x,double y,double z)
{
	return (x<y)?
		( (y<z)? z : y ):
		( (x<z)? z : x );
}

kmb::Calculator::Calculator(void)
: maxValue(-DBL_MAX)
, minValue(DBL_MAX)
, sum(0.0)
, counter(0)
{
	initialize();
}

kmb::Calculator::~Calculator(void)
{
}

void
kmb::Calculator::initialize(void)
{
	this->maxValue = -DBL_MAX;
	this->minValue = DBL_MAX;
	this->sum = 0.0;
	this->counter = 0;
}

bool
kmb::Calculator::update(double value)
{
	++counter;
	if( this->minValue < value && value < this->maxValue ){
		this->sum += value;
		return false;
	}
	if( value > this->maxValue ){
		this->maxValue = value;
		this->sum += value;
	}
	if( value < this->minValue ){
		this->minValue = value;
		this->sum += value;
	}
	return true;
}

double
kmb::Calculator::getSum(void) const
{
	return sum;
}

double
kmb::Calculator::getAverage(void) const
{
	if( counter == 0 )
		return DBL_MAX;
	else
		return sum / counter;
}

double
kmb::Calculator::getMaxAbs(void) const
{
	return kmb::Maximizer::getMax( maxValue, -minValue );
}

kmb::IntegerCalculator::IntegerCalculator(void)
: maxValue(-LONG_MAX)
, minValue(LONG_MAX)
, sum(0L)
, counter(0)
{
	initialize();
}

kmb::IntegerCalculator::~IntegerCalculator(void)
{
}

void
kmb::IntegerCalculator::initialize(void)
{
	this->maxValue = -LONG_MAX;
	this->minValue = LONG_MAX;
	this->sum = 0L;
	this->counter = 0;
}

bool
kmb::IntegerCalculator::update(long value)
{
	++counter;
	if( this->minValue < value && value < this->maxValue ){
		this->sum += value;
		return false;
	}
	if( value > this->maxValue ){
		this->maxValue = value;
		this->sum += value;
	}
	if( value < this->minValue ){
		this->minValue = value;
		this->sum += value;
	}
	return true;
}

long
kmb::IntegerCalculator::getAverage(void) const
{
	if( counter == 0 )
		return LONG_MAX;
	else
		return sum / counter;
}

long
kmb::IntegerCalculator::getMax(long x,long y)
{
	return (x<y)? y : x;
}

long
kmb::IntegerCalculator::getMax(long x,long y,long z)
{
	return (x<y)?
		( (y<z)? z : y ):
		( (x<z)? z : x );
}

long
kmb::IntegerCalculator::getMin(long x,long y)
{
	return (x>y)? y : x;
}

long
kmb::IntegerCalculator::getMin(long x,long y,long z)
{
	return (x>y)?
		( (y>z)? z : y ):
		( (x>z)? z : x );
}

long
kmb::IntegerCalculator::getAve(long x,long y)
{
	return (x+y)/2;
}

long
kmb::IntegerCalculator::getAve(long x,long y,long z)
{
	return (x+y+z)/3;
}
