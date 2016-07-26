/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Program Name : rcapRefiner                                           #
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
#ifndef REVOCAP_REFINER_HEADER
#define REVOCAP_REFINER_HEADER

/* プリポストでも定義しているため、重複しないように */
#ifndef REVOCAP_SIZE_DEF
#define REVOCAP_SIZE_DEF

/* for size_t */
#include <stddef.h>

#ifdef _MSC_VER
 #if (_MSC_VER >= 1300 )
  #ifndef HAVE_INT8_T
   typedef signed __int8 int8_t;
  #endif
  #ifndef HAVE_UINT8_T
   typedef unsigned __int8 uint8_t;
  #endif
  #ifndef HAVE_INT16_T
   typedef signed __int16 int16_t;
  #endif
  #ifndef HAVE_UINT16_T
   typedef unsigned __int16 uint16_t;
  #endif
  #ifndef HAVE_INT32_T
   typedef signed __int32 int32_t;
  #endif
  #ifndef HAVE_UINT32_T
   typedef unsigned __int32 uint32_t;
  #endif
  #ifndef HAVE_INT64_T
   typedef signed __int64 int64_t;
  #endif
  #ifndef HAVE_UINT64_T
   typedef unsigned __int64 uint64_t;
  #endif
 #else
  #ifndef HAVE_INT8_T
   typedef signed char int8_t;
  #endif
  #ifndef HAVE_UINT8_T
   typedef unsigned char uint8_t;
  #endif
  #ifndef HAVE_INT16_T
   typedef short int16_t;
  #endif
  #ifndef HAVE_UINT16_T
   typedef unsigned short uint16_t;
  #endif
  #ifndef HAVE_INT32_T
   typedef long int32_t;
  #endif
  #ifndef HAVE_UINT32_T
   typedef unsigned long uint32_t;
  #endif
  #ifndef HAVE_INT64_T
   typedef long long int64_t;
  #endif
  #ifndef HAVE_UINT64_T
   typedef unsigned long long uint64_t;
  #endif
 #endif
#else
/* C99 から導入されている */
#include <stdint.h>
#endif

typedef float float32_t;  /* REAL*4 */
typedef double float64_t; /* REAL*8 */

#endif

/* 要素の型情報 int8_t */
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
/* 14 は要素と点の境界として内部で予約 */
#define RCAP_VERTEX       15

const int RCAP_TETRAFACES[12] =
{
	1, 2, 3,  /* 0 番目の面 */
	0, 3, 2,  /* 1 番目の面 */
	0, 1, 3,  /* 2 番目の面 */
	0, 2, 1   /* 3 番目の面 */
};

const int RCAP_HEXAFACES[24] =
{
	3, 2, 1, 0,  /* 0 番目の面 */
	4, 5, 6, 7,  /* 1 番目の面 */
	1, 5, 4, 0,  /* 2 番目の面 */
	1, 2, 6, 5,  /* 3 番目の面 */
	3, 7, 6, 2,  /* 4 番目の面 */
	4, 7, 3, 0   /* 5 番目の面 */
};

const int RCAP_PYRAFACES[20] =
{
	0, 1, 2,-1,  /* 0 番目の面 */
	0, 2, 3,-1,  /* 1 番目の面 */
	0, 3, 4,-1,  /* 2 番目の面 */
	0, 4, 1,-1,  /* 3 番目の面 */
	4, 3, 2, 1   /* 4 番目の面 */
};

const int RCAP_WEDGEFACES[20] =
{
	0, 2, 1,-1,  /* 0 番目の面 */
	3, 4, 5,-1,  /* 1 番目の面 */
	0, 1, 4, 3,  /* 2 番目の面 */
	1, 2, 5, 4,  /* 3 番目の面 */
	2, 0, 3, 5   /* 4 番目の面 */
};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Version 文字列を標準出力に出力する。
 */
void rcapGetVersion( void );

/**
 * @brief Refiner を初期化し、節点番号と要素番号のオフセット値を与える。
 * @param[in] nodeOffset 呼び出し側の節点番号の初期値（C言語なら0、Fortran言語なら1など）
 * @param[in] elementOffset 呼び出し側の節点番号の初期値（C言語なら0、Fortran言語なら1など）
 * @note これらのオフセット値よりも小さい値を節点番号や要素番号で与えた場合は無効となる。
 */
void rcapInitRefiner( int32_t nodeOffset, int32_t elementOffset );

/**
 * @brief Refiner が内部で保持している中間節点データのキャッシュをクリアする。
 * @note 細分を複数回行う場合に、一旦内部で保持しているデータをクリアするために使う。
 * 例えば第１段階の細分が終わって、第２段階の細分を始める前に呼ぶ。
 * ここでクリアされるデータは中間節点のデータのみであり、節点の座標値、
 * 細分時に更新する節点グループの情報等はクリアされないので注意する。
 */
void rcapClearRefiner( void );

/**
 * @brief Refiner の終了処理を行う。これを呼び出した後はすべての処理が無効になる。
 * @note Refiner に登録した節点、要素、境界条件などのメモリを解放する前に
 * Refiner の終了処理を行ってください。Refiner の終了処理を行う前に
 * メモリを解放すると、不正なアドレスにアクセスする可能性があります。
 */
void rcapTermRefiner( void );

