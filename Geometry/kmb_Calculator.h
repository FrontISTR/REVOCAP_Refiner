/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Calculator                                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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

#include "Geometry/kmb_idTypes.h"

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

class MinMaxWithId
{
protected:
	double minValue;
	double maxValue;
	kmb::idType minId;
	kmb::idType maxId;
public:
	MinMaxWithId(void);
	virtual ~MinMaxWithId(void);
	double getMin(void) const{ return minValue; }
	double getMax(void) const{ return maxValue; }
	kmb::idType getMinId(void) const{ return minId; }
	kmb::idType getMaxId(void) const{ return maxId; }
	void initialize(void);

	bool update(double value,kmb::idType id);
};

class IntegerCalculator
{
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
protected:
	long maxValue;
	long minValue;
	long sum;
	int counter;
};

}
