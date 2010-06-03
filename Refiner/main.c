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
 * サンプル実行例＆テスト用プログラム
 *
 */
/**
 * \page tutorial 4 REVOCAP Refiner チュートリアル
 * \section init_and_term 起動と終了
 *
 *  REVOCAP Refiner の起動は rcapInitRefiner 関数で行い、終了は rcapTermRefiner 関数で行います。
 *  REVOCAP Refiner のすべての処理は rcapInitRefiner を読んだ後、rcapTermRefiner を呼ばれる前で有効です。
 *  それ以外のところで REVOCAP Refiner の処理を呼んでも何も処理を行わないか、エラーコードを返します。
 *
 * \code
 * #include "rcapRefiner.h"
 * int main(void){
 *   rcapInitRefiner(0,0);
 *   // ここで Refiner の処理を記述する
 *   rcapTermRefiner();
 *   return 0;
 * }
 * \endcode
 *
 *  Fortran 言語では次のようになります。
 *
 * \code
 * program RefinerSample
 *   implicit none
 *   integer, parameter :: RCAP_SIZE_T = 8
 *   include "rcapRefiner.inc"
 *   call rcapInitRefiner(1,1)
 *   ! ここで Refiner の処理を記述する
 *   call rcapTermRefiner()
 * end program RefinerSample
 * \endcode
 *
 *  rcapInitRefiner の引数は、節点番号および要素番号の開始番号(nodeOffset,elementOffset)とします。
 *
 *  以下ではすべてサンプルは Fortran 言語とします。
 *
 * \section set_model モデルの登録
 *
 *  REVOCAP Refiner へは、節点データ、要素データ、境界条件データを登録することができます。
 *  登録してあるデータは要素の細分の時に自動的に更新されます。
 *
 *  CAD データから取り出した曲面情報を用いて形状補正を行う場合は、
 *  細分する前のモデルのメッシュ生成を行った CAD データと、
 *  局所節点番号と大域節点番号の対応を与える必要があります。
 *  現在は未対応です。
 *
 *  現在対応しているモデルのデータは以下の通りです。
 *
 *  \li 節点座標
 *  \li 四面体１次要素、２次要素
 *  \li 六面体１次要素、２次要素
 *  \li 三角柱１次要素、２次要素
 *  \li 四角錐１次要素、２次要素
 *  \li 三角形１次要素、２次要素
 *  \li 四角形１次要素、２次要素
 *  \li 線分１次要素、２次要素
 *  \li 節点グループ
 *  \li 要素グループ
 *  \li 面（要素番号と要素内面番号の組）グループ
 *  \li 境界節点グループ
 *  \li 境界節点整数値
 *
 *  節点と４面体１次要素を登録して細分するには次のようにします。
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
 * Fortran では NULL アドレスの代入の代わりに -1 を代入することに注意します。
 *
 * \section howto_refine 要素の細分（単一要素）
 *
 *  REVOCAP Refiner で FEM モデルの要素の細分を行うには、rcapRefineElement 関数を使います。
 *  この関数は細分したい要素の型（４面体１次要素など）と、要素の個数、要素の節点配列を与えて、
 *  細分した要素の節点配列を取得します。
 *  細分した要素の節点配列はあらかじめ呼び出し側でメモリに確保しておく必要があります。\n
 *
 *  細分後の節点配列の個数を取得するには、データ取得用の節点配列を NULL とするか、
 *  最初の節点番号に -1 を入れて呼び出せば、個数を返します。\n
 *
 *  rcapRefineElement は rcapTermRefiner を実行するまで何度も呼び出すことができます。
 *  rcapRefineElement を呼び出す場合は、そこで細分をする要素の節点の座標を rcapSetNode32 または
 *  rcapSetNode64 であらかじめ登録しておく必要があります。
 *  節点の座標は要素を細分するときに細分のトポロジーを判定する時の材料として使われます。\n
 *
 *  最初に４面体からなる要素を細分し、次に
 *  その４面体の境界の３角形からなる要素について rcapRefineElement を呼び出した場合には、
 *  ３角形の細分による中間節点の節点番号は、４面体の細分の時に付与された中間節点の節点番号を使います。
 *
 *  最初に４面体からなる要素について rcapRefineElement を呼び出して細分し、
 *  さらにその結果である細分された４面体に対し、rcapRefineElement を呼び出した場合には、
 *  ２段階の細分がなされます。ただし、境界条件を同時に更新する場合に２段階の細分を行う場合は、
 *  １段階目の細分が終わったときに rcapCommit を実行してください。詳細は次章に記述してあります。
 *
 * \section howto_refine_multi 要素の細分（混合要素）
 *
 *  rcapRefineElement 関数は要素の型、個数、節点配列を与えるため、要素の種類は全て同じである必要があります。
 *  混合要素の FEM モデルを細分するためには、次の2通りの方法があります。
 *
 *  \li 呼び出し側で要素の型ごとにまとめて、rcapRefineElement を複数回呼ぶ
 *  \li rcapRefineElementMulti を使う
 *
 *  後者の rcapRefineElementMulti を使う場合は節点配列は複数の種類の要素を含むため、
 *  何番目にどの種類の要素かを記述した配列を別に用意する必要があります。それは eTypeArray に
 *  格納して呼び出してください。戻り値も複数の種類の要素が混在した節点配列となるため、
 *  戻り値の要素の種類を格納する配列を与える必要があります。\n
 *
 *  rcapRefineElement で配列を NULL または最初の節点番号を -1 として与えた時には、
 *  細分後の要素の個数を返したが、この関数では細分後の要素の個数は refinedNum で
 *  与えられるため、戻り値は細分した結果を格納するのに必要な節点配列の大きさを返します。\n
 *
 *  一度 resultNodeArray を NULL または最初の成分を -1 として呼び出せば、
 *  細分に必要な節点配列の大きさと、要素の個数が分かるので、
 *  戻り値を格納する配列 resultEtypeArray と resultNodeArray
 *  の大きさを確認してから再度呼び出すことができます。
 *
 * \section update_conditions 境界条件を要素の細分時に同時に更新する
 *
 *  REVOCAP_Refiner では FEM モデルの要素の細分時に、境界条件を同時に更新することができます。
 *  現在対応している境界条件は
 *
 *  \li 節点グループ
 *  \li 要素グループ
 *  \li 面（要素番号と要素内面番号のペア）グループ
 *  \li 境界節点グループ
 *  \li 境界節点整数値グループ
 *
 *  です。\n
 *
 *  境界節点グループは、境界面にのみ定義されている節点グループで、更新後も
 *  境界面にのみ存在することが保証されます。
 *  ただし、領域分割後の領域境界面に存在する場合を排除していません
 *  （領域境界面は与えないことになっている）ので、ソルバの呼び出し側で
 *  領域境界面にある境界条件を削除する必要があります。\n
 *
 *  境界節点整数値グループは、境界面にのみ定義されている、節点上の分布を与えるものです。
 *  FrontFlow/blue の境界条件を更新するために、整数値で境界条件を更新するときに優先順位を
 *  記述します。中間節点に与える整数値についてのルールは以下の通りです。
 *
 *  \li もとの節点なかに変数が与えられていない節点があれば、中間節点には変数は与えない。
 *  \li もとの節点に変数が与えられていて、変数の値がすべて等しい時は、中間節点にその等しい値を与える。
 *  \li もとの節点に変数が与えられていて、変数の値が異なる時は、中間節点に最も小さい値を与える。
 *
 *  rcapRefineElement を呼び出す前に rcapAppendNodeGroup rcapAppendElementGroup rcapAppendFaceGroup
 *  rcapAppendBNodeGroup rcapAppendBNodeVarInt で
 *  細分時に更新したい条件を登録します。登録する場合の識別子を変えることで複数の条件を登録することもできます。
 *  登録後に rcapRefineElement を実行すると、内部で登録した節点グループ、要素グループ、面グループは更新されます。
 *  rcapCommit を実行すると、rcapAppendBNodeGroup および rcapAppendBNodeVarInt で登録した境界条件が更新されます。
 *  さらに rcapCommit を実行後は細分後のデータの取り出しが可能になります。
 *  細分後のデータを取り出すには、rcapGetNodeGroup rcapGetElementGroup rcapGetFaceGroup rcapGetBNodeGroup rcapBNodeVarInt を使います。
 *  データの個数（節点グループならば節点数）を取得するには rcapGetNodeGroupCount rcapGetElementGroupCount rcapGetFaceGroupCount rcapBnodeGroupCount
 *  rcapGetBNodeVarIntCount で調べることができます。
 *
 *  REVOCAP Refiner では細分対象の境界条件と、細分後の境界条件の両方を内部で保持しています。
 *  rcapGetNodeGroup などで取得できる境界条件は、細分対象の境界条件です。
 *  rcapCommit では細分対象の境界条件を細分対象の境界条件に置き換えます。
 *  すなわち rcapRefineElement を実行後、rcapCommit を実行した後は、それまで細分後の境界条件が、
 *  細分対象の境界条件になるため、rcapGetNodeGroup などで境界条件を取得することができるようになります。
 *  多段階の細分を行う場合は、それぞれの段階の細分が完了した時点で rcapCommit を実行することが必要です。
 *
 *  以下がその例です。
 *
 * \code
 * program RefinerSample
 * ! 途中略
 *   integer(kind=4), dimension(3) :: ng0
 *   integer(kind=4), dimension(:), allocatable :: result_ng0
 *   character(80) :: str
 *   integer(kind=4) :: res
 * ! 途中略
 *   str = "const"//CHAR(0)
 *   ng0 = (/ 1, 2, 3 /)
 *   nodeCount = 3
 *   call rcapAppendNodeGroup(str,nodeCount,ng0)
 *   call rcapRefineElement( elementCount, RCAP_TETRAHEDRON, tetras, refineTetras )
 *   call rcapCommit()
 *   res = rcapGetNodeGroupCount(str)
 *   allocate( result_ng0(res) )
 *   call rcapGetNodeGroup(str,res,result_ng0)
 * ! 途中略
 *   deallocate( result_ng0 )
 *   call rcapTermRefiner()
 * end program RefinerSample
 * \endcode
 *
 *  rcapRefineElement を呼び出した時に、その前に rcapAppendNodeGroup で登録された
 *  節点グループを同時に更新します。
 *  登録するときにつける識別子は C 言語との互換性のため CHAR(0) を最後につけて
 *  呼び出してください。
 *
 * \section howto_refine_edge 要素の面や辺を細分する方法
 *
 *  要素の面や辺を細分する場合は次のようにします。
 *  例えば４面体要素の場合、面を３角形要素、辺を線分要素とみなし、
 *  rcapRefineElement を呼び出します。４面体要素を細分した後に３角形要素を細分した場合、
 *  ３角形要素の中間節点の節点番号は、４面体要素を細分したときに与えられた中間節点の節点番号が用いられます。
 *  逆に面の３角形要素を細分し、その後で４面体要素を細分した場合は、すでに面が３角形要素で細分されている場合には
 *  その面上の中間節点の節点番号は、３角形要素を細分したときに与えられた中間節点の節点番号が用いられます。
 *
 * \section element_id_after_refine 細分後の要素番号について
 *
 *  面グループや要素グループは要素番号を用いて表現されています。
 *  rcapRefineElement で要素を細分してこれらの境界条件を更新すると、それに付随する要素番号
 *  も更新されますが、要素番号の付け方は以下の通りとします。
 *
 *  \li 入力される要素番号は rcapRefineElement を呼ぶ順に elementOffset から順に並んでいるものとする
 *  \li 出力される要素番号は rcapRefineElement を呼ぶ順に elementOffset から順に並んでいるものとする
 *  \li 要素番号は rcapClearRefiner を呼ぶと elementOffset にリセットされる
 *
 *  例えば、elementOffset = 1 で rcapRefineElement を100個の４面体と50個の３角形について呼んだとします。
 *
 * \code
 *   call rcapRefineElement( tetraCount, RCAP_TETRAHEDRON, tetras, refineTetras )
 *   call rcapRefineElement( triCount, RCAP_TRIANGLE, tris, refineTris )
 * \endcode
 *
 *  この場合、細分前の要素番号は４面体が1から100まで、３角形が101から150まで与えられていると解釈し、
 *  細分された４面体の要素番号は1から800まで、３角形の要素番号が801から1000まで与えられていると解釈します。
 *  この要素番号に従って、要素グループと面グループの境界条件を更新します。
 *
 * \section get_middle_point 細分後に中間節点の節点番号を取得する
 *
 *  細分後の中間節点の節点番号は rcapRefineElement の結果から判断できますが、
 *  それ以外に Refiner に問い合わせることによっても可能です。
 *  後から問い合わせる方法は、次の２通りあります。
 *
 *  \li 問い合わせ関数 rcapGetMiddle を呼び出す
 *  \li 要素細分関数 rcapRefineElement を両端の節点番号からなる線分要素について呼び出す
 *
 *  既に細分されて中間節点が存在する場合は、両者の振る舞いは同じです。
 *  細分されていなくて中間節点が存在しない場合は、前者は無効な節点番号 -1 を返しますが、
 *  後者は改めて細分を行い、中間節点を生成してその節点番号を返します。
 *
 * \section get_original_point 細分後に中間節点から親節点（要素）を取得する
 *
 *  細分後に rcapRefineElement の結果から中間節点から親節点を知ることができますが、
 *  その対応は REVOCAP_Refiner 側で覚えているので、呼び出し側で検索用のテーブルなどを
 *  用意しておく必要はありません。rcapGetOriginal で中間節点を与えると、親節点を
 *  取得することができます。
 *  ここで親節点、親要素と呼んでいるものは
 *
 *  \li 元の要素の辺の中点に生成された中間節点の場合：両端の2点を親節点として返します
 *  \li 四角形の中心、または六面体の面の中心に生成された中間節点の場合：四角形を構成する4点（2次要素なら8点）を親要素として返します。六面体の面の中心に生成された中間節点の場合は、ここで与えられる要素は細分された要素ではないことに注意してください。
 *  \li 六面体の中心に生成された中間節点の場合：六面体を構成する8点（2次要素の場合は20点）を親要素として返します。
 *  \li 元の要素の辺ではなく、細分によって生成された中間節点同士をつないで出来て辺の中間節点の場合：両端の２点を親節点として返します。この場合は親節点として返す値は、細分前の節点番号ではないことに注意してください。
 *
 *  である。
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */

int main(void)
{
	/*
	 * 使い方の例
	 * 初めの5つは細分する前の節点座標
	 * 局所的には 1 から順に節点番号が付与されていると解釈する
	 */
	float64_t coords[15] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0
	};
	/*
	 * 細分後の節点番号を格納する
	 */
	float64_t* resultCoords = NULL;
	/* 節点番号の GlobalId */
	int32_t globalIds[5] = { 100, 101, 102, 103, 104 };
	/* メッシュの節点配列に現れる節点番号の最初の値
	 * 通常 C から呼ぶときは 0 fortran から呼ぶ場合は 1
	 * ここでは C から利用しているが、あえて 1 ずらした場合を考える
	 */
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* 四面体の節点配列：入力は2個 */
	int32_t tetras[8] = {
		1, 2, 3, 4,
		2, 3, 4, 5
	};
	/* 細分後の四面体の節点配列：出力は2*8=16個*/
	int32_t* refineTetras = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_TETRAHEDRON;
	/* 初期節点の個数 */
	size_t nodeCount = 5;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineElementCount = 0;
	/* 細分後の節点の個数 */
	size_t refineNodeCount = 0;
	/* 要素の細分と同時に更新する節点グループ */
	int32_t ng0[3] = {1,2,3};
	int32_t* result_ng0 = NULL;
	size_t ngCount = 3;
	/* 要素の細分と同時に更新する面グループ */
	/* 要素番号と要素内面番号の順に交互に並べる */
	int32_t fg0[4] = {1,3,2,1};   /* [1,2,3] と [2,5,4] */
	int32_t result_fg0[12];
	size_t fgCount = 2;
	/* 要素の細分と同時に更新する要素グループ */
	int32_t eg0[1] = {2};
	int32_t* result_eg0 = NULL;
	size_t egCount = 1;
	/* 関数の戻り値格納用 */
	int32_t seg[2] = {-1,-1};
	int8_t orgtype = RCAP_UNKNOWNTYPE;
	int32_t middle = -1;
	int32_t flag = 0;
	/* ループのカウンタ */
	int32_t i = 0;
	int32_t j = 0;

	/* サイズのチェック */
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
	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* globalId と座標値を Refiner に教える
	 * localIds は NULL をあたえると coords は nodeOffset から順番に並んでいるものと解釈する
	 */
	rcapSetNode64( nodeCount, coords, globalIds, NULL );
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	printf("Original Node Count = %u\n", nodeCount );
	/* 細分前の要素数 */
	printf("Original Element Count = %u\n", elementCount );
	/* 節点グループの登録 */
	rcapAppendNodeGroup("innovate",ngCount,ng0);
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Original node group count %u\n", ngCount );
	/* 面グループの登録 */
	rcapAppendFaceGroup("revolute",fgCount,fg0);
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Original face group count %u\n", fgCount );
	/* 要素グループの登録 */
	rcapAppendElementGroup("eg",egCount,eg0);
	egCount = rcapGetElementGroupCount("eg");
	printf("Original element group count %u\n", egCount );
	/* 要素の細分 */
	refineElementCount = rcapRefineElement( elementCount, etype, tetras, NULL );
	refineTetras = (int32_t*)calloc( 4*refineElementCount, sizeof(int32_t) );
	elementCount = rcapRefineElement( elementCount, etype, tetras, refineTetras );
	printf("Refined Element Count = %u\n", refineElementCount );
	if( elementCount != refineElementCount ){
		printf("Refine Element Error! different elelment count\n");
	}
	rcapCommit();
	/* 中間節点のチェック */
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
	/* 細分後の節点グループの更新 */
	ngCount = rcapGetNodeGroupCount("innovate");
	printf("Refined node group count %u\n", ngCount );
	if( ngCount > 0 ){
		result_ng0 = (int32_t*)calloc( ngCount, sizeof(int32_t) );
		rcapGetNodeGroup("innovate",ngCount,result_ng0);
		/* 細分後の節点グループのチェック */
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

	/* 細分後の要素グループの更新 */
	egCount = rcapGetElementGroupCount("eg");
	printf("Refined element group count %u\n", egCount );
	if( egCount > 0 ){
		result_eg0 = (int32_t*)calloc( egCount, sizeof(int32_t) );
		rcapGetElementGroup("eg",egCount,result_eg0);
		/* 細分後の要素グループのチェック */
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

	/* 細分後の面グループの更新 */
	fgCount = rcapGetFaceGroupCount("revolute");
	printf("Refined face group count %u\n", fgCount );
	rcapGetFaceGroup("revolute",fgCount,result_fg0);
	/* 細分後の面グループのチェック */
	if( fgCount != 8 ){
		printf( "Refine Face Group Error! Bad Face Group Count\n");
	}

	/* 細分後の節点の個数 */
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