/**
 * @brief 形状補正に用いる CAD ファイルを指定する。\n
 * 細分前のメッシュはこの CAD ファイルから生成されているものとする。
 * この関数を呼んで CAD ファイルを指定しなかった場合は、形状補正は行わない。
 * メッシュ生成時の節点の globalID と CAD の形状データの対応が与えられているとする。
 * 領域分割後のメッシュに対して細分を行う場合は、setPartitionFilename などで
 * globalID と localID の対応付けを与える必要がある。
 *
 * @remark この関数で与える CAD ファイルには、形状データだけではなく、
 * 初期メッシュの節点と CAD の面の間の対応関係も記述されているため、
 * 初期メッシュが異なる場合には、CAD ファイルも置き換えなければならない。
 * CAD ファイルの節点番号は 0 始まりの番号を用いている。
 * rcapSetNode32 rcapSetNode64 または rcapSetPartitionFilename で与える場合には
 * rcapInitRefiner で与える nodeOffset だけずれるので注意する。
 *
 * @remark 細分前の事前補正を行う場合は rcapSetNode32/64 で節点座標を与える前に
 * この関数を呼ぶ必要があります。
 * この関数を先に呼んだ場合も細分時の形状適合は行いますが、細分前の事前補正をしません。
 *
 * @param[in] filename ファイル名
 */
void rcapSetCADFilename( const char* filename );
/**
 * @brief 形状補正に用いる CAD ファイルと事前補正情報を出力する。
 * globalID と localID を与えた後で出力する補正情報は、localID と曲面との対応になる。
 *
 * @remark このファイルで出力される節点番号は 0 始まりである。
 *
 * @remark この関数はデバッグ用途または、領域分割した後に局所的な細分を行う場合に用いる。
 *
 * @param[in] filename ファイル名
 */
void rcapWriteFittingFile( const char* filename );

/**
 * @brief 中間節点の生成に2次要素の形状関数を使うかどうかを設定する。
 * @param[in] flag 非零の時に有効にし、零の時に無効にします
 *
 */
void rcapSetSecondFitting( int32_t flag );

/**
 * @brief 中間節点を Laplacian Smoothing するかどうかを設定する。
 * @param[in] flag 非零の時に有効にし、零の時に無効にします
 *
 */
void rcapSetSmoothing( int32_t flag );

/**
 * @brief 節点の globalID と localID の対応関係を記述したファイルを指定する。\n
 * 指定しない場合は、globalID と localID は区別しない。
 * ファイルではなく、節点座標を登録するときに globalID と localID の関係を与えることもできる。
 * @param[in] filename ファイル名
 * @note ファイルフォーマットは革新プロジェクトでソルバとカプラの間で用いたものに準ずる。
 */
void rcapSetPartitionFilename( const char* filename );

/**
 * @brief 節点座標を Refiner に与える
 * @param[in] num 与える節点の個数
 * @param[in] coords 節点の座標、double 型の配列で (x,y,z) の順番に 3*num 個並べたもの
 * @param[in] localIds 領域分割した領域内の局所節点ID の配列
 * @param[in] globalIds 大域節点ID の配列
 *
 * @note 初期値から順に num 個並んでいる場合は localIds = NULL でよい。
 * Global ID をファイルから与える場合は globalIds = NULL にする。
 * setPartitionFilename で領域分割の大域節点と局所節点の対応が与えられている時には、
 * globalIds を与えなくてもよい。この関数で与えた場合には、setPartitionFilename で与えた対応を上書きする。
 * setPartitionFilename で与えない場合に、この関数でも globalIds を与えない場合は、localId をそのまま使う。
 *
 * @li setNode64( num, coords, NULL, NULL ) => global と local は同じで nodeOffset から順に num 個
 * @li setNode64( num, coords, globalIds, NULL ) => local は nodeOffset から順に num 個、global は引数で与える
 * @li setNode64( num, coords, NULL, localIds ) => global と local は同じで local は与える
 * @li setNode64( num, coords, globalIds, localIds ) => global と local をそれぞれ与える
 *
 * @note CADファイルを与えずに形状補正をしない場合は、局所節点番号と大域節点番号の対応は必要ない。
 * @note Fortran から呼ぶ場合には NULL アドレスの代わりに最初の値が nodeOffset よりも小さい配列を与える。
 */
void rcapSetNode64( size_t num, float64_t* coords, int32_t* globalIds, int32_t* localIds );

/**
 * @brief 節点座標を Refiner に与える
 * @param[in] num 与える節点の個数
 * @param[in] coords 節点の座標、float 型の配列で (x,y,z) の順番に 3*num 個並べたもの
 * @param[in] localIds 領域分割した領域内の局所節点ID の配列
 * @param[in] globalIds 大域節点ID の配列
 *
 * @note 詳細は rcapSetNode64 に準ずる
 */
void rcapSetNode32( size_t num, float32_t* coords, int32_t* globalIds, int32_t* localIds );

/**
 * @brief 現在 Refiner が保持している節点の個数を返す。細分したら増える。
 * @return 節点の個数
 */
size_t rcapGetNodeCount( void );

/**
 * @brief Refiner が管理している節点座標を取得する
 * @param[in] num 節点の個数
 * @param[in] localIds 領域分割した領域内の局所節点ID の配列
 * @param[out] coords 節点の座標、(x,y,z) の順番に 3*num 個並べたもの
 *
 * @note RefineElement の戻り値を num として、resultNodeArray を localIds に入れると
 * coords で座標値を取得できる。\n
 * localIds = {1,5,8} の場合
 * coords = {x1,y1,z1,x5,y5,z5,x8,y8,z8} となる。\n
 * localIds の節点番号は自動的に nodeOffset でずらして Refiner に問い合わせる
 */
