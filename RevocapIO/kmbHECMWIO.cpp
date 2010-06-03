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







#include "RevocapIO/kmbHECMWIO.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainerMap.h"

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>


#ifdef WIN32
 #if _MSC_VER >= 1400
  #define sprintf sprintf_s
 #endif
#endif


kmb::HECMWIO::HECMWIO(kmb::nodeIdType offsetNodeId,kmb::elementIdType offsetElementId)
: offsetNodeId(0)
, offsetElementId(0)
, soltype(kmb::HECMWIO::STATIC)
{
	this->offsetNodeId = offsetNodeId;
	this->offsetElementId = offsetElementId;
}

kmb::HECMWIO::~HECMWIO(void)
{
}

kmb::HECMWIO::solutionType
kmb::HECMWIO::setSolutionType( const char* soltype )
{
	if( strcmp(soltype,"STATIC")==0 ){
		this->soltype = kmb::HECMWIO::STATIC;
	}else if( strcmp(soltype,"HEAT")==0 ){
		this->soltype = kmb::HECMWIO::HEAT;
	}else if( strcmp(soltype,"EIGEN")==0 ){
		this->soltype = kmb::HECMWIO::EIGEN;
	}else if( strcmp(soltype,"DYNAMIC")==0 ){
		this->soltype = kmb::HECMWIO::DYNAMIC;
	}
	return this->soltype;
}

const char*
kmb::HECMWIO::getSolutionType( void )
{
	switch( this->soltype )
	{
	case kmb::HECMWIO::STATIC:
		return "STATIC";
	case kmb::HECMWIO::HEAT:
		return "HEAT";
	case kmb::HECMWIO::EIGEN:
		return "EIGEN";
	case kmb::HECMWIO::DYNAMIC:
		return "DYNAMIC";
	default:
		break;
	}
	return "UNKNOWN";
}

int
kmb::HECMWIO::getHECType( kmb::elementType etype )
{
	switch(etype)
	{
	case kmb::SEGMENT:		return 111;
	case kmb::SEGMENT2:		return 112;
	case kmb::TRIANGLE:		return 231;
	case kmb::TRIANGLE2:	return 232;
	case kmb::QUAD:			return 241;
	case kmb::QUAD2:		return 242;
	case kmb::TETRAHEDRON:	return 341;
	case kmb::TETRAHEDRON2:	return 342;
	case kmb::WEDGE:		return 351;
	case kmb::WEDGE2:		return 352;
	case kmb::HEXAHEDRON:	return 361;
	case kmb::HEXAHEDRON2:	return 362;
	case kmb::PYRAMID:		return 371;
	case kmb::PYRAMID2:		return 372;
	default: break;
	}
	return -1;
}

kmb::elementType
kmb::HECMWIO::getRevocapType( int etype )
{
	switch(etype)
	{
	case 111:	return kmb::SEGMENT;
	case 112:	return kmb::SEGMENT2;
	case 231:	return kmb::TRIANGLE;
	case 232:	return kmb::TRIANGLE2;
	case 241:	return kmb::QUAD;
	case 242:	return kmb::QUAD2;
	case 341:	return kmb::TETRAHEDRON;
	case 342:	return kmb::TETRAHEDRON2;
	case 351:	return kmb::WEDGE;
	case 352:	return kmb::WEDGE2;
	case 361:	return kmb::HEXAHEDRON;
	case 362:	return kmb::HEXAHEDRON2;
	case 371:	return kmb::PYRAMID;
	case 372:	return kmb::PYRAMID2;
	}
	return kmb::UNKNOWNTYPE;
}

