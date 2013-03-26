/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : HecmwIO                                                 #
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






#include "RevocapIO/kmbHecmwIO.h"
#include "RevocapIO/kmbRevocapIOUtils.h"
#include "RevocapIO/kmbFortranIO.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbFaceBucket.h"
#include "MeshDB/kmbElementEvaluator.h"

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iomanip>
#include <algorithm>


#ifdef WIN32
 #if _MSC_VER >= 1400
  #define sprintf sprintf_s
 #endif
#endif


kmb::HecmwIO::HecmwIO(kmb::nodeIdType offsetNodeId,kmb::elementIdType offsetElementId)
: offsetNodeId(0)
, offsetElementId(0)
, soltype(kmb::HecmwIO::STATIC)
, dataFlag(false)
, resElementFlag(true)
, dummySectionFlag(false)
, resHeader()
, egrpInfo()
{
	this->offsetNodeId = offsetNodeId;
	this->offsetElementId = offsetElementId;
	this->resHeader = "*fstrresult";
}

kmb::HecmwIO::~HecmwIO(void)
{
	clear();
}

void
kmb::HecmwIO::clear(void)
{
	egrpInfo.clear();
}

void
kmb::HecmwIO::setOffsetElementId(kmb::elementIdType offset)
{
	offsetElementId = offset;
}

void
kmb::HecmwIO::setOffsetNodeId(kmb::nodeIdType offset)
{
	offsetNodeId = offset;
}

kmb::elementIdType
kmb::HecmwIO::getOffsetElementId(void) const
{
	return this->offsetElementId;
}

kmb::nodeIdType
kmb::HecmwIO::getOffsetNodeId(void) const
{
	return this->offsetNodeId;
}

void
kmb::HecmwIO::setDataFlag(bool flag)
{
	this->dataFlag = flag;
}

void
kmb::HecmwIO::setResElementFlag(bool flag)
{
	this->resElementFlag = flag;
}

void
kmb::HecmwIO::setDummySectionFlag(bool flag)
{
	this->dummySectionFlag = flag;
}

void
kmb::HecmwIO::setResHeader(const char* header)
{
	this->resHeader = header;
}

kmb::HecmwIO::solutionType
kmb::HecmwIO::setSolutionType( const char* soltype )
{
	if( strcmp(soltype,"STATIC")==0 ){
		this->soltype = kmb::HecmwIO::STATIC;
	}else if( strcmp(soltype,"NLSTATIC")==0 ){
		this->soltype = kmb::HecmwIO::NLSTATIC;
	}else if( strcmp(soltype,"HEAT")==0 ){
		this->soltype = kmb::HecmwIO::HEAT;
	}else if( strcmp(soltype,"EIGEN")==0 ){
		this->soltype = kmb::HecmwIO::EIGEN;
	}else if( strcmp(soltype,"DYNAMIC")==0 ){
		this->soltype = kmb::HecmwIO::DYNAMIC;
	}
	return this->soltype;
}

const char*
kmb::HecmwIO::getSolutionType( void ) const
{
	switch( this->soltype )
	{
	case kmb::HecmwIO::STATIC:
		return "STATIC";
	case kmb::HecmwIO::NLSTATIC:
		return "NLSTATIC";
	case kmb::HecmwIO::HEAT:
		return "HEAT";
	case kmb::HecmwIO::EIGEN:
		return "EIGEN";
	case kmb::HecmwIO::DYNAMIC:
		return "DYNAMIC";
	default:
		break;
	}
	return "UNKNOWN";
}

int
kmb::HecmwIO::getHECType( kmb::elementType etype )
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
kmb::HecmwIO::getRevocapType( int etype )
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
	case 731:	return kmb::TRIANGLE;
	case 732:	return kmb::TRIANGLE2;
	case 741:	return kmb::QUAD;
	case 742:	return kmb::QUAD2;
	}
	return kmb::UNKNOWNTYPE;
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

int
kmb::HecmwIO::getEgrpInfoIndex( std::string name ) const
{
	int index = -1;
	for(unsigned int i=0;i<egrpInfo.size();++i){
		if( egrpInfo[i].egrpName == name ){
			index = i;
			break;
		}
	}
	return index;
}

int
kmb::HecmwIO::createEgrpInfo( std::string name, std::string matname )
{
	int index = static_cast<int>( egrpInfo.size() );
	egrpData e = {name,matname,0,-1};
	egrpInfo.push_back(e);
	return index;
}

int
kmb::HecmwIO::readHeader( std::ifstream &input, std::string &line ) const
{
	while( !input.eof() ){
		std::getline( input, line );
		if( line.find("!") == 0 ){
			break;
		}
	}
	return 0;
}

size_t
kmb::HecmwIO::readNode( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const
{
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	size_t nodeCount = 0;
	double x=0.0,y=0.0,z=0.0;
	char comma;
	if( mesh == NULL ){
		while( !input.eof() ){
			std::getline( input, line );
			if( line.find("!") == 0 ){
				break;
			}else{
				++nodeCount;
			}
		}
	}else{
		while( !input.eof() ){
			std::getline( input, line );
			if( line.find("!") == 0 ){
				break;
			}else{
				std::istringstream tokenizer(line);
				tokenizer >> nodeId >> comma >> x >> comma >> y >> comma >> z;
				mesh->addNodeWithId( x,y,z, nodeId-offsetNodeId );
				++nodeCount;
			}
		}
	}
	return nodeCount;
}

size_t
kmb::HecmwIO::readElement( std::ifstream &input, std::string &line, kmb::ElementContainer* body ) const
{
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	int typeNum = atoi( kmb::RevocapIOUtils::getValue( line, "TYPE" ).c_str() );
	kmb::elementType etype = kmb::HecmwIO::getRevocapType( typeNum );
	size_t count = 0;
	if( body && etype != kmb::UNKNOWNTYPE ){
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
			body->addElement( etype, nodeTable, elementId - this->offsetElementId );
			++count;
		}
		delete[] nodeTable;
	}else{
		while( !input.eof() ){
			std::getline( input, line );
			if( line.find("!") == 0 ){
				break;
			}
			++count;
		}
	}
	return count;
}

