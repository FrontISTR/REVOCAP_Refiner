/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshRefiner                                             #
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




#pragma once

#include "MeshDB/kmbElement.h"
#include <vector>
#include <string>

namespace kmb{

class MeshData;
class DataBindings;
class ElementContainer;
class MiddleNodeManager;

class MeshRefiner
{
public:
	enum interpolationRule{
		MAX,
		MIN,
		MIDDLE
	};
protected:
	struct DataPair{
		std::string name;
		std::string stype;
		const kmb::DataBindings* originalData;
		kmb::DataBindings* refinedData;
	};
	kmb::MeshData* mesh;
	kmb::MiddleNodeManager* middleMan;
	bool middleManDeletable;




	std::vector< kmb::MeshRefiner::DataPair > dataPairs;

	void setSecondNodes( kmb::ElementBase& element );
	double distanceSqBetweenNodes( kmb::nodeIdType n0, kmb::nodeIdType n1 );
	interpolationRule interpRule;

	bool secondFitting;
public:
	MeshRefiner(void);
	virtual ~MeshRefiner(void);
	void clear(void);
	void setMesh(kmb::MeshData* mesh);
	void setMiddleNodeManager(kmb::MiddleNodeManager* manager);

	void setSecondNodesByData(const char* name,const char* stype=NULL);

	void setSecondNodesByBody(kmb::bodyIdType bodyId);

	kmb::bodyIdType convertToSecondBody(kmb::bodyIdType bodyId);

	size_t getRefinedElementCount(const kmb::ElementContainer* orgBody) const;


	kmb::bodyIdType refineBody(kmb::bodyIdType orgBodyId);





	size_t refineBody(const kmb::ElementContainer* orgBody, kmb::ElementContainer* refinedBody);

	kmb::elementType getOriginal(kmb::nodeIdType middleNodeId, kmb::nodeIdType* originalNodes) const;

	kmb::nodeIdType getMiddle(kmb::ElementBase &element) const;

	void appendData(const char* dataname,const char* stype=NULL);
	size_t getDataCount(void) const;

	void clearData(void);







	void commitData(void);
	void setInterpolationMode(const char* mode);
	const char* getInterpolationMode(void) const;
	void setSecondFitting(bool flag);
	bool setSecondFitting(void) const;
private:


	void refineSegment( kmb::elementIdType elementId, kmb::ElementBase &segment, kmb::ElementContainer* refinedBody );
	void refineSegment2( kmb::elementIdType elementId, kmb::ElementBase &segment2, kmb::ElementContainer* refinedBody );
	void refineTriangle( kmb::elementIdType elementId, kmb::ElementBase &triangle, kmb::ElementContainer* refinedBody );
	void refineTriangle2( kmb::elementIdType elementId, kmb::ElementBase &triangle2, kmb::ElementContainer* refinedBody );
	void refineQuad( kmb::elementIdType elementId, kmb::ElementBase &quad, kmb::ElementContainer* refinedBody );
	void refineQuad2( kmb::elementIdType elementId, kmb::ElementBase &quad2, kmb::ElementContainer* refinedBody );
	void refineTetrahedron( kmb::elementIdType elementId, kmb::ElementBase &tetra, kmb::ElementContainer* refinedBody );
	void refineTetrahedron2( kmb::elementIdType elementId, kmb::ElementBase &tetra2, kmb::ElementContainer* refinedBody );
	void refineHexahedron( kmb::elementIdType elementId, kmb::ElementBase &hexa, kmb::ElementContainer* refinedBody );
	void refineHexahedron2( kmb::elementIdType elementId, kmb::ElementBase &hexa2, kmb::ElementContainer* refinedBody );
	void refineWedge( kmb::elementIdType elementId, kmb::ElementBase &wedge, kmb::ElementContainer* refinedBody );
	void refineWedge2( kmb::elementIdType elementId, kmb::ElementBase &wedge2, kmb::ElementContainer* refinedBody );
	void refinePyramid( kmb::elementIdType elementId, kmb::ElementBase &pyramid, kmb::ElementContainer* refinedBody );
	void refinePyramid2( kmb::elementIdType elementId, kmb::ElementBase &pyramid2, kmb::ElementContainer* refinedBody );

	bool nodeGroupUpdate( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n01, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeGroupUpdate( kmb::ElementBase &elem, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeGroupUpdate( kmb::ElementBase &elem, int faceIndex, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);

	bool nodeVariableUpdate( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n01, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeVariableUpdate( kmb::ElementBase &elem, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeVariableUpdate( kmb::ElementBase &elem, int faceIndex, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
};

}
