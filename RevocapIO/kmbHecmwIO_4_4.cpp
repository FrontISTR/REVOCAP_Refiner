/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : HecmwIO (Ver.4)                                         #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2015/11/08     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
//
// HecMW Ver.4 のためのファイル I/O
//

#include "RevocapIO/kmbHecmwIO.h"
#include "MeshDB/kmbMeshData.h"

#include <fstream>

int kmb::HecmwIO::saveToMeshFile_4_4(const char* filename,const kmb::MeshData* mesh,const char* partName) const
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
