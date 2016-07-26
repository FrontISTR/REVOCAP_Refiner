/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : HecmwIO (Ver.3)                                         #
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
//
// HecMW Ver.3 のためのファイル I/O
//
// material 属性は CNT ファイルを読まないとわからない
//

#include "RevocapIO/kmbHecmwIO.h"
#include "RevocapIO/kmbRevocapIOUtils.h"
#include "MeshDB/kmbMeshData.h"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>

#ifdef WIN32
 #if _MSC_VER >= 1400
  #define sprintf sprintf_s
 #endif
#endif

// section 情報が解析制御ファイルに書かれている
int
kmb::HecmwIO::loadFromMW3File(const char* filename,MeshData* mesh) const
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		std::stringstream ss;
		std::string line;
		char comma;  // for comma
		std::getline( input, line );
		while( !input.eof() ){
			if( line.find("!HEADER") == 0 ){
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}
				}
			}else if( line.find("!NODE") == 0 ){
				size_t nodeCount = static_cast<size_t>( atoi( kmb::RevocapIOUtils::getValue( line, "NUM" ).c_str() ) );
				std::string name = kmb::RevocapIOUtils::getValue( line, "NGRP" );
				kmb::DataBindings* data = NULL;
				if( dataFlag ){
					data = mesh->getDataBindingsPtr(name.c_str());
				}
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				double x=0.0,y=0.0,z=0.0;
				if( mesh->getNodes() == NULL ){
					mesh->beginNode(nodeCount);
					mesh->endNode();
				}
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}else{
						std::istringstream tokenizer(line);
						tokenizer >> nodeId >> comma >> x >> comma >> y >> comma >> z;
						mesh->insertNodeWithId( x,y,z, nodeId-offsetNodeId );
						if( data ){
							data->addId( nodeId-offsetNodeId );
						}
					}
				}
			}else if( line.find("!ELEMENT") == 0 ){
				std::string name = kmb::RevocapIOUtils::getValue( line, "EGRP" );
				// 名前が既にある領域名と一致するかどうか
				kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
				if( name.length() > 0 ){
					bodyId = mesh->getBodyIdByName( name.c_str() );
				}
				// ElementContainer を取得
				kmb::ElementContainer* body = NULL;
				if( bodyId == kmb::Body::nullBodyId ){
					bodyId = mesh->beginElement();
					mesh->endElement();
					body = mesh->getBodyPtr(bodyId);
					body->setBodyName( name.c_str() );
				}else{
					body = mesh->getBodyPtr(bodyId);
				}
				ss << kmb::RevocapIOUtils::getValue( line, "TYPE" );
				int etypeHec;
				ss >> etypeHec;
				kmb::elementType etype = kmb::HecmwIO::getRevocapType( etypeHec );
				if( etype != kmb::UNKNOWNTYPE ){
					int len = kmb::Element::getNodeCount( etype );
					kmb::elementIdType elementId = kmb::Element::nullElementId;
					kmb::nodeIdType* nodeTable = new kmb::nodeIdType[len];
					char comma;  // for comma
					while( !input.eof() ){
						std::getline( input, line );
						if( line.find("!") == 0 ){
							break;
						}else{
							std::istringstream tokenizer(line);
							tokenizer >> elementId >> comma;
							for(int i=0;i<len;++i){
								tokenizer >> nodeTable[i] >> comma;
								nodeTable[i] -= offsetNodeId;
							}
							body->addElement( etype, nodeTable, elementId - this->offsetElementId );
						}
					}
				}else{
					std::getline( input, line );
				}
			}else if( line.find("!EGROUP") == 0 ){
				std::string name = kmb::RevocapIOUtils::getValue( line, "EGRP" );
				kmb::DataBindings* data = NULL;
				if( dataFlag ){
					data = mesh->getDataBindingsPtr( name.c_str() );
				}
				if( data ){
					kmb::elementIdType elementId = kmb::Element::nullElementId;
					std::istringstream tokenLine;
					std::istringstream tokenDecimal;
					std::string decimal;
					while( std::getline( input, line ) ){
						if( line.find("!") == 0 ){
							break;
						}
						tokenLine.str(line);
						tokenLine.clear();
						while( std::getline( tokenLine, decimal, ',') ){
							tokenDecimal.str(decimal);
							tokenDecimal.clear();
							tokenDecimal >> elementId;
							data->addId( elementId - this->offsetElementId );
						}
					}
				}else{
					std::getline( input, line );
				}
			}else if( line.find("!NGROUP") == 0 ){
				std::string name = kmb::RevocapIOUtils::getValue( line, "NGRP" );
				kmb::DataBindings* data = NULL;
				if( dataFlag ){
					data = mesh->getDataBindingsPtr( name.c_str() );
				}
				if( data ){
					kmb::nodeIdType nodeId = kmb::nullNodeId;
					std::istringstream tokenLine;
					std::istringstream tokenDecimal;
					std::string decimal;
					while( std::getline( input, line ) ){
						if( line.find("!") == 0 ){
							break;
						}
						tokenLine.str(line);
						tokenLine.clear();
						while( std::getline( tokenLine, decimal, ',') ){
							tokenDecimal.str(decimal);
							tokenDecimal.clear();
							tokenDecimal >> nodeId;
							data->addId( nodeId - this->offsetNodeId );
						}
					}
				}else{
					std::getline( input, line );
				}
			}else if( line.find("!SGROUP") == 0 ){
				std::string name = kmb::RevocapIOUtils::getValue( line, "SGRP" );
				kmb::DataBindings* data = NULL;
				if( dataFlag ){
					data = mesh->getDataBindingsPtr( name.c_str() );
				}
				if( data ){
					kmb::elementIdType elementId = kmb::Element::nullElementId;
					kmb::idType localId = kmb::nullId;
					kmb::Face f;
					std::istringstream tokenLine;
					std::istringstream tokenDecimal;
					std::string decimal;
					while( std::getline( input, line ) ){
						if( line.find("!") == 0 ){
							break;
						}
						tokenLine.str(line);
						tokenLine.clear();
						while( !tokenLine.eof() ){
							std::getline( tokenLine, decimal, ',');
							tokenDecimal.str(decimal);
							tokenDecimal.clear();
							tokenDecimal >> elementId;
							std::getline( tokenLine, decimal, ',');
							tokenDecimal.str(decimal);
							tokenDecimal.clear();
							tokenDecimal >> localId;
							kmb::ElementContainer::const_iterator eIter = mesh->findElement( elementId - this->offsetElementId );
							switch( eIter.getType() )
							{
							case kmb::TETRAHEDRON:
							case kmb::TETRAHEDRON2:
								f.setId( elementId - this->offsetElementId, tetRmap[ localId-1 ] );
								data->addId( f );
								break;
							case kmb::WEDGE:
							case kmb::WEDGE2:
								f.setId( elementId - this->offsetElementId, wedRmap[ localId-1 ] );
								data->addId( f );
								break;
							case kmb::HEXAHEDRON:
							case kmb::HEXAHEDRON2:
								f.setId( elementId - this->offsetElementId, hexRmap[ localId-1 ] );
								data->addId( f );
								break;
							case kmb::PYRAMID:
							case kmb::PYRAMID2:
								f.setId( elementId - this->offsetElementId, pyrRmap[ localId-1 ] );
								data->addId( f );
								break;
							default:
								// findElement でエラーの時もここを通る
								break;
							}
						}
					}
				}else{
					std::getline( input, line );
				}
			}else{
				std::getline( input, line );
			}
		}
		input.close();
		return 0;
	}
}

