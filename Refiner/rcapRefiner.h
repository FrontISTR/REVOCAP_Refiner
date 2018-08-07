/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Program Name : rcapRefiner                                           #
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
/**
 * \page program_in_out REVOCAP Refiner �v���O�����������R�i���o�͎d�l�V�X�e���j
 *
 * \section function_contract REVOCAP Refiner �̊֐��Ăяo���K��
 *
 * \subsection howto_call_function �֐��̌Ăяo����
 *
 *  REVOCAP Refiner �̓\���o�܂��̓J�b�v���ɑg�ݍ��܂�Ă��̃v���Z�X�̒��Ŏ��s�����i�t�H�[�N���Ȃ��j�B
 *  �V���ȃv���Z�X���N�����ăv���Z�X�ԒʐM�Ńf�[�^�̂���������̂ł͂Ȃ��A
 *  �\���o��J�b�v���̃v���Z�X�̒��ŁA�֐���ʂ��ăf�[�^�̂���������B
 *  �֐��ւ̓��͂�o�͂ƂȂ�ϐ��̃������Ǘ��͌Ăяo�������ӔC�����B
 *  �\���o��J�b�v���֎󂯓n���Ȃ� REVOCAP Refiner �̓����f�[�^��
 *  REVOCAP Refiner ���������Ǘ��ɂ��ĐӔC�����B
 *
 *  REVOCAP Refiner �̃�������Ԃ͌Ăяo�����v���Z�X�̒��ň�ӂł���B
 *  �����v���Z�X�̒��ŕ����� REVOCAP Refiner �̏������s���ꍇ�́A���o�[�W�����ł͖��Ή��ł���B
 *  �������ƏI���𐳂����g��Ȃ��ꍇ�̓f�[�^���j�󂳂��ꍇ������̂Œ��ӂ��K�v�ł���B
 *
 * \subsection name_of_function �֐��̖��O
 *
 *  ���O�� rcapXXX(...) �Ƃ���\n
 *  fortran77 fortran90 �̌Ăяo���̂��߂̊֐����̕ϊ��̓}�N���ł͂Ȃ��A���b�p�֐����o�R���邱�Ƃōs���B
 *  �r���h����ꍇ�ɂ́A�Ăяo�� Fortran �̃R���p�C���̎�ނɂ���ă��C�u�����̃R���p�C������
 *  �قȂ�}�N���萔��^���Ȃ���΂Ȃ�Ȃ��B
 *  \li gfortran �R���p�C���Fgfortran, Intel Fortran, PGI Fortran
 *
 * \subsection howto_allocate_arrays �z��̃������m�ۂ̕��@
 *
 *  �z��̎󂯓n���͌Ăяo������ allocate ���āA���̃��W���[���ł� allocate �� free �������A�v�f�ɑ�����邾���Ƃ���B\n
 *  �z�񂪂��ӂꂽ�ꍇ�̓G���[�R�[�h��Ԃ��B
 *
 * \subsection constants REVOCAP Refiner �ɂ�����萔
 *
 *  �v�f�̌^��\�� RCAP_TETRAHEDRON, RCAP_TETRAHEDRON2, RCAP_HEXAHEDRON �Ȃ�\n
 *  C ����Ŏg���ꍇ�́A���� rcapRefiner.h �� include ����B\n
 *  Fortran ����Ŏg���ꍇ�� rcapRefiner.inc �� include ����B
 *
 * \subsection variable_types REVOCAP Refiner �ɂ�����ϐ��̌^
 *
 *  UNIX �W�� C �� stdint.h ����сA���������_�ɂ��Ă����l�̒�`������B
 *  unsigned �^�� Fortran �Œ�`�ł��Ȃ��ꍇ�����邽�ߎg��Ȃ��B
 *  C ����� size_t �^�̓R���e�i�̑傫��������킷���Ƃ��Ďg�����A
 *  Fortran �ł̓��b�p�[�ŕ����t���ɕϊ����Ďg���B
 *  ���s���Ɉˑ�����\���̂���^�̓v���v���Z�b�T�ŋz������B
 *  \code
 *  #ifdef LINUX64
 *    integer(kind=8) :: nodeCount
 *    integer(kind=8) :: refineNodeCount
 *    integer(kind=8) :: appendNodeCount
 *    integer(kind=8) :: elementCount
 *  #else
 *    integer(kind=4) :: nodeCount
 *    integer(kind=4) :: refineNodeCount
 *    integer(kind=4) :: appendNodeCount
 *    integer(kind=4) :: elementCount
 *  #endif
 *  \endcode
 *  �̂悤�ɒ萔�l�ŃR���p�C�����ɐ؂�ւ��邱�Ƃ��K�v�B
 *
 * \subsection in_out_variables REVOCAP Refiner �̊֐��̓��o�͈���
 *
 *  \li �����͓��͌n��O�ɁA�o�͌n�����ɂ���
 *  \li �z��̓��͂�^���鎞�́A�z��̑傫���Ɛ擪�A�h���X��^����
 *  \li �v�f�̌^�ȂǁA���̏��Ŕz��̑傫�������܂�ꍇ�́A�z��̑傫���͏ȗ�����
 *  \li �o�͂Ŕz���p����ꍇ�́A���炩���� allocate ���ꂽ�z��ɒl�������邱�Ƃōs��
 *  \li �߂�l�̓G���[�����A�o�͂Ŕz���p�����ꍇ�̒l�����������̂��߂Ɏg��
 *
 * \subsection element_node_order REVOCAP Refiner �̗v�f�̐ߓ_�z��̏���
 *
 *  �ו�����v�f�̐ߓ_�z��̏��Ԃ̓\���o�A�J�b�v�����ƂɈقȂ�\�������邪�A
 *  �����ł͊v�V�v���W�F�N�g�ɂ����� REVOCAP �ŗp�������Ԃɏ]�����Ƃɂ���B
 *
 *  �܂��ߓ_�z��̏��Ԃ�A�ʔԍ��̕t�����ɂ��Ă͒萔�z��Ƃ��āA
 *  �\���o�A�J�b�v���A����� Refiner �ŋ��L���A�K�X�Q�Ƃł�����̂Ƃ���B
 *
 */
