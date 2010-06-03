/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : RevocapNeutralIO                                        #
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
 * ここでは YAML 形式で書かれた REVOCAP Neutral Yaml Mesh Format を
 * YAML ライブラリを使わずに読み込むメソッドを実装する
 *
 * このバージョンでは、記述されているタグの出てくる順番が決め打ちになっているの注意
 */
/**
 * REVOCAP Neutral Yaml Mesh Format 仕様
 * # が先頭の行はコメント
 * node タグは節点
 * node/size は節点の個数
 * node/coordinate は座標配列
 * node/coordinate[i] は [id,x,y,z]
 * bodyCount は要素グループの個数
 * element タグは要素グループの配列
 * element[i] は要素グループ
 * element[i]/size は要素グループに含まれる要素
 * element[i]/connectivity は要素グループに含まれる要素の節点配列
 * element[i]/connectivity[j] は [id,type,node[0],node[1],..,node[k]]
 */
#pragma once

#include <fstream>

namespace kmb{

class MeshData;

class RevocapNeutralIO
{
public:
	RevocapNeutralIO(void);
	virtual ~RevocapNeutralIO(void);
	int loadFromRNFFile(const char* filename,kmb::MeshData* mesh);
	int saveToRNFFile(const char* filename,kmb::MeshData* mesh);
protected:
	int readNode(std::ifstream &input,kmb::MeshData* mesh);
	int readElement(std::ifstream &input,kmb::MeshData* mesh);
	int readData(std::ifstream &input,kmb::MeshData* mesh);
	int readShape(std::ifstream &input,kmb::MeshData* mesh);
};

}