// CONT PAIR と ASSEM PAIR は同じ部品に属するとしている仮実装
int
kmb::HecmwIO::saveToFileMW3(const char* filename,const kmb::MeshData* mesh,const char* partName) const
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		writeHeader(output,"4");
		writeNode(output,mesh,partName);

		// SECTION で与えられる要素は、Body ごとに EGRP 名を付けて !ELEMENT で出力する
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			writeElement(output,mesh,bodyId,partName);
		}

		writeEGroup(output,mesh,partName);
		writeNGroup(output,mesh,partName);
		writeSGroup(output,mesh,partName);

		writeContactPair(output,mesh,"!CONTACT_PAIR",partName);
		writeAssemblyPair(output,mesh,partName);

		output.close();
	}
	return 0;
}

int kmb::HecmwIO::saveToMeshFile_3_6(const char* filename, const kmb::MeshData* mesh) const
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		writeHeader(output);
		writeNode(output,mesh);

		// SECTION で与えられる要素は、Body ごとに EGRP 名を付けて !ELEMENT で出力する
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			writeElement(output,mesh,bodyId);
		}
		
		writeMaterial(output,mesh);

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			writeSection(output,mesh,bodyId);
		}

		writeEGroup(output,mesh);
		writeNGroup(output,mesh);
		writeSGroup(output,mesh);
		writeInitialCondition(output,mesh);
		writeZero(output,mesh);
		writeAmplitude(output,mesh);
		writeContactPair(output,mesh,"!CONTACT PAIR");
		// equation

		output.close();
	}
	return 0;
}


