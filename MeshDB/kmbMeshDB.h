/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : MeshDB                                                  #
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

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbPhysicalValue.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementOctree.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbMeshData.h"


#include "Geometry/kmb_BoundingBox.h"
#include "Geometry/kmb_Point3DContainer.h"
#include "Geometry/kmb_Point2DContainer.h"
#include "Geometry/kmb_Point3DOctree.h"

#include <vector>
#include <map>
#include <set>
#include <string>

namespace kmb{

class Vector3D;
class Triangle;
class Tetrahedron;
class FramedPlane;
class ElementEvaluator;
class DataEvaluator;

class MeshDB : public MeshData
{
public:
	MeshDB(void);
	virtual ~MeshDB(void);

public:
	virtual void clearModel(void);
	void setDefaultNodeContainerType(const char* nodeContainerType);
	const char* getDefaultNodeContainerType(void) const;
	void setDefaultElementContainerType(const char* elementContainerType);
	const char* getDefaultElementContainerType(void) const;




	void setDefaultDataContainerType(const char* dataContainerType);
	const char* getDefaultDataContainerType(void) const;




	void setDefaultContainerType(const char* containerType);
private:
	std::string defaultNodeContainerType;
	std::string defaultElementContainerType;
	std::string defaultDataContainerType;


private:
	kmb::Point2DContainer* node2Ds;
public:
	int getNodeDim(void);
	virtual const kmb::Point2DContainer* getNode2Ds(void) const{	return node2Ds; };

	virtual void beginNode(size_t size,kmb::Point2DContainer* point2Ds);
	virtual void beginNode(size_t size,kmb::Point3DContainer* point3Ds);

	virtual void beginNode(size_t size=0,const char* containerType=NULL);

	virtual nodeIdType addNode(double x,double y,double z);
	nodeIdType addNode(const Point3D& point);
	virtual nodeIdType addNodeWithId(double x,double y,double z, kmb::nodeIdType id);
	nodeIdType addNodeWithId(const Point3D& point, kmb::nodeIdType id);
	virtual size_t getNodeCount(void) const;
	kmb::nodeIdType getMaxNodeId(void) const;
	kmb::nodeIdType getMinNodeId(void) const;
	bool deleteNode(kmb::nodeIdType id);
	void clearNodes(void);
	void convertAllNodes( kmb::Matrix4x4& mat );
	void translateAllNodes( double x,double y,double z );
	void scaleAllNodes( double r );

	const kmb::Point3D* getMaxPointPtr(void) const;
	const kmb::Point3D* getMinPointPtr(void) const;
	virtual const kmb::BoundingBox getBoundingBox(void) const;
	void updateBoundingBox(void);



	bool replaceNodeId(kmb::nodeIdType oldID, kmb::nodeIdType newID);
	kmb::Point3DContainerMap::idContinuityType getNodeContinuity() const;
	void nodeIdDefragmentation(nodeIdType initId=0);
	int deleteUselessNodes(void);

	nodeIdType insertNode(const Point3D& point);
	nodeIdType duplicatePoint(nodeIdType nodeId);

	const char* getNodeContainerType(void) const;
public:

	double getNearestNode(const kmb::nodeIdType id,kmb::nodeIdType& nearestId);
	double getNearestNodeInBody(double x,double y,double z,kmb::bodyIdType bodyID,kmb::nodeIdType& nearestId);
	double getNearestNodeInBody(const kmb::Point3D& point,kmb::bodyIdType bodyID,kmb::nodeIdType& nearestId);
	double getNearestNodeInBody(const kmb::nodeIdType id,kmb::bodyIdType bodyID,kmb::nodeIdType& nearestId);

public:
	virtual kmb::bodyIdType beginElement(size_t size,kmb::ElementContainer* container);
	virtual kmb::bodyIdType beginElement(size_t size=0,const char* containerType=NULL);
	kmb::elementIdType addElement(kmb::elementType type,kmb::nodeIdType *ary);
	kmb::elementIdType addElementWithId(kmb::elementType type,kmb::nodeIdType *ary,elementIdType elementId);

