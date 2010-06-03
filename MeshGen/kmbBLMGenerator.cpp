/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : BLMGenerator                                            #
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
#include "MeshGen/kmbBLMGenerator.h"

#include "Geometry/kmb_Geometry3D.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbElementContainer.h"
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

void kmb::BLMGenerator::
setLayer(int layerNum,double* thicks)
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

kmb::elementType kmb::BLMGenerator::
appendElement2D( kmb::bodyIdType bodyID,std::vector< kmb::nodeIdType >* nodes )
{
	if( this->meshDB == NULL ){
		return kmb::UNKNOWNTYPE;
	}
	const int len = static_cast<int>(nodes->size());
	if( len == 3 ){
		if( nodes->at(0) == nodes->at(1) ||
			nodes->at(1) == nodes->at(2) ||
			nodes->at(2) == nodes->at(0) )
		{
			return kmb::UNKNOWNTYPE;
		}else{
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[3];
			ary[0] = nodes->at(0);
			ary[1] = nodes->at(1);
			ary[2] = nodes->at(2);
			this->meshDB->insertElement(bodyID,kmb::TRIANGLE,ary);
			return kmb::TRIANGLE;
		}
	}else if( len == 4 ){
		if(
			( nodes->at(0) == nodes->at(1) && nodes->at(1) == nodes->at(2) ) ||
			( nodes->at(1) == nodes->at(2) && nodes->at(2) == nodes->at(3) ) ||
			( nodes->at(2) == nodes->at(3) && nodes->at(3) == nodes->at(0) ) ||
			( nodes->at(3) == nodes->at(0) && nodes->at(0) == nodes->at(1) ) )
		{

			return kmb::UNKNOWNTYPE;
		}else if( nodes->at(0) == nodes->at(1) ){

			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[3];
			ary[0] = nodes->at(1);
			ary[1] = nodes->at(2);
			ary[2] = nodes->at(3);
			this->meshDB->insertElement(bodyID,kmb::TRIANGLE,ary);
			return kmb::TRIANGLE;
		}else if( nodes->at(1) == nodes->at(2) ){

			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[3];
			ary[0] = nodes->at(0);
			ary[1] = nodes->at(2);
			ary[2] = nodes->at(3);
			this->meshDB->insertElement(bodyID,kmb::TRIANGLE,ary);
			return kmb::TRIANGLE;
		}else if( nodes->at(2) == nodes->at(3) ){

			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[3];
			ary[0] = nodes->at(0);
			ary[1] = nodes->at(1);
			ary[2] = nodes->at(3);
			this->meshDB->insertElement(bodyID,kmb::TRIANGLE,ary);
			return kmb::TRIANGLE;
		}else if( nodes->at(3) == nodes->at(0) ){

			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[3];
			ary[0] = nodes->at(0);
			ary[1] = nodes->at(1);
			ary[2] = nodes->at(2);
			this->meshDB->insertElement(bodyID,kmb::TRIANGLE,ary);
			return kmb::TRIANGLE;
		}else if( nodes->at(0) == nodes->at(2) || nodes->at(1) == nodes->at(3) ){

			return kmb::UNKNOWNTYPE;
		}else{

			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[4];
			ary[0] = nodes->at(0);
			ary[1] = nodes->at(1);
			ary[2] = nodes->at(2);
			ary[3] = nodes->at(3);
			this->meshDB->insertElement(bodyID,kmb::QUAD,ary);
			return kmb::QUAD;
		}
	}else{
		return kmb::UNKNOWNTYPE;
	}
}

