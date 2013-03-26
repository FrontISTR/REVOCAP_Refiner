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
#include "Common/kmbPermutation.h"
#include "Common/kmbCommon.h"

kmb::Permutation::Permutation(void)
: n(0)
, k(0)
, seq(NULL)
{
}

kmb::Permutation::Permutation(const kmb::Permutation &other)
: n(0)
, k(0)
, seq(NULL)
{
	n = other.n;
	k = other.k;
	if( other.seq ){
		seq = new int[k];
		for(int i=0;i<k;++i){
			seq[i] = other.seq[i];
		}
	}
}

kmb::Permutation::~Permutation(void)
{
	if( seq ){
		delete[] seq;
	}
}

int
kmb::Permutation::getN(void) const
{
	return n;
}

int
kmb::Permutation::getK(void) const
{
	return k;
}

bool
kmb::Permutation::initialize(int n,int k)
{
	if( 0 >= k || k > n ){
		return false;
	}
	if( seq ){
		delete[] seq;
	}
	this->n = n;
	this->k = k;
	seq = new int[k];
	for(int i=0;i<k;++i){
		seq[i] = i;
	}
	return true;
}

bool
kmb::Permutation::isFinished(void) const
{
	return ( seq == NULL );
}



bool
kmb::Permutation::nextPerm(int i)
{
	if( seq && 0 <= i && i < k ){
		if( seq[i] == n-1 ){
			if( i == 0 ){
				delete[] seq;
				seq = NULL;
				return false;
			}
			return nextPerm(i-1);
		}

		bool* temp = new bool[n];
		for(int j=0;j<n;++j){
			temp[j] = false;
		}
		for(int j=0;j<i;++j){
			temp[ seq[j] ] = true;
		}

		bool flag = false;
		for(int j=0;j<n;++j){
			if( temp[j] == false && j > seq[i] ){
				seq[i] = j;
				temp[j] = true;
				flag = true;
				break;
			}
		}

		if( !flag ){
			delete[] temp;
			return nextPerm(i-1);
		}

		++i;
		if( i < k ){
			for(int j=0;j<n;++j){
				if( temp[j] == false ){
					seq[i] = j;
					temp[j] = true;
					++i;
					if( i == k ){
						break;
					}
				}
			}
		}
		delete[] temp;
		return true;
	}
	return false;
}

int
kmb::Permutation::getPerm(int index) const
{
	if( seq && 0 <= index && index < k){
		return seq[index];
	}
	return -1;
}

bool
kmb::Permutation::hasNext(void) const
{
	if( seq ){
		for(int i=0;i<k;++i){
			if( seq[i] != n-1-i ){
				return true;
			}
		}
		return false;
	}
	return false;
}

kmb::Permutation&
kmb::Permutation::operator=(const Permutation& other)
{
	if( seq ){
		delete[] seq;
		seq = NULL;
	}
	this->n = other.n;
	this->k = other.k;
	if( other.seq ){
		seq = new int[k];
		for(int i=0;i<k;++i){
			seq[i] = other.seq[i];
		}
	}
	return *this;
}

kmb::Permutation&
kmb::Permutation::operator++(void)
{
	this->nextPerm( k-1 );
	return *this;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::Permutation
kmb::Permutation::operator++(int n)
{
	kmb::Permutation other( *this );
	this->nextPerm( k-1 );
	return other;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
