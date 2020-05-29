/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Common Definitions for Debug                                         #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

/* DEBUG メッセージ */
/*
REVOCAP_Debug 一般的なデバッグ用メッセージ
REVOCAP_Debug_1 詳細なデバッグ用メッセージ１
REVOCAP_Debug_2 詳細なデバッグ用メッセージ２（ファイル出力の中でも使ってよい）
REVOCAP_Debug_3 詳細なデバッグ用メッセージ３（ループの中でも使ってよい）
REVOCAP_Debug_X 詳細なデバッグ用メッセージＸ（必ず出力）

デバッグ時に局所的に出力したい場合は

#define _DEBUG_ 3
#include "Common/kmbDebug.h"

... ここでは _DEBUG_ = 3 が有効
... ここで呼び出した関数内でも REVOCAP_Debug() が有効になる

#undef _DEBUG_
#include "Common/kmbDebug.h"

などと記述してください
*/

#include <cstdio>

#undef REVOCAP_Debug
#undef REVOCAP_Debug_1
#undef REVOCAP_Debug_2
#undef REVOCAP_Debug_3
#undef REVOCAP_Debug_X

#if defined( _MSC_VER ) && ( _MSC_VER < 1400 ) || defined( DEBUG_DISABLE )
	#define REVOCAP_Debug(format)
	#define REVOCAP_Debug_1(format)
	#define REVOCAP_Debug_2(format)
	#define REVOCAP_Debug_3(format)
	#define REVOCAP_Debug_X(format)
#else
	/* _DEBUG マクロに関係なく出力 */
	#define REVOCAP_Debug_X(fmt, ...) fprintf(stderr,"%s, line %d: " fmt,__FILE__,__LINE__, ##__VA_ARGS__)

	#if defined _DEBUG || _DEBUG_
	#include <cstdio>
	#define REVOCAP_Debug(fmt, ...) fprintf(stderr,"%s, line %d: " fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug(format, ...)
	#endif

	#if ( _DEBUG >= 1 ) || ( _DEBUG_ >= 1 )
	#define REVOCAP_Debug_1(fmt, ...) fprintf(stderr,"%s, line %d: " fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug_1(format, ...)
	#endif

	#if ( _DEBUG >= 2 ) || ( _DEBUG_ >= 2 )
	#define REVOCAP_Debug_2(fmt, ...) fprintf(stderr,"%s, line %d: " fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug_2(format, ...)
	#endif

	#if ( _DEBUG >= 3 ) || ( _DEBUG_ >= 3 )
	#define REVOCAP_Debug_3(fmt, ...) fprintf(stderr,"%s, line %d: " fmt,__FILE__,__LINE__, ##__VA_ARGS__)
	#else
	#define REVOCAP_Debug_3(format, ...)
	#endif
#endif
