/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapNeutralIO                                        #
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
#include "RevocapIO/kmbRevocapNeutralIO.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbVector2WithIntBindings.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <ctime>

kmb::RevocapNeutralIO::RevocapNeutralIO(void)
{
}

kmb::RevocapNeutralIO::~RevocapNeutralIO(void)
{
}

int
kmb::RevocapNeutralIO::loadFromRNFFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		std::string line;
		std::string tag;
		while( !std::getline( input, line ).eof() ){
			if( line.size() == 0 || line[0] == '#' ){
				continue;
			}
			std::istringstream tokenizer(line);
			tokenizer >> tag;
			if( tag == "node:" ){
				readNode(input,mesh);
			}
			else if( tag == "element:" ){
				readElement(input,mesh);
			}
			else if( tag == "data:" ){
				readData(input,mesh);
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::RevocapNeutralIO::readNode(std::ifstream &input,kmb::MeshData* mesh)
{
	std::string line;
	std::string tag;
	int dim=3;
	long size=0L;
	while( !std::getline( input, line ).eof() ){
		if( line.size() == 0 || line[0] == '#' ){
			continue;
		}
		std::istringstream tokenizer(line);
		tokenizer >> tag;
		if( tag == "dim:" ){
			tokenizer >> dim;
		}
		else if( tag == "size:" ){
			tokenizer >> size;
		}
		else if( tag == "coordinate:" ){
			break;
		}
	}
	mesh->beginNode( static_cast<size_t>(size) );
	double x, y, z;
	kmb::nodeIdType id;
	char c;
	long nodeCounter=0;
	while( !std::getline( input, line ).eof() ){
		if( line.size() == 0 || line[0] == '#' ){
			continue;
		}
		std::istringstream tokenizer(line);
		tokenizer >> c >> c;
		tokenizer >> id >> c >> x >> c >> y >> c >> z >> c;
		mesh->addNode( x, y, z );
		++nodeCounter;
		if( nodeCounter >= size ){
			break;
		}
	}
	mesh->endNode();
	return 0;
}



int
kmb::RevocapNeutralIO::readElement(std::ifstream &input,kmb::MeshData* mesh)
{
	std::string line;
	std::string tag;
	std::string name;
	char c;
	long size=0L;
	kmb::elementIdType id;
	kmb::nodeIdType nodes[20];
	while( !input.eof() ){
		std::streampos pos = input.tellg();
		std::getline( input, line );
		if( line.size() == 0 || line[0] == '#' ){
			continue;
		}
		if( line[0] != ' ' ){
			input.seekg( pos );
			return 0;
		}
		std::istringstream tokenizer(line);
		tokenizer >> c >> tag;
		if( tag == "size:" ){
			tokenizer >> size;
			while( !std::getline( input, line ).eof() ){
				if( line.size() == 0 || line[0] == '#' ){
					continue;
				}
				if( line[0] != ' ' ){
					return -1;
				}
				tokenizer.str(line);
				tokenizer.clear();
				tokenizer >> tag;
				if( tag == "connectivity:" ){
					long elementCounter = 0;
					kmb::bodyIdType bodyId = mesh->beginElement( static_cast<size_t>(size) );
					while( elementCounter < size && !std::getline( input, line ).eof() ){
						if( line.size() == 0 || line[0] == '#' ){
							continue;
						}
						if( line[0] != ' ' ){
							return -1;
						}
						tokenizer.str(line);
						tokenizer.clear();
						tokenizer >> c >> c;
						tokenizer >> id >> c >> tag;
						if( tag[ tag.size()-1 ] == ',' ){
							tag.erase( tag.size()-1, 1 );
						}
						kmb::elementType etype = kmb::ElementBase::getType( tag );
						int len = kmb::Element::getNodeCount( etype );
						if( len <= 0 ){
							continue;
						}
						for(int i=0;i<len;++i){
							tokenizer >> nodes[i] >> c;
						}
						mesh->addElementWithId( etype, nodes, id );
						++elementCounter;
					}
					mesh->endElement();
					if( name.length() > 0 ){
						mesh->setBodyName( bodyId, name.c_str() );
					}
					break;
				}
			}
		}else if( tag == "name:" ){
			tokenizer >> name;
		}
	}
	return 0;
}





int
kmb::RevocapNeutralIO::readData(std::ifstream &input,kmb::MeshData* mesh)
{
	std::string line;
	std::string tag;
	std::string name;
	std::string mode;
	std::string vtype;
	std::string stype;
	char c;
	long size=0L;
	kmb::elementIdType elementId;
	kmb::idType localId;
	kmb::nodeIdType nodeId;
	kmb::bodyIdType bodyId;
	kmb::PhysicalValue* value = NULL;
	kmb::DataBindings* data = NULL;
	while( input.good() ){
		std::streampos pos = input.tellg();
		std::getline( input, line );
		if( line.size() == 0 || line[0] == '#' ){
			continue;
		}
		if( line[0] != ' ' ){
			input.seekg( pos );
			return 0;
		}

		if( line[2] == '-' ){
			name = "";
			mode = "";
			vtype = "";
			stype = "";
			size = 0L;
			bodyId = kmb::Body::nullBodyId;
			data = NULL;
			if( value != NULL ){
				delete value;
				value = NULL;
			}
			std::istringstream tokenizer(line);
			tokenizer >> c >> tag;
			if( tag == "name:" ){
				tokenizer >> name;
				if( name.at(0) == '"' && name.at(name.size()-1) == '"' ){
					name = name.substr(1,name.size()-2);
				}
			}
			while( !std::getline( input, line ).eof() ){
				if( line.size() == 0 || line[0] == '#' ){
					continue;
				}
				if( line[0] != ' ' ){
					return -1;
				}
				if( line[2] == '-' ){
					break;
				}
				pos = input.tellg();
				tokenizer.str(line);
				tokenizer.clear();
				tokenizer >> tag;
				if( tag == "mode:" ){
					tokenizer >> mode;
				}else if( tag == "vtype:" ){
					tokenizer >> vtype;
				}else if( tag == "stype:" ){
					tokenizer >> stype;
				}else if( tag == "size:" ){
					tokenizer >> size;
				}else if( tag == "targetBodyId:" ){
					tokenizer >> bodyId;
				}else if( tag == "value:" ){
					if( data == NULL ){
						data = mesh->createDataBindings(
							name.c_str(),
							kmb::DataBindings::string2bindingMode( mode ),
							kmb::PhysicalValue::string2valueType( vtype ),
							stype.c_str(),
							bodyId
						);
					}
					if( data &&
						data->getBindingMode() == kmb::DataBindings::NodeVariable &&
						data->getValueType() == kmb::PhysicalValue::Vector2withInt ){
						long dataCounter = 0;
						double u=0.0, v=0.0;
						long s = 0L;
						while( dataCounter < size && !std::getline( input, line ).eof() ){
							if( line.size() == 0 || line[0] == '#' ){
								continue;
							}
							if( line[0] != ' ' ){
								return -1;
							}
							if( line[2] == '-' ){
								break;
							}
							pos = input.tellg();
							tokenizer.str(line);
							tokenizer.clear();
							tokenizer >> c >> c >> nodeId >> c >> c >> u >> c >> v >> c >> s >> c >> c;

							reinterpret_cast< kmb::Vector2WithIntBindings<kmb::nodeIdType>* >(data)->setValue(nodeId,u,v,s);
							++dataCounter;
						}
					}else{

						switch( kmb::PhysicalValue::string2valueType( vtype ) )
						{
						case kmb::PhysicalValue::Scalar:
							{
								double v = 0.0;
								tokenizer >> v;
								value = new ScalarValue(v);
								break;
							}
						case kmb::PhysicalValue::Vector2:
							{
								double v[] = {0.0, 0.0};
								tokenizer >> c >> v[0] >> c >> v[1] >> c;
								value = new Vector2Value(v);
								break;
							}
						case kmb::PhysicalValue::Vector3:
							{
								double v[] = {0.0, 0.0, 0.0};
								tokenizer >> c >> v[0] >> c >> v[1] >> c >> v[2] >> c;
								value = new Vector3Value(v);
								break;
							}
						case kmb::PhysicalValue::Tensor6:
							{
								double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
								tokenizer >> c >> v[0] >> c >> v[1] >> c >> v[2] >> c >> v[3] >> c >> v[4] >> c >> v[5] >> c;
								value = new Tensor6Value(v);
								break;
							}
						case kmb::PhysicalValue::Point3Vector3:
							{
								double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
								tokenizer >> c >> c >> v[0] >> c >> v[1] >> c >> v[2] >> c >> c

									>> c >> v[3] >> c >> v[4] >> c >> v[5] >> c >> c;

								value = new Point3Vector3Value( &v[0], &v[3] );
								break;
							}
						case kmb::PhysicalValue::Integer:
							{
								int n;
								tokenizer >> n;
								value = new IntegerValue(n);
								break;
							}
						default:
							break;
						}
					}
				}else if( tag == "id:" ){
					if( data == NULL ){
						data = mesh->createDataBindings(
							name.c_str(),
							kmb::DataBindings::string2bindingMode( mode ),
							kmb::PhysicalValue::string2valueType( vtype ),
							stype.c_str(),
							bodyId
						);
					}
					long dataCounter = 0;
					if( data && data->getBindingMode() == kmb::DataBindings::FaceGroup ){
						while( dataCounter < size && !std::getline( input, line ).eof() ){
							if( line.size() == 0 || line[0] == '#' ){
								continue;
							}
							if( line[0] != ' ' ){
								return -1;
							}
							if( line[2] == '-' ){
								break;
							}
							pos = input.tellg();
							tokenizer.str(line);
							tokenizer.clear();
							tokenizer >> c >> c >> elementId >> c >> localId >> c;

							data->addId( kmb::Face(elementId,localId) );
							++dataCounter;
						}
					}else if( data && data->getBindingMode() == kmb::DataBindings::NodeGroup ){
						while( dataCounter < size && !std::getline( input, line ).eof() ){
							if( line.size() == 0 || line[0] == '#' ){
								continue;
							}
							if( line[0] != ' ' ){
								return -1;
							}
							if( line[2] == '-' ){
								break;
							}
							pos = input.tellg();
							tokenizer.str(line);
							tokenizer.clear();
							tokenizer >> c >> nodeId;;

							data->addId( nodeId );
							++dataCounter;
						}
					}else if( data && data->getBindingMode() == kmb::DataBindings::ElementGroup ){
						while( dataCounter < size && !std::getline( input, line ).eof() ){
							if( line.size() == 0 || line[0] == '#' ){
								continue;
							}
							if( line[0] != ' ' ){
								return -1;
							}
							if( line[2] == '-' ){
								break;
							}
							pos = input.tellg();
							tokenizer.str(line);
							tokenizer.clear();
							tokenizer >> c >> elementId;;

							data->addId( elementId );
							++dataCounter;
						}
					}else{

						while( !std::getline( input, line ).eof() ){
							if( line.size() == 0 || line[0] == '#' ){
								continue;
							}
							if( line[0] != ' ' ){
								return -1;
							}
							if( line[2] == '-' ){
								break;
							}
							pos = input.tellg();
						}
					}
				}
			}
			if( data != NULL && value != NULL && value->getType() == data->getValueType() ){
				data->setPhysicalValue( value );

				value = NULL;
			}
			input.seekg( pos );
		}
	}
	if( value != NULL ){
		delete value;
	}
	return 0;
}

int
kmb::RevocapNeutralIO::readShape(std::ifstream &input,kmb::MeshData* mesh)
{
	return 0;
}

int
kmb::RevocapNeutralIO::writeHeader(std::ofstream &output)
{
	time_t current;
	struct tm  *local = NULL;
	time(&current);
#if defined _MSC_VER && _MSC_VER >= 1400
	struct tm stime;
	local = &stime;
	localtime_s(local,&current);
#else
	local = localtime(&current);
#endif
	output << "# REVOCAP Neutral Yaml Mesh Format ver. 1.5.0" << std::endl;
	output << "# Generated by REVOCAP_PrePost at " << local->tm_year+1900 << "-" <<
		std::setw(2) << std::setfill('0') << local->tm_mon+1 << "-" <<
		std::setw(2) << std::setfill('0') << local->tm_mday << " " <<
		std::setw(2) << std::setfill('0') << local->tm_hour << ":" <<
		std::setw(2) << std::setfill('0') << local->tm_min << ":" <<
		std::setw(2) << std::setfill('0') << local->tm_sec << std::endl;
	output << "---" << std::endl;
	return 0;
}

int
kmb::RevocapNeutralIO::saveToRNFFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		writeHeader(output);
		output << "node:" << std::endl;
		output << "  size: " << mesh->getNodeCount() << std::endl;
		output << "  coordinate: " << std::endl;
		double x,y,z;
		kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
		kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
		while( nIter != nIterEnd ){
			if( nIter.getXYZ(x,y,z) ){
				output << "  - [" << nIter.getId() << ", " << std::showpoint << x << ", " << y << ", " << z <<  "]" << std::endl;
			}
			++nIter;
		}
		int bodyCount = static_cast<int>(mesh->getBodyCount());
		output << "bodyCount: " << bodyCount << std::endl;
		output << "element:" << std::endl;
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
			if( body )
			{
				output << "  - size: " << body->getCount() << std::endl;
				const char* bodyName = body->getBodyName();
				if( bodyName && strlen(bodyName) > 0 ){
					output << "    name: \"" << body->getBodyName() << "\"" << std::endl;
				}
				output << "    connectivity:" << std::endl;
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					output << "      - [" << eIter.getId() << ", ";
					output << eIter.getTypeString();
					int num = eIter.getNodeCount();
					for(int i=0;i<num;++i){
						output << ", " << eIter.getCellId(i);
					}
					output << "]" << std::endl;
					++eIter;
				}
			}
		}
		output << "data:" << std::endl;
		const std::multimap< std::string, kmb::DataBindings* > mapper = mesh->getDataBindingsMap();
		std::multimap< std::string, kmb::DataBindings* >::const_iterator dIter = mapper.begin();
		while( dIter != mapper.end() ){
			output << "  - name: " << "\"" << dIter->first << "\"" << std::endl;
			output << "    mode: " << kmb::DataBindings::bindingMode2string(dIter->second->getBindingMode()) << std::endl;
			output << "    vtype: " << kmb::PhysicalValue::valueType2string(dIter->second->getValueType()) << std::endl;
			std::string stype = dIter->second->getSpecType();
			if( stype.size() > 0 ){
				output << "    stype: " << stype << std::endl;
			}
			kmb::bodyIdType bodyId = dIter->second->getTargetBodyId();
			if( bodyId >= 0 ){
				output << "    targetBodyId: " << bodyId << std::endl;
			}
			output << "    size: " << dIter->second->getIdCount() << std::endl;
			switch( dIter->second->getBindingMode() )
			{
			case kmb::DataBindings::NodeGroup:
			case kmb::DataBindings::ElementGroup:
			case kmb::DataBindings::BodyGroup:
			case kmb::DataBindings::FaceGroup:
				{
					switch( dIter->second->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						{
							double v = 0.0;
							dIter->second->getPhysicalValue()->getValue(&v);
							output << "    value: " << std::showpoint << v << std::endl;
						}
						break;
					case kmb::PhysicalValue::Vector2:
						{
							double v[] = {0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [" << std::showpoint << v[0] << ", " << v[1] << "]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Vector3:
						{
							double v[] = {0.0, 0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Tensor6:
						{
							double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Point3Vector3:
						{
							double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [[" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << "], [" << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Vector2withInt:
						{
							double v[] = {0.0, 0.0};
							long k = 0L;
							dIter->second->getPhysicalValue()->getValue(v);
							dIter->second->getPhysicalValue()->getValue(&k);
							output << "    value: [" << std::showpoint << v[0] << ", " << v[1] << ", " << k << "]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Integer:
						{
							output << "    value: " << reinterpret_cast<kmb::IntegerValue*>(dIter->second->getPhysicalValue())->getValue() << std::endl;
						}
						break;
					case kmb::PhysicalValue::Array:
						{
						}
						break;
					case kmb::PhysicalValue::Hash:
						{
						}
						break;
					case kmb::PhysicalValue::Boolean:
						{
						}
						break;
					default:
						break;
					}
					if( dIter->second->getBindingMode() == kmb::DataBindings::FaceGroup ){
						if( dIter->second->getIdCount() > 0 ){
							output << "    id:" << std::endl;
							kmb::DataBindings::iterator gIter = dIter->second->begin();
							while( !gIter.isFinished() ){
								kmb::Face f;
								gIter.getFace(f);
								output << "    - [" << f.getElementId() << ", " << f.getLocalFaceId() << "]" << std::endl;
								++gIter;
							}
						}
					}else{
						if( dIter->second->getIdCount() > 0 ){
							output << "    id:" << std::endl;
							kmb::DataBindings::iterator gIter = dIter->second->begin();
							while( !gIter.isFinished() ){
								output << "    - " << gIter.getId() << std::endl;
								++gIter;
							}
						}
					}
				}
				break;
			case kmb::DataBindings::NodeVariable:
			case kmb::DataBindings::ElementVariable:
			case kmb::DataBindings::BodyVariable:
				{
					if( dIter->second->getIdCount() > 0 ){
						output << "    value:" << std::endl;
						switch( dIter->second->getValueType() )
						{
						case kmb::PhysicalValue::Scalar:
							{
								double v = 0.0;
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getValue(&v);
									output << "    - [" << gIter.getId() << ", " << std::showpoint << v << "]" << std::endl;
									++gIter;
								}
							}
							break;
						case kmb::PhysicalValue::Vector2:
							{
								double v[] = {0.0, 0.0};
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getValue(v);
									output << "    - [" << gIter.getId() << ", [" << std::showpoint << v[0] << ", " << v[1] << "]]" << std::endl;
									++gIter;
								}
							}
							break;
						case kmb::PhysicalValue::Vector3:
							{
								double v[] = {0.0, 0.0, 0.0};
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getValue(v);
									output << "    - [" << gIter.getId() << ", [" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << "]]" << std::endl;
									++gIter;
								}
							}
							break;
						case kmb::PhysicalValue::Tensor6:
							{
								double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getValue(v);
									output << "    - [" << gIter.getId() << ", [" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
									++gIter;
								}
							}
							break;
						case kmb::PhysicalValue::Vector2withInt:
							{
								double v[] = {0.0, 0.0};
								long k = 0L;
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getValue(v);
									gIter.getValue(&k);
									output << "    - [" << gIter.getId() << ", [" << std::showpoint << v[0] << ", " << v[1] << ", " << k << "]]" << std::endl;
									++gIter;
								}
							}
							break;
						default:
							break;
						}
					}
				}
				break;
			case kmb::DataBindings::FaceVariable:
				{
					if( dIter->second->getIdCount() > 0 ){
						output << "    id:" << std::endl;
						kmb::Face f;
						switch( dIter->second->getValueType() )
						{
						case kmb::PhysicalValue::Scalar:
							{
								double v = 0.0;
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getFace(f);
									gIter.getValue(&v);
									output << "    - [[" << f.getElementId() << ", " << f.getLocalFaceId() << "], ";
									output << std::showpoint << v << "]" << std::endl;
									++gIter;
								}
							}
							break;
						case kmb::PhysicalValue::Vector2:
							{
								double v[] = {0.0, 0.0};
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getFace(f);
									gIter.getValue(v);
									output << "    - [[" << f.getElementId() << ", " << f.getLocalFaceId() << "], ";
									output << std::showpoint << v[0] << ", " << v[1] << "]]" << std::endl;
									++gIter;
								}
							}
							break;
						case kmb::PhysicalValue::Vector3:
							{
								double v[] = {0.0, 0.0, 0.0};
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getFace(f);
									gIter.getValue(v);
									output << "    - [[" << f.getElementId() << ", " << f.getLocalFaceId() << "], ";
									output << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << "]]" << std::endl;
									++gIter;
								}
							}
							break;
						case kmb::PhysicalValue::Tensor6:
							{
								double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
								kmb::DataBindings::iterator gIter = dIter->second->begin();
								while( !gIter.isFinished() ){
									gIter.getFace(f);
									gIter.getValue(v);
									output << "    - [[" << f.getElementId() << ", " << f.getLocalFaceId() << "], ";
									output << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
									++gIter;
								}
							}
							break;
						default:
							break;
						}
					}
				}
				break;
			case kmb::DataBindings::Global:
				{
					switch( dIter->second->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						{
							double v = 0.0;
							dIter->second->getPhysicalValue()->getValue(&v);
							output << "    value: " << std::showpoint << v << std::endl;
						}
						break;
					case kmb::PhysicalValue::Vector2:
						{
							double v[] = {0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [" << std::showpoint << v[0] << ", " << v[1] << "]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Vector3:
						{
							double v[] = {0.0, 0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Tensor6:
						{
							double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Point3Vector3:
						{
							double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
							dIter->second->getPhysicalValue()->getValue(v);
							output << "    value: [[" << std::showpoint << v[0] << ", " << v[1] << ", " << v[2] << "], [" << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
						}
						break;
					case kmb::PhysicalValue::Integer:
						{
							output << "    value: " << reinterpret_cast<kmb::IntegerValue*>(dIter->second->getPhysicalValue())->getValue() << std::endl;
						}
						break;
					case kmb::PhysicalValue::Array:
						{
						}
						break;
					case kmb::PhysicalValue::Hash:
						{
						}
						break;
					case kmb::PhysicalValue::Boolean:
						{
						}
						break;
					default:
						break;
					}
				}
				break;
			default:
				break;
			}
			++dIter;
		}
		output.close();
	}
	return 0;
}

int
kmb::RevocapNeutralIO::saveHeader(const char* filename)
{
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	writeHeader(output);
	output.close();
	return 0;
}

int
kmb::RevocapNeutralIO::appendHeader(const char* filename)
{
	std::ofstream output( filename, std::ios_base::app );
	if( output.fail() ){
		return -1;
	}
	writeHeader(output);
	output.close();
	return 0;
}

int
kmb::RevocapNeutralIO::appendDataToRNFFile(const char* filename,kmb::MeshData* mesh,const char* name,const char* stype)
{
	kmb::DataBindings* data = NULL;
	if( mesh == NULL || (data=mesh->getDataBindingsPtr(name,stype))==NULL ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::app );
		if( output.fail() ){
			return -1;
		}
		output << "data:" << std::endl;
		output << "  - name: " << "\"" << name << "\"" << std::endl;
		output << "    mode: " << kmb::DataBindings::bindingMode2string(data->getBindingMode()) << std::endl;
		output << "    vtype: " << kmb::PhysicalValue::valueType2string(data->getValueType()) << std::endl;
		std::string spec = data->getSpecType();
		if( spec.size() > 0 ){
			output << "    stype: " << spec << std::endl;
		}
		kmb::bodyIdType bodyId = data->getTargetBodyId();
		if( bodyId >= 0 ){
			output << "    targetBodyId: " << bodyId << std::endl;
		}
		output << "    size: " << data->getIdCount() << std::endl;
		switch( data->getBindingMode() )
		{
		case kmb::DataBindings::NodeGroup:
		case kmb::DataBindings::ElementGroup:
		case kmb::DataBindings::BodyGroup:
		case kmb::DataBindings::FaceGroup:
			{
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					{
						double v = 0.0;
						data->getPhysicalValue()->getValue(&v);
						output << "    value: " << std::scientific << std::setprecision(8) << v << std::endl;
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						double v[] = {0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << "]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						double v[] = {0.0, 0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Point3Vector3:
					{
						double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [[" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << "], [" << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Vector2withInt:
					{
						double v[] = {0.0, 0.0};
						long k = 0L;
						data->getPhysicalValue()->getValue(v);
						data->getPhysicalValue()->getValue(&k);
						output << "    value: [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << k << "]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Integer:
					{
						output << "    value: " << reinterpret_cast<kmb::IntegerValue*>(data->getPhysicalValue())->getValue() << std::endl;
					}
					break;
				case kmb::PhysicalValue::Array:
					{
					}
					break;
				case kmb::PhysicalValue::Hash:
					{
					}
					break;
				case kmb::PhysicalValue::Boolean:
					{
					}
					break;
				default:
					break;
				}
				if( data->getBindingMode() == kmb::DataBindings::FaceGroup ){
					if( data->getIdCount() > 0 ){
						output << "    id:" << std::endl;
						kmb::DataBindings::iterator gIter = data->begin();
						while( !gIter.isFinished() ){
							kmb::Face f;
							gIter.getFace(f);
							output << "    - [" << f.getElementId() << ", " << f.getLocalFaceId() << "]" << std::endl;
							++gIter;
						}
					}
				}else{
					if( data->getIdCount() > 0 ){
						output << "    id:" << std::endl;
						kmb::DataBindings::iterator gIter = data->begin();
						while( !gIter.isFinished() ){
							output << "    - " << gIter.getId() << std::endl;
							++gIter;
						}
					}
				}
			}
			break;
		case kmb::DataBindings::NodeVariable:
		case kmb::DataBindings::ElementVariable:
		case kmb::DataBindings::BodyVariable:
			{
				if( data->getIdCount() > 0 ){
					output << "    value:" << std::endl;
					switch( data->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						{
							double v = 0.0;
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getValue(&v);
								output << "    - [" << gIter.getId() << ", " << std::scientific << std::setprecision(8) << v << "]" << std::endl;
								++gIter;
							}
						}
						break;
					case kmb::PhysicalValue::Vector2:
						{
							double v[] = {0.0, 0.0};
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getValue(v);
								output << "    - [" << gIter.getId() << ", [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << "]]" << std::endl;
								++gIter;
							}
						}
						break;
					case kmb::PhysicalValue::Vector3:
						{
							double v[] = {0.0, 0.0, 0.0};
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getValue(v);
								output << "    - [" << gIter.getId() << ", [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << "]]" << std::endl;
								++gIter;
							}
						}
						break;
					case kmb::PhysicalValue::Tensor6:
						{
							double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getValue(v);
								output << "    - [" << gIter.getId() << ", [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
								++gIter;
							}
						}
						break;
					case kmb::PhysicalValue::Vector2withInt:
						{
							double v[] = {0.0, 0.0};
							long k = 0L;
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getValue(v);
								gIter.getValue(&k);
								output << "    - [" << gIter.getId() << ", [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << k << "]]" << std::endl;
								++gIter;
							}
						}
						break;
					default:
						break;
					}
				}
			}
			break;
		case kmb::DataBindings::FaceVariable:
			{
				if( data->getIdCount() > 0 ){
					output << "    id:" << std::endl;
					kmb::Face f;
					switch( data->getValueType() )
					{
					case kmb::PhysicalValue::Scalar:
						{
							double v = 0.0;
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getFace(f);
								gIter.getValue(&v);
								output << "    - [[" << f.getElementId() << ", " << f.getElementId() << "], ";
								output << std::scientific << std::setprecision(8) << v << "]" << std::endl;
								++gIter;
							}
						}
						break;
					case kmb::PhysicalValue::Vector2:
						{
							double v[] = {0.0, 0.0};
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getFace(f);
								gIter.getValue(v);
								output << "    - [[" << f.getElementId() << ", " << f.getElementId() << "], ";
								output << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << "]]" << std::endl;
								++gIter;
							}
						}
						break;
					case kmb::PhysicalValue::Vector3:
						{
							double v[] = {0.0, 0.0, 0.0};
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getFace(f);
								gIter.getValue(v);
								output << "    - [[" << f.getElementId() << ", " << f.getElementId() << "], ";
								output << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << "]]" << std::endl;
								++gIter;
							}
						}
						break;
					case kmb::PhysicalValue::Tensor6:
						{
							double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
							kmb::DataBindings::iterator gIter = data->begin();
							while( !gIter.isFinished() ){
								gIter.getFace(f);
								gIter.getValue(v);
								output << "    - [[" << f.getElementId() << ", " << f.getElementId() << "], ";
								output << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
								++gIter;
							}
						}
						break;
					default:
						break;
					}
				}
			}
			break;
		case kmb::DataBindings::Global:
			{
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					{
						double v = 0.0;
						data->getPhysicalValue()->getValue(&v);
						output << "    value: " << std::scientific << std::setprecision(8) << v << std::endl;
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						double v[] = {0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << "]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						double v[] = {0.0, 0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", " << v[4] << ", " << v[5] << "]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Point3Vector3:
					{
						double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
						data->getPhysicalValue()->getValue(v);
						output << "    value: [[" << std::scientific << std::setprecision(8) << v[0] << ", " << v[1] << ", " << v[2] << "], [" << v[3] << ", " << v[4] << ", " << v[5] << "]]" << std::endl;
					}
					break;
				case kmb::PhysicalValue::Integer:
					{
						output << "    value: " << reinterpret_cast<kmb::IntegerValue*>(data->getPhysicalValue())->getValue() << std::endl;
					}
					break;
				case kmb::PhysicalValue::Array:
					{
					}
					break;
				case kmb::PhysicalValue::Hash:
					{
					}
					break;
				case kmb::PhysicalValue::Boolean:
					{
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
		output.close();
	}
	return 0;
}
