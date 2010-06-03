/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : TetMeshMIO                                              #
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
#include "RevocapIO/kmbTetMeshMIO.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainerArray.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

int
kmb::TetMeshMIO::loadFromFile(const char* filename,MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		kmb::nodeIdType ary[20] =
		{
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId,
			kmb::nullNodeId
		};
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		size_t i;
		std::vector<kmb::nodeIdType> vertices;
		std::string line;
		nodeIdType nodeId;
		std::getline( input, line );
		size_t elementCount = atol(line.c_str());
		mesh->beginElement( elementCount, new kmb::ElementContainerArray(elementCount) );
		for(i = 0;i<elementCount;++i){
			std::getline( input, line );
			std::istringstream tokenizer(line);
			while(tokenizer >> nodeId){
				vertices.push_back(nodeId);
			}
			switch( vertices.size() ){
				case 4:
					{
						ary[0] = vertices[0];
						ary[1] = vertices[1];
						ary[2] = vertices[2];
						ary[3] = vertices[3];
						mesh->addElement( kmb::TETRAHEDRON, ary );
					}
					break;
				case 10:
					{
						ary[0] = vertices[0];
						ary[1] = vertices[1];
						ary[2] = vertices[2];
						ary[3] = vertices[3];
						ary[4] = vertices[7];
						ary[5] = vertices[5];
						ary[6] = vertices[4];
						ary[7] = vertices[6];
						ary[8] = vertices[9];
						ary[9] = vertices[8];
						mesh->addElement( kmb::TETRAHEDRON2, ary );
					}
					break;
				case 8:
					{
						ary[0] = vertices[0];
						ary[1] = vertices[1];
						ary[2] = vertices[2];
						ary[3] = vertices[3];
						ary[4] = vertices[4];
						ary[5] = vertices[5];
						ary[6] = vertices[6];
						ary[7] = vertices[7];
						mesh->addElement( kmb::HEXAHEDRON, ary );
					}
					break;
				case 20:
					{
						ary[0] = vertices[0];
						ary[1] = vertices[1];
						ary[2] = vertices[2];
						ary[3] = vertices[3];
						ary[4] = vertices[4];
						ary[5] = vertices[5];
						ary[6] = vertices[6];
						ary[7] = vertices[7];
						ary[8] = vertices[8];
						ary[9] = vertices[9];
						ary[10] = vertices[10];
						ary[11] = vertices[11];
						ary[12] = vertices[16];
						ary[13] = vertices[17];
						ary[14] = vertices[18];
						ary[15] = vertices[19];
						ary[16] = vertices[12];
						ary[17] = vertices[13];
						ary[18] = vertices[14];
						ary[19] = vertices[15];
						mesh->addElement( kmb::HEXAHEDRON2, ary );
					}
					break;
				default:
					break;
			}
			vertices.clear();
		}
		mesh->endElement();
		unsigned int nodeCount = 0;
		double x=0.0,y=0.0,z=0.0;
		input >> nodeCount;
		mesh->beginNode( nodeCount );
		for(i = 0;i<nodeCount;++i){
			input >> x >> y >> z;
			mesh->addNode(x,y,z);
		}
		mesh->endNode();
		unsigned int bodyCount = 0;
		kmb::elementIdType elementId = kmb::Element::nullElementId;
		input >> bodyCount;
		for(i=0;i<bodyCount;++i){
			input >> elementCount;
			if(i>0){
				mesh->beginElement(elementCount);
				mesh->endElement();
			}
			for(unsigned int j=0;j<elementCount;++j){
				input >> elementId;
				mesh->moveElement(0,static_cast<kmb::bodyIdType>(i),elementId);
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::TetMeshMIO::saveToFile(const char* filename, const kmb::MeshData* mesh)
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		size_t elementCount = 0;
		size_t volumeCount = 0;
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getCount() > 0 &&
				( body->isUniqueType( kmb::TETRAHEDRON )
				|| body->isUniqueType( kmb::TETRAHEDRON2 )
				|| body->isUniqueType( kmb::HEXAHEDRON )
				|| body->isUniqueType( kmb::HEXAHEDRON2 ) ) )
			{
				elementCount += body->getCount();
				++volumeCount;
			}
		}
		output << elementCount << std::endl;
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body &&
				( body->isUniqueType( kmb::TETRAHEDRON )
				|| body->isUniqueType( kmb::TETRAHEDRON2 )
				|| body->isUniqueType( kmb::HEXAHEDRON )
				|| body->isUniqueType( kmb::HEXAHEDRON2 ) ) )
			{
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					switch( eIter.getType() ){
					case kmb::TETRAHEDRON:
						output <<
							eIter.getCellId(0) << " " <<
							eIter.getCellId(1) << " " <<
							eIter.getCellId(2) << " " <<
							eIter.getCellId(3) << std::endl;
						break;
					case kmb::TETRAHEDRON2:
						output <<
							eIter.getCellId(0) << " " <<
							eIter.getCellId(1) << " " <<
							eIter.getCellId(2) << " " <<
							eIter.getCellId(3) << " " <<
							eIter.getCellId(6) << " " <<
							eIter.getCellId(5) << " " <<
							eIter.getCellId(7) << " " <<
							eIter.getCellId(4) << " " <<
							eIter.getCellId(9) << " " <<
							eIter.getCellId(8) << std::endl;
						break;
					case kmb::HEXAHEDRON:
						output <<
							eIter.getCellId(0) << " " <<
							eIter.getCellId(1) << " " <<
							eIter.getCellId(2) << " " <<
							eIter.getCellId(3) << " " <<
							eIter.getCellId(4) << " " <<
							eIter.getCellId(5) << " " <<
							eIter.getCellId(6) << " " <<
							eIter.getCellId(7) << std::endl;
						break;
					case kmb::HEXAHEDRON2:
						output <<
							eIter.getCellId(0) << " " <<
							eIter.getCellId(1) << " " <<
							eIter.getCellId(2) << " " <<
							eIter.getCellId(3) << " " <<
							eIter.getCellId(4) << " " <<
							eIter.getCellId(5) << " " <<
							eIter.getCellId(6) << " " <<
							eIter.getCellId(7) << " " <<
							eIter.getCellId(8) << " " <<
							eIter.getCellId(9) << " " <<
							eIter.getCellId(10) << " " <<
							eIter.getCellId(11) << " " <<
							eIter.getCellId(16) << " " <<
							eIter.getCellId(17) << " " <<
							eIter.getCellId(18) << " " <<
							eIter.getCellId(19) << " " <<
							eIter.getCellId(12) << " " <<
							eIter.getCellId(13) << " " <<
							eIter.getCellId(14) << " " <<
							eIter.getCellId(15) << std::endl;
						break;
					default:
						break;
					}
					++eIter;
				}
			}
		}
		output << mesh->getNodeCount() << std::endl;
		double x,y,z;
		kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
		kmb::Point3DContainer::const_iterator nIterEnd = mesh->getNodes()->end();
		while( nIter != nIterEnd ){
			if( nIter.getXYZ(x,y,z) ){
				output << x << " " << y << " " << z << std::endl;
			}
			++nIter;
		}
		output << volumeCount << std::endl;

		kmb::elementIdType elementId = 0;
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body && body->getCount() > 0 &&
				( body->isUniqueType( kmb::TETRAHEDRON )
				|| body->isUniqueType( kmb::TETRAHEDRON2 )
				|| body->isUniqueType( kmb::HEXAHEDRON )
				|| body->isUniqueType( kmb::HEXAHEDRON2 ) ) )
			{
				output << body->getCount() << std::endl;
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					switch( eIter.getType() ){
					case kmb::TETRAHEDRON:
					case kmb::TETRAHEDRON2:
					case kmb::HEXAHEDRON:
					case kmb::HEXAHEDRON2:
						output << elementId << std::endl;
						++elementId;
						break;
					default:
						break;
					}
					++eIter;
				}
			}
		}
	}
	return 0;
}
