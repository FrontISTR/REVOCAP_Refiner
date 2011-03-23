/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : BLMGenerator                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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
#include "MeshGen/kmbBLMGenerator.h"

#include "Geometry/kmb_Geometry3D.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbNodeNeighborFaceInfo.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"

#include <set>
#include <map>
#include <iostream>

kmb::BLMGenerator::BLMGenerator(void)
: layerNum(0)
, layerThick(NULL)
, meshDB(NULL)
{
}

kmb::BLMGenerator::BLMGenerator(MeshDB* meshDB)
{
	this->meshDB = meshDB;
	layerNum = 1;
	layerThick = new double[1];
	layerThick[0] = 1.0;
}

kmb::BLMGenerator::BLMGenerator(MeshDB* meshDB,int layerNum,double* layerThick)
{
	this->meshDB = meshDB;
	this->layerNum = layerNum;
	this->layerThick = layerThick;
}

kmb::BLMGenerator::~BLMGenerator(void)
{
	clearLayerNodes();
	if(layerThick){
		delete[] layerThick;
	}
}

void
kmb::BLMGenerator::setLayer(int layerNum,double* thicks)
{
	this->layerNum = layerNum;
	if( this->layerThick ){
		delete[] this->layerThick;
	}
	this->layerThick = thicks;
}

void
kmb::BLMGenerator::setMeshDB(kmb::MeshDB* meshDB)
{
	this->meshDB = meshDB;
}

bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyId, kmb::ElementBase &upper, kmb::ElementBase &lower)
{
	if( this->meshDB == NULL || upper.getDimension() != 2 || lower.getDimension() != 2 ){
		return false;
	}
	if( upper.getType() != lower.getType() ){
		return false;
	}
	if( upper.getType() == kmb::TRIANGLE ){

		kmb::nodeIdType ary[6];
		for(int i=0;i<layerNum;++i){
			ary[0] = getLayerNodeId( upper[0], lower[0],i);
			ary[1] = getLayerNodeId( upper[1], lower[1],i);
			ary[2] = getLayerNodeId( upper[2], lower[2],i);
			ary[3] = getLayerNodeId( upper[0], lower[0],i+1);
			ary[4] = getLayerNodeId( upper[1], lower[1],i+1);
			ary[5] = getLayerNodeId( upper[2], lower[2],i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0){
				this->meshDB->insertElement(bodyId,kmb::WEDGE,ary);
			}else{
				return false;
			}
		}
		return true;
	}else if( upper.getType() == kmb::QUAD ){

		kmb::nodeIdType ary[8];
		for(int i=0;i<layerNum;++i){
			ary[0] = getLayerNodeId( upper[0], lower[0],i);
			ary[1] = getLayerNodeId( upper[1], lower[1],i);
			ary[2] = getLayerNodeId( upper[2], lower[2],i);
			ary[3] = getLayerNodeId( upper[3], lower[3],i);
			ary[4] = getLayerNodeId( upper[0], lower[0],i+1);
			ary[5] = getLayerNodeId( upper[1], lower[1],i+1);
			ary[6] = getLayerNodeId( upper[2], lower[2],i+1);
			ary[7] = getLayerNodeId( upper[3], lower[3],i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
					this->meshDB->insertElement(bodyId,kmb::HEXAHEDRON,ary);
			}else{
				return false;
			}
		}
		return true;
	}else{
		return false;
	}
}

bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyId, kmb::ElementBase &upper, kmb::nodeIdType lowerId)
{
	if( this->meshDB == NULL || upper.getDimension() != 2 ){
		return false;
	}
	if( upper.getType() == kmb::TRIANGLE ){


		kmb::nodeIdType ary[6];
		ary[0] = lowerId;
		ary[1] = getLayerNodeId( upper[0], lowerId, 1);
		ary[2] = getLayerNodeId( upper[1], lowerId, 1);
		ary[3] = getLayerNodeId( upper[2], lowerId, 1);
		if( ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 ){
			this->meshDB->insertElement(bodyId,kmb::TETRAHEDRON,ary);
		}else{
			return false;
		}
		for(int i=1;i<layerNum;++i){
			ary[0] = getLayerNodeId( upper[0], lowerId, i);
			ary[1] = getLayerNodeId( upper[1], lowerId, i);
			ary[2] = getLayerNodeId( upper[2], lowerId, i);
			ary[3] = getLayerNodeId( upper[0], lowerId, i+1);
			ary[4] = getLayerNodeId( upper[1], lowerId, i+1);
			ary[5] = getLayerNodeId( upper[2], lowerId, i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0){
				this->meshDB->insertElement(bodyId,kmb::WEDGE,ary);
			}else{
				return false;
			}
		}
		return true;
	}else if( upper.getType() == kmb::QUAD ){


		kmb::nodeIdType ary[8];
		ary[0] = lowerId;
		ary[1] = getLayerNodeId( upper[0], lowerId, 1);
		ary[2] = getLayerNodeId( upper[1], lowerId, 1);
		ary[3] = getLayerNodeId( upper[2], lowerId, 1);
		ary[4] = getLayerNodeId( upper[3], lowerId, 1);
		if( ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 ){
			this->meshDB->insertElement(bodyId,kmb::HEXAHEDRON,ary);
		}else{
			return false;
		}
		for(int i=1;i<layerNum;++i){
			ary[0] = getLayerNodeId( upper[0], lowerId, i);
			ary[1] = getLayerNodeId( upper[1], lowerId, i);
			ary[2] = getLayerNodeId( upper[2], lowerId, i);
			ary[3] = getLayerNodeId( upper[3], lowerId, i);
			ary[4] = getLayerNodeId( upper[0], lowerId, i+1);
			ary[5] = getLayerNodeId( upper[1], lowerId, i+1);
			ary[6] = getLayerNodeId( upper[2], lowerId, i+1);
			ary[7] = getLayerNodeId( upper[3], lowerId, i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
				this->meshDB->insertElement(bodyId,kmb::HEXAHEDRON,ary);
			}else{
				return false;
			}
		}
		return true;
	}else{
		return false;
	}
}

bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyId, kmb::nodeIdType upperId, kmb::ElementBase &lower)
{
	if( this->meshDB == NULL || lower.getDimension() != 2 ){
		return false;
	}
	int len = lower.getNodeCount();
	if( len == 3 ){

		kmb::nodeIdType ary[6];
		for(int i=0;i<layerNum-1;++i){
			ary[0] = getLayerNodeId( upperId, lower[0],i);
			ary[1] = getLayerNodeId( upperId, lower[1],i);
			ary[2] = getLayerNodeId( upperId, lower[2],i);
			ary[3] = getLayerNodeId( upperId, lower[0],i+1);
			ary[4] = getLayerNodeId( upperId, lower[1],i+1);
			ary[5] = getLayerNodeId( upperId, lower[2],i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0){
				this->meshDB->insertElement(bodyId,kmb::WEDGE,ary);
			}else{
				return false;
			}
		}

		ary[0] = getLayerNodeId( upperId, lower[0],layerNum-1);
		ary[1] = getLayerNodeId( upperId, lower[1],layerNum-1);
		ary[2] = getLayerNodeId( upperId, lower[2],layerNum-1);
		ary[3] = upperId;
		if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 ){
			this->meshDB->insertElement(bodyId,kmb::TETRAHEDRON,ary);
		}else{
			return false;
		}
		return true;
	}else if( len == 4 ){
		kmb::nodeIdType ary[8];
		for(int i=0;i<layerNum-1;++i){
			ary[0] = getLayerNodeId( upperId, lower[0],i);
			ary[1] = getLayerNodeId( upperId, lower[1],i);
			ary[2] = getLayerNodeId( upperId, lower[2],i);
			ary[3] = getLayerNodeId( upperId, lower[3],i);
			ary[4] = getLayerNodeId( upperId, lower[0],i+1);
			ary[5] = getLayerNodeId( upperId, lower[1],i+1);
			ary[6] = getLayerNodeId( upperId, lower[2],i+1);
			ary[7] = getLayerNodeId( upperId, lower[3],i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
				this->meshDB->insertElement(bodyId,kmb::HEXAHEDRON,ary);
			}else{
				return false;
			}
		}

		ary[4] = getLayerNodeId( upperId, lower[0],layerNum-1);
		ary[3] = getLayerNodeId( upperId, lower[1],layerNum-1);
		ary[2] = getLayerNodeId( upperId, lower[2],layerNum-1);
		ary[1] = getLayerNodeId( upperId, lower[3],layerNum-1);
		ary[0] = upperId;
		if( ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 ){
			this->meshDB->insertElement(bodyId,kmb::PYRAMID,ary);
		}else{
			return false;
		}
		return true;
	}else{
		return false;
	}
}

bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyId, kmb::ElementBase &upper, kmb::nodeIdType lowerId0, kmb::nodeIdType lowerId1)
{
	if( this->meshDB == NULL || upper.getType() != kmb::QUAD ){
		return false;
	}


	kmb::nodeIdType ary[8];
	ary[0] = lowerId0;
	ary[1] = getLayerNodeId( upper[0], lowerId0, 1);
	ary[2] = getLayerNodeId( upper[1], lowerId0, 1);
	ary[3] = lowerId1;
	ary[4] = getLayerNodeId( upper[2], lowerId1, 1);
	ary[5] = getLayerNodeId( upper[3], lowerId1, 1);
	if( ary[1] >= 0 && ary[2] >= 0 && ary[4] >= 0 && ary[5] >= 0 ){
		this->meshDB->insertElement(bodyId,kmb::WEDGE,ary);
	}else{
		return false;
	}
	for(int i=1;i<layerNum;++i){
		ary[0] = getLayerNodeId( upper[0], lowerId0, i);
		ary[1] = getLayerNodeId( upper[1], lowerId0, i);
		ary[2] = getLayerNodeId( upper[2], lowerId1, i);
		ary[3] = getLayerNodeId( upper[3], lowerId1, i);
		ary[4] = getLayerNodeId( upper[0], lowerId0, i+1);
		ary[5] = getLayerNodeId( upper[1], lowerId0, i+1);
		ary[6] = getLayerNodeId( upper[2], lowerId1, i+1);
		ary[7] = getLayerNodeId( upper[3], lowerId1, i+1);
		if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
			ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
			this->meshDB->insertElement(bodyId,kmb::HEXAHEDRON,ary);
		}else{
			return false;
		}
	}
	return true;
}

bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyId, kmb::nodeIdType upperId0, kmb::nodeIdType upperId1, kmb::ElementBase &lower)
{
	if( this->meshDB == NULL || lower.getType() != kmb::QUAD ){
		return false;
	}

	kmb::nodeIdType ary[8];
	for(int i=0;i<layerNum-1;++i){
		ary[0] = getLayerNodeId( upperId0, lower[0],i);
		ary[1] = getLayerNodeId( upperId0, lower[1],i);
		ary[2] = getLayerNodeId( upperId1, lower[2],i);
		ary[3] = getLayerNodeId( upperId1, lower[3],i);
		ary[4] = getLayerNodeId( upperId0, lower[0],i+1);
		ary[5] = getLayerNodeId( upperId0, lower[1],i+1);
		ary[6] = getLayerNodeId( upperId1, lower[2],i+1);
		ary[7] = getLayerNodeId( upperId1, lower[3],i+1);
		if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
			ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
			this->meshDB->insertElement(bodyId,kmb::HEXAHEDRON,ary);
		}else{
			return false;
		}
	}

	ary[0] = getLayerNodeId( upperId0, lower[0],layerNum-1);
	ary[1] = getLayerNodeId( upperId0, lower[1],layerNum-1);
	ary[2] = upperId0;
	ary[3] = getLayerNodeId( upperId1, lower[2],layerNum-1);
	ary[4] = getLayerNodeId( upperId1, lower[3],layerNum-1);
	ary[5] = upperId1;
	if( ary[0] >= 0 && ary[1] >= 0 && ary[3] >= 0 && ary[4] >= 0 ){
		this->meshDB->insertElement(bodyId,kmb::WEDGE,ary);
	}else{
		return false;
	}
	return true;
}

