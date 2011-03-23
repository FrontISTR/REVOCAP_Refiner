/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : HecmwIO                                                 #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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

class HecmwIO
{
public:
	enum solutionType{
		STATIC,
		HEAT,
		EIGEN,
		DYNAMIC
	};
private:
	struct egrpData{
		std::string egrpName;
		std::string materialName;
		size_t egrpCount;
		kmb::bodyIdType bodyId;
	};


	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;
	solutionType soltype;
	bool dataFlag;
	bool resElementFlag;
	bool dummySectionFlag;
	std::string resheader;
public:
	HecmwIO( kmb::nodeIdType offsetNodeId=1, kmb::elementIdType offsetElementId=1);
	~HecmwIO(void);

















	int loadFromFile(const char* filename,kmb::MeshData* mesh) const;
	int loadFromFRSFile(const char* filename,kmb::MeshData* mesh) const;
	int loadFromResFile(const char* filename,kmb::MeshData* mesh) const;
	int saveToFile(const char* filename,const kmb::MeshData* mesh) const;
	int saveToResFile(const char* filename,const kmb::MeshData* mesh) const;
	int saveToFileMW3(const char* filename,const kmb::MeshData* mesh,const char* partName) const;



	int appendEquationToFile(const char* filename,const kmb::MeshData* mesh,const char* master,const char* slave,double thresh=1.0e-10,double outThresh=-1.0e-6) const;

	int appendSgroupToFile(const char* filename,const kmb::MeshData* mesh,const char* sgroup,const char* stype=NULL) const;
	int appendFooterToFile(const char* filename) const;
	solutionType setSolutionType( const char* soltype );
	const char* getSolutionType( void ) const;

	void setDataFlag(bool flag);

	void setResElementFlag(bool flag);

	void setDummySectionFlag(bool flag);

	void setResHeader(const char* header);
	void setOffsetElementId(kmb::elementIdType offset);
	void setOffsetNodeId(kmb::nodeIdType offset);
	kmb::elementIdType getOffsetElementId(void) const;
	kmb::nodeIdType getOffsetNodeId(void) const;
protected:


	static int getHECType( kmb::elementType etype );
	static kmb::elementType getRevocapType( int etype );
};

}
