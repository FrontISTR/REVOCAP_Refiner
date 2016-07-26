/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 2.0                          #
# Class Name : StringUtils                                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2015/02/20     #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once
#include <string>

namespace kmb{

class StringUtils{
public:
	// 文字列 str の from を to に置換する
	// パス区切り文字の一括変換に用いる
	static void replaceString(std::string& str,std::string from,std::string to){
		std::string::size_type pos( str.find( from ) );
		while( pos != std::string::npos )
		{
			str.replace( pos, from.length(), to );
			pos = str.find( from, pos + to.length() );
		}
	}

	// 文字列 str の最後に現れる sep 以下を削除
	static void chompString(std::string& str,std::string sep="\n\r"){
		std::string::size_type pos( str.rfind( sep ) );
		if( pos != std::string::npos )
		{
			str.replace( pos, str.length() - pos, "");
		}
	}
};

}
