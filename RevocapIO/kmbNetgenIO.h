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
#pragma once

namespace kmb{

class MeshData;

class NetgenIO
{
public:
	NetgenIO(void){};
	~NetgenIO(void){};
	int loadFromFile(const char* filename,kmb::MeshData* mesh);
	int saveToFile(const char* filename,const kmb::MeshData* mesh);
};

}
