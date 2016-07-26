/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ArrayUtil                                               #
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


#include <cstring>

namespace kmb{

class ArrayUtil
{
public:
	// ary[i0] から n0 個を ary[i1] から n1 個と入れ替える
	// n0 と n1 が異なるときは、間を調節する。
	// i0 < i1 を仮定
	template <typename T> static bool arraySwap(T* ary, int i0, int n0, int i1, int n1){
		if( ary == NULL || i0 < 0 || n0 < 0 || i1 < 0 || n1 < 0 ){
			return false;
		}
		// 重なっていたらエラー
		if( i0 + n0 > i1 ){
			return false;
		}
		if( n0 == n1 ){
			if( n0 > 0 ){
				// そのまま交換
				T* tmp = new T[n0];
				memmove(tmp,ary+i0,sizeof(T)*n0);
				memmove(ary+i0,ary+i1,sizeof(T)*n0);
				memmove(ary+i1,tmp,sizeof(T)*n0);
				delete[] tmp;
			}
		}else if( n0 < n1 ){
			// 間を後ろにずらす
			T* tmp = new T[n1];
			memmove(tmp,ary+i1,sizeof(T)*n1);
			memmove(ary+i1+n1-n0,ary+i0,sizeof(T)*n0);
			memmove(ary+i0+n1,ary+i0+n0,sizeof(T)*(i1 - i0 - n0));
			memmove(ary+i0,tmp,sizeof(T)*n1);
			delete[] tmp;
		}else if( n0 > n1 ){
			// 間を前にずらす
			T* tmp = new T[n0];
			memmove(tmp,ary+i0,sizeof(T)*n0);
			memmove(ary+i0,ary+i1,sizeof(T)*n1);
			memmove(ary+i0+n1,ary+i0+n0,sizeof(T)*(i1 - i0 - n0));
			memmove(ary+i1+n1-n0,tmp,sizeof(T)*n0);
			delete[] tmp;
		}
		return true;
	}
};

}