void rcapGetNode64( size_t num, int32_t* localIds, float64_t* coords );
/**
 * @brief Refiner が管理している節点座標を取得する
 * @param[in] num 節点の個数
 * @param[in] localIds 領域分割した領域内の局所節点ID の配列
 * @param[out] coords 節点の座標、(x,y,z) の順番に 3*num 個並べたもの
 *
 * @note rcapGetNode64 の 32bit 版
 */
void rcapGetNode32( size_t num, int32_t* localIds, float32_t* coords );

/**
 * @brief initId から連続して num 個の節点座標を取得する
 * @param[in] num 節点の個数
 * @param[in] initId 取得する先頭の節点番号
 * @param[out] coords 節点の座標、(x,y,z) の順番に 3*num 個並べたもの
 * @note coords の initId 番目から代入する
 */
void rcapGetNodeSeq64( size_t num, size_t initId, float64_t* coords );
/**
 * @brief rcapGetNodeSeq64 の 32bit 版
 */
void rcapGetNodeSeq32( size_t num, size_t initId, float32_t* coords );

/**
 * @brief 要素をそれぞれ辺の２分割して細分する
 * @param[in] num 要素の個数
 * @param[in] etype 入力要素の型
 * @param[in] nodeArray 入力要素の節点配列（etype型の要素をnum個並べたもの）
 * @param[out] resultNodeArray 細分結果の要素の節点配列
 * @return 細分した結果の要素の個数
 *
 * @note nodeArray はローカル節点IDで記述したものを与えてください。
 *
 * @note resultNodeArray を NULL または -1 として呼び出すと、細分した場合の個数だけを計算して返す。
 * 例えば４面体ならば入力要素の個数の８倍を返す。\n
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
 * @note 複数の要素の型が混じっている場合は、同じ型の要素をまとめてこの関数を要素の型ごとに複数回呼ぶか、
 * 節点配列にどの型の要素が順番に入っているかの配列を作成して refineElementMulti 関数を呼ぶ。\n
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
 * @note 要素を細分した後に、その要素の面や辺を三角形要素や線分要素とみなしてこの関数を呼び出すと、
 * 要素を細分した結果を面や辺に制限した結果を返す。逆に要素の面や辺の細分を呼び出した後で、
 * 要素の細分を行うと、先に細分した面や辺についてはその節点番号を使う。
 *
 * @note rcapClearRefiner を呼ぶまでは nodeArray resultNodeArray を解放しないでください。
 *
 * @note 四面体要素の細分後の８個の要素の現れる順番
 *
 *  - (1)四面体の1番目の節点が属する要素
 *  - (2)四面体の2番目の節点が属する要素
 *  - (3)四面体の3番目の節点が属する要素
 *  - (4)四面体の4番目の節点が属する要素
 *  - (5)四面体の1番目の面が属する要素
 *  - (6)四面体の2番目の面が属する要素
 *  - (7)四面体の3番目の面が属する要素
 *  - (8)四面体の4番目の面が属する要素
 *
 * 前半の4つの面番号は細分前の要素と同じ向き
 * 後半の4つの面番号は0番目が細分前の要素の外側の向き
 *
 * @note 六面体要素の細分後の８個の要素の現れる順番
 *  - (1)六面体の1番目の節点が属する要素
 *  - (2)六面体の2番目の節点が属する要素
 *  - (3)六面体の3番目の節点が属する要素
 *  - (4)六面体の4番目の節点が属する要素
 *  - (5)六面体の5番目の節点が属する要素
 *  - (6)六面体の6番目の節点が属する要素
 *  - (7)六面体の7番目の節点が属する要素
 *  - (8)六面体の8番目の節点が属する要素
 *
 * 面番号は細分前の要素と同じ向き
 *
 * @note 三角柱要素の細分後の8個の要素の現れる順番
 *
 *  - (1)三角柱の1番目の節点が属する要素
 *  - (2)三角柱の2番目の節点が属する要素
 *  - (3)三角柱の3番目の節点が属する要素
 *  - (4)三角柱の4番目の節点が属する要素
 *  - (5)三角柱の5番目の節点が属する要素
 *  - (6)三角柱の6番目の節点が属する要素
 *  - (7)三角柱の0,1,2番目の節点による三角形に接する要素
 *  - (8)三角柱の3,4,5番目の節点による三角形に接する要素
 *
 * はじめの6個の面番号は細分前の要素と同じ向き
 * 最後の2個の面番号は0番目が細分前の要素の外側の向き
 *
 * @note 四角錐要素の細分後の10個の要素の現れる順番
 *
 *  - (1)四角錐の1番目の節点が属する要素
 *  - (2)四角錐の2番目の節点が属する要素
 *  - (3)四角錐の3番目の節点が属する要素
 *  - (4)四角錐の4番目の節点が属する要素
 *  - (5)四角錐の5番目の節点が属する要素
 *  - (6)四角錐の0,1,2番目の節点による三角形に接する四面体要素
 *  - (7)四角錐の0,2,3番目の節点による三角形に接する四面体要素
 *  - (8)四角錐の0,3,4番目の節点による三角形に接する四面体要素
 *  - (9)四角錐の0,4,1番目の節点による三角形に接する四面体要素
 *  - (10)四角錐の中に埋め込まれている要素
 *
 * はじめの5個の面番号は細分前の要素と同じ向き
 * 次の4個の面番号は細分前の四角錐の面番号と、四面体要素の外に向いている面番号が一致する向き
 * 最後の1個は全ての面が埋め込まれている
 */
