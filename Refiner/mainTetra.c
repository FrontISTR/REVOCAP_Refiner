/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Sample Program Tetrahedron                                           #
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
 * 四面体の細分チェック用
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
	/* 四面体を1つ並べる */
	float64_t coords[12] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
	};
	size_t refineNodeCount = 0;
	float64_t* resultCoords = NULL;
	int32_t tetras[4] = {
		1, 2, 3, 4,
	};
	/* 細分後の四面体の節点配列：出力は8個 */
	int32_t* refineTetras = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_TETRAHEDRON;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* 初期節点の個数 */
	size_t nodeCount = 4;
	/* 初期要素の個数 */
	size_t elementCount = 1;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;

	/* 境界条件（節点グループ） */
	int32_t ng0[3] = {1,2,3};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 3;

	/* カウンタ */
	int32_t i,j;

	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );

	printf("REVOCAP_Refiner sample program : Tetra Refine\n");
	printf("----- Original Model -----\n");
	printf("---\n");

	/* 座標値を Refiner に与える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 4 );
	printf("node:\n");
	printf("  size: %zu\n", nodeCount );
	printf("  coordinate:\n");
	for(i=0;(size_t)i<nodeCount;++i){
		printf("  - [%d, %f, %f, %f]\n", i+nodeOffset, coords[3*i], coords[3*i+1], coords[3*i+2] );
	}

	/* 細分前の要素数 */
	assert( elementCount == 1 );
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
	assert( ng0Count == 3 );
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
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras);
	rcapCommit();

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
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	printf("Refined Node Group : Count = %zu\n", ng0Count );
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

	free( refineTetras );
	rcapTermRefiner();
	return 0;
}

#endif
