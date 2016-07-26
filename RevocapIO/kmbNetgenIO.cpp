/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NetgenIO                                                #
#                                                                      #
#                                Written by                            #
#                                           Y. Ihara 2015/03/27        #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

#include "RevocapIO/kmbNetgenIO.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainerArray.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

int
kmb::NetgenIO::loadFromFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		unsigned int i;
		unsigned int j;
		FILE *fp;
		unsigned int elementCount=0;
		unsigned int nodeCount=0;
		if ((fp = fopen(filename, "r")) == NULL) {
			return -1;
		}
			char st[1024];
			while( fgets( st, sizeof(st), fp ) != NULL ){
				fpos_t fpos;
				fgetpos(fp, &fpos);
				if ( (st[0] == '#') || ( (st[0] == '!') && (st[1] == '!') ) ) continue;
				if (strncmp(st,"volumeelements",strlen("volumeelements"))==0){
					if( fgets( st, sizeof(st), fp ) ){
						sscanf(st, "%d", &elementCount );
						mesh->beginElement(elementCount);
						kmb::nodeIdType nodeTable[20];
						std::fill( nodeTable, nodeTable+20, kmb::nullNodeId );
						for(j=0;j<elementCount;++j){
							if( fgets( st, sizeof(st), fp ) ){
								int mat;
								int array_num;
								sscanf(st, "%d %d", &mat, &array_num);
								if (array_num == 4){
									sscanf(st, "%d %d %d %d %d %d",
										&mat, &array_num,
										&nodeTable[3], &nodeTable[1], &nodeTable[2], &nodeTable[0]);
									for (i=0;i<4;i++)	nodeTable[i] -= 1;
									mesh->addElement(kmb::TETRAHEDRON, nodeTable);
								} else if (array_num == 10){
									sscanf(st, "%d %d %d %d %d %d %d %d %d %d %d %d",
										&mat, &array_num,
										&nodeTable[3], &nodeTable[1], &nodeTable[2], &nodeTable[0],
										&nodeTable[8], &nodeTable[9], &nodeTable[7], &nodeTable[4],&nodeTable[6], &nodeTable[5]);
									for (i=0;i<10;i++)	nodeTable[i] -= 1;
									mesh->addElement(kmb::TETRAHEDRON2, nodeTable);
								}
							}
						}
						mesh->endElement();
					}
				}else if (strncmp(st,"points",strlen("points"))==0){
					if( fgets( st, sizeof(st), fp ) ){
						sscanf(st, "%d", &nodeCount );
						mesh->beginNode(nodeCount);
						double vx=0.0,vy=0.0,vz=0.0;
						for(j=0;j<nodeCount;++j){
							if( fgets( st, sizeof(st), fp ) ){
								sscanf(st, "%lf %lf %lf", &vx,&vy,&vz);
								mesh->addNode(vx,vy,vz);
							}
						}
						mesh->endNode();
					}
				}
			}
		fclose(fp);
	}
	
	return 0;
}

int
kmb::NetgenIO::saveToFile(const char* filename, const kmb::MeshData* mesh)
{
	if( mesh == NULL || !mesh->getNodes() ){
		return -1;
	}else{
		std::ofstream output( filename, std::ios_base::out );
		if( output.fail() ){
			return -1;
		}
		return -1;
	}
	return 0;
}
