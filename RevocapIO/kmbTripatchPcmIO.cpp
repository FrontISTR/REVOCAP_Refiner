/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TripatchPcmIO                                           #
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
#include "RevocapIO/kmbTripatchPcmIO.h"

#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbMeshData.h"

#include <fstream>
#include <sstream>
#include <string>

int
kmb::TripatchPcmIO::loadFromFile(const char* filename,MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		std::string line;
		unsigned int nodeCount = 0;
		unsigned int dummy;
		unsigned int bodyCount = 0;
		{
			std::getline( input, line );
			std::istringstream tokenizer(line);
			tokenizer >> nodeCount >> dummy >> bodyCount;
		}
		size_t i;
		double x=0.0,y=0.0,z=0.0;
		mesh->beginNode( nodeCount );
		for(i = 0;i<nodeCount;++i){
			std::getline( input, line );
			std::istringstream tokenizer(line);
			tokenizer >> x >> y >> z;
			mesh->addNode(x,y,z);
		}
		mesh->endNode();
		unsigned int elementCount = 0;
		kmb::nodeIdType nodes[3] = {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};
		for(unsigned int j=0;j<bodyCount;++j){
			std::getline( input, line );
			std::istringstream tokenizer(line);
			tokenizer >> elementCount >> dummy >> dummy;
			mesh->beginElement( elementCount );
			for(i = 0;i<elementCount;++i){
				std::getline( input, line );
				tokenizer.str(line);
				tokenizer.clear();
				tokenizer >> nodes[0] >> nodes[1] >> nodes[2];
				mesh->addElement( kmb::TRIANGLE, nodes );
			}
			mesh->endElement();
		}
		input.close();
		return 0;
	}
}




int
kmb::TripatchPcmIO::saveToFile(const char* filename,const kmb::MeshData* mesh,int packOption)
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;


	}else{
		int patchCount = 0;
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( !body || !body->isUniqueType( kmb::TRIANGLE ) )
			{
				return -1;
			}
			if( body->getCount() > 0 ){
				++patchCount;
			}
		}
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		if( packOption == 0 ){
			output << mesh->getNodeCount() << " " << 0 << " " << patchCount << std::endl;
		}else if( packOption == 1 ){
			output << mesh->getNodeCount() << " " << 0 << " " << 1 << std::endl;
		}
		kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
		kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
		double x,y,z;
		while( nIter != nIterEnd ){
			if( nIter.getXYZ(x,y,z) ){
				output << x << " " << y << " " << z << std::endl;
			}
			++nIter;
		}
		size_t allElementCount = mesh->getElementCount();
		if( packOption == 1 ){
			output << allElementCount << " " << 0 << " " << 0 << std::endl;
		}
		size_t elementCount = 0;
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->isUniqueType( kmb::TRIANGLE ) && body->getCount() > 0 )
			{
				if( packOption == 0 ){
					elementCount = body->getCount();
					output << elementCount << " " << 0 << " " << 0 << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					output <<
						eIter.getCellId(0) << " " <<
						eIter.getCellId(1) << " " <<
						eIter.getCellId(2) << std::endl;
					++eIter;
				}
			}
		}
	}
	return 0;
}

