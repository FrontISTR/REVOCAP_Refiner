/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : HECMWIO                                                 #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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

#include <string>
#include "MeshDB/kmbElement.h"

namespace kmb{

class MeshData;

class HECMWIO
{
private:
	struct egrpData{
		std::string egrpName;
		std::string materialName;
		size_t egrpCount;
		kmb::bodyIdType bodyId;
	};
public:
	enum solutionType{
		STATIC,
		HEAT,
		EIGEN,
		DYNAMIC
	};
	HECMWIO( kmb::nodeIdType offsetNodeId=1, kmb::elementIdType offsetElementId=1 );
	~HECMWIO(void);

















	int loadFromFile(const char* filename,kmb::MeshData* mesh);
	int loadFromFRSFile(const char* filename,kmb::MeshData* mesh);
	int saveToFile(const char* filename,const kmb::MeshData* mesh);
	solutionType setSolutionType( const char* soltype );
	const char* getSolutionType( void );
protected:

	static std::string getValue( std::string exp, std::string key );


	static int getHECType( kmb::elementType etype );
	static kmb::elementType getRevocapType( int etype );


	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;
	solutionType soltype;
};

}
