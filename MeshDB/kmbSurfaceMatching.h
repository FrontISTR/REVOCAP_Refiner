/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Matching                                                #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/*
 * �ړI
 * Surface �� FaceGroup �̏��Ԃƌ����܂ō��߂��}�b�`���O
 * Surface �ő���Ȃ��Ƃ���̓_�~�[�̖ʂ�ǉ����邱�Ƃ��\
 * ����
 * Surface �� bodyId
 * FaceGroup
 * �o��
 * FaceGroup �� face �� Surface �� elementId �̊Ԃ̑Ή�
 */

#pragma once

#include <string>
#include <map>
#include "MeshDB/kmbTypes.h"

namespace kmb{

class MeshData;
class ElementContainer;
class DataBindings;
class NodeNeighborFaceInfo;
class Face;
class Collision;
class Matrix;

class SurfaceMatching
{
private:
	kmb::MeshData* mesh;
	kmb::bodyIdType masterId;
	kmb::ElementContainer* masterSurf;
	std::string slaveName;
	kmb::DataBindings* slaveFaceGroup;
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
public:
	SurfaceMatching(void);
	virtual ~SurfaceMatching(void);
	void setMesh(kmb::MeshData* mesh,const char* insNodes=NULL);
	void setPair(kmb::bodyIdType bodyId,const char* faceGroup);
	kmb::elementIdType getMatchingElementId(kmb::Face f,int &index);



	size_t constructDummyElements(void);
private:
	void clear(void);

	void calcMapping(void);



	kmb::elementIdType appendDummyElement(kmb::Face f);


	kmb::nodeIdType duplicateNode(kmb::nodeIdType n0);
};

}
