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

#include "Common/kmbEnvInfo.h"
#include "Common/kmbCommon.h"

#include <cstdio>
#include <ctime>
#include <iostream>

kmb::EnvInfo::endianType kmb::EnvInfo::checkEndian(void)
{
	uint16_t n = 1;
	uint8_t* p = reinterpret_cast<uint8_t*>(&n);
	if( p[0] == 0 ){
		std::cout << "big endian" << std::endl;
		return kmb::EnvInfo::kBigEndian;
	}else{
		std::cout << "little endian" << std::endl;
		return kmb::EnvInfo::kLittleEndian;
	}
}

void
kmb::EnvInfo::printCVersion(void)
{
	printf("----- Environmental Information -----\n");
	/* C Version check */
#ifdef __STDC__
	printf("__STDC__ = %d\n",__STDC__);
#else
	printf("__STDC__ is undefined\n");
#endif

#ifdef __STDC_HOSTED__
	printf("__STDC_HOSTED__ = %d\n",__STDC_HOSTED__);
#else
	printf("__STDC_HOSTED__ is undefined\n");
#endif

#ifdef __STDC_IEC_559__
	printf("__STDC_IEC_559__ = %d\n",__STDC_IEC_559__);
#else
	printf("__STDC_IEC_559__ is undefined\n");
#endif

#ifdef __STDC_IEC_559_COMPLEX__
	printf("__STDC_IEC_559_COMPLEX__ = %d\n",__STDC_IEC_559_COMPLEX__);
#else
	printf("__STDC_IEC_559_COMPLEX__ is undefined\n");
#endif

#ifdef __STDC_ISO_10646__
	printf("__STDC_ISO_10646__ = %ld\n",__STDC_ISO_10646__);
#else
	printf("__STDC_ISO_10646__ is undefined\n");
#endif

#ifdef __STDC_VERSION__
	printf("__STDC_VERSION__ = %ld\n",__STDC_VERSION__);
#else
	printf("__STDC_VERSION__ is undefined\n");
#endif
}

void
kmb::EnvInfo::printSize(void)
{
	std::cout << "sizeof(int) = " << sizeof(int) << std::endl;
	std::cout << "sizeof(long) = " << sizeof(long) << std::endl;
	std::cout << "sizeof(size_t) = " << sizeof(size_t) << std::endl;
	std::cout << "sizeof(float) = " << sizeof(float) << std::endl;
	std::cout << "sizeof(double) = " << sizeof(double) << std::endl;
}

void kmb::EnvInfo::getCurrentTimeString(std::string& str)
{
	std::time_t t = std::time(NULL);
#if defined WIN32 && _MSC_VER >= 1400
	std::tm tm;
	localtime_s(&tm,&t);
#else
	std::tm tm = *std::localtime(&t);
#endif
	char mbstr[100];
	std::strftime( mbstr, 100, "%Y/%m/%d %H:%M:%S", &tm );
	str.clear();
	str.append( mbstr );
}
