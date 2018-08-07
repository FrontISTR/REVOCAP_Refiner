/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.0                          #
# Sample Program MultiByType                                           #
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
 * ������ނ̗v�f�ו��`�F�b�N�p
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
	/* �Z�ʑ̂̏�ɎO�p�����悹�� */
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
	/* �ו���̘Z�ʑ̂̐ߓ_�z��F�o�͂�1*8=8�� */
	int32_t* refineHexas = NULL;
	/* �ו���̎O�p���̐ߓ_�z��F�o�͂�1*8=8�� */
	int32_t* refineWedges = NULL;
	/* �ו�����v�f�̌^(�萔�l) */
	int8_t etype = RCAP_HEXAHEDRON;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* �����ߓ_�̌� */
	size_t nodeCount = 10;
	/* �����v�f�̌� */
	size_t elementCount = 2;
	/* �ו���̗v�f�̌� */
	size_t refineHexaCount = 0;
	size_t refineWedgeCount = 0;
	size_t refineElementCount = 0;

	/* ���E�����i�ߓ_�O���[�v�j */
	int32_t ng0[5] = {1,2,5,6,9};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 5;

	/* �J�E���^ */
	int32_t i,j;

	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* ���W�l�� Refiner �ɗ^���� */
	rcapSetNode64( nodeCount, coords, NULL, NULL );

	printf("----- Original Model -----\n");
	/* �ו��O�̐ߓ_�� */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 10 );
	printf("Node : Count = %"PRIsz"\n", nodeCount );
	for(i=0;(size_t)i<nodeCount;++i){
		printf("%d : %f, %f, %f\n", i+nodeOffset, coords[3*i], coords[3*i+1], coords[3*i+2] );
	}
	/* �ו��O�̗v�f�� */
	assert( elementCount == 2 );
	printf("Element : Count = %"PRIsz"\n", elementCount );
	j = 0;
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	for(i=0;(size_t)i<elementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			hexas[8*i], hexas[8*i+1], hexas[8*i+2], hexas[8*i+3],
			hexas[8*i+4], hexas[8*i+5], hexas[8*i+6], hexas[8*i+7] );
		j++;
	}
	etype = RCAP_WEDGE;
	elementCount = 1;
	for(i=0;(size_t)i<elementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			wedges[6*i], wedges[6*i+1], wedges[6*i+2],
			wedges[6*i+3], wedges[6*i+4], wedges[6*i+5] );
		j++;
	}

	/* �ߓ_�O���[�v�̓o�^ */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	printf("Node Group : Count = %"PRIsz"\n", ng0Count );
	assert( ng0Count == 5 );
	for(i=0;(size_t)i<ng0Count;++i){
		printf("%d\n", ng0[i]);
	}

	printf("----- Refined Model -----\n");
	/* �v�f�̍ו� */
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	refineHexaCount = rcapRefineElement( elementCount, etype, hexas, NULL );
	refineHexas = (int32_t*)calloc( 8*refineHexaCount, sizeof(int32_t) );
	etype = RCAP_WEDGE;
	elementCount = 1;
	refineWedgeCount = rcapRefineElement( elementCount, etype, wedges, NULL );
	refineWedges = (int32_t*)calloc( 6*refineWedgeCount, sizeof(int32_t) );
	refineElementCount = 0;
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	refineHexaCount = rcapRefineElement( elementCount, etype, hexas, refineHexas );
	refineElementCount += refineHexaCount;
	etype = RCAP_WEDGE;
	elementCount = 1;
	refineWedgeCount = rcapRefineElement( elementCount, etype, wedges, refineWedges );
	refineElementCount += refineWedgeCount;
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
	j = 0;
	etype = RCAP_HEXAHEDRON;
	for(i=0;(size_t)i<refineHexaCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			refineHexas[8*i], refineHexas[8*i+1], refineHexas[8*i+2], refineHexas[8*i+3],
			refineHexas[8*i+4], refineHexas[8*i+5], refineHexas[8*i+6], refineHexas[8*i+7] );
		j++;
	}
	etype = RCAP_WEDGE;
	for(i=0;(size_t)i<refineWedgeCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			refineWedges[6*i], refineWedges[6*i+1], refineWedges[6*i+2],
			refineWedges[6*i+3], refineWedges[6*i+4], refineWedges[6*i+5] );
		j++;
	}

	/* �ו���̐ߓ_�O���[�v */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	printf("Node Group : Count = %"PRIsz"\n", ng0Count );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	for(j=0;(size_t)j<ng0Count;++j){
		printf("%d\n", result_ng0[j]);
	}
	free( result_ng0 );

	free( refineHexas );
	free( refineWedges );

	rcapTermRefiner();
	return 0;
}

#endif
