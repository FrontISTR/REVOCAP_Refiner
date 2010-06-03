/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program MultiType                                             #
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
 * gcc -D_CONSOLE mainMultiType.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * サンプル実行例＆テスト用プログラム
 * 複数種類の要素細分チェック用（要素タイプ別に）
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */

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
	int32_t i;

	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* 座標値を Refiner に与える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );

	printf("---------------------- ORIGINAL -----------------------------------------\n");
	/* 細分前の要素数 */
	printf("Original Element Count = %d\n", elementCount );
	/* 節点グループの登録 */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	printf("Original Node Group Count %u\n", ng0Count );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );

	printf("---------------------- REFINE -----------------------------------------\n");
	/* 要素の細分 */
	refineNodesArraySize = rcapRefineElementMulti( elementCount, etypes, nodes, &refineElementCount, NULL, NULL );
	if( refineNodesArraySize == 0 ){
		rcapTermRefiner();
		return 0;
	}
	printf("Refined Element Size = %u\n", refineElementCount);
	printf("Refined Node Array Size = %u\n", refineNodesArraySize);
	refineNodes = (int32_t*)calloc( refineNodesArraySize, sizeof(int32_t) );
	/* 要素の型も受け取る場合 */
	resultEtypes = (int8_t*)calloc( refineElementCount, sizeof(int8_t) );
	refineElementCount = rcapRefineElementMulti( elementCount, etypes, nodes, &refineElementCount, resultEtypes, refineNodes );
	printf("Refined Element Count = %u\n", refineElementCount );
	printf("Refined types\n");
	for(i=0;i<refineElementCount;++i){
		printf("Refined Element type %d = %d\n", i, resultEtypes[i] );
	}
	free( resultEtypes );
/*
	refineElementCount = rcapRefineElementMulti( elementCount, etypes, nodes, &refineElementCount, NULL, refineNodes );
	printf("Refined Element Count = %u\n", refineElementCount );
*/
	rcapCommit();

	/* 細分後の要素 */
	printf("HEXA\n");
	for(i=0;i<8;++i){
		printf("%d %d %d %d %d %d %d %d\n",
			refineNodes[8*i],
			refineNodes[8*i+1],
			refineNodes[8*i+2],
			refineNodes[8*i+3],
			refineNodes[8*i+4],
			refineNodes[8*i+5],
			refineNodes[8*i+6],
			refineNodes[8*i+7]);
	}
	printf("WEDGE\n");
	for(i=0;i<8;++i){
		printf("%d %d %d %d %d %d\n",
			refineNodes[64+6*i],
			refineNodes[64+6*i+1],
			refineNodes[64+6*i+2],
			refineNodes[64+6*i+3],
			refineNodes[64+6*i+4],
			refineNodes[64+6*i+5]);
	}

	/* 細分後の節点グループ */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	printf("Refined Node Group Count %u\n", ng0Count );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	free( result_ng0 );

	/* 細分後の節点 */
	refineNodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", refineNodeCount );
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	free( resultCoords );

	free( refineNodes );

	rcapTermRefiner();
	return 0;
}

#endif
