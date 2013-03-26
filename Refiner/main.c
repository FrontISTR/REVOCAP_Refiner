/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Sample Program By C Language                                         #
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
 * サンプル実行例＆テスト用プログラム
 *
 * これはもっとも単純な場合として、2つの四面体からなるモデルを
 * 細分するサンプルである。
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
	/*
	 * 使い方の例
	 * 初めの5つは細分する前の節点座標
	 * 局所的には 1 から順に節点番号が付与されていると解釈する
	 */
	float64_t coords[15] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0
	};
	/*
	 * 細分後の節点番号を格納する
	 */
	float64_t* resultCoords = NULL;
	/* 節点番号の GlobalId */
	int32_t globalIds[5] = { 100, 101, 102, 103, 104 };
	/* メッシュの節点配列に現れる節点番号の最初の値
	 * 通常 C から呼ぶときは 0 fortran から呼ぶ場合は 1
	 * ここでは C から利用しているが、あえて 1 ずらした場合を考える
	 */
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* 四面体の節点配列：入力は2個 */
	int32_t tetras[8] = {
		1, 2, 3, 4,
		2, 3, 4, 5
	};
	/* 細分後の四面体の節点配列：出力は2*8=16個*/
	int32_t* refineTetras = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_TETRAHEDRON;
	/* 初期節点の個数 */
	size_t nodeCount = 5;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;
	/* 細分後の節点の個数 */
	size_t refineNodeCount = 0;
	/* 要素の細分と同時に更新する節点グループ */
	int32_t ng0[3] = {1,2,3};
	size_t ngCount = 3;
	int32_t* result_ng0 = NULL;
	/* 要素の細分と同時に更新する面グループ */
	/* 要素番号と要素内面番号の順に交互に並べる */
	int32_t fg0[4] = {1,3,2,1};   /* [1,2,3] と [2,5,4] */
	size_t fgCount = 2;
	int32_t* result_fg0 = NULL;
	/* 要素の細分と同時に更新する要素グループ */
	int32_t eg0[1] = {2};
	size_t egCount = 1;
	int32_t* result_eg0 = NULL;
	/* 関数の戻り値格納用 */
	int32_t seg[2] = {-1,-1};
	int8_t orgtype = RCAP_UNKNOWNTYPE;
	int32_t middle = -1;
	int32_t flag = 0;
	/* ループのカウンタ */
	int32_t i = 0;
	int32_t j = 0;

	assert( sizeof(int8_t) == 1 );
	assert( sizeof(uint8_t) == 1 );
	assert( sizeof(int16_t) == 2 );
	assert( sizeof(uint16_t) == 2 );
	assert( sizeof(int32_t) == 4 );
	assert( sizeof(uint32_t) == 4 );
	assert( sizeof(float32_t) == 4 );
	assert( sizeof(float64_t) == 8 );

	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );

	printf("----- Original Model -----\n");
	printf("---\n");
	/*
	 * globalId と座標値を Refiner に教える
	 * localIds は NULL をあたえると coords は nodeOffset から順番に並んでいるものと解釈する
	 */
	rcapSetNode64( nodeCount, coords, globalIds, NULL );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 5 );
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
	for(i=0;i<elementCount;++i){
		printf("      - [%d, TETRAHEDRON, %d, %d, %d, %d]\n", i+elementOffset,
			tetras[4*i], tetras[4*i+1], tetras[4*i+2], tetras[4*i+3] );
	}
	/* 節点グループの登録 */
	rcapAppendNodeGroup("innovate",ngCount,ng0);
	ngCount = rcapGetNodeGroupCount("innovate");
	assert( ngCount == 3 );
	printf("data:\n");
	printf("  - name: innovate\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",ngCount);
	printf("    id:\n");
	for(i=0;(size_t)i<ngCount;++i){
		printf("    - %d\n", ng0[i]);
	}
	/* 面グループの登録 */
	rcapAppendFaceGroup("revolute",fgCount,fg0);
	fgCount = rcapGetFaceGroupCount("revolute");
	assert( fgCount == 2 );
	printf("  - name: revolute\n");
	printf("    mode: FACEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",fgCount);
	printf("    id:\n");
	for(i=0;(size_t)i<fgCount;++i){
		printf("    - [%d, %d]\n", fg0[2*i], fg0[2*i+1]);
	}
	/* 要素グループの登録 */
	rcapAppendElementGroup("eg",egCount,eg0);
	egCount = rcapGetElementGroupCount("eg");
	assert( egCount == 1 );
	printf("  - name: eg\n");
	printf("    mode: ELEMENTGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",egCount);
	printf("    id:\n");
	for(i=0;(size_t)i<egCount;++i){
		printf("    - %d\n", eg0[i]);
	}

	/* 要素の細分 */
	refineElementCount = rcapGetRefineElementCount( elementCount, etype );
	assert( refineElementCount == 16 );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	assert( refineElementCount == 16 );
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
	for(j=0;(size_t)j<refineNodeCount;++j){
		printf("  - [%d, %f, %f, %f]\n", j+nodeOffset, resultCoords[3*j], resultCoords[3*j+1], resultCoords[3*j+2] );
		orgtype = rcapGetOriginal( j+nodeOffset, seg );
		if( orgtype == RCAP_SEGMENT ){
			assert( coords[3*(seg[0]-nodeOffset)] + coords[3*(seg[1]-nodeOffset)] == 2.0 * resultCoords[3*j] );
			assert( coords[3*(seg[0]-nodeOffset)+1] + coords[3*(seg[1]-nodeOffset)+1] == 2.0 * resultCoords[3*j+1] );
			assert( coords[3*(seg[0]-nodeOffset)+2] + coords[3*(seg[1]-nodeOffset)+2] == 2.0 * resultCoords[3*j+2] );
		}
	}

	/* 中間節点のチェック */
	for(j=0;(size_t)j<4*refineElementCount;++j){
		orgtype = rcapGetOriginal( refineTetras[j], seg );
		if( orgtype == RCAP_SEGMENT ){
			middle = rcapGetMiddle( orgtype, seg );
			/* 中間節点 => 元の両端 => 中間節点 が同じものをさすこと */
			assert( middle == refineTetras[j] );
		}else{
			/* 中間節点ではない => 元からある節点 */
			assert( refineTetras[j] <= (int32_t)nodeCount );
		}
	}

	/* 細分後の要素 */
	printf("element:\n");
	printf("  - size: %zu\n", refineElementCount );
	printf("    connectivity:\n");
	for(i=0;(size_t)i<refineElementCount;++i){
		printf("      - [%d, TETRAHEDRON, %d, %d, %d, %d]\n", i+elementOffset,
			refineTetras[4*i], refineTetras[4*i+1], refineTetras[4*i+2], refineTetras[4*i+3] );
	}

	/* 細分後の節点グループの更新 */
	ngCount = rcapGetNodeGroupCount("innovate");
	assert( ngCount > 0 );
	result_ng0 = (int32_t*)calloc( ngCount, sizeof(int32_t) );
	rcapGetNodeGroup("innovate",ngCount,result_ng0);
	printf("data:\n");
	printf("  - name: innovate\n");
	printf("    mode: NODEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %d\n",ngCount);
	printf("    id:\n");

	/* 細分後の節点グループのチェック */
	/* 元の節点グループを含む */
	for(i=0;i<3;++i){
		flag = 0;
		for(j=0;(size_t)j<ngCount;++j){
			if( result_ng0[j] == ng0[i] ){
				flag = 1;
			}
		}
		assert( flag == 1 );
	}
	/* 細分後の節点グループの確認 */
	for(j=0;(size_t)j<ngCount;++j){
		printf("    - %d\n", result_ng0[j]);
		orgtype = rcapGetOriginal( result_ng0[j], seg );
		if( orgtype == RCAP_SEGMENT ){
			/* 中間節点 => 元の両端 */
			flag = 0;
			for(i=0;i<3;++i){
				if( seg[0] == ng0[i] ){
					flag = 1;
				}
			}
			assert( flag == 1 );
			flag = 0;
			for(i=0;i<3;++i){
				if( seg[1] == ng0[i] ){
					flag = 1;
				}
			}
			assert( flag == 1 );
		}else{
			/* 中間節点ではない */
			flag = 0;
			for(i=0;i<3;++i){
				if( result_ng0[j] == ng0[i] ){
					flag = 1;
				}
			}
			assert( flag == 1 );
		}
	}
	free( result_ng0 );

	/* 細分後の面グループの更新 */
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("  - name: revolute\n");
	printf("    mode: FACEGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",fgCount);
	printf("    id:\n");
	if( fgCount > 0 ){
		result_fg0 = (int32_t*)calloc( fgCount*2, sizeof(int32_t) );
		rcapGetFaceGroup("revolute",fgCount,result_fg0);
		assert( fgCount == 8 );
		for(i=0;(size_t)i<fgCount;++i){
			printf("    - [%d, %d]\n", result_fg0[2*i], result_fg0[2*i+1]);
		}
		free( result_fg0 );
	}

	/* 細分後の要素グループの更新 */
	egCount = rcapGetElementGroupCount("eg");
	printf("  - name: eg\n");
	printf("    mode: ELEMENTGROUP\n");
	printf("    vtype: NONE\n");
	printf("    size: %zu\n",egCount);
	printf("    id:\n");
	if( egCount > 0 ){
		result_eg0 = (int32_t*)calloc( egCount, sizeof(int32_t) );
		rcapGetElementGroup("eg",egCount,result_eg0);
		assert( egCount == 8 );
		flag = 1;
		for(i=0;i<egCount;++i){
			printf("    - %d\n", result_eg0[i]);
			if( result_eg0[i] != 9+i ){
				flag = 0;
			}
		}
		assert( flag == 1 );
		free( result_eg0 );
	}

	free( resultCoords );
	free( refineTetras );

	rcapTermRefiner();
	return 0;
}

#endif
