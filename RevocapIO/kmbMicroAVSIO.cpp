/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MicroAVSIO                                              #
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

#include "RevocapIO/kmbMicroAVSIO.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbPhysicalValue.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

kmb::MicroAVSIO::MicroAVSIO(void)
: dtype(Undefined)
, nodeOffset(1)
, elementOffset(1)
, asVector3(false)
, summary(true)
, version(-1)
, nodeCount(0)
, elementCount(0)
, nodeDimCount(0)
, elementDimCount(0)
{
}

kmb::MicroAVSIO::~MicroAVSIO(void)
{
}

/* version 9 のとき

( コメント行)
( ステップ数)
( データの繰り返しタイプ)
( ステップ番号1) ( コメント)
( 全節点数) ( 全要素数)
( 節点番号1) (X 座標) (Y 座標) (Z 座標)

 */
int
kmb::MicroAVSIO::getVersion(std::ifstream &input) const
{
	std::string line;
	if( !input.eof() ){
		std::getline( input, line );
		if( line.find( "#" ) != std::string::npos ){
			return 9;
		}else if( line.find_first_of("0123456789") != std::string::npos ){
			if( line.find_first_not_of("0123456789") == std::string::npos ){

				return 9;
			}else{
				return 8;
			}
		}
	}
	return -1;
}

void
kmb::MicroAVSIO::setReadAsVector3( bool flag )
{
	this->asVector3 = flag;
}

void
kmb::MicroAVSIO::setSummary( bool flag )
{
	this->summary = flag;
}

int
kmb::MicroAVSIO::readHeader(std::ifstream &input)
{
	if( version == 9 ){
		int step = -1;
		std::string line;
		while( !input.eof() ){
			std::getline( input, line );
			if( line.length() > 0 &&  line.at(0) == '#' ){
				continue;
			}

			if( step == -1 ){
				std::istringstream stream(line);
				stream >> step;
				break;
			}
		}
		std::getline( input, line );
		if( line.find("data") != std::string::npos ){
			dtype = kmb::MicroAVSIO::DATA;
		}else if( line.find("geom") != std::string::npos ){
			dtype = kmb::MicroAVSIO::GEOM;
		}else if( line.find("data_geom") != std::string::npos ){
			dtype = kmb::MicroAVSIO::DATA_GEOM;
		}else{
			return -1;
		}
		return step;
	}else if( version == 8 ){
		int modelDataCount;
		std::string line;
		if( !input.eof() ){
			std::getline( input, line );
			std::istringstream stream(line);
			stream >> nodeCount >> elementCount >> nodeDimCount >> elementDimCount >> modelDataCount;
		}
		dtype = kmb::MicroAVSIO::DATA_GEOM;
		return 1;
	}
	return -1;
}