	kmb::elementIdType insertElement(bodyIdType bodyId,kmb::elementType type,kmb::nodeIdType *ary);
	kmb::elementIdType insertElementWithId(bodyIdType bodyId,kmb::elementType type,kmb::nodeIdType *ary,elementIdType elementId);

	kmb::elementIdType deleteElement(bodyIdType bodyId,elementIdType elementId);


	kmb::elementIdType generateElementId(void);


	bool replaceNodeIdOfElement(kmb::bodyIdType bodyId,kmb::elementIdType elementId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId);
	bool reverseElement(kmb::elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId);
	kmb::ElementContainer::iterator findElement(elementIdType elementID,bodyIdType bodyID=kmb::Body::nullBodyId);
	kmb::ElementContainer::const_iterator findElement(elementIdType elementID,bodyIdType bodyID=kmb::Body::nullBodyId) const;


public:
	bodyIdType getBodyId(elementIdType elementID) const;
	void updateBoundingBox(bodyIdType bodyId);
	const kmb::BoundingBox getBoundingBox(bodyIdType bodyId) const;


	virtual void endModel(void);






	virtual void removeAllBodies(void);
	bool replaceNodeIdOfBody(kmb::bodyIdType bodyId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId);
	bool replaceNodeIdOfBody(kmb::bodyIdType bodyId,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper);
	void reverseBody(kmb::bodyIdType bodyID);
	bool elementToFace( const kmb::ElementBase &element, kmb::Face &face, kmb::bodyIdType bodyId=kmb::Body::nullBodyId );

public:
	size_t generateElementSearchCache( kmb::bodyIdType bodyId );
	void clearElementSearchCache(void);
	kmb::elementIdType searchElement(kmb::bodyIdType bodyId,double x,double y,double z,double* coeff,double tolerance=0.0) const;
	kmb::elementIdType searchElementWithNormal(kmb::bodyIdType bodyId,double x,double y,double z,kmb::Vector3D &normal,double* coeff,double tolerance=0.0) const;
	kmb::elementIdType searchElementInData(const char* name,double x,double y,double z,double* coeff,double tolerance=0.0,const char* stype=NULL) const;

	size_t generateNodeSearchCache(void);
	size_t generateNodeSearchCacheOfBody( kmb::bodyIdType bodyId );
	size_t generateNodeSearchCacheOfData( const char* name, const char* stype=NULL );
	void appendNodeSearchCache(kmb::nodeIdType nodeId);
	void clearNodeSearchCache(void);
	double getNearestNode(double x,double y,double z,kmb::nodeIdType &nearestId);
	void getNodesInRegion(kmb::Region* region,std::set<kmb::nodeIdType> &selectedNodes);

private:
	kmb::ElementOctree elementOctree;
	kmb::Point3DOctree nodeOctree;
	kmb::ElementEvaluator *evaluator;
	kmb::DataEvaluator *dataEvaluator;
public:



	kmb::NodeNeighborInfo neighborInfo;

	void getSurroundingElements(nodeIdType nodeId,std::vector<elementIdType>& elements,bool cacheOnly=true);
public:

