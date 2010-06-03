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
 * \page program_in_out REVOCAP Refiner プログラム説明書３（入出力仕様システム）
 *
 * \section function_contract REVOCAP Refiner の関数呼び出し規約
 *
 * \subsection howto_call_function 関数の呼び出し方
 *
 *  REVOCAP Refiner はソルバまたはカップラに組み込まれてそのプロセスの中で実行される（フォークしない）。
 *  新たなプロセスを起動してプロセス間通信でデータのやり取りをするのではなく、
 *  ソルバやカップラのプロセスの中で、関数を通じてデータのやり取りをする。
 *  関数への入力や出力となる変数のメモリ管理は呼び出し側が責任を持つ。
 *  ソルバやカップラへ受け渡さない REVOCAP Refiner の内部データは
 *  REVOCAP Refiner がメモリ管理について責任を持つ。
 *
 *  REVOCAP Refiner のメモリ空間は呼び出されるプロセスの中で一意である。
 *  同じプロセスの中で複数の REVOCAP Refiner の処理を行う場合は、現バージョンでは未対応である。
 *  初期化と終了を正しく使わない場合はデータが破壊される場合があるので注意が必要である。
 *
 * \subsection name_of_function 関数の名前
 *
 *  名前は rcapXXX(...) とする\n
 *  fortran77 fortran90 の呼び出しのための関数名の変換はマクロではなく、ラッパ関数を経由することで行う。
 *  ビルドする場合には、呼び出す Fortran のコンパイラの種類によってライブラリのコンパイル時に
 *  異なるマクロ定数を与えなければならない。
 *  \li gfortran コンパイラ：gfortran, Intel Fortran, PGI Fortran
 *
 * \subsection howto_allocate_arrays 配列のメモリ確保の方法
 *
 *  配列の受け渡しは呼び出し側で allocate して、このモジュールでは allocate も free もせず、要素に代入するだけとする。\n
 *  配列があふれた場合はエラーコードを返す。
 *
 * \subsection constants REVOCAP Refiner における定数
 *
 *  要素の型を表す RCAP_TETRAHEDRON, RCAP_TETRAHEDRON2, RCAP_HEXAHEDRON など\n
 *  C 言語で使う場合は、この rcapRefiner.h を include する。\n
 *  Fortran 言語で使う場合は rcapRefiner.inc を include する。
 *
 * \subsection variable_types REVOCAP Refiner における変数の型
 *
 *  UNIX 標準 C の stdint.h および、浮動小数点についても同様の定義をする。
 *  unsigned 型は Fortran で定義できない場合があるため使わない。
 *  C 言語の size_t 型はコンテナの大きさをあらわす数として使うが、
 *  Fortran ではラッパーで符号付きに変換して使う。
 *  実行環境に依存する可能性のある型はプリプロセッサで吸収する。
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
 *  のように定数値でコンパイル時に切り替えることが必要。
 *
 * \subsection in_out_variables REVOCAP Refiner の関数の入出力引数
 *
 *  \li 引数は入力系を前に、出力系を後ろにする
 *  \li 配列の入力を与える時は、配列の大きさと先頭アドレスを与える
 *  \li 要素の型など、他の情報で配列の大きさが決まる場合は、配列の大きさは省略する
 *  \li 出力で配列を用いる場合は、あらかじめ allocate された配列に値を代入することで行う
 *  \li 戻り値はエラー処理、出力で配列を用いた場合の値を代入した個数のために使う
 *
 * \subsection element_node_order REVOCAP Refiner の要素の節点配列の順番
 *
 *  細分する要素の節点配列の順番はソルバ、カップラごとに異なる可能性があるが、
 *  ここでは革新プロジェクトにおける REVOCAP で用いた順番に従うことにする。
 *
 *  また節点配列の順番や、面番号の付け方については定数配列として、
 *  ソルバ、カップラ、および Refiner で共有し、適宜参照できるものとする。
 *
 */
/*
 * http:
 * 仕様の書き方 (doxgen に準拠)
 *  引数の説明   @param[in] @param[out]
 *  戻り値の説明 @return
 *  処理の概要   @brief
 *  注意項目     @remarks
 *  \section 等の第１引数は html にしたときの name tag に使われる
 */