/*
 * http:
 * �d�l�̏����� (doxgen �ɏ���)
 *  �����̐���   @param[in] @param[out]
 *  �߂�l�̐��� @return
 *  �����̊T�v   @brief
 *  ���Ӎ���     @remarks
 *  \section ���̑�P������ html �ɂ����Ƃ��� name tag �Ɏg����
 */

#ifndef REVOCAP_REFINER_HEADER
#define REVOCAP_REFINER_HEADER

/* �v���|�X�g�ł���`���Ă��邽�߁A�d�����Ȃ��悤�� */
#ifndef REVOCAP_SIZE_DEF
#define REVOCAP_SIZE_DEF

/* for size_t */
#include <stddef.h>

#ifdef WIN32
typedef signed char    int8_t;
typedef unsigned char uint8_t;
typedef short          int16_t;
typedef unsigned short uint16_t;
typedef long          int32_t;
typedef unsigned long uint32_t;
typedef long long     int64_t;
typedef unsigned long long uint64_t;
#else
/* C99 ���瓱������Ă��� */
#include <stdint.h>
#endif

typedef float float32_t;  /* REAL*4 */
typedef double float64_t; /* REAL*8 */

#endif

/* �v�f�̌^��� int8_t */
#define RCAP_UNKNOWNTYPE  -1
#define RCAP_SEGMENT      0
#define RCAP_SEGMENT2     1
#define RCAP_TRIANGLE     2
#define RCAP_TRIANGLE2    3
#define RCAP_QUAD         4
#define RCAP_QUAD2        5
#define RCAP_TETRAHEDRON  6
#define RCAP_TETRAHEDRON2 7
#define RCAP_WEDGE        8
#define RCAP_WEDGE2       9
#define RCAP_PYRAMID      10
#define RCAP_PYRAMID2     11
#define RCAP_HEXAHEDRON   12
#define RCAP_HEXAHEDRON2  13
/* 14 �͗v�f�Ɠ_�̋��E�Ƃ��ē����ŗ\�� */
#define RCAP_VERTEX       15

const int RCAP_TETRAFACES[12] =
{
	1, 2, 3,  /* 0 �Ԗڂ̖� */
	0, 3, 2,  /* 1 �Ԗڂ̖� */
	0, 1, 3,  /* 2 �Ԗڂ̖� */
	0, 2, 1   /* 3 �Ԗڂ̖� */
};

const int RCAP_HEXAFACES[24] =
{
	3, 2, 1, 0,  /* 0 �Ԗڂ̖� */
	4, 5, 6, 7,  /* 1 �Ԗڂ̖� */
	1, 5, 4, 0,  /* 2 �Ԗڂ̖� */
	1, 2, 6, 5,  /* 3 �Ԗڂ̖� */
	3, 7, 6, 2,  /* 4 �Ԗڂ̖� */
	4, 7, 3, 0   /* 5 �Ԗڂ̖� */
};

const int RCAP_PYRAFACES[20] =
{
	0, 1, 2,-1,  /* 0 �Ԗڂ̖� */
	0, 2, 3,-1,  /* 1 �Ԗڂ̖� */
	0, 3, 4,-1,  /* 2 �Ԗڂ̖� */
	0, 4, 1,-1,  /* 3 �Ԗڂ̖� */
	4, 3, 2, 1   /* 4 �Ԗڂ̖� */
};

const int RCAP_WEDGEFACES[20] =
{
	0, 2, 1,-1,  /* 0 �Ԗڂ̖� */
	3, 4, 5,-1,  /* 1 �Ԗڂ̖� */
	0, 1, 4, 3,  /* 2 �Ԗڂ̖� */
	1, 2, 5, 4,  /* 3 �Ԗڂ̖� */
	2, 0, 3, 5   /* 4 �Ԗڂ̖� */
};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Version �������W���o�͂ɏo�͂���B
 */
void rcapGetVersion( void );
/**
 * @brief Refiner �����������A�ߓ_�ԍ��Ɨv�f�ԍ��̃I�t�Z�b�g�l��^����B
 * @param[in] nodeOffset �Ăяo�����̐ߓ_�ԍ��̏����l�iC����Ȃ�0�AFortran����Ȃ�1�Ȃǁj
 * @param[in] elementOffset �Ăяo�����̐ߓ_�ԍ��̏����l�iC����Ȃ�0�AFortran����Ȃ�1�Ȃǁj
 * @note �����̃I�t�Z�b�g�l�����������l��ߓ_�ԍ���v�f�ԍ��ŗ^�����ꍇ�͖����ƂȂ�B
 */
void rcapInitRefiner( int32_t nodeOffset, int32_t elementOffset );
/**
 * @brief Refiner �������ŕێ����Ă��钆�Ԑߓ_�f�[�^�̃L���b�V�����N���A����B
 * @note �ו��𕡐���s���ꍇ�ɁA��U�����ŕێ����Ă���f�[�^���N���A���邽�߂Ɏg���B
 * �Ⴆ�Α�P�i�K�̍ו����I����āA��Q�i�K�̍ו����n�߂�O�ɌĂԁB
 * �����ŃN���A�����f�[�^�͒��Ԑߓ_�̃f�[�^�݂̂ł���A�ߓ_�̍��W�l�A
 * �ו����ɍX�V����ߓ_�O���[�v�̏�񓙂̓N���A����Ȃ��̂Œ��ӂ���B
 */