kmb::bodyIdType
kmb::HecmwIO::readSection( std::ifstream &input, std::string &line, kmb::MeshData* mesh )
{
	kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
	if( mesh != NULL ){
		kmb::DataBindings* sectionInfo = mesh->getDataBindingsPtr( "SECTION", "SECTION" );
		if( sectionInfo == NULL ){
			sectionInfo = mesh->createDataBindings( "SECTION",
				kmb::DataBindings::BodyVariable,
				kmb::PhysicalValue::Hash,
				"SECTION");
		}
		std::string name = kmb::RevocapIOUtils::getValue( line, "EGRP" );
		std::string matname = kmb::RevocapIOUtils::getValue( line, "MATERIAL" );

		if( name.size() > 0 ){

			int index = getEgrpInfoIndex( name );
			if( index != -1 ){

				egrpInfo[index].materialName = matname;
			}else{

				index = createEgrpInfo( name, matname );
			}
			std::cout << "section " << egrpInfo[index].bodyId << " " << egrpInfo[index].egrpName << " => " << egrpInfo[index].materialName << std::endl;

			bodyId = mesh->beginElement( egrpInfo[index].egrpCount );
			mesh->endElement();
			mesh->setBodyName( bodyId, name.c_str() );
			egrpInfo[index].bodyId = bodyId;

			kmb::HashValue* hash = new kmb::HashValue();
			std::string typestr = kmb::RevocapIOUtils::getValue( line, "TYPE" );
			hash->setValue( "TYPE", new kmb::TextValue( typestr.c_str() ) );
			hash->setValue( "EGRP", new kmb::TextValue( name.c_str() ) );
			hash->setValue( "MATERIAL", new kmb::TextValue( matname.c_str() ) );
			sectionInfo->setPhysicalValue( bodyId, hash );

			std::getline( input, line );
			if( line.find("!") == 0 || line.size() == 0 ){


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
		}else{
			while( !input.eof() ){
				std::getline( input, line );
				if( line.find("!") == 0 ){
					break;
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
	return bodyId;
}

int
kmb::HecmwIO::readMaterial( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const
{
	if( mesh == NULL ){
		while( !input.eof() ){
			std::getline( input, line );
			if( line.find("!") == 0 && line.find("!ITEM") != 0){
				break;
			}
		}
	}else{
		double value;
		char comma;
		std::stringstream ss;
		std::string matname = kmb::RevocapIOUtils::getValue(line,"NAME");
		kmb::DataBindings* data = mesh->getDataBindingsPtr( matname.c_str(), "Material" );
		if( data == NULL ){
			data = mesh->createDataBindings( matname.c_str(),
			kmb::DataBindings::BodyGroup,
			kmb::PhysicalValue::Hash,
			"Material");
		}
		std::cout << "material " << matname << std::endl;
		kmb::HashValue* material = new kmb::HashValue();
		data->setPhysicalValue( material );
		int item = atoi( kmb::RevocapIOUtils::getValue( line, "ITEM" ).c_str() );
		for(int i=0;i<item;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			ss << kmb::RevocapIOUtils::getValue( line, "SUBITEM" );
			int subitem = 0;
			ss >> subitem;


			if( subitem == 0 ){
				subitem = 1;
			}
			std::getline( input, line );
			std::istringstream tokenizer(line);
			for(int j=0;j<subitem;++j){
				if( !tokenizer.good() ){
					continue;
				}
				value = 0.0;
				tokenizer >> value;

				tokenizer >> comma;

				switch(this->soltype)
				{
					case kmb::HecmwIO::STATIC:
					case kmb::HecmwIO::NLSTATIC:
					case kmb::HecmwIO::EIGEN:
					case kmb::HecmwIO::DYNAMIC:
						if( i==0 ){
							if( j==0 ){
								std::cout << "youngs " << value << std::endl;
								material->setValue("youngs", new kmb::ScalarValue( value ) );
							}else if( j==1 ){
								std::cout << "poisson " << value << std::endl;
								material->setValue("poisson", new kmb::ScalarValue( value ) );
							}
						}else if( i==1 ){
							if( j==0 ){
								std::cout << "density " << value << std::endl;
								material->setValue("density", new kmb::ScalarValue( value ) );
							}
						}else if( i==2 ){
							if( j==0 ){
								std::cout << "linearexp " << value << std::endl;
								material->setValue("linearexp", new kmb::ScalarValue( value ) );
							}
						}
						break;
					case kmb::HecmwIO::HEAT:
						if( i==0 ){
							if( j==0 ){
								std::cout << "density " << value << std::endl;
								material->setValue("density", new kmb::ScalarValue( value ) );
							}
						}else if( i==1 ){
							if( j==0 ){
								std::cout << "specific_heat " << value << std::endl;
								material->setValue("specific_heat", new kmb::ScalarValue( value ) );
							}
						}else if( i==2 ){
							if( j==0 ){
								std::cout << "thermal_conductivity " << value << std::endl;
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
	}
	return 0;
}

size_t
kmb::HecmwIO::readEGroup( std::ifstream &input, std::string &line, kmb::MeshData* mesh, kmb::ElementContainer* parentBody )
{
	size_t count = 0;
	if( mesh == NULL ){
		std::string name = kmb::RevocapIOUtils::getValue( line, "EGRP" );

		int index = getEgrpInfoIndex( name );
		if( index == -1 ){

			index = createEgrpInfo( name, "" );
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
	}else{
		std::string name = kmb::RevocapIOUtils::getValue( line, "EGRP" );
		kmb::bodyIdType bodyId_eg = kmb::Body::nullBodyId;
		size_t count = 0;
		std::string matName;

		int index = getEgrpInfoIndex( name );
		if( index != -1 ){
			bodyId_eg = egrpInfo[index].bodyId;
			count = egrpInfo[index].egrpCount;
			matName = egrpInfo[index].materialName;
		}
		if( bodyId_eg != kmb::Body::nullBodyId ){

			if( parentBody != NULL ){
				kmb::ElementContainer* body = mesh->getBodyPtr( bodyId_eg );
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
							kmb::ElementContainer::iterator eIter = parentBody->find( elementId - this->offsetElementId );
							if( eIter.getElement( etype, nodeTable ) ){
								body->addElement( etype, nodeTable, elementId - this->offsetElementId );
							}
						}
						parentBody->deleteElement( elementId - this->offsetElementId );
					}
				}
				delete[] nodeTable;
			}
		}else{

			kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
			if( data == NULL && dataFlag ){
				data = mesh->createDataBindings( name.c_str(), kmb::DataBindings::ElementGroup, kmb::PhysicalValue::None );
			}


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
	}
	return count;
}

size_t
kmb::HecmwIO::readNGroup( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const
{
	size_t count = 0;
	if( mesh == NULL ){
		while( !input.eof() ){
			std::getline( input, line );
			if( line.find("!") == 0 ){
				break;
			}
		}
	}else{
		std::string name = kmb::RevocapIOUtils::getValue( line, "NGRP" );
		bool generateFlag = kmb::RevocapIOUtils::hasKey( line, "GENERATE" );
		kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
		if( data == NULL && dataFlag ){
			data = mesh->createDataBindings( name.c_str(), kmb::DataBindings::NodeGroup, kmb::PhysicalValue::None );
		}


		if( data ){
			kmb::nodeIdType nodeId = kmb::nullNodeId;
			kmb::nodeIdType nodeId0 = kmb::nullNodeId;
			kmb::nodeIdType nodeId1 = kmb::nullNodeId;
			int interval = 1;
			while( !input.eof() ){
				std::getline( input, line );
				if( line.find("!") == 0 ){
					break;
				}else{
					std::istringstream tokenizer(line);
					if( generateFlag ){
						tokenizer >> nodeId0;
						if( tokenizer.get() != ',' ){
							tokenizer.unget();
						}
						tokenizer >> nodeId1;
						if( tokenizer.get() != ',' ){
							tokenizer.unget();
						}
						tokenizer >> interval;
						for( nodeId = nodeId0; nodeId <= nodeId1; nodeId += interval ){
							data->addId( nodeId - this->offsetNodeId );
							++count;
						}
					}else{
						while( !tokenizer.eof() ){
							tokenizer >> nodeId;

							if( tokenizer.get() != ',' ){
								tokenizer.unget();
							}
							data->addId( nodeId - this->offsetNodeId );
							++count;
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
	}
	return count;
}

size_t
kmb::HecmwIO::readSGroup( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const
{
	size_t count = 0;
	if( mesh == NULL ){
		while( !input.eof() ){
			std::getline( input, line );
			if( line.find("!") == 0 ){
				break;
			}
		}
	}else{
		int tetFmap[] = {3,2,0,1};
		int wedFmap[] = {0,1,2,3,4};
		int hexFmap[] = {0,1,2,3,4,5};
		std::string name = kmb::RevocapIOUtils::getValue( line, "SGRP" );
		kmb::DataBindings* data = mesh->getDataBindingsPtr( name.c_str() );
		if( data == NULL && dataFlag ){
			data = mesh->createDataBindings( name.c_str(), kmb::DataBindings::FaceGroup, kmb::PhysicalValue::None );
		}
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
							eIter = mesh->findElement( elementId - this->offsetElementId );
						}
						switch( eIter.getType() )
						{
						case kmb::TETRAHEDRON:
						case kmb::TETRAHEDRON2:
							f.setId( elementId - this->offsetElementId, tetFmap[ localId-1 ] );
							data->addId( f );
							++count;
							break;
						case kmb::WEDGE:
						case kmb::WEDGE2:
							f.setId( elementId - this->offsetElementId, wedFmap[ localId-1 ] );
							data->addId( f );
							++count;
							break;
						case kmb::HEXAHEDRON:
						case kmb::HEXAHEDRON2:
							f.setId( elementId - this->offsetElementId, hexFmap[ localId-1 ] );
							data->addId( f );
							++count;
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
	}
	return count;
}

/*
 * (1) elementCount ‚Æ nodeCount ‚ð’²‚×‚é
 * (2) element ‚ª‚Ç‚Ì body ‚É‘®‚·‚é‚©‚ð’²‚×‚é
 *     !EGROUP ‚Å’è‹`‚³‚ê‚Ä‚¢‚éê‡
 *     !ELEMENT, EGRP=XXX ‚Å’è‹`‚³‚ê‚Ä‚¢‚éê‡
 * (3) body –ˆ‚Ì material ‚Æ‚Ì‘Î‰ž‚ð’²‚×‚é
 * ‹ó“Ç‚ÝŽž‚Ì\‘¢‘Ì
 * egrpName
 * materialName
 * elementCount
 * materialName ‚ª—^‚¦‚ç‚ê‚Ä‚¢‚½‚ç Body ‚Æ‚Ý‚È‚·
 * egrpName = ALL ‚Å materialName ‚ª—^‚¦‚ç‚ê‚Ä‚¢‚é‚Æ‚«‚ÍAˆÃ–Ù‚Ì‘S‚Ä‚Ì—v‘f
 */
int
kmb::HecmwIO::loadFromFile(const char* filename,MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}

		mesh->createDataBindings(
			"SECTION",
			kmb::DataBindings::BodyVariable,
			kmb::PhysicalValue::Hash,
			"SECTION");

		kmb::ElementContainerMap elements;

		size_t nodeCount = 0;
		kmb::bodyIdType bodyId = 0;
		std::string line;


		std::getline( input, line );
		while( !input.eof() ){

			if( line.find("!HEADER") == 0 ){
				readHeader( input, line );
			}else if( line.find("!NODE") == 0 ){
				nodeCount = readNode( input, line, NULL );
			}else if( line.find("!ELEMENT") == 0 ){
				std::string name = kmb::RevocapIOUtils::getValue( line, "EGRP" );
				size_t count = 0;
				if( name != "" ){



					int index = getEgrpInfoIndex( name );
					if( index == -1 ){

						index = createEgrpInfo( name, "" );
					}


					count = readElement( input, line, NULL );
					egrpInfo[index].egrpCount += count;
				}else{



					int index = getEgrpInfoIndex( "ALL" );
					if( index == -1 ){

						index = createEgrpInfo( "ALL", "" );
					}

					count = readElement( input, line, &elements );
					egrpInfo[index].egrpCount += count;
				}
			}else if( line.find("!SECTION") == 0 ){

				bodyId = readSection( input, line, mesh );
			}else if( line.find("!MATERIAL") == 0 ){
				readMaterial( input, line, mesh );
			}else if( line.find("!EGROUP") == 0 ){
				readEGroup( input, line, NULL );
			}else if( line.find("!NGROUP") == 0 ){
				readNGroup( input, line, NULL );
			}else if( line.find("!SGROUP") == 0 ){
				readSGroup( input, line, NULL );
			}else{
				std::getline( input, line );
			}
		}
		input.clear();
		input.seekg( 0, std::ios_base::beg	);




		{
			std::string matName;
			std::vector< kmb::HecmwIO::egrpData >::iterator egrpIter = egrpInfo.begin();
			while( egrpIter != egrpInfo.end() ){

				std::cout << "material binding " << egrpIter->bodyId << " " << egrpIter->egrpName << " => " << egrpIter->materialName << std::endl;
				if( egrpIter->bodyId != kmb::Body::nullBodyId ){

					matName = egrpIter->materialName;
					if( matName.size() > 0 && mesh->hasData( matName.c_str() ) ){
						mesh->addId( matName.c_str(), egrpIter->bodyId );
					}
					if( egrpIter->egrpName == "ALL" ){
						kmb::ElementContainer* body = mesh->getBodyPtr( egrpIter->bodyId );
						kmb::ElementContainer::iterator eIter = elements.begin();
						kmb::nodeIdType* nodeTable = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
						kmb::elementType etype = kmb::UNKNOWNTYPE;
						while( !eIter.isFinished() ){
							eIter.getElement( etype, nodeTable );
							body->addElement( etype, nodeTable, eIter.getId() );
							++eIter;
						}
						delete[] nodeTable;
						elements.clear();
						break;
					}
				}
				++egrpIter;
			}
		}



		mesh->beginNode( nodeCount );
		mesh->endNode();

		std::getline( input, line );
		while( !input.eof() ){

			if( line.find("!HEADER") == 0 ){
				readHeader( input, line );
			}else if( line.find("!NODE") == 0 ){
				readNode( input, line, mesh );
			}else if( line.find("!ELEMENT") == 0 ){
				std::string name = kmb::RevocapIOUtils::getValue( line, "EGRP" );
				if( name == "" ){

					readElement( input, line, NULL );
				}else{

					kmb::bodyIdType bodyId_eg = kmb::Body::nullBodyId;
					std::string matName_eg;



					int index = getEgrpInfoIndex( name );
					if( index != -1 ){
						bodyId_eg = egrpInfo[index].bodyId;
						matName_eg = egrpInfo[index].materialName;
					}else{
						std::cout << "Warning (HecmwIO) : not find section of egrp " << name << std::endl;
					}
					if( bodyId_eg != kmb::Body::nullBodyId ){

						kmb::ElementContainer* body = mesh->getBodyPtr( bodyId_eg );
						readElement( input, line, body );




					}else{

						readElement( input, line, NULL );
					}
				}
			}else if( line.find("!SECTION") == 0 ){
				readSection( input, line, NULL );
			}else if( line.find("!MATERIAL") == 0 ){
				readMaterial( input, line, NULL );
			}else if( line.find("!EGROUP") == 0 ){
				readEGroup( input, line, mesh, &elements );
			}else if( line.find("!NGROUP") == 0 ){
				readNGroup( input, line, mesh );
			}else if( line.find("!SGROUP") == 0 ){
				readSGroup( input, line, mesh );
			}else{
				std::getline( input, line );
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::HecmwIO::loadFromFRSFile(const char* filename,kmb::MeshData* mesh) const
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
			nodeCount = atoi( kmb::RevocapIOUtils::getValue( line, "NNODE" ).c_str() );
			elementCount = atoi( kmb::RevocapIOUtils::getValue( line, "NELEM" ).c_str() );
			continue;
		}else if( line.find("NPARAM") == 0 ){
			int paramNum = atoi( kmb::RevocapIOUtils::getValue( line, "NPARAM" ).c_str() );
			int* dims = new int[paramNum];
			int dimCount = 0;
			std::getline( input, line );
			std::istringstream tokenizer(line);
			for(int i=0;i<paramNum;++i){
				tokenizer >> dims[i];
				dimCount += dims[i];
			}
			for(int i=0;i<paramNum;++i){
				kmb::RevocapIOUtils::readOneLine( input, line );
				switch( dims[i] )
				{
				case 1:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::Global, kmb::PhysicalValue::Scalar, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				case 3:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::Global, kmb::PhysicalValue::Vector3, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				case 6:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::Global, kmb::PhysicalValue::Tensor6, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				default:
					break;
				}
			}
			double* values = new double[dimCount];
			std::fill(values,values+dimCount,0.0);
			{
				std::getline( input, line );
				tokenizer.str(line);
				tokenizer.clear();
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
			int ndataNum = atoi( kmb::RevocapIOUtils::getValue( line, "NNDATA" ).c_str() );
			int* dims = new int[ndataNum];
			int dimCount = 0;
			std::getline( input, line );
			std::istringstream tokenizer(line);
			for(int i=0;i<ndataNum;++i){
				tokenizer >> dims[i];
				dimCount += dims[i];
			}
			for(int i=0;i<ndataNum;++i){
				kmb::RevocapIOUtils::readOneLine( input, line );
				switch( dims[i] )
				{
				case 1:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				case 3:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				case 6:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				default:
					break;
				}
			}
			double* values = new double[dimCount];
			std::fill_n(values,dimCount,0.0);
			for(int i=0;i<nodeCount;++i){
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				std::getline( input, line );
				tokenizer.str(line);
				tokenizer.clear();
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
			int edataNum = atoi( kmb::RevocapIOUtils::getValue( line, "NEDATA" ).c_str() );
			int* dims = new int[edataNum];
			int dimCount = 0;
			std::getline( input, line );
			std::istringstream tokenizer(line);
			for(int i=0;i<edataNum;++i){
				tokenizer >> dims[i];
				dimCount += dims[i];
			}
			for(int i=0;i<edataNum;++i){
				kmb::RevocapIOUtils::readOneLine( input, line );
				switch( dims[i] )
				{
				case 1:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				case 3:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector3, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				case 6:
					mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
					mesh->appendTargetData( line.c_str() );
					break;
				default:
					break;
				}
			}
			double* values = new double[dimCount];
			std::fill_n(values,dimCount,0.0);
			for(int i=0;i<elementCount;++i){
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				std::getline( input, line );
				tokenizer.str(line);
				tokenizer.clear();
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
kmb::HecmwIO::loadFromResFile(const char* filename,kmb::MeshData* mesh) const
{
	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	if( input.fail() ){
		return -1;
	}

	char buf[64];
	input.read( buf, 19 );
	buf[19] = '\0';
	if( strncmp( "HECMW_BINARY_RESULT", buf, 19 ) == 0 ){
		input.close();
		return loadFromResBinFile(filename,mesh);
	}
	input.clear();
	input.seekg(0, std::ios::beg);

	input.get( buf, 64, '\0' );
	if( strncmp( resHeader.c_str(), buf, resHeader.size() ) == 0 ){
		std::cout << "HECMW ASCII RESULT READING..." << std::endl;
		input.close();
		return loadFromResAsciiFile(filename,mesh);
	}
	input.close();
	return -2;
}

int
kmb::HecmwIO::loadFromResFileItem(const char* filename,kmb::DataBindings* data,const char* name) const
{
	if( data == NULL ){
		return -1;
	}

	std::ifstream input( filename, std::ios_base::in );
	if( input.fail() ){
		return -1;
	}
	int nodeCount = 0;
	int elementCount = 0;
	int nodeValueCount = 0;
	int elementValueCount = 0;

	int nodeValueOffset = -1;
	int elementValueOffset = -1;

	int dataDim = 0;
	std::vector<int> nodeValDims;
	std::vector<int> elementValDims;
	std::string line;

	if( std::getline( input, line ) ){
		if( line.find( resHeader) != 0 ){
			return -1;
		}
	}else{
		return -1;
	}

	if( std::getline( input, line ) ){
		std::istringstream tokenizer(line);
		tokenizer >> nodeCount;
		tokenizer >> elementCount;
	}

	if( std::getline( input, line ) ){
		std::istringstream tokenizer(line);
		tokenizer >> nodeValueCount;
		tokenizer >> elementValueCount;
	}
	if( nodeValueCount > 0 ){

		if( std::getline( input, line ) ){
			std::istringstream tokenizer(line);
			for(int i=0;i<nodeValueCount;++i){
				int d = 0;
				tokenizer >> d;
				nodeValDims.push_back(d);
			}
		}

		int dim = 0;
		for(int i=0;i<nodeValueCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			if( line == name &&
				data->getBindingMode() == kmb::DataBindings::NodeVariable &&
				data->getDimension() == nodeValDims[i] ){
				nodeValueOffset = dim;
				dataDim = nodeValDims[i];
			}
			dim += nodeValDims[i];
		}

		kmb::nodeIdType nodeId = kmb::nullNodeId;
		std::istringstream tokenizer;
		double* v = new double[dim];
		for(int i=0;i<nodeCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer >> nodeId;
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			for(int j=0;j<dim;++j){
				if( tokenizer.eof() ){
					kmb::RevocapIOUtils::readOneLine( input, line );
					tokenizer.str(line);
					tokenizer.clear();
				}
				tokenizer >> v[j];
			}
			if( nodeValueOffset >= 0 ){
				data->setPhysicalValue( nodeId, &v[nodeValueOffset] );
			}
		}
		delete[] v;
	}
	if( elementValueCount > 0 ){

		if( std::getline( input, line ) ){
			std::istringstream tokenizer(line);
			for(int i=0;i<elementValueCount;++i){
				int d = 0;
				tokenizer >> d;
				elementValDims.push_back(d);
			}
		}

		int dim = 0;
		for(int i=0;i<elementValueCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			if( line == name &&
				data->getBindingMode() == kmb::DataBindings::ElementVariable &&
				data->getDimension() == elementValDims[i] ){
				elementValueOffset = dim;
				dataDim = elementValDims[i];
			}
			dim += elementValDims[i];
		}

		kmb::elementIdType elementId = kmb::Element::nullElementId;
		std::istringstream tokenizer;
		double* v = new double[dim];
		for(int i=0;i<elementCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer >> elementId;
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			for(int j=0;j<dim;++j){
				if( tokenizer.eof() ){
					kmb::RevocapIOUtils::readOneLine( input, line );
					tokenizer.str(line);
					tokenizer.clear();
				}
				tokenizer >> v[j];
			}
			if( elementValueOffset >= 0 ){
				data->setPhysicalValue( elementId, &v[elementValueOffset] );
			}
		}
		delete[] v;
	}
	input.close();
	return 0;
}

int
kmb::HecmwIO::loadFromResAsciiFile(const char* filename,kmb::MeshData* mesh) const
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
	int nodeValueCount = 0;
	int elementValueCount = 0;
	std::vector<int> nodeValDims;
	std::vector<int> elementValDims;
	std::string line;

	if( std::getline( input, line ) ){
		if( line.find( resHeader) != 0 ){
			std::cout << "Warning Hecmw Result file format error." << std::endl;
			input.close();
			return -1;
		}
	}else{
		return -1;
	}

	if( std::getline( input, line ) ){
		std::istringstream tokenizer(line);
		tokenizer >> nodeCount;
		tokenizer >> elementCount;
	}

	if( std::getline( input, line ) ){
		std::istringstream tokenizer(line);
		tokenizer >> nodeValueCount;
		tokenizer >> elementValueCount;
	}
	mesh->clearTargetData();
	std::cout << "nodeValueCount " << nodeValueCount << std::endl;
	if( nodeValueCount > 0 ){

		if( std::getline( input, line ) ){
			std::istringstream tokenizer(line);
			for(int i=0;i<nodeValueCount;++i){
				int d = 0;
				tokenizer >> d;
				nodeValDims.push_back(d);
			}
		}

		int dim = 0;
		for(int i=0;i<nodeValueCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			switch( nodeValDims[i] ){
				case 1:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 1;
					break;
				case 2:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector2, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 2;
					break;
				case 3:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 3;
					break;
				case 6:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 6;
					break;
				case 7:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					if( !mesh->hasData( "MISES" ) ){
						mesh->createDataBindings( "MISES", kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
					}
					mesh->appendTargetData( "MISES" );
					dim += 7;
					break;
				default:
					break;
			}
		}

		std::cout << "nodeCount " << nodeCount << " dim " << dim << std::endl;
		kmb::nodeIdType nodeId = kmb::nullNodeId;
		std::istringstream tokenizer;
		double* v = new double[dim];
		for(int i=0;i<nodeCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			tokenizer >> nodeId;
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			for(int j=0;j<dim;++j){
				if( tokenizer.eof() ){
					kmb::RevocapIOUtils::readOneLine( input, line );
					tokenizer.str(line);
					tokenizer.clear();
				}
				tokenizer >> v[j];
			}
			mesh->setMultiPhysicalValues( nodeId - this->offsetNodeId, v );
		}
		delete[] v;
	}
	mesh->clearTargetData();
	std::cout << "elementValueCount " << elementValueCount << std::endl;
	if( elementValueCount > 0 && this->resElementFlag ){

		if( std::getline( input, line ) ){
			std::istringstream tokenizer(line);
			for(int i=0;i<elementValueCount;++i){
				int d = 0;
				tokenizer >> d;
				elementValDims.push_back(d);
			}
		}

		int dim = 0;
		for(int i=0;i<elementValueCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			switch( elementValDims[i] ){
				case 1:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 1;
					break;
				case 2:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector2, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 2;
					break;
				case 3:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector3, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 3;
					break;
				case 6:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					dim += 6;
					break;
				case 7:
					if( !mesh->hasData( line.c_str() ) ){
						mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
					}
					mesh->appendTargetData( line.c_str() );
					if( !mesh->hasData( "EMISES" ) ){
						mesh->createDataBindings( "EMISES", kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
					}
					mesh->appendTargetData( "EMISES" );
					dim += 7;
					break;
				default:
					break;
			}
		}

		kmb::elementIdType elementId = kmb::Element::nullElementId;
		std::istringstream tokenizer;
		double* v = new double[dim];
		for(int i=0;i<elementCount;++i){
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			tokenizer >> elementId;
			kmb::RevocapIOUtils::readOneLine( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			for(int j=0;j<dim;++j){
				if( tokenizer.eof() ){
					kmb::RevocapIOUtils::readOneLine( input, line );
					tokenizer.str(line);
					tokenizer.clear();
				}
				tokenizer >> v[j];
			}
			mesh->setMultiPhysicalValues( elementId - this->offsetElementId, v );
		}
		delete[] v;
	}
	input.close();

	return 0;
}

int
kmb::HecmwIO::loadFromResBinFile(const char* filename,kmb::MeshData* mesh) const
{
	if( mesh == NULL ){
		return -1;
	}
	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	if( input.fail() ){
		return -1;
	}
	long nodeCount = 0;
	long elementCount = 0;
	long nodeValueCount = 0;
	long elementValueCount = 0;
	int dim = 0;
	std::vector<int> nodeValDims;
	std::vector<int> elementValDims;

	char buf[64];
	input.read( buf, 19 );
	buf[19] = '\0';
	if( strncmp( "HECMW_BINARY_RESULT", buf, 19 ) != 0 ){
		input.close();
		return -1;
	}

	input.read( buf, 2 );
	buf[2] = '\0';
	std::cout << "HECMW BINARY RESULT INTEGERSIZE : " << buf << std::endl;

	input.get( buf, 64, '\0' );
	if( resHeader != buf ){
		input.close();
		return -2;
	}

	input.read( buf, 1 );

	input.read(reinterpret_cast<char*>(&nodeCount),sizeof(long));
	input.read(reinterpret_cast<char*>(&elementCount),sizeof(long));
	input.read(reinterpret_cast<char*>(&nodeValueCount),sizeof(long));
	input.read(reinterpret_cast<char*>(&elementValueCount),sizeof(long));



	mesh->clearTargetData();
	for(int i=0;i<nodeValueCount;++i){
		long dl = 0;
		input.read(reinterpret_cast<char*>(&dl),sizeof(long));
		int d = static_cast<int>(dl);
		nodeValDims.push_back(d);
	}
	dim = 0;
	for(int i=0;i<nodeValueCount;++i){
		input.get( buf, 64, '\0' );
		switch( nodeValDims[i] ){
			case 1:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 1;
				break;
			case 2:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector2, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 2;
				break;
			case 3:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 3;
				break;
			case 6:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 6;
				break;
			case 7:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Tensor6, "post" );
				}
				mesh->appendTargetData( buf );
				if( !mesh->hasData( "MISES" ) ){
					mesh->createDataBindings( "MISES", kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
				}
				mesh->appendTargetData( "MISES" );
				dim += 7;
				break;
			default:
				break;
		}
		input.read( buf, 1 );
	}
	if( nodeValueCount > 0 ){
		kmb::nodeIdType nodeId = kmb::nullNodeId;
		double* v = new double[dim];
		for(int i=0;i<nodeCount;++i){
			long il = 0;
			input.read(reinterpret_cast<char*>(&il),sizeof(long));
			nodeId = static_cast<kmb::nodeIdType>(il);
			input.read(reinterpret_cast<char*>(v),sizeof(double)*dim);
			mesh->setMultiPhysicalValues( nodeId - this->offsetNodeId, v );
		}
		delete[] v;
	}


	mesh->clearTargetData();
	for(int i=0;i<elementValueCount;++i){
		long dl = 0;
		input.read(reinterpret_cast<char*>(&dl),sizeof(long));
		int d = static_cast<int>(dl);
		elementValDims.push_back(d);
	}
	dim = 0;
	for(int i=0;i<elementValueCount;++i){
		input.get( buf, 64, '\0' );
		switch( elementValDims[i] ){
			case 1:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 1;
				break;
			case 2:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector2, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 2;
				break;
			case 3:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Vector3, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 3;
				break;
			case 6:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
				}
				mesh->appendTargetData( buf );
				dim += 6;
				break;
			case 7:
				if( !mesh->hasData( buf ) ){
					mesh->createDataBindings( buf, kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Tensor6, "post" );
				}
				mesh->appendTargetData( buf );
				if( !mesh->hasData( "EMISES" ) ){
					mesh->createDataBindings( "EMISES", kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
				}
				mesh->appendTargetData( "EMISES" );
				dim += 7;
				break;
			default:
				break;
		}
		input.read( buf, 1 );
	}
	if( elementValueCount > 0 ){
		kmb::elementIdType elementId = kmb::Element::nullElementId;
		double* v = new double[dim];
		for(int i=0;i<elementCount;++i){
			long il = 0;
			input.read(reinterpret_cast<char*>(&il),sizeof(long));
			elementId = static_cast<kmb::elementIdType>(il);
			input.read(reinterpret_cast<char*>(v),sizeof(double)*dim);
			mesh->setMultiPhysicalValues( elementId - this->offsetElementId, v );
		}
		delete[] v;
	}

	input.close();

	return 0;
}


int
kmb::HecmwIO::saveToFile(const char* filename, const kmb::MeshData* mesh) const
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		output << "!HEADER" << std::endl;
		output << " HECMW_MSH File generated by REVOCAP" << std::endl;
		output << "!NODE" << std::endl;
		double x,y,z;
		kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
		kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
		while( nIter != nIterEnd ){
			if( nIter.getXYZ(x,y,z) ){
				output << std::setw(9) << nIter.getId()+offsetNodeId << ", "
					<< std::scientific
					<< std::setprecision(8)
					<< x << ", " << y << ", " << z << std::endl;
			}
			++nIter;
		}

		kmb::bodyIdType bodyCount = mesh->getBodyCount();



		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getDimension() == 3 && body->getCountByType( kmb::TETRAHEDRON ) > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::TETRAHEDRON );
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::TETRAHEDRON ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getDimension() == 3 && body->getCountByType( kmb::TETRAHEDRON2 ) > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::TETRAHEDRON2 );
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::TETRAHEDRON2 ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << "," <<
							eIter[8]+offsetNodeId << "," <<
							eIter[9]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getDimension() == 3 && body->getCountByType( kmb::HEXAHEDRON ) > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::HEXAHEDRON );
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::HEXAHEDRON ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getDimension() == 3 && body->getCountByType( kmb::HEXAHEDRON2 ) > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::HEXAHEDRON2 );
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::HEXAHEDRON2 ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << "," <<
							eIter[8]+offsetNodeId << "," <<
							eIter[9]+offsetNodeId << "," <<
							eIter[10]+offsetNodeId << "," <<
							eIter[11]+offsetNodeId << "," <<
							eIter[12]+offsetNodeId << "," <<
							eIter[13]+offsetNodeId << "," <<
							eIter[14]+offsetNodeId << "," <<
							eIter[15]+offsetNodeId << "," <<
							eIter[16]+offsetNodeId << "," <<
							eIter[17]+offsetNodeId << "," <<
							eIter[18]+offsetNodeId << "," <<
							eIter[19]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getDimension() == 3 && body->getCountByType( kmb::WEDGE ) > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::WEDGE );
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::WEDGE ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getDimension() == 3 && body->getCountByType( kmb::WEDGE2 ) > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::WEDGE2 );
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::WEDGE2 ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << "," <<
							eIter[8]+offsetNodeId << "," <<
							eIter[9]+offsetNodeId << "," <<
							eIter[10]+offsetNodeId << "," <<
							eIter[11]+offsetNodeId << "," <<
							eIter[12]+offsetNodeId << "," <<
							eIter[13]+offsetNodeId << "," <<
							eIter[14]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		output.setf( std::ios::showpoint );
		output.unsetf( std::ios::fixed );
		std::multimap< std::string, kmb::DataBindings*> dataBindings = mesh->getDataBindingsMap();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
		std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
		bool matflag = false;
		while( dIter != dEnd ){
			kmb::HashValue* mat = NULL;
			if( dIter->second && dIter->second->getSpecType() == "Material" &&
				dIter->second->getValueType() == kmb::PhysicalValue::Hash &&
				dIter->second->getIdCount() > 0 &&
				(mat = dynamic_cast<kmb::HashValue*>( dIter->second->getPhysicalValue() )) != NULL )
			{
				switch( this->soltype )
				{
				case kmb::HecmwIO::STATIC:
				case kmb::HecmwIO::NLSTATIC:
				case kmb::HecmwIO::EIGEN:
				case kmb::HecmwIO::DYNAMIC:
					{
						int item = 0;
						if( mat->getValue("youngs") && mat->getValue("poisson") ){
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
							if( mat->getValue("youngs") && mat->getValue("poisson") ){
								++index;
								output << "!ITEM=" << index << ", SUBITEM=2" << std::endl;
								double young=0.0, poisson=0.0;
								mat->getValue("youngs")->getValue(&young);
								mat->getValue("poisson")->getValue(&poisson);
								output << std::setw(8) << young << ", " << std::setw(8) << poisson << std::endl;
							}
							if( mat->getValue("density") ){
								++index;
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								double density=0.0;
								mat->getValue("density")->getValue(&density);
								output << std::setw(8) << density << std::endl;
							}
							if( mat->getValue("linearexp") ){
								++index;
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								double linearexp=0.0;
								mat->getValue("linearexp")->getValue(&linearexp);
								output << std::setw(8) << linearexp << std::endl;
							}
							matflag = true;
						}else{

							output << "!MATERIAL, NAME=" << dIter->first << ", ITEM=1" << std::endl;
							output << "!ITEM=1, SUBITEM=2" << std::endl;
							output << " 0.0, 0.0" << std::endl;
							matflag = true;
						}
					}
					break;
				case kmb::HecmwIO::HEAT:
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
								++index;
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								double density=0.0;
								mat->getValue("density")->getValue(&density);
								output << std::setw(8) << density << std::endl;
							}
							if( mat->getValue("specific_heat") ){
								++index;
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								double specific_heat=0.0;
								mat->getValue("specific_heat")->getValue(&specific_heat);
								output << std::setw(8) << specific_heat << std::endl;
							}
							if( mat->getValue("thermal_conductivity") ){
								++index;
								output << "!ITEM=" << index << ", SUBITEM=1" << std::endl;
								double thermal_conductivity=0.0;
								mat->getValue("thermal_conductivity")->getValue(&thermal_conductivity);
								output << std::setw(8) << thermal_conductivity << std::endl;
							}
							matflag = true;
						}else{

							output << "!MATERIAL, NAME=" << dIter->first << ", ITEM=1" << std::endl;
							output << "!ITEM=1, SUBITEM=2" << std::endl;
							output << " 0.0, 0.0" << std::endl;
							matflag = true;
						}
					}
					break;
				}
			}
			++dIter;
		}
		if( matflag == false ){

			output << "!MATERIAL, NAME=MAT, ITEM=1" << std::endl;
			output << "!ITEM=1, SUBITEM=2" << std::endl;
			output << " 0.0, 0.0" << std::endl;
			matflag = true;
		}















		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			std::string matName;
			std::multimap< std::string, kmb::DataBindings*>::const_iterator sIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator sEnd = dataBindings.end();
			while( sIter != sEnd ){
				if( sIter->second->getSpecType() == "Material" && sIter->second->hasId( bodyId ) ){
					matName = sIter->first;
					break;
				}
				++sIter;
			}
			if( matName.size() != 0 ){
				const kmb::PhysicalValue* v = mesh->getPhysicalValueAtId( "SECTION", bodyId, "SECTION" );
				if( v && v->getType() == kmb::PhysicalValue::Hash ){
					const kmb::HashValue* h = reinterpret_cast< const kmb::HashValue* >(v);
					kmb::PhysicalValue* u = NULL;
					u = h->getValue("TYPE");
					if( u != NULL && u->getType() == kmb::PhysicalValue::String ){
						output << "!SECTION, TYPE=" << reinterpret_cast<kmb::TextValue*>(u)->getValue() << ", ";
						if( strlen( mesh->getBodyName(bodyId) ) == 0 ){
							output << "EGRP=Body_" << bodyId << ", MATERIAL=" << matName << std::endl;
						}else{
							output << "EGRP=" << mesh->getBodyName(bodyId) << ", MATERIAL=" << matName << std::endl;
						}
						u = h->getValue("THICKNESS");
						if( u != NULL && u->getType() == kmb::PhysicalValue::Scalar ){
							x=0.0;
							u->getValue(&x);
							output << x;
							u = h->getValue("INTEGPOINTS");
							if( u != NULL && u->getType() == kmb::PhysicalValue::Scalar ){
								u->getValue(&x);
								output << ", " << x;
							}
							u = h->getValue("GAPCON");
							if( u != NULL && u->getType() == kmb::PhysicalValue::Scalar ){
								u->getValue(&x);
								output << ", " << x;
							}
							u = h->getValue("GAPRAD1");
							if( u != NULL && u->getType() == kmb::PhysicalValue::Scalar ){
								u->getValue(&x);
								output << ", " << x;
							}
							u = h->getValue("GAPRAD2");
							if( u != NULL && u->getType() == kmb::PhysicalValue::Scalar ){
								u->getValue(&x);
								output << ", " << x;
							}
							output << std::endl;
						}
					}
				}else{
					output << "!SECTION, TYPE=SOLID, ";
					if( strlen( mesh->getBodyName(bodyId) ) == 0 ){
						output << "EGRP=Body_" << bodyId << ", MATERIAL=" << matName << std::endl;
					}else{
						output << "EGRP=" << mesh->getBodyName(bodyId) << ", MATERIAL=" << matName << std::endl;
					}
				}
			}else if( dummySectionFlag && mesh->getDimension(bodyId) == 3 ){

				output << "!SECTION, TYPE=SOLID, ";
				if( strlen( mesh->getBodyName(bodyId) ) == 0 ){
					output << "EGRP=Body_" << bodyId << ", MATERIAL=MAT1" << std::endl;
				}else{
					output << "EGRP=" << mesh->getBodyName(bodyId) << ", MATERIAL=MAT1" << std::endl;
				}
			}
		}

		{
			std::multimap< std::string, kmb::DataBindings*>::const_iterator egIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator egEnd = dataBindings.end();
			while( egIter != egEnd ){
				if( dataFlag )
				{
					if( egIter->second->getBindingMode() == kmb::DataBindings::ElementGroup ){
						output << "!EGROUP, EGRP=" << egIter->first << std::endl;
						kmb::DataBindings::const_iterator eIter = egIter->second->begin();
						while( !eIter.isFinished() ){
							output << eIter.getId()+offsetElementId << std::endl;
							++eIter;
						}
					}
				}
				++egIter;
			}
		}

		{
			std::multimap< std::string, kmb::DataBindings*>::const_iterator ngIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator ngEnd = dataBindings.end();
			while( ngIter != ngEnd ){
				if( dataFlag ||
					ngIter->second->getSpecType() == "BOUNDARY" ||
					ngIter->second->getSpecType() == "CLOAD" ||
					ngIter->second->getSpecType() == "FIXTEMP" ||
					ngIter->second->getSpecType() == "CFLUX" ||
					ngIter->second->getSpecType() == "TEMPERATURE" )
				{
					if( ngIter->second->getBindingMode() == kmb::DataBindings::NodeGroup ){
						output << "!NGROUP, NGRP=" << ngIter->first << std::endl;
						kmb::DataBindings::const_iterator ndIter = ngIter->second->begin();
						while( !ndIter.isFinished() ){
							output << ndIter.getId()+offsetNodeId << std::endl;
							++ndIter;
						}
					}
				}
				++ngIter;
			}
		}


		{
			int tetFmap[] = {3,4,2,1};
			int wedFmap[] = {1,2,3,4,5};
			int hexFmap[] = {1,2,3,4,5,6};
			std::multimap< std::string, kmb::DataBindings*>::const_iterator sgIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator sgEnd = dataBindings.end();
			while( sgIter != sgEnd ){
				if( dataFlag ||
					sgIter->second->getSpecType() == "DLOAD" ||



					sgIter->second->getSpecType() == "COUPLING" )
				{
					if( sgIter->second->getBindingMode() == kmb::DataBindings::FaceGroup ){
						output << "!SGROUP, SGRP=" << sgIter->first << std::endl;
						kmb::DataBindings::const_iterator fIter = sgIter->second->begin();
						while( !fIter.isFinished() ){
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
				++sgIter;
			}
		}

		{
			std::multimap< std::string, kmb::DataBindings*>::const_iterator aIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator aEnd = dataBindings.end();
			while( aIter != aEnd ){
				if( aIter->second->getSpecType() == "AMPLITUDE" )
				{
					if( aIter->second->getValueType() == kmb::PhysicalValue::Hash ){
						const kmb::HashValue* h = reinterpret_cast< const kmb::HashValue* >( aIter->second );
						output << "!AMPLITUDE, NAME=" << aIter->first;
						const kmb::PhysicalValue* d = NULL;
						d = h->getValue("definition");
						if( d != NULL && d->getType() == kmb::PhysicalValue::String ){
							output << ", DEFINITION=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						d = h->getValue("time");
						if( d != NULL && d->getType() == kmb::PhysicalValue::String ){
							output << ", TIME=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						d = h->getValue("value");
						if( d != NULL && d->getType() == kmb::PhysicalValue::String ){
							output << ", VALUE=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						d = h->getValue("input");
						if( d != NULL && d->getType() == kmb::PhysicalValue::String ){
							output << ", INPUT=" << reinterpret_cast< const kmb::TextValue* >(d)->getValue();
						}
						output << std::endl;
						d = h->getValue("table");
						if( d != NULL && d->getType() == kmb::PhysicalValue::Array ){
							const kmb::ArrayValue* a = reinterpret_cast< const kmb::ArrayValue* >(d);
							size_t num = a->getSize();
							for(unsigned int i=0;i<num/2;++i){
								output << reinterpret_cast< const kmb::ScalarValue* >(a->getValue(2*i))->getValue() << ", ";
								output << reinterpret_cast< const kmb::ScalarValue* >(a->getValue(2*i))->getValue() << std::endl;
							}
						}
					}
				}
				++aIter;
			}
		}

		{
			int tetFmap[] = {3,4,2,1};
			int wedFmap[] = {1,2,3,4,5};
			int hexFmap[] = {1,2,3,4,5,6};
			kmb::ArrayValue* ary = NULL;
			std::multimap< std::string, kmb::DataBindings*>::const_iterator cIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator cEnd = dataBindings.end();
			while( cIter != cEnd ){
				if( dataFlag ||
					cIter->second->getSpecType() == "CONTPAIR" )
				{
					if( cIter->second->getValueType() == kmb::PhysicalValue::Array &&
						(ary = reinterpret_cast<kmb::ArrayValue*>(cIter->second->getPhysicalValue())) != NULL )
					{
						std::string masterName = reinterpret_cast<kmb::TextValue*>( ary->getValue(0) )->getValue();
						std::string slaveName = reinterpret_cast<kmb::TextValue*>( ary->getValue(1) )->getValue();
						kmb::DataBindings* masterData = dataBindings.find( masterName )->second;
						kmb::DataBindings* slaveData = dataBindings.find( slaveName )->second;
						if( masterData && masterData->getBindingMode() == kmb::DataBindings::FaceGroup &&
							slaveData && slaveData->getBindingMode() == kmb::DataBindings::FaceGroup )
						{
							output << "!SGROUP, SGRP=" << masterName << "_CONT_MASTER" << std::endl;
							kmb::DataBindings::const_iterator fIter = masterData->begin();
							while( !fIter.isFinished() ){
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
							output << "!NGROUP, NGRP=" << slaveName << "_CONT_SLAVE" << std::endl;
							std::set<kmb::nodeIdType> nodeSet;
							mesh->getNodeSetFromDataBindings(nodeSet,slaveName.c_str());
							std::set<kmb::nodeIdType>::iterator ngIter = nodeSet.begin();
							kmb::nodeIdType nodeId;
							while( ngIter != nodeSet.end() ){
								nodeId = *ngIter;
								output << nodeId+offsetNodeId << std::endl;
								++ngIter;
							}
							output << "!CONTACT PAIR, NAME=" << cIter->first << std::endl;
							output << slaveName << "_CONT_SLAVE, " << masterName << "_CONT_MASTER" << std::endl;
						}
					}
				}
				++cIter;
			}
		}
		output.close();
	}
	return 0;
}

int
kmb::HecmwIO::saveToResFile(const char* filename,kmb::MeshData* mesh) const
{
	if( mesh == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out|std::ios_base::binary );
	if( output.fail() ){
		return -1;
	}
	output << "HECMW_BINARY_RESULT";
	char buf[64];
	long n;
	sprintf(buf,"%2lu",sizeof(long));
	output.write(buf,2);
	output << this->resHeader << '\0';
	n = static_cast<long>( mesh->getNodeCount() );
	output.write( reinterpret_cast<char*>(&n), sizeof(long) );
	n = static_cast<long>( mesh->getElementCountOfDim(3) );
	output.write( reinterpret_cast<char*>(&n), sizeof(long) );

	long nodeValueCount = 0;
	long elementValueCount = 0;
	std::vector< std::string > nodeValues;
	std::vector< std::string > elementValues;
	std::multimap< std::string, kmb::DataBindings* >::const_iterator dIter = mesh->getDataBindingsMap().begin();
	std::multimap< std::string, kmb::DataBindings* >::const_iterator endIter = mesh->getDataBindingsMap().end();
	while( dIter != endIter ){
		kmb::DataBindings* data = dIter->second;
		if( data ){
			switch( data->getBindingMode() )
			{
			case kmb::DataBindings::NodeVariable:
				nodeValues.push_back( dIter->first );
				++nodeValueCount;
				break;
			case kmb::DataBindings::ElementVariable:
				elementValues.push_back( dIter->first );
				++elementValueCount;
				break;
			default:
				break;
			}
		}
		++dIter;
	}
	int dim = 0;
	mesh->clearTargetData();
	output.write( reinterpret_cast<char*>(&nodeValueCount), sizeof(long) );
	output.write( reinterpret_cast<char*>(&elementValueCount), sizeof(long) );

	{
		std::vector< std::string >::iterator vIter = nodeValues.begin();
		std::vector< std::string >::iterator endIter = nodeValues.end();
		while( vIter != endIter ){
			const kmb::DataBindings* data = mesh->getDataBindingsPtr( vIter->c_str() );
			if( data ){
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					n = 1;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				case kmb::PhysicalValue::Vector2:
					n = 2;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				case kmb::PhysicalValue::Vector3:
					n = 3;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				case kmb::PhysicalValue::Tensor6:
					n = 6;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				default:
					break;
				}
			}
			++vIter;
		}
	}
	{
		std::vector< std::string >::iterator vIter = nodeValues.begin();
		std::vector< std::string >::iterator endIter = nodeValues.end();
		while( vIter != endIter ){
			const kmb::DataBindings* data = mesh->getDataBindingsPtr( vIter->c_str() );
			if( data ){
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
				case kmb::PhysicalValue::Vector2:
				case kmb::PhysicalValue::Vector3:
				case kmb::PhysicalValue::Tensor6:
					output << *vIter << '\0';
					mesh->appendTargetData( vIter->c_str() );
					break;
				default:
					break;
				}
			}
			++vIter;
		}
	}
	if( nodeValueCount > 0 ){
		kmb::nodeIdType nodeId = kmb::nullNodeId;
		double* v = new double[dim];
		kmb::Point3DContainer::const_iterator pIter = mesh->getNodes()->begin();
		while( !pIter.isFinished() ){
			nodeId = pIter.getId();
			mesh->getMultiPhysicalValues( nodeId, v );
			n = static_cast<long>(nodeId + this->offsetNodeId);
			output.write( reinterpret_cast<char*>(&n), sizeof(long) );
			output.write( reinterpret_cast<char*>(v), sizeof(double)*dim );
			++pIter;
		}
		delete[] v;
	}
	dim = 0;
	mesh->clearTargetData();

	{
		std::vector< std::string >::iterator vIter = elementValues.begin();
		std::vector< std::string >::iterator endIter = elementValues.end();
		while( vIter != endIter ){
			const kmb::DataBindings* data = mesh->getDataBindingsPtr( vIter->c_str() );
			if( data ){
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					n = 1;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				case kmb::PhysicalValue::Vector2:
					n = 2;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				case kmb::PhysicalValue::Vector3:
					n = 3;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				case kmb::PhysicalValue::Tensor6:
					n = 6;
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					dim += n;
					break;
				default:
					break;
				}
			}
			++vIter;
		}
	}
	{
		std::vector< std::string >::iterator vIter = elementValues.begin();
		std::vector< std::string >::iterator endIter = elementValues.end();
		while( vIter != endIter ){
			const kmb::DataBindings* data = mesh->getDataBindingsPtr( vIter->c_str() );
			if( data ){
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
				case kmb::PhysicalValue::Vector2:
				case kmb::PhysicalValue::Vector3:
				case kmb::PhysicalValue::Tensor6:
					output << *vIter << '\0';
					mesh->appendTargetData( vIter->c_str() );
					break;
				default:
					break;
				}
			}
			++vIter;
		}
	}
	if( elementValueCount > 0 ){
		kmb::elementIdType elementId = kmb::Element::nullElementId;
		double* v = new double[dim];
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
			if( mesh->getDimension(bodyId) == 3 ){
				const kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( !eIter.isFinished() ){
					elementId = eIter.getId();
					mesh->getMultiPhysicalValues( elementId, v );
					n = static_cast<long>(elementId + this->offsetElementId);
					output.write( reinterpret_cast<char*>(&n), sizeof(long) );
					output.write( reinterpret_cast<char*>(v), sizeof(double)*dim );
					++eIter;
				}
			}
		}
		delete[] v;
	}
	output.close();
	return 0;
}


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
		output << "!HEADER, VER=4" << std::endl;
		output << " HECMW_MSH File generated by REVOCAP" << std::endl;
		output << "!NODE, PARTNAME=" << partName << ", NUM=" << mesh->getNodeCount() << std::endl;
		double x,y,z;
		{
			kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
			kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
			output.setf( std::ios::fixed );
			while( nIter != nIterEnd ){
				if( nIter.getXYZ(x,y,z) ){
					output << std::setw(9) << nIter.getId()+offsetNodeId << ", "
						<< std::scientific
						<< std::setprecision(8)
						<< x << ", " << y << ", " << z << std::endl;
				}
				++nIter;
			}
		}

		kmb::bodyIdType bodyCount = mesh->getBodyCount();



		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			size_t elemNum = body->getCountByType( kmb::TETRAHEDRON );
			if( body && body->getDimension() == 3 && elemNum > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::TETRAHEDRON ) <<
					", PARTNAME=" << partName << ", NUM=" << elemNum;
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::TETRAHEDRON ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			size_t elemNum = body->getCountByType( kmb::TETRAHEDRON2 );
			if( body && body->getDimension() == 3 && elemNum > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::TETRAHEDRON2 ) <<
					", PARTNAME=" << partName << ", NUM=" << elemNum;
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::TETRAHEDRON2 ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << "," <<
							eIter[8]+offsetNodeId << "," <<
							eIter[9]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			size_t elemNum = body->getCountByType( kmb::HEXAHEDRON );
			if( body && body->getDimension() == 3 && elemNum > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::HEXAHEDRON ) <<
					", PARTNAME=" << partName << ", NUM=" << elemNum;
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::HEXAHEDRON ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			size_t elemNum = body->getCountByType( kmb::HEXAHEDRON2 );
			if( body && body->getDimension() == 3 && elemNum > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::HEXAHEDRON2 ) <<
					", PARTNAME=" << partName << ", NUM=" << elemNum;
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::HEXAHEDRON2 ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << "," <<
							eIter[8]+offsetNodeId << "," <<
							eIter[9]+offsetNodeId << "," <<
							eIter[10]+offsetNodeId << "," <<
							eIter[11]+offsetNodeId << "," <<
							eIter[12]+offsetNodeId << "," <<
							eIter[13]+offsetNodeId << "," <<
							eIter[14]+offsetNodeId << "," <<
							eIter[15]+offsetNodeId << "," <<
							eIter[16]+offsetNodeId << "," <<
							eIter[17]+offsetNodeId << "," <<
							eIter[18]+offsetNodeId << "," <<
							eIter[19]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			size_t elemNum = body->getCountByType( kmb::WEDGE );
			if( body && body->getDimension() == 3 && elemNum > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::WEDGE ) <<
					", PARTNAME=" << partName << ", NUM=" << elemNum;
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::WEDGE ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			size_t elemNum = body->getCountByType( kmb::WEDGE2 );
			if( body && body->getDimension() == 3 && elemNum > 0 ){
				output << "!ELEMENT, TYPE=" << kmb::HecmwIO::getHECType( kmb::WEDGE2 ) <<
					", PARTNAME=" << partName << ", NUM=" << elemNum;
				if( strcmp( body->getBodyName(), "ALL" ) == 0 ){
					output << std::endl;
				}else if( strlen( body->getBodyName() ) == 0 ){
					output << ", EGRP=Body_" << bodyId << std::endl;
				}else{
					output << ", EGRP=" << body->getBodyName() << std::endl;
				}
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					if( eIter.getType() == kmb::WEDGE2 ){
						output << eIter.getId()+offsetElementId << "," <<
							eIter[0]+offsetNodeId << "," <<
							eIter[1]+offsetNodeId << "," <<
							eIter[2]+offsetNodeId << "," <<
							eIter[3]+offsetNodeId << "," <<
							eIter[4]+offsetNodeId << "," <<
							eIter[5]+offsetNodeId << "," <<
							eIter[6]+offsetNodeId << "," <<
							eIter[7]+offsetNodeId << "," <<
							eIter[8]+offsetNodeId << "," <<
							eIter[9]+offsetNodeId << "," <<
							eIter[10]+offsetNodeId << "," <<
							eIter[11]+offsetNodeId << "," <<
							eIter[12]+offsetNodeId << "," <<
							eIter[13]+offsetNodeId << "," <<
							eIter[14]+offsetNodeId << std::endl;
					}
					++eIter;
				}
			}
		}

		std::multimap< std::string, kmb::DataBindings*> dataBindings = mesh->getDataBindingsMap();






















		{
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
			while( dIter != dEnd ){
				if( dataFlag )
				{
					if( dIter->second->getBindingMode() == kmb::DataBindings::ElementGroup ){
						output << "!EGROUP, EGRP=" << dIter->first <<
							", PARTNAME=" << partName << ", NUM=" << dIter->second->getIdCount() << std::endl;
						kmb::DataBindings::const_iterator nIter = dIter->second->begin();
						while( !nIter.isFinished() ){
							output << nIter.getId()+offsetElementId << std::endl;
							++nIter;
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
				if( dataFlag ||
					dIter->second->getSpecType() == "BOUNDARY" ||
					dIter->second->getSpecType() == "CLOAD" ||
					dIter->second->getSpecType() == "FIXTEMP" ||
					dIter->second->getSpecType() == "CFLUX" ||
					dIter->second->getSpecType() == "TEMPERATURE" )
				{
					if( dIter->second->getBindingMode() == kmb::DataBindings::NodeGroup ){
						output << "!NGROUP, NGRP=" << dIter->first <<
							", PARTNAME=" << partName << ", NUM=" << dIter->second->getIdCount() << std::endl;
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
				if( dataFlag ||
					dIter->second->getSpecType() == "DLOAD" ||



					dIter->second->getSpecType() == "COUPLING" )
				{
					if( dIter->second->getBindingMode() == kmb::DataBindings::FaceGroup ){
						output << "!SGROUP, SGRP=" << dIter->first <<
							", PARTNAME=" << partName << ", NUM=" << dIter->second->getIdCount() << std::endl;
						kmb::DataBindings::const_iterator fIter = dIter->second->begin();
						while( !fIter.isFinished() ){
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
			int tetFmap[] = {3,4,2,1};
			int wedFmap[] = {1,2,3,4,5};
			int hexFmap[] = {1,2,3,4,5,6};
			kmb::ArrayValue* ary = NULL;
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
			while( dIter != dEnd ){
				if( dataFlag ||
					dIter->second->getSpecType() == "CONTPAIR" )
				{
					if( dIter->second->getValueType() == kmb::PhysicalValue::Array &&
						(ary = reinterpret_cast<kmb::ArrayValue*>(dIter->second->getPhysicalValue())) != NULL )
					{
						std::string masterName = reinterpret_cast<kmb::TextValue*>( ary->getValue(0) )->getValue();
						std::string slaveName = reinterpret_cast<kmb::TextValue*>( ary->getValue(1) )->getValue();
						kmb::DataBindings* masterData = dataBindings.find( masterName )->second;
						kmb::DataBindings* slaveData = dataBindings.find( slaveName )->second;
						if( masterData && masterData->getBindingMode() == kmb::DataBindings::FaceGroup &&
							slaveData && slaveData->getBindingMode() == kmb::DataBindings::FaceGroup )
						{
							output << "!SGROUP, SGRP=" << masterName << "_CONT_MASTER" <<
								", PARTNAME=" << partName << ", NUM=" << masterData->getIdCount() << std::endl;
							kmb::DataBindings::const_iterator fIter = masterData->begin();
							while( !fIter.isFinished() ){
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
							std::set<kmb::nodeIdType> nodeSet;
							mesh->getNodeSetFromDataBindings(nodeSet,slaveName.c_str());
							output << "!NGROUP, NGRP=" << slaveName << "_CONT_SLAVE" <<
								", PARTNAME=" << partName << ", NUM=" << nodeSet.size() << std::endl;
							std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
							kmb::nodeIdType nodeId;
							while( nIter != nodeSet.end() ){
								nodeId = *nIter;
								output << nodeId+offsetNodeId << std::endl;
								++nIter;
							}
							output << "!CONTACT_PAIR, NAME=" << dIter->first << ", NUM=1" << std::endl;
							output << slaveName << "_CONT_SLAVE, " << masterName << "_CONT_MASTER, " <<
								partName << ", " << partName << std::endl;
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
			kmb::ArrayValue* ary = NULL;
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
			std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
			while( dIter != dEnd ){
				if( dataFlag ||
					dIter->second->getSpecType() == "ASSEMBLY" )
				{
					if( dIter->second->getValueType() == kmb::PhysicalValue::Array &&
						(ary = reinterpret_cast<kmb::ArrayValue*>(dIter->second->getPhysicalValue())) != NULL )
					{
						std::string masterName = reinterpret_cast<kmb::TextValue*>( ary->getValue(0) )->getValue();
						std::string slaveName = reinterpret_cast<kmb::TextValue*>( ary->getValue(1) )->getValue();
						kmb::DataBindings* masterData = dataBindings.find( masterName )->second;
						kmb::DataBindings* slaveData = dataBindings.find( slaveName )->second;
						if( masterData && masterData->getBindingMode() == kmb::DataBindings::FaceGroup &&
							slaveData && slaveData->getBindingMode() == kmb::DataBindings::FaceGroup )
						{
							output << "!SGROUP, SGRP=" << masterName << "_ASSEM_MASTER" <<
								", PARTNAME=" << partName << ", NUM=" << masterData->getIdCount() << std::endl;
							{
								kmb::DataBindings::const_iterator fIter = masterData->begin();
								while( !fIter.isFinished() ){
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
							output << "!SGROUP, SGRP=" << slaveName << "_ASSEM_SLAVE" <<
								", PARTNAME=" << partName << ", NUM=" << slaveData->getIdCount() << std::endl;
							{
								kmb::DataBindings::const_iterator fIter = slaveData->begin();
								while( !fIter.isFinished() ){
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
							output << "!ASSEMBLY_PAIR, NAME=" << dIter->first << ", NUM=1" << std::endl;





							output << slaveName << "_ASSEM_SLAVE, " << masterName << "_ASSEM_MASTER, " <<
								partName << ", " << partName << std::endl;
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

int
kmb::HecmwIO::appendSgroupToFile(const char* filename,const kmb::MeshData* mesh,const char* sgroup,const char* stype) const
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}
	const kmb::DataBindings* data = mesh->getDataBindingsPtr(sgroup,stype);
	if( data == NULL || data->getBindingMode() != kmb::DataBindings::FaceGroup ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::app );
	if( output.fail() ){
		return -1;
	}
	int tetFmap[] = {3,4,2,1};
	int wedFmap[] = {1,2,3,4,5};
	int hexFmap[] = {1,2,3,4,5,6};

	output << "!SGROUP, SGRP=" << sgroup << std::endl;
	kmb::DataBindings::const_iterator fIter = data->begin();
	while( !fIter.isFinished() ){
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
	output.close();
	return 0;
}

int
kmb::HecmwIO::appendEquationToFile(const char* filename,const kmb::MeshData* mesh,const char* master,const char* slave,double thresh,double outThresh) const
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::app );
	if( output.fail() ){
		return -1;
	}
	output << std::showpoint << std::setprecision(8);

	output << "!EQUATION" << std::endl;
	int equationCount = 0;


	const kmb::DataBindings* mGroup = mesh->getDataBindingsPtr(master);
	kmb::FaceBucket fbucket;
	fbucket.setContainer(mesh,mGroup);
	fbucket.setAutoBucketSize();
	fbucket.appendAll();
	std::set< kmb::nodeIdType > nodeset;
	mesh->getNodeSetFromDataBindings( nodeset, slave );
	std::set< kmb::nodeIdType >::iterator nIter = nodeset.begin();
	std::set< kmb::nodeIdType >::iterator endIter = nodeset.end();
	kmb::Point3D pt;
	kmb::nodeIdType nodeId;
	kmb::Face f;
	double dist;
	kmb::ElementContainer::const_iterator element;
	kmb::ElementEvaluator eev( mesh->getNodes() );
	double coeff[20];
	while( nIter != endIter ){
		nodeId = *nIter;
		mesh->getNode( nodeId, pt );
		fbucket.getNearest( pt.x(), pt.y(), pt.z(), dist, f );
		element = mesh->findElement( f.getElementId(), mGroup->getTargetBodyId() );

/*
		if( eev.getWeightElement( element, pt.x(), pt.y(), pt.z(), coeff ) >= outThresh ){
			int len = element.getNodeCount();
			int count = 1;
			double sum = 0.0;
			for(int j=0;j<len;++j){

				if( fabs( coeff[j] ) > thresh ){
					++count;
					sum += coeff[j];
				}else{
					coeff[j] = 0.0;
				}
			}
			if( len+1 != count ){
				for(int j=0;j<len;++j){
					coeff[j] /= sum;
				}
			}


			for(int i=0;i<3;++i){
				int term = 0;
				output << count << std::endl;
				output << nodeId + offsetNodeId << ", " << i+1 << ", " << 1.0;
				++term;
				for(int j=0;j<len;++j){
					if( coeff[j] != 0.0 ){
						if( term > 0 ){
							output << ", ";
						}
						output << element[j] + offsetNodeId << ", " << i+1 << ", " << -coeff[j];
						++term;
					}
					if( term == 4 ){
						output << std::endl;
						term = 0;
					}
				}
				if( term != 0 ){
					output << std::endl;
				}
			}
		}
*/
		int findex = f.getLocalFaceId();
		if( eev.getWeightElementFace( element, findex, pt.x(), pt.y(), pt.z(), coeff ) >= outThresh ){
			int len = element.getBoundaryNodeCount(findex);
			int count = 1;
			double sum = 0.0;
			for(int j=0;j<len;++j){

				if( fabs( coeff[j] ) > thresh ){
					++count;
					sum += coeff[j];
				}else{
					coeff[j] = 0.0;
				}
			}
			if( len+1 != count ){
				for(int j=0;j<len;++j){
					coeff[j] /= sum;
				}
			}



			for(int i=0;i<3;++i){
				int term = 0;
				output << count << std::endl;
				output << nodeId + offsetNodeId << ", " << i+1 << ", " << 1.0;
				++term;
				for(int j=0;j<len;++j){
					if( coeff[j] != 0.0 ){
						if( term > 0 ){
							output << ", ";
						}
						output << element.getBoundaryCellId(findex,j) + offsetNodeId << ", " << i+1 << ", " << -coeff[j];
						++term;
					}
					if( term == 4 ){
						output << std::endl;
						term = 0;
					}
				}
				if( term != 0 ){
					output << std::endl;
				}
			}
			equationCount += 3;
		}
		++nIter;
	}
	output.close();
	return equationCount;
}

int
kmb::HecmwIO::appendFooterToFile(const char* filename) const
{
	std::ofstream output( filename, std::ios_base::app );
	if( output.fail() ){
		return -1;
	}
	output << "!END" << std::endl;
	output.close();
	return 0;
}

int kmb::HecmwIO::tetRmap[] = {3,2,0,1};
int kmb::HecmwIO::wedRmap[] = {0,1,2,3,4};
int kmb::HecmwIO::hexRmap[] = {0,1,2,3,4,5};
int kmb::HecmwIO::pyrRmap[] = {4,0,1,2,3};

int
kmb::HecmwIO::correctLocalFaceId( kmb::MeshData* mesh, const char* faceName, const char* stype )
{
	if( mesh == NULL ){
		return 0;
	}
	kmb::DataBindings* data = mesh->getDataBindingsPtr( faceName, stype );
	if( data == NULL || data->getBindingMode() != kmb::DataBindings::FaceVariable ){
		return 0;
	}
	kmb::DataBindings* data2 = kmb::DataBindings::createDataBindings(
		data->getBindingMode(),
		data->getValueType(),
		stype, data->getTargetBodyId() );

	int correctCount(0);
	kmb::Face f;
	double* v = new double[data->getDimension()];
	kmb::DataBindings::iterator dIter = data->begin();
	while( !dIter.isFinished() ){
		if( dIter.getFace(f) ){
			kmb::ElementContainer::iterator eIter = mesh->findElement( f.getElementId() );
			if( !eIter.isFinished() ){
				switch( eIter.getType() ){
				case kmb::TETRAHEDRON:
				case kmb::TETRAHEDRON2:
					f.setId( eIter.getId(), tetRmap[ f.getLocalFaceId()-1 ] );
					break;
				case kmb::HEXAHEDRON:
				case kmb::HEXAHEDRON2:
					f.setId( eIter.getId(), hexRmap[ f.getLocalFaceId()-1 ] );
					break;
				case kmb::WEDGE:
				case kmb::WEDGE2:
					f.setId( eIter.getId(), wedRmap[ f.getLocalFaceId()-1 ] );
					break;
				case kmb::PYRAMID:
				case kmb::PYRAMID2:
					f.setId( eIter.getId(), pyrRmap[ f.getLocalFaceId()-1 ] );
					break;
				default:
					break;
				}
			}
			dIter.getValue(v);
			data2->setPhysicalValue( f, v );
			++correctCount;
		}
		++dIter;
	}
	mesh->replaceData( data, data2, faceName, stype );
	delete data;
	delete[] v;
	return correctCount;
}

