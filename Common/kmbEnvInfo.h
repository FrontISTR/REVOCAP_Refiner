/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : EnvInfo                                                 #
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
#include <algorithm>

namespace kmb{

class EnvInfo
{
public:
	enum endianType{
		kBigEndian,
		kLittleEndian
	};
	static endianType checkEndian(void);
	template<typename T>
	static void reverse_endian(T* p)
	{
		std::reverse(
			reinterpret_cast<uint8_t*>(p),
			reinterpret_cast<uint8_t*>(p) + sizeof(T)
		);
	}	
	static void printCVersion(void);
	static void printSize(void);
	static void getCurrentTimeString(std::string& str);
};

}
