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
#pragma once

#include <cfloat>

namespace kmb{

class AverageCalculator
{
public:
	AverageCalculator(void);
	virtual ~AverageCalculator(void);
	void initialize(void);
	void add(double value);
	double getAverage(void) const;
protected:
	double sum;
	int counter;

};

class Minimizer
{
public:
	Minimizer(void);
	virtual ~Minimizer(void);
	double getMin(void) const{ return minValue; }
	void initialize(void);

	bool update(double value);
	static double getMin(double x,double y);
	static double getMin(double x,double y,double z);
protected:
	double minValue;
};

class Maximizer
{
public:
	Maximizer(void);
	virtual ~Maximizer(void);
	double getMax(void) const{ return maxValue; }
	void initialize(void);

	bool update(double value);
	static double getMax(double x,double y);
	static double getMax(double x,double y,double z);
protected:
	double maxValue;
};


template<typename T>
class MinMaxWithId
{
protected:
	double minValue;
	double maxValue;
	T minId;
	T maxId;
public:
	MinMaxWithId(void)
		: minValue(DBL_MAX), maxValue(-DBL_MAX), minId(-1), maxId(-1)
	{
	}
	virtual ~MinMaxWithId(void){};
	double getMin(void) const{ return minValue; }
	double getMax(void) const{ return maxValue; }
	T getMinId(void) const{ return minId; }
	T getMaxId(void) const{ return maxId; }
	void initialize(void)
	{
		minValue = DBL_MAX;
		maxValue = -DBL_MAX;
	}

	bool update(double value,T id)
	{
		bool flag = false;
		if( value > this->maxValue ){
			this->maxValue = value;
			this->maxId = id;
			flag = true;
		}
		if( value < this->minValue ){
			this->minValue = value;
			this->minId = id;
			flag = true;
		}
		return flag;
	}
};

class Calculator
{
protected:
	double maxValue;
	double minValue;
	double sum;
	int counter;
public:
	Calculator(void);
	virtual ~Calculator(void);
	double getMax(void) const{ return maxValue; }
	double getMin(void) const{ return minValue; }
	double getAverage(void) const;
	double getMaxAbs(void) const;
	double getSum(void) const;
	void initialize(void);

	bool update(double value);
};

class IntegerCalculator
{
protected:
	long maxValue;
	long minValue;
	long sum;
	int counter;
public:
	IntegerCalculator(void);
	virtual ~IntegerCalculator(void);
	long getMax(void) const{ return maxValue; }
	long getMin(void) const{ return minValue; }
	long getAverage(void) const;
	void initialize(void);

	bool	update(long value);
	static long getMax(long x,long y);
	static long getMax(long x,long y,long z);
	static long getMin(long x,long y);
	static long getMin(long x,long y,long z);
	static long getAve(long x,long y);
	static long getAve(long x,long y,long z);
};

}
