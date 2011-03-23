/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.0                          #
# Program Name : rcapRefinerMacros                                     #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

/*
 * テストプログラム用のマクロ集
 */

/* size_t 型の出力を C99 でも VC8 でも警告が出ないようにする */
#ifdef _MSC_VER
/* #if __STDC_VERSION__ < 199901L */
# define PRIsz "u"
#else
# define PRIsz "zd"
#endif