void rcapClearRefiner( void );
/**
 * @brief Refiner �̏I���������s���B������Ăяo������͂��ׂĂ̏����������ɂȂ�B
 * @note Refiner �ɓo�^�����ߓ_�A�v�f�A���E�����Ȃǂ̃��������������O��
 * Refiner �̏I���������s���Ă��������BRefiner �̏I���������s���O��
 * ���������������ƁA�s���ȃA�h���X�ɃA�N�Z�X����\��������܂��B
 */
void rcapTermRefiner( void );
/**
 * ���ݖ������ł�
 *
 * @brief �`��␳�ɗp���� CAD �t�@�C�����w�肷��B\n
 * �ו��O�̃��b�V���͂��� CAD �t�@�C�����琶������Ă�����̂Ƃ���B
 * ���̊֐����Ă�� CAD �t�@�C�����w�肵�Ȃ������ꍇ�́A�`��␳�͍s��Ȃ��B
 * ���b�V���������̐ߓ_�� globalID �� CAD �̌`��f�[�^�̑Ή����^�����Ă���Ƃ���B
 * �̈敪����̃��b�V���ɑ΂��čו����s���ꍇ�́AsetPartitionFilename �Ȃǂ�
 * globalID �� localID �̑Ή��t����^����K�v������B
 * @param[in] filename �t�@�C����
 */
void rcapSetCADFilename( const char* filename );
/**
 * ���݉������ł��BCAD �Ō`��␳����ꍇ�ɂ͕K�{�B
 *
 * @brief �ߓ_�� globalID �� localID �̑Ή��֌W���L�q�����t�@�C�����w�肷��B\n
 * �w�肵�Ȃ��ꍇ�́AglobalID �� localID �͋�ʂ��Ȃ��B
 * �t�@�C���ł͂Ȃ��A�ߓ_���W��o�^����Ƃ��� globalID �� localID �̊֌W��^���邱�Ƃ��ł���B
 * @param[in] filename �t�@�C����
 * @note �t�@�C���t�H�[�}�b�g�͊v�V�v���W�F�N�g�Ń\���o�ƃJ�v���̊Ԃŗp�������̂ɏ�����B
 */
void rcapSetPartitionFilename( const char* filename );
/**
 * @brief �ߓ_���W�� Refiner �ɗ^����
 * @param[in] num �^����ߓ_�̌�
 * @param[in] coords �ߓ_�̍��W�Adouble �^�̔z��� (x,y,z) �̏��Ԃ� 3*num ���ׂ�����
 * @param[in] localIds �̈敪�������̈���̋Ǐ��ߓ_ID �̔z��
 * @param[in] globalIds ���ߓ_ID �̔z��
 *
 * @note �����l���珇�� num ����ł���ꍇ�� localIds = NULL �ł悢�B
 * Global ID ���t�@�C������^����ꍇ�� globalIds = NULL �ɂ���B
 * setPartitionFilename �ŗ̈敪���̑��ߓ_�ƋǏ��ߓ_�̑Ή����^�����Ă��鎞�ɂ́A
 * globalIds ��^���Ȃ��Ă��悢�B���̊֐��ŗ^�����ꍇ�ɂ́AsetPartitionFilename �ŗ^�����Ή����㏑������B
 * setPartitionFilename �ŗ^���Ȃ��ꍇ�ɁA���̊֐��ł� globalIds ��^���Ȃ��ꍇ�́AlocalId �����̂܂܎g���B
 *
 * ���݂̃o�[�W�����ł� globalIds �͍l�����Ă��܂���B
 *
 * @li setNode64( num, coords, NULL, NULL ) => global �� local �͓����� nodeOffset ���珇�� num ��
 * @li setNode64( num, coords, globalIds, NULL ) => local �� nodeOffset ���珇�� num �Aglobal �͈����ŗ^����
 * @li setNode64( num, coords, NULL, localIds ) => global �� local �͓����� local �͗^����
 * @li setNode64( num, coords, globalIds, localIds ) => global �� local �����ꂼ��^����
 *
 * @note CAD�t�@�C����^�����Ɍ`��␳�����Ȃ��ꍇ�́A�Ǐ��ߓ_�ԍ��Ƒ��ߓ_�ԍ��̑Ή��͕K�v�Ȃ��B
 * @note Fortran ����Ăԏꍇ�ɂ� NULL �A�h���X�̑���ɍŏ��̒l�� nodeOffset �����������z���^����B
 */
void rcapSetNode64( size_t num, float64_t* coords, int32_t* globalIds, int32_t* localIds );
/**
 * @brief �ߓ_���W�� Refiner �ɗ^����
 * @param[in] num �^����ߓ_�̌�
 * @param[in] coords �ߓ_�̍��W�Afloat �^�̔z��� (x,y,z) �̏��Ԃ� 3*num ���ׂ�����
 * @param[in] localIds �̈敪�������̈���̋Ǐ��ߓ_ID �̔z��
 * @param[in] globalIds ���ߓ_ID �̔z��
 *
 * @note �ڍׂ� rcapSetNode64 �ɏ�����
 */
void rcapSetNode32( size_t num, float32_t* coords, int32_t* globalIds, int32_t* localIds );
/**
 * @brief ���� Refiner ���ێ����Ă���ߓ_�̌���Ԃ��B�ו������瑝����B
 * @return �ߓ_�̌�
 */