bool
kmb::BLMGenerator::appendLayerElements( kmb::bodyIdType bodyID, kmb::ElementBase &upper, kmb::ElementBase &lower)
{
	if( this->meshDB == NULL || upper.getDimension() != 2 || lower.getDimension() != 2 ){
		return false;
	}
	if( upper.getType() != lower.getType() ){
		return false;
	}
	if( upper.getType() == kmb::TRIANGLE ){

		for(int i=0;i<layerNum;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[6];
			ary[0] = getLayerNodeId( upper.getCellId(0), lower.getCellId(0),i);
			ary[1] = getLayerNodeId( upper.getCellId(1), lower.getCellId(1),i);
			ary[2] = getLayerNodeId( upper.getCellId(2), lower.getCellId(2),i);
			ary[3] = getLayerNodeId( upper.getCellId(0), lower.getCellId(0),i+1);
			ary[4] = getLayerNodeId( upper.getCellId(1), lower.getCellId(1),i+1);
			ary[5] = getLayerNodeId( upper.getCellId(2), lower.getCellId(2),i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0){
				this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}
		return true;
	}else if( upper.getType() == kmb::QUAD ){

		for(int i=0;i<layerNum;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[8];
			ary[0] = getLayerNodeId( upper.getCellId(0), lower.getCellId(0),i);
			ary[1] = getLayerNodeId( upper.getCellId(1), lower.getCellId(1),i);
			ary[2] = getLayerNodeId( upper.getCellId(2), lower.getCellId(2),i);
			ary[3] = getLayerNodeId( upper.getCellId(3), lower.getCellId(3),i);
			ary[4] = getLayerNodeId( upper.getCellId(0), lower.getCellId(0),i+1);
			ary[5] = getLayerNodeId( upper.getCellId(1), lower.getCellId(1),i+1);
			ary[6] = getLayerNodeId( upper.getCellId(2), lower.getCellId(2),i+1);
			ary[7] = getLayerNodeId( upper.getCellId(3), lower.getCellId(3),i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
					this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}
		return true;
	}else{
		return false;
	}
}

bool kmb::BLMGenerator::
appendLayerElements( kmb::bodyIdType bodyID, kmb::ElementBase &upper, kmb::nodeIdType lowerID)
{
	if( this->meshDB == NULL || upper.getDimension() != 2 ){
		return false;
	}
	if( upper.getType() == kmb::TRIANGLE ){


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = lowerID;
		ary[1] = getLayerNodeId( upper.getCellId(0), lowerID, 1);
		ary[2] = getLayerNodeId( upper.getCellId(1), lowerID, 1);
		ary[3] = getLayerNodeId( upper.getCellId(2), lowerID, 1);
		if( ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 ){
			this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
		}else{
			if( ary )	delete[] ary;
			return false;
		}
		for(int i=1;i<layerNum;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[6];
			ary[0] = getLayerNodeId( upper.getCellId(0), lowerID, i);
			ary[1] = getLayerNodeId( upper.getCellId(1), lowerID, i);
			ary[2] = getLayerNodeId( upper.getCellId(2), lowerID, i);
			ary[3] = getLayerNodeId( upper.getCellId(0), lowerID, i+1);
			ary[4] = getLayerNodeId( upper.getCellId(1), lowerID, i+1);
			ary[5] = getLayerNodeId( upper.getCellId(2), lowerID, i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0){
				this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}
		return true;
	}else if( upper.getType() == kmb::QUAD ){


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[5];
		ary[0] = lowerID;
		ary[1] = getLayerNodeId( upper.getCellId(0), lowerID, 1);
		ary[2] = getLayerNodeId( upper.getCellId(1), lowerID, 1);
		ary[3] = getLayerNodeId( upper.getCellId(2), lowerID, 1);
		ary[4] = getLayerNodeId( upper.getCellId(3), lowerID, 1);
		if( ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 ){
			this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
		}else{
			if( ary )	delete[] ary;
			return false;
		}
		for(int i=1;i<layerNum;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[8];
			ary[0] = getLayerNodeId( upper.getCellId(0), lowerID, i);
			ary[1] = getLayerNodeId( upper.getCellId(1), lowerID, i);
			ary[2] = getLayerNodeId( upper.getCellId(2), lowerID, i);
			ary[3] = getLayerNodeId( upper.getCellId(3), lowerID, i);
			ary[4] = getLayerNodeId( upper.getCellId(0), lowerID, i+1);
			ary[5] = getLayerNodeId( upper.getCellId(1), lowerID, i+1);
			ary[6] = getLayerNodeId( upper.getCellId(2), lowerID, i+1);
			ary[7] = getLayerNodeId( upper.getCellId(3), lowerID, i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
				this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}
		return true;
	}else{
		return false;
	}
}

bool kmb::BLMGenerator::
appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID, kmb::ElementBase &lower)
{
	if( this->meshDB == NULL || lower.getDimension() != 2 ){
		return false;
	}
	int len = lower.getNodeCount();
	if( len == 3 ){

		for(int i=0;i<layerNum-1;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[6];
			ary[0] = getLayerNodeId( upperID, lower.getCellId(0),i);
			ary[1] = getLayerNodeId( upperID, lower.getCellId(1),i);
			ary[2] = getLayerNodeId( upperID, lower.getCellId(2),i);
			ary[3] = getLayerNodeId( upperID, lower.getCellId(0),i+1);
			ary[4] = getLayerNodeId( upperID, lower.getCellId(1),i+1);
			ary[5] = getLayerNodeId( upperID, lower.getCellId(2),i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 && ary[5] >= 0){
				this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}

		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = getLayerNodeId( upperID, lower.getCellId(0),layerNum-1);
		ary[1] = getLayerNodeId( upperID, lower.getCellId(1),layerNum-1);
		ary[2] = getLayerNodeId( upperID, lower.getCellId(2),layerNum-1);
		ary[3] = upperID;
		if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 ){
			this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
		}else{
			if( ary )	delete[] ary;
			return false;
		}
		return true;
	}else if( len == 4 ){
		for(int i=0;i<layerNum-1;++i){
			kmb::nodeIdType* ary = NULL;
			ary = new kmb::nodeIdType[8];
			ary[0] = getLayerNodeId( upperID, lower.getCellId(0),i);
			ary[1] = getLayerNodeId( upperID, lower.getCellId(1),i);
			ary[2] = getLayerNodeId( upperID, lower.getCellId(2),i);
			ary[3] = getLayerNodeId( upperID, lower.getCellId(3),i);
			ary[4] = getLayerNodeId( upperID, lower.getCellId(0),i+1);
			ary[5] = getLayerNodeId( upperID, lower.getCellId(1),i+1);
			ary[6] = getLayerNodeId( upperID, lower.getCellId(2),i+1);
			ary[7] = getLayerNodeId( upperID, lower.getCellId(3),i+1);
			if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
				ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
				this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
			}else{
				if( ary )	delete[] ary;
				return false;
			}
		}

		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[5];
		ary[4] = getLayerNodeId( upperID, lower.getCellId(0),layerNum-1);
		ary[3] = getLayerNodeId( upperID, lower.getCellId(1),layerNum-1);
		ary[2] = getLayerNodeId( upperID, lower.getCellId(2),layerNum-1);
		ary[1] = getLayerNodeId( upperID, lower.getCellId(3),layerNum-1);
		ary[0] = upperID;
		if( ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 && ary[4] >= 0 ){
			this->meshDB->insertElement(bodyID,kmb::PYRAMID,ary);
		}else{
			if( ary )	delete[] ary;
			return false;
		}
		return true;
	}else{
		return false;
	}
}

bool kmb::BLMGenerator::
appendLayerElements( kmb::bodyIdType bodyID, kmb::ElementBase &upper, kmb::nodeIdType lowerID0, kmb::nodeIdType lowerID1)
{
	if( this->meshDB == NULL || upper.getType() != kmb::QUAD ){
		return false;
	}


	kmb::nodeIdType* ary = NULL;
	ary = new kmb::nodeIdType[6];
	ary[0] = lowerID0;
	ary[1] = getLayerNodeId( upper.getCellId(0), lowerID0, 1);
	ary[2] = getLayerNodeId( upper.getCellId(1), lowerID0, 1);
	ary[3] = lowerID1;
	ary[4] = getLayerNodeId( upper.getCellId(2), lowerID1, 1);
	ary[5] = getLayerNodeId( upper.getCellId(3), lowerID1, 1);
	if( ary[1] >= 0 && ary[2] >= 0 && ary[4] >= 0 && ary[5] >= 0 ){
		this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
	}else{
		if( ary )	delete[] ary;
		return false;
	}
	for(int i=1;i<layerNum;++i){
		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[8];
		ary[0] = getLayerNodeId( upper.getCellId(0), lowerID0, i);
		ary[1] = getLayerNodeId( upper.getCellId(1), lowerID0, i);
		ary[2] = getLayerNodeId( upper.getCellId(2), lowerID1, i);
		ary[3] = getLayerNodeId( upper.getCellId(3), lowerID1, i);
		ary[4] = getLayerNodeId( upper.getCellId(0), lowerID0, i+1);
		ary[5] = getLayerNodeId( upper.getCellId(1), lowerID0, i+1);
		ary[6] = getLayerNodeId( upper.getCellId(2), lowerID1, i+1);
		ary[7] = getLayerNodeId( upper.getCellId(3), lowerID1, i+1);
		if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
			ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
			this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
		}else{
			if( ary )	delete[] ary;
			return false;
		}
	}
	return true;
}

bool kmb::BLMGenerator::
appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID0, kmb::nodeIdType upperID1, kmb::ElementBase &lower)
{
	if( this->meshDB == NULL || lower.getType() != kmb::QUAD ){
		return false;
	}

	for(int i=0;i<layerNum-1;++i){
		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[8];
		ary[0] = getLayerNodeId( upperID0, lower.getCellId(0),i);
		ary[1] = getLayerNodeId( upperID0, lower.getCellId(1),i);
		ary[2] = getLayerNodeId( upperID1, lower.getCellId(2),i);
		ary[3] = getLayerNodeId( upperID1, lower.getCellId(3),i);
		ary[4] = getLayerNodeId( upperID0, lower.getCellId(0),i+1);
		ary[5] = getLayerNodeId( upperID0, lower.getCellId(1),i+1);
		ary[6] = getLayerNodeId( upperID1, lower.getCellId(2),i+1);
		ary[7] = getLayerNodeId( upperID1, lower.getCellId(3),i+1);
		if( ary[0] >= 0 && ary[1] >= 0 && ary[2] >= 0 && ary[3] >= 0 &&
			ary[4] >= 0 && ary[5] >= 0 && ary[6] >= 0 && ary[7] >= 0){
			this->meshDB->insertElement(bodyID,kmb::HEXAHEDRON,ary);
		}else{
			if( ary )	delete[] ary;
			return false;
		}
	}

	kmb::nodeIdType* ary = NULL;
	ary = new kmb::nodeIdType[6];
	ary[0] = getLayerNodeId( upperID0, lower.getCellId(0),layerNum-1);
	ary[1] = getLayerNodeId( upperID0, lower.getCellId(1),layerNum-1);
	ary[2] = upperID0;
	ary[3] = getLayerNodeId( upperID1, lower.getCellId(2),layerNum-1);
	ary[4] = getLayerNodeId( upperID1, lower.getCellId(3),layerNum-1);
	ary[5] = upperID1;
	if( ary[0] >= 0 && ary[1] >= 0 && ary[3] >= 0 && ary[4] >= 0 ){
		this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
	}else{
		if( ary )	delete[] ary;
		return false;
	}
	return true;
}