int
kmb::MicroAVSIO::readGeom(std::ifstream &input,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	if( version == 9 ){
		std::string etype,line;
		kmb::nodeIdType nodeId=kmb::nullNodeId;
		double x=0.0,y=0.0,z=0.0;
		input >> nodeCount >> elementCount;

		mesh->beginNode( nodeCount );
		for(int i=0;i<nodeCount;++i){
			input >> nodeId >> x >> y >> z;
			mesh->addNodeWithId(x,y,z,nodeId-nodeOffset);
		}
		mesh->endNode();
		std::getline( input, line );

		kmb::elementIdType elementId=kmb::Element::nullElementId;
		int material = 0;
		kmb::nodeIdType nodeTable[20] =
		{
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId
		};
		mesh->beginElement( elementCount );
		for(int i=0;i<elementCount;++i){
			input >> elementId;
			input >> material;
			input >> etype;
			if( etype == "pt" ){
			}else if( etype == "line" ){
				input >> nodeTable[0] >> nodeTable[1];
				for(int j=0;j<2;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT, nodeTable, elementId-elementOffset );
			}else if( etype == "line2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT2, nodeTable, elementId-elementOffset );
			}else if( etype == "tri" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE, nodeTable, elementId-elementOffset );
			}else if( etype == "tri2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[5] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE2, nodeTable, elementId-elementOffset );
			}else if( etype == "quad" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD, nodeTable, elementId-elementOffset );
			}else if( etype == "quad2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD2, nodeTable, elementId-elementOffset );
			}else if( etype == "tet" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "tet2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				input >> nodeTable[9] >> nodeTable[6] >> nodeTable[4] >> nodeTable[5] >> nodeTable[7] >> nodeTable[8];
				for(int j=0;j<10;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON2, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<5;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				input >> nodeTable[5] >> nodeTable[6] >> nodeTable[7] >> nodeTable[8];
				input >> nodeTable[9] >> nodeTable[10] >> nodeTable[11] >> nodeTable[12];
				for(int j=0;j<13;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID2, nodeTable, elementId-elementOffset );
			}else if( etype == "prism" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE, nodeTable, elementId-elementOffset );
			}else if( etype == "prism2" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[11] >> nodeTable[9] >> nodeTable[10];
				input >> nodeTable[8] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14];
				for(int j=0;j<15;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE2, nodeTable, elementId-elementOffset );
			}else if( etype == "hex" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "hex2" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14] >> nodeTable[15];
				input >> nodeTable[8] >> nodeTable[9] >> nodeTable[10] >> nodeTable[11];
				input >> nodeTable[16] >> nodeTable[17] >> nodeTable[18] >> nodeTable[19];
				for(int j=0;j<20;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON2, nodeTable, elementId-elementOffset );
			}
			std::getline( input, line );
		}
		mesh->endElement();
	}else if( version == 8 ){
		std::string etype,line;
		kmb::nodeIdType nodeId=kmb::nullNodeId;
		double x=0.0,y=0.0,z=0.0;
		mesh->beginNode( nodeCount );
		for(int i=0;i<nodeCount;++i){
			input >> nodeId >> x >> y >> z;
			mesh->addNodeWithId(x,y,z,nodeId-nodeOffset);
		}
		mesh->endNode();
		std::getline( input, line );

		kmb::elementIdType elementId=kmb::Element::nullElementId;
		int material = 0;
		kmb::nodeIdType nodeTable[20] =
		{
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId
		};
		mesh->beginElement( elementCount );
		for(int i=0;i<elementCount;++i){
			input >> elementId;
			input >> material;
			input >> etype;
			if( etype == "pt" ){
			}else if( etype == "line" ){
				input >> nodeTable[0] >> nodeTable[1];
				for(int j=0;j<2;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT, nodeTable, elementId-elementOffset );
			}else if( etype == "line2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT2, nodeTable, elementId-elementOffset );
			}else if( etype == "tri" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE, nodeTable, elementId-elementOffset );
			}else if( etype == "tri2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[5] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE2, nodeTable, elementId-elementOffset );
			}else if( etype == "quad" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD, nodeTable, elementId-elementOffset );
			}else if( etype == "quad2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD2, nodeTable, elementId-elementOffset );
			}else if( etype == "tet" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "tet2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				input >> nodeTable[6] >> nodeTable[7] >> nodeTable[5] >> nodeTable[8] >> nodeTable[9] >> nodeTable[4];
				for(int j=0;j<10;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON2, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<5;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				input >> nodeTable[5] >> nodeTable[6] >> nodeTable[7] >> nodeTable[8];
				input >> nodeTable[9] >> nodeTable[10] >> nodeTable[11] >> nodeTable[12];
				for(int j=0;j<13;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID2, nodeTable, elementId-elementOffset );
			}else if( etype == "prism" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE, nodeTable, elementId-elementOffset );
			}else if( etype == "prism2" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[11] >> nodeTable[9] >> nodeTable[10];
				input >> nodeTable[8] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14];
				for(int j=0;j<15;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE2, nodeTable, elementId-elementOffset );
			}else if( etype == "hex" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "hex2" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14] >> nodeTable[15];
				input >> nodeTable[8] >> nodeTable[9] >> nodeTable[10] >> nodeTable[11];
				input >> nodeTable[16] >> nodeTable[17] >> nodeTable[18] >> nodeTable[19];
				for(int j=0;j<20;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON2, nodeTable, elementId-elementOffset );
			}
			std::getline( input, line );
		}
		mesh->endElement();
	}
	return 0;
}

int
kmb::MicroAVSIO::skipGeom(std::ifstream &input)
{
	std::string line;
	if( version == 9 ){
		input >> nodeCount >> elementCount;
		std::getline( input, line );
		for(int i=0;i<nodeCount;++i){
			std::getline( input, line );
		}
		for(int i=0;i<elementCount;++i){
			std::getline( input, line );
		}
	}else if( version == 8 ){
		for(int i=0;i<nodeCount;++i){
			std::getline( input, line );
		}
		for(int i=0;i<elementCount;++i){
			std::getline( input, line );
		}
	}
	return 0;
}


void
kmb::MicroAVSIO::getDataName(std::string &line)
{
	size_t comma = line.find_first_of(",");
	if(comma!=std::string::npos){
		line.erase(comma);
	}
	size_t not_space = line.find_first_not_of(" ");
	if(not_space!=std::string::npos){
		line.erase(0,not_space);
	}
}

