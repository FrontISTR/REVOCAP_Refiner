/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshDB                                                  #
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

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbPhysicalValue.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementOctree.h"
#include "MeshDB/kmbElementBucket.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbMeshData.h"


#include "Geometry/kmbBoundingBox.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbOctree.h"

#include <vector>
#include <map>
#include <set>
#include <string>

namespace kmb{

enum coupleType{
	NONE,
	MASTER,
	SLAVE,
	BOTH
};

class Vector3D;
class Triangle;
class Tetrahedron;
class FramedPlane;
class ElementEvaluator;
class DataEvaluator;
class MeshOperation;

class MeshDB : public MeshData
{
private:
	std::string defaultNodeContainerType;
	std::string defaultElementContainerType;
	std::string defaultDataContainerType;

	kmb::Point2DContainer* node2Ds;

	kmb::Element3DOctree elementOctree;
	kmb::ElementBucket elementBucket;
	kmb::OctreePoint3D nodeOctree;
	kmb::ElementEvaluator *evaluator;
	kmb::DataEvaluator *dataEvaluator;
	kmb::MeshOperation* meshOperation;
public:

	kmb::NodeNeighborInfo neighborInfo;
public:
	MeshDB(void);
	virtual ~MeshDB(void);
	virtual void clearModel(void);
public:

	void setDefaultNodeContainerType(const char* nodeContainerType);
	const char* getDefaultNodeContainerType(void) const;
	void setDefaultElementContainerType(const char* elementContainerType);
	const char* getDefaultElementContainerType(void) const;




	void setDefaultDataContainerType(const char* dataContainerType);
	const char* getDefaultDataContainerType(void) const;




	void setDefaultContainerType(const char* containerType);
public:

	virtual const kmb::Point2DContainer* getNode2Ds(void) const{ return node2Ds; };

	virtual void beginNode(size_t size,kmb::Point2DContainer* point2Ds);
	virtual void beginNode(size_t size,kmb::Point3DContainer* point3Ds);

	virtual void beginNode(size_t size=0,const char* containerType=NULL);

	virtual nodeIdType addNode(double x,double y,double z);
	nodeIdType addNode(const Point3D& point);
	virtual nodeIdType addNodeWithId(double x,double y,double z, kmb::nodeIdType id);
	nodeIdType addNodeWithId(const Point3D& point, kmb::nodeIdType id);
	virtual size_t getNodeCount(void) const;
	bool deleteNode(kmb::nodeIdType id);
	void clearNodes(void);
	void convertAllNodes( kmb::Matrix4x4& mat );
	void translateAllNodes( double x,double y,double z );
	void scaleAllNodes( double r );

	const kmb::Point3D* getMaxPointPtr(void) const;
	const kmb::Point3D* getMinPointPtr(void) const;
	virtual const kmb::BoundingBox getBoundingBox(void) const;
	void updateBoundingBox(void);



	bool replaceNodeId(kmb::nodeIdType oldId, kmb::nodeIdType newId);
	kmb::Point3DContainerMap::idContinuityType getNodeContinuity() const;
	void nodeIdDefragmentation(nodeIdType initId=0);

	size_t uniteNodes(double thresh);
	int deleteUselessNodes(void);

	nodeIdType insertNode(const Point3D& point);
	nodeIdType duplicatePoint(nodeIdType nodeId);

	const char* getNodeContainerType(void) const;
public:

	double getNearestNode(const kmb::nodeIdType id,kmb::nodeIdType& nearestId) const;
	double getNearestNodeInBody(double x,double y,double z,kmb::bodyIdType bodyId,kmb::nodeIdType& nearestId) const;
	double getNearestNodeInBody(const kmb::Point3D& point,kmb::bodyIdType bodyId,kmb::nodeIdType& nearestId) const;
	double getNearestNodeInBody(const kmb::nodeIdType id,kmb::bodyIdType bodyId,kmb::nodeIdType& nearestId) const;
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

public:

	void updateBoundingBox(bodyIdType bodyId);
	const kmb::BoundingBox getBoundingBox(bodyIdType bodyId) const;


	virtual void endModel(void);

	virtual void removeAllBodies(void);




	int duplicateNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName=NULL);


	int replaceNodeIdOfBody(kmb::bodyIdType bodyId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId);
	int replaceNodeIdOfBody(kmb::bodyIdType bodyId,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper);
	int replaceNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName);

	void reverseBody(kmb::bodyIdType bodyID);
	bool elementToFace( const kmb::ElementBase &element, kmb::Face &face, kmb::bodyIdType bodyId=kmb::Body::nullBodyId );

public:


	size_t generateElementSearchCache( kmb::bodyIdType bodyId, const char* cacheType="octree" );
	void clearElementSearchCache(void);
	kmb::elementIdType searchElement(kmb::bodyIdType bodyId,double x,double y,double z,double tolerance=0.0) const;
	kmb::elementIdType searchElementInData(const char* name,double x,double y,double z,double tolerance=0.0) const;





	size_t generateNodeSearchCache(void);
	size_t generateNodeSearchCacheOfBody( kmb::bodyIdType bodyId );
	size_t generateNodeSearchCacheOfData( const char* name, const char* stype=NULL );
	void appendNodeSearchCache(kmb::nodeIdType nodeId);
	void clearNodeSearchCache(void);
	double getNearestNode(double x,double y,double z,kmb::nodeIdType &nearestId) const;
	void getNodesInRegion(const kmb::Region* region,std::set<kmb::nodeIdType> &selectedNodes) const;
public:


	void getSurroundingElements(nodeIdType nodeId,std::vector<elementIdType>& elements,bool cacheOnly=true) const;