size_t rcapGetNodeCount( void );
/**
 * @brief Refiner ���Ǘ����Ă���ߓ_���W���擾����
 * @param[in] num �ߓ_�̌�
 * @param[in] localIds �̈敪�������̈���̋Ǐ��ߓ_ID �̔z��
 * @param[out] coords �ߓ_�̍��W�A(x,y,z) �̏��Ԃ� 3*num ���ׂ�����
 *
 * @note RefineElement �̖߂�l�� num �Ƃ��āAresultNodeArray �� localIds �ɓ�����
 * coords �ō��W�l���擾�ł���B\n
 * localIds = {1,5,8} �̏ꍇ
 * coords = {x1,y1,z1,x5,y5,z5,x8,y8,z8} �ƂȂ�B\n
 * localIds �̐ߓ_�ԍ��͎����I�� nodeOffset �ł��炵�� Refiner �ɖ₢���킹��
 */
void rcapGetNode64( size_t num, int32_t* localIds, float64_t* coords );
void rcapGetNode32( size_t num, int32_t* localIds, float32_t* coords );
/**
 * @brief initId ����A������ num �̐ߓ_���W���擾����
 * @param[in] num �ߓ_�̌�
 * @param[in] initId �擾����擪�̐ߓ_�ԍ�
 * @param[out] coords �ߓ_�̍��W�A(x,y,z) �̏��Ԃ� 3*num ���ׂ�����
 * @note coords �� initId �Ԗڂ���������
 */
void rcapGetNodeSeq64( size_t num, size_t initId, float64_t* coords );
void rcapGetNodeSeq32( size_t num, size_t initId, float32_t* coords );
/**
 * @brief �v�f�����ꂼ��ӂ̂Q�������čו�����
 * @param[in] num �v�f�̌�
 * @param[in] etype ���͗v�f�̌^
 * @param[in] nodeArray ���͗v�f�̐ߓ_�z��ietype�^�̗v�f��num���ׂ����́j
 * @param[out] resultNodeArray �ו����ʂ̗v�f�̐ߓ_�z��
 * @return �ו��������ʂ̗v�f�̌�
 *
 * @note resultNodeArray �� NULL �܂��� -1 �Ƃ��ČĂяo���ƁA�ו������ꍇ�̌��������v�Z���ĕԂ��B
 * �������ו������ۂɍs��ꂽ���ǂ����ɂ��Ă͂킩��Ȃ��B\n
 * �Ⴆ�΂S�ʑ̂Ȃ�Γ��͗v�f�̌��̂W�{��Ԃ��B\n
 *
 * @code
 * etype = TETRAHEDRON;
 * num = 5;
 * nodeArray = new int32_t[4*5];
 * resultNodeArray = new int32_t[4*5*8];
 *
 * refineElement( &num, &etype, nodeArray, resultNodeArray )
 * @endcode
 *
 * @note �����̗v�f�̌^���������Ă���ꍇ�́A�����^�̗v�f���܂Ƃ߂Ă��̊֐���v�f�̌^���Ƃɕ�����ĂԂ��A
 * �ߓ_�z��ɂǂ̌^�̗v�f�����Ԃɓ����Ă��邩�̔z����쐬���� refineElementMulti �֐����ĂԁB\n
 *
 * @code
 * etype = TETRAHEDRON;
 * num = 5;
 * nodeArrayTetra = new int32_t[4*5];
 * resultNodeArrayTetra = new int32_t[4*5*8];
 *
 * refineElement( &num, &etype, nodeArrayTetra, resultNodeArrayTetra )
 * etype = HEXAHEDRON;
 * num = 5;
 * nodeArrayHex = new int32_t[8*5];
 * resultNodeArrayHex = new int32_t[8*5*8];
 *
 * refineElement( &num, &etype, nodeArrayHex, resultNodeArrayHex )
 * @endcode
 *
 * @note �v�f���ו�������ɁA���̗v�f�̖ʂ�ӂ��O�p�`�v�f������v�f�Ƃ݂Ȃ��Ă��̊֐����Ăяo���ƁA
 * �v�f���ו��������ʂ�ʂ�ӂɐ����������ʂ�Ԃ��B�t�ɗv�f�̖ʂ�ӂ̍ו����Ăяo������ŁA
 * �v�f�̍ו����s���ƁA��ɍו������ʂ�ӂɂ��Ă͂��̐ߓ_�ԍ����g���B
 *
 * @note rcapClearRefiner ���ĂԂ܂ł� nodeArray resultNodeArray ��������Ȃ��ł��������B
 *
 * @note �l�ʑ̗v�f�̍ו���̂W�̗v�f�̌���鏇��
 *
 *  - (1)�l�ʑ̂�1�Ԗڂ̐ߓ_��������v�f
 *  - (2)�l�ʑ̂�2�Ԗڂ̐ߓ_��������v�f
 *  - (3)�l�ʑ̂�3�Ԗڂ̐ߓ_��������v�f
 *  - (4)�l�ʑ̂�4�Ԗڂ̐ߓ_��������v�f
 *  - (5)�l�ʑ̂�1�Ԗڂ̖ʂ�������v�f
 *  - (6)�l�ʑ̂�2�Ԗڂ̖ʂ�������v�f
 *  - (7)�l�ʑ̂�3�Ԗڂ̖ʂ�������v�f
 *  - (8)�l�ʑ̂�4�Ԗڂ̖ʂ�������v�f
 *
 * �O����4�̖ʔԍ��͍ו��O�̗v�f�Ɠ�������
 * �㔼��4�̖ʔԍ���0�Ԗڂ��ו��O�̗v�f�̊O���̌���
 *
 * @note �Z�ʑ̗v�f�̍ו���̂W�̗v�f�̌���鏇��
 *  - (1)�Z�ʑ̂�1�Ԗڂ̐ߓ_��������v�f
 *  - (2)�Z�ʑ̂�2�Ԗڂ̐ߓ_��������v�f
 *  - (3)�Z�ʑ̂�3�Ԗڂ̐ߓ_��������v�f
 *  - (4)�Z�ʑ̂�4�Ԗڂ̐ߓ_��������v�f
 *  - (5)�Z�ʑ̂�5�Ԗڂ̐ߓ_��������v�f
 *  - (6)�Z�ʑ̂�6�Ԗڂ̐ߓ_��������v�f
 *  - (7)�Z�ʑ̂�7�Ԗڂ̐ߓ_��������v�f
 *  - (8)�Z�ʑ̂�8�Ԗڂ̐ߓ_��������v�f
 *
 * �ʔԍ��͍ו��O�̗v�f�Ɠ�������
 *
 * @note �O�p���v�f�̍ו����8�̗v�f�̌���鏇��
 *
 *  - (1)�O�p����1�Ԗڂ̐ߓ_��������v�f
 *  - (2)�O�p����2�Ԗڂ̐ߓ_��������v�f
 *  - (3)�O�p����3�Ԗڂ̐ߓ_��������v�f
 *  - (4)�O�p����4�Ԗڂ̐ߓ_��������v�f
 *  - (5)�O�p����5�Ԗڂ̐ߓ_��������v�f
 *  - (6)�O�p����6�Ԗڂ̐ߓ_��������v�f
 *  - (7)�O�p����0,1,2�Ԗڂ̐ߓ_�ɂ��O�p�`�ɐڂ���v�f
 *  - (8)�O�p����3,4,5�Ԗڂ̐ߓ_�ɂ��O�p�`�ɐڂ���v�f
 *
 * �͂��߂�6�̖ʔԍ��͍ו��O�̗v�f�Ɠ�������
 * �Ō��2�̖ʔԍ���0�Ԗڂ��ו��O�̗v�f�̊O���̌���
 *
 * @note �l�p���v�f�̍ו����10�̗v�f�̌���鏇��
 *
 *  - (1)�l�p����1�Ԗڂ̐ߓ_��������v�f
 *  - (2)�l�p����2�Ԗڂ̐ߓ_��������v�f
 *  - (3)�l�p����3�Ԗڂ̐ߓ_��������v�f
 *  - (4)�l�p����4�Ԗڂ̐ߓ_��������v�f
 *  - (5)�l�p����5�Ԗڂ̐ߓ_��������v�f
 *  - (6)�l�p����0,1,2�Ԗڂ̐ߓ_�ɂ��O�p�`�ɐڂ���l�ʑ̗v�f
 *  - (7)�l�p����0,2,3�Ԗڂ̐ߓ_�ɂ��O�p�`�ɐڂ���l�ʑ̗v�f
 *  - (8)�l�p����0,3,4�Ԗڂ̐ߓ_�ɂ��O�p�`�ɐڂ���l�ʑ̗v�f
 *  - (9)�l�p����0,4,1�Ԗڂ̐ߓ_�ɂ��O�p�`�ɐڂ���l�ʑ̗v�f
 *  - (10)�l�p���̒��ɖ��ߍ��܂�Ă���v�f
 *
 * �͂��߂�5�̖ʔԍ��͍ו��O�̗v�f�Ɠ�������
 * ����4�̖ʔԍ��͍ו��O�̎l�p���̖ʔԍ��ƁA�l�ʑ̗v�f�̊O�Ɍ����Ă���ʔԍ�����v�������
 * �Ō��1�͑S�Ă̖ʂ����ߍ��܂�Ă���
 */
