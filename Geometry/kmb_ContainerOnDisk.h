/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ContainerOnDisk                                         #
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
 * バイナリファイル上にデータを格納してストリームでデータを走査する
 * ファイルアクセスの排他処理がまだ（ファイル名をインスタンスと1対1対応にすれば良い）
 *
 * ファイル：ヘッダの仕様
 * MAGIC NUMBER        (4byte)  char * 4
 * 文字列              (80byte) char * 80
 * ヘッダの大きさ      (4byte)  unsinged int : 4 + 80 + 4 + 4 + 4
 * Unit の大きさ       (4byte)  unsigned int : double*3 ならば 8*3 = 24 可変の場合は 0
 * Unit の個数         (4byte)  unsigned int
 */

#pragma once

#include <fstream>

namespace kmb{

class ContainerOnDisk
{
private:
	static unsigned int HEADER_MAGIC_NUMBER;
	unsigned int headerSize;
	std::fstream fileBindings;
protected:
	unsigned int dataUnitSize;
	unsigned long dataSize;
	unsigned long indexCounter;
	long maxIndex;
public:
	ContainerOnDisk(void);
	virtual ~ContainerOnDisk(void);
	bool initialize(const char* filename, const char headerString[80],unsigned int dataUnitSize,unsigned long dataSize);
	void terminate(void);


	bool setData( unsigned long index, const char* data, unsigned long num=1 );
	bool getData( unsigned long index, char* data, unsigned long num=1 ) const;

	unsigned long getSize(void) const;
	long getMaxIndex(void) const;
	bool good(void) const;
	void debug(void);
};

}
