/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FFbIO                                                   #
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

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "MeshDB/kmbElement.h"
#include "RevocapIO/kmbFortranIO.h"

namespace kmb{

class MeshData;

class FFbIO : public FortranIO
{
private:
	enum boundaryType{
		INLET,
		FREE,
		WALL,
		MOVINGWALL,
		SYMMETRIC,
		CYCLIC,
		TEMP,
		HEAT,
		BODY,
		CUSTOM,
		A_WALL,
		A_NRBC,
		INTERCONNECT
	};
	std::vector< kmb::nodeIdType > inlet;
	std::vector< kmb::nodeIdType > movingWall;
	std::vector< kmb::nodeIdType > temp;
	std::vector< kmb::nodeIdType > heat;
	bool readPost;

	std::string header;


	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;
public:
	FFbIO(void);
	virtual ~FFbIO(void);
	int loadFromMeshFile(const char* filename,kmb::MeshData* mesh);
	int loadFromBoundaryFile(const char* filename,kmb::MeshData* mesh);

	int loadPostStep(const char* filename,kmb::MeshData* mesh,int step=-1);
	const char* loadHeader(const char* filename);


	int saveToMeshFile(const char* filename,const kmb::MeshData* mesh);
	int saveToMeshFileVer6(const char* filename,const kmb::MeshData* mesh);


	int saveToBoundaryFile(const char* filename,const kmb::MeshData* mesh);
	int saveToBoundaryFileAcoustic(const char* filename,const kmb::MeshData* mesh);
protected:



	bool checkFormat(std::ifstream &input);

	int readHeader(std::ifstream &input,kmb::MeshData* mesh);
	int readHeader(std::ifstream &input,std::string &str);
	int writeHeader(std::ofstream &output,const kmb::MeshData* mesh);
	int writeFooter(std::ofstream &output,const kmb::MeshData* mesh);

	int readNewSet(std::ifstream &input,kmb::MeshData* mesh);
	int writeNewSet(std::ofstream &output,const kmb::MeshData* mesh);

	int readGrid3D(std::ifstream &input,kmb::MeshData* mesh);
	int writeGrid3D(std::ofstream &output,const kmb::MeshData* mesh);

	int readNode2D(std::ifstream &input,kmb::MeshData* mesh);
	int readNode3D(std::ifstream &input,kmb::MeshData* mesh);
	int writeNode2D(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	int writeNode3D(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	int writeNode2DVer6(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	int writeNode3DVer6(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId);




	int readBC(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int readBCV(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int readBC3D(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int readBCINTR(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name);
	int writeBC(std::ofstream &output,const kmb::MeshData* mesh,boundaryType btype,const char* name="BC_INLT");

	int readPres2E(std::ifstream &input,kmb::MeshData* mesh);
	int readPres3E(std::ifstream &input,kmb::MeshData* mesh);
	int readPres3D(std::ifstream &input,kmb::MeshData* mesh);

	int readVelo3D(std::ifstream &input,kmb::MeshData* mesh);
	int readVelo2D(std::ifstream &input,kmb::MeshData* mesh);

	int readHeaderFlt(std::ifstream &input,kmb::MeshData* mesh);
	int readHeaderInt(std::ifstream &input,kmb::MeshData* mesh);
	int writeHeaderFlt(std::ofstream &output,const kmb::MeshData* mesh,const char* name,const char* comment);
	int writeHeaderInt(std::ofstream &output,const kmb::MeshData* mesh,const char* name,const char* comment);
};

}