size_t rcapRefineElement( size_t num, int8_t etype, int32_t* nodeArray, int32_t* resultNodeArray );
/**
 * @brief 細分した要素の個数を計算する（実際には細分はしない）
 * @param[in] num 要素の個数
 * @param[in] etype 入力要素の型
 * @return 細分した結果の要素の個数
 */
size_t rcapGetRefineElementCount( size_t num, int8_t etype );

/**
 * @brief 複数の種類の型が混在しているモデルを一度に細分する
 * @param[in] num 要素の個数
 * @param[in] etypeArray 入力要素の型の配列
 * @param[in] nodeArray 入力要素の節点配列
 * @param[in,out] refinedNum 細分結果の要素の個数
 * @param[out] resultEtypeArray 細分結果の要素の型の配列
 * @param[out] resultNodeArray 細分結果の要素の節点配列
 * @return 細分した結果を格納するのに[必要な/使用した]節点配列の大きさ
 *
 * @note nodeArray はローカル節点IDで記述したものを与えてください。
 * @note rcapClearRefiner を呼ぶまでは etypeArray, nodeArray, resultETypeArray, resultNodeArray を解放しないでください。
 *
 * @note この関数は実際に細分する機能と、節点配列の大きさを調べるだけの機能とを兼ねています。
 * resultNodeArray を NULL または -1 として呼び出すと、
 * 細分した結果を格納するのに必要な節点配列の大きさを返し、
 * refinedNum に細分後の要素の個数を代入します。
 * resultEtypeArray は実際に細分する機能と、節点配列の大きさを調べるだけの機能の
 * 切り替えのための情報としては利用しません。
 * このモジュールを利用する場合にはこの戻り値の大きさの
 * 配列 resultNodeArray を呼び出し側がメモリ上に確保し、
 * 細分するためにこの関数を再度呼び出す必要があります。
 *
 * @note 実際に細分を行うときには、配列 resultEtypeArray と resultNodeArray をメモリ上に確保し、
 * それぞれ初期化してからこの関数を呼び出します。
 * このとき、refinedNum は resultEtypeArray / resultNodeArray で
 * 格納可能な要素の個数を与えます。与えられた refinedNum よりも
 * 細分された要素の個数が大きい場合は、超えた部分は格納しません。
 * 戻り値は細分の結果を格納するのに使用した節点配列の大きさを返します。
 *
 * @note 細分後の要素の型が自明の場合は、resultNodeArray をメモリ上に確保して、
 * resultEtypeArray に NULL を与えても細分の実行は可能です。
 *
 * @note
 * result = rcapRefineElementMulti(num,etypeArray,nodeArray,[refinedNum],NULL,NULL)
 * result = rcapRefineElementMulti(num,etypeArray,nodeArray,[refinedNum],[-1],[-1])
 * refinedNum に細分で得られる要素の個数を代入し、result に細分結果を格納するのに必要な節点配列の大きさを返す
 * 実際には細分は行わない。
 *
 * @note
 * result = rcapRefineElementMulti(num,etypeArray,nodeArray,[refinedNum],resultEtypeArray,resultNodeArray)
 * 実際に細分を行い、細分結果を resultEtypeArray と resultNodeArray に格納する。
 * refinedNum で与えられた大きさの要素を格納する配列が確保されていることを仮定している。
 *
 * @note 要素の型ごとに節点配列をまとめれば rcapRefineElement で同等のことを実行できます。
 *
 * @remark *** 戻り値の仕様が 2010/2/9 バージョンから変更しています ***
 */
size_t rcapRefineElementMulti( size_t num, int8_t* etypeArray, int32_t* nodeArray, size_t* refinedNum, int8_t* resultEtypeArray, int32_t* resultNodeArray );
/**
 * @brief 複数の種類の型が混在しているモデルを細分したときの要素の個数を計算する（実際には細分しない）
 * @param[in] num 要素の個数
 * @param[in] etypeArray 入力要素の型の配列
 * @param[out] refinedNum 細分結果の要素の個数
 * @return 細分した結果を格納するのに必要な節点配列の大きさ
 */
size_t rcapGetRefineElementMultiCount( size_t num, int8_t* etypeArray, size_t* refinedNum );

/**
 * @brief
 * rcapRefineElement により細分されたデータ（節点グループ、要素グループ、面グループ）をコミットする。
 * すなわち、以下の rcapGet[Node|Element|Face]Group[Count] メソッドの対象を細分前のデータから
 * 細分後のデータに変える。細分前のデータは削除される。
 * この関数を実行後に rcapRefineElement を再度実行した場合、更新されるデータは
 * 細分後のデータになる。
 * また、rcapRefineElement で細分後の要素に付与される要素番号も elementOffset 値にリセットされる。
 * @remark
 * rcapRefineElement を複数回呼ぶ場合は、複数回呼んだ後に一度だけ rcapCommit を実行する。
 * ２段階の細分を行う場合は、１段階目の細分 rcapRefineElement を呼んだ後、
 * ２段階目の rcapRefineElement を呼ぶ前に実行する。
 * @remark
 * rcapAppendBoundaryNodeGroup、rcapAppendBoundaryNodeVariableXXX で追加された境界条件については
 * この関数が呼ばれた時点で登録されている要素の境界面を抽出して、その面で境界条件を更新する。
 * 大規模複雑モデルのこれらの条件を更新する場合は、境界面の抽出に時間とメモリを要する場合がある。
 */
void rcapCommit(void);

