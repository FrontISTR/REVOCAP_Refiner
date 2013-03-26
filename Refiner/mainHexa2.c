/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Sample Program Hexahedron2                                           #
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
 * gcc -D_CONSOLE mainHexa2.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * サンプル実行例＆テスト用プログラム
 * 六面体２次要素の細分チェック用
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include "rcapRefinerMacros.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */
#include <assert.h>

int main(void)
{
	/* 六面体を2つ並べる */
	float64_t coords[96] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 0.0, 2.0,
		1.0, 0.0, 2.0,
		1.0, 1.0, 2.0,
		0.0, 1.0, 2.0,
		0.5, 0.0, 0.0,
		1.0, 0.5, 0.0,
		0.5, 1.0, 0.0,
		0.0, 0.5, 0.0,
		0.5, 0.0, 1.0,
		1.0, 0.5, 1.0,
		0.5, 1.0, 1.0,
		0.0, 0.5, 1.0,
		0.5, 0.0, 2.0,
		1.0, 0.5, 2.0,
		0.5, 1.0, 2.0,
		0.0, 0.5, 2.0,
		0.0, 0.0, 0.5,
		1.0, 0.0, 0.5,
		1.0, 1.0, 0.5,
		0.0, 1.0, 0.5,
		0.0, 0.0, 1.5,
		1.0, 0.0, 1.5,
		1.0, 1.0, 1.5,
		0.0, 1.0, 1.5,
	};

	size_t refineNodeCount = 0;
	float64_t* resultCoords = NULL;
	int32_t hexas[40] = {
		1,2,3,4, 5,6,7,8,    13,14,15,16, 17,18,19,20, 25,26,27,28,
		5,6,7,8, 9,10,11,12, 17,18,19,20, 21,22,23,24, 29,30,31,32,
	};

	/* 細分後の六面体の節点配列：出力は2*8=16個 */
	int32_t* refineHexas = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_HEXAHEDRON2;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;

	/* 初期節点の個数 */
	size_t nodeCount = 32;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;
	/* 元の節点の取得 */
	int32_t org[20] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

	/* 境界条件（節点グループ） */
	int32_t ng0[13] = {1,2,5,6,9,10,13,25,26,17,29,30,21};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 13;

	/* カウンタ */
	int32_t i = 0;
	size_t j = 0;
	/* テンポラリ */
	float64_t x = 0.0, y = 0.0, z = 0.0;
	int32_t n = 0;

	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );

	printf("REVOCAP_Refiner sample program : Fitting Hexahedron Second\n");
	printf("----- Original Model -----\n");
	printf("---\n");

	/* 座標値を Refiner に与える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 32 );
	printf("node:\n");
	printf("  size: %zu\n", nodeCount );
	printf("  coordinate:\n");
	for(i=0;(size_t)i<nodeCount;++i){
		printf("  - [%d, %f, %f, %f]\n", i+nodeOffset, coords[3*i], coords[3*i+1], coords[3*i+2] );
	}

	/* 細分前の要素数 */
	assert( elementCount == 2 );
	printf("element:\n");
	printf("  - size: %zu\n", elementCount );
	printf("    connectivity:\n");
	for(i=0;(size_t)i<elementCount;++i){
		printf("      - [%d, HEXAHEDRON2, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
			hexas[20*i], hexas[20*i+1], hexas[20*i+2], hexas[20*i+3],
			hexas[20*i+4], hexas[20*i+5], hexas[20*i+6], hexas[20*i+7],
			hexas[20*i+8], hexas[20*i+9], hexas[20*i+10], hexas[20*i+11],
			hexas[20*i+12], hexas[20*i+13], hexas[20*i+14], hexas[20*i+15],
			hexas[20*i+16], hexas[20*i+17], hexas[20*i+18], hexas[20*i+19]);
	}

	/* 節点グループの登録 */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	assert( ng0Count == 13 );
	printf("data:\n");
	printf("  - name: ng0\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",ng0Count);
	printf("    id:\n");
	for(i=0;(size_t)i<ng0Count;++i){
		printf("    - %d\n", ng0[i]);
	}

	printf("----- Refined Model -----\n");
	printf("---\n");

	/* 要素の細分 */
	refineElementCount = rcapGetRefineElementCount( elementCount, etype );
	assert( refineElementCount == 16 );
	refineHexas = (int32_t*)calloc( 20*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, hexas, refineHexas );
	assert( elementCount == 16 );

	rcapCommit();

	/* 細分後の節点 */
	refineNodeCount = rcapGetNodeCount();
	assert( refineNodeCount == 141 );
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	printf("node:\n");
	printf("  size: %zu\n", refineNodeCount );
	printf("  coordinate:\n");
	for(i=0;(size_t)i<refineNodeCount;++i){
		printf("  - [%d, %f, %f, %f]\n", i+nodeOffset, resultCoords[3*i], resultCoords[3*i+1], resultCoords[3*i+2] );
	}
	// チェック
	for(i=0;i<(int32_t)refineNodeCount;++i){
		etype = rcapGetOriginal( i+nodeOffset, org );
		if( etype == RCAP_SEGMENT ){
			x = 0.0;
			y = 0.0;
			z = 0.0;
			for(j=0;j<2;++j){
				org[j]-=nodeOffset;
				x += resultCoords[3*org[j]];
				y += resultCoords[3*org[j]+1];
				z += resultCoords[3*org[j]+2];
			}
			assert( resultCoords[3*i] == 0.5 * x );
			assert( resultCoords[3*i+1] == 0.5 * y );
			assert( resultCoords[3*i+2] == 0.5 * z );
		}else if( etype == RCAP_QUAD2 ){
			x = 0.0;
			y = 0.0;
			z = 0.0;
			for(j=0;j<4;++j){
				org[j]-=nodeOffset;
				x += resultCoords[3*org[j]];
				y += resultCoords[3*org[j]+1];
				z += resultCoords[3*org[j]+2];
			}
			assert( resultCoords[3*i] == 0.45 * x );
			assert( resultCoords[3*i+1] == 0.45 * y );
			assert( resultCoords[3*i+2] == 0.45 * z );
		}else if( etype == RCAP_HEXAHEDRON2 ){
			x = 0.0;
			y = 0.0;
			z = 0.0;
			for(j=0;j<8;++j){
				org[j]-=nodeOffset;
				x += resultCoords[3*org[j]];
				y += resultCoords[3*org[j]+1];
				z += resultCoords[3*org[j]+2];
			}
			assert( resultCoords[3*i] == 0.125 * x );
			assert( resultCoords[3*i+1] == 0.125 * y );
			assert( resultCoords[3*i+2] == 0.125 * z );
		}else if( etype == RCAP_UNKNOWNTYPE ){
			assert( resultCoords[3*i] == coords[3*i] );
			assert( resultCoords[3*i+1] == coords[3*i+1] );
			assert( resultCoords[3*i+2] == coords[3*i+2] );
		}else{
			assert( !"error refined node coordinate" );
		}
	}
	free( resultCoords );

	/* 細分後の要素 */
	printf("element:\n");
	printf("  - size: %zu\n", refineElementCount );
	printf("    connectivity:\n");
	for(i=0;(size_t)i<refineElementCount;++i){
		printf("      - [%d, HEXAHEDRON2, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
			refineHexas[20*i], refineHexas[20*i+1], refineHexas[20*i+2], refineHexas[20*i+3],
			refineHexas[20*i+4], refineHexas[20*i+5], refineHexas[20*i+6], refineHexas[20*i+7],
			refineHexas[20*i+8], refineHexas[20*i+9], refineHexas[20*i+10], refineHexas[20*i+11],
			refineHexas[20*i+12], refineHexas[20*i+13], refineHexas[20*i+14], refineHexas[20*i+15],
			refineHexas[20*i+16], refineHexas[20*i+17], refineHexas[20*i+18], refineHexas[20*i+19]
			);
	}

	/* 細分後の節点グループの更新 */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	assert( ng0Count == 37 );
	printf("data:\n");
	printf("  - name: ng0\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %d\n",ng0Count);
	printf("    id:\n");
	for(i=0;(size_t)i<ng0Count;++i){
		printf("    - %d\n", result_ng0[i]);
	}

	// チェック
	org[0] = 1; org[1] = 13;
	n = rcapGetMiddle( RCAP_SEGMENT, org );
	assert( n != -1 );
	for(j=0;j<ng0Count;++j){
		if( n == result_ng0[j] ){
			n = -1;
			break;
		}
	}
	assert( n == -1 );

	org[0] = 6; org[1] = 30;
	n = rcapGetMiddle( RCAP_SEGMENT, org );
	assert( n != -1 );
	for(j=0;j<ng0Count;++j){
		if( n == result_ng0[j] ){
			n = -1;
			break;
		}
	}
	assert( n == -1 );
	free( result_ng0 );

	free( refineHexas );

	rcapTermRefiner();
	return 0;
}

#endif
