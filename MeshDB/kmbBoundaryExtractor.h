/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : BoundaryExtractor                                       #
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
#pragma once

#include <map>
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"

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
public:
	BoundaryExtractor(void);
	virtual ~BoundaryExtractor(void);
	void setMesh(kmb::MeshData* mesh);
	void appendBody(kmb::bodyIdType bodyId);
	void appendData(const char* name);
	void eraseData(const char* name,bool reverse=false);


	kmb::bodyIdType getBoundary(kmb::bodyIdType bodyId);
	kmb::bodyIdType getBoundary(void);

	size_t getBoundary(const kmb::ElementContainer* parent,kmb::ElementContainer* boundary);

	int getBoundaryComponents(kmb::bodyIdType bodyId,kmb::bodyIdType* &boundaryIds);


	bool getBoundaryFace(kmb::bodyIdType bodyId,const char* name);


	bool isClosed(kmb::bodyIdType bodyId);

	void clear(void);

	void appendBody(kmb::ElementContainer* body);

	void appendUnitBody(const kmb::ElementContainer* body);
	void appendData(const kmb::DataBindings* data);
	void eraseData(const kmb::DataBindings* data,bool reverse);
	void setReverseMode(bool mode);
	bool getReverseMode(void) const;
};

}
