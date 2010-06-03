/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : MicroAVSIO                                              #
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

#pragma once

#include "MeshDB/kmbTypes.h"
#include <fstream>

namespace kmb{

class MeshData;

class MicroAVSIO
{
private:
	enum dataType{
		DATA = 0x01,
		GEOM = 0x02,
		DATA_GEOM = 0x03,
		UNDEFINED
	};
	dataType dtype;
	kmb::nodeIdType nodeOffset;
	kmb::elementIdType elementOffset;

	bool asVector3;

	int version;
	int nodeCount;
	int elementCount;
	int nodeDimCount;
	int elementDimCount;
protected:



	int getVersion(std::ifstream &input);

	int readHeader(std::ifstream &input);
	int skipGeom(std::ifstream &input);
	int readGeom(std::ifstream &input,kmb::MeshData* mesh);
	int readData(std::ifstream &input,kmb::MeshData* mesh);

	void getDataName(std::string &line);
public:
	MicroAVSIO(void);
	virtual ~MicroAVSIO(void);
	int loadFromFile(const char* filename,kmb::MeshData* mesh);
	int loadPostFromFile(const char* filename,kmb::MeshData* mesh);

	int saveToFile(const char* filename,kmb::MeshData* mesh);

	void setReadAsVector3( bool flag );
};

}