/**
 * @brief 細分と同時に更新する節点グループを登録
 * @param[in] dataname 節点グループの識別子
 * @param[in] num 節点グループの節点数
 * @param[in] nodeArray 節点グループの節点
 * @note refineElement を呼び出す前にこの関数で節点グループを登録しておくと、refineElement を呼び出した時に、
 * 細分する要素の辺の両端が節点グループに含まれるならば、細分によって生成された辺の中点の節点を
 * 節点グループに追加する。
 * 識別子は文字列です。Fortran から呼び出す場合は、文字列の最後にヌル文字を追加してください。
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
 * @brief Refiner に登録されている節点グループの節点の個数を返す
 * @param[in] dataname 節点グループの識別子
 * @return 節点グループの節点の個数
 * @note refineElement を呼ぶ前は、appendNodeGroup で登録した個数そのものを返す。
 */
size_t rcapGetNodeGroupCount( const char dataname[80] );

/**
 * @brief Refiner に登録されている節点グループを返す
 * @param[in] dataname 節点グループの識別子
 * @param[in] num 取得する節点グループの個数
 * @param[out] nodeArray 節点グループの節点
 * @note 引数 num は getNodeGroupCount で取得した値を与えて、nodeArray はあらかじめ num 個の
 * 大きさの配列であらかじめ allocate しているものとする。
 */
void rcapGetNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );

/**
 * @brief BoundaryNodeGroup とは、境界面上にのみある節点グループのこと。この関数で登録する。
 * @param[in] dataname 境界節点グループの識別子
 * @param[in] num 取得する境界節点グループの個数
 * @param[out] nodeArray 境界節点グループの節点
 * @note rcapGetBoundaryNodeGroup するときは、あらかじめ nodeArray を allocate しておくこと。
 * @note refineElement をするときに、このタイプの境界条件がある時は、表面抽出を行い、表面の三角形または四角形を
 * 使って境界条件を更新する。
 */
void rcapAppendBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );

/**
 * @brief Refiner に登録されている境界節点グループの節点の個数を返す
 * @param[in] dataname 節点グループの識別子
 * @return 節点グループの節点の個数
 * @note refineElement を呼ぶ前は、appendBNodeGroup で登録した個数そのものを返す。
 */
size_t rcapGetBNodeGroupCount( const char dataname[80] );

/**
 * @brief Refiner に登録されている境界節点グループを返す
 * @param[in] dataname 境界節点グループの識別子
 * @param[in] num 取得する境界節点グループの個数
 * @param[out] nodeArray 境界節点グループの節点
 * @note 引数 num は getBNodeGroupCount で取得した値を与えて、nodeArray はあらかじめ num 個の
 * 大きさの配列であらかじめ allocate しているものとする。
 */
void rcapGetBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );

/**
 * @brief BoundaryNodeVariableInt とは、境界面上にのみある節点上の整数値変数のこと
 * @param[in] dataname 境界節点変数の識別子
 * @param[in] num 取得する境界節点変数の（節点の）個数
 * @param[out] nodeArray 境界節点変数の節点
 * @param[out] nodeVars 境界節点変数の値（nodeArrayと同じ大きさ）
 * @note rcapGetBoundaryNodeVariableInt するときは、あらかじめ nodeArray と nodeVars を allocate しておくこと。
 * @note refineElement をするときに、このタイプの境界条件がある時は、表面抽出を行い、表面の三角形または四角形を
 * 使って境界条件を更新する。
 * @note 細分で生じる中間節点について、
 * もとの節点のなかで変数が与えられていない節点があれば、中間節点には変数は与えない。
 * もとの節点に変数が与えられていて、変数の値がすべて等しい時は、中間節点にその等しい値を与える。
 * もとの節点に変数が与えられていて、変数の値が異なる時は、中間節点に最も小さい値を与える。
 */
void rcapAppendBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars );

/**
 * @brief Refiner に登録されている整数値境界節点変数の節点の個数を返す
 * @param[in] dataname 境界節点変数の識別子
 * @return 境界節点変数の節点の個数
 * @note refineElement を呼ぶ前は、appendBNodeVarInt で登録した個数そのものを返す。
 */
size_t rcapGetBNodeVarIntCount( const char dataname[80] );

/**
 * @brief Refiner に登録されている整数値境界節点変数を返す
 * @param[in] dataname 境界節点変数の識別子
 * @param[in] num 取得する境界節点変数の個数
 * @param[out] nodeArray 境界節点変数の節点
 * @param[out] nodeVars 境界節点変数の値
 * @note 引数 num は getBNodeVarIntCount で取得した値を与えて、nodeArray, nodeVars はあらかじめ num 個の
 * 大きさの配列であらかじめ allocate しているものとする。
 */
void rcapGetBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars );

/**
 * @brief ElementGroup とは、要素番号の集合のこと。
 * @param[in] dataname 要素グループの識別子
 * @param[in] num 取得する要素グループの個数
 * @param[out] elementArray 要素グループの配列
 * @note rcapGetElementGroup するときは、あらかじめ elementArray を allocate しておくこと。
 */
void rcapAppendElementGroup( const char dataname[80], size_t num, int32_t* elementArray );

/**
 * @brief Refiner に登録されている要素グループの要素の個数を返す
 * @param[in] dataname 要素グループの識別子
 * @return 要素グループの要素の個数
 * @note refineElement を呼ぶ前は、appendElementGroup で登録した個数そのものを返す。
 */
size_t rcapGetElementGroupCount( const char dataname[80] );

