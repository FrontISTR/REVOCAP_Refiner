/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : SurfaceOperation                                        #
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
#include <vector>
#include <string>
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "Common/kmbClassification.h"

namespace kmb{

class MeshData;
class Face;
class ElementContainer;
class NodeNeighborInfo;
class Point3DContainer;
class DataBindings;

class SurfaceOperation
{
protected:
	kmb::MeshData* mesh;
	kmb::ElementContainer* elements;
	kmb::NodeNeighborInfo* neighborInfo;
	kmb::Classification< kmb::elementIdType > dividedElementsInfo;
	std::set< std::pair<kmb::elementIdType, kmb::elementIdType> > edgesBetweenElements;
	kmb::Classification< kmb::Face > dividedFacesInfo;
	std::set< std::pair<kmb::Face,kmb::Face> > edgesBetweenFaces;

	bool permitReverse;
public:
	SurfaceOperation(void);
	virtual ~SurfaceOperation(void);
	void setMesh(kmb::MeshData* mesh);
	void setPermitReverse(bool f);










	void divideSurface(bodyIdType bodyId,double angle,std::vector<kmb::bodyIdType> &surfIDs);
	void divideSurfaceWithRidge(bodyIdType bodyId,double angle,std::map<kmb::bodyIdType,kmb::bodyIdType> &surfIDs);









	kmb::bodyIdType divideFaceGroup(std::string name,double angle,std::vector<std::string> &subFaceGroups);
	kmb::bodyIdType divideFaceGroupWithRidge(std::string name,double angle,std::map<std::string,kmb::bodyIdType> &subFaceGroupMapper);



	void decomposeSurface(kmb::bodyIdType bodyID, std::set< kmb::bodyIdType > &components);

	bool decomposeFaceGroup(std::string faceGroup, std::vector<std::string> &subFaceGroups);


	void decomposeByElementContainer(kmb::ElementContainer* elements,std::set< kmb::ElementContainer* > &components);
	void clear(void);

	void setSurface(kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	enum vertexType{
		Unknown,
		CONVEX,
		CONCAVE,
		FLAT,
		HYPERBOLIC
	};


	static vertexType getVertexType
		(kmb::nodeIdType nodeId,
		 kmb::MeshData* mesh,
		 kmb::ElementContainer* elements,
		 kmb::NodeNeighborInfo* neighborInfo);
	vertexType getVertexType(kmb::nodeIdType nodeId) const;

	kmb::bodyIdType faceGroup2Surface(const char* name);
	bool surface2FaceGroup(kmb::bodyIdType bodyId,kmb::bodyIdType parentBodyId,const char* name);


	bool surface2FaceGroup(kmb::bodyIdType bodyId,const char* parentFaceGroup,const char* name);
protected:


	size_t divideSurface(const kmb::ElementContainer* elements,const kmb::Point3DContainer* points,double angle);


	bool getDividedSurfaceElementCount(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,size_t &triCount,size_t &quadCount) const;

	size_t getDividedSurface(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,kmb::ElementContainer* dividedSurf) const;
	size_t getTotalRidge(const kmb::ElementContainer* org,kmb::ElementContainer* ridges) const;
	size_t getSurfaceRidge(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,kmb::ElementContainer* ridges) const;


	size_t divideFaceGroup(const kmb::DataBindings* data, const kmb::ElementContainer* elements, const kmb::Point3DContainer* points, double angle);
	size_t divideComponentsFaceGroup(const kmb::DataBindings* data, const kmb::ElementContainer* elements);

	size_t getDividedFaceGroup(kmb::Face initFace,kmb::DataBindings* data) const;
	size_t getSurfaceRidge(const kmb::ElementContainer* elements,kmb::Face initFace,kmb::ElementContainer* ridges) const;
};

}
