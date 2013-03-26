/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Geometry                                                #
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

namespace kmb{

class Permutation
{
private:
	int n;
	int k;
	int* seq;
public:
	Permutation(void);
	Permutation(const kmb::Permutation &other);
	virtual ~Permutation(void);
	bool initialize(int n,int k);
	int getN(void) const;
	int getK(void) const;

	bool nextPerm(int i);
	bool hasNext(void) const;
	int getPerm(int index) const;
	bool isFinished(void) const;
	Permutation& operator=(const Permutation& other);
	Permutation& operator++(void);
	Permutation operator++(int n);
};

}