public:

	kmb::Vector3D getNormalVector(kmb::elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	kmb::Vector3D getNormalVector(kmb::Face face,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	kmb::Vector3D getEdgeVector(nodeIdType i0,nodeIdType i1) const;
	kmb::Vector3D getAreaVectorOfSurface(kmb::bodyIdType bodyId) const;
	kmb::Vector3D getAreaVectorOfFaceGroup(const char* faceGroup) const;


	kmb::nodeIdType getCornerNodeIdOfSurface(kmb::bodyIdType bodyId,kmb::Vector3D dir) const;
	kmb::nodeIdType getCornerNodeIdOfFaceGroup(const char* faceGroup,kmb::Vector3D dir) const;

	double getElementArea(elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	double getElementVolume(elementIdType elementID,kmb::bodyIdType bodyID=kmb::Body::nullBodyId) const;
	double getArea(kmb::bodyIdType bodyId) const;
	double getVolume(kmb::bodyIdType bodyId) const;

	double getDistance(kmb::nodeIdType nodeId0, kmb::nodeIdType nodeId1) const;
	double getDistanceToElement(double x,double y,double z,kmb::elementIdType elementId0,kmb::bodyIdType bodyId=kmb::Body::nullBodyId) const;



	double getAspectRatio(elementIdType elementId,kmb::bodyIdType bodyId=kmb::Body::nullBodyId) const;

	int getNaturalCoordinates(bodyIdType bodyId,elementIdType elementId,double x,double y,double z,double* values) const;
	bool getPhysicalCoordinates(bodyIdType bodyId,elementIdType elementId,double s,double t,double u,kmb::Point3D &target) const;
	static void shapeFunction( kmb::elementType etype, double* naturalCoords, double* values );

	int getEdgeCountOfBody(kmb::bodyIdType bodyId) const;
	int getNodeCountOfBody(kmb::bodyIdType bodyId) const;





	kmb::bodyIdType cloneBody(bodyIdType bodyID);
	kmb::bodyIdType cloneReverseBody(bodyIdType bodyID);
	kmb::bodyIdType cloneBodyWithNode(bodyIdType bodyID);
	kmb::bodyIdType cloneReverseBodyWithNode(bodyIdType bodyID);




	kmb::bodyIdType mergeBody(const std::set< kmb::bodyIdType >& bodyIDs);
	kmb::bodyIdType mergeBody( kmb::bodyIdType appendBodyId, kmb::bodyIdType bodyId, bool moveFlag=true);
	kmb::bodyIdType convertToLinearBody( kmb::bodyIdType bodyId );






	kmb::bodyIdType importBody(kmb::MeshData& otherMesh,kmb::bodyIdType bodyId, const char* coupleName=NULL,kmb::coupleType ctype=kmb::SLAVE);
	kmb::bodyIdType importBodyWithNodeMatching(kmb::MeshData& otherMesh, kmb::bodyIdType bodyId, double tolerance, const char* coupleName=NULL);
	kmb::bodyIdType importBody(kmb::MeshData& otherMesh,kmb::bodyIdType bodyId, std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper);

	size_t importAllBody(const kmb::MeshData& otherMesh);



	kmb::nodeIdType importNode(kmb::MeshDB& otherMesh);






	void triangulation(bodyIdType bodyID);
	bodyIdType polygonPartition(bodyIdType polygonID,kmb::FramedPlane &plane);

public:

	kmb::DataBindings* createDataBindings(const char* name,kmb::DataBindings::bindingMode bmode,PhysicalValue::valueType vtype,const char* stype="",kmb::bodyIdType targetBodyId=kmb::Body::nullBodyId);
	size_t getElementCountOfData(const char* name,const char* stype=NULL) const;
	size_t getNodeCountOfData(const char* name,const char* stype=NULL) const;

	bool replaceIdOfData(const char* name,kmb::idType oldID,kmb::idType newID,const char* stype=NULL);



	void getNearestValue(std::string key,const kmb::PhysicalValue* value,kmb::idType& id) const;
	void getNearestValue(std::string key,const kmb::PhysicalValue* value,kmb::Face& f) const;

	const kmb::BoundingBox getBoundingBoxOfData(const char* key,const char* stype=NULL) const;
	void getBoundingBoxOfData(kmb::BoundingBox &bbox,const kmb::DataBindings* data) const;


	bool getMinMaxValue(const kmb::DataBindings* data,kmb::BoundingBox1D &bbox) const;
	bool getMinMaxValue(const kmb::DataBindings* data,kmb::BoundingBox &bbox) const;
	bool getMinMaxValueWithId(const kmb::DataBindings* data,kmb::MinMaxWithId<kmb::idType>& minmax,int comp=-1) const;


	virtual int getValueOnNearestNode(const char* name,double x,double y,double z,double *values,const char* stype=NULL) const;

	int getInterpolatedValue(const char* name,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const;
	int getInterpolatedValueInBody(const char* name,kmb::bodyIdType bodyId,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const;

	int getInterpolatedValueInData(const char* name,const char* target,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const;


	int getInterpolatedValues(double x,double y,double z,double *values,double tolerance=0.0) const;
	int getInterpolatedValuesInBody(kmb::bodyIdType bodyId,double x,double y,double z,double *values,double tolerance=0.0) const;

	int getInterpolatedValuesInData(const char* target,double x,double y,double z,double *values,double tolerance=0.0) const;



	int convertData(const char* org, const char* conv, const char* orgstype=NULL,const char* convstype=NULL);
	int convertData(const kmb::DataBindings* orgData, kmb::DataBindings* convData);


	int convertBodyToData(kmb::bodyIdType bodyId, const char* name,const char* stype=NULL);
public:












	bool calcMeshProperty(const char* name,kmb::bodyIdType bodyId=kmb::Body::nullBodyId);
};

}