/*
bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyID, std::vector<kmb::nodeIdType>& upperID0, std::vector<kmb::nodeIdType>& upperID1, kmb::nodeIdType lowerID0, kmb::nodeIdType lowerID1)
{
	if( this->meshDB == NULL ){
		return false;
	}

	if( upperID0.size() < 1 || upperID0.size() > 2 ){
		return false;
	}
	if( upperID1.size() < 1 || upperID1.size() > 2 ){
		return false;
	}

	if( upperID0.size() == 1 && upperID1.size() == 1 ){
		return false;
	}

	if( upperID0.size() == 1 && upperID1.size() == 2 ){


		kmb::nodeIdType* ary0 = NULL;
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = lowerID0;
		ary0[1] = getLayerNodeId( upperID0[0], lowerID0, 1);
		ary0[2] = getLayerNodeId( upperID1[1], lowerID1, 1);
		ary0[3] = getLayerNodeId( upperID1[0], lowerID1, 1);
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
		ary0 = NULL;
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = lowerID1;
		ary0[1] = getLayerNodeId( upperID1[1], lowerID1, 1);
		ary0[2] = getLayerNodeId( upperID1[0], lowerID1, 1);
		ary0[3] = lowerID0;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
		for(int i=1;i<layerNum;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[6];
			ary[0] = getLayerNodeId( upperID1[1], lowerID1, i);
			ary[1] = getLayerNodeId( upperID1[0], lowerID1, i);
			ary[2] = getLayerNodeId( upperID0[0], lowerID0, i);
			ary[3] = getLayerNodeId( upperID1[1], lowerID1, i+1);
			ary[4] = getLayerNodeId( upperID1[0], lowerID1, i+1);
			ary[5] = getLayerNodeId( upperID0[0], lowerID0, i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 &&
				ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0 ){
				this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}
	}else if( upperID0.size() == 2 && upperID1.size() == 1 ){


		kmb::nodeIdType* ary0 = NULL;
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = lowerID1;
		ary0[1] = getLayerNodeId( upperID1[0], lowerID1, 1);
		ary0[2] = getLayerNodeId( upperID0[1], lowerID0, 1);
		ary0[3] = getLayerNodeId( upperID0[0], lowerID0, 1);
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
		ary0 = NULL;
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = lowerID0;
		ary0[1] = getLayerNodeId( upperID0[1], lowerID0, 1);
		ary0[2] = getLayerNodeId( upperID0[0], lowerID0, 1);
		ary0[3] = lowerID1;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
		for(int i=1;i<layerNum;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[6];
			ary[0] = getLayerNodeId( upperID0[1], lowerID0, i);
			ary[1] = getLayerNodeId( upperID0[0], lowerID0, i);
			ary[2] = getLayerNodeId( upperID1[0], lowerID1, i);
			ary[3] = getLayerNodeId( upperID0[1], lowerID0, i+1);
			ary[4] = getLayerNodeId( upperID0[0], lowerID0, i+1);
			ary[5] = getLayerNodeId( upperID1[0], lowerID1, i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 &&
				ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0 ){
				this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}
	}else if( upperID0.size() == 2 && upperID1.size() == 2 ){


		kmb::nodeIdType* ary0 = NULL;
		ary0 = new kmb::nodeIdType[6];
		ary0[0] = lowerID0;
		ary0[1] = getLayerNodeId( upperID0[1], lowerID0, 1);
		ary0[2] = getLayerNodeId( upperID0[0], lowerID0, 1);
		ary0[3] = lowerID1;
		ary0[4] = getLayerNodeId( upperID1[0], lowerID1, 1);
		ary0[5] = getLayerNodeId( upperID1[1], lowerID1, 1);
		this->meshDB->insertElement(bodyID,kmb::WEDGE,ary0);
		for(int i=1;i<layerNum;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[8];
			ary[0] = getLayerNodeId( upperID0[1], lowerID0, i);
			ary[1] = getLayerNodeId( upperID0[0], lowerID0, i);
			ary[2] = getLayerNodeId( upperID1[1], lowerID1, i);
			ary[3] = getLayerNodeId( upperID1[0], lowerID1, i);
			ary[4] = getLayerNodeId( upperID0[1], lowerID0, i+1);
			ary[5] = getLayerNodeId( upperID0[0], lowerID0, i+1);
			ary[6] = getLayerNodeId( upperID1[1], lowerID1, i+1);
			ary[7] = getLayerNodeId( upperID1[0], lowerID1, i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
				this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}
	}
	return true;
}

bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID0, kmb::nodeIdType upperID1, std::vector<kmb::nodeIdType>& lowerID0, std::vector<kmb::nodeIdType>& lowerID1)
{
	if( this->meshDB == NULL ){
		return false;
	}

	if( lowerID0.size() < 1 || lowerID0.size() > 2 ){
		return false;
	}
	if( lowerID1.size() < 1 || lowerID1.size() > 2 ){
		return false;
	}

	if( lowerID0.size() == 1 && lowerID1.size() == 1 ){
		return false;
	}

	if( lowerID0.size() == 1 && lowerID1.size() == 2 ){
		for(int i=0;i<layerNum-1;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[6];
			ary[0] = getLayerNodeId( upperID0, lowerID0[0], i);
			ary[1] = getLayerNodeId( upperID1, lowerID1[0], i);
			ary[2] = getLayerNodeId( upperID1, lowerID1[1], i);
			ary[3] = getLayerNodeId( upperID0, lowerID0[0], i+1);
			ary[4] = getLayerNodeId( upperID1, lowerID1[0], i+1);
			ary[5] = getLayerNodeId( upperID1, lowerID1[1], i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 &&
				ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0 ){
				this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}


		kmb::nodeIdType* ary0 = NULL;
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = upperID0;
		ary0[1] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1);
		ary0[2] = getLayerNodeId( upperID1, lowerID1[1], layerNum-1);
		ary0[3] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1);
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = upperID1;
		ary0[1] = getLayerNodeId( upperID1, lowerID1[1], layerNum-1);
		ary0[2] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1);
		ary0[3] = upperID0;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
	}else if( lowerID0.size() == 2 && lowerID1.size() == 1 ){
		for(int i=0;i<layerNum-1;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[6];
			ary[0] = getLayerNodeId( upperID1, lowerID1[0], i);
			ary[1] = getLayerNodeId( upperID0, lowerID0[0], i);
			ary[2] = getLayerNodeId( upperID0, lowerID0[1], i);
			ary[3] = getLayerNodeId( upperID1, lowerID1[0], i+1);
			ary[4] = getLayerNodeId( upperID0, lowerID0[0], i+1);
			ary[5] = getLayerNodeId( upperID0, lowerID0[1], i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 &&
				ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0 ){
				this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}


		kmb::nodeIdType* ary0 = NULL;
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1);
		ary0[1] = getLayerNodeId( upperID0, lowerID0[1], layerNum-1);
		ary0[2] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1);
		ary0[3] = upperID1;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
		ary0 = NULL;
		ary0 = new kmb::nodeIdType[4];
		ary0[0] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1);
		ary0[1] = getLayerNodeId( upperID0, lowerID0[1], layerNum-1);
		ary0[2] = upperID1;
		ary0[3] = upperID0;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary0);
	}else if( lowerID0.size() == 2 && lowerID1.size() == 2 ){
		for(int i=0;i<layerNum-1;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[8];
			ary[0] = getLayerNodeId( upperID0, lowerID0[0], i);
			ary[1] = getLayerNodeId( upperID0, lowerID0[1], i);
			ary[2] = getLayerNodeId( upperID1, lowerID1[0], i);
			ary[3] = getLayerNodeId( upperID1, lowerID1[1], i);
			ary[4] = getLayerNodeId( upperID0, lowerID0[0], i+1);
			ary[5] = getLayerNodeId( upperID0, lowerID0[1], i+1);
			ary[6] = getLayerNodeId( upperID1, lowerID1[0], i+1);
			ary[7] = getLayerNodeId( upperID1, lowerID1[1], i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
				this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}


		kmb::nodeIdType* ary0 = NULL;
		ary0 = new kmb::nodeIdType[6];
		ary0[0] = upperID0;
		ary0[1] = getLayerNodeId( upperID0, lowerID0[1], layerNum-1 );
		ary0[2] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1 );
		ary0[3] = upperID1;
		ary0[4] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1 );
		ary0[5] = getLayerNodeId( upperID1, lowerID1[1], layerNum-1 );
		this->meshDB->insertElement(bodyID,kmb::WEDGE,ary0);
	}
	return true;
}
*/