bool kmb::BLMGenerator::
appendLayerElements( kmb::bodyIdType bodyID, std::vector<kmb::nodeIdType>& upperID0, std::vector<kmb::nodeIdType>& upperID1, kmb::nodeIdType lowerID0, kmb::nodeIdType lowerID1)
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


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = lowerID0;
		ary[1] = getLayerNodeId( upperID0[0], lowerID0, 1);
		ary[2] = getLayerNodeId( upperID1[1], lowerID1, 1);
		ary[3] = getLayerNodeId( upperID1[0], lowerID1, 1);
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
		ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = lowerID1;
		ary[1] = getLayerNodeId( upperID1[1], lowerID1, 1);
		ary[2] = getLayerNodeId( upperID1[0], lowerID1, 1);
		ary[3] = lowerID0;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
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


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = lowerID1;
		ary[1] = getLayerNodeId( upperID1[0], lowerID1, 1);
		ary[2] = getLayerNodeId( upperID0[1], lowerID0, 1);
		ary[3] = getLayerNodeId( upperID0[0], lowerID0, 1);
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
		ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = lowerID0;
		ary[1] = getLayerNodeId( upperID0[1], lowerID0, 1);
		ary[2] = getLayerNodeId( upperID0[0], lowerID0, 1);
		ary[3] = lowerID1;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
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


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[6];
		ary[0] = lowerID0;
		ary[1] = getLayerNodeId( upperID0[1], lowerID0, 1);
		ary[2] = getLayerNodeId( upperID0[0], lowerID0, 1);
		ary[3] = lowerID1;
		ary[4] = getLayerNodeId( upperID1[0], lowerID1, 1);
		ary[5] = getLayerNodeId( upperID1[1], lowerID1, 1);
		this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
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

