/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Program Name : rcapRefinerIO                                         #
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
#ifndef REVOCAP_REFINERIO_HEADER
#define REVOCAP_REFINERIO_HEADER

#include "rcapRefiner.h"

/**
 * @brief デバッグ用ファイル入出力ルーチン
 * @return 読み込んだ、または書き込んだ要素の個数
 *
 * File から File への細分例
 * rcapInitRefiner(0,0);
 * rcapLoadGFFile( "MESH", "BOUN" );
 * rcapRefineFFbModel();
 * rcapSaveGFFile( "REFINEMESH", "REFINEBOUN" );
 * rcapTermRefiner();
 *
 * @remark ここで出力するデータは rcapRefinerDoc.mesh に格納されたもの
 * なので、細分後のメッシュを出力できるわけではない。
 */
int32_t rcapLoadGFFile( const char* gffile, const char* bounfile );

/**
 * @brief FFbModel細分（デバッグ用）
 */
void rcapRefineFFbModel();

/**
 * @brief GFファイルへ書き込み（デバッグ用）
 */
int32_t rcapSaveGFFile( const char* gffile, const char* bounfile );

/**
 * @brief HECファイルの読み込み（デバッグ用）
 */
int32_t rcapLoadHECFile( const char* hecfile );

/**
 * @brief HECファイルへ書き込み（デバッグ用）
 */
int32_t rcapSaveHECFile( const char* hecfile );

/**
 * @brief RNFファイルへ書き込み（デバッグ用）
 */
int32_t rcapSaveRNFFile( const char* rnffile );

/* rcapxxx_  すべて小文字 */
/* gfortran, intel fortran, pgi fortran はこれでよい */
#ifdef FORTRAN90
int32_t rcaploadgffile_( const char* gffile, const char* bounfile );
int32_t rcaploadhecfile_( const char* hecfile );
int32_t rcapsavegffile_( const char* gffile, const char* bounfile );
int32_t rcapsavehecfile_( const char* hecfile );
int32_t rcapsavernffile_( const char* rnffile );
void rcaprefineffbmodel_(void);
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
