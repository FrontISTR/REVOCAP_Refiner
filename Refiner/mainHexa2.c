/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program Hexahedron2                                           #
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
 * gcc -D_CONSOLE mainHexa2.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * �T���v�����s�ၕ�e�X�g�p�v���O����
 * �Z�ʑ̂Q���v�f�̍ו��`�F�b�N�p
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */
#include <assert.h>

int main(void)
{
	/* �Z�ʑ̂�2���ׂ� */
	float64_t coords[96] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 0.0, 2.0,
		1.0, 0.0, 2.0,
		1.0, 1.0, 2.0,
		0.0, 1.0, 2.0,
		0.5, 0.0, 0.0,
		1.0, 0.5, 0.0,
		0.5, 1.0, 0.0,
		0.0, 0.5, 0.0,
		0.5, 0.0, 1.0,
		1.0, 0.5, 1.0,
		0.5, 1.0, 1.0,
		0.0, 0.5, 1.0,
		0.5, 0.0, 2.0,
		1.0, 0.5, 2.0,
		0.5, 1.0, 2.0,
		0.0, 0.5, 2.0,
		0.0, 0.0, 0.5,
		1.0, 0.0, 0.5,
		1.0, 1.0, 0.5,
		0.0, 1.0, 0.5,
		0.0, 0.0, 1.5,
		1.0, 0.0, 1.5,
		1.0, 1.0, 1.5,
		0.0, 1.0, 1.5,
	};

	size_t refineNodeCount = 0;
	float64_t* resultCoords = NULL;
	int32_t hexas[40] = {
		1,2,3,4, 5,6,7,8,    13,14,15,16, 17,18,19,20, 25,26,27,28,
		5,6,7,8, 9,10,11,12, 17,18,19,20, 21,22,23,24, 29,30,31,32,
	};

	/* �ו���̘Z�ʑ̂̐ߓ_�z��F�o�͂�2*8=16�� */
	int32_t* refineHexas = NULL;
	/* �ו�����v�f�̌^(�萔�l) */
	int8_t etype = RCAP_HEXAHEDRON2;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;

	/* �����ߓ_�̌� */
	size_t nodeCount = 32;
	/* �����v�f�̌� */
	size_t elementCount = 2;
	/* �ו���̗v�f�̌� */
	size_t refineElementCount = 0;
	/* ���̐ߓ_�̎擾 */
	int32_t org[20] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

	/* ���E�����i�ߓ_�O���[�v�j */
	int32_t ng0[13] = {1,2,5,6,9,10,13,25,26,17,29,30,21};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 13;

	/* �J�E���^ */
	int32_t i = 0;
	size_t j = 0;
	/* �e���|���� */
	float64_t x = 0.0, y = 0.0, z = 0.0;
	int32_t n = 0;

	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* globalId �ƍ��W�l�� Refiner �ɋ�����
	 * localIds �� NULL ����������� coords �� nodeOffset ���珇�Ԃɕ���ł�����̂Ɖ��߂���
	 */
	rcapSetNode64( nodeCount, coords, NULL, NULL );

	printf("---------------------- ORIGINAL -----------------------------------------\n");
	/* �ו��O�̐ߓ_�̊m�F */
	resultCoords = (float64_t*)calloc( 3*nodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( nodeCount, nodeOffset, resultCoords );
	for(j=0;j<nodeCount;++j){
		assert( resultCoords[3*j] == coords[3*j] );
		assert( resultCoords[3*j+1] == coords[3*j+1] );
		assert( resultCoords[3*j+2] == coords[3*j+2] );
	}
	free( resultCoords );

	/* �ו��O�̗v�f�� */
	printf("Original Element Count = %u\n", elementCount );

	/* �ߓ_�O���[�v�̓o�^ */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	assert( ng0Count == 13 );
	printf("Original Node Group Count %u\n", ng0Count );

	/* �ו��O�̐ߓ_�� */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );

	printf("---------------------- REFINE -----------------------------------------\n");
	/* �v�f�̍ו� */
	refineElementCount = rcapRefineElement( elementCount, etype, hexas, NULL );
	assert( refineElementCount == 16 );
	printf("Refined Element Count = %u\n", refineElementCount );
	refineHexas = (int32_t*)calloc( 20*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, hexas, refineHexas );
	assert( elementCount == 16 );
	free( refineHexas );

	rcapCommit();

	refineNodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", refineNodeCount );
	assert( refineNodeCount == 141 );

	/* �ו���̐ߓ_�̊m�F */

	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	for(i=0;i<refineNodeCount;++i){
		etype = rcapGetOriginal( i+nodeOffset, org );
		if( etype == RCAP_SEGMENT ){
			x = 0.0;
			y = 0.0;
			z = 0.0;
			for(j=0;j<2;++j){
				org[j]-=nodeOffset;
				x += resultCoords[3*org[j]];
				y += resultCoords[3*org[j]+1];
				z += resultCoords[3*org[j]+2];
			}
			assert( resultCoords[3*i] == 0.5 * x );
			assert( resultCoords[3*i+1] == 0.5 * y );
			assert( resultCoords[3*i+2] == 0.5 * z );
		}else if( etype == RCAP_QUAD2 ){
			x = 0.0;
			y = 0.0;
			z = 0.0;
			for(j=0;j<4;++j){
				org[j]-=nodeOffset;
				x += resultCoords[3*org[j]];
				y += resultCoords[3*org[j]+1];
				z += resultCoords[3*org[j]+2];
			}
			assert( resultCoords[3*i] == 0.45 * x );
			assert( resultCoords[3*i+1] == 0.45 * y );
			assert( resultCoords[3*i+2] == 0.45 * z );
		}else if( etype == RCAP_HEXAHEDRON2 ){
			x = 0.0;
			y = 0.0;
			z = 0.0;
			for(j=0;j<8;++j){
				org[j]-=nodeOffset;
				x += resultCoords[3*org[j]];
				y += resultCoords[3*org[j]+1];
				z += resultCoords[3*org[j]+2];
			}
			assert( resultCoords[3*i] == 0.125 * x );
			assert( resultCoords[3*i+1] == 0.125 * y );
			assert( resultCoords[3*i+2] == 0.125 * z );
		}else if( etype == RCAP_UNKNOWNTYPE ){
			assert( resultCoords[3*i] == coords[3*i] );
			assert( resultCoords[3*i+1] == coords[3*i+1] );
			assert( resultCoords[3*i+2] == coords[3*i+2] );
		}else{
			assert( !"error refined node coordinate" );
		}
	}
	free( resultCoords );

	/* �ו���̐ߓ_�O���[�v�̍X�V */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	printf("Refined Node Group Count %u\n", ng0Count );
	assert( ng0Count == 37 );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);

	org[0] = 1;	org[1] = 13;
	n = rcapGetMiddle( RCAP_SEGMENT, org );
	assert( n != -1 );
	for(j=0;j<ng0Count;++j){
		if( n == result_ng0[j] ){
			n = -1;
			break;
		}
	}
	assert( n == -1 );

	org[0] = 6;	org[1] = 30;
	n = rcapGetMiddle( RCAP_SEGMENT, org );
	assert( n != -1 );
	for(j=0;j<ng0Count;++j){
		if( n == result_ng0[j] ){
			n = -1;
			break;
		}
	}
	assert( n == -1 );
	free( result_ng0 );

	rcapTermRefiner();
	return 0;
}

#endif
