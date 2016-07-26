/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 2.0                          #
# Class Name : GmshIO                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2015/09/24     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "RevocapIO/kmbGmshIO.h"
#include "MeshDB/kmbMeshData.h"
#include <fstream>
#include <sstream>
#include <iostream>

kmb::GmshIO::GmshIO() : offsetNodeId(1), offsetElementId(1)
{
}

int kmb::GmshIO::loadMeshFromFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ifstream input( filename, std::ios_base::in );
	if( input.fail() ){
		return -1;
	}
	std::string line;
	kmb::GmshIO::gmshFormat ver = getVersion(input);
	switch(ver){
	case kmb::GmshIO::k2_2:
		while( !input.eof() ){
			std::getline( input, line );
			if( line.find("$Nodes") == 0 ){
				readNodes_2_2(input,mesh);
			}
			else if( line.find("$Elements") == 0 ){
				readElements_2_2(input,mesh);
			}
		}
		break;
	case kmb::GmshIO::kUndefined:
		break;
	default:
		break;
	}
	return -1;
}

kmb::GmshIO::gmshFormat kmb::GmshIO::getVersion(std::ifstream &input) const
{
	input.seekg(0, std::ios_base::beg);
	std::string line0, line1, line2;
	if( input.eof() )
		return kmb::GmshIO::kUndefined;
	std::getline( input, line0 );
	std::getline( input, line1 );
	std::getline( input, line2 );
	if( line0.find("$MeshFormat") == 0 &&
		line1.find("2.2") == 0 &&
		line2.find("$EndMeshFormat") == 0 ){
		return kmb::GmshIO::k2_2;
	}
	return kmb::GmshIO::kUndefined;
}

// $Nodes の次の行から
int kmb::GmshIO::readNodes_2_2(std::ifstream &input,kmb::MeshData* mesh)
{
	int nodeCount = 0;
	kmb::nodeIdType nodeId;
	double x,y,z;
	std::string line;
	if( input.eof() )
		return -1;
	std::getline( input, line );
	std::istringstream tokenizer(line);
	tokenizer >> nodeCount;
	mesh->beginNode();
	for(int i=0;i<nodeCount;++i){
		std::getline( input, line );
		tokenizer.str(line);
		tokenizer.clear();
		tokenizer >> nodeId >> x >> y >> z;
		mesh->addNodeWithId(x,y,z,nodeId-offsetNodeId);
	}
	mesh->endNode();
	std::getline( input, line );
	if( line.find("$EndNodes") != 0 ){
		return -1;
	}
	return nodeCount;
}

