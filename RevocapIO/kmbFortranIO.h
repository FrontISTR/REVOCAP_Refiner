/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : FortranIO                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/




#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "Geometry/kmb_Common.h"

namespace kmb{

class FortranIO
{
public:
	template<typename T>
	static void reverse_endian(T* p)
	{
		std::reverse(
			reinterpret_cast<uint8_t*>(p),
			reinterpret_cast<uint8_t*>(p) + sizeof(T)
		);
	}
protected:
	bool unformatFlag;
	bool endianFlag;
	template<typename T> int readArray(std::ifstream &input, std::vector<T> &val){
		val.clear();
		int size = 0;
		input.read(reinterpret_cast<char*>(&size),sizeof(int));
		if( endianFlag ) reverse_endian<int>(&size);
		const int len = size / sizeof(T);
		val.reserve(len);
		T value = 0;
		for(int i=0;i<len;++i){
			input.read(reinterpret_cast<char*>(&value),sizeof(T));
			if( endianFlag ) reverse_endian<T>(&value);
			val.push_back(value);
		}
		input.read(reinterpret_cast<char*>(&size),sizeof(int));
		return len;
	}
	virtual int readString(std::ifstream &input, std::string &str);

	void strip(std::string &str);

	int writeFloat(std::ofstream &output,float f);
public:
	FortranIO(void);
	virtual ~FortranIO(void);
	void setReverseEndian(bool flag);
	bool getReverseEndian(void);
};

}
