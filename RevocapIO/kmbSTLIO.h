/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : STLIO                                                   #
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

namespace kmb{

class MeshData;

class STLIO
{
private:
	enum fileType{
		BINARY,
		ASCII,
		UNKNOWN
	};
	double thresh;
public:
	STLIO(void);
	~STLIO(void);
	int loadFromFile(const char* filename,kmb::MeshData* mesh);
	int saveToFile(const char* filename,const kmb::MeshData* mesh);
protected:
	fileType checkFormat(const char* filename);
	int loadFromBinaryFile(const char* filename,kmb::MeshData* mesh);
	int loadFromAsciiFile(const char* filename,kmb::MeshData* mesh);
};

}