bool
kmb::BLMGenerator::generate(kmb::bodyIdType bodyID,kmb::bodyIdType &layerID,kmb::bodyIdType &boundaryID)
{
	kmb::ElementEvaluator evaluator( this->meshDB->getNodes() );


	std::map<kmb::elementIdType,kmb::elementIdType> elementMap;
	kmb::ElementContainer* body = this->meshDB->getBodyPtr(bodyID);
	size_t size;


	if( body ){
		kmb::nodeIdType* cells = new kmb::nodeIdType[kmb::Element::MAX_NODE_COUNT];
		size = body->getCount();
		boundaryID = this->meshDB->beginElement(size);


		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() ){
			kmb::elementType etype = kmb::UNKNOWNTYPE;
			if( eIter.getElement( etype, cells ) ){
				kmb::elementIdType newID = this->meshDB->addElement( etype, cells );

				elementMap[eIter.getId()] = newID;
			}
			++eIter;
		}
		this->meshDB->endElement();
		delete cells;
	}else{
		return false;
	}


	kmb::NodeNeighborInfo neighborInfo;
	neighborInfo.appendCoboundary( body );

	std::set<kmb::nodeIdType> nodeSet;
	if( body )
	{
		body->getNodesOfBody( nodeSet );
	}


	kmb::Node node;
	std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
	while( nIter != nodeSet.end() )
	{
		kmb::nodeIdType nodeID = (*nIter);
		++nIter;

		kmb::Vector3D normVect;
		kmb::Vector3D v;
		kmb::NodeNeighbor::iterator eIter = neighborInfo.beginIteratorAt( nodeID );
		kmb::NodeNeighbor::iterator end = neighborInfo.endIteratorAt( nodeID );
		while( eIter != end )
		{
			kmb::elementIdType elementID = eIter->second;
			kmb::ElementContainer::iterator e = body->find( elementID );
			if( evaluator.getNormalVector( e, v ) ){
				normVect += v;
			}
			++eIter;
		}
		normVect.normalize();
		this->meshDB->getNode(nodeID,node);

		kmb::Point3D point = node + normVect.scalar(layerThick[layerNum-1]);

		kmb::nodeIdType outerNodeID = this->meshDB->addNode(point);

		eIter = neighborInfo.beginIteratorAt( nodeID );
		while( eIter != end )
		{
			kmb::elementIdType elementID = eIter->second;
			kmb::elementIdType outerElementID = elementMap[elementID];
			kmb::ElementContainer::iterator element = this->meshDB->findElement(outerElementID,boundaryID);
			element.replaceNodeId(nodeID,outerNodeID);
			++eIter;
		}

	}

	kmb::ElementContainer* boundary = this->meshDB->getBodyPtr(boundaryID);
	if( boundary ){
		layerID = this->meshDB->beginElement(size*layerNum);
		this->meshDB->endElement();

		for( std::map<kmb::elementIdType,kmb::elementIdType>::iterator p = elementMap.begin();
			p != elementMap.end();
			++p )
		{
			kmb::elementIdType lowerID = p->first;
			kmb::elementIdType upperID = p->second;
			kmb::ElementContainer::iterator lower = this->meshDB->findElement( lowerID, bodyID );
			kmb::ElementContainer::iterator upper = this->meshDB->findElement( upperID, boundaryID );
			appendLayerElements( layerID, upper, lower );
		}
	}

	clearLayerNodes();
	return true;
}


