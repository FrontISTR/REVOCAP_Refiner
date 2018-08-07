/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program By C Language                                         #
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
 * gcc -D_CONSOLE main.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * �T���v�����s�ၕ�e�X�g�p�v���O����
 *
 */
/**
 * \page tutorial 4 REVOCAP Refiner �`���[�g���A��
 * \section init_and_term �N���ƏI��
 *
 *  REVOCAP Refiner �̋N���� rcapInitRefiner �֐��ōs���A�I���� rcapTermRefiner �֐��ōs���܂��B
 *  REVOCAP Refiner �̂��ׂĂ̏����� rcapInitRefiner ��ǂ񂾌�ArcapTermRefiner ���Ă΂��O�ŗL���ł��B
 *  ����ȊO�̂Ƃ���� REVOCAP Refiner �̏������Ă�ł������������s��Ȃ����A�G���[�R�[�h��Ԃ��܂��B
 *
 * \code
 * #include "rcapRefiner.h"
 * int main(void){
 *   rcapInitRefiner(0,0);
 *   // ������ Refiner �̏������L�q����
 *   rcapTermRefiner();
 *   return 0;
 * }
 * \endcode
 *
 *  Fortran ����ł͎��̂悤�ɂȂ�܂��B
 *
 * \code
 * program RefinerSample
 *   implicit none
 *   integer, parameter :: RCAP_SIZE_T = 8
 *   include "rcapRefiner.inc"
 *   call rcapInitRefiner(1,1)
 *   ! ������ Refiner �̏������L�q����
 *   call rcapTermRefiner()
 * end program RefinerSample
 * \endcode
 *
 *  rcapInitRefiner �̈����́A�ߓ_�ԍ�����їv�f�ԍ��̊J�n�ԍ�(nodeOffset,elementOffset)�Ƃ��܂��B
 *
 *  �ȉ��ł͂��ׂăT���v���� Fortran ����Ƃ��܂��B
 *
 * \section set_model ���f���̓o�^
 *
 *  REVOCAP Refiner �ւ́A�ߓ_�f�[�^�A�v�f�f�[�^�A���E�����f�[�^��o�^���邱�Ƃ��ł��܂��B
 *  �o�^���Ă���f�[�^�͗v�f�̍ו��̎��Ɏ����I�ɍX�V����܂��B
 *
 *  CAD �f�[�^������o�����Ȗʏ���p���Č`��␳���s���ꍇ�́A
 *  �ו�����O�̃��f���̃��b�V���������s���� CAD �f�[�^�ƁA
 *  �Ǐ��ߓ_�ԍ��Ƒ��ߓ_�ԍ��̑Ή���^����K�v������܂��B
 *  ���݂͖��Ή��ł��B
 *
 *  ���ݑΉ����Ă��郂�f���̃f�[�^�͈ȉ��̒ʂ�ł��B
 *
 *  \li �ߓ_���W
 *  \li �l�ʑ̂P���v�f�A�Q���v�f
 *  \li �Z�ʑ̂P���v�f�A�Q���v�f
 *  \li �O�p���P���v�f�A�Q���v�f
 *  \li �l�p���P���v�f�A�Q���v�f
 *  \li �O�p�`�P���v�f�A�Q���v�f
 *  \li �l�p�`�P���v�f�A�Q���v�f
 *  \li �����P���v�f�A�Q���v�f
 *  \li �ߓ_�O���[�v
 *  \li �v�f�O���[�v
 *  \li �ʁi�v�f�ԍ��Ɨv�f���ʔԍ��̑g�j�O���[�v
 *  \li ���E�ߓ_�O���[�v
 *  \li ���E�ߓ_�����l
 *
 *  �ߓ_�ƂS�ʑ̂P���v�f��o�^���čו�����ɂ͎��̂悤�ɂ��܂��B
 *
 * \code
 * program RefinerSample
 *   implicit none
 *   integer, parameter :: RCAP_SIZE_T = 8
 *   include "rcapRefiner.inc"
 *   double precision :: coords(30)
 *   integer(kind=4), dimension(4) :: tetras
 *   integer(kind=4), dimension(32) :: refineTetras
 *   integer(kind=4) :: nodeCount
 *   integer(kind=4) :: elementCount
 *   coords = (/&
 *     & 0.0, 0.0, 0.0, &
 *     & 1.0, 0.0, 0.0, &
 *     & 0.0, 1.0, 0.0, &
 *     & 0.0, 0.0, 1.0, &
 *     & (0.0, I=13, 30) /)
 *   tetras = (/&
 *     & 1, 2, 3, 4, /)
 *   nodeCount = 4
 *   elementCount = 1
 *   call rcapInitRefiner(1,1)
 *   call rcapSetNode64( nodeCount, coords, 0, 0 )
 *   call rcapRefineElement( elementCount, RCAP_TETRAHEDRON, tetras, refineTetras )
 *   call rcapCommit()
 *   call rcapTermRefiner()
 * end program RefinerSample
 * \endcode
 *
 * Fortran �ł� NULL �A�h���X�̑���̑���� -1 �������邱�Ƃɒ��ӂ��܂��B
 *
 * \section howto_refine �v�f�̍ו��i�P��v�f�j
 *
 *  REVOCAP Refiner �� FEM ���f���̗v�f�̍ו����s���ɂ́ArcapRefineElement �֐����g���܂��B
 *  ���̊֐��͍ו��������v�f�̌^�i�S�ʑ̂P���v�f�Ȃǁj�ƁA�v�f�̌��A�v�f�̐ߓ_�z���^���āA
 *  �ו������v�f�̐ߓ_�z����擾���܂��B
 *  �ו������v�f�̐ߓ_�z��͂��炩���ߌĂяo�����Ń������Ɋm�ۂ��Ă����K�v������܂��B\n
 *
 *  �ו���̐ߓ_�z��̌����擾����ɂ́A�f�[�^�擾�p�̐ߓ_�z��� NULL �Ƃ��邩�A
 *  �ŏ��̐ߓ_�ԍ��� -1 �����ČĂяo���΁A����Ԃ��܂��B\n
 *
 *  rcapRefineElement �� rcapTermRefiner �����s����܂ŉ��x���Ăяo�����Ƃ��ł��܂��B
 *  rcapRefineElement ���Ăяo���ꍇ�́A�����ōו�������v�f�̐ߓ_�̍��W�� rcapSetNode32 �܂���
 *  rcapSetNode64 �ł��炩���ߓo�^���Ă����K�v������܂��B
 *  �ߓ_�̍��W�͗v�f���ו�����Ƃ��ɍו��̃g�|���W�[�𔻒肷�鎞�̍ޗ��Ƃ��Ďg���܂��B\n
 *
 *  �ŏ��ɂS�ʑ̂���Ȃ�v�f���ו����A����
 *  ���̂S�ʑ̂̋��E�̂R�p�`����Ȃ�v�f�ɂ��� rcapRefineElement ���Ăяo�����ꍇ�ɂ́A
 *  �R�p�`�̍ו��ɂ�钆�Ԑߓ_�̐ߓ_�ԍ��́A�S�ʑ̂̍ו��̎��ɕt�^���ꂽ���Ԑߓ_�̐ߓ_�ԍ����g���܂��B
 *
 *  �ŏ��ɂS�ʑ̂���Ȃ�v�f�ɂ��� rcapRefineElement ���Ăяo���čו����A
 *  ����ɂ��̌��ʂł���ו����ꂽ�S�ʑ̂ɑ΂��ArcapRefineElement ���Ăяo�����ꍇ�ɂ́A
 *  �Q�i�K�̍ו����Ȃ���܂��B�������A���E�����𓯎��ɍX�V����ꍇ�ɂQ�i�K�̍ו����s���ꍇ�́A
 *  �P�i�K�ڂ̍ו����I������Ƃ��� rcapCommit �����s���Ă��������B�ڍׂ͎��͂ɋL�q���Ă���܂��B
 *
 * \section howto_refine_multi �v�f�̍ו��i�����v�f�j
 *
 *  rcapRefineElement �֐��͗v�f�̌^�A���A�ߓ_�z���^���邽�߁A�v�f�̎�ނ͑S�ē����ł���K�v������܂��B
 *  �����v�f�� FEM ���f�����ו����邽�߂ɂ́A����2�ʂ�̕��@������܂��B
 *
 *  \li �Ăяo�����ŗv�f�̌^���Ƃɂ܂Ƃ߂āArcapRefineElement �𕡐���Ă�
 *  \li rcapRefineElementMulti ���g��
 *
 *  ��҂� rcapRefineElementMulti ���g���ꍇ�͐ߓ_�z��͕����̎�ނ̗v�f���܂ނ��߁A
 *  ���Ԗڂɂǂ̎�ނ̗v�f�����L�q�����z���ʂɗp�ӂ���K�v������܂��B����� eTypeArray ��
 *  �i�[���ČĂяo���Ă��������B�߂�l�������̎�ނ̗v�f�����݂����ߓ_�z��ƂȂ邽�߁A
 *  �߂�l�̗v�f�̎�ނ��i�[����z���^����K�v������܂��B\n
 *
 *  rcapRefineElement �Ŕz��� NULL �܂��͍ŏ��̐ߓ_�ԍ��� -1 �Ƃ��ė^�������ɂ́A
 *  �ו���̗v�f�̌���Ԃ������A���̊֐��ł͍ו���̗v�f�̌��� refinedNum ��
 *  �^�����邽�߁A�߂�l�͍ו��������ʂ��i�[����̂ɕK�v�Ȑߓ_�z��̑傫����Ԃ��܂��B\n
 *
 *  ��x resultNodeArray �� NULL �܂��͍ŏ��̐����� -1 �Ƃ��ČĂяo���΁A
 *  �ו��ɕK�v�Ȑߓ_�z��̑傫���ƁA�v�f�̌���������̂ŁA
 *  �߂�l���i�[����z�� resultEtypeArray �� resultNodeArray
 *  �̑傫�����m�F���Ă���ēx�Ăяo�����Ƃ��ł��܂��B
 *
 * \section update_conditions ���E������v�f�̍ו����ɓ����ɍX�V����
 *
 *  REVOCAP_Refiner �ł� FEM ���f���̗v�f�̍ו����ɁA���E�����𓯎��ɍX�V���邱�Ƃ��ł��܂��B
 *  ���ݑΉ����Ă��鋫�E������
 *
 *  \li �ߓ_�O���[�v
 *  \li �v�f�O���[�v
 *  \li �ʁi�v�f�ԍ��Ɨv�f���ʔԍ��̃y�A�j�O���[�v
 *  \li ���E�ߓ_�O���[�v
 *  \li ���E�ߓ_�����l�O���[�v
 *
 *  �ł��B\n
 *
 *  ���E�ߓ_�O���[�v�́A���E�ʂɂ̂ݒ�`����Ă���ߓ_�O���[�v�ŁA�X�V���
 *  ���E�ʂɂ̂ݑ��݂��邱�Ƃ��ۏ؂���܂��B
 *  �������A�̈敪����̗̈拫�E�ʂɑ��݂���ꍇ��r�����Ă��܂���
 *  �i�̈拫�E�ʂ͗^���Ȃ����ƂɂȂ��Ă���j�̂ŁA�\���o�̌Ăяo������
 *  �̈拫�E�ʂɂ��鋫�E�������폜����K�v������܂��B\n
 *
 *  ���E�ߓ_�����l�O���[�v�́A���E�ʂɂ̂ݒ�`����Ă���A�ߓ_��̕��z��^������̂ł��B
 *  FrontFlow/blue �̋��E�������X�V���邽�߂ɁA�����l�ŋ��E�������X�V����Ƃ��ɗD�揇�ʂ�
 *  �L�q���܂��B���Ԑߓ_�ɗ^���鐮���l�ɂ��Ẵ��[���͈ȉ��̒ʂ�ł��B
 *
 *  \li ���Ƃ̐ߓ_�Ȃ��ɕϐ����^�����Ă��Ȃ��ߓ_������΁A���Ԑߓ_�ɂ͕ϐ��͗^���Ȃ��B
 *  \li ���Ƃ̐ߓ_�ɕϐ����^�����Ă��āA�ϐ��̒l�����ׂē��������́A���Ԑߓ_�ɂ��̓������l��^����B
 *  \li ���Ƃ̐ߓ_�ɕϐ����^�����Ă��āA�ϐ��̒l���قȂ鎞�́A���Ԑߓ_�ɍł��������l��^����B
 *
 *  rcapRefineElement ���Ăяo���O�� rcapAppendNodeGroup rcapAppendElementGroup rcapAppendFaceGroup
 *  rcapAppendBNodeGroup rcapAppendBNodeVarInt ��
 *  �ו����ɍX�V������������o�^���܂��B�o�^����ꍇ�̎��ʎq��ς��邱�Ƃŕ����̏�����o�^���邱�Ƃ��ł��܂��B
 *  �o�^��� rcapRefineElement �����s����ƁA�����œo�^�����ߓ_�O���[�v�A�v�f�O���[�v�A�ʃO���[�v�͍X�V����܂��B
 *  rcapCommit �����s����ƁArcapAppendBNodeGroup ����� rcapAppendBNodeVarInt �œo�^�������E�������X�V����܂��B
 *  ����� rcapCommit �����s��͍ו���̃f�[�^�̎��o�����\�ɂȂ�܂��B
 *  �ו���̃f�[�^�����o���ɂ́ArcapGetNodeGroup rcapGetElementGroup rcapGetFaceGroup rcapGetBNodeGroup rcapBNodeVarInt ���g���܂��B
 *  �f�[�^�̌��i�ߓ_�O���[�v�Ȃ�ΐߓ_���j���擾����ɂ� rcapGetNodeGroupCount rcapGetElementGroupCount rcapGetFaceGroupCount rcapBnodeGroupCount
 *  rcapGetBNodeVarIntCount �Œ��ׂ邱�Ƃ��ł��܂��B
 *
 *  REVOCAP Refiner �ł͍ו��Ώۂ̋��E�����ƁA�ו���̋��E�����̗���������ŕێ����Ă��܂��B
 *  rcapGetNodeGroup �ȂǂŎ擾�ł��鋫�E�����́A�ו��Ώۂ̋��E�����ł��B
 *  rcapCommit �ł͍ו��Ώۂ̋��E�������ו��Ώۂ̋��E�����ɒu�������܂��B
 *  ���Ȃ킿 rcapRefineElement �����s��ArcapCommit �����s������́A����܂ōו���̋��E�������A
 *  �ו��Ώۂ̋��E�����ɂȂ邽�߁ArcapGetNodeGroup �Ȃǂŋ��E�������擾���邱�Ƃ��ł���悤�ɂȂ�܂��B
 *  ���i�K�̍ו����s���ꍇ�́A���ꂼ��̒i�K�̍ו��������������_�� rcapCommit �����s���邱�Ƃ��K�v�ł��B
 *
 *  �ȉ������̗�ł��B
 *
 * \code
 * program RefinerSample
 * ! �r����
 *   integer(kind=4), dimension(3) :: ng0
 *   integer(kind=4), dimension(:), allocatable :: result_ng0
 *   character(80) :: str
 *   integer(kind=4) :: res
 * ! �r����
 *   str = "const"//CHAR(0)
 *   ng0 = (/ 1, 2, 3 /)
 *   nodeCount = 3
 *   call rcapAppendNodeGroup(str,nodeCount,ng0)
 *   call rcapRefineElement( elementCount, RCAP_TETRAHEDRON, tetras, refineTetras )
 *   call rcapCommit()
 *   res = rcapGetNodeGroupCount(str)
 *   allocate( result_ng0(res) )
 *   call rcapGetNodeGroup(str,res,result_ng0)
 * ! �r����
 *   deallocate( result_ng0 )
 *   call rcapTermRefiner()
 * end program RefinerSample
 * \endcode
 *
 *  rcapRefineElement ���Ăяo�������ɁA���̑O�� rcapAppendNodeGroup �œo�^���ꂽ
 *  �ߓ_�O���[�v�𓯎��ɍX�V���܂��B
 *  �o�^����Ƃ��ɂ��鎯�ʎq�� C ����Ƃ̌݊����̂��� CHAR(0) ���Ō�ɂ���
 *  �Ăяo���Ă��������B
 *
 * \section howto_refine_edge �v�f�̖ʂ�ӂ��ו�������@
 *
 *  �v�f�̖ʂ�ӂ��ו�����ꍇ�͎��̂悤�ɂ��܂��B
 *  �Ⴆ�΂S�ʑ̗v�f�̏ꍇ�A�ʂ��R�p�`�v�f�A�ӂ�����v�f�Ƃ݂Ȃ��A
 *  rcapRefineElement ���Ăяo���܂��B�S�ʑ̗v�f���ו�������ɂR�p�`�v�f���ו������ꍇ�A
 *  �R�p�`�v�f�̒��Ԑߓ_�̐ߓ_�ԍ��́A�S�ʑ̗v�f���ו������Ƃ��ɗ^����ꂽ���Ԑߓ_�̐ߓ_�ԍ����p�����܂��B
 *  �t�ɖʂ̂R�p�`�v�f���ו����A���̌�łS�ʑ̗v�f���ו������ꍇ�́A���łɖʂ��R�p�`�v�f�ōו�����Ă���ꍇ�ɂ�
 *  ���̖ʏ�̒��Ԑߓ_�̐ߓ_�ԍ��́A�R�p�`�v�f���ו������Ƃ��ɗ^����ꂽ���Ԑߓ_�̐ߓ_�ԍ����p�����܂��B
 *
 * \section element_id_after_refine �ו���̗v�f�ԍ��ɂ���
 *
 *  �ʃO���[�v��v�f�O���[�v�͗v�f�ԍ���p���ĕ\������Ă��܂��B
 *  rcapRefineElement �ŗv�f���ו����Ă����̋��E�������X�V����ƁA����ɕt������v�f�ԍ�
 *  ���X�V����܂����A�v�f�ԍ��̕t�����͈ȉ��̒ʂ�Ƃ��܂��B
 *
 *  \li ���͂����v�f�ԍ��� rcapRefineElement ���Ăԏ��� elementOffset ���珇�ɕ���ł�����̂Ƃ���
 *  \li �o�͂����v�f�ԍ��� rcapRefineElement ���Ăԏ��� elementOffset ���珇�ɕ���ł�����̂Ƃ���
 *  \li �v�f�ԍ��� rcapClearRefiner ���ĂԂ� elementOffset �Ƀ��Z�b�g�����
 *
 *  �Ⴆ�΁AelementOffset = 1 �� rcapRefineElement ��100�̂S�ʑ̂�50�̂R�p�`�ɂ��ČĂ񂾂Ƃ��܂��B
 *
 * \code
 *   call rcapRefineElement( tetraCount, RCAP_TETRAHEDRON, tetras, refineTetras )
 *   call rcapRefineElement( triCount, RCAP_TRIANGLE, tris, refineTris )
 * \endcode
 *
 *  ���̏ꍇ�A�ו��O�̗v�f�ԍ��͂S�ʑ̂�1����100�܂ŁA�R�p�`��101����150�܂ŗ^�����Ă���Ɖ��߂��A
 *  �ו����ꂽ�S�ʑ̗̂v�f�ԍ���1����800�܂ŁA�R�p�`�̗v�f�ԍ���801����1000�܂ŗ^�����Ă���Ɖ��߂��܂��B
 *  ���̗v�f�ԍ��ɏ]���āA�v�f�O���[�v�ƖʃO���[�v�̋��E�������X�V���܂��B
 *
 * \section get_middle_point �ו���ɒ��Ԑߓ_�̐ߓ_�ԍ����擾����
 *
 *  �ו���̒��Ԑߓ_�̐ߓ_�ԍ��� rcapRefineElement �̌��ʂ��画�f�ł��܂����A
 *  ����ȊO�� Refiner �ɖ₢���킹�邱�Ƃɂ���Ă��\�ł��B
 *  �ォ��₢���킹����@�́A���̂Q�ʂ肠��܂��B
 *
 *  \li �₢���킹�֐� rcapGetMiddle ���Ăяo��
 *  \li �v�f�ו��֐� rcapRefineElement �𗼒[�̐ߓ_�ԍ�����Ȃ�����v�f�ɂ��ČĂяo��
 *
 *  ���ɍו�����Ē��Ԑߓ_�����݂���ꍇ�́A���҂̐U�镑���͓����ł��B
 *  �ו�����Ă��Ȃ��Ē��Ԑߓ_�����݂��Ȃ��ꍇ�́A�O�҂͖����Ȑߓ_�ԍ� -1 ��Ԃ��܂����A
 *  ��҂͉��߂čו����s���A���Ԑߓ_�𐶐����Ă��̐ߓ_�ԍ���Ԃ��܂��B
 *
 * \section get_original_point �ו���ɒ��Ԑߓ_����e�ߓ_�i�v�f�j���擾����
 *
 *  �ו���� rcapRefineElement �̌��ʂ��璆�Ԑߓ_����e�ߓ_��m�邱�Ƃ��ł��܂����A
 *  ���̑Ή��� REVOCAP_Refiner ���Ŋo���Ă���̂ŁA�Ăяo�����Ō����p�̃e�[�u���Ȃǂ�
 *  �p�ӂ��Ă����K�v�͂���܂���BrcapGetOriginal �Œ��Ԑߓ_��^����ƁA�e�ߓ_��
 *  �擾���邱�Ƃ��ł��܂��B
 *  �����Őe�ߓ_�A�e�v�f�ƌĂ�ł�����̂�
 *
 *  \li ���̗v�f�̕ӂ̒��_�ɐ������ꂽ���Ԑߓ_�̏ꍇ�F���[��2�_��e�ߓ_�Ƃ��ĕԂ��܂�
 *  \li �l�p�`�̒��S�A�܂��͘Z�ʑ̖̂ʂ̒��S�ɐ������ꂽ���Ԑߓ_�̏ꍇ�F�l�p�`���\������4�_�i2���v�f�Ȃ�8�_�j��e�v�f�Ƃ��ĕԂ��܂��B�Z�ʑ̖̂ʂ̒��S�ɐ������ꂽ���Ԑߓ_�̏ꍇ�́A�����ŗ^������v�f�͍ו����ꂽ�v�f�ł͂Ȃ����Ƃɒ��ӂ��Ă��������B
 *  \li �Z�ʑ̂̒��S�ɐ������ꂽ���Ԑߓ_�̏ꍇ�F�Z�ʑ̂��\������8�_�i2���v�f�̏ꍇ��20�_�j��e�v�f�Ƃ��ĕԂ��܂��B
 *  \li ���̗v�f�̕ӂł͂Ȃ��A�ו��ɂ���Đ������ꂽ���Ԑߓ_���m���Ȃ��ŏo���ĕӂ̒��Ԑߓ_�̏ꍇ�F���[�̂Q�_��e�ߓ_�Ƃ��ĕԂ��܂��B���̏ꍇ�͐e�ߓ_�Ƃ��ĕԂ��l�́A�ו��O�̐ߓ_�ԍ��ł͂Ȃ����Ƃɒ��ӂ��Ă��������B
 *
 *  �ł���B
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
	 * �Ǐ��I�ɂ� 1 ���珇�ɐߓ_�ԍ����t�^����Ă���Ɖ��߂���
	 */
	float64_t coords[15] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0
	};
	/*
	 * �ו���̐ߓ_�ԍ����i�[����
	 */
	float64_t* resultCoords = NULL;
	/* �ߓ_�ԍ��� GlobalId */
	int32_t globalIds[5] = { 100, 101, 102, 103, 104 };
	/* ���b�V���̐ߓ_�z��Ɍ����ߓ_�ԍ��̍ŏ��̒l
	 * �ʏ� C ����ĂԂƂ��� 0 fortran ����Ăԏꍇ�� 1
	 * �����ł� C ���痘�p���Ă��邪�A������ 1 ���炵���ꍇ���l����
	 */
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* �l�ʑ̂̐ߓ_�z��F���͂�2�� */
	int32_t tetras[8] = {
		1, 2, 3, 4,
		2, 3, 4, 5
	};
	/* �ו���̎l�ʑ̂̐ߓ_�z��F�o�͂�2*8=16��*/
	int32_t* refineTetras = NULL;
	/* �ו�����v�f�̌^(�萔�l) */
	int8_t etype = RCAP_TETRAHEDRON;
	/* �����ߓ_�̌� */
	size_t nodeCount = 5;
	/* �����v�f�̌� */
	size_t elementCount = 2;
	/* �ו���̗v�f�̌� */
	size_t refineElementCount = 0;
	/* �ו���̐ߓ_�̌� */
	size_t refineNodeCount = 0;
	/* �v�f�̍ו��Ɠ����ɍX�V����ߓ_�O���[�v */
	int32_t ng0[3] = {1,2,3};
	int32_t* result_ng0 = NULL;
	size_t ngCount = 3;
	/* �v�f�̍ו��Ɠ����ɍX�V����ʃO���[�v */
	/* �v�f�ԍ��Ɨv�f���ʔԍ��̏��Ɍ��݂ɕ��ׂ� */
	int32_t fg0[4] = {1,3,2,1};   /* [1,2,3] �� [2,5,4] */
	int32_t result_fg0[12];
	size_t fgCount = 2;
	/* �v�f�̍ו��Ɠ����ɍX�V����v�f�O���[�v */
	int32_t eg0[1] = {2};
	int32_t* result_eg0 = NULL;
	size_t egCount = 1;
	/* �֐��̖߂�l�i�[�p */
	int32_t seg[2] = {-1,-1};
	int8_t orgtype = RCAP_UNKNOWNTYPE;
	int32_t middle = -1;
	int32_t flag = 0;
	/* ���[�v�̃J�E���^ */
	int32_t i = 0;
	int32_t j = 0;

	/* �T�C�Y�̃`�F�b�N */
	printf("sizeof(size_t) == %u\n",sizeof(size_t));
	if( sizeof(int8_t) != 1 )
		printf(" sizeof(int8_t) error %u\n", sizeof(int8_t) );
	if( sizeof(uint8_t) != 1 )
		printf(" sizeof(uint8_t) error %u\n", sizeof(uint8_t) );
	if( sizeof(int16_t) != 2 )
		printf(" sizeof(int16_t) error %u\n", sizeof(int16_t) );
	if( sizeof(uint16_t) != 2 )
		printf(" sizeof(uint16_t) error %u\n", sizeof(uint16_t) );
	if( sizeof(int32_t) != 4 )
		printf(" sizeof(int32_t) error %u\n", sizeof(int32_t) );
	if( sizeof(uint32_t) != 4 )
		printf(" sizeof(uint32_t) error %u\n", sizeof(uint32_t) );
	if( sizeof(float32_t) != 4 )
		printf(" sizeof(float32_t) error %u\n", sizeof(float32_t) );
	if( sizeof(float64_t) != 8 )
		printf(" sizeof(float64_t) error %u\n", sizeof(float64_t) );
	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* globalId �ƍ��W�l�� Refiner �ɋ�����
	 * localIds �� NULL ����������� coords �� nodeOffset ���珇�Ԃɕ���ł�����̂Ɖ��߂���
	 */
	rcapSetNode64( nodeCount, coords, globalIds, NULL );
	/* �ו��O�̐ߓ_�� */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );
	/* �ו��O�̗v�f�� */
	printf("Original Element Count = %u\n", elementCount );
	/* �ߓ_�O���[�v�̓o�^ */
	rcapAppendNodeGroup("innovate",ngCount,ng0);
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Original node group count %u\n", ngCount );
	/* �ʃO���[�v�̓o�^ */
	rcapAppendFaceGroup("revolute",fgCount,fg0);
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Original face group count %u\n", fgCount );
	/* �v�f�O���[�v�̓o�^ */
	rcapAppendElementGroup("eg",egCount,eg0);
	egCount = rcapGetElementGroupCount("eg");
	printf("Original element group count %u\n", egCount );
	/* �v�f�̍ו� */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	printf("Refined Element Count = %u\n", refineElementCount );
	if( elementCount != refineElementCount ){
		printf("Refine Element Error! different elelment count\n");
	}
	rcapCommit();
	/* ���Ԑߓ_�̃`�F�b�N */
	for(j=0;(size_t)j<4*refineElementCount;++j){
		orgtype = rcapGetOriginal( refineTetras[j], seg );
		if( orgtype == RCAP_SEGMENT ){
			middle = rcapGetMiddle( orgtype, seg );
			if( middle != refineTetras[j] ){
				printf( "Refine Node Error! Middle Node Different! %d => [%d,%d] => %d\n", refineTetras[j], seg[0], seg[1], middle);
			}
		}else{
			if( refineTetras[j] > (int32_t)nodeCount ){
				printf( "Refine Node Error! Bad Node Id Of Middle Node\n");
			}
		}
	}
	/* �ו���̐ߓ_�O���[�v�̍X�V */
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Refined node group count %u\n", ngCount );
	if( ngCount > 0 ){
		result_ng0 = (int32_t*)calloc( ngCount, sizeof(int32_t) );
		rcapGetNodeGroup("innovate",ngCount,result_ng0);
		/* �ו���̐ߓ_�O���[�v�̃`�F�b�N */
		for(i=0;i<3;++i){
			flag = 0;
			for(j=0;(size_t)j<ngCount;++j){
				if( result_ng0[j] == ng0[i] ){
					flag = 1;
				}
			}
			if( flag != 1 ){
				printf( "Refine Node Group Error! Updated Data Don't have an original node id\n");
			}
		}
		free( result_ng0 );
	}else{
		printf( "Refine Node Group Error! No Refined Data\n");
	}

	/* �ו���̗v�f�O���[�v�̍X�V */
	egCount = rcapGetElementGroupCount("eg");
	printf("Refined element group count %u\n", egCount );
	if( egCount > 0 ){
		result_eg0 = (int32_t*)calloc( egCount, sizeof(int32_t) );
		rcapGetElementGroup("eg",egCount,result_eg0);
		/* �ו���̗v�f�O���[�v�̃`�F�b�N */
		if( egCount != 8 ){
			printf( "Refine Element Group Error! Bad Element Group Count\n");
		}
		flag = 1;
		for(i=0;i<8;++i){
			if( result_eg0[i] != 9+i ){
				flag = 0;
			}
		}
		if( flag == 0 ){
			printf( "Refine Element Group Error! Bad Element Group Id\n");
		}
		free( result_eg0 );
	}

	/* �ו���̖ʃO���[�v�̍X�V */
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Refined face group count %u\n", fgCount );
	rcapGetFaceGroup("revolute",fgCount,result_fg0);
	/* �ו���̖ʃO���[�v�̃`�F�b�N */
	if( fgCount != 8 ){
		printf( "Refine Face Group Error! Bad Face Group Count\n");
	}

	/* �ו���̐ߓ_�̌� */
	refineNodeCount = rcapGetNodeCount();
	printf("Refined Node Count = %u\n", refineNodeCount );

	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	for(j=0;(size_t)j<refineNodeCount;++j){
		orgtype = rcapGetOriginal( j+nodeOffset, seg );
		if( orgtype == RCAP_SEGMENT ){
			if( coords[3*(seg[0]-nodeOffset)] + coords[3*(seg[1]-nodeOffset)] != 2.0 * resultCoords[3*j] ||
			    coords[3*(seg[0]-nodeOffset)+1] + coords[3*(seg[1]-nodeOffset)+1] != 2.0 * resultCoords[3*j+1] ||
			    coords[3*(seg[0]-nodeOffset)+2] + coords[3*(seg[1]-nodeOffset)+2] != 2.0 * resultCoords[3*j+2] ){
				printf( "Refine Node Coordinate Error %d => [%d %d]\n",j+nodeOffset,seg[0],seg[1]);
				printf( "[%f,%f,%f] + [%f,%f,%f] != 2 * [%f,%f,%f]\n",
					coords[3*(seg[0]-nodeOffset)], coords[3*(seg[0]-nodeOffset)+1], coords[3*(seg[0]-nodeOffset)+2],
					coords[3*(seg[1]-nodeOffset)], coords[3*(seg[1]-nodeOffset)+1], coords[3*(seg[1]-nodeOffset)+2],
					resultCoords[3*j], resultCoords[3*j+1], resultCoords[3*j+2] );
			}
		}
	}

	free( resultCoords );
	free( refineTetras );

	rcapTermRefiner();
	return 0;
}

#endif
