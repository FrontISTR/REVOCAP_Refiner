/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapCouplerIO                                        #
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
// REVOCAP_Coupler が利用するデータのＩ／Ｏ
//
// 領域分割後のファイル
// 大域Id=先頭の数字
// 局所Id=ファイルに表れる順番(0,1,2,...)
//

#pragma once

#include "MeshDB/kmbElement.h"
#include <fstream>

namespace kmb{

class MeshData;

class RevocapCouplerIO
{
private:
	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;
	// 要素を読み込んで globalId で登録
	kmb::bodyIdType loadElement( std::ifstream &input, kmb::elementType etype, size_t elementCount, kmb::MeshData* mesh );
	size_t loadNode( std::ifstream &input, size_t nodeCount, kmb::MeshData* mesh );
	size_t loadLocalNode( std::ifstream &input, size_t nodeCount, kmb::MeshData* mesh );
public:
	RevocapCouplerIO( kmb::nodeIdType offsetNodeId=0, kmb::elementIdType offsetElementId=0 );
	virtual ~RevocapCouplerIO(void);
	// mesh にはglobalIdで格納する
	int loadFromPartitionFile(const char* filename,kmb::MeshData* mesh);
	// 節点のみを読み込む
	// Id はlocalIdで格納する
	// globalIdは Data として name = "GlobalId" で参照
	int loadLocalNodesFromPartitionFile(const char* filename,kmb::MeshData* mesh);
};

}
