/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matching                                                #
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
/*
 * 目的
 * Surface と FaceGroup の順番と向きまで込めたマッチング
 * Surface で足りないところはダミーの面を追加することも可能
 * 入力
 * Surface の bodyId
 * FaceGroup
 * 出力
 * FaceGroup の face と Surface の elementId の間の対応
 */

#pragma once

#include <string>
#include <map>
#include <set>
#include "MeshDB/kmbTypes.h"

namespace kmb{

class MeshData;
class ElementContainer;
class DataBindings;
class NodeNeighborFaceInfo;
class Face;
class Collision;
class Matrix;
class Permutation;

class SurfaceMatching
{
private:
	kmb::MeshData* mesh;
	kmb::bodyIdType masterId;
	kmb::ElementContainer* masterSurf;
	std::string slaveName;
	kmb::DataBindings* slaveFaceGroup;
	kmb::ElementContainer* slaveElements;

	kmb::NodeNeighborFaceInfo* neighborInfo;
	kmb::Matrix* distanceMatrix;
	kmb::elementIdType* elementIds;
	kmb::Face* faces;
	kmb::Collision* collision;
	kmb::DataBindings* insertedNodes;
	struct rotatedElement{
		kmb::elementIdType elementId;
		int index;
	};
	std::map< kmb::Face, rotatedElement > mapping;

	std::set< std::pair< int, int > > connectionTable;
public:
	SurfaceMatching(void);
	virtual ~SurfaceMatching(void);
	void setMesh(kmb::MeshData* mesh,const char* insNodes=NULL);

	void setPair(kmb::bodyIdType bodyId,const char* faceGroup);
	kmb::elementIdType getMatchingElementId(kmb::Face f,int &index);



	size_t constructDummyElements(void);
private:
	void clear(void);
	void clearPairInfo(void);

	bool calcMapping(void);



	kmb::elementIdType appendDummyElement(kmb::Face f);


	kmb::nodeIdType duplicateNode(kmb::nodeIdType n0);




	int checkTopologicalMapping( kmb::Permutation& perm ) const;
};

}
