/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.0                          #
# Sample Program ShapeFuncFitting                                      #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/*
 *
 * 形状関数による四面体2次要素の細分テスト用プログラム
 *
 * Sample Program for refinement by shape functions.
 * This model consists of one element of second degree tetrahedron.
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
	int32_t nodeOffset = 0;
	int32_t elementOffset = 0;
	/* 初期節点 */
	size_t nodeCount = 10;
	float64_t coords[30] = {
		0.0, 0.0, 0.0, // 0
		1.0, 0.0, 0.0, // 1
		0.0, 1.0, 0.0, // 2
		0.0, 0.0, 1.0, // 3
		0.5, 0.5,-0.1, // 4
		0.0, 0.5,-0.1, // 5
		0.5, 0.0,-0.1, // 6
		0.0, 0.0, 0.5, // 7
		0.5, 0.0, 0.5, // 8
		0.0, 0.5, 0.5  // 9
	};
	/* 細分後の節点 */
	size_t refineNodeCount = 0;
	float64_t* resultCoords = NULL;
	/* 細分前の要素 */
	int8_t etype = RCAP_TETRAHEDRON2;
	size_t elementCount = 1;
	int32_t tetras[10] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	};
	/* 細分後の要素 */
	size_t refineElementCount = 0;
	int32_t* refineTetras = NULL;

	/* カウンタ */
	int32_t i,j;

	/* 初期化：節点番号、要素番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );

	/* 形状関数による中間節点を有効にする */
	rcapSetSecondFitting(1);

	printf("----- Original Model -----\n");
	/* 座標値を Refiner に与える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 10 );
	printf("Node : Count = %"PRIsz"\n", nodeCount );
	for(i=0;(size_t)i<nodeCount;++i){
		printf("%d : %f, %f, %f\n", i+nodeOffset, coords[3*i], coords[3*i+1], coords[3*i+2] );
	}
	/* 細分前の要素数 */
	assert( elementCount == 1 );
	printf("Element : Count = %"PRIsz"\n", elementCount );
	for(i=0;(size_t)i<elementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", i+elementOffset, etype,
			tetras[10*i],   tetras[10*i+1], tetras[10*i+2], tetras[10*i+3], tetras[10*i+4],
			tetras[10*i+5], tetras[10*i+6], tetras[10*i+7], tetras[10*i+8], tetras[10*i+9]
			);
	}

	printf("----- Refined Model -----\n");

	/* 要素の細分 */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 10*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras);
	rcapCommit();

	/* 細分後の節点 */
	refineNodeCount = rcapGetNodeCount();
	printf("Node : Count = %"PRIsz"\n", refineNodeCount );
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	for(j=0;(size_t)j<refineNodeCount;++j){
		printf("%d : %f, %f, %f\n", j+nodeOffset, resultCoords[3*j], resultCoords[3*j+1], resultCoords[3*j+2] );
	}
	free( resultCoords );

	/* 細分後の要素 */
	printf("Element : Count = %"PRIsz"\n", refineElementCount );
	for(i=0;(size_t)i<refineElementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", i+elementOffset, etype,
			refineTetras[10*i],   refineTetras[10*i+1], refineTetras[10*i+2], refineTetras[10*i+3], refineTetras[10*i+4],
			refineTetras[10*i+5], refineTetras[10*i+6], refineTetras[10*i+7], refineTetras[10*i+8], refineTetras[10*i+9]
			);
	}
	free( refineTetras );

	rcapTermRefiner();
	return 0;
}

#endif
