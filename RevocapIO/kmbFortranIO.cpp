/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FortranIO                                               #
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
#include "RevocapIO/kmbFortranIO.h"

#ifdef WIN32
 typedef unsigned char uint8_t;
 #if _MSC_VER >= 1400
  #define sprintf sprintf_s
  #define strcpy strcpy_s
  #define strcat strcat_s
 #endif
#else
#include <stdint.h>
#endif

#include <cstdio>
#include <cstring>

kmb::FortranIO::FortranIO(void)
: unformatFlag(true)
, endianFlag(false)
{
}

kmb::FortranIO::~FortranIO(void)
{
}

void
kmb::FortranIO::setReverseEndian(bool flag)
{
	endianFlag = flag;
}

bool
kmb::FortranIO::getReverseEndian(void) const
{
	return endianFlag;
}


int
kmb::FortranIO::readString(std::ifstream &input, std::string &str)
{
	if( unformatFlag ){
		int size = 0;
		int footer = 0;
		input.read(reinterpret_cast<char*>(&size),sizeof(int));
		if( endianFlag ){
			std::reverse( reinterpret_cast<uint8_t*>(&size),
				reinterpret_cast<uint8_t*>(&size) + sizeof(int));
		}
		char* buf = new char[size+1];
		input.read(buf,size);
		buf[size] = '\0';
		str = buf;
		input.read(reinterpret_cast<char*>(&footer),sizeof(int));
		delete[] buf;
		return size;
	}else{
		std::getline( input, str );
		if( str.length() > 0 && str.at( str.length()-1 ) == '\r' ){
			str.erase( str.length()-1 ,1 );
		}
		return static_cast<int>( str.size() );
	}
}

void
kmb::FortranIO::strip(std::string &str)
{
	std::string whitespaces (" \t\f\v\n\r");
	size_t found=str.find_last_not_of(whitespaces);
	if (found!=std::string::npos){
		str.erase(found+1);
	}else{
		str.clear();
	}
	found=str.find_first_not_of(whitespaces);
	if (found!=std::string::npos){
		str.erase(0,found);
	}
}

int
kmb::FortranIO::writeFloat(std::ofstream &output,float f)
{
	char buf[32];
	char pow[8];
	sprintf( buf, " % 8.6E", f );
	if( strlen(buf) == 15 ){
		strcpy(pow,(&buf[13]));
		buf[12] = '\0';
		strcat(buf,pow);
	}
	output << buf;







	return 0;
}