size_t rcapRefineElement( size_t num, int8_t etype, int32_t* nodeArray, int32_t* resultNodeArray );
/**
 * @brief �����̎�ނ̌^�����݂��Ă��郂�f������x�ɍו�����
 * @param[in] num �v�f�̌�
 * @param[in] etypeArray ���͗v�f�̌^�̔z��
 * @param[in] nodeArray ���͗v�f�̐ߓ_�z��
 * @param[in/out] refinedNum �ו���̗v�f�̌�
 * @param[out] resultEtypeArray �ו����ʂ̌^�̐ߓ_�z��
 * @param[out] resultNodeArray �ו����ʂ̗v�f�̐ߓ_�z��
 * @return �ו��������ʂ��i�[����̂ɕK�v�Ȑߓ_�z��̑傫��
 *
 * @note resultNodeArray �� NULL �܂��� -1 �Ƃ��ČĂяo���ƁA
 * �ו��������ʂ��i�[����̂ɕK�v�Ȑߓ_�z��̑傫����Ԃ��B\n
 * refinedNum �ɍו���̗v�f�̌���������B
 * �]���āAresultNodeArray = NULL �ŌĂяo�������ʂ̖߂�l�̑傫����
 * �z�� resultNodeArray ����������Ɋm�ۂ���B
 * resultEtypeArray �̔z��̑傫���� refinedNum �ɑ�����ꂽ�l���g���B
 *
 * @note ���ۂɍו����s���Ƃ��ɂ́A�z�� resultNodeArray �ɒl�����đ�����ČĂяo���B
 * ���̏ꍇ�̖߂�l���ו��̌��ʂɕK�v�Ȑߓ_�z��̑傫����Ԃ��B
 *
 * @note resultEtypeArray �� NULL �����Ă��ו��̎��s�͉\�B
 * �z�������ƁA�ו������v�f�̌^�������ĕԂ��B
 *
 * @note �v�f�̌^���Ƃɐߓ_�z����܂Ƃ߂�� rcapRefineElement �œ����̂��Ƃ����s�ł���
 *
 * @note rcapClearRefiner ���ĂԂ܂ł� etypeArray, nodeArray, resultETypeArray, resultNodeArray ��������Ȃ��ł��������B
 *
 * @remark *** �߂�l�̎d�l�� 2010/2/9 �o�[�W��������ύX���Ă��܂� ***
 */
