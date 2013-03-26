/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Sample Program FittingRefine                                         #
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
 * 形状補正機能テスト用プログラムサンプル
 * 六面体２つからなる格子を円柱で形状補正する
 * 形状ファイルは column.rnf (z=0で二つの曲面を張り合わせ)
 *
 * Sample Program for refinement with fitting to CAD surfaces.
 * This model consists two hexahedra with a column.
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
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	float64_t coords[36] = {
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		0.0,-1.0, 0.0,
		0.0, 0.0,-1.0,
		1.0, 1.0, 0.0,
		1.0, 0.0, 1.0,
		1.0,-1.0, 0.0,
		1.0, 0.0,-1.0,
		2.0, 1.0, 0.0,
		2.0, 0.0, 1.0,
		2.0,-1.0, 0.0,
		2.0, 0.0,-1.0,
	};
	/*
	 * CAD ファイルに記述してあるグローバル節点番号との対応
	 * nodeOffset = 1 を引いた値が CAD ファイルに記述してあることに注意
	 */
	int32_t globalIds[12] = {
		101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112
	};
	int32_t localIds[12] = {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
	};
	size_t refineNodeCount = 0;
	float64_t* resultCoords = NULL;
	int32_t hexas[16] = {
		1, 2, 3, 4, 5, 6, 7, 8,
		5, 6, 7, 8, 9, 10, 11, 12,
	};
	/* 細分後の要素の節点配列 */
	int32_t* refineHexas = NULL;
	int8_t etype = RCAP_HEXAHEDRON;
	/* 初期節点の個数 */
	size_t nodeCount = 12;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;

	/* 境界条件（節点グループ） */
	int32_t ng0[4] = {1,4,5,8};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 4;

	/* カウンタ */
	int32_t i,j;

	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	rcapSetCADFilename( "data/column/column.rnf" );

	printf("REVOCAP_Refiner sample program : Fitting Refine1\n");
	printf("----- Original Model -----\n");
	printf("---\n");
	/* 座標値を Refiner に与える */
	/* 節点配列で与える局所節点番号と、CAD ファイルに記述してある大域節点番号との対応も与える */
	rcapSetNode64( nodeCount, coords, globalIds, localIds );
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
	assert( elementCount == 2 );
	printf("element:\n");
	printf("  - size: %zu\n", elementCount );
	printf("    connectivity:\n");
	for(i=0;(size_t)i<elementCount;++i){
		printf("      - [%d, HEXAHEDRON, %d, %d, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
			hexas[8*i], hexas[8*i+1], hexas[8*i+2], hexas[8*i+3],
			hexas[8*i+4], hexas[8*i+5], hexas[8*i+6], hexas[8*i+7]);
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

	printf("----- Refined Model -----\n");
	printf("---\n");

	/* 要素の細分 */
	refineElementCount = rcapGetRefineElementCount( elementCount, etype );
	refineHexas = (int32_t*)calloc( 8*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, hexas, refineHexas);
	rcapCommit();

	/* 細分後の節点 */
	refineNodeCount = rcapGetNodeCount();
	printf("node:\n");
	printf("  size: %zu\n", refineNodeCount );
	printf("  coordinate:\n");
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	for(j=0;(size_t)j<refineNodeCount;++j){
		printf("  - [%d, %f, %f, %f]\n", j+nodeOffset, resultCoords[3*j], resultCoords[3*j+1], resultCoords[3*j+2] );
	}
	free( resultCoords );

	/* 細分後の要素 */
	printf("element:\n");
	printf("  - size: %zu\n", refineElementCount );
	printf("    connectivity:\n");
	for(i=0;(size_t)i<refineElementCount;++i){
		printf("      - [%d, HEXAHEDRON, %d, %d, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
			refineHexas[8*i], refineHexas[8*i+1], refineHexas[8*i+2], refineHexas[8*i+3],
			refineHexas[8*i+4], refineHexas[8*i+5], refineHexas[8*i+6], refineHexas[8*i+7] );
	}

	/* 細分後の節点グループの更新 */
	ng0Count = rcapGetNodeGroupCount("ng0");
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

	free( refineHexas );
	rcapTermRefiner();
	return 0;
}

#endif
