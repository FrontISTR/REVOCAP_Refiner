/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : OffIO                                                   #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2015/12/15     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include "RevocapIO/kmbOffIO.h"

#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbMeshData.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

int kmb::OffIO::loadFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ifstream input( filename, std::ios_base::in );
	if( input.fail() ){
		return -1;
	}
	std::string line;
	std::getline( input, line );
	if( line.find("OFF") != 0 ){
		return -1;
	}
	unsigned int nodeCount = 0;
	unsigned int polygonCount = 0;
	unsigned int edgeCount = 0;
	do{ std::getline( input, line ); }while(line.size()==0 || line[0]=='#');
	std::istringstream tokenizer(line);
	tokenizer >> nodeCount >> polygonCount >> edgeCount;
	double x=0.0,y=0.0,z=0.0;
	mesh->beginNode( nodeCount );
	for(unsigned int i = 0;i<nodeCount;++i){
		do{ std::getline( input, line ); }while(!input.eof() && (line.size()==0 || line[0]=='#'));
		tokenizer.str(line);
		tokenizer.clear();
		tokenizer >> x >> y >> z;
		mesh->addNode(x,y,z);
	}
	mesh->endNode();
	int vertexCount;
	kmb::nodeIdType nodes[4] = {kmb::nullNodeId};
	mesh->beginElement( polygonCount );
	for(unsigned int i = 0;i<polygonCount;++i){
		do{ std::getline( input, line ); }while(!input.eof() && (line.size()==0 || line[0]=='#'));
		tokenizer.str(line);
		tokenizer.clear();
		tokenizer >> vertexCount;
		if( vertexCount == 3 ){
			tokenizer >> nodes[0] >> nodes[1] >> nodes[2];
			mesh->addElement( kmb::TRIANGLE, nodes );
		}else if( vertexCount == 4 ){
			tokenizer >> nodes[0] >> nodes[1] >> nodes[2] >> nodes[3];
			mesh->addElement( kmb::QUAD, nodes );
		}
	}
	mesh->endElement();
	input.close();
	return 0;
}

int kmb::OffIO::saveFile(const char* filename,const kmb::MeshData* mesh)
{
	if( mesh == NULL || mesh->getNodes() == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	output << "OFF" << std::endl;
	int polygonCount = 0;
	kmb::bodyIdType bodyCount = mesh->getBodyCount();
	for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
		const kmb::Body* body = mesh->getBodyPtr(bodyId);
		polygonCount += body->getCountByType(kmb::TRIANGLE) + body->getCountByType(kmb::QUAD);
	}
	output << mesh->getNodeCount() << " " << polygonCount << " " << 0 << std::endl;
	output << std::endl;
	kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
	kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
	while( nIter != nIterEnd ){
		output << std::setprecision(8) << nIter.x() << " " << nIter.y() << " " << nIter.z() << std::endl;
		++nIter;
	}
	for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
		const kmb::Body* body = mesh->getBodyPtr(bodyId);
		kmb::ElementContainer::const_iterator eIter = body->begin();
		while( eIter != body->end() ){
			if( eIter.getType() == kmb::TRIANGLE ){
				output << eIter[0] << " " << eIter[1] << " " << eIter[2] << std::endl;
			}else if( eIter.getType() == kmb::QUAD ){
				output << eIter[0] << " " << eIter[1] << " " << eIter[2] << " " << eIter[3] << std::endl;
			}
			++eIter;
		}
	}
	return 0;
}
