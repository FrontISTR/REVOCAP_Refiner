/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MicroAVSIO                                              #
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
// tetra の順序が REVOCAP と逆であることに注意すること
//
// REVOCAP AVS
// 0       1
// 1       2
// 3       3
// 2       4
#pragma once

#include "MeshDB/kmbTypes.h"
#include <fstream>

namespace kmb{

class MeshData;
#ifndef REVOCAP_SUPPORT_RUBY
class StructuredGrid;
#endif

class MicroAVSIO
{
private:
	enum dataType{
		DATA = 0x01,
		GEOM = 0x02,
		DATA_GEOM = 0x03,
		Undefined
	};
	dataType dtype;
	kmb::nodeIdType nodeOffset;
	kmb::elementIdType elementOffset;
	// ベクトル値として解釈
	bool asVector3;
	bool summary;
	// avs ファイルのバージョン
	int version;
	int nodeCount;
	int elementCount;
	int nodeDimCount;
	int elementDimCount;
protected:
	// 最初のコメント以外の行が１行の時、ver 9
	// それ以外の時は ver 8
	// -1 ならエラー
	int getVersion(std::ifstream &input) const;
	// ステップ数を返す
	int readHeader(std::ifstream &input);
	int skipGeom(std::ifstream &input);
	int readGeom(std::ifstream &input,kmb::MeshData* mesh);
	int readData(std::ifstream &input,kmb::MeshData* mesh);
	// データの名前を抽出する
	void getDataName(std::string &line);
#ifndef REVOCAP_SUPPORT_RUBY
	// Field データ参照型
	int readCoord(int n,std::string filename,std::string filetype,int skip,int offset,int stride,kmb::StructuredGrid* grid);
	int readVariable(std::string label,int n,std::string filename,std::string filetype,int skip,int offset,int stride,kmb::StructuredGrid* grid);
#endif
public:
	MicroAVSIO(void);
	virtual ~MicroAVSIO(void);
	int loadFromFile(const char* filename,kmb::MeshData* mesh);
#ifndef REVOCAP_SUPPORT_RUBY
	int loadFromFile(const char* filename,kmb::StructuredGrid* grid);
#endif
	// grid は読まずに data だけ読む
	int loadPostFromFile(const char* filename,kmb::MeshData* mesh);
	int saveToFile(const char* filename,kmb::MeshData* mesh);
	int saveToFile_V8(const char* filename,kmb::MeshData* mesh);
	// 格納されているデータが 3 つしかない時に自動的にベクトル値にする
	void setReadAsVector3( bool flag );
	// FrontISTR の MicroAVS 形式を自動的にベクトル値にする
	void setSummary( bool flag );
};

}