// $Elements の次の行から
// 次元ごとにまとめる
int kmb::GmshIO::readElements_2_2(std::ifstream &input,kmb::MeshData* mesh)
{
	int elementCount = 0;
	kmb::elementIdType elementId;
	int etype;
	int numberOfTags;
	int tag;
	kmb::nodeIdType nodes[125];
	std::string line;
	if( input.eof() )
		return -1;
	std::getline( input, line );
	std::istringstream tokenizer(line);
	tokenizer >> elementCount;
	kmb::ElementContainer* body0=NULL;
	kmb::ElementContainer* body1=NULL;
	kmb::ElementContainer* body2=NULL;
	kmb::ElementContainer* body3=NULL;
	for(int i=0;i<elementCount;++i){
		std::getline( input, line );
		tokenizer.str(line);
		tokenizer.clear();
		tokenizer >> elementId >> etype >> numberOfTags;
		for(int j=0;j<numberOfTags;j++){
			tokenizer >> tag;
		}
		switch(etype){
		case kmb::GmshIO::kPoint:
			if(body0==NULL){
				kmb::bodyIdType bodyId=mesh->beginElement();
				mesh->endElement();
				body0=mesh->getBodyPtr(bodyId);
			}
			tokenizer >> nodes[0]; nodes[0] -= offsetNodeId;
			body0->addElement(kmb::VERTEX,nodes,elementId-offsetElementId);
			break;
		case kmb::GmshIO::kLine:
		case kmb::GmshIO::kLine3:
			if(body1==NULL){
				kmb::bodyIdType bodyId=mesh->beginElement();
				mesh->endElement();
				body1=mesh->getBodyPtr(bodyId);
			}
			switch(etype){
			case kmb::GmshIO::kLine:
				tokenizer >> nodes[0]; nodes[0] -= offsetNodeId;
				tokenizer >> nodes[1]; nodes[1] -= offsetNodeId;
				body1->addElement(kmb::SEGMENT,nodes,elementId-offsetElementId);
				break;
			case kmb::GmshIO::kLine3:
				tokenizer >> nodes[0]; nodes[0] -= offsetNodeId;
				tokenizer >> nodes[1]; nodes[1] -= offsetNodeId;
				tokenizer >> nodes[2]; nodes[2] -= offsetNodeId;
				body1->addElement(kmb::SEGMENT2,nodes,elementId-offsetElementId);
				break;
			default: break;
			}
			break;
		case kmb::GmshIO::kTriangle:
		case kmb::GmshIO::kTriangle6:
			if(body2==NULL){
				kmb::bodyIdType bodyId=mesh->beginElement();
				mesh->endElement();
				body2=mesh->getBodyPtr(bodyId);
			}
			switch(etype){
			case kmb::GmshIO::kTriangle:
				tokenizer >> nodes[0]; nodes[0] -= offsetNodeId;
				tokenizer >> nodes[1]; nodes[1] -= offsetNodeId;
				tokenizer >> nodes[2]; nodes[2] -= offsetNodeId;
				body2->addElement(kmb::TRIANGLE,nodes,elementId-offsetElementId);
				break;
			case kmb::GmshIO::kTriangle6:
				tokenizer >> nodes[0]; nodes[0] -= offsetNodeId;
				tokenizer >> nodes[1]; nodes[1] -= offsetNodeId;
				tokenizer >> nodes[2]; nodes[2] -= offsetNodeId;
				tokenizer >> nodes[3]; nodes[3] -= offsetNodeId;
				tokenizer >> nodes[4]; nodes[4] -= offsetNodeId;
				tokenizer >> nodes[5]; nodes[5] -= offsetNodeId;
				body2->addElement(kmb::TRIANGLE2,nodes,elementId-offsetElementId);
				break;
			default: break;
			}
			break;
		case kmb::GmshIO::kTetrahedron:
		case kmb::GmshIO::kTetrahedron10:
			if(body3==NULL){
				kmb::bodyIdType bodyId=mesh->beginElement();
				mesh->endElement();
				body3=mesh->getBodyPtr(bodyId);
			}
			switch(etype){
			case kmb::GmshIO::kTetrahedron:
				tokenizer >> nodes[0]; nodes[0] -= offsetNodeId;
				tokenizer >> nodes[1]; nodes[1] -= offsetNodeId;
				tokenizer >> nodes[2]; nodes[2] -= offsetNodeId;
				tokenizer >> nodes[3]; nodes[3] -= offsetNodeId;
				body3->addElement(kmb::TETRAHEDRON,nodes,elementId-offsetElementId);
				break;
			case kmb::GmshIO::kTetrahedron10:
				tokenizer >> nodes[0]; nodes[0] -= offsetNodeId;
				tokenizer >> nodes[1]; nodes[1] -= offsetNodeId;
				tokenizer >> nodes[2]; nodes[2] -= offsetNodeId;
				tokenizer >> nodes[3]; nodes[3] -= offsetNodeId;
				tokenizer >> nodes[4]; nodes[4] -= offsetNodeId;
				tokenizer >> nodes[5]; nodes[5] -= offsetNodeId;
				tokenizer >> nodes[6]; nodes[6] -= offsetNodeId;
				tokenizer >> nodes[7]; nodes[7] -= offsetNodeId;
				tokenizer >> nodes[8]; nodes[8] -= offsetNodeId;
				tokenizer >> nodes[9]; nodes[9] -= offsetNodeId;
				body3->addElement(kmb::TETRAHEDRON2,nodes,elementId-offsetElementId);
				break;
			default: break;
			}
			break;
		default: break;
		}
	}
	std::getline( input, line );
	if( line.find("$EndElements") != 0 ){
		return -1;
	}
	return elementCount;
}
