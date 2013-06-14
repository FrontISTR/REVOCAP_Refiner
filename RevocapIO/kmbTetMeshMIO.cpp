/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TetMeshMIO                                              #
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
#include "RevocapIO/kmbTetMeshMIO.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainerArray.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

int
kmb::TetMeshMIO::loadFromFile(const char* filename,MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		kmb::nodeIdType tmp[20];
		kmb::nodeIdType ary[20];
		std::fill( tmp, tmp+20, kmb::nullNodeId );
		std::fill( ary, ary+20, kmb::nullNodeId );
		std::ifstream input( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		size_t i;
		size_t count=0;
		std::string line;
		nodeIdType nodeId;
		std::getline( input, line );
		size_t elementCount = atol(line.c_str());
		mesh->beginElement( elementCount );
		for(i = 0;i<elementCount;++i){
			count = 0;
			std::getline( input, line );
			std::istringstream tokenizer(line);
			while(tokenizer >> nodeId){
				if( count < 20 ){
					tmp[count] = nodeId;
				}
				++count;
			}
			switch( count ){
			case 4:
				mesh->addElement( kmb::TETRAHEDRON, tmp );
				break;
			case 10:
				ary[0] = tmp[0];
				ary[1] = tmp[1];
				ary[2] = tmp[2];
				ary[3] = tmp[3];
				ary[6] = tmp[4];
				ary[5] = tmp[5];
				ary[7] = tmp[6];
				ary[4] = tmp[7];
				ary[9] = tmp[8];
				ary[8] = tmp[9];
				mesh->addElement( kmb::TETRAHEDRON2, ary );
				break;
			case 6:
				mesh->addElement( kmb::WEDGE, tmp );
				break;
			case 8:
				mesh->addElement( kmb::HEXAHEDRON, tmp );
				break;
			case 20:
				ary[0] = tmp[0];
				ary[1] = tmp[1];
				ary[2] = tmp[2];
				ary[3] = tmp[3];
				ary[4] = tmp[4];
				ary[5] = tmp[5];
				ary[6] = tmp[6];
				ary[7] = tmp[7];
				ary[8] = tmp[8];
				ary[9] = tmp[9];
				ary[10] = tmp[10];
				ary[11] = tmp[11];
				ary[16] = tmp[12];
				ary[17] = tmp[13];
				ary[18] = tmp[14];
				ary[19] = tmp[15];
				ary[12] = tmp[16];
				ary[13] = tmp[17];
				ary[14] = tmp[18];
				ary[15] = tmp[19];
				mesh->addElement( kmb::HEXAHEDRON2, ary );
				break;
			default:
				break;
			}
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
				kmb::bodyIdType bodyId = mesh->beginElement(elementCount);
				mesh->endElement();



				mesh->getBodyPtr(bodyId)->setOffsetId( 0 );
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
		size_t* elementCountEach = new size_t[bodyCount];
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			elementCountEach[bodyId] = 0;
			if( body && body->getCount() > 0 ){
				elementCountEach[bodyId] += body->getCountByType( kmb::TETRAHEDRON );
				elementCountEach[bodyId] += body->getCountByType( kmb::TETRAHEDRON2 );
				elementCountEach[bodyId] += body->getCountByType( kmb::HEXAHEDRON );
				elementCountEach[bodyId] += body->getCountByType( kmb::HEXAHEDRON2 );
				elementCountEach[bodyId] += body->getCountByType( kmb::WEDGE );
				elementCount += elementCountEach[bodyId];
				++volumeCount;
			}
		}
		output << elementCount << std::endl;
		for(kmb::bodyIdType bodyId = 0;bodyId<bodyCount;++bodyId){
			const kmb::Body* body = mesh->getBodyPtr(bodyId);
			if( body ){
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
					case kmb::WEDGE:
						output <<
							eIter.getCellId(0) << " " <<
							eIter.getCellId(1) << " " <<
							eIter.getCellId(2) << " " <<
							eIter.getCellId(3) << " " <<
							eIter.getCellId(4) << " " <<
							eIter.getCellId(5) << std::endl;
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
			if( body && body->getCount() > 0 ){
				output << elementCountEach[bodyId] << std::endl;
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() ){
					switch( eIter.getType() ){
					case kmb::TETRAHEDRON:
					case kmb::TETRAHEDRON2:
					case kmb::WEDGE:
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
		output.close();
	}
	return 0;
}