	kmb::Vector3D getNormalVector(kmb::elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	kmb::Vector3D getNormalVector(kmb::Face face,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	kmb::Vector3D getEdgeVector(nodeIdType i0,nodeIdType i1) const;
	kmb::Vector3D getAreaVectorOfSurface(kmb::bodyIdType bodyId) const;
	kmb::Vector3D getAreaVectorOfFaceGroup(const char* faceGroup) const;

	double getElementArea(elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	double getElementVolume(elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	double getArea(kmb::bodyIdType bodyId) const;
	double getVolume(kmb::bodyIdType bodyId) const;

	double getDistance(kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1) const;



	double getAspectRatio(elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId);


	int getNaturalCoordinates(bodyIdType bodyId,elementIdType elementId,double x,double y,double z,double* values,double alpha=1.0);
	bool getPhysicalCoordinates(bodyIdType bodyId,elementIdType elementId,double s,double t,double u,double* values);
	static void shapeFunction( kmb::elementType etype, double* naturalCoords, double* values );

	int getEdgeCountOfBody(kmb::bodyIdType bodyID);
	int getNodeCountOfBody(kmb::bodyIdType bodyID);





	kmb::bodyIdType cloneBody(bodyIdType bodyID);
	kmb::bodyIdType cloneReverseBody(bodyIdType bodyID);
	kmb::bodyIdType cloneBodyWithNode(bodyIdType bodyID);
	kmb::bodyIdType cloneReverseBodyWithNode(bodyIdType bodyID);




	kmb::bodyIdType mergeBody(const std::set< kmb::bodyIdType >& bodyIDs);
	kmb::bodyIdType mergeBody( kmb::bodyIdType appendBodyId, kmb::bodyIdType bodyId, bool moveFlag=true);
	kmb::bodyIdType convertToLinearBody( kmb::bodyIdType bodyId );






	kmb::bodyIdType importBody(kmb::MeshDB& otherMesh,kmb::bodyIdType bodyId, const char* coupleName=NULL);
	kmb::bodyIdType importBodyWithNodeMatching(kmb::MeshDB& otherMesh, kmb::bodyIdType bodyId, double tolerance, const char* coupleName=NULL);
	kmb::bodyIdType importBody(kmb::MeshDB& otherMesh,kmb::bodyIdType bodyId, std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper);



	kmb::nodeIdType importNode(kmb::MeshDB& otherMesh);






	void triangulation(bodyIdType bodyID);
	bodyIdType polygonPartition(bodyIdType polygonID,kmb::FramedPlane &plane);


public:
	kmb::DataBindings*	createDataBindings(const char* name,kmb::DataBindings::bindingMode bmode,PhysicalValue::valueType vtype,const char* stype="",kmb::bodyIdType targetBodyId=kmb::Body::nullBodyId);
	size_t getElementCountOfData(const char* name,const char* stype=NULL) const;
	bool replaceIdOfData(const char* name,kmb::idType oldID,kmb::idType newID,const char* stype=NULL);


	void getNearestValue(std::string key,kmb::PhysicalValue* value,kmb::idType& id);
	void getNearestValue(std::string key,kmb::PhysicalValue* value,kmb::Face& f);

	void getBoundingBox(kmb::BoundingBox &bbox,const kmb::DataBindings* data) const;
	const kmb::BoundingBox getBoundingBoxOfData(const char* key,const char* stype=NULL) const;

	bool getMinMaxValue(const kmb::DataBindings* data,kmb::BoundingBox1D &bbox) const;
	bool getMinMaxValue(const kmb::DataBindings* data,kmb::BoundingBox &bbox) const;
	bool getMinMaxValueWithId(const kmb::DataBindings* data,kmb::MinMaxWithId& minmax,int comp=-1) const;

	PhysicalValue* getInterpolatedValue(const char* key,double x,double y,double z,double tolerance=0.0,const char* stype=NULL) const;
	PhysicalValue* getInterpolatedValueInData(const char* key,const char* name,double x,double y,double z,double tolerance=0.0,const char* stype=NULL) const;
	void getInterpolatedMultiValues(std::vector<kmb::PhysicalValue*> &values,double x,double y,double z,double tolerance=0.0) const;
	void getInterpolatedMultiValuesInData(std::vector<kmb::PhysicalValue*> &values,const char* name,double x,double y,double z,double tolerance=0.0,const char* stype=NULL) const;
	PhysicalValue* getInterpolatedValueInElement(const kmb::DataBindingsEach<kmb::nodeIdType>* bindings,kmb::ElementBase &element,double* coeff) const;

	int convertData(const char* org, const char* conv, const char* orgstype=NULL,const char* convstype=NULL);
private:

public:

	std::string getUniqueDataName(std::string prefix,int num=0);
public:











	bool calcMeshProperty(const char* name,kmb::bodyIdType bodyId=kmb::Body::nullBodyId);
};

}