/**
 * @brief Refiner に登録されている要素グループを返す
 * @param[in] dataname 要素グループの識別子
 * @param[in] num 取得する要素グループの個数
 * @param[out] elementArray 要素グループの節点
 * @note 引数 num は getElementGroupCount で取得した値を与えて、elementArray はあらかじめ num 個の
 * 大きさの配列であらかじめ allocate しているものとする。
 */
void rcapGetElementGroup( const char dataname[80], size_t num, int32_t* elementArray );

/**
 * @brief FaceGroup とは、要素番号、要素内面番号の組のこと。連成面を細分する場合などに用いる。
 * @param[in] dataname 面グループの識別子
 * @param[in] num 取得する面グループの個数（要素番号と面番号の組の個数）
 * @param[out] faceArray 面グループの要素番号、面番号の組の配列。
 * @note faceArray は要素番号と面番号を交互に並べた 2*num 個の配列になる。
 */
void rcapAppendFaceGroup( const char dataname[80], size_t num, int32_t* faceArray );

/**
 * @brief Refiner に登録されている面グループの個数を返す
 * @param[in] dataname 面グループの識別子
 * @return 面グループの面の個数
 * @note refineElement を呼ぶ前は、appendFaceGroup で登録した個数そのものを返す。
 */
size_t rcapGetFaceGroupCount( const char dataname[80] );

/**
 * @brief Refiner に登録されている面グループを返す
 * @param[in] dataname 面グループの識別子
 * @param[in] num 取得する面グループの個数
 * @param[out] faceArray 面グループの面
 * @note 引数 num は getFaceGroupCount で取得した値を与えて、faceArray はあらかじめ num 個の
 * 大きさの配列であらかじめ allocate しているものとする。
 */
void rcapGetFaceGroup( const char dataname[80], size_t num, int32_t* faceArray );

/**
 * @brief NodeVariable を登録したときに、中間節点に与える値の決め方を選択します。
 * 現在は "MIN" "MAX" "MIDDLE" の3種類に対応しています。
 * MIN は中間節点を生成するのに用いた節点上の値の最小値を与えます。
 * MAX は中間節点を生成するのに用いた節点上の値の最大値を与えます。
 * MIDDLE は中間節点を生成するのに用いた節点上の値の平均値を与えます。
 */
void rcapSetInterpolateMode( const char mode[32] );

/**
 * @brief NodeVariable を登録したときに、中間節点に与える値の決め方を返します。
 * 戻り値は "MIN" "MAX" "MIDDLE" という文字列のいずれかです。
 */
void rcapGetInterpolateMode( char mode[32] );

/**
 * @brief 中間節点から、それを生成するのに使った辺、面、要素の節点配列を返す
 * @param[in] localNodeId 細分された中間節点の局所節点番号
 * @param[out] originalNodeArray localNodeId を生成するのに使った要素の節点配列を返します
 * @return 戻り値は要素の型
 * @note 例：辺の中点の場合は節点配列に２つ値を代入して RCAP_SEGMENT を返す。localNodeId に細分で生成された点
 * でない点を与えた場合は、何もせずに -1 = RCAP_UNKNOWNTYPE を返します。
 *       例：四角形の中心の場合は、節点配列に４つ値を代入して RCAP_QUAD を返す。
 *
 * @note この関数を呼ぶ場合は rcapRefineElement で与えた細分前の要素の節点配列 nodeArray
 * および rcapRefineElementMulti で与えた細分前の要素の要素タイプ配列 etypeArray と 節点配列 nodeArray
 * のメモリを解放しないでください。別の値で上書きすることもやめてください。
 * （この関数の内部でその節点配列を参照しているため）
 *
 */
int8_t rcapGetOriginal( int32_t localNodeId, int32_t* originalNodeArray );

/**
 * @brief 辺、面、要素を与えて、それから作られた中間節点を戻り値で返す
 * @return 中間節点の節点番号
 *
 * 既に細分が行われた後での
 *
 * \code
 * middle = rcapGetMiddle( RCAP_SEGMENT, nodeArray );
 * \endcode
 *
 * と
 *
 * \code
 * rcapRefineElement( 1, RCAP_SEGMENT, nodeArray, resultArray );
 * middle = resultArray[2];
 * \endcode
 *
 * は等価です。ただし、細分が行われていない場合（中間節点が存在しない場合）は
 * rcapGetMiddle は無効な節点番号(-1)を返しますが、
 * rcapRefineElement は細分をしてからその結果を返します。
 */
int32_t rcapGetMiddle( int8_t etype, int32_t* originalNodeArray );

/**
 * @brief 要素の品質に関する情報を出力します。
 * @param[in] name 品質を測定する指標
 * @param[in] filename ファイル名（指定しない場合は標準エラーに出力します）
 */
void rcapQualityReport( const char name[80], const char* filename );

/**
 * @brief 細分後の自然座標の変換
 * 未実装
 */


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
#if defined FORTRAN90 || defined FORTRAN_CALL_C_DOWNCASE_
void rcapgetversion_( void );
void rcapinitrefiner_( int32_t* nodeOffset, int32_t* elementOffset );
void rcapclearrefiner_( void );
void rcaptermrefiner_( void );
void rcapsetcadfilename_( const char* filename );
void rcapsetsecondfitting_( int32_t* flag );
void rcapsetsmoothing_( int32_t* flag );
void rcapsetpartitionfilename_( const char* filename );

