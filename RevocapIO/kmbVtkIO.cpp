/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : VtkIO                                                   #
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
#include "RevocapIO/kmbVtkIO.h"

#include "MeshDB/kmbMeshData.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

kmb::VtkIO::VtkIO(void){
}

kmb::VtkIO::~VtkIO(void)
{
}

int
kmb::VtkIO::loadFromFile(const char* filename,kmb::MeshData* mesh)
{
	return loadFromFile_V4(filename,mesh);
}

int
kmb::VtkIO::saveToFile(const char* filename,const kmb::MeshData* mesh)
{
	return saveToLegacyFile_V3(filename,mesh);
}

int
kmb::VtkIO::loadFromFile_V4(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}

		std::string line;
		std::string dummy;
		std::istringstream tokenizer;
		kmb::nodeIdType nodeTable[20];


		int* cellTypes = NULL;

		while( !input.eof() ){
			if( line.find("CELL_TYPES") == 0 ){
				size_t cellCount = 0;
				tokenizer.str(line);
				tokenizer.clear();
				tokenizer >> dummy >> cellCount;
				cellTypes = new int[cellCount];
				for(size_t i=0;i<cellCount;++i){
					std::getline( input, line );
					tokenizer.str(line);
					tokenizer.clear();
					tokenizer >> cellTypes[i];
				}
				std::getline( input, line );
			}else{
				std::getline( input, line );
			}
		}


		input.clear();
		input.close();
		input.open( filename, std::ios_base::in );

		while( !input.eof() ){
			if( line.find("DATASET") == 0 ){
				if( line.find("UNSTRUCTURED_GRID") == std::string::npos ){
					input.close();
					return -2;
				}
				std::getline( input, line );
			}else if( line.find("POINTS") == 0 ){
				unsigned int nodeCount = 0;
				double x=0.0,y=0.0,z=0.0;
				tokenizer.str(line);
				tokenizer.clear();
				tokenizer >> dummy >> nodeCount;
				mesh->beginNode( nodeCount );
				for(unsigned int i = 0;i<nodeCount;++i){
					input >> x >> y >> z;
					mesh->addNode(x,y,z);
				}
				mesh->endNode();
				std::getline( input, line );
			}else if( line.find("CELLS") == 0 ){
				size_t cellCount = 0;
				size_t tableSize = 0;
				tokenizer.str(line);
				tokenizer.clear();
				tokenizer >> dummy >> cellCount >> tableSize;
				mesh->beginElement(cellCount);
				for(size_t i=0;i<cellCount;++i){
					std::getline( input, line );
					tokenizer.str(line);
					tokenizer.clear();
					switch(cellTypes[i]){
					case 5:
						tokenizer >> tableSize;
						tokenizer >> nodeTable[0];
						tokenizer >> nodeTable[1];
						tokenizer >> nodeTable[2];
						mesh->addElement( kmb::TRIANGLE, nodeTable );
						break;
					case 9:
						tokenizer >> tableSize;
						tokenizer >> nodeTable[0];
						tokenizer >> nodeTable[1];
						tokenizer >> nodeTable[2];
						tokenizer >> nodeTable[3];
						mesh->addElement( kmb::QUAD, nodeTable );
						break;
					case 10:
						tokenizer >> tableSize;
						tokenizer >> nodeTable[0];
						tokenizer >> nodeTable[1];
						tokenizer >> nodeTable[2];
						tokenizer >> nodeTable[3];
						mesh->addElement( kmb::TETRAHEDRON, nodeTable );
						break;
					case 12:
						tokenizer >> tableSize;
						tokenizer >> nodeTable[0];
						tokenizer >> nodeTable[1];
						tokenizer >> nodeTable[2];
						tokenizer >> nodeTable[3];
						tokenizer >> nodeTable[4];
						tokenizer >> nodeTable[5];
						tokenizer >> nodeTable[6];
						tokenizer >> nodeTable[7];
						mesh->addElement( kmb::HEXAHEDRON, nodeTable );
						break;
					case 13:
						tokenizer >> tableSize;
						tokenizer >> nodeTable[0];
						tokenizer >> nodeTable[2];
						tokenizer >> nodeTable[1];
						tokenizer >> nodeTable[3];
						tokenizer >> nodeTable[5];
						tokenizer >> nodeTable[4];
						mesh->addElement( kmb::WEDGE, nodeTable );
						break;
					case 14:
						tokenizer >> tableSize;
						tokenizer >> nodeTable[1];
						tokenizer >> nodeTable[2];
						tokenizer >> nodeTable[3];
						tokenizer >> nodeTable[4];
						tokenizer >> nodeTable[0];
						mesh->addElement( kmb::PYRAMID, nodeTable );
						break;
					default:
						break;
					}
				}
				mesh->endElement();
				std::getline( input, line );
			}else{
				std::getline( input, line );
			}
		}
		delete[] cellTypes;
		input.close();
		return 0;
	}
}