std::string
kmb::HECMWIO::getValue( std::string exp, std::string key )
{
	std::string::size_type keyFirst = exp.find( key );
	if( keyFirst == std::string::npos )	return "";
	std::string::size_type eqEnd = exp.find( "=", keyFirst )+1;
	if( eqEnd == std::string::npos )	return "";
	std::string::size_type valFirst = exp.find_first_not_of( " ", eqEnd );
	if( valFirst == std::string::npos )	return "";
	std::string::size_type valEnd = exp.find_first_not_of( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-", valFirst );

	return exp.substr( valFirst, valEnd-valFirst);
}

enum loadStatus{
	HEADER,
	NODE,
	ELEMENT,
	SECTION,
	MATERIAL,
	EGROUP,
	NGROUP,
	SGROUP
};

/*
 * (1) elementCount ‚Æ nodeCount ‚ð’²‚×‚é
 * (2) element ‚ª‚Ç‚Ì body ‚É‘®‚·‚é‚©‚ð’²‚×‚é
 * (3) body –ˆ‚Ì material ‚Æ‚Ì‘Î‰ž‚ð’²‚×‚é
 * ‹ó“Ç‚ÝŽž‚Ì\‘¢‘Ì
 * egrpName
 * materialName
 * elementCount
 * materialName ‚ª—^‚¦‚ç‚ê‚Ä‚¢‚½‚ç Body ‚Æ‚Ý‚È‚·
 * egrpName = ALL ‚Å materialName ‚ª—^‚¦‚ç‚ê‚Ä‚¢‚é‚Æ‚«‚ÍAˆÃ–Ù‚Ì‘S‚Ä‚Ì—v‘f
 */
int
kmb::HECMWIO::loadFromFile(const char* filename,MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}

		kmb::DataBindings* sectionInfo =
			mesh->createDataBindings( "SECTION",
			kmb::DataBindings::BODYVARIABLE,
			kmb::PhysicalValue::HASH,
			"SECTION");
		std::vector< kmb::HECMWIO::egrpData > egrpInfo;

		kmb::ElementContainerMap elements;
		std::multimap< kmb::elementIdType, int > elementIdMapper;

		size_t nodeCount = 0;
		kmb::bodyIdType bodyId = 0;
		std::string line;


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
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}else{
						++nodeCount;
					}
				}
			}else if( line.find("!ELEMENT") == 0 ){
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				int typeNum = atoi( getValue( line, "TYPE" ).c_str() );
				kmb::elementType etype = kmb::HECMWIO::getRevocapType( typeNum );
				int len = kmb::Element::getNodeCount( etype );
				char comma;
				kmb::nodeIdType *nodeTable = new kmb::nodeIdType[len];
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}
					std::istringstream tokenizer(line);
					tokenizer >> elementId >> comma;
					for(int i=0;i<len;++i){
						tokenizer >> nodeTable[i] >> comma;
						nodeTable[i] -= offsetNodeId;
					}
					elements.addElement( etype, nodeTable, elementId - this->offsetElementId );
				}
				delete[] nodeTable;
			}else if( line.find("!SECTION") == 0 ){
				std::string name = getValue( line, "EGRP" );
				std::string mat = getValue( line, "MATERIAL" );
				int index = -1;
				for(unsigned int i=0;i<egrpInfo.size();++i){
					if( egrpInfo[i].egrpName == name ){
						egrpInfo[i].materialName = mat;
						egrpInfo[i].bodyId = bodyId;
						index = i;
						break;
					}
				}
				if( index == -1 ){
					egrpData e = {name,mat,0,bodyId};
					egrpInfo.push_back(e);
				}

				kmb::HashValue* hash = new kmb::HashValue();
				std::string typestr = getValue( line, "TYPE" );
				hash->setValue( "TYPE", new kmb::TextValue( typestr.c_str() ) );

				std::string matname = getValue(line,"MATERIAL");
				kmb::DataBindings* data = mesh->getDataBindingsPtr( matname.c_str(), "Material" );
				if( data == NULL ){
					data = mesh->createDataBindings( matname.c_str(),
					kmb::DataBindings::BODYGROUP,
					kmb::PhysicalValue::HASH,
					"Material");
				}
				data->addId( bodyId );
				sectionInfo->setPhysicalValue( bodyId, hash );

				std::getline( input, line );
				if( line.find("!") == 0 ){


				}else{
					std::istringstream tokenizer(line);
					char comma;
					if( typestr == "SOLID" ){
						double thickness;
						tokenizer >> thickness;
						hash->setValue( "THICKNESS", new kmb::ScalarValue( thickness ) );
					}else if( typestr == "SHELL" ){
						double thickness;
						int integpoints;
						tokenizer >> thickness >> comma >> integpoints;
						hash->setValue( "THICKNESS", new kmb::ScalarValue( thickness ) );
						hash->setValue( "INTEGPOINTS", new kmb::IntegerValue( integpoints ) );
					}else if( typestr == "INTERFACE" ){
						double thickness;
						double gapcon;
						double gaprad1;
						double gaprad2;
						tokenizer >> thickness >> comma >> gapcon >> comma >> gaprad1 >> comma >> gaprad2;
						hash->setValue( "THICKNESS", new kmb::ScalarValue( thickness ) );
						hash->setValue( "GAPCON", new kmb::ScalarValue( gapcon ) );
						hash->setValue( "GAPRAD1", new kmb::ScalarValue( gaprad1 ) );
						hash->setValue( "GAPRAD2", new kmb::ScalarValue( gaprad2 ) );
					}
					std::getline( input, line );
				}
				++bodyId;
			}else if( line.find("!MATERIAL") == 0 ){
				double value;
				char comma;
				std::string matname = getValue(line,"NAME");
				kmb::DataBindings* data = mesh->getDataBindingsPtr( matname.c_str(), "Material" );
				if( data == NULL ){
					data = mesh->createDataBindings( matname.c_str(),
					kmb::DataBindings::BODYGROUP,
					kmb::PhysicalValue::HASH,
					"Material");
				}
				kmb::HashValue* material = new kmb::HashValue();
				data->setPhysicalValue( material );
				int item = atoi( getValue( line, "ITEM" ).c_str() );
				for(int i=0;i<item;++i){
					std::getline( input, line );
					int subitem = atoi( getValue( line, "SUBITEM" ).c_str() );
					std::getline( input, line );
					std::istringstream tokenizer(line);
					for(int j=0;j<subitem;++j){
						tokenizer >> value;
						tokenizer >> comma;
						switch(this->soltype)
						{
							case kmb::HECMWIO::STATIC:
							case kmb::HECMWIO::EIGEN:
							case kmb::HECMWIO::DYNAMIC:
								if( i==0 ){
									if( j==0 ){
										material->setValue("young", new kmb::ScalarValue( value ) );
									}else if( j==1 ){
										material->setValue("poisson", new kmb::ScalarValue( value ) );
									}
								}else if( i==1 ){
									if( j==0 ){
										material->setValue("density", new kmb::ScalarValue( value ) );
									}
								}else if( i==2 ){
									if( j==0 ){
										material->setValue("linearexp", new kmb::ScalarValue( value ) );
									}
								}
								break;
							case kmb::HECMWIO::HEAT:
								if( i==0 ){
									if( j==0 ){
										material->setValue("density", new kmb::ScalarValue( value ) );
									}
								}else if( i==1 ){
									if( j==0 ){
										material->setValue("specific_heat", new kmb::ScalarValue( value ) );
									}
								}else if( i==2 ){
									if( j==0 ){
										material->setValue("thermal_conductivity", new kmb::ScalarValue( value ) );
									}
								}
								break;
							default:
								break;
						}
					}
				}
				std::getline( input, line );
			}else if( line.find("!EGROUP") == 0 ){
				std::string name = getValue( line, "EGRP" );
				int index = -1;
				for(unsigned int i=0;i<egrpInfo.size();++i){
					if( egrpInfo[i].egrpName == name ){
						index = static_cast<int>(i);
						break;
					}
				}
				if( index == -1 ){
					egrpData e = {name,"",0,-1};
					index = static_cast<int>(egrpInfo.size());
					egrpInfo.push_back(e);
				}
				int count = 0;
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}else{
						++count;
					}
				}
				egrpInfo[index].egrpCount += count;
			}else if( line.find("!NGROUP") == 0 ){
				std::string name = getValue( line, "NGRP" );
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}
				}
			}else if( line.find("!SGROUP") == 0 ){
				std::string name = getValue( line, "SGRP" );
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}
				}
			}else{
				std::getline( input, line );
			}
		}
		input.clear();
		input.seekg( 0, std::ios_base::beg	);

		REVOCAP_Debug( "read hec mesh file at the first time\n" );
		REVOCAP_Debug( "node count = %d, element count = %d\n", nodeCount, elements.getCount() );


		std::string matName;
		std::vector< kmb::HECMWIO::egrpData >::iterator egrpIter = egrpInfo.begin();
		while( egrpIter != egrpInfo.end() ){
			if( egrpIter->egrpName == "ALL" && egrpIter->bodyId != kmb::Body::nullBodyId ){
				kmb::bodyIdType bodyId = mesh->beginElement( elements.getCount() );
				kmb::ElementContainer::iterator eIter = elements.begin();
				kmb::nodeIdType* nodeTable = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
				kmb::elementType etype = kmb::UNKNOWNTYPE;
				while( !eIter.isFinished() ){
					eIter.getElement( etype, nodeTable );
					mesh->addElementWithId( etype, nodeTable, eIter.getId() );
					++eIter;
				}
				mesh->endElement();
				mesh->setBodyName( bodyId, egrpIter->egrpName.c_str() );
				matName = egrpIter->materialName;
				if( matName.size() > 0 && mesh->hasData( matName.c_str() ) ){
					mesh->addId( matName.c_str(), bodyId );
				}
				delete[] nodeTable;
				elements.clear();
				break;
			}
			++egrpIter;
		}


		mesh->beginNode( nodeCount );
		mesh->endNode();

		std::getline( input, line );
		while( !input.eof() ){
			REVOCAP_Debug( "%s\n", line.c_str() );

			if( line.find("!HEADER") == 0 ){
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}
				}
			}else if( line.find("!NODE") == 0 ){
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				double x=0.0,y=0.0,z=0.0;
				char comma;
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}else{
						std::istringstream tokenizer(line);
						tokenizer >> nodeId >> comma >> x >> comma >> y >> comma >> z;
						mesh->addNodeWithId( x,y,z, nodeId-offsetNodeId );
					}
				}
			}else if( line.find("!ELEMENT") == 0 ){
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}
				}
			}else if( line.find("!SECTION") == 0 ){
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 ){
						break;
					}
				}
			}else if( line.find("!MATERIAL") == 0 ){
				while( !input.eof() ){
					std::getline( input, line );
					if( line.find("!") == 0 && line.find("!ITEM") == 0){
						break;
					}
				}
			}else if( line.find("!EGROUP") == 0 ){
				std::string name = getValue( line, "EGRP" );
				kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
				size_t count = 0;
				std::string matName;

				std::vector< kmb::HECMWIO::egrpData >::iterator egrpIter = egrpInfo.begin();
				while( egrpIter != egrpInfo.end() ){
					if( egrpIter->egrpName == name ){
						bodyId = egrpIter->bodyId;
						count = egrpIter->egrpCount;
						matName = egrpIter->materialName;
						break;
					}
					++egrpIter;
				}
				if( bodyId != kmb::Body::nullBodyId ){
					bodyId = mesh->beginElement( count );
					kmb::elementIdType elementId = kmb::Element::nullElementId;
					kmb::nodeIdType* nodeTable = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
					kmb::elementType etype = kmb::UNKNOWNTYPE;
					while( !input.eof() ){
						std::getline( input, line );
						if( line.find("!") == 0 ){
							break;
						}else{
							std::istringstream tokenizer(line);
							while( !tokenizer.eof() ){
								tokenizer >> elementId;

								if( tokenizer.get() != ',' ){
									tokenizer.unget();
								}
								kmb::ElementContainer::iterator eIter = elements.find( elementId - this->offsetElementId );
								if( eIter.getElement( etype, nodeTable ) ){
									mesh->addElementWithId( etype, nodeTable, elementId - this->offsetElementId );
								}
							}
							elements.deleteElement( elementId - this->offsetElementId );
						}
					}
					mesh->endElement();
					mesh->setBodyName( bodyId, egrpIter->egrpName.c_str() );
					if( matName.size() > 0 && mesh->hasData( matName.c_str() ) ){
						mesh->addId( matName.c_str(), bodyId );
					}
					delete[] nodeTable;
				}else{

					kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
					if( data ){
						kmb::elementIdType elementId = kmb::Element::nullElementId;
						while( !input.eof() ){
							std::getline( input, line );
							if( line.find("!") == 0 ){
								break;
							}else{
								std::istringstream tokenizer(line);
								while( !tokenizer.eof() ){
									tokenizer >> elementId;

									if( tokenizer.get() != ',' ){
										tokenizer.unget();
									}
									data->addId( elementId - this->offsetElementId );
								}
							}
						}
					}else{
						while( !input.eof() ){
							std::getline( input, line );
							if( line.find("!") == 0 ){
								break;
							}
						}
					}
				}
			}else if( line.find("!NGROUP") == 0 ){
				std::string name = getValue( line, "NGRP" );
				kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
				if( data ){
					kmb::nodeIdType nodeId = kmb::nullNodeId;
					while( !input.eof() ){
						std::getline( input, line );
						if( line.find("!") == 0 ){
							break;
						}else{
							std::istringstream tokenizer(line);
							while( !tokenizer.eof() ){
								tokenizer >> nodeId;

								if( tokenizer.get() != ',' ){
									tokenizer.unget();
								}
								data->addId( nodeId - this->offsetNodeId );
							}
						}
					}
				}else{
					while( !input.eof() ){
						std::getline( input, line );
						if( line.find("!") == 0 ){
							break;
						}
					}
				}
			}else if( line.find("!SGROUP") == 0 ){
				int tetFmap[] = {3,2,0,1};
				int wedFmap[] = {0,1,2,3,4};
				int hexFmap[] = {0,1,2,3,4,5};
				std::string name = getValue( line, "SGRP" );
				kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
				if( data ){
					kmb::elementIdType elementId = kmb::Element::nullElementId;
					kmb::idType localId = kmb::nullId;
					char comma;
					kmb::Face f;
					while( !input.eof() ){
						std::getline( input, line );
						if( line.find("!") == 0 ){
							break;
						}else{
							std::istringstream tokenizer(line);
							while( !tokenizer.eof() ){
								tokenizer >> elementId >> comma >> localId;

								if( tokenizer.get() != ',' ){
									tokenizer.unget();
								}
								tokenizer >> localId;

								if( tokenizer.get() != ',' ){
									tokenizer.unget();
								}
								kmb::ElementContainer::const_iterator eIter = mesh->findElement( elementId - this->offsetElementId );
								if( eIter.isFinished() ){
									eIter = elements.find( elementId - this->offsetElementId );
								}
								switch( eIter.getType() )
								{
								case kmb::TETRAHEDRON:
								case kmb::TETRAHEDRON2:
									f.setId( elementId - this->offsetElementId, tetFmap[ localId-1 ] );
									data->addId( f );
									break;
								case kmb::WEDGE:
								case kmb::WEDGE2:
									f.setId( elementId - this->offsetElementId, wedFmap[ localId-1 ] );
									data->addId( f );
									break;
								case kmb::HEXAHEDRON:
								case kmb::HEXAHEDRON2:
									f.setId( elementId - this->offsetElementId, hexFmap[ localId-1 ] );
									data->addId( f );
									break;
								default:
									break;
								}
							}
						}
					}
				}else{
					while( !input.eof() ){
						std::getline( input, line );
						if( line.find("!") == 0 ){
							break;
						}
					}
				}
			}else{
				std::getline( input, line );
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::HECMWIO::loadFromFRSFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ifstream input( filename, std::ios_base::in );
	if( input.fail() ){
		return -1;
	}
	int nodeCount = 0;
	int elementCount = 0;
	std::string line;
	while( !input.eof() ){
		std::getline( input, line );

		if( line.find("#") == 0 ){
			continue;
		}else if( line.find("EGRP") == 0 ){
			nodeCount = atoi( getValue( line, "NNODE" ).c_str() );
			elementCount = atoi( getValue( line, "NELEM" ).c_str() );
			continue;
		}else if( line.find("NPARAM") == 0 ){
			int paramNum = atoi( getValue( line, "NPARAM" ).c_str() );
			int* dims = new int[paramNum];
			int dimCount = 0;
			std::getline( input, line );
			std::istringstream tokenizer(line);
			for(int i=0;i<paramNum;++i){
				tokenizer >> dims[i];
				dimCount += dims[i];
			}
			for(int i=0;i<paramNum;++i){
				std::getline( input, line );
				switch( dims[i] )
				{
				case 1:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::GLOBAL, kmb::PhysicalValue::SCALAR );
					mesh->appendTargetData( line.c_str() );
					break;
				case 3:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::GLOBAL, kmb::PhysicalValue::VECTOR3 );
					mesh->appendTargetData( line.c_str() );
					break;
				case 6:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::GLOBAL, kmb::PhysicalValue::TENSOR6 );
					mesh->appendTargetData( line.c_str() );
					break;
				default:
					break;
				}
			}
			double* values = new double[dimCount];
			{
				std::getline( input, line );
				std::istringstream tokenizer(line);
				for(int j=0;j<dimCount;++j){
					if( tokenizer.eof() ){
						std::getline( input, line );
						tokenizer.str( line );
						tokenizer.clear();
					}
					tokenizer >> values[j];
				}
				mesh->setMultiPhysicalValues( values );
			}
			delete[] dims;
			delete[] values;
			mesh->clearTargetData();
			continue;
		}else if( line.find("NNDATA") == 0 ){
			int ndataNum = atoi( getValue( line, "NNDATA" ).c_str() );
			int* dims = new int[ndataNum];
			int dimCount = 0;
			std::getline( input, line );
			std::istringstream tokenizer(line);
			for(int i=0;i<ndataNum;++i){
				tokenizer >> dims[i];
				dimCount += dims[i];
			}
			for(int i=0;i<ndataNum;++i){
				std::getline( input, line );
				switch( dims[i] )
				{
				case 1:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::SCALAR );
					mesh->appendTargetData( line.c_str() );
					break;
				case 3:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::VECTOR3 );
					mesh->appendTargetData( line.c_str() );
					break;
				case 6:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::TENSOR6 );
					mesh->appendTargetData( line.c_str() );
					break;
				default:
					break;
				}
			}
			double* values = new double[dimCount];
			for(int i=0;i<nodeCount;++i){
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				std::getline( input, line );
				std::istringstream tokenizer(line);
				tokenizer >> nodeId;
				for(int j=0;j<dimCount;++j){
					if( tokenizer.eof() ){
						std::getline( input, line );
						tokenizer.str( line );
						tokenizer.clear();
					}
					tokenizer >> values[j];
				}
				mesh->setMultiPhysicalValues( nodeId-1, values );
			}
			delete[] dims;
			delete[] values;
			mesh->clearTargetData();
			continue;
		}else if( line.find("NEDATA") == 0 ){
			int edataNum = atoi( getValue( line, "NEDATA" ).c_str() );
			int* dims = new int[edataNum];
			int dimCount = 0;
			std::getline( input, line );
			std::istringstream tokenizer(line);
			for(int i=0;i<edataNum;++i){
				tokenizer >> dims[i];
				dimCount += dims[i];
			}
			for(int i=0;i<edataNum;++i){
				std::getline( input, line );
				switch( dims[i] )
				{
				case 1:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::SCALAR );
					mesh->appendTargetData( line.c_str() );
					break;
				case 3:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::VECTOR3 );
					mesh->appendTargetData( line.c_str() );
					break;
				case 6:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::TENSOR6 );
					mesh->appendTargetData( line.c_str() );
					break;
				default:
					break;
				}
			}
			double* values = new double[dimCount];
			for(int i=0;i<elementCount;++i){
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				std::getline( input, line );
				std::istringstream tokenizer(line);
				tokenizer >> elementId;
				for(int j=0;j<dimCount;++j){
					if( tokenizer.eof() ){
						std::getline( input, line );
						tokenizer.str( line );
						tokenizer.clear();
					}
					tokenizer >> values[j];
				}
				mesh->setMultiPhysicalValues( elementId-1, values );
			}
			delete[] dims;
			delete[] values;
			mesh->clearTargetData();
			continue;
		}
	}
	input.close();
	return 0;
}


