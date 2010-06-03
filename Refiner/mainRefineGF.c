/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program RefineGF                                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/*
 * gcc -D_CONSOLE mainRefineGF.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * サンプル実行例＆テスト用プログラム
 * GF ファイルの読み込みと細分
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	printf( "REVOCAP Refiner Test Program\n" );
	if( argc < 3 ){
		printf( "GF File and GF Boundary should be given to this program.\n" );
		return -1;
	}
	rcapInitRefiner(0,0);
	rcapLoadGFFile( argv[1], argv[2] );
	rcapRefineFFbModel();
	rcapSaveGFFile( "REFINEMESH", "REFINEBOUN" );
	rcapTermRefiner();
	return 0;
}

#endif
