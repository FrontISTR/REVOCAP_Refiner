/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.0                          #
# Sample Program Tetrahedron                                           #
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
 * �T���v�����s�ၕ�e�X�g�p�v���O����
 * �l�ʑ̂̍ו��`�F�b�N�p
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
	/* �l�ʑ̂�1���ׂ� */
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
	/* �ו���̎l�ʑ̂̐ߓ_�z��F�o�͂�8�� */
	int32_t* refineTetras = NULL;
	/* �ו�����v�f�̌^(�萔�l) */
	int8_t etype = RCAP_TETRAHEDRON;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* �����ߓ_�̌� */
	size_t nodeCount = 4;
	/* �����v�f�̌� */
	size_t elementCount = 1;
	/* �ו���̗v�f�̌� */
	size_t refineElementCount = 0;

	/* ���E�����i�ߓ_�O���[�v�j */
	int32_t ng0[3] = {1,2,3};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 3;

	/* �J�E���^ */
	int32_t i,j;

	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	rcapInitRefiner( nodeOffset, elementOffset );
	printf("----- Original Model -----\n");
	/* ���W�l�� Refiner �ɗ^���� */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* �ו��O�̐ߓ_�� */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 4 );
	printf("Node : Count = %"PRIsz"\n", nodeCount );
	for(i=0;(size_t)i<nodeCount;++i){
		printf("%d : %f, %f, %f\n", i+nodeOffset, coords[3*i], coords[3*i+1], coords[3*i+2] );
	}
	/* �ו��O�̗v�f�� */
	assert( elementCount == 1 );
	printf("Element : Count = %"PRIsz"\n", elementCount );
	for(i=0;(size_t)i<elementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d\n", i+elementOffset, etype,
			tetras[4*i], tetras[4*i+1], tetras[4*i+2], tetras[4*i+3]);
	}
	/* �ߓ_�O���[�v�̓o�^ */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	assert( ng0Count == 3 );
	printf("Node Group : Count = %"PRIsz"\n", ng0Count );
	for(i=0;(size_t)i<ng0Count;++i){
		printf("%d\n", ng0[i]);
	}

	printf("----- Refined Model -----\n");

	/* �v�f�̍ו� */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras);
	rcapCommit();

	/* �ו���̐ߓ_ */
	refineNodeCount = rcapGetNodeCount();
	printf("Node : Count = %"PRIsz"\n", refineNodeCount );
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	for(j=0;(size_t)j<refineNodeCount;++j){
		printf("%d : %f, %f, %f\n", j+nodeOffset, resultCoords[3*j], resultCoords[3*j+1], resultCoords[3*j+2] );
	}
	free( resultCoords );

	/* �ו���̗v�f */
	printf("Element : Count = %"PRIsz"\n", refineElementCount );
	for(i=0;(size_t)i<refineElementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d\n", i+elementOffset, etype,
			refineTetras[4*i], refineTetras[4*i+1], refineTetras[4*i+2], refineTetras[4*i+3] );
	}

	/* �ו���̐ߓ_�O���[�v�̍X�V */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	printf("Refined Node Group : Count = %"PRIsz"\n", ng0Count );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	for(i=0;(size_t)i<ng0Count;++i){
		printf("%d\n", result_ng0[i]);
	}
	free( result_ng0 );

	free( refineTetras );
	rcapTermRefiner();
	return 0;
}

#endif
