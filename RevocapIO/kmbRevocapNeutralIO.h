/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapNeutralIO                                        #
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
/*
 * ここでは YAML 形式で書かれた REVOCAP Neutral Yaml Mesh Format を
 * YAML ライブラリを使わずに読み込むメソッドを実装する
 *
 * このバージョンでは、記述されているタグの出てくる順番が決め打ちになっているので注意
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
#include <iostream>
#include <sstream>
#include <string>

namespace kmb{

class MeshData;

class RevocapNeutralIO
{
public:
	RevocapNeutralIO(void);
	virtual ~RevocapNeutralIO(void);
	int loadFromRNFFile(const char* filename,kmb::MeshData* mesh);
	int saveToRNFFile(const char* filename,kmb::MeshData* mesh);
	int saveHeader(const char* filename);
	int appendHeader(const char* filename);
	int appendDataToRNFFile(const char* filename,kmb::MeshData* mesh,const char* name,const char* stype=NULL);

	template <typename T> int loadParameterFromRNFFile(const char* filename,int num, const char** tags,T &param)
	{
		if( tags == NULL || num <= 0 ){
			std::cerr << "loadParameterFromRNFFile argument error" << std::endl;
			return -1;
		}
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			std::cerr << "loadParameterFromRNFFile file error" << std::endl;
			return -1;
		}
		int res = readParam(input,num,tags,param);
		input.close();
		return res;
	}
protected:
	int writeHeader(std::ofstream &output);
	int readNode(std::ifstream &input,kmb::MeshData* mesh);
	int readElement(std::ifstream &input,kmb::MeshData* mesh);
	int readData(std::ifstream &input,kmb::MeshData* mesh);
	int readShape(std::ifstream &input,kmb::MeshData* mesh);
	template <typename T> int readParam(std::ifstream &input,int num, const char** tags,T &param)
	{
		std::string line;
		std::string token;
		std::string tag(tags[0]);
		tag.append(":");
		while( !std::getline( input, line ).eof() ){
			if( line.size() == 0 || line[0] == '#' ){
				continue;
			}
			std::istringstream tokenizer(line);
			tokenizer >> token;
			if( token == tag ){
				if( num == 1 ){
					tokenizer >> param;
					return 0;
				}else{
					return readParam(input,num-1,&tags[1],param);
				}
			}
		}
		return -1;
	}
};

}
