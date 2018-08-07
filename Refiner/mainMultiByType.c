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
 * �T���v�����s�ၕ�e�X�g�p�v���O����
 * ������ނ̗v�f�ו��`�F�b�N�p
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */

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
	size_t refineElementCount = 0;

	/* ���E�����i�ߓ_�O���[�v�j */
	int32_t ng0[5] = {1,2,5,6,9};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 5;

	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* ���W�l�� Refiner �ɗ^���� */
	rcapSetNode64( nodeCount, coords, NULL, NULL );

	printf("---------------------- ORIGINAL -----------------------------------------\n");
	/* �ו��O�̗v�f�� */
	printf("Original Element Count = %d\n", elementCount );
	/* �ߓ_�O���[�v�̓o�^ */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	printf("Original Node Group Count %u\n", ng0Count );
	/* �ו��O�̐ߓ_�� */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );

	printf("---------------------- REFINE -----------------------------------------\n");
	/* �v�f�̍ו� */
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

	/* �ו���̐ߓ_�O���[�v�̍X�V */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	printf("Refined Node Group Count %u\n", ng0Count );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	free( result_ng0 );

	/* �ו���̐ߓ_ */
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
