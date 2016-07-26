/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FFbIO                                                   #
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
//
// FrontFlow/blue GF1 形式
//
// HISTORY データの読み込み方法
// (1) NEWSET に記述されている文字列を取得
// (2) eachHistory( filename, ["TIME","MAXIMUM DIVERGENT"] ){ |ary| }
//     で TIME と MAXIMUM DIVERGENT の値を Array にして yield する
//

//
// 配列ヘッダとキーワード
//
// #FLT_ARY
//  *GRID_2D  節点座標 (X,Y)
//  *GRID_3D  節点座標 (X,Y,Z)
//  *BC_IV2D  流入速度 (U,V)
//  *BC_IV3D  流入速度 (U,V,W)
//  *BC_WV2D  壁速度   (U,V)
//  *BC_WV3D  壁速度   (U,V,W)
//  *BC_TMPV  境界温度
//  *BC_HFXV  境界熱流束
//  *TIME_PS  時間
//  *VELO_2D  速度     (U,V)
//  *VELO_3D  速度     (U,V,W)
//  *PRES_2E  要素における圧力(2D)
//  *PRES_3E  要素における圧力(3D)
//  *PRES_3D  節点における圧力(3D)
//  *BC_PGET  オーバーセット節点の局所座標  (ξ,ζ,η)
//  *BC_EGET  オーバーセット要素の局所座標  (ξ,ζ,η)
//  *HISTORY  (時間ステップ,流速の発散の最大値,粘性の空間平均値,...)
// #INT_ARY
//  *NODE_2D  要素接続データ(2D)
//  *NODE_3D  要素接続データ(3D)
//  *ELM_TYP  未使用
//  *BC_INLT  流入境界条件節点リスト
//  *BC_WALL  壁境界条件節点リスト
//  *BC_SYMT  対称境界条件節点リスト
//  *BC_CYCL  周期境界条件節点リスト
//  *BC_BODY  流体力計算境界条件節点リスト
//  *BC_FREE  自由境界節点リスト
//  *BC_MWAL  移動境界節点リスト => BCVector
//  *BC_INTR  領域間通信境界節点リスト (節点番号,相手領域番号,相手節点番号)
//  *BC_TMPN  温度境界条件節点リスト => BCTemp
//  *BC_HFXN  熱流束境界条件節点リスト => BCFlux
//  *STEP_PS  時間ステップ
//  *BC_PSET  オーバーセット境界節点リスト (節点番号,要素番号,領域番号)
//  *BC_ESET  オーバーセット境界要素リスト (要素番号,要素番号,領域番号)
//  *ELM_ATRE 座標系番号
//  *PT_NODE  節点数
//  *PT_ELEM  要素数
//  *BC_INTR  領域間通信境界 (節点番号,相手の領域番号,相手の節点番号)

// ポストデータについては読み込まれている要素・節点以上のデータは読まない

#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "MeshDB/kmbElement.h"
#include "RevocapIO/kmbFortranIO.h"

namespace kmb{

class MeshData;

class FFbIO : public FortranIO
{
private:
	enum boundaryType{
		INLET,
		FREE,
		WALL,
		MOVINGWALL,
		SYMMETRIC,
		CYCLIC,
		TEMP,
		HEAT,
		BODY,
		CUSTOM,       // 汎用境界条件設定用
		A_WALL,
		A_NRBC,
		INTERCONNECT
	};
	std::vector< kmb::nodeIdType > inlet;
	std::vector< kmb::nodeIdType > movingWall;
	std::vector< kmb::nodeIdType > temp;
	std::vector< kmb::nodeIdType > heat;
	bool readPost;
	// c_str で返した後も消えないように
	std::string header;
	// 読み込むときに引いて、書き込むときに足す
	// ファイルの id が 1 から始まる時は、これらの offset は 1 にする
	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;
public:
	FFbIO(void);
	virtual ~FFbIO(void);
	int loadFromMeshFile(const char* filename,kmb::MeshData* mesh);
	int loadFromBoundaryFile(const char* filename,kmb::MeshData* mesh);
	// 特定のステップのものを読む step = -1 の時は必ず読む
	int loadPostStep(const char* filename,kmb::MeshData* mesh,int step=-1);
	const char* loadHeader(const char* filename);
	// 戻り値は四面体モデルだったら 4、六面体モデルだったら 6
	// 混合の場合は 8
	int saveToMeshFile(const char* filename,const kmb::MeshData* mesh);
	int saveToMeshFileVer6(const char* filename,const kmb::MeshData* mesh);
	// REVOCAP 独自拡張
	// stype = "CUSTOM" の NodeGroup を BoundaryFile に出力する
	int saveToBoundaryFile(const char* filename,const kmb::MeshData* mesh);
	int saveToBoundaryFileAcoustic(const char* filename,const kmb::MeshData* mesh);
protected:
	// read 系は header の次の行から
	// write 系は header も含めて出力
	// unformatted endian check
	bool checkFormat(std::ifstream &input);
	// #A_GF_V1 or #U_GF_V1 の後のコメント
	int readHeader(std::ifstream &input,kmb::MeshData* mesh);
	int readHeader(std::ifstream &input,std::string &str);
	int writeHeader(std::ofstream &output,const kmb::MeshData* mesh);
	int writeFooter(std::ofstream &output,const kmb::MeshData* mesh);
	// #NEW_SET の後のコメントの解釈
	int readNewSet(std::ifstream &input,kmb::MeshData* mesh);
	int writeNewSet(std::ofstream &output,const kmb::MeshData* mesh);
	// 節点
	int readGrid3D(std::ifstream &input,kmb::MeshData* mesh);
	int writeGrid3D(std::ofstream &output,const kmb::MeshData* mesh);
	// 要素
	int readNode2D(std::ifstream &input,kmb::MeshData* mesh);
	int readNode3D(std::ifstream &input,kmb::MeshData* mesh);
	int writeNode2D(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	int writeNode3D(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	int writeNode2DVer6(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	int writeNode3DVer6(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	// 境界面
	// BC_INLT BC_MWAL : NodeVariable, Vector3
	// BC_FREE BC_WALL BC_CYCL BC_SYMT BCA_WAL BCA_NOR : NodeGroup
	// BC_INTR : NodeVariable, Array of Integer
	int readBC(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int readBCV(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int readBC3D(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int readBCINTR(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int writeBC(std::ofstream &output,const kmb::MeshData* mesh,boundaryType btype,const char* name="BC_INLT");
	// 圧力値
	int readPres2E(std::ifstream &input,kmb::MeshData* mesh);
	int readPres3E(std::ifstream &input,kmb::MeshData* mesh);
	int readPres3D(std::ifstream &input,kmb::MeshData* mesh);
	// 速度
	int readVelo3D(std::ifstream &input,kmb::MeshData* mesh);
	int readVelo2D(std::ifstream &input,kmb::MeshData* mesh);
	// その他の一般的なキーワード対応
	int readHeaderFlt(std::ifstream &input,kmb::MeshData* mesh);
	int readHeaderInt(std::ifstream &input,kmb::MeshData* mesh);
	int writeHeaderFlt(std::ofstream &output,const kmb::MeshData* mesh,const char* name,const char* comment);
	int writeHeaderInt(std::ofstream &output,const kmb::MeshData* mesh,const char* name,const char* comment);
};

}
