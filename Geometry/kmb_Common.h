/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Common Definitions                                                   #
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
#pragma once

#ifndef PI
 #define PI 3.1415926535897932384626
#endif
#ifndef NULL
 #define NULL 0
#endif

#ifndef REVOCAP_SIZE_DEF
#define REVOCAP_SIZE_DEF

#ifdef WIN32
 #ifndef _WIN64
 typedef signed char			int8_t;
 typedef unsigned char		uint8_t;
 typedef short				int16_t;
 typedef unsigned short		uint16_t;
 typedef int					int32_t;
 typedef unsigned int		uint32_t;
 typedef long long			int64_t;
 typedef unsigned long long	uint64_t;
 #endif


#else

#include <stdint.h>
#endif

typedef float float32_t;  /* REAL*4 */
typedef double float64_t; /* REAL*8 */

#endif /* REVOCAP_SIZE_DEF */

/* DEBUG ���b�Z�[�W
REVOCAP_Debug ��ʓI�ȃf�o�b�O�p���b�Z�[�W
REVOCAP_Debug_1 �ڍׂȃf�o�b�O�p���b�Z�[�W�P
REVOCAP_Debug_2 �ڍׂȃf�o�b�O�p���b�Z�[�W�Q
REVOCAP_Debug_3 �ڍׂȃf�o�b�O�p���b�Z�[�W�R�i���[�v�̒��ł��g���Ă悢�j

�f�o�b�O���ɋǏ��I�ɏo�͂������ꍇ��

#include "Geometry/kmb_Common.h"

������O��

#define _DEBUG_ 3

�ȂǂƋL�q���Ă�������

*/

#if defined( _MSC_VER ) && ( _MSC_VER < 1400 )
	#define REVOCAP_Debug(format)
#else
	#if defined _DEBUG || _DEBUG_
	#define REVOCAP_Debug(fmt, ...) fprintf(stderr,"%s,line %d:"fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug(format, ...)
	#endif

	#if ( _DEBUG >= 1 ) || ( _DEBUG_ >= 1 )
	#define REVOCAP_Debug_1(fmt, ...) fprintf(stderr,"%s,line %d:"fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug_1(format, ...)
	#endif

	#if ( _DEBUG >= 2 ) || ( _DEBUG_ >= 2 )
	#define REVOCAP_Debug_2(fmt, ...) fprintf(stderr,"%s,line %d:"fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug_2(format, ...)
	#endif

	#if ( _DEBUG >= 3 ) || ( _DEBUG_ >= 3 )
	#define REVOCAP_Debug_3(fmt, ...) fprintf(stderr,"%s,line %d:"fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug_3(format, ...)
	#endif
#endif
