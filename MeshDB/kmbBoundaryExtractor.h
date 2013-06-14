/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BoundaryExtractor                                       #
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
#pragma once

#include <map>
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbElementRelation.h"

namespace kmb{

class MeshData;
class Face;
class ElementContainer;
class DataBindings;

class BoundaryExtractor
{
protected:
	kmb::MeshData* mesh;

	bool reverseMode;

	std::multimap< kmb::nodeIdType, kmb::Face > facemap;


	void appendElement(kmb::elementIdType elementId,kmb::ElementBase &element,const kmb::ElementContainer* elements=NULL);

	void appendFaceMap(kmb::elementIdType elementId,kmb::ElementBase &element,kmb::idType faceId,const kmb::ElementContainer* elements=NULL);




	void eraseFaceMap(kmb::ElementBase &element,kmb::idType faceId,bool reverse=false,const kmb::ElementContainer* elements=NULL);

	void appendElementContainer(const kmb::ElementContainer* body);

	kmb::ElementRelation::relationType getFaceRelation(kmb::ElementBase &e0,kmb::idType f0,kmb::ElementBase &e1,kmb::idType f1) const;
public:
	BoundaryExtractor(void);
	virtual ~BoundaryExtractor(void);
	void setMesh(kmb::MeshData* mesh);
	void appendBody(kmb::bodyIdType bodyId);

	void appendUnitBody(const kmb::ElementContainer* body);


	void appendData(const char* name);
	void appendData(const kmb::DataBindings* data);
	void eraseData(const char* name,bool reverse=false);



	kmb::bodyIdType getBoundary(kmb::bodyIdType bodyId) const;

	kmb::bodyIdType getBoundary(void) const;

	kmb::bodyIdType getBoundary(kmb::BoundaryExtractor& other) const;


	size_t getBoundary(const kmb::ElementContainer* parent,kmb::ElementContainer* boundary) const;


	size_t getBoundaryNodeGroup(const kmb::ElementContainer* parent,kmb::DataBindings* nodeGroup) const;

	int getBoundaryComponents(kmb::bodyIdType bodyId,kmb::bodyIdType* &boundaryIds) const;


	bool getBoundaryFace(kmb::bodyIdType bodyId,const char* name) const;



	bool isClosed(kmb::bodyIdType bodyId) const;

	bool isClosed(void) const;

	void clear(void);
	void eraseData(const kmb::DataBindings* data,bool reverse);
	void setReverseMode(bool mode);
	bool getReverseMode(void) const;
};

}