int
kmb::VtkIO::saveToLegacyFile_V3(const char* filename,const kmb::MeshData* mesh,int dim,const char* header)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	output << "# vtk DataFile Version 3.0" << std::endl;
	if( header ){
		output << header;
	}
	output << " Generated by REVOCAP" << std::endl;
	output << "ASCII" << std::endl;
	output << "DATASET UNSTRUCTURED_GRID" << std::endl;
	output << "POINTS " << mesh->getNodeCount() << " float" << std::endl;
	kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
	kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
	while( nIter != nIterEnd ){
		output << std::setprecision(8) << nIter.x() << " " << nIter.y() << " " << nIter.z() << std::endl;
		++nIter;
	}
	size_t dataSize=0;
	size_t elementCount=0;
	kmb::bodyIdType bodyCount = static_cast<kmb::bodyIdType>(mesh->getBodyCount());
	kmb::elementType etypes[] = {
		kmb::TRIANGLE,
		kmb::TRIANGLE2,
		kmb::QUAD,
		kmb::QUAD2,
		kmb::TETRAHEDRON,
		kmb::TETRAHEDRON2,
		kmb::HEXAHEDRON,
		kmb::HEXAHEDRON2,
		kmb::WEDGE,
		kmb::WEDGE2,
		kmb::PYRAMID,
		kmb::PYRAMID2,
	};

	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
		for(int i=0;i<12;++i){
			if(dim==-1){
				size_t eCount = mesh->getBodyPtr(bodyId)->getCountByType(etypes[i]);
				size_t nCount = kmb::Element::getNodeCount(etypes[i]);
				elementCount += eCount;
				dataSize += eCount * (1+nCount);
			}else{
				if( kmb::Element::getDimension( etypes[i] ) == dim ){
					size_t eCount = mesh->getBodyPtr(bodyId)->getCountByType(etypes[i]);
					size_t nCount = kmb::Element::getNodeCount(etypes[i]);
					if( etypes[i] == kmb::WEDGE2 ){
						nCount = kmb::Element::getNodeCount(kmb::WEDGE);
					}else if( etypes[i] == kmb::PYRAMID2 ){
						nCount = kmb::Element::getNodeCount(kmb::PYRAMID);
					}
					elementCount += eCount;
					dataSize += eCount * (1+nCount);
				}
			}
		}
	}
	output << "CELLS " << elementCount << " " << dataSize << std::endl;
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
		const kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
		kmb::ElementContainer::const_iterator eIter = body->begin();
		kmb::ElementContainer::const_iterator endIter = body->end();
		while( eIter != endIter ){
			kmb::elementType etype = eIter.getType();
			if( dim == 2 || dim == -1 ){
				if( etype == kmb::TRIANGLE ){
					output << "3 " << eIter[0] << " " << eIter[1] << " " << eIter[2] << std::endl;
				}else if( etype == kmb::TRIANGLE2 ){
					output << "6 " << eIter[0] << " " << eIter[1] << " " << eIter[2] <<
						" " << eIter[5] << " " << eIter[3] << " " << eIter[4] << std::endl;
				}else if( etype == kmb::QUAD ){
					output << "4 " << eIter[0] << " " << eIter[1] << " " << eIter[2] << " " << eIter[3] << std::endl;
				}else if( etype == kmb::QUAD2 ){
					output << "8 " << eIter[0] << " " << eIter[1] << " " << eIter[2] << " " << eIter[3] <<
						" " << eIter[4] << " " << eIter[5] << " " << eIter[6] << " " << eIter[7] << std::endl;
				}
			}
			if( dim == 3 || dim == -1 ){
				if( etype == kmb::TETRAHEDRON ){
					output << "4 " << eIter[0] << " " << eIter[1] << " " << eIter[2] << " " << eIter[3] << std::endl;
				}else if( etype == kmb::TETRAHEDRON2 ){
					output << "10 " << eIter[0] << " " << eIter[1] << " " << eIter[2] << " " << eIter[3] <<
						" " << eIter[6] << " " << eIter[4] << " " << eIter[5] <<
						" " << eIter[7] << " " << eIter[8] << " " << eIter[9] << std::endl;
				}else if( etype == kmb::HEXAHEDRON ){
					output << "8 " << eIter[0] << " " << eIter[1] << " " << eIter[2] << " " << eIter[3] <<
						" " << eIter[4] << " " << eIter[5] << " " << eIter[6] << " " << eIter[7] << std::endl;
				}else if( etype == kmb::HEXAHEDRON2 ){
					output << "20 " << eIter[0] << " " << eIter[1] << " " << eIter[2] << " " << eIter[3] <<
						" " << eIter[4] << " " << eIter[5] << " " << eIter[6] << " " << eIter[7] <<
						" " << eIter[8] << " " << eIter[9] << " " << eIter[10] << " " << eIter[11] <<
						" " << eIter[12] << " " << eIter[13] << " " << eIter[14] << " " << eIter[15] <<
						" " << eIter[16] << " " << eIter[17] << " " << eIter[18] << " " << eIter[19] << std::endl;
				}else if( etype == kmb::WEDGE || etype == kmb::WEDGE2 ){
					output << "6 " << eIter[0] << " " << eIter[2] << " " << eIter[1] <<
						" " << eIter[3] << " " << eIter[5] << " " << eIter[4] << std::endl;
				}else if( etype == kmb::PYRAMID || etype == kmb::PYRAMID2 ){
					output << "5 " << eIter[1] << " " << eIter[2] << " " << eIter[3] << " " << eIter[4] <<
						" " << eIter[0] << std::endl;
				}
			}
			++eIter;
		}
	}
	output << "CELL_TYPES " << elementCount << std::endl;
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
		const kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
		kmb::ElementContainer::const_iterator eIter = body->begin();
		kmb::ElementContainer::const_iterator endIter = body->end();
		while( eIter != endIter ){
			kmb::elementType etype = eIter.getType();
			if( dim == 2 || dim == -1 ){
				if( etype == kmb::TRIANGLE ){
					output << "5" << std::endl;
				}else if( etype == kmb::TRIANGLE2 ){
					output << "22" << std::endl;
				}else if( etype == kmb::QUAD ){
					output << "8" << std::endl;
				}else if( etype == kmb::QUAD2 ){
					output << "23" << std::endl;
				}
			}
			if( dim == 3 || dim == -1 ){
				if( etype == kmb::TETRAHEDRON ){
					output << "10" << std::endl;
				}else if( etype == kmb::TETRAHEDRON2 ){
					output << "24" << std::endl;
				}else if( etype == kmb::HEXAHEDRON ){
					output << "12" << std::endl;
				}else if( etype == kmb::HEXAHEDRON2 ){
					output << "25" << std::endl;
				}else if( etype == kmb::WEDGE || etype == kmb::WEDGE2 ){
					output << "13" << std::endl;
				}else if( etype == kmb::PYRAMID || etype == kmb::PYRAMID2 ){
					output << "14" << std::endl;
				}
			}
			++eIter;
		}
	}
	int nodeDataCount = 0;
	int elementDataCount = 0;

	std::multimap< std::string, kmb::DataBindings*> dataBindings = mesh->getDataBindingsMap();
	std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = dataBindings.begin();
	std::multimap< std::string, kmb::DataBindings*>::const_iterator dEnd = dataBindings.end();
	while( dIter != dEnd ){
		kmb::DataBindings* data = dIter->second;
		if( data ){
			switch( data->getBindingMode() )
			{
			case kmb::DataBindings::NodeVariable:
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
				case kmb::PhysicalValue::Vector3:
				case kmb::PhysicalValue::Tensor6:
				case kmb::PhysicalValue::Integer:
					++nodeDataCount;
					break;
				default:
					break;
				}
				break;
			case kmb::DataBindings::ElementVariable:
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
				case kmb::PhysicalValue::Vector3:
				case kmb::PhysicalValue::Tensor6:
				case kmb::PhysicalValue::Integer:
					++elementDataCount;
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
	output << "CELL_DATA " << elementCount << std::endl;
	output << "FIELD FieldData " << elementDataCount << std::endl;
	dIter = dataBindings.begin();
	while( dIter != dEnd ){
		kmb::DataBindings* data = dIter->second;
		if( data ){
			if( data->getBindingMode() == kmb::DataBindings::ElementVariable ){
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					{
						output << dIter->first << " 1 " << elementCount << " float" <<std::endl;
						double v;
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(&v);
							if( dim == -1 || mesh->findElement(vIter.getId()).getDimension() == dim ){
								output << std::setprecision(8) << v << std::endl;
							}
							++vIter;
						}
						break;
					}
				case kmb::PhysicalValue::Vector3:
					{
						output << dIter->first << " 3 " << elementCount << " float" <<std::endl;
						double v[3];
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(v);
							if( dim == -1 || mesh->findElement(vIter.getId()).getDimension() == dim ){
								output << std::setprecision(8) << v[0] << " " << v[1] << " " << v[2] << std::endl;
							}
							++vIter;
						}
						break;
					}
				case kmb::PhysicalValue::Tensor6:
					{
						output << dIter->first << " 6 " << elementCount << " float" <<std::endl;
						double v[6];
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(v);
							if( dim == -1 || mesh->findElement(vIter.getId()).getDimension() == dim ){
								output << std::setprecision(8) << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " " << v[4] << " " << v[5] << std::endl;
							}
							++vIter;
						}
						break;
					}
				case kmb::PhysicalValue::Integer:
					{
						output << dIter->first << " 1 " << elementCount << " int" <<std::endl;
						long v;
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(&v);
							if( dim == -1 || mesh->findElement(vIter.getId()).getDimension() == dim ){
								output << v << std::endl;
							}
							++vIter;
						}
						break;
					}
				default:
					break;
				}
			}
		}
		++dIter;
	}
	output << "POINT_DATA " << mesh->getNodeCount() << std::endl;
	output << "FIELD FieldData " << nodeDataCount << std::endl;
	dIter = dataBindings.begin();
	while( dIter != dEnd ){
		kmb::DataBindings* data = dIter->second;
		if( data ){
			if( data->getBindingMode() == kmb::DataBindings::NodeVariable ){
				switch( data->getValueType() )
				{
				case kmb::PhysicalValue::Scalar:
					{
						output << dIter->first << " 1 " << mesh->getNodeCount() << " float" <<std::endl;
						double v;
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(&v);
							output << std::setprecision(8) << v << std::endl;
							++vIter;
						}
						break;
					}
				case kmb::PhysicalValue::Vector3:
					{
						output << dIter->first << " 3 " << mesh->getNodeCount() << " float" <<std::endl;
						double v[3];
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(v);
							output << std::setprecision(8) << v[0] << " " << v[1] << " " << v[2] << std::endl;
							++vIter;
						}
						break;
					}
				case kmb::PhysicalValue::Tensor6:
					{
						output << dIter->first << " 6 " << mesh->getNodeCount() << " float" <<std::endl;
						double v[6];
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(v);
							output << std::setprecision(8) << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " " << v[4] << " " << v[5] << std::endl;
							++vIter;
						}
						break;
					}
				case kmb::PhysicalValue::Integer:
					{
						output << dIter->first << " 1 " << mesh->getNodeCount() << " int" <<std::endl;
						double v;
						kmb::DataBindings::const_iterator vIter = data->begin();
						kmb::DataBindings::const_iterator vEnd = data->end();
						while( vIter != vEnd ){
							vIter.getValue(&v);
							output << v << std::endl;
							++vIter;
						}
						break;
					}
				default:
					break;
				}
			}
		}
		++dIter;
	}
	output.close();
	return 0;
}