size_t rcapRefineElementMulti( size_t num, int8_t* etypeArray, int32_t* nodeArray, size_t* refinedNum, int8_t* resultEtypeArray, int32_t* resultNodeArray );
/**
 * @brief
 * rcapRefineElement �ɂ��ו����ꂽ�f�[�^�i�ߓ_�O���[�v�A�v�f�O���[�v�A�ʃO���[�v�j���R�~�b�g����B
 * ���Ȃ킿�A�ȉ��� rcapGet[Node|Element|Face]Group[Count] ���\�b�h�̑Ώۂ��ו��O�̃f�[�^����
 * �ו���̃f�[�^�ɕς���B�ו��O�̃f�[�^�͍폜�����B
 * ���̊֐������s��� rcapRefineElement ���ēx���s�����ꍇ�A�X�V�����f�[�^��
 * �ו���̃f�[�^�ɂȂ�B
 * �܂��ArcapRefineElement �ōו���̗v�f�ɕt�^�����v�f�ԍ��� elementOffset �l�Ƀ��Z�b�g�����B
 * @remark
 * rcapRefineElement �𕡐���Ăԏꍇ�́A������Ă񂾌�Ɉ�x���� rcapCommit �����s����B
 * �Q�i�K�̍ו����s���ꍇ�́A�P�i�K�ڂ̍ו� rcapRefineElement ���Ă񂾌�A
 * �Q�i�K�ڂ� rcapRefineElement ���ĂԑO�Ɏ��s����B
 * @remark
 * rcapAppendBoundaryNodeGroup�ArcapAppendBoundaryNodeVariableXXX �Œǉ����ꂽ���E�����ɂ��Ă�
 * ���̊֐����Ă΂ꂽ���_�œo�^����Ă���v�f�̋��E�ʂ𒊏o���āA���̖ʂŋ��E�������X�V����B
 * ��K�͕��G���f���̂����̏������X�V����ꍇ�́A���E�ʂ̒��o�Ɏ��Ԃƃ�������v����ꍇ������B
 */
void rcapCommit(void);

/**
 * @brief �ו��Ɠ����ɍX�V����ߓ_�O���[�v��o�^
 * @param[in] dataname �ߓ_�O���[�v�̎��ʎq
 * @param[in] num �ߓ_�O���[�v�̐ߓ_��
 * @param[in] nodeArray �ߓ_�O���[�v�̐ߓ_
 * @note refineElement ���Ăяo���O�ɂ��̊֐��Őߓ_�O���[�v��o�^���Ă����ƁArefineElement ���Ăяo�������ɁA
 * �ו�����v�f�̕ӂ̗��[���ߓ_�O���[�v�Ɋ܂܂��Ȃ�΁A�ו��ɂ���Đ������ꂽ�ӂ̒��_�̐ߓ_��
 * �ߓ_�O���[�v�ɒǉ�����B
 * ���ʎq�͕�����ł��BFortran ����Ăяo���ꍇ�́A������̍Ō�Ƀk��������ǉ����Ă��������B
 * @code
 * int32_t cl[2] = {1,2};
 * appendNodeGroup("CL",2,cl);
 * refineElement( num, etype, nodeArray, resultNodeArray );
 * rcapCommit();
 * size_t s = getNodeGroupCount("CL");
 * int32_t* resultcl = new int_32[s];
 * getNodeGroup("CL",s,resultcl);
 * @endcode
 */
void rcapAppendNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );
/**
 * @brief Refiner �ɓo�^����Ă���ߓ_�O���[�v�̐ߓ_�̌���Ԃ�
 * @param[in] dataname �ߓ_�O���[�v�̎��ʎq
 * @return �ߓ_�O���[�v�̐ߓ_�̌�
 * @note refineElement ���ĂԑO�́AappendNodeGroup �œo�^���������̂��̂�Ԃ��B
 */
size_t rcapGetNodeGroupCount( const char dataname[80] );
/**
 * @brief Refiner �ɓo�^����Ă���ߓ_�O���[�v��Ԃ�
 * @param[in] dataname �ߓ_�O���[�v�̎��ʎq
 * @param[in] num �擾����ߓ_�O���[�v�̌�
 * @param[out] nodeArray �ߓ_�O���[�v�̐ߓ_
 * @note ���� num �� getNodeGroupCount �Ŏ擾�����l��^���āAnodeArray �͂��炩���� num ��
 * �傫���̔z��ł��炩���� allocate ���Ă�����̂Ƃ���B
 */
void rcapGetNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );

/**
 * @brief BoundaryNodeGroup �Ƃ́A���E�ʏ�ɂ݂̂���ߓ_�O���[�v�̂��Ƃōו��Ɠ����ɍX�V����B
 * @param[in] dataname ���E�ߓ_�O���[�v�̎��ʎq
 * @param[in] num �擾���鋫�E�ߓ_�O���[�v�̌�
 * @param[out] nodeArray ���E�ߓ_�O���[�v�̐ߓ_
 * @note rcapGetBoundaryNodeGroup ����Ƃ��́A���炩���� nodeArray �� allocate ���Ă������ƁB
 * @note refineElement ������Ƃ��ɁA���̃^�C�v�̋��E���������鎞�́A�\�ʒ��o���s���A�\�ʂ̎O�p�`�܂��͎l�p�`��
 * �g���ċ��E�������X�V����B
 */
void rcapAppendBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );
size_t rcapGetBNodeGroupCount( const char dataname[80] );
void rcapGetBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );

