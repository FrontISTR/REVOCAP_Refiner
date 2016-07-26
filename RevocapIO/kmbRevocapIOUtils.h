/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapIOUtils                                          #
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

#include "Common/kmbCommon.h"

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace kmb{

class RevocapIOUtils
{
public:
	template<typename T> static T reverseEndian(T t){
		std::reverse(
			reinterpret_cast<uint8_t*>(&t),
			reinterpret_cast<uint8_t*>(&t) + sizeof(t)
		);
		return t;
	}
	// "key0 = val0, key1 = val1, ..." という文字列で key0 を与えて val0 を返す
	// keyCaseSensitive = false の時は、key を小文字に変えて調べる
	static std::string getValue( std::string exp, std::string key, bool keyCaseSensitive=true );
	// "key0, key1 = val1, ..." という文字列で key0 または key1 があるかどうかを調べる
	// keyCaseSensitive = false の時は、key を小文字に変えて調べる
	static bool hasKey( std::string exp, std::string key, bool keyCaseSensitive=true );
	// stl の stl::getline の safe 版
	// linux 環境で \r が残ってしまうバグを除去
	static std::istream& readOneLine(std::istream &input, std::string &str);
	// コンマとスペースをスキップする
	static std::istringstream& skipComma(std::istringstream &tokenizer);
};

}
