/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program MultiByType                                           #
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
 * gcc -D_CONSOLE mainMultiByType.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * サンプル実行例＆テスト用プログラム
 * 複数種類の要素細分チェック用
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
	int32_t hexas[8] = {
		1, 2, 3, 4, 5, 6, 7, 8,
	};
	int32_t wedges[6] = {
		5, 9, 6, 8, 10, 7,
	};
	/* 細分後の六面体の節点配列：出力は1*8=8個 */
	int32_t* refineHexas = NULL;
	/* 細分後の三角柱の節点配列：出力は1*8=8個 */
	int32_t* refineWedges = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_HEXAHEDRON;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* 初期節点の個数 */
	size_t nodeCount = 10;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;

	/* 境界条件（節点グループ） */
	int32_t ng0[5] = {1,2,5,6,9};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 5;

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
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	refineElementCount = rcapRefineElement( elementCount, etype, hexas, NULL );
	refineHexas = (int32_t*)calloc( 8*refineElementCount, sizeof(int32_t) );
	etype = RCAP_WEDGE;
	elementCount = 1;
	refineElementCount = rcapRefineElement( elementCount, etype, wedges, NULL );
	refineWedges = (int32_t*)calloc( 6*refineElementCount, sizeof(int32_t) );
	refineElementCount = 0;
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	refineElementCount += rcapRefineElement( elementCount, etype, hexas, refineHexas );
	etype = RCAP_WEDGE;
	elementCount = 1;
	refineElementCount += rcapRefineElement( elementCount, etype, wedges, refineWedges );
	printf("Refined Element Count = %u\n", refineElementCount );
	rcapCommit();

	/* 細分後の節点グループの更新 */
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

	free( refineHexas );
	free( refineWedges );

	rcapTermRefiner();
	return 0;
}

#endif