int
kmb::MicroAVSIO::readData(std::ifstream &input,kmb::MeshData* mesh)
{
	struct dataItem{
		std::string name;
		int dim;
	};
	if( mesh == NULL ){
		return -1;
	}
	kmb::DataBindings* data = NULL;
	if( version == 9 ){
		int nodeDataCount = 0, elementDataCount = 0;

		input >> nodeDimCount >> elementDimCount;
		if( nodeDimCount > 0 ){
			mesh->clearTargetData();

			input >> nodeDataCount;
			if( nodeDataCount>0 ){
				std::string line;
				int d = 0;
				dataItem* items = new dataItem[nodeDataCount];
				for(int i=0;i<nodeDataCount;++i){
					input >> d;
					items[i].dim = d;
				}
				std::getline( input, line );

				for(int i=0;i<nodeDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
				}
				if( summary ){

					for(int i=0;i<nodeDataCount-2;++i){
						size_t semiColon = items[i].name.find(";");
						if( semiColon != std::string::npos ){
							items[i].name.erase(0,semiColon+1);
							items[i].dim += items[i+1].dim;
							items[i].dim += items[i+2].dim;
							items[i+1].dim = 0;
							items[i+2].dim = 0;
						}
					}

					for(int i=0;i<nodeDataCount;++i){
						size_t bar = items[i].name.find("_1");
						if( bar != std::string::npos ){
							items[i].name.erase(bar);
							for(int j=i+1;j<nodeDataCount;++j){
								if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
									items[i].dim += items[j].dim;
									items[j].dim = 0;
								}else{
									i = j-1;
									break;
								}
								if( j==i+5 ){
									i = j;
									break;
								}
							}
						}
					}
				}

				if( asVector3 && nodeDataCount == 3 && items[0].dim == 1 && items[1].dim == 1 && items[1].dim == 1 ){
					items[0].dim = 3;
					items[1].dim = 0;
					items[2].dim = 0;
				}
				for(int i=0;i<nodeDataCount;++i){
					switch( items[i].dim ){
					case 1:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 2:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector2 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector2, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 3:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector3 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 6:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 7:

						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						data = mesh->getDataBindingsPtr( "MISES", "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( "MISES", "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( "MISES", kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
				}
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				double* values = new double[nodeDimCount];
				std::fill_n(values,nodeDimCount,0.0);
				for(int i = 0;i<nodeCount;++i){
					input >> nodeId;
					for(int j=0;j<nodeDimCount;++j){
						input >> values[j];
					}
					mesh->setMultiPhysicalValues( nodeId - nodeOffset, values );
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
		if( elementDimCount > 0 ){
			mesh->clearTargetData();

			input >> elementDataCount;
			if( elementDataCount>0 ){
				std::string line;
				dataItem* items = new dataItem[elementDataCount];
				for(int i=0;i<elementDataCount;++i){
					input >> items[i].dim;
				}
				std::getline( input, line );

				for(int i=0;i<elementDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
				}
				if( summary ){

					for(int i=0;i<elementDataCount-2;++i){
						size_t semiColon = items[i].name.find(";");
						if( semiColon != std::string::npos ){
							items[i].name.erase(0,semiColon+1);
							items[i].dim += items[i+1].dim;
							items[i].dim += items[i+2].dim;
							items[i+1].dim = 0;
							items[i+2].dim = 0;
						}
					}

					for(int i=0;i<elementDataCount;++i){
						size_t bar = items[i].name.find("_1");
						if( bar != std::string::npos ){
							items[i].name.erase(bar);
							for(int j=i+1;j<elementDataCount;++j){
								if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
									items[i].dim += items[j].dim;
									items[j].dim = 0;
								}else{
									i = j-1;
									break;
								}
								if( j==i+5 ){
									i = j;
									break;
								}
							}
						}
					}
				}
				for(int i=0;i<elementDataCount;++i){
					switch( items[i].dim ){
					case 1:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 2:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector2 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector2, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 3:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector3 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector3, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 6:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 7:

						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						data = mesh->getDataBindingsPtr( "EMISES", "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( "EMISES", "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( "EMISES", kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
				}
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				double* values = new double[elementDimCount];
				std::fill_n(values,elementDimCount,0.0);
				for(int i = 0;i<elementCount;++i){
					input >> elementId;
					for(int j=0;j<elementDimCount;++j){
						input >> values[j];
					}
					mesh->setMultiPhysicalValues( elementId - elementOffset, values );
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
	}else if( version == 8 ){
		int nodeDataCount = 0, elementDataCount = 0;
		if( nodeDimCount > 0 ){
			mesh->clearTargetData();

			input >> nodeDataCount;
			if( nodeDataCount>0 ){
				std::string line;
				dataItem* items = new dataItem[nodeDataCount];
				for(int i=0;i<nodeDataCount;++i){
					input >> items[i].dim;
				}
				std::getline( input, line );

				for(int i=0;i<nodeDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
				}
				if( summary ){

					for(int i=0;i<nodeDataCount-2;++i){
						size_t semiColon = items[i].name.find(";");
						if( semiColon != std::string::npos ){
							items[i].name.erase(0,semiColon+1);
							items[i].dim += items[i+1].dim;
							items[i].dim += items[i+2].dim;
							items[i+1].dim = 0;
							items[i+2].dim = 0;
						}
					}

					for(int i=0;i<nodeDataCount;++i){
						size_t bar = items[i].name.find("_1");
						if( bar != std::string::npos ){
							items[i].name.erase(bar);
							for(int j=i+1;j<nodeDataCount;++j){
								if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
									items[i].dim += items[j].dim;
									items[j].dim = 0;
								}else{
									i = j-1;
									break;
								}
								if( j==i+5 ){
									i = j;
									break;
								}
							}
						}
					}
				}

				if( asVector3 && nodeDataCount == 3 && items[0].dim == 1 && items[1].dim == 1 && items[1].dim == 1 ){
					items[0].dim = 3;
					items[1].dim = 0;
					items[2].dim = 0;
				}
				for(int i=0;i<nodeDataCount;++i){
					switch( items[i].dim ){
					case 1:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 2:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector2 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector2, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 3:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector3 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 6:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data =NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 7:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						data = mesh->getDataBindingsPtr( "MISES", "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::NodeVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( "MISES", "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( "MISES", kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
				}
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				double* values = new double[nodeDimCount];
				std::fill_n(values,nodeDimCount,0.0);
				for(int i = 0;i<nodeCount;++i){
					input >> nodeId;
					for(int j=0;j<nodeDimCount;++j){
						input >> values[j];
					}
					mesh->setMultiPhysicalValues( nodeId - nodeOffset, values );
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
		if( elementDimCount > 0 ){
			mesh->clearTargetData();

			input >> elementDataCount;
			if( elementDataCount>0 ){
				std::string line;
				dataItem* items = new dataItem[elementDataCount];
				for(int i=0;i<elementDataCount;++i){
					input >> items[i].dim;
				}
				std::getline( input, line );

				for(int i=0;i<elementDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
				}
				if( summary ){

					for(int i=0;i<elementDataCount-2;++i){
						size_t semiColon = items[i].name.find(";");
						if( semiColon != std::string::npos ){
							items[i].name.erase(0,semiColon+1);
							items[i].dim += items[i+1].dim;
							items[i].dim += items[i+2].dim;
							items[i+1].dim = 0;
							items[i+2].dim = 0;
						}
					}

					for(int i=0;i<elementDataCount;++i){
						size_t bar = items[i].name.find("_1");
						if( bar != std::string::npos ){
							items[i].name.erase(bar);
							for(int j=i+1;j<elementDataCount;++j){
								if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
									items[i].dim += items[j].dim;
									items[j].dim = 0;
								}else{
									i = j-1;
									break;
								}
								if( j==i+5 ){
									i = j;
									break;
								}
							}
						}
					}
				}
				for(int i=0;i<elementDataCount;++i){
					switch( items[i].dim ){
					case 1:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 2:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector2 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector2, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 3:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Vector3 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector3, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 6:
						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					case 7:

						data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Tensor6 ) )
						{
							mesh->deleteData( items[i].name.c_str(), "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
						}
						mesh->appendTargetDataPtr( data );
						data = mesh->getDataBindingsPtr( "EMISES", "post" );
						if( data && (
								data->getBindingMode() != kmb::DataBindings::ElementVariable ||
								data->getValueType() != kmb::PhysicalValue::Scalar ) )
						{
							mesh->deleteData( "EMISES", "post" );
							data = NULL;
						}
						if( data == NULL ){
							data = mesh->createDataBindings( "EMISES", kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
						}
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
				}
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				double* values = new double[elementDimCount];
				std::fill_n(values,elementDimCount,0.0);
				for(int i = 0;i<elementCount;++i){
					input >> elementId;
					for(int j=0;j<elementDimCount;++j){
						input >> values[j];
					}
					mesh->setMultiPhysicalValues( elementId - elementOffset, values );
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
	}

	return 0;
}

int
kmb::MicroAVSIO::loadFromFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		std::string line;
		if( input.fail() ){
			return -1;
		}
		version = getVersion(input);
		if( version == -1 ){
			input.close();
			return -1;
		}
		input.close();
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		int step = readHeader(input);
		if( step == -1 ){
			input.close();
			return -1;
		}
		if( version == 8 ){
			readGeom(input,mesh);
			readData(input,mesh);
		}else if( version == 9 ){
			for(int i=0;i<step;++i){
				std::getline( input, line );
				if( line.find("step") == std::string::npos ){
					break;
				}
				if( i==0 || dtype & kmb::MicroAVSIO::GEOM ){
					readGeom(input,mesh);
				}
				if( i==0 || dtype & kmb::MicroAVSIO::DATA ){
					readData(input,mesh);
				}
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::MicroAVSIO::loadPostFromFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		std::string line;
		if( input.fail() ){
			return -1;
		}
		version = getVersion(input);
		if( version == -1 ){
			input.close();
			return -1;
		}
		input.close();
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		int step = readHeader(input);
		if( step == -1 ){
			input.close();
			return -1;
		}
		if( version == 8 ){
			skipGeom(input);
			readData(input,mesh);
		}else if( version == 9 ){
			for(int i=0;i<step;++i){
				std::getline( input, line );
				if( line.find("step") == std::string::npos ){
					break;
				}
				if( i==0 || dtype & kmb::MicroAVSIO::DATA ){
					skipGeom(input);
				}
				if( i==0 || dtype & kmb::MicroAVSIO::GEOM ){
					readData(input,mesh);
				}
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::MicroAVSIO::saveToFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}



	int modelDim = 3;
	size_t nodeCount = mesh->getNodeCount();
	size_t elemCount = 0;

	kmb::bodyIdType bodyCount = mesh->getBodyCount();

	for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
		const kmb::Body* body = mesh->getBodyPtr(bodyId);
		if( body == NULL || body->getDimension() != modelDim ){
			continue;
		}
		elemCount += body->getCount();
	}
	if( elemCount == 0 ){
		modelDim = 2;
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body == NULL || body->getDimension() != modelDim ){
				continue;
			}
			elemCount += body->getCount();
		}
	}

	output << "# UCD (AVS/Express)" << std::endl;
	output << "1" << std::endl;
	output << "data" << std::endl;
	output << "step1 (fixed time)" << std::endl;
	output << "  " << nodeCount << " " << elemCount << std::endl;

	kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
	output.setf( std::ios::fixed );
	while( !nIter.isFinished() ){
		output << std::setw(8) << nIter.getId()+this->nodeOffset << " "
			<< std::setw(15) << nIter.x() << " "
			<< std::setw(15) << nIter.y() << " "
			<< std::setw(15) << nIter.z() << std::endl;
		++nIter;
	}



	for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
		const kmb::Body* body = mesh->getBodyPtr(bodyId);
		if( body == NULL || body->getDimension() != modelDim ){
			continue;
		}
		kmb::ElementContainer::const_iterator eIter = body->begin();
		while( !eIter.isFinished() ){
			output << " " << eIter.getId() + this->elementOffset << " " << bodyId << " ";
			switch( eIter.getType() ){
				case kmb::TETRAHEDRON:
					output << "tet " <<
						eIter[0] + this->nodeOffset << " " <<
						eIter[1] + this->nodeOffset << " " <<
						eIter[3] + this->nodeOffset << " " <<
						eIter[2] + this->nodeOffset << std::endl;
					break;
				case kmb::TETRAHEDRON2:
					output << "tet2 " <<
						eIter[0] + this->nodeOffset << " " <<
						eIter[1] + this->nodeOffset << " " <<
						eIter[3] + this->nodeOffset << " " <<
						eIter[2] + this->nodeOffset << " " <<
						eIter[6] + this->nodeOffset << " " <<
						eIter[7] + this->nodeOffset << " " <<
						eIter[5] + this->nodeOffset << " " <<
						eIter[8] + this->nodeOffset << " " <<
						eIter[9] + this->nodeOffset << " " <<
						eIter[4] + this->nodeOffset << std::endl;
					break;
				case kmb::TRIANGLE:
					output << "tri " <<
						eIter[0] + this->nodeOffset << " " <<
						eIter[1] + this->nodeOffset << " " <<
						eIter[2] + this->nodeOffset << std::endl;
					break;
				case kmb::QUAD:
					output << "quad " <<
						eIter[0] + this->nodeOffset << " " <<
						eIter[1] + this->nodeOffset << " " <<
						eIter[2] + this->nodeOffset << " " <<
						eIter[3] + this->nodeOffset << std::endl;
					break;
				case kmb::WEDGE:
					output << "prism " <<
						eIter[3] + this->nodeOffset << " " <<
						eIter[4] + this->nodeOffset << " " <<
						eIter[5] + this->nodeOffset << " " <<
						eIter[0] + this->nodeOffset << " " <<
						eIter[1] + this->nodeOffset << " " <<
						eIter[2] + this->nodeOffset << std::endl;
					break;
				case kmb::PYRAMID:
					output << "pyr " <<
						eIter[0] + this->nodeOffset << " " <<
						eIter[1] + this->nodeOffset << " " <<
						eIter[2] + this->nodeOffset << " " <<
						eIter[3] + this->nodeOffset << " " <<
						eIter[4] + this->nodeOffset << std::endl;
					break;
				case kmb::HEXAHEDRON:
					output << "hex " <<
						eIter[4] + this->nodeOffset << " " <<
						eIter[5] + this->nodeOffset << " " <<
						eIter[6] + this->nodeOffset << " " <<
						eIter[7] + this->nodeOffset << " " <<
						eIter[0] + this->nodeOffset << " " <<
						eIter[1] + this->nodeOffset << " " <<
						eIter[2] + this->nodeOffset << " " <<
						eIter[3] + this->nodeOffset << std::endl;
					break;
				default:
					break;
			}
			++eIter;
		}
	}

	int nodeDataDim = 0;
	int elementDataDim = 0;

	std::multimap< std::string, kmb::DataBindings*> dataBindings = mesh->getDataBindingsMap();
	std::multimap< std::string, kmb::DataBindings*>::const_iterator ngIter = dataBindings.begin();
	std::multimap< std::string, kmb::DataBindings*>::const_iterator ngEnd = dataBindings.end();
	while( ngIter != ngEnd ){
		kmb::DataBindings* data = ngIter->second;
		if( data ){
			switch( data->getBindingMode() )
			{
			case kmb::DataBindings::NodeVariable:
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					nodeDataDim += 1;
					break;
				case kmb::PhysicalValue::Vector3:
					nodeDataDim += 3;
					break;
				case kmb::PhysicalValue::Tensor6:
					nodeDataDim += 6;
					break;
				default:
					break;
				}
				break;
			case kmb::DataBindings::ElementVariable:
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					elementDataDim += 1;
					break;
				case kmb::PhysicalValue::Vector3:
					elementDataDim += 3;
					break;
				case kmb::PhysicalValue::Tensor6:
					elementDataDim += 6;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		++ngIter;
	}
	output << " " << nodeDataDim << " " << elementDataDim << std::endl;
	if( nodeDataDim > 0 ){
		output << " " << nodeDataDim;
		for(int i=0;i<nodeDataDim;++i){
			output << " 1";
		}
		output << std::endl;
		mesh->clearTargetData();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator ngIter = dataBindings.begin();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator ngEnd = dataBindings.end();
		while( ngIter != ngEnd ){
			kmb::DataBindings* data = ngIter->second;
			if( data ){
				switch( data->getBindingMode() )
				{
				case kmb::DataBindings::NodeVariable:
					switch( data->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						output << " " << ngIter->first << ", unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Vector3:
						output << " " << ngIter->first << "_u;" << ngIter->first << ", unknown" << std::endl;
						output << " " << ngIter->first << "_v, unknown" << std::endl;
						output << " " << ngIter->first << "_w, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Tensor6:
						output << " " << ngIter->first << "_0, unknown" << std::endl;
						output << " " << ngIter->first << "_1, unknown" << std::endl;
						output << " " << ngIter->first << "_2, unknown" << std::endl;
						output << " " << ngIter->first << "_3, unknown" << std::endl;
						output << " " << ngIter->first << "_4, unknown" << std::endl;
						output << " " << ngIter->first << "_5, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			}
			++ngIter;
		}
		double* values = new double[nodeDataDim];
		std::fill_n(values,nodeDataDim,0.0);
		kmb::nodeIdType nodeId = 0;
		kmb::nodeIdType nodeCount = static_cast<kmb::nodeIdType>(mesh->getNodeCount());
		for(nodeId=0;nodeId<nodeCount;++nodeId){
			mesh->getMultiPhysicalValues(nodeId,values);
			output << " " << nodeId+this->nodeOffset;
			for(int i=0;i<nodeDataDim;++i){
				output << " " << std::setw(15) << values[i];
			}
			output << std::endl;
		}
		delete[] values;
	}
	if( elementDataDim > 0 ){
		output << " " << elementDataDim;
		for(int i=0;i<elementDataDim;++i){
			output << " 1";
		}
		output << std::endl;
		mesh->clearTargetData();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator ngIter = dataBindings.begin();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator ngEnd = dataBindings.end();
		while( ngIter != ngEnd ){
			kmb::DataBindings* data = ngIter->second;
			if( data ){
				switch( data->getBindingMode() )
				{
				case kmb::DataBindings::ElementVariable:
					switch( data->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						output << " " << ngIter->first << ", unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Vector3:
						output << " " << ngIter->first << "_u;" << ngIter->first << ", unknown" << std::endl;
						output << " " << ngIter->first << "_v, unknown" << std::endl;
						output << " " << ngIter->first << "_w, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Tensor6:
						output << " " << ngIter->first << "_0, unknown" << std::endl;
						output << " " << ngIter->first << "_1, unknown" << std::endl;
						output << " " << ngIter->first << "_2, unknown" << std::endl;
						output << " " << ngIter->first << "_3, unknown" << std::endl;
						output << " " << ngIter->first << "_4, unknown" << std::endl;
						output << " " << ngIter->first << "_5, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			}
			++ngIter;
		}
		double* values = new double[elementDataDim];
		std::fill_n(values,elementDataDim,0.0);
		kmb::elementIdType elementId = 0;
		kmb::elementIdType elementCount = static_cast<kmb::elementIdType>( mesh->getElementCount() );
		for(elementId=0;elementId<elementCount;++elementId){
			mesh->getMultiPhysicalValues(elementId,values);
			output << " " << elementId+this->elementOffset;
			for(int i=0;i<elementDataDim;++i){
				output << " " << std::setw(15) << values[i];
			}
			output << std::endl;
		}
		delete[] values;
	}

	output.close();
	return 0;
}

int kmb::MicroAVSIO::saveToFile_V8(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}

	size_t nodeCount = mesh->getNodeCount();
	size_t elemCount = 0;
	size_t nDataCount = 0;
	size_t nDataDim = 0;
	size_t eDataCount = 0;
	size_t eDataDim = 0;
	size_t mDataCount = 0;
	size_t mDataDim = 0;



	int modelDim = 3;

	kmb::bodyIdType bodyCount = mesh->getBodyCount();

	for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
		const kmb::Body* body = mesh->getBodyPtr(bodyId);
		if( body == NULL || body->getDimension() != modelDim ){
			continue;
		}
		elemCount += body->getCount();
	}
	if( elemCount == 0 ){
		modelDim = 2;
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body == NULL || body->getDimension() != modelDim ){
				continue;
			}
			elemCount += body->getCount();
		}
	}

	kmb::datamap::const_iterator dBegin = mesh->beginDataIterator();
	kmb::datamap::const_iterator dEnd = mesh->endDataIterator();

	for(kmb::datamap::const_iterator dIter = dBegin; dIter != dEnd; ++dIter )
	{
		kmb::DataBindings* data = dIter->second;
		if( data ){
			switch( data->getBindingMode() )
			{
			case kmb::DataBindings::NodeVariable:
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					nDataDim += 1;
					nDataCount += 1;
					break;
				case kmb::PhysicalValue::Vector3:
					nDataDim += 3;
					nDataCount += 1;
					break;
				case kmb::PhysicalValue::Tensor6:
					nDataDim += 6;
					nDataCount += 1;
					break;
				default:
					break;
				}
				break;
			case kmb::DataBindings::ElementVariable:
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					eDataDim += 1;
					eDataCount += 1;
					break;
				case kmb::PhysicalValue::Vector3:
					eDataDim += 3;
					eDataCount += 1;
					break;
				case kmb::PhysicalValue::Tensor6:
					eDataDim += 6;
					eDataCount += 1;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
	}

	output << " " << std::setw(10) << nodeCount <<
		" " << std::setw(10) << elemCount <<
		" " << std::setw(10) << nDataDim <<
		" " << std::setw(10) << eDataDim <<
		" " << std::setw(10) << mDataDim << std::endl;

	kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
	output.setf( std::ios::fixed );
	while( !nIter.isFinished() ){
		output << " " << std::setw(10) << nIter.getId()+this->nodeOffset << " " <<
			std::setfill(' ') << std::setprecision(8) << std::scientific <<
			std::setw(16) << nIter.x() << " " <<
			std::setw(16) << nIter.y() << " " <<
			std::setw(16) << nIter.z() << std::endl;
		++nIter;
	}



	for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
		const kmb::Body* body = mesh->getBodyPtr(bodyId);
		if( body == NULL || body->getDimension() != modelDim ){
			continue;
		}
		kmb::ElementContainer::const_iterator eIter = body->begin();
		while( !eIter.isFinished() ){
			output << " " << std::setw(10) << eIter.getId() + this->elementOffset << " " << bodyId << " ";
			switch( eIter.getType() ){
				case kmb::TETRAHEDRON:
					output << "tet " <<
						std::setw(10) << eIter[0] + this->nodeOffset << " " <<
						std::setw(10) << eIter[1] + this->nodeOffset << " " <<
						std::setw(10) << eIter[3] + this->nodeOffset << " " <<
						std::setw(10) << eIter[2] + this->nodeOffset << std::endl;
					break;
				case kmb::TETRAHEDRON2:
					output << "tet2 " <<
						std::setw(10) << eIter[0] + this->nodeOffset << " " <<
						std::setw(10) << eIter[1] + this->nodeOffset << " " <<
						std::setw(10) << eIter[3] + this->nodeOffset << " " <<
						std::setw(10) << eIter[2] + this->nodeOffset << " " <<
						std::setw(10) << eIter[6] + this->nodeOffset << " " <<
						std::setw(10) << eIter[7] + this->nodeOffset << " " <<
						std::setw(10) << eIter[5] + this->nodeOffset << " " <<
						std::setw(10) << eIter[8] + this->nodeOffset << " " <<
						std::setw(10) << eIter[9] + this->nodeOffset << " " <<
						std::setw(10) << eIter[4] + this->nodeOffset << std::endl;
					break;
				case kmb::TRIANGLE:
					output << "tri " <<
						std::setw(10) << eIter[0] + this->nodeOffset << " " <<
						std::setw(10) << eIter[1] + this->nodeOffset << " " <<
						std::setw(10) << eIter[2] + this->nodeOffset << std::endl;
					break;
				case kmb::QUAD:
					output << "quad " <<
						std::setw(10) << eIter[0] + this->nodeOffset << " " <<
						std::setw(10) << eIter[1] + this->nodeOffset << " " <<
						std::setw(10) << eIter[2] + this->nodeOffset << " " <<
						std::setw(10) << eIter[3] + this->nodeOffset << std::endl;
					break;
				case kmb::WEDGE:
					output << "prism " <<
						std::setw(10) << eIter[3] + this->nodeOffset << " " <<
						std::setw(10) << eIter[4] + this->nodeOffset << " " <<
						std::setw(10) << eIter[5] + this->nodeOffset << " " <<
						std::setw(10) << eIter[0] + this->nodeOffset << " " <<
						std::setw(10) << eIter[1] + this->nodeOffset << " " <<
						std::setw(10) << eIter[2] + this->nodeOffset << std::endl;
					break;
				case kmb::PYRAMID:
					output << "pyr " <<
						std::setw(10) << eIter[0] + this->nodeOffset << " " <<
						std::setw(10) << eIter[1] + this->nodeOffset << " " <<
						std::setw(10) << eIter[2] + this->nodeOffset << " " <<
						std::setw(10) << eIter[3] + this->nodeOffset << " " <<
						std::setw(10) << eIter[4] + this->nodeOffset << std::endl;
					break;
				case kmb::HEXAHEDRON:
					output << "hex " <<
						std::setw(10) << eIter[4] + this->nodeOffset << " " <<
						std::setw(10) << eIter[5] + this->nodeOffset << " " <<
						std::setw(10) << eIter[6] + this->nodeOffset << " " <<
						std::setw(10) << eIter[7] + this->nodeOffset << " " <<
						std::setw(10) << eIter[0] + this->nodeOffset << " " <<
						std::setw(10) << eIter[1] + this->nodeOffset << " " <<
						std::setw(10) << eIter[2] + this->nodeOffset << " " <<
						std::setw(10) << eIter[3] + this->nodeOffset << std::endl;
					break;
				default:
					break;
			}
			++eIter;
		}
	}

	if( nDataDim > 0 ){
		output << " " << nDataDim;
		for(int i=0;i<nDataDim;++i){
			output << " 1";
		}
		output << std::endl;
		mesh->clearTargetData();
		kmb::datamap::const_iterator dIter = dBegin;
		while( dIter != dEnd ){
			kmb::DataBindings* data = dIter->second;
			if( data ){
				switch( data->getBindingMode() )
				{
				case kmb::DataBindings::NodeVariable:
					switch( data->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						output << " " << dIter->first << ", unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Vector3:
						output << " " << dIter->first << "_u, unknown" << std::endl;
						output << " " << dIter->first << "_v, unknown" << std::endl;
						output << " " << dIter->first << "_w, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Tensor6:
						output << " " << dIter->first << "_0, unknown" << std::endl;
						output << " " << dIter->first << "_1, unknown" << std::endl;
						output << " " << dIter->first << "_2, unknown" << std::endl;
						output << " " << dIter->first << "_3, unknown" << std::endl;
						output << " " << dIter->first << "_4, unknown" << std::endl;
						output << " " << dIter->first << "_5, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			}
			++dIter;
		}
		double* values = new double[nDataDim];
		std::fill_n(values,nDataDim,0.0);
		kmb::nodeIdType nodeId = 0;
		kmb::nodeIdType nodeCount = static_cast<kmb::nodeIdType>(mesh->getNodeCount());
		for(nodeId=0;nodeId<nodeCount;++nodeId){
			mesh->getMultiPhysicalValues(nodeId,values);
			output << " " << std::setw(12) << nodeId+this->nodeOffset;
			for(int i=0;i<nDataDim;++i){
				output << std::setfill(' ') << std::setprecision(8) << std::scientific << " " <<
				std::setw(16) << values[i];
			}
			output << std::endl;
		}
		delete[] values;
	}
	if( eDataDim > 0 ){
		output << " " << eDataDim;
		for(int i=0;i<eDataDim;++i){
			output << " 1";
		}
		output << std::endl;
		mesh->clearTargetData();
		kmb::datamap::const_iterator dIter = dBegin;
		while( dIter != dEnd ){
			kmb::DataBindings* data = dIter->second;
			if( data ){
				switch( data->getBindingMode() )
				{
				case kmb::DataBindings::ElementVariable:
					switch( data->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						output << " " << dIter->first << ", unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Vector3:
						output << " " << dIter->first << "_u, unknown" << std::endl;
						output << " " << dIter->first << "_v, unknown" << std::endl;
						output << " " << dIter->first << "_w, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					case kmb::PhysicalValue::Tensor6:
						output << " " << dIter->first << "_0, unknown" << std::endl;
						output << " " << dIter->first << "_1, unknown" << std::endl;
						output << " " << dIter->first << "_2, unknown" << std::endl;
						output << " " << dIter->first << "_3, unknown" << std::endl;
						output << " " << dIter->first << "_4, unknown" << std::endl;
						output << " " << dIter->first << "_5, unknown" << std::endl;
						mesh->appendTargetDataPtr( data );
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			}
			++dIter;
		}
		double* values = new double[eDataDim];
		std::fill_n(values,eDataDim,0.0);
		kmb::elementIdType elementId = 0;
		kmb::elementIdType elementCount = static_cast<kmb::elementIdType>( mesh->getElementCount() );
		for(elementId=0;elementId<elementCount;++elementId){
			mesh->getMultiPhysicalValues(elementId,values);
			output << " " << std::setw(12) << elementId+this->elementOffset;
			for(int i=0;i<nDataDim;++i){
				output << std::setfill(' ') << std::setprecision(8) << std::scientific << " " <<
				std::setw(16) << values[i];
			}
			output << std::endl;
		}
		delete[] values;
	}

	output.close();
	return 0;
}
