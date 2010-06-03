/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program NodeVariable                                          #
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
 * gcc -D_CONSOLE mainNV.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * サンプル実行例＆テスト用プログラム
 * BoundaryNodeGroup と BoundaryNodeVariable の挙動チェック用
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */

int main(void)
{
	/* 六面体を５分割したものを2つ並べる */
	float64_t coords[36] = {
		0.0, 0.0, -1.0,
		1.0, 0.0, -1.0,
		1.0, 1.0, -1.0,
		0.0, 1.0, -1.0,
		0.0, 0.0,  0.0,
		1.0, 0.0,  0.0,
		1.0, 1.0,  0.0,
		0.0, 1.0,  0.0,
		0.0, 0.0,  1.0,
		1.0, 0.0,  1.0,
		1.0, 1.0,  1.0,
		0.0, 1.0,  1.0,
	};
	int32_t tetras[40] = {
		1, 2, 4, 5,
		2, 3, 4, 7,
		4, 5, 7, 8,
		2, 4, 5, 7,
		6, 2, 5, 7,
		5, 6, 7,10,
		7,10,11,12,
		5, 9,10,12,
		5, 7, 8,12,
		5,10, 7,12
	};
	/* 細分後の四面体の節点配列：出力は10*8=80個 */
	int32_t* refineTetras = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_TETRAHEDRON;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* 初期節点の個数 */
	size_t nodeCount = 12;
	/* 初期要素の個数 */
	size_t elementCount = 10;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;
	/* 元の線分の取得 */
	int32_t seg[2] = {-1,-1};
	int8_t orgtype = RCAP_UNKNOWNTYPE;

	/* 境界条件 */
	int32_t ng0[4] = {1,2,5,6};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 4;
	int32_t bng0[3] = {5,6,7};
	int32_t* result_bng0 = NULL;
	size_t bng0Count = 3;
	int32_t bnv0[3] = {5,6,7};  /* 節点 */
	int32_t bnv1[3] = {1,2,3};  /* 節点上の値 */
	int32_t* result_bnv0 = NULL;
	int32_t* result_bnv1 = NULL;
	size_t bnv0Count = 3;
	size_t i = 0;
	size_t j = 0;
	int32_t middle = -1;
	int32_t flag = 0;
	char mode[32];
	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* 中点の Variable は小さい方を与える */
	rcapSetInterpolateMode( "MIN" );
	rcapGetInterpolateMode( mode );
	printf("Interpolation Mode %s\n",mode);
	printf("---------------------- ORIGINAL -----------------------------------------\n");
	/* 座標値を Refiner に教える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* 細分前の要素数 */
	printf("Original Element Count = %u\n", elementCount );
	/* 節点グループの登録 */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	printf("Original Node Group Count %u\n", ng0Count );
	/* 境界節点グループの登録 */
	rcapAppendBNodeGroup("bng0",bng0Count,bng0);
	bng0Count = rcapGetBNodeGroupCount("bng0");
	printf("Original Boundary Node Group Count %u\n", bng0Count );
	/* 境界節点変数の登録 */
	rcapAppendBNodeVarInt("bnv0",bnv0Count,bnv0,bnv1);
	bng0Count = rcapGetBNodeVarIntCount("bnv0");
	printf("Original Boundary Node Variable Count %u\n", bnv0Count );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );

	printf("---------------------- REFINE -----------------------------------------\n");
	/* 要素の細分 */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	printf("Refined Element Count = %u\n", refineElementCount );
	rcapCommit();
	/* 細分後の節点グループの更新 */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	printf("Refined Node Group Count %u\n", ng0Count );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	free( result_ng0 );
	/* 細分後の境界節点グループの更新 */
	bng0Count = rcapGetBNodeGroupCount("bng0");
	result_bng0 = (int32_t*)calloc( bng0Count, sizeof(int32_t) );
	printf("Refined Boundary Node Group Count %u\n", bng0Count );
	rcapGetBNodeGroup("bng0",bng0Count,result_bng0);
	free( result_bng0 );
	/* 細分後の境界節点変数の更新 */
	bnv0Count = rcapGetBNodeVarIntCount("bnv0");
	result_bnv0 = (int32_t*)calloc( bnv0Count, sizeof(int32_t) );
	result_bnv1 = (int32_t*)calloc( bnv0Count, sizeof(int32_t) );
	printf("Refined Boundary Node Variable Count %u\n", bnv0Count );
	rcapGetBNodeVarInt("bnv0",bnv0Count,result_bnv0,result_bnv1);
	/* 細分後の節点の個数 */
	nodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", nodeCount );
	/* ここでチェック！ */
	for(i=0;i<bnv0Count;++i){
		orgtype = rcapGetOriginal( result_bnv0[i], seg );
		if( orgtype == RCAP_SEGMENT ){
			middle = rcapGetMiddle( orgtype, seg );
			if( middle != result_bnv0[i] ){
				printf( "Refine Node Error! Middle Node Different! %d => [%d,%d] => %d\n",  result_bnv0[i], seg[0], seg[1], middle);
			}
			/* 小さい方になっていることを確かめる */
			flag = 0;
			for(j=0;j<bnv0Count;++j){
				if( bnv0[j] == seg[0] ){
					if( bnv1[j] < result_bnv1[i] ){
						printf( "Refine Node Variable Error! %d : %d < %d\n",  result_bnv0[i], bnv1[j], result_bnv1[i] );
					}else if( bnv1[j] == result_bnv1[i] ){
						flag = 1;
					}
				}
				if( bnv0[j] == seg[1] ){
					if( bnv1[j] < result_bnv1[i] ){
						printf( "Refine Node Variable Error! %d : %d < %d\n",  result_bnv0[i], bnv1[j], result_bnv1[i] );
					}else if( bnv1[j] == result_bnv1[i] ){
						flag = 1;
					}
				}
			}
			if( flag == 0 ){
				printf( "Refine Node Variable Error! Not Min Value %d : %d\n",  result_bnv0[i], result_bnv1[i] );
			}
		}
	}
	free( result_bnv0 );
	free( result_bnv1 );
	free( refineTetras );
	rcapTermRefiner();
	return 0;
}

#endif
