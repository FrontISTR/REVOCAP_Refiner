/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapMeshDumpIO                                       #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include <cstdio>

namespace kmb{

class MeshData;
class DataBindings;

class RevocapMeshDumpIO
{
public:

	struct SizeInfo{
		unsigned char nodeCount;
		unsigned char nodeId;
		unsigned char xyz;
		unsigned char bodyCount;
		unsigned char bodyId;
		unsigned char elementCount;
		unsigned char elementId;
		unsigned char elementType;
		unsigned char dataCount;
		unsigned char bindingMode;
		unsigned char idCount;
		unsigned char valueType;
		unsigned char value;
		unsigned char faceId;
	};

	RevocapMeshDumpIO(void);
	~RevocapMeshDumpIO(void);

	static bool checkTypeSize(void);
	int loadFromFile(const char* filename,kmb::MeshData* mesh);
	int saveToFile(const char* filename,const kmb::MeshData* mesh);
	int loadFromFileNeglectingElementId(const char* filename,kmb::MeshData* mesh);
	int saveToFileNeglectingElementId(const char* filename,const kmb::MeshData* mesh);
private:
	static const char* headerString;
protected:
	FILE* fp;
	struct SizeInfo sInfo;
	bool getVersion(int &ver);
	bool setVersion(const int ver);
	int loadNodeData(kmb::MeshData* mesh);
	int saveNodeData(const kmb::MeshData* mesh);
	int loadElementData(kmb::MeshData* mesh);
	int saveElementData(const kmb::MeshData* mesh);
	int loadElementDataNeglectingId(kmb::MeshData* mesh);
	int saveElementDataNeglectingId(const kmb::MeshData* mesh);
	int loadBindingData(kmb::MeshData* mesh);
	int saveBindingData(const kmb::MeshData* mesh);


	int loadBindingDataId(kmb::DataBindings** dataVec,int num,size_t idcount);
	int saveBindingDataId(kmb::DataBindings** dataVec,int num,size_t idcount);
	int loadValue(kmb::DataBindings* data);
	int saveValue(kmb::DataBindings* data);
	template<typename T>
	int loadValueAtId(kmb::DataBindings* data,T t);
	template<typename T>
	int saveValueAtId(kmb::DataBindings* data,T t);
};

}
