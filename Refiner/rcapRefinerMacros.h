/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.0                          #
# Program Name : rcapRefinerMacros                                     #
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
 * �e�X�g�v���O�����p�̃}�N���W
 */

/* size_t �^�̏o�͂� C99 �ł� VC8 �ł��x�����o�Ȃ��悤�ɂ��� */
#ifdef _MSC_VER
/* #if __STDC_VERSION__ < 199901L */
# define PRIsz "u"
#else
# define PRIsz "zd"
#endif
