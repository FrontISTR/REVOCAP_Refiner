/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapCouplerIO                                        #
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
#include "RevocapIO/kmbRevocapCouplerIO.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbNodeMapperBindings.h"
#include "MeshDB/kmbPhysicalValue.h"
#include "MeshDB/kmbMeshData.h"
#include <sstream>
#include <iostream>

kmb::RevocapCouplerIO::RevocapCouplerIO( kmb::nodeIdType offsetNodeId, kmb::elementIdType offsetElementId )
: offsetNodeId(0)
, offsetElementId(0)
{
	this->offsetNodeId = offsetNodeId;
	this->offsetElementId = offsetElementId;
}

kmb::RevocapCouplerIO::~RevocapCouplerIO(void)
{
}

int
kmb::RevocapCouplerIO::loadFromPartitionFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ifstream input( filename, std::ios_base::in );
	if( input.fail() ){
		return -1;
	}
	std::string line;
	std::string dummy;
	kmb::elementType eType = kmb::UNKNOWNTYPE;
	while( !input.eof() ){
		std::getline( input, line );
		if( line.find("Solid_PartID") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			int partId = 0;
			tokenizer >> partId;
			kmb::DataBindings* data = mesh->createDataBindings("Solid_Part",kmb::DataBindings::Global,kmb::PhysicalValue::Integer);
			if(data){
				data->setPhysicalValue( new kmb::IntegerValue(partId) );
			}
		}
		else if( line.find("Fluid_PartID") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			int partId = 0;
			tokenizer >> partId;
			kmb::DataBindings* data = mesh->createDataBindings("Fluid_Part",kmb::DataBindings::Global,kmb::PhysicalValue::Integer);
			if(data){
				data->setPhysicalValue( new kmb::IntegerValue(partId) );
			}
		}
		else if( line.find("Element_Type") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			std::string eTypeStr;
			tokenizer >> eTypeStr;
			if( eTypeStr == "Tet4" ){
				eType = kmb::TETRAHEDRON;
			}else if( eTypeStr == "Tet10" ){
				eType = kmb::TETRAHEDRON2;
			}else if( eTypeStr == "Hex8" ){
				eType = kmb::HEXAHEDRON;
			}else if( eTypeStr == "Hex20" ){
				eType = kmb::HEXAHEDRON2;
			}
		}
		else if( line.find("Number_of_Elemen_Ids") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			size_t elementCount = 0;
			tokenizer >> elementCount;
			loadElement( input, eType, elementCount, mesh );
		}
		else if( line.find("Number_of_Node_Ids") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			size_t nodeCount = 0;
			tokenizer >> nodeCount;
			loadNode( input, nodeCount, mesh );
		}
	}
	input.close();
	return 0;
}




int
kmb::RevocapCouplerIO::loadLocalNodesFromPartitionFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ifstream input( filename, std::ios_base::in );
	if( input.fail() ){
		return -1;
	}
	std::string line;
	std::string dummy;
	while( !input.eof() ){
		std::getline( input, line );
		if( line.find("Solid_PartID") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			int partId = 0;
			tokenizer >> partId;
			std::cout << "Solid_PartID" << partId << std::endl;
			kmb::DataBindings* data = mesh->createDataBindings("Solid_Part",kmb::DataBindings::Global,kmb::PhysicalValue::Integer);
			if(data){
				std::cout << "Solid_Pard Setting" << std::endl;
				data->setPhysicalValue( new kmb::IntegerValue(partId) );
			}
		}
		else if( line.find("Fluid_PartID") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			int partId = 0;
			tokenizer >> partId;
			std::cout << "Fluid_PartID" << partId << std::endl;
			kmb::DataBindings* data = mesh->createDataBindings("Fluid_Part",kmb::DataBindings::Global,kmb::PhysicalValue::Integer);
			if(data){
				data->setPhysicalValue( new kmb::IntegerValue(partId) );
			}
		}
		else if( line.find("Element_Type") == 0 ){















		}
		else if( line.find("Number_of_Elemen_Ids") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			size_t elementCount = 0;
			tokenizer >> elementCount;
			if( elementCount > 0 ){
				for(size_t i=0;i<elementCount;++i){
					std::getline( input, line );
				}
			}
		}
		else if( line.find("Number_of_Node_Ids") == 0 ){
			std::istringstream tokenizer(line);
			tokenizer >> dummy;
			size_t nodeCount = 0;
			tokenizer >> nodeCount;

			loadLocalNode( input, nodeCount, mesh );
		}
	}
	input.close();
	return 0;
}




kmb::bodyIdType
kmb::RevocapCouplerIO::loadElement( std::ifstream &input, kmb::elementType etype, size_t elementCount, kmb::MeshData* mesh )
{
	kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
	if( mesh == NULL || input.fail() || input.eof() || etype == kmb::UNKNOWNTYPE ){
		return bodyId;
	}
	std::string line;
	size_t nodeCount = kmb::Element::getNodeCount( etype );
	kmb::nodeIdType* nodes = new kmb::nodeIdType[nodeCount];
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	bodyId = mesh->beginElement( elementCount );
	for(size_t i =0;i<elementCount;++i){
		input >> elementId;
		for(size_t j=0;j<nodeCount;++j){
			input >> nodes[j];
		}
		std::getline( input, line );
		mesh->addElementWithId( etype, nodes, elementId );
	}
	mesh->endElement();
	delete[] nodes;
	return bodyId;
}

size_t
kmb::RevocapCouplerIO::loadNode( std::ifstream &input, size_t nodeCount, kmb::MeshData* mesh )
{
	if( mesh == NULL || input.fail() || input.eof() ){
		return 0;
	}
	double x=0.0, y=0.0, z=0.0;
	std::string line;
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	mesh->beginNode( nodeCount );
	for(size_t i =0;i<nodeCount;++i){
		input >> nodeId;
		input >> x >> y >> z;
		std::getline( input, line );
		mesh->addNodeWithId( x, y, z, nodeId );
	}
	mesh->endNode();
	return mesh->getNodeCount();
}



size_t
kmb::RevocapCouplerIO::loadLocalNode( std::ifstream &input, size_t nodeCount, kmb::MeshData* mesh )
{
	std::string line;
	if( input.fail() || input.eof() ){
		return 0;
	}
	if( mesh == NULL ){
		for(size_t i =0;i<nodeCount;++i){
			std::getline( input, line );
		}
		return 0;
	}
	kmb::DataBindings* data = new kmb::NodeMapperBindings<kmb::nodeIdType>();
	if( !mesh->setDataBindingsPtr("GlobalId",data) ){
		if( data != NULL ){
			delete data;
		}
		data = NULL;
		for(size_t i =0;i<nodeCount;++i){
			std::getline( input, line );
		}
		return 0;
	}
	double x=0.0, y=0.0, z=0.0;
	long globalId = kmb::nullNodeId;
	kmb::nodeIdType localId = kmb::nullNodeId;
	mesh->beginNode( nodeCount );
	for(size_t i =0;i<nodeCount;++i){
		input >> globalId;
		input >> x >> y >> z;
		std::getline( input, line );
		localId = mesh->addNode( x, y, z );
		data->setPhysicalValue( localId, &globalId );
	}
	mesh->endNode();
	return mesh->getNodeCount();
}