kmb::nodeIdType getDuplicatedNodeId(kmb::nodeIdType nodeId,std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper,kmb::MeshDB*mesh){
	std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator nIter = nodeMapper.find(nodeId);
	if( nIter != nodeMapper.end() ){
		return nIter->second;
	}else{
		kmb::Point3D pt;
		mesh->getNode(nodeId,pt);
		kmb::nodeIdType duplicatedId = mesh->addNode(pt);
		nodeMapper.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(nodeId,duplicatedId) );
		return duplicatedId;
	}
}

bool
kmb::BLMGenerator::generateFromData(const char* faceGroup,kmb::bodyIdType &layerId)
{











	kmb::DataBindings* faceData = meshDB->getDataBindingsPtr(faceGroup);
	if( faceData == NULL || faceData->getBindingMode() != kmb::DataBindings::FACEGROUP ){
		return false;
	}
	kmb::ElementContainer* parentBody = meshDB->getBodyPtr( faceData->getTargetBodyId() );
	if( parentBody == NULL ){
		return false;
	}
	kmb::ElementContainer* tmpBody = new kmb::ElementContainerMap();
	kmb::ElementEvaluator evaluator( this->meshDB->getNodes() );
	std::map< kmb::nodeIdType, kmb::nodeIdType > nodeMapper;


	kmb::DataBindings::iterator fIter = faceData->begin();
	kmb::nodeIdType nodeTable[8];
	while( !fIter.isFinished() ){
		kmb::Face f;
		if( fIter.getFace(f) ){
			int localId = static_cast<int>( f.getLocalFaceId() );
			kmb::ElementContainer::iterator eIter = parentBody->find( f.getElementId() );
			if( !eIter.isFinished() ){
				switch( eIter.getBoundaryType(localId) )
				{
				case kmb::TRIANGLE:
					nodeTable[0] = eIter.getBoundaryCellId(localId,0);
					nodeTable[1] = eIter.getBoundaryCellId(localId,1);
					nodeTable[2] = eIter.getBoundaryCellId(localId,2);
					nodeTable[3] = getDuplicatedNodeId( nodeTable[0], nodeMapper, meshDB );
					nodeTable[4] = getDuplicatedNodeId( nodeTable[1], nodeMapper, meshDB );
					nodeTable[5] = getDuplicatedNodeId( nodeTable[2], nodeMapper, meshDB );
					tmpBody->addElement(kmb::WEDGE,nodeTable);
					break;
				case kmb::QUAD:
					nodeTable[0] = eIter.getBoundaryCellId(localId,0);
					nodeTable[1] = eIter.getBoundaryCellId(localId,1);
					nodeTable[2] = eIter.getBoundaryCellId(localId,2);
					nodeTable[3] = eIter.getBoundaryCellId(localId,3);
					nodeTable[4] = getDuplicatedNodeId( nodeTable[0], nodeMapper, meshDB );
					nodeTable[5] = getDuplicatedNodeId( nodeTable[1], nodeMapper, meshDB );
					nodeTable[6] = getDuplicatedNodeId( nodeTable[2], nodeMapper, meshDB );
					nodeTable[7] = getDuplicatedNodeId( nodeTable[3], nodeMapper, meshDB );
					tmpBody->addElement(kmb::HEXAHEDRON,nodeTable);
					break;
				default:
					break;
				}
			}
		}
		++fIter;
	}


	kmb::NodeNeighborFaceInfo neighborInfo;
	neighborInfo.appendCoboundary( faceData, parentBody );


	kmb::Node node;
	std::map<kmb::nodeIdType,kmb::nodeIdType>::iterator nIter = nodeMapper.begin();
	while( nIter != nodeMapper.end() )
	{
		kmb::nodeIdType nodeId = nIter->first;

		kmb::Vector3D normVect;
		kmb::Vector3D v;
		kmb::NodeNeighborFace::iterator nfIter = neighborInfo.beginIteratorAt( nodeId );
		kmb::NodeNeighborFace::iterator nfEnd = neighborInfo.endIteratorAt( nodeId );
		while( nfIter != nfEnd )
		{
			kmb::Face f = nfIter->second;
			kmb::ElementContainer::iterator element = parentBody->find( f.getElementId() );
			if( !element.isFinished() &&
				evaluator.getNormalVectorOfFace( element, f.getLocalFaceId(), v ) )
			{
				normVect += v;
			}
			++nfIter;
		}
		normVect.normalize();
		this->meshDB->getNode(nodeId,node);

		kmb::Point3D point = node + normVect.scalar(layerThick[layerNum-1]);
		meshDB->updateNode( point.x(), point.y(), point.z(), nIter->second );
		++nIter;
	}


	layerId = this->meshDB->beginElement( faceData->getIdCount()*layerNum );

	kmb::ElementContainer::iterator bIter = tmpBody->begin();
	while( !bIter.isFinished() ){
		switch( bIter.getType() )
		{
		case kmb::WEDGE:
			for(int i=0;i<layerNum;++i){
				nodeTable[0] = getLayerNodeId( bIter[3], bIter[0],i);
				nodeTable[1] = getLayerNodeId( bIter[4], bIter[1],i);
				nodeTable[2] = getLayerNodeId( bIter[5], bIter[2],i);
				nodeTable[3] = getLayerNodeId( bIter[3], bIter[0],i+1);
				nodeTable[4] = getLayerNodeId( bIter[4], bIter[1],i+1);
				nodeTable[5] = getLayerNodeId( bIter[5], bIter[2],i+1);
				if( nodeTable[0] >= 0 &&
					nodeTable[1] >= 0 &&
					nodeTable[2] >= 0 &&
					nodeTable[3] >= 0 &&
					nodeTable[4] >= 0 &&
					nodeTable[5] >= 0)
				{
					this->meshDB->addElement(kmb::WEDGE,nodeTable);
				}
			}
			break;
		case kmb::HEXAHEDRON:
			for(int i=0;i<layerNum;++i){
				nodeTable[0] = getLayerNodeId( bIter[4], bIter[0],i);
				nodeTable[1] = getLayerNodeId( bIter[5], bIter[1],i);
				nodeTable[2] = getLayerNodeId( bIter[6], bIter[2],i);
				nodeTable[3] = getLayerNodeId( bIter[7], bIter[3],i);
				nodeTable[4] = getLayerNodeId( bIter[4], bIter[0],i+1);
				nodeTable[5] = getLayerNodeId( bIter[5], bIter[1],i+1);
				nodeTable[6] = getLayerNodeId( bIter[6], bIter[2],i+1);
				nodeTable[7] = getLayerNodeId( bIter[7], bIter[3],i+1);
				if( nodeTable[0] >= 0 &&
					nodeTable[1] >= 0 &&
					nodeTable[2] >= 0 &&
					nodeTable[3] >= 0 &&
					nodeTable[4] >= 0 &&
					nodeTable[5] >= 0 &&
					nodeTable[6] >= 0 &&
					nodeTable[7] >= 0)
				{
					this->meshDB->addElement(kmb::HEXAHEDRON,nodeTable);
				}
			}
			break;
		default:
			break;
		}
		++bIter;
	}
	this->meshDB->endElement();


	delete tmpBody;
	clearLayerNodes();
	return true;
}