void rcapsetnode64_( int32_t* num, float64_t* coords, int32_t* globalIds, int32_t* localIds );
void rcapsetnode32_( int32_t* num, float32_t* coords, int32_t* globalIds, int32_t* localIds );
int32_t rcapgetnodecount_( void );
void rcapgetnode64_( int32_t* num, int32_t* localIds, float64_t* coords );
void rcapgetnode32_( int32_t* num, int32_t* localIds, float32_t* coords );
void rcapgetnodeseq64_( int32_t* num, int32_t* initId, float64_t* coords );
void rcapgetnodeseq32_( int32_t* num, int32_t* initId, float32_t* coords );

int32_t rcapgetrefineelementcount_( int32_t* num, int8_t* etype );
int32_t rcaprefineelement_( int32_t* num, int8_t* etype, int32_t* nodeArray, int32_t* resultNodeArray );
int32_t rcapgetrefineelementmulticount_( int32_t* num, int8_t* etypeArray, int32_t* refinedNum );
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

void rcapqualityreport_( const char mode[80], const char* filename );

int32_t rcaploadgffile_( const char* gffile, const char* bounfile );
int32_t rcaploadhecfile_( const char* hecfile );
int32_t rcapsavegffile_( const char* gffile, const char* bounfile );
int32_t rcapsavehecfile_( const char* hecfile );
int32_t rcapsavernffile_( const char* rnffile );
#endif

#if defined FORTRAN_CALL_C_DOWNCASE__
#define rcapgetversion_ rcapgetversion__
#define rcapinitrefiner_ rcapinitrefiner__
#define rcapclearrefiner_ rcapclearrefiner__
#define rcaptermrefiner_ rcaptermrefiner__
#define rcapsetcadfilename_ rcapsetcadfilename__
#define rcapsetsecondfitting_ rcapsetsecondfitting__
#define rcapsetsmoothing_ rcapsetsmoothing__
#define rcapsetpartitionfilename_ rcapsetpartitionfilename__
#define rcapsetnode64_ rcapsetnode64__
#define rcapsetnode32_ rcapsetnode32__
#define rcapgetnodecount_ rcapgetnodecount__
#define rcapgetnode64_ rcapgetnode64__
#define rcapgetnode32_ rcapgetnode32__
#define rcapgetnodeseq64_ rcapgetnodeseq64__
#define rcapgetnodeseq32_ rcapgetnodeseq32__
#define rcapgetrefineelementcount_ rcapgetrefineelementcount__
#define rcaprefineelement_ rcaprefineelement__
#define rcapgetrefineelementmulticount_ rcapgetrefineelementmulticount__
#define rcaprefineelementmulti_ rcaprefineelementmulti__
#define rcapcommit_ rcapcommit__
#define rcapappendnodegroup_ rcapappendnodegroup__
#define rcapgetnodegroupcount_ rcapgetnodegroupcount__
#define rcapgetnodegroup_ rcapgetnodegroup__
#define rcapappendbnodegroup_ rcapappendbnodegroup__
#define rcapgetbnodegroupcount_ rcapgetbnodegroupcount__
#define rcapgetbnodegroup_ rcapgetbnodegroup__
#define rcapappendbnodevarint_ rcapappendbnodevarint__
#define rcapgetbnodevarintcount_ rcapgetbnodevarintcount__
#define rcapgetbnodevarint_ rcapgetbnodevarint__
#define rcapappendelementgroup_ rcapappendelementgroup__
#define rcapgetelementgroupcount_ rcapgetelementgroupcount__
#define rcapgetelementgroup_ rcapgetelementgroup__
#define rcapappendfacegroup_ rcapappendfacegroup__
#define rcapgetfacegroupcount_ rcapgetfacegroupcount__
#define rcapgetfacegroup_ rcapgetfacegroup__
#define rcapgetoriginal_ rcapgetoriginal__
#define rcapgetmiddle_ rcapgetmiddle__
#define rcapsetinterpolatemode_ rcapsetinterpolatemode__
#define rcapgetinterpolatemode_ rcapgetinterpolatemode__
#define rcapqualityreport_ rcapqualityreport__
#define rcaploadgffile_ rcaploadgffile__
#define rcaploadhecfile_ rcaploadhecfile__
#define rcapsavegffile_ rcapsavegffile__
#define rcapsavehecfile_ rcapsavehecfile__
#define rcapsavernffile_ rcapsavernffile__
#endif

