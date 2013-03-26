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
#include "RevocapIO/kmbSTLIO.h"
#include "MeshDB/kmbMeshData.h"
#include "Geometry/kmbOctree.h"

#include <fstream>
#include <string>
#include <sstream>

kmb::STLIO::STLIO(void)
: thresh(1.0e-8)
{
}

kmb::STLIO::~STLIO(void)
{
}

int
kmb::STLIO::loadFromFile(const char* filename,kmb::MeshData* mesh)
{
	switch( checkFormat(filename) )
	{
	case kmb::STLIO::BINARY:
		return loadFromBinaryFile(filename,mesh);
	case kmb::STLIO::ASCII:
		return loadFromAsciiFile(filename,mesh);
	default:
		break;
	}
	return 0;
}

int
kmb::STLIO::saveToFile(const char* filename,const kmb::MeshData* mesh)
{
	return 0;
}

kmb::STLIO::fileType
kmb::STLIO::checkFormat(const char* filename)
{
	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	input.seekg(0,std::ios::beg);
	char buf[80];
 	input.read(buf,80);
 	for(int i=0;i<80;++i){
 		if(buf[i] == '\n' || buf[i] == '\r'){
 			input.close();
 			return kmb::STLIO::ASCII;
 		}
 	}
 	input.close();
	return kmb::STLIO::BINARY;
}

int
kmb::STLIO::loadFromBinaryFile(const char* filename,kmb::MeshData* mesh)
{
	kmb::OctreePoint3D nodeOctree;
	kmb::nodeIdType nearestId = kmb::nullNodeId;
	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	input.seekg(0,std::ios::beg);
	char buf[80];
 	input.read(buf,80);
	kmb::nodeIdType nodes[3] = {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};
	int size = 0;
	float v[3] = {0.0,0.0,0.0};
	short attr;
	double dist = 0.0;
	mesh->beginNode();
	mesh->beginElement();
	nodeOctree.setContainer(mesh->getNodes());
	input.read(reinterpret_cast<char*>(&size),sizeof(int));
	for(int i=0;i<size;++i){
		input.read(reinterpret_cast<char*>(v),3*sizeof(float));
		for(int j=0;j<3;++j){
			input.read(reinterpret_cast<char*>(v),3*sizeof(float));
			dist = nodeOctree.getNearest( static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]), nearestId );

			if( nearestId != kmb::nullNodeId && dist < thresh ){
				nodes[j] = nearestId;
			}else{
				nodes[j] = mesh->addNode( static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]) );
				nodeOctree.append( nodes[j] );

			}
		}
		input.read(reinterpret_cast<char*>(&attr),sizeof(short));
		mesh->addElement( kmb::TRIANGLE, nodes );
	}
	mesh->endElement();
	mesh->endNode();
 	input.close();
 	return 0;
}

int
kmb::STLIO::loadFromAsciiFile(const char* filename,kmb::MeshData* mesh)
{
	kmb::OctreePoint3D nodeOctree;
	std::ifstream input( filename, std::ios_base::in );
	input.seekg(0,std::ios::beg);
	std::string str,tag;
	std::getline( input, str );
	int index = 0;
	double x,y,z,dist;
	kmb::nodeIdType nodes[3] = {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};
	kmb::nodeIdType nearestId;
	mesh->beginNode();
	mesh->beginElement();
	nodeOctree.setContainer(mesh->getNodes());
	while( std::getline( input, str ) ){
		if( str.find("endsol") != std::string::npos ){
			break;
		}else if( str.find("vertex") != std::string::npos && index < 3 ){
			std::stringstream is(str);
			is >> tag >> x >> y >> z;

			dist = nodeOctree.getNearest( x,y,z,nearestId );
			if( nearestId != kmb::nullNodeId && dist < thresh ){
				nodes[index] = nearestId;
			}else{
				nodes[index] = mesh->addNode( x, y, z );

				nodeOctree.append( nodes[index] );
			}
			++index;
		}else if( str.find("endfacet") != std::string::npos ){
			mesh->addElement( kmb::TRIANGLE, nodes );
		}else if( str.find("facet") != std::string::npos ){
			index = 0;
		}
	}
	mesh->endElement();
	mesh->endNode();
	return 0;
}

