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
/**
 * Bernstein Šî’êŠÖ”‚ÌÀ‘•
 *
 * B_{m,i}(t) = m_C_i (1-t)^{m-i} t^i
 *
 * ”÷•ª‚Í
 * B_{m,i}(t)' = m{ B_{m-1,i-1}(t) - B_{m-1,i}(t) }
 * ‚Å—^‚¦‚ç‚ê‚éB
 *
 */
#pragma once

namespace kmb{

class Bernstein
{
public:
	Bernstein(void);
	virtual ~Bernstein(void);
	static double getValue(int m,int i,double t);
	static int getCombinatorial(int n,int k);
};

}
