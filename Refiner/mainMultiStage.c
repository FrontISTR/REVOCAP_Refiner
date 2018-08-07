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
 * �Q�i�K���s�T���v���v���O����
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */

int main(void)
{
	/*
	 * �g�����̗�
	 * ���߂�5�͍ו�����O�̐ߓ_���W
	 */
	float64_t coords[15] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0
	};
	/* �ו���̍��W�F�K�v�ɉ����� calloc ���� */
	float64_t* resultCoords = NULL;
	/* �l�ʑ̂̐ߓ_�z��F���͂�2�� */
	int32_t tetras[8] = {
		0, 1, 2, 3,
		1, 2, 3, 4
	};
	/* �ו���̎l�ʑ̂̐ߓ_�z��F�o�͂�2*8=16��*/
	int32_t* refineTetras = NULL;
	/* �Q�i�K�ו���̎l�ʑ̂̐ߓ_�z��F�o�͂�2*8*8=128��*/
	int32_t* refine2Tetras = NULL;
	/* �ו�����v�f�̌^(�萔�l) */
	int8_t etype = RCAP_TETRAHEDRON;
	/* ���b�V���̐ߓ_�z��Ɍ����ߓ_�ԍ��̍ŏ��̒l */
	/* C ����ĂԂƂ��� 0 fortran ����Ăԏꍇ�� 1 */
	int32_t nodeOffset = 0;
	int32_t elementOffset = 0;
	/* �����ߓ_�̌� */
	size_t nodeCount = 5;
	/* �����v�f�̌� */
	size_t elementCount = 2;
	/* �ו���̗v�f�̌� */
	size_t refineElementCount = 0;
	/* �ו���̐ߓ_�̌� */
	size_t refineNodeCount = 0;
	/* �v�f�̍ו��Ɠ����ɍX�V����ߓ_�O���[�v */
	int32_t ng0[3] = {0,1,4};
	size_t ngCount = 3;
	int32_t* result_ng0 = NULL;
	/* �v�f�̍ו��Ɠ����ɍX�V����ʃO���[�v */
	/* �v�f�ԍ��Ɨv�f���ʔԍ��̏��Ɍ��݂ɕ��ׂ� */
	int32_t fg0[4] = {0,0,1,1};  /* [1,2,3] [1,4,3] */
	size_t fgCount = 2;
	int32_t* result_fg0 = NULL;

	printf("---------------------- ORIGINAL DATA -----------------------------------------\n");
	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	rcapInitRefiner( nodeOffset, elementOffset );
	/*
	 * globalId �ƍ��W�l�� Refiner �ɋ�����
	 * localIds �� NULL ����������� coords �� nodeOffset ���珇�Ԃɕ���ł�����̂Ɖ��߂���
	 */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* �ו��O�̗v�f�� */
	printf("Original Element Count = 2\n" );
	/* �ߓ_�O���[�v�̓o�^ */
	rcapAppendNodeGroup("innovate",ngCount,ng0);
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Original Node Group Count %u\n", ngCount );
	/* �ʃO���[�v�̓o�^ */
	rcapAppendFaceGroup("revolute",fgCount,fg0);
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Original Face Group Count %u\n", fgCount );
	/* �ו��O�̐ߓ_�� */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );

	printf("---------------------- REFINE STEP 1 -----------------------------------------\n");
	/* �v�f�̍ו� */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	printf("Refined Element Count = %u\n", elementCount );
	rcapCommit();
	/* �ו���̐ߓ_�O���[�v�̍X�V */
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Refined Node Group Count %u\n", ngCount );
	result_ng0 = (int32_t*)calloc( ngCount, sizeof(int32_t) );
	rcapGetNodeGroup("innovate",ngCount,result_ng0);
	/* �����Ń`�F�b�N */
	free( result_ng0 );
	result_ng0 = NULL;
	/* �ו���̖ʃO���[�v�̍X�V */
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Refined Face Group Count %u\n", fgCount );
	result_fg0 = (int32_t*)calloc( 2*fgCount, sizeof(int32_t) );
	rcapGetFaceGroup("revolute",fgCount,result_fg0);
	/* �����Ń`�F�b�N */
	free( result_fg0 );
	result_fg0 = NULL;
	/* �ו���̐ߓ_�̌� */
	refineNodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", refineNodeCount );
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, 0, resultCoords );
	free( resultCoords );
	resultCoords = NULL;

	/* ��Q�i�̍ו��̑O�ɃL���b�V�����N���A */
	rcapClearRefiner();

	printf("---------------------- REFINE STEP 2 -----------------------------------------\n");

	/* �v�f�̍ו� */
	refineElementCount = rcapRefineElement( elementCount, etype, refineTetras, NULL );
	refine2Tetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, refineTetras, refine2Tetras );
	printf("Refined Element Count = %u\n", elementCount );
	rcapCommit();

	/* �ו���̐ߓ_�O���[�v�̍X�V */
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Refined Node Group Count %u\n", ngCount );
	result_ng0 = (int32_t*)calloc( ngCount, sizeof(int32_t) );
	rcapGetNodeGroup("innovate",ngCount,result_ng0);
	free( result_ng0 );
	result_ng0 = NULL;

	/* �ו���̖ʃO���[�v�̍X�V */
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Refined Face Group Count %u\n", fgCount );
	result_fg0 = (int32_t*)calloc( 2*fgCount, sizeof(int32_t) );
	rcapGetFaceGroup("revolute",fgCount,result_fg0);
	free( result_fg0 );
	result_fg0 = NULL;

	/* �ו���̐ߓ_�̌� */
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