/**
 * @brief BoundaryNodeVariableInt �Ƃ́A���E�ʏ�ɂ݂̂���ߓ_��̐����l�ϐ��̂���
 * @param[in] dataname ���E�ߓ_�ϐ��̎��ʎq
 * @param[in] num �擾���鋫�E�ߓ_�ϐ��́i�ߓ_�́j��
 * @param[out] nodeArray ���E�ߓ_�ϐ��̐ߓ_
 * @param[out] nodeVars ���E�ߓ_�ϐ��̕ϐ��inodeArray�Ɠ����傫���j
 * @note rcapGetBoundaryNodeVariableInt ����Ƃ��́A���炩���� nodeArray �� nodeVars �� allocate ���Ă������ƁB
 * @note refineElement ������Ƃ��ɁA���̃^�C�v�̋��E���������鎞�́A�\�ʒ��o���s���A�\�ʂ̎O�p�`�܂��͎l�p�`��
 * �g���ċ��E�������X�V����B
 * @note �ו��Ő����钆�Ԑߓ_�ɂ��āA
 * ���Ƃ̐ߓ_�̂Ȃ��ŕϐ����^�����Ă��Ȃ��ߓ_������΁A���Ԑߓ_�ɂ͕ϐ��͗^���Ȃ��B
 * ���Ƃ̐ߓ_�ɕϐ����^�����Ă��āA�ϐ��̒l�����ׂē��������́A���Ԑߓ_�ɂ��̓������l��^����B
 * ���Ƃ̐ߓ_�ɕϐ����^�����Ă��āA�ϐ��̒l���قȂ鎞�́A���Ԑߓ_�ɍł��������l��^����B
 */
void rcapAppendBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars );
size_t rcapGetBNodeVarIntCount( const char dataname[80] );
void rcapGetBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars  );

/**
 * @brief ElementGroup �Ƃ́A�v�f�ԍ��̏W���̂��ƁB
 * @param[in] dataname �v�f�O���[�v�̎��ʎq
 * @param[in] num �擾����v�f�O���[�v�̌�
 * @param[out] elementArray �v�f�O���[�v�̔z��
 * @note rcapGetElementGroup ����Ƃ��́A���炩���� elementArray �� allocate ���Ă������ƁB
 */
void rcapAppendElementGroup( const char dataname[80], size_t num, int32_t* elementArray );
size_t rcapGetElementGroupCount( const char dataname[80] );
void rcapGetElementGroup( const char dataname[80], size_t num, int32_t* elementArray );

/**
 * @brief FaceGroup �Ƃ́A�v�f�ԍ��A�v�f���ʔԍ��̑g�̂��ƁB�A���ʂ��ו�����ꍇ�Ȃǂɗp����B
 * @param[in] dataname �ʃO���[�v�̎��ʎq
 * @param[in] num �擾����ʃO���[�v�̌��i�v�f�ԍ��Ɩʔԍ��̑g�̌��j
 * @param[out] faceArray �ʃO���[�v�̗v�f�ԍ��A�ʔԍ��̑g�̔z��B
 * @note faceArray �͗v�f�ԍ��Ɩʔԍ������݂ɕ��ׂ� 2*num �̔z��ɂȂ�B
 */
void rcapAppendFaceGroup( const char dataname[80], size_t num, int32_t* faceArray );
size_t rcapGetFaceGroupCount( const char dataname[80] );
void rcapGetFaceGroup( const char dataname[80], size_t num, int32_t* faceArray );

/**
 * @brief NodeVariable ��o�^�����Ƃ��ɁA���Ԑߓ_�ɗ^����l�̌��ߕ���I�����܂��B
 * ���݂� "MIN" "MAX" "MIDDLE" ��3��ނɑΉ����Ă��܂��B
 * MIN �͒��Ԑߓ_�𐶐�����̂ɗp�����ߓ_��̒l�̍ŏ��l��^���܂��B
 * MAX �͒��Ԑߓ_�𐶐�����̂ɗp�����ߓ_��̒l�̍ő�l��^���܂��B
 * MIDDLE �͒��Ԑߓ_�𐶐�����̂ɗp�����ߓ_��̒l�̕��ϒl��^���܂��B
 */
void rcapSetInterpolateMode( const char mode[32] );
void rcapGetInterpolateMode( char mode[32] );

/**
 *
 * @brief ���Ԑߓ_����A����𐶐�����̂Ɏg�����ӁA�ʁA�v�f�̐ߓ_�z���Ԃ�
 * @param[in] localNodeId �ו����ꂽ���Ԑߓ_�̋Ǐ��ߓ_�ԍ�
 * @param[out] originalNodeArray localNodeId �𐶐�����̂Ɏg�����v�f�̐ߓ_�z���Ԃ��܂�
 * @return �߂�l�͗v�f�̌^
 * @note ��F�ӂ̒��_�̏ꍇ�͐ߓ_�z��ɂQ�l�������� RCAP_SEGMENT ��Ԃ��BlocalNodeId �ɍו��Ő������ꂽ�_
 * �łȂ��_��^�����ꍇ�́A���������� -1 = RCAP_UNKNOWNTYPE ��Ԃ��܂��B
 *       ��F�l�p�`�̒��S�̏ꍇ�́A�ߓ_�z��ɂS�l�������� RCAP_QUAD ��Ԃ��B
 *
 * @note ���̊֐����Ăԏꍇ�� rcapRefineElement �ŗ^�����ו��O�̗v�f�̐ߓ_�z�� nodeArray
 * ����� rcapRefineElementMulti �ŗ^�����ו��O�̗v�f�̗v�f�^�C�v�z�� etypeArray �� �ߓ_�z�� nodeArray
 * �̃�������������Ȃ��ł��������B�ʂ̒l�ŏ㏑�����邱�Ƃ���߂Ă��������B
 * �i���̊֐��̓����ł��̐ߓ_�z����Q�Ƃ��Ă��邽�߁j
 *
 */
int8_t rcapGetOriginal( int32_t localNodeId, int32_t* originalNodeArray );

