/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Sample Program NodeVariable                                          #
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
/*
 *
 * サンプル実行例＆テスト用プログラム
 * BoundaryNodeGroup と BoundaryNodeVariable の挙動チェック用
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include "rcapRefinerMacros.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */
#include <assert.h>
#include <string.h>

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
	/* 細分後の座標：必要に応じて calloc する */
	float64_t* resultCoords = NULL;
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
	/* 細分後の節点の個数 */
	size_t refineNodeCount = 0;

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
	int32_t i = 0;
	int32_t j = 0;
	int32_t middle = -1;
	int32_t flag = 0;
	char mode[32];
	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* 中点の Variable は小さい方を与える */
	rcapSetInterpolateMode( "MIN" );
	rcapGetInterpolateMode( mode );
	assert( strncmp( "MIN", mode, 3 )==0 );

	printf("REVOCAP_Refiner sample program : Boundary Node Variable Refine\n");
	printf("----- Original Model -----\n");
	printf("---\n");
	/* 座標値を Refiner に教える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 12 );
	printf("node:\n");
	printf("  size: %zu\n", nodeCount );
	printf("  coordinate:\n");
	for(i=0;(size_t)i<nodeCount;++i){
		printf("  - [%d, %f, %f, %f]\n", i+nodeOffset, coords[3*i], coords[3*i+1], coords[3*i+2] );
	}

	/* 細分前の要素数 */
	assert( elementCount == 10 );
	printf("element:\n");
	printf("  - size: %zu\n", elementCount );
	printf("    connectivity:\n");
	for(i=0;(size_t)i<elementCount;++i){
		printf("      - [%d, TETRAHEDRON, %d, %d, %d, %d]\n", i+elementOffset,
			tetras[4*i], tetras[4*i+1], tetras[4*i+2], tetras[4*i+3] );
	}

	/* 節点グループの登録 */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	assert( ng0Count == 4 );
	printf("data:\n");
	printf("  - name: ng0\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",ng0Count);
	printf("    id:\n");
	for(i=0;(size_t)i<ng0Count;++i){
		printf("    - %d\n", ng0[i]);
	}

	/* 境界節点グループの登録 */
	rcapAppendBNodeGroup("bng0",bng0Count,bng0);
	bng0Count = rcapGetBNodeGroupCount("bng0");
	assert( bng0Count == 3 );
	printf("  - name: bng0\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",bng0Count);
	printf("    id:\n");
	for(i=0;(size_t)i<ng0Count;++i){
		printf("    - %d\n", bng0[i]);
	}

	/* 境界節点変数の登録 */
	rcapAppendBNodeVarInt("bnv0",bnv0Count,bnv0,bnv1);
	bnv0Count = rcapGetBNodeVarIntCount("bnv0");
	assert( bnv0Count == 3 );
	printf("  - name: bnv0\n");
	printf("    mode: NODEVARIABLE\n");
	printf("    vtype: INTEGER\n");
	printf("    size: %zu\n",bnv0Count);
	printf("    value:\n");
	for(i=0;(size_t)i<bnv0Count;++i){
		printf("    - [%d, %d]\n", bnv0[i], bnv1[i]);
	}

	/*---------------------- REFINE -----------------------------------------*/

	/* 要素の細分 */
	refineElementCount = rcapGetRefineElementCount( elementCount, etype );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	rcapCommit();

	printf("----- Refined Model -----\n");
	printf("---\n");

	/* 細分後の節点 */
	refineNodeCount = rcapGetNodeCount();
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	printf("node:\n");
	printf("  size: %zu\n", refineNodeCount );
	printf("  coordinate:\n");
	for(i=0;(size_t)i<refineNodeCount;++i){
		printf("  - [%d, %f, %f, %f]\n", i+nodeOffset, resultCoords[3*i], resultCoords[3*i+1], resultCoords[3*i+2] );
	}
	free( resultCoords );

	/* 細分後の要素 */
	printf("element:\n");
	printf("  - size: %zu\n", refineElementCount );
	printf("    connectivity:\n");
	for(i=0;(size_t)i<refineElementCount;++i){
		printf("      - [%d, TETRAHEDRON, %d, %d, %d, %d]\n", i+elementOffset,
			refineTetras[4*i], refineTetras[4*i+1], refineTetras[4*i+2], refineTetras[4*i+3] );
	}

	/* 細分後の節点グループの更新 */
	ng0Count = rcapGetNodeGroupCount("ng0");
	assert( ng0Count > 0 );
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	printf("data:\n");
	printf("  - name: ng0\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %d\n",ng0Count);
	printf("    id:\n");
	for(i=0;(size_t)i<ng0Count;++i){
		printf("    - %d\n", result_ng0[i]);
	}
	free( result_ng0 );
	result_ng0 = NULL;

	/* 境界節点グループの登録 */
	bng0Count = rcapGetBNodeGroupCount("bng0");
	result_bng0 = (int32_t*)calloc( bng0Count, sizeof(int32_t) );
	rcapGetBNodeGroup("bng0",bng0Count,result_bng0);
	printf("  - name: bng0\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %d\n",bng0Count);
	printf("    id:\n");
	for(i=0;(size_t)i<bng0Count;++i){
		printf("    - %d\n", result_bng0[i]);
	}
	free( result_bng0 );

	/* 境界節点変数の登録 */
	bnv0Count = rcapGetBNodeVarIntCount("bnv0");
	result_bnv0 = (int32_t*)calloc( bnv0Count, sizeof(int32_t) );
	result_bnv1 = (int32_t*)calloc( bnv0Count, sizeof(int32_t) );
	rcapGetBNodeVarInt("bnv0",bnv0Count,result_bnv0,result_bnv1);
	printf("  - name: bnv0\n");
	printf("    mode: NODEVARIABLE\n");
	printf("    vtype: INTEGER\n");
	printf("    size: %zu\n",bnv0Count);
	printf("    value:\n");
	for(i=0;(size_t)i<bnv0Count;++i){
		printf("    - [%d, %d]\n", result_bnv0[i], result_bnv1[i]);
	}

	/* ここでチェック！ */
	for(i=0;(size_t)i<bnv0Count;++i){
		orgtype = rcapGetOriginal( result_bnv0[i], seg );
		if( orgtype == RCAP_SEGMENT ){
			middle = rcapGetMiddle( orgtype, seg );
			assert( middle == result_bnv0[i] );
			/* 小さい方になっていることを確かめる */
			flag = 0;
			for(j=0;(size_t)j<bnv0Count;++j){
				if( bnv0[j] == seg[0] ){
					assert( bnv1[j] >= result_bnv1[i] );
					if( bnv1[j] == result_bnv1[i] ){
						flag = 1;
					}
				}
				if( bnv0[j] == seg[1] ){
					assert( bnv1[j] >= result_bnv1[i] );
					if( bnv1[j] == result_bnv1[i] ){
						flag = 1;
					}
				}
			}
			assert( flag == 1 );
		}
	}
	free( result_bnv0 );
	free( result_bnv1 );
	free( refineTetras );
	rcapTermRefiner();
	return 0;
}

#endif