int
kmb::HECMWIO::saveToFile(const char* filename, const kmb::MeshData* mesh)
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		output << "!HEADER" << std::endl;
		output << " HECMW_MSH File Generated By REVOCAP" << std::endl;
		output << "!NODE" << std::endl;
		double x,y,z;
		kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
		kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
		while( nIter != nIterEnd ){
			if( nIter.getXYZ(x,y,z) ){
				output << nIter.getId()+offsetNodeId << "," << x << "," << y << "," << z << std::endl;
			}
			++nIter;
		}
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		size_t *etypeCount = new size_t[kmb::ELEMENT_TYPE_NUM];
		for(int i=0;i<kmb::ELEMENT_TYPE_NUM;++i){
			etypeCount[i]=0;
		}
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body ){
				for(int i=0;i<kmb::ELEMENT_TYPE_NUM;++i){
					etypeCount[i] += body->getCountByType( static_cast<kmb::elementType>(i) );
				}
			}
		}


		if( etypeCount[ kmb::TETRAHEDRON ] > 0 ){
			output << "!ELEMENT, TYPE=" << kmb::HECMWIO::getHECType( kmb::TETRAHEDRON ) << std::endl;
			for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
				const kmb::Body* body = mesh->getBodyPtr(bodyId);
				if( body )
				{
					kmb::ElementContainer::const_iterator eIter = body->begin();
					while( eIter != body->end() ){
						if( eIter.getType() == kmb::TETRAHEDRON ){
							output << eIter.getId()+offsetElementId << "," <<
								eIter.getCellId(0)+offsetNodeId << "," <<
								eIter.getCellId(1)+offsetNodeId << "," <<
								eIter.getCellId(2)+offsetNodeId << "," <<
								eIter.getCellId(3)+offsetNodeId << std::endl;
						}
						++eIter;
					}
				}
			}
		}

		if( etypeCount[ kmb::TETRAHEDRON2 ] > 0 ){
			output << "!ELEMENT, TYPE=" << kmb::HECMWIO::getHECType( kmb::TETRAHEDRON2 ) << std::endl;
			for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
				const kmb::Body* body = mesh->getBodyPtr(bodyId);
				if( body )
				{
					kmb::ElementContainer::const_iterator eIter = body->begin();
					while( eIter != body->end() ){
						if( eIter.getType() == kmb::TETRAHEDRON2 ){
							output << eIter.getId()+offsetElementId << "," <<
								eIter.getCellId(0)+offsetNodeId << "," <<
								eIter.getCellId(1)+offsetNodeId << "," <<
								eIter.getCellId(2)+offsetNodeId << "," <<
								eIter.getCellId(3)+offsetNodeId << "," <<
								eIter.getCellId(4)+offsetNodeId << "," <<
								eIter.getCellId(5)+offsetNodeId << "," <<
								eIter.getCellId(6)+offsetNodeId << "," <<
								eIter.getCellId(7)+offsetNodeId << "," <<
								eIter.getCellId(8)+offsetNodeId << "," <<
								eIter.getCellId(9)+offsetNodeId << std::endl;
						}
						++eIter;
					}
				}
			}
		}

		if( etypeCount[ kmb::HEXAHEDRON ] > 0 ){
			output << "!ELEMENT, TYPE=" << kmb::HECMWIO::getHECType( kmb::HEXAHEDRON ) << std::endl;
			for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
				const kmb::Body* body = mesh->getBodyPtr(bodyId);
				if( body )
				{
					kmb::ElementContainer::const_iterator eIter = body->begin();
					while( eIter != body->end() ){
						if( eIter.getType() == kmb::HEXAHEDRON ){
							output << eIter.getId()+offsetElementId << "," <<
								eIter.getCellId(0)+offsetNodeId << "," <<
								eIter.getCellId(1)+offsetNodeId << "," <<
								eIter.getCellId(2)+offsetNodeId << "," <<
								eIter.getCellId(3)+offsetNodeId << "," <<
								eIter.getCellId(4)+offsetNodeId << "," <<
								eIter.getCellId(5)+offsetNodeId << "," <<
								eIter.getCellId(6)+offsetNodeId << "," <<
								eIter.getCellId(7)+offsetNodeId << std::endl;
						}
						++eIter;
					}
				}
			}
		}

		if( etypeCount[ kmb::HEXAHEDRON2 ] > 0 ){
			output << "!ELEMENT, TYPE=" << kmb::HECMWIO::getHECType( kmb::HEXAHEDRON2 ) << std::endl;
			for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
				const kmb::Body* body = mesh->getBodyPtr(bodyId);
				if( body )
				{
					kmb::ElementContainer::const_iterator eIter = body->begin();
					while( eIter != body->end() ){
						if( eIter.getType() == kmb::HEXAHEDRON2 ){
							output << eIter.getId()+offsetElementId << "," <<
								eIter.getCellId(0)+offsetNodeId << "," <<
								eIter.getCellId(1)+offsetNodeId << "," <<
								eIter.getCellId(2)+offsetNodeId << "," <<
								eIter.getCellId(3)+offsetNodeId << "," <<
								eIter.getCellId(4)+offsetNodeId << "," <<
								eIter.getCellId(5)+offsetNodeId << "," <<
								eIter.getCellId(6)+offsetNodeId << "," <<
								eIter.getCellId(7)+offsetNodeId << "," <<
								eIter.getCellId(8)+offsetNodeId << "," <<
								eIter.getCellId(9)+offsetNodeId << "," <<
								eIter.getCellId(10)+offsetNodeId << "," <<
								eIter.getCellId(11)+offsetNodeId << "," <<
								eIter.getCellId(12)+offsetNodeId << "," <<
								eIter.getCellId(13)+offsetNodeId << "," <<
								eIter.getCellId(14)+offsetNodeId << "," <<
								eIter.getCellId(15)+offsetNodeId << "," <<
								eIter.getCellId(16)+offsetNodeId << "," <<
								eIter.getCellId(17)+offsetNodeId << "," <<
								eIter.getCellId(18)+offsetNodeId << "," <<
								eIter.getCellId(19)+offsetNodeId << std::endl;
						}
						++eIter;
					}
				}
			}
		}

		if( etypeCount[ kmb::WEDGE ] > 0 ){
			output << "!ELEMENT, TYPE=" << kmb::HECMWIO::getHECType( kmb::WEDGE ) << std::endl;
			for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
				const kmb::Body* body = mesh->getBodyPtr(bodyId);
				if( body )
				{
					kmb::ElementContainer::const_iterator eIter = body->begin();
					while( eIter != body->end() ){
						if( eIter.getType() == kmb::WEDGE ){
							output << eIter.getId()+offsetElementId << "," <<
								eIter.getCellId(0)+offsetNodeId << "," <<
								eIter.getCellId(1)+offsetNodeId << "," <<
								eIter.getCellId(2)+offsetNodeId << "," <<
								eIter.getCellId(3)+offsetNodeId << "," <<
								eIter.getCellId(4)+offsetNodeId << "," <<
								eIter.getCellId(5)+offsetNodeId << std::endl;
						}
						++eIter;
					}
				}
			}
		}

		if( etypeCount[ kmb::WEDGE2 ] > 0 ){
			output << "!ELEMENT, TYPE=" << kmb::HECMWIO::getHECType( kmb::WEDGE2 ) << std::endl;
			for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
				const kmb::Body* body = mesh->getBodyPtr(bodyId);
				if( body )
				{
					kmb::ElementContainer::const_iterator eIter = body->begin();
					while( eIter != body->end() ){
						if( eIter.getType() == kmb::WEDGE2 ){
							output << eIter.getId()+offsetElementId << "," <<
								eIter.getCellId(0)+offsetNodeId << "," <<
								eIter.getCellId(1)+offsetNodeId << "," <<
								eIter.getCellId(2)+offsetNodeId << "," <<
								eIter.getCellId(3)+offsetNodeId << "," <<
								eIter.getCellId(4)+offsetNodeId << "," <<
								eIter.getCellId(5)+offsetNodeId << "," <<
								eIter.getCellId(6)+offsetNodeId << "," <<
								eIter.getCellId(7)+offsetNodeId << "," <<
								eIter.getCellId(8)+offsetNodeId << "," <<
								eIter.getCellId(9)+offsetNodeId << "," <<
								eIter.getCellId(10)+offsetNodeId << "," <<
								eIter.getCellId(11)+offsetNodeId << "," <<
								eIter.getCellId(12)+offsetNodeId << "," <<
								eIter.getCellId(13)+offsetNodeId << "," <<
								eIter.getCellId(14)+offsetNodeId << std::endl;
						}
						++eIter;
					}
				}
			}
		}
		delete[] etypeCount;

		std::multimap< std::string, kmb::DataBindings*> dataBindings = mesh->getDataBindingsMap();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
		while( dIter != dEnd ){
			if( dIter->second->getSpecType() == "Material" && dIter->second->getValueType() == kmb::PhysicalValue::HASH && dIter->second->getIdCount() > 0 ){
				kmb::HashValue* mat = reinterpret_cast<kmb::HashValue*>( dIter->second->getPhysicalValue() );
				switch( this->soltype )
				{
				case kmb::HECMWIO::STATIC:
				case kmb::HECMWIO::EIGEN:
				case kmb::HECMWIO::DYNAMIC:
					{
						int item = 0;
						if( mat->getValue("young") && mat->getValue("poisson") ){
							++item;
						}
						if( mat->getValue("density") ){
							++item;
						}
						if( mat->getValue("linearexp") ){
							++item;
						}
						if( item > 0 ){
							output << "!MATERIAL, NAME=" << dIter->first << ", ITEM=" << item << std::endl;
							int index=0;
							if( mat->getValue("young") && mat->getValue("poisson") ){
								output << "!ITEM=" << index << ", SUBITEM=2" << std::endl;
								++index;
								double young=0.0, poisson=0.0;
								mat->getValue("young")->getValue(&young);
								mat->getValue("poisson")->getValue(&poisson);
								output << young << ", " << poisson << std::endl;
							}
							if( mat->getValue("density") ){
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								++index;
								double density=0.0;
								mat->getValue("density")->getValue(&density);
								output << density << std::endl;
							}
							if( mat->getValue("linearexp") ){
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								++index;
								double linearexp=0.0;
								mat->getValue("linearexp")->getValue(&linearexp);
								output << linearexp << std::endl;
							}
						}
					}
					break;
				case kmb::HECMWIO::HEAT:
					{
						int item = 0;
						if( mat->getValue("density") ){
							++item;
						}
						if( mat->getValue("specific_heat") ){
							++item;
						}
						if( mat->getValue("thermal_conductivity") ){
							++item;
						}
						if( item > 0 ){
							output << "!MATERIAL, NAME=" << dIter->first << ", ITEM=" << item << std::endl;
							int index=0;
							if( mat->getValue("density") ){
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								++index;
								double density=0.0;
								mat->getValue("density")->getValue(&density);
								output << density << std::endl;
							}
							if( mat->getValue("specific_heat") ){
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								++index;
								double specific_heat=0.0;
								mat->getValue("specific_heat")->getValue(&specific_heat);
								output << specific_heat << std::endl;
							}
							if( mat->getValue("thermal_conductivity") ){
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								++index;
								double thermal_conductivity=0.0;
								mat->getValue("themal_conductivity")->getValue(&thermal_conductivity);
								output << thermal_conductivity << std::endl;
							}
						}
					}
					break;
				}
			}
			++dIter;
		}




		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			std::string matName;
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
			while( dIter != dEnd ){
				if( dIter->second->getSpecType() == "Material" && dIter->second->hasId( bodyId ) ){
					matName = dIter->first;
					break;
				}
				++dIter;
			}
			if( matName.size() != 0 ){
				const kmb::PhysicalValue* v = mesh->getPhysicalValueAtId( "SECTION", bodyId, "SECTION" );
				if( v && v->getType() == kmb::PhysicalValue::HASH ){
					const kmb::HashValue* h = reinterpret_cast< const kmb::HashValue* >(v);
					kmb::PhysicalValue* u = NULL;
					u=h->getValue("TYPE");
					if( u != NULL && u->getType() == kmb::PhysicalValue::STRING ){
						output << "!SECTION, TYPE=" << reinterpret_cast<kmb::TextValue*>(u)->getValue() << ",";
						if( strlen( mesh->getBodyName(bodyId) ) == 0 ){
							output << "EGRP=Body_" << bodyId << ", MATERIAL=" << matName << std::endl;
						}else{
							output << "EGRP=" << mesh->getBodyName(bodyId) << ", MATERIAL=" << matName << std::endl;
						}
						u = h->getValue("THICKNESS");
						if( u != NULL && u->getType() == kmb::PhysicalValue::SCALAR ){
							double x=0.0;
							u->getValue(&x);
							output << x;
							u = h->getValue("INTEGPOINTS");
							if( u != NULL && u->getType() == kmb::PhysicalValue::SCALAR ){
								u->getValue(&x);
								output << ", " << x;
							}
							u = h->getValue("GAPCON");
							if( u != NULL && u->getType() == kmb::PhysicalValue::SCALAR ){
								u->getValue(&x);
								output << ", " << x;
							}
							u = h->getValue("GAPRAD1");
							if( u != NULL && u->getType() == kmb::PhysicalValue::SCALAR ){
								u->getValue(&x);
								output << ", " << x;
							}
							u = h->getValue("GAPRAD2");
							if( u != NULL && u->getType() == kmb::PhysicalValue::SCALAR ){
								u->getValue(&x);
								output << ", " << x;
							}
							output << std::endl;
						}
					}
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body )
			{
				if( strlen( body->getBodyName() ) == 0 ){
					output << "!EGROUP, EGRP=Body_" << bodyId << std::endl;
				}else{
					output << "!EGROUP, EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					output << eIter.getId()+offsetElementId << std::endl;
					++eIter;
				}
			}
		}

		{
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
			while( dIter != dEnd ){
				if( dIter->second->getSpecType() == "BOUNDARY" ||
					dIter->second->getSpecType() == "CLOAD" ||
					dIter->second->getSpecType() == "FIXTEMP" ||
					dIter->second->getSpecType() == "CFLUX" ||
					dIter->second->getSpecType() == "TEMPERATURE" )
				{
					if( dIter->second->getBindingMode() == kmb::DataBindings::NODEGROUP ){
						output << "!NGROUP, NGRP=" << dIter->first << std::endl;
						kmb::DataBindings::const_iterator nIter = dIter->second->begin();
						while( !nIter.isFinished() ){
							output << nIter.getId()+offsetNodeId << std::endl;
							++nIter;
						}
					}
				}
				++dIter;
			}
		}

		{
			int tetFmap[] = {3,4,2,1};
			int wedFmap[] = {1,2,3,4,5};
			int hexFmap[] = {1,2,3,4,5,6};
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
			while( dIter != dEnd ){
				if( dIter->second->getSpecType() == "DLOAD" ||
					dIter->second->getSpecType() == "COUPLING" )
				{
					if( dIter->second->getBindingMode() == kmb::DataBindings::FACEGROUP ){
						output << "!SGROUP, SGRP=" << dIter->first << std::endl;
						kmb::DataBindings::const_iterator fIter = dIter->second->begin();
						while( !nIter.isFinished() ){
							kmb::Face f;
							fIter.getFace(f);
							switch( mesh->findElement( f.getElementId() ).getType() )
							{
							case kmb::TETRAHEDRON:
							case kmb::TETRAHEDRON2:
								output << f.getElementId()+offsetElementId << "," << tetFmap[f.getLocalFaceId()] << std::endl;
								break;
							case kmb::WEDGE:
							case kmb::WEDGE2:
								output << f.getElementId()+offsetElementId << "," << wedFmap[f.getLocalFaceId()] << std::endl;
								break;
							case kmb::HEXAHEDRON:
							case kmb::HEXAHEDRON2:
								output << f.getElementId()+offsetElementId << "," << hexFmap[f.getLocalFaceId()] << std::endl;
								break;
							default:
								break;
							}
							++fIter;
						}
					}
				}
				++dIter;
			}
		}

		{
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
			while( dIter != dEnd ){
				if( dIter->second->getSpecType() == "AMPLITUDE" )
				{
					if( dIter->second->getValueType() == kmb::PhysicalValue::HASH ){
						const kmb::HashValue* h = reinterpret_cast< const kmb::HashValue* >( dIter->second );
						output << "!AMPLITUDE, NAME=" << dIter->first;
						const kmb::PhysicalValue* d = NULL;
						d = h->getValue("definition");
						if( d != NULL && d->getType() == kmb::PhysicalValue::STRING ){
							output << ", DEFINITION=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						d = h->getValue("time");
						if( d != NULL && d->getType() == kmb::PhysicalValue::STRING ){
							output << ", TIME=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						d = h->getValue("value");
						if( d != NULL && d->getType() == kmb::PhysicalValue::STRING ){
							output << ", VALUE=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						d = h->getValue("input");
						if( d != NULL && d->getType() == kmb::PhysicalValue::STRING ){
							output << ", INPUT=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						output << std::endl;
						d = h->getValue("table");
						if( d != NULL && d->getType() == kmb::PhysicalValue::ARRAY ){
							const kmb::ArrayValue* a = reinterpret_cast< const kmb::ArrayValue* >(d);
							size_t num = a->getSize();
							for(unsigned int i=0;i<num/2;++i){
								output << reinterpret_cast< const kmb::ScalarValue* >(a->getValue(2*i))->getValue() << ", ";
								output << reinterpret_cast< const kmb::ScalarValue* >(a->getValue(2*i))->getValue() << std::endl;
							}
						}
					}
				}
				++dIter;
			}
		}
		output.close();
	}
	return 0;
}
