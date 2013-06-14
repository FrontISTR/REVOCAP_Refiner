/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : VtkIO                                                   #
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

namespace kmb{

class MeshData;
class DataBindings;

class VtkIO
{
public:
	VtkIO(void);
	~VtkIO(void);
	int loadFromFile(const char* filename,kmb::MeshData* mesh);
	int saveToFile(const char* filename,const kmb::MeshData* mesh);
	int saveToStructuredPointsFile_V3(const char* filename,int nx, int ny, int nz,const kmb::DataBindings* data);
protected:
	int loadFromFile_V4(const char* filename,kmb::MeshData* mesh);



	int saveToLegacyFile_V3(const char* filename,const kmb::MeshData* mesh,int dim=3,const char* header=NULL);
	int saveToSerialXMLFile(const char* filename,const kmb::MeshData* mesh);
	int saveToParallelXMLFile(const char* filename,const kmb::MeshData* mesh);
};

}