#ifndef REVOCAP_REFINER_HEADER
#define REVOCAP_REFINER_HEADER

/* プリポストでも定義しているため、重複しないように */
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
 * 現在未実装です
 *
 * @brief 形状補正に用いる CAD ファイルを指定する。\n
 * 細分前のメッシュはこの CAD ファイルから生成されているものとする。
 * この関数を呼んで CAD ファイルを指定しなかった場合は、形状補正は行わない。
 * メッシュ生成時の節点の globalID と CAD の形状データの対応が与えられているとする。
 * 領域分割後のメッシュに対して細分を行う場合は、setPartitionFilename などで
 * globalID と localID の対応付けを与える必要がある。
 * @param[in] filename ファイル名
 */
void rcapSetCADFilename( const char* filename );
/**
 * 現在仮実装です。CAD で形状補正する場合には必須。
 *
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
 * 現在のバージョンでは globalIds は考慮していません。
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
void rcapGetNode32( size_t num, int32_t* localIds, float32_t* coords );
/**
 * @brief initId から連続して num 個の節点座標を取得する
 * @param[in] num 節点の個数
 * @param[in] initId 取得する先頭の節点番号
 * @param[out] coords 節点の座標、(x,y,z) の順番に 3*num 個並べたもの
 * @note coords の initId 番目から代入する
 */
void rcapGetNodeSeq64( size_t num, size_t initId, float64_t* coords );
void rcapGetNodeSeq32( size_t num, size_t initId, float32_t* coords );
/**
 * @brief 要素をそれぞれ辺の２分割して細分する
 * @param[in] num 要素の個数
 * @param[in] etype 入力要素の型
 * @param[in] nodeArray 入力要素の節点配列（etype型の要素をnum個並べたもの）
 * @param[out] resultNodeArray 細分結果の要素の節点配列
 * @return 細分した結果の要素の個数
 *
 * @note resultNodeArray を NULL または -1 として呼び出すと、細分した場合の個数だけを計算して返す。
 * ただし細分が実際に行われたかどうかについてはわからない。\n
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
 * @brief 複数の種類の型が混在しているモデルを一度に細分する
 * @param[in] num 要素の個数
 * @param[in] etypeArray 入力要素の型の配列
 * @param[in] nodeArray 入力要素の節点配列
 * @param[in/out] refinedNum 細分後の要素の個数
 * @param[out] resultEtypeArray 細分結果の型の節点配列
 * @param[out] resultNodeArray 細分結果の要素の節点配列
 * @return 細分した結果を格納するのに必要な節点配列の大きさ
 *
 * @note resultNodeArray を NULL または -1 として呼び出すと、
 * 細分した結果を格納するのに必要な節点配列の大きさを返す。\n
 * refinedNum に細分後の要素の個数を代入する。
 * 従って、resultNodeArray = NULL で呼び出した結果の戻り値の大きさで
 * 配列 resultNodeArray をメモリ上に確保する。
 * resultEtypeArray の配列の大きさは refinedNum に代入された値を使う。
 *
 * @note 実際に細分を行うときには、配列 resultNodeArray に値を入れて代入して呼び出す。
 * この場合の戻り値も細分の結果に必要な節点配列の大きさを返す。
 *
 * @note resultEtypeArray に NULL を入れても細分の実行は可能。
 * 配列を入れると、細分した要素の型を代入して返す。
 *
 * @note 要素の型ごとに節点配列をまとめれば rcapRefineElement で同等のことを実行できる
 *
 * @note rcapClearRefiner を呼ぶまでは etypeArray, nodeArray, resultETypeArray, resultNodeArray を解放しないでください。
 *
 * @remark *** 戻り値の仕様が 2010/2/9 バージョンから変更しています ***
 */
size_t rcapRefineElementMulti( size_t num, int8_t* etypeArray, int32_t* nodeArray, size_t* refinedNum, int8_t* resultEtypeArray, int32_t* resultNodeArray );
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
 * @brief BoundaryNodeGroup とは、境界面上にのみある節点グループのことで細分と同時に更新する。
 * @param[in] dataname 境界節点グループの識別子
 * @param[in] num 取得する境界節点グループの個数
 * @param[out] nodeArray 境界節点グループの節点
 * @note rcapGetBoundaryNodeGroup するときは、あらかじめ nodeArray を allocate しておくこと。
 * @note refineElement をするときに、このタイプの境界条件がある時は、表面抽出を行い、表面の三角形または四角形を
 * 使って境界条件を更新する。
 */
void rcapAppendBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );
size_t rcapGetBNodeGroupCount( const char dataname[80] );
void rcapGetBNodeGroup( const char dataname[80], size_t num, int32_t* nodeArray );

/**
 * @brief BoundaryNodeVariableInt とは、境界面上にのみある節点上の整数値変数のこと
 * @param[in] dataname 境界節点変数の識別子
 * @param[in] num 取得する境界節点変数の（節点の）個数
 * @param[out] nodeArray 境界節点変数の節点
 * @param[out] nodeVars 境界節点変数の変数（nodeArrayと同じ大きさ）
 * @note rcapGetBoundaryNodeVariableInt するときは、あらかじめ nodeArray と nodeVars を allocate しておくこと。
 * @note refineElement をするときに、このタイプの境界条件がある時は、表面抽出を行い、表面の三角形または四角形を
 * 使って境界条件を更新する。
 * @note 細分で生じる中間節点について、
 * もとの節点のなかで変数が与えられていない節点があれば、中間節点には変数は与えない。
 * もとの節点に変数が与えられていて、変数の値がすべて等しい時は、中間節点にその等しい値を与える。
 * もとの節点に変数が与えられていて、変数の値が異なる時は、中間節点に最も小さい値を与える。
 */
void rcapAppendBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars );
size_t rcapGetBNodeVarIntCount( const char dataname[80] );
void rcapGetBNodeVarInt( const char dataname[80], size_t num, int32_t* nodeArray, int32_t* nodeVars  );

/**
 * @brief ElementGroup とは、要素番号の集合のこと。
 * @param[in] dataname 要素グループの識別子
 * @param[in] num 取得する要素グループの個数
 * @param[out] elementArray 要素グループの配列
 * @note rcapGetElementGroup するときは、あらかじめ elementArray を allocate しておくこと。
 */
void rcapAppendElementGroup( const char dataname[80], size_t num, int32_t* elementArray );
size_t rcapGetElementGroupCount( const char dataname[80] );
void rcapGetElementGroup( const char dataname[80], size_t num, int32_t* elementArray );

/**
 * @brief FaceGroup とは、要素番号、要素内面番号の組のこと。連成面を細分する場合などに用いる。
 * @param[in] dataname 面グループの識別子
 * @param[in] num 取得する面グループの個数（要素番号と面番号の組の個数）
 * @param[out] faceArray 面グループの要素番号、面番号の組の配列。
 * @note faceArray は要素番号と面番号を交互に並べた 2*num 個の配列になる。
 */
void rcapAppendFaceGroup( const char dataname[80], size_t num, int32_t* faceArray );
size_t rcapGetFaceGroupCount( const char dataname[80] );
void rcapGetFaceGroup( const char dataname[80], size_t num, int32_t* faceArray );

/**
 * @brief NodeVariable を登録したときに、中間節点に与える値の決め方を選択します。
 * 現在は "MIN" "MAX" "MIDDLE" の3種類に対応しています。
 * MIN は中間節点を生成するのに用いた節点上の値の最小値を与えます。
 * MAX は中間節点を生成するのに用いた節点上の値の最大値を与えます。
 * MIDDLE は中間節点を生成するのに用いた節点上の値の平均値を与えます。
 */
void rcapSetInterpolateMode( const char mode[32] );
void rcapGetInterpolateMode( char mode[32] );

/**
 *
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
 * @brief 細分後の自然座標の変換
 */
int32_t rcapGetRefinedNaturalCoord( int32_t* elementId, float32_t* coords, int32_t* refinedElementId, float32_t* refinedCoords );

/**
 * @breif デバッグ用ファイル入出力ルーチン
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
void rcapRefineFFbModel();
int32_t rcapSaveGFFile( const char* gffile, const char* bounfile );
int32_t rcapLoadHECFile( const char* hecfile );
int32_t rcapSaveHECFile( const char* hecfile );
int32_t rcapSaveRNFFile( const char* rnffile );

/* rcapxxx_  すべて小文字 */
/* gfortran, intel fortran, pgi fortran はこれでよい */
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