int
kmb::VtkIO::saveToSerialXMLFile(const char* filename,const kmb::MeshData* mesh)
{
	return 0;
}

int
kmb::VtkIO::saveToParallelXMLFile(const char* filename,const kmb::MeshData* mesh)
{
	return 0;
}

int kmb::VtkIO::saveToStructuredPointsFile_V3(const char* filename,int nx, int ny, int nz,const kmb::DataBindings* data)
{
	if( data == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	output << "# vtk DataFile Version 3.0" << std::endl;
	output << " Generated by REVOCAP" << std::endl;
	output << "ASCII" << std::endl;
	output << "DATASET STRUCTURED_POINTS" << std::endl;
	output << "DIMENSIONS " << nx << " " << ny << " " << nz << std::endl;
	output << "ORIGIN 0.0 0.0 0.0" << std::endl;
	output << "SPACING 1.0 1.0 1.0" << std::endl;
	output << std::endl;
	output << "POINT_DATA " << nx*ny*nz << std::endl;
	output << "SCALARS data float 1" << std::endl;
	output << "LOOKUP_TABLE default" << std::endl;
	if( data->getBindingMode() == kmb::DataBindings::NodeVariable && data->getValueType() == kmb::PhysicalValue::Scalar ){
		double v;
		kmb::DataBindings::const_iterator vIter = data->begin();
		kmb::DataBindings::const_iterator vEnd = data->end();
		while( vIter != vEnd ){
			vIter.getValue(&v);
			output << std::setprecision(8) << v << std::endl;
			++vIter;
		}
	}
	output.close();
	return 0;
}