#if defined FORTRAN_CALL_C_UPCASE
#define rcapgetversion_ RCAPGETVERSION
#define rcapinitrefiner_ RCAPINITREFINER
#define rcapclearrefiner_ RCAPCLEARREFINER
#define rcaptermrefiner_ RCAPTERMREFINER
#define rcapsetcadfilename_ RCAPSETCADFILENAME
#define rcapsetsecondfitting_ RCAPSETSECONDFITTING
#define rcapsetsmoothing_ RCAPSETSMOOTHING
#define rcapsetpartitionfilename_ RCAPSETPARTITIONFILENAME
#define rcapsetnode64_ RCAPSETNODE64
#define rcapsetnode32_ RCAPSETNODE32
#define rcapgetnodecount_ RCAPGETNODECOUNT
#define rcapgetnode64_ RCAPGETNODE64
#define rcapgetnode32_ RCAPGETNODE32
#define rcapgetnodeseq64_ RCAPGETNODESEQ64
#define rcapgetnodeseq32_ RCAPGETNODESEQ32
#define rcapgetrefineelementcount_ RCAPGETREFINEELEMENTCOUNT
#define rcaprefineelement_ RCAPREFINEELEMENT
#define rcapgetrefineelementmulticount_ RCAPGETREFINEELEMENTMULTICOUNT
#define rcaprefineelementmulti_ RCAPREFINEELEMENTMULTI
#define rcapcommit_ RCAPCOMMIT
#define rcapappendnodegroup_ RCAPAPPENDNODEGROUP
#define rcapgetnodegroupcount_ RCAPGETNODEGROUPCOUNT
#define rcapgetnodegroup_ RCAPGETNODEGROUP
#define rcapappendbnodegroup_ RCAPAPPENDBNODEGROUP
#define rcapgetbnodegroupcount_ RCAPGETBNODEGROUPCOUNT
#define rcapgetbnodegroup_ RCAPGETBNODEGROUP
#define rcapappendbnodevarint_ RCAPAPPENDBNODEVARINT
#define rcapgetbnodevarintcount_ RCAPGETBNODEVARINTCOUNT
#define rcapgetbnodevarint_ RCAPGETBNODEVARINT
#define rcapappendelementgroup_ RCAPAPPENDELEMENTGROUP
#define rcapgetelementgroupcount_ RCAPGETELEMENTGROUPCOUNT
#define rcapgetelementgroup_ RCAPGETELEMENTGROUP
#define rcapappendfacegroup_ RCAPAPPENDFACEGROUP
#define rcapgetfacegroupcount_ RCAPGETFACEGROUPCOUNT
#define rcapgetfacegroup_ RCAPGETFACEGROUP
#define rcapgetoriginal_ RCAPGETORIGINAL
#define rcapgetmiddle_ RCAPGETMIDDLE
#define rcapsetinterpolatemode_ RCAPSETINTERPOLATEMODE
#define rcapgetinterpolatemode_ RCAPGETINTERPOLATEMODE
#define rcapqualityreport_ RCAPQUALITYREPORT
#define rcaploadgffile_ RCAPLOADGFFILE
#define rcaploadhecfile_ RCAPLOADHECFILE
#define rcapsavegffile_ RCAPSAVEGFFILE
#define rcapsavehecfile_ RCAPSAVEHECFILE
#define rcapsavernffile_ RCAPSAVERNFFILE
#endif

#if defined FORTRAN_CALL_C_UPCASE_
#define rcapgetversion_ RCAPGETVERSION_
#define rcapinitrefiner_ RCAPINITREFINER_
#define rcapclearrefiner_ RCAPCLEARREFINER_
#define rcaptermrefiner_ RCAPTERMREFINER_
#define rcapsetcadfilename_ RCAPSETCADFILENAME_
#define rcapsetsecondfitting_ RCAPSETSECONDFITTING_
#define rcapsetsmoothing_ RCAPSETSMOOTHING_
#define rcapsetpartitionfilename_ RCAPSETPARTITIONFILENAME_
#define rcapsetnode64_ RCAPSETNODE64_
#define rcapsetnode32_ RCAPSETNODE32_
#define rcapgetnodecount_ RCAPGETNODECOUNT_
#define rcapgetnode64_ RCAPGETNODE64_
#define rcapgetnode32_ RCAPGETNODE32_
#define rcapgetnodeseq64_ RCAPGETNODESEQ64_
#define rcapgetnodeseq32_ RCAPGETNODESEQ32_
#define rcapgetrefineelementcount_ RCAPGETREFINEELEMENTCOUNT_
#define rcaprefineelement_ RCAPREFINEELEMENT_
#define rcapgetrefineelementmulticount_ RCAPGETREFINEELEMENTMULTICOUNT_
#define rcaprefineelementmulti_ RCAPREFINEELEMENTMULTI_
#define rcapcommit_ RCAPCOMMIT_
#define rcapappendnodegroup_ RCAPAPPENDNODEGROUP_
#define rcapgetnodegroupcount_ RCAPGETNODEGROUPCOUNT_
#define rcapgetnodegroup_ RCAPGETNODEGROUP_
#define rcapappendbnodegroup_ RCAPAPPENDBNODEGROUP_
#define rcapgetbnodegroupcount_ RCAPGETBNODEGROUPCOUNT_
#define rcapgetbnodegroup_ RCAPGETBNODEGROUP_
#define rcapappendbnodevarint_ RCAPAPPENDBNODEVARINT_
#define rcapgetbnodevarintcount_ RCAPGETBNODEVARINTCOUNT_
#define rcapgetbnodevarint_ RCAPGETBNODEVARINT_
#define rcapappendelementgroup_ RCAPAPPENDELEMENTGROUP_
#define rcapgetelementgroupcount_ RCAPGETELEMENTGROUPCOUNT_
#define rcapgetelementgroup_ RCAPGETELEMENTGROUP_
#define rcapappendfacegroup_ RCAPAPPENDFACEGROUP_
#define rcapgetfacegroupcount_ RCAPGETFACEGROUPCOUNT_
#define rcapgetfacegroup_ RCAPGETFACEGROUP_
#define rcapgetoriginal_ RCAPGETORIGINAL_
#define rcapgetmiddle_ RCAPGETMIDDLE_
#define rcapsetinterpolatemode_ RCAPSETINTERPOLATEMODE_
#define rcapgetinterpolatemode_ RCAPGETINTERPOLATEMODE_
#define rcapqualityreport_ RCAPQUALITYREPORT_
#define rcaploadgffile_ RCAPLOADGFFILE_
#define rcaploadhecfile_ RCAPLOADHECFILE_
#define rcapsavegffile_ RCAPSAVEGFFILE_
#define rcapsavehecfile_ RCAPSAVEHECFILE_
#define rcapsavernffile_ RCAPSAVERNFFILE_
#endif

#ifdef __cplusplus
}
#endif

#endif
