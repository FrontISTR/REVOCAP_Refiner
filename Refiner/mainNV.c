/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program NodeVariable                                          #
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
 * gcc -D_CONSOLE mainNV.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * �T���v�����s�ၕ�e�X�g�p�v���O����
 * BoundaryNodeGroup �� BoundaryNodeVariable �̋����`�F�b�N�p
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */

int main(void)
{
	/* �Z�ʑ̂��T�����������̂�2���ׂ� */
	float64_t coords[36] = {
		0.0, 0.0, -1.0,
		1.0, 0.0, -1.0,
		1.0, 1.0, -1.0,
		0.0, 1.0, -1.0,
		0.0, 0.0,  0.0,
		1.0, 0.0,  0.0,
		1.0, 1.0,  0.0,
		0.0, 1.0,  0.0,
		0.0, 0.0,  1.0,
		1.0, 0.0,  1.0,
		1.0, 1.0,  1.0,
		0.0, 1.0,  1.0,
	};
	int32_t tetras[40] = {
		1, 2, 4, 5,
		2, 3, 4, 7,
		4, 5, 7, 8,
		2, 4, 5, 7,
		6, 2, 5, 7,
		5, 6, 7,10,
		7,10,11,12,
		5, 9,10,12,
		5, 7, 8,12,
		5,10, 7,12
	};
	/* �ו���̎l�ʑ̂̐ߓ_�z��F�o�͂�10*8=80�� */
	int32_t* refineTetras = NULL;
	/* �ו�����v�f�̌^(�萔�l) */
	int8_t etype = RCAP_TETRAHEDRON;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* �����ߓ_�̌� */
	size_t nodeCount = 12;
	/* �����v�f�̌� */
	size_t elementCount = 10;
	/* �ו���̗v�f�̌� */
	size_t refineElementCount = 0;
	/* ���̐����̎擾 */
	int32_t seg[2] = {-1,-1};
	int8_t orgtype = RCAP_UNKNOWNTYPE;

	/* ���E���� */
	int32_t ng0[4] = {1,2,5,6};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 4;
	int32_t bng0[3] = {5,6,7};
	int32_t* result_bng0 = NULL;
	size_t bng0Count = 3;
	int32_t bnv0[3] = {5,6,7};  /* �ߓ_ */
	int32_t bnv1[3] = {1,2,3};  /* �ߓ_��̒l */
	int32_t* result_bnv0 = NULL;
	int32_t* result_bnv1 = NULL;
	size_t bnv0Count = 3;
	size_t i = 0;
	size_t j = 0;
	int32_t middle = -1;
	int32_t flag = 0;
	char mode[32];
	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* ���_�� Variable �͏���������^���� */
	rcapSetInterpolateMode( "MIN" );
	rcapGetInterpolateMode( mode );
	printf("Interpolation Mode %s\n",mode);
	printf("---------------------- ORIGINAL -----------------------------------------\n");
	/* ���W�l�� Refiner �ɋ����� */
	rcapSetNode64( nodeCount, coords, NULL, NULL );
	/* �ו��O�̗v�f�� */
	printf("Original Element Count = %u\n", elementCount );
	/* �ߓ_�O���[�v�̓o�^ */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	printf("Original Node Group Count %u\n", ng0Count );
	/* ���E�ߓ_�O���[�v�̓o�^ */
	rcapAppendBNodeGroup("bng0",bng0Count,bng0);
	bng0Count = rcapGetBNodeGroupCount("bng0");
	printf("Original Boundary Node Group Count %u\n", bng0Count );
	/* ���E�ߓ_�ϐ��̓o�^ */
	rcapAppendBNodeVarInt("bnv0",bnv0Count,bnv0,bnv1);
	bng0Count = rcapGetBNodeVarIntCount("bnv0");
	printf("Original Boundary Node Variable Count %u\n", bnv0Count );
	/* �ו��O�̐ߓ_�� */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );

	printf("---------------------- REFINE -----------------------------------------\n");
	/* �v�f�̍ו� */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	printf("Refined Element Count = %u\n", refineElementCount );
	rcapCommit();
	/* �ו���̐ߓ_�O���[�v�̍X�V */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	printf("Refined Node Group Count %u\n", ng0Count );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	free( result_ng0 );
	/* �ו���̋��E�ߓ_�O���[�v�̍X�V */
	bng0Count = rcapGetBNodeGroupCount("bng0");
	result_bng0 = (int32_t*)calloc( bng0Count, sizeof(int32_t) );
	printf("Refined Boundary Node Group Count %u\n", bng0Count );
	rcapGetBNodeGroup("bng0",bng0Count,result_bng0);
	free( result_bng0 );
	/* �ו���̋��E�ߓ_�ϐ��̍X�V */
	bnv0Count = rcapGetBNodeVarIntCount("bnv0");
	result_bnv0 = (int32_t*)calloc( bnv0Count, sizeof(int32_t) );
	result_bnv1 = (int32_t*)calloc( bnv0Count, sizeof(int32_t) );
	printf("Refined Boundary Node Variable Count %u\n", bnv0Count );
	rcapGetBNodeVarInt("bnv0",bnv0Count,result_bnv0,result_bnv1);
	/* �ו���̐ߓ_�̌� */
	nodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", nodeCount );
	/* �����Ń`�F�b�N�I */
	for(i=0;i<bnv0Count;++i){
		orgtype = rcapGetOriginal( result_bnv0[i], seg );
		if( orgtype == RCAP_SEGMENT ){
			middle = rcapGetMiddle( orgtype, seg );
			if( middle != result_bnv0[i] ){
				printf( "Refine Node Error! Middle Node Different! %d => [%d,%d] => %d\n",  result_bnv0[i], seg[0], seg[1], middle);
			}
			/* ���������ɂȂ��Ă��邱�Ƃ��m���߂� */
			flag = 0;
			for(j=0;j<bnv0Count;++j){
				if( bnv0[j] == seg[0] ){
					if( bnv1[j] < result_bnv1[i] ){
						printf( "Refine Node Variable Error! %d : %d < %d\n",  result_bnv0[i], bnv1[j], result_bnv1[i] );
					}else if( bnv1[j] == result_bnv1[i] ){
						flag = 1;
					}
				}
				if( bnv0[j] == seg[1] ){
					if( bnv1[j] < result_bnv1[i] ){
						printf( "Refine Node Variable Error! %d : %d < %d\n",  result_bnv0[i], bnv1[j], result_bnv1[i] );
					}else if( bnv1[j] == result_bnv1[i] ){
						flag = 1;
					}
				}
			}
			if( flag == 0 ){
				printf( "Refine Node Variable Error! Not Min Value %d : %d\n",  result_bnv0[i], result_bnv1[i] );
			}
		}
	}
	free( result_bnv0 );
	free( result_bnv1 );
	free( refineTetras );
	rcapTermRefiner();
	return 0;
}

#endif