/**
 * @brief �ӁA�ʁA�v�f��^���āA���ꂩ����ꂽ���Ԑߓ_��߂�l�ŕԂ�
 * @return ���Ԑߓ_�̐ߓ_�ԍ�
 *
 * ���ɍו����s��ꂽ��ł�
 *
 * \code
 * middle = rcapGetMiddle( RCAP_SEGMENT, nodeArray );
 * \endcode
 *
 * ��
 *
 * \code
 * rcapRefineElement( 1, RCAP_SEGMENT, nodeArray, resultArray );
 * middle = resultArray[2];
 * \endcode
 *
 * �͓����ł��B�������A�ו����s���Ă��Ȃ��ꍇ�i���Ԑߓ_�����݂��Ȃ��ꍇ�j��
 * rcapGetMiddle �͖����Ȑߓ_�ԍ�(-1)��Ԃ��܂����A
 * rcapRefineElement �͍ו������Ă��炻�̌��ʂ�Ԃ��܂��B
 */
int32_t rcapGetMiddle( int8_t etype, int32_t* originalNodeArray );
/**
 * @brief �ו���̎��R���W�̕ϊ�
 */
int32_t rcapGetRefinedNaturalCoord( int32_t* elementId, float32_t* coords, int32_t* refinedElementId, float32_t* refinedCoords );

/**
 * @breif �f�o�b�O�p�t�@�C�����o�̓��[�`��
 * @return �ǂݍ��񂾁A�܂��͏������񂾗v�f�̌�
 *
 * File ���� File �ւ̍ו���
 * rcapInitRefiner(0,0);
 * rcapLoadGFFile( "MESH", "BOUN" );
 * rcapRefineFFbModel();
 * rcapSaveGFFile( "REFINEMESH", "REFINEBOUN" );
 * rcapTermRefiner();
 *
 * @remark �����ŏo�͂���f�[�^�� rcapRefinerDoc.mesh �Ɋi�[���ꂽ����
 * �Ȃ̂ŁA�ו���̃��b�V�����o�͂ł���킯�ł͂Ȃ��B
 */
int32_t rcapLoadGFFile( const char* gffile, const char* bounfile );
void rcapRefineFFbModel();
int32_t rcapSaveGFFile( const char* gffile, const char* bounfile );
int32_t rcapLoadHECFile( const char* hecfile );
int32_t rcapSaveHECFile( const char* hecfile );
int32_t rcapSaveRNFFile( const char* rnffile );

/* rcapxxx_  ���ׂď����� */
/* gfortran, intel fortran, pgi fortran �͂���ł悢 */
#ifdef FORTRAN90
void racpgetversion_( void );
void rcapinitrefiner_( int32_t* nodeOffset, int32_t* elementOffset );
void rcapclearrefiner_( void );
void rcaptermrefiner_( void );
void rcapsetcadfilename_( const char* filename );
void rcapsetpartitionfilename_( const char* filename );

void rcapsetnode64_( int32_t* num, float64_t* coords, int32_t* globalIds, int32_t* localIds );
void rcapsetnode32_( int32_t* num, float32_t* coords, int32_t* globalIds, int32_t* localIds );
int32_t rcapgetnodecount_( void );
void rcapgetnode64_( int32_t* num, int32_t* localIds, float64_t* coords );
void rcapgetnode32_( int32_t* num, int32_t* localIds, float32_t* coords );
void rcapgetnodeseq64_( int32_t* num, int32_t* initId, float64_t* coords );
void rcapgetnodeseq32_( int32_t* num, int32_t* initId, float32_t* coords );

int32_t rcaprefineelement_( int32_t* num, int8_t* etype, int32_t* nodeArray, int32_t* resultNodeArray );
int32_t rcaprefineelementmulti_( int32_t* num, int8_t* etypeArray, int32_t* nodeArray, int32_t* refinedNum, int8_t* resultEtypeArray, int32_t* resultNodeArray );
void rcapcommit_( void );

void rcapappendnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray );
int32_t rcapgetnodegroupcount_( const char dataname[80] );
void rcapgetnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray );

void rcapappendbnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray );
int32_t rcapgetbnodegroupcount_( const char dataname[80] );
void rcapgetbnodegroup_( const char dataname[80], int32_t* num, int32_t* nodeArray );

void rcapappendbnodevarint_( const char dataname[80], int32_t* num, int32_t* nodeArray, int32_t* nodeVars );
int32_t rcapgetbnodevarintcount_( const char dataname[80] );
void rcapgetbnodevarint_( const char dataname[80], int32_t* num, int32_t* nodeArray, int32_t* nodeVars  );

void rcapappendelementgroup_( const char dataname[80], int32_t* num, int32_t* elementArray );
int32_t rcapgetelementgroupcount_( const char dataname[80] );
void rcapgetelementgroup_( const char dataname[80], int32_t* num, int32_t* elementArray );

void rcapappendfacegroup_( const char dataname[80], int32_t* num, int32_t* faceArray );
int32_t rcapgetfacegroupcount_( const char dataname[80] );
void rcapgetfacegroup_( const char dataname[80], int32_t* num, int32_t* faceArray );

int8_t rcapgetoriginal_( int32_t* localNodeId, int32_t* originalNodeArray );
int32_t rcapgetmiddle_( int8_t *etype, int32_t* originalNodeArray );

void rcapsetinterpolatemode_( const char mode[32] );
void rcapgetinterpolatemode_( char mode[32] );

int32_t rcaploadgffile_( const char* gffile, const char* bounfile );
int32_t rcaploadhecfile_( const char* hecfile );
int32_t rcapsavegffile_( const char* gffile, const char* bounfile );
int32_t rcapsavehecfile_( const char* hecfile );
int32_t rcapsavernffile_( const char* rnffile );
#endif

#ifdef FORTRAN77
#endif

#ifdef G95
#endif

#ifdef INTEL_FORTRAN
#endif

#ifdef PG_FORTRAN
#endif

#ifdef __cplusplus
}
#endif

#endif
