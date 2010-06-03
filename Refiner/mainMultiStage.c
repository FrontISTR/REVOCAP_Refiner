/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program MultiStage                                            #
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
 * gcc -D_CONSOLE mainMultiStage.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * ２段階実行サンプルプログラム
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */

int main(void)
{
	/*
	 * 使い方の例
	 * 初めの5つは細分する前の節点座標
	 */
	float64_t coords[15] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0
	};
	/* 細分後の座標：必要に応じて calloc する */
	float64_t* resultCoords = NULL;
	/* 四面体の節点配列：入力は2個 */
	int32_t tetras[8] = {
		0, 1, 2, 3,
		1, 2, 3, 4
	};
	/* 細分後の四面体の節点配列：出力は2*8=16個*/
	int32_t* refineTetras = NULL;
	/* ２段階細分後の四面体の節点配列：出力は2*8*8=128個*/
	int32_t* refine2Tetras = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_TETRAHEDRON;
	/* メッシュの節点配列に現れる節点番号の最初の値 */
	/* C から呼ぶときは 0 fortran から呼ぶ場合は 1 */
	int32_t nodeOffset = 0;
	int32_t elementOffset = 0;
	/* 初期節点の個数 */
	size_t nodeCount = 5;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;
	/* 細分後の節点の個数 */
	size_t refineNodeCount = 0;
	/* 要素の細分と同時に更新する節点グループ */
	int32_t ng0[3] = {0,1,4};
	size_t ngCount = 3;
	int32_t* result_ng0 = NULL;
	/* 要素の細分と同時に更新する面グループ */
	/* 要素番号と要素内面番号の順に交互に並べる */
	int32_t fg0[4] = {0,0,1,1};  /* [1,2,3] [1,4,3] */
	size_t fgCount = 2;
	int32_t* result_fg0 = NULL;

	printf("---------------------- ORIGINAL DATA -----------------------------------------\n");
	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	/*
	 * globalId と座標値を Refiner に教える
	 * localIds は NULL をあたえると coords は nodeOffset から順番に並んでいるものと解釈する
	 */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* 細分前の要素数 */
	printf("Original Element Count = 2\n" );
	/* 節点グループの登録 */
	rcapAppendNodeGroup("innovate",ngCount,ng0);
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Original Node Group Count %u\n", ngCount );
	/* 面グループの登録 */
	rcapAppendFaceGroup("revolute",fgCount,fg0);
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Original Face Group Count %u\n", fgCount );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );

	printf("---------------------- REFINE STEP 1 -----------------------------------------\n");
	/* 要素の細分 */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	printf("Refined Element Count = %u\n", elementCount );
	rcapCommit();
	/* 細分後の節点グループの更新 */
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Refined Node Group Count %u\n", ngCount );
	result_ng0 = (int32_t*)calloc( ngCount, sizeof(int32_t) );
	rcapGetNodeGroup("innovate",ngCount,result_ng0);
	/* ここでチェック */
	free( result_ng0 );
	result_ng0 = NULL;
	/* 細分後の面グループの更新 */
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Refined Face Group Count %u\n", fgCount );
	result_fg0 = (int32_t*)calloc( 2*fgCount, sizeof(int32_t) );
	rcapGetFaceGroup("revolute",fgCount,result_fg0);
	/* ここでチェック */
	free( result_fg0 );
	result_fg0 = NULL;
	/* 細分後の節点の個数 */
	refineNodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", refineNodeCount );
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, 0, resultCoords );
	free( resultCoords );
	resultCoords = NULL;

	/* 第２段の細分の前にキャッシュをクリア */
	rcapClearRefiner();

	printf("---------------------- REFINE STEP 2 -----------------------------------------\n");

	/* 要素の細分 */
	refineElementCount = rcapRefineElement( elementCount, etype, refineTetras, NULL );
	refine2Tetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, refineTetras, refine2Tetras );
	printf("Refined Element Count = %u\n", elementCount );
	rcapCommit();

	/* 細分後の節点グループの更新 */
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Refined Node Group Count %u\n", ngCount );
	result_ng0 = (int32_t*)calloc( ngCount, sizeof(int32_t) );
	rcapGetNodeGroup("innovate",ngCount,result_ng0);
	free( result_ng0 );
	result_ng0 = NULL;

	/* 細分後の面グループの更新 */
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Refined Face Group Count %u\n", fgCount );
	result_fg0 = (int32_t*)calloc( 2*fgCount, sizeof(int32_t) );
	rcapGetFaceGroup("revolute",fgCount,result_fg0);
	free( result_fg0 );
	result_fg0 = NULL;

	/* 細分後の節点の個数 */
	refineNodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", refineNodeCount );

	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, 0, resultCoords );
	free( resultCoords );
	resultCoords = NULL;

	free( refineTetras );
	refineTetras = NULL;
	free( refine2Tetras );
	refine2Tetras = NULL;

	rcapTermRefiner();
	return 0;
}

#endif