bool kmb::BLMGenerator::
appendLayerElements( kmb::bodyIdType bodyID, kmb::nodeIdType upperID0, kmb::nodeIdType upperID1, std::vector<kmb::nodeIdType>& lowerID0, std::vector<kmb::nodeIdType>& lowerID1)
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


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = upperID0;
		ary[1] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1);
		ary[2] = getLayerNodeId( upperID1, lowerID1[1], layerNum-1);
		ary[3] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1);
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
		ary = new kmb::nodeIdType[4];
		ary[0] = upperID1;
		ary[1] = getLayerNodeId( upperID1, lowerID1[1], layerNum-1);
		ary[2] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1);
		ary[3] = upperID0;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
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


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1);
		ary[1] = getLayerNodeId( upperID0, lowerID0[1], layerNum-1);
		ary[2] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1);
		ary[3] = upperID1;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
		ary = NULL;
		ary = new kmb::nodeIdType[4];
		ary[0] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1);
		ary[1] = getLayerNodeId( upperID0, lowerID0[1], layerNum-1);
		ary[2] = upperID1;
		ary[3] = upperID0;
		this->meshDB->insertElement(bodyID,kmb::TETRAHEDRON,ary);
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


		kmb::nodeIdType* ary = NULL;
		ary = new kmb::nodeIdType[6];
		ary[0] = upperID0;
		ary[1] = getLayerNodeId( upperID0, lowerID0[1], layerNum-1 );
		ary[2] = getLayerNodeId( upperID0, lowerID0[0], layerNum-1 );
		ary[3] = upperID1;
		ary[4] = getLayerNodeId( upperID1, lowerID1[0], layerNum-1 );
		ary[5] = getLayerNodeId( upperID1, lowerID1[1], layerNum-1 );
		this->meshDB->insertElement(bodyID,kmb::WEDGE,ary);
	}
	return true;
}

bool
kmb::BLMGenerator::generate
(kmb::bodyIdType bodyID,kmb::bodyIdType &layerID,kmb::bodyIdType &boundaryID)
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

kmb::nodeIdType kmb::BLMGenerator::
getLayerNodeId(kmb::nodeIdType outerNodeID, kmb::nodeIdType innerNodeID, int layerIndex )
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

void kmb::BLMGenerator::
clearLayerNodes()
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