kmb::nodeIdType
kmb::BLMGenerator::getLayerNodeId(kmb::nodeIdType outerNodeID, kmb::nodeIdType innerNodeID, int layerIndex )
{
	if( this->meshDB == NULL || 0 > layerIndex || layerIndex > this->layerNum ){
		return kmb::nullNodeId;

	}
	kmb::nodeIdType* nodes = NULL;

	if( this->layerNodes.find( std::pair<kmb::nodeIdType,kmb::nodeIdType>(innerNodeID,outerNodeID) ) != this->layerNodes.end() ){
		nodes = this->layerNodes[ std::pair<kmb::nodeIdType,kmb::nodeIdType>(innerNodeID,outerNodeID) ];
	}else{

		kmb::Node inner,outer;
		if( !this->meshDB->getNode(innerNodeID,inner)
			|| !this->meshDB->getNode(outerNodeID,outer) ){
			return kmb::nullNodeId;
		}
		nodes = new kmb::nodeIdType[ this->layerNum-1 ];
		if( nodes != NULL ){
			kmb::Vector3D dict = outer - inner;
			dict.normalize();
			for( int i = 0 ; i < this->layerNum-1; ++i )
			{

				kmb::Point3D pt = inner + dict.scalar( layerThick[i] );
				nodes[i] = this->meshDB->addNode(pt);
			}
			this->layerNodes.insert( std::pair< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType*>( std::pair<kmb::nodeIdType,kmb::nodeIdType>(innerNodeID,outerNodeID), nodes ) );
		}
	}
	if( nodes == NULL ){
		return kmb::nullNodeId;
	}
	if( layerIndex == this->layerNum ){
		return outerNodeID;
	}else if( layerIndex == 0 ){
		return innerNodeID;
	}else{
		return nodes[layerIndex-1];
	}
}

void
kmb::BLMGenerator::clearLayerNodes()
{
	for( std::map< std::pair<kmb::nodeIdType, kmb::nodeIdType>, kmb::nodeIdType*>::iterator p = this->layerNodes.begin();
		p != this->layerNodes.end();
		p++)
	{
		kmb::nodeIdType* a = p->second;
		if( a ){
			delete[] a;
			a = NULL;
		}
	}
	layerNodes.clear();
}
