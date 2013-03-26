/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Sample Program MultiType                                             #
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
 * 複数種類の要素細分チェック用（要素タイプ別に）
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
	/* 六面体の上に三角柱を乗せる */
	float64_t coords[30] = {
		0.0, 0.0, -1.0,
		1.0, 0.0, -1.0,
		1.0, 1.0, -1.0,
		0.0, 1.0, -1.0,
		0.0, 0.0,  0.0,
		1.0, 0.0,  0.0,
		1.0, 1.0,  0.0,
		0.0, 1.0,  0.0,
		0.5, 0.0,  1.0,
		0.5, 1.0,  1.0,
	};
	size_t refineNodeCount = 0;
	float64_t* resultCoords = NULL;
	int32_t nodes[14] = {
		1, 2, 3, 4, 5, 6, 7, 8,
		5, 9, 6, 8, 10, 7,
	};
	/* 細分後の節点配列：出力は 8*8 + 8*6 = 112 個 */
	int32_t* refineNodes = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etypes[2] = {
		RCAP_HEXAHEDRON,
		RCAP_WEDGE,
	};
	int8_t* resultEtypes = NULL;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* 初期節点の個数 */
	size_t nodeCount = 10;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;
	/* 細分後の節点配列の大きさ */
	size_t refineNodesArraySize = 0;

	/* 境界条件（節点グループ） */
	int32_t ng0[5] = {1,2,5,6,9};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 5;

	/* カウンタ */
	int32_t i,j;

	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* 座標値を Refiner に与える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );

	printf("REVOCAP_Refiner sample program : Multi Type\n");
	printf("----- Original Model -----\n");
	printf("---\n");
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 10 );
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
	j = 0;
	for(i=0;(size_t)i<elementCount;++i){
		switch( etypes[i] ){
		case RCAP_WEDGE:
			printf("      - [%d, WEDGE, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
				nodes[j], nodes[j+1], nodes[j+2], nodes[j+3], nodes[j+4], nodes[j+5] );
			j += 6;
			break;
		case RCAP_HEXAHEDRON:
			printf("      - [%d, HEXAHEDRON, %d, %d, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
				nodes[j], nodes[j+1], nodes[j+2], nodes[j+3], nodes[j+4], nodes[j+5], nodes[j+6], nodes[j+7] );
			j += 8;
			break;
		default:
			break;
		}
	}

	/* 節点グループの登録 */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	assert( ng0Count == 5 );
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

	/* 細分後の要素を格納するのに必要な節点配列の大きさの取得 */
	refineNodesArraySize = rcapGetRefineElementMultiCount( elementCount, etypes, &refineElementCount );
	if( refineNodesArraySize == 0 ){
		rcapTermRefiner();
		return 0;
	}
	refineNodes = (int32_t*)calloc( refineNodesArraySize, sizeof(int32_t) );
	/* 要素の型も受け取る場合 */
	resultEtypes = (int8_t*)calloc( refineElementCount, sizeof(int8_t) );
	rcapRefineElementMulti( elementCount, etypes, nodes, &refineElementCount, resultEtypes, refineNodes );
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
	j = 0;
	for(i=0;(size_t)i<refineElementCount;++i){
		switch( resultEtypes[i] ){
		case RCAP_WEDGE:
			printf("      - [%d, WEDGE, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
				refineNodes[j], refineNodes[j+1], refineNodes[j+2], refineNodes[j+3], refineNodes[j+4], refineNodes[j+5] );
			j += 6;
			break;
		case RCAP_HEXAHEDRON:
			printf("      - [%d, HEXAHEDRON, %d, %d, %d, %d, %d, %d, %d, %d]\n", i+elementOffset,
				refineNodes[j], refineNodes[j+1], refineNodes[j+2], refineNodes[j+3], refineNodes[j+4], refineNodes[j+5], refineNodes[j+6], refineNodes[j+7] );
			j += 8;
			break;
		default:
			break;
		}
	}
	free( refineNodes );
	free( resultEtypes );

	/* 細分後の節点グループ */
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

	rcapTermRefiner();
	return 0;
}

#endif
