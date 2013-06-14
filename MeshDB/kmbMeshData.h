/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshData                                                #
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

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbPhysicalValue.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementOctree.h"

#include "Geometry/kmbBoundingBox.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbOctree.h"
#include "Common/kmbCalculator.h"

#include <vector>
#include <string>

namespace kmb{

class Matrix4x4;

typedef std::multimap< std::string, kmb::DataBindings* > datamap;

class MeshData
{
protected:
	kmb::Point3DContainer* node3Ds;
	std::vector< kmb::ElementContainer* > bodies;
	kmb::ElementContainer* currentBody;
	std::multimap< std::string, kmb::DataBindings* > bindings;

	std::vector< kmb::DataBindings* > bindingsStack;
	int bindingsStackDim;
	std::string defaultSpecType;

	elementIdType maxElementId;

	kmb::Matrix4x4* coordMatrix;
public:
	MeshData(void);
	virtual ~MeshData(void);

	virtual void clearModel(void);



	virtual void beginModel(void);



	virtual void endModel(void);


	void setCoordinate( const kmb::Matrix4x4& matrix );
	void translateCoordinate( double x, double y, double z );
	kmb::Matrix4x4* getCoordMatrix(void);
	const kmb::Matrix4x4* getCoordMatrix(void) const;

	int getNodeDim(void) const;


	virtual void beginNode(size_t size=0,const char* containerType=NULL);
	virtual void beginNode(size_t size,kmb::Point3DContainer* point3Ds);

	virtual kmb::nodeIdType addNode(double x,double y,double z);

	virtual kmb::nodeIdType addNodeWithId(double x,double y,double z, kmb::nodeIdType id);



	virtual void endNode(void);



	kmb::nodeIdType getMaxNodeId(void) const;
	kmb::nodeIdType getMinNodeId(void) const;


	const kmb::Point3DContainer* getNodes(void) const;
	kmb::Point3DContainer* getNodes(void);
	virtual const kmb::Point2DContainer* getNode2Ds(void) const{ return NULL; };
	kmb::Point3DContainer* replaceNodes(kmb::Point3DContainer* nodes);

	virtual size_t getNodeCount(void) const;

	virtual bool getNode(nodeIdType i,kmb::Point3D &node) const;
	bool getNodeXYZ(nodeIdType i,double &x,double &y,double &z) const;


	virtual kmb::nodeIdType insertNode(double x,double y,double z);


	virtual kmb::nodeIdType insertNodeWithId(double x,double y,double z, kmb::nodeIdType id);

	virtual bool updateNode(double x,double y,double z, kmb::nodeIdType id);

	const char* getNodeContainerType(void) const;
	virtual const kmb::BoundingBox getBoundingBox(void) const;




	virtual kmb::bodyIdType beginElement(size_t size,kmb::ElementContainer* container);
	virtual kmb::bodyIdType beginElement(size_t size=0,const char* containerType=NULL);

	virtual kmb::elementIdType addElement(kmb::elementType type,kmb::nodeIdType *ary);

	virtual kmb::elementIdType addElementWithId(kmb::elementType type,kmb::nodeIdType *ary,kmb::elementIdType elementId);

	virtual void endElement(void);

	kmb::elementIdType getMaxElementId(void) const;

	virtual kmb::elementIdType insertElement(kmb::bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary);
	virtual kmb::elementIdType insertElementWithId(kmb::bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary,kmb::elementIdType elementId);

	virtual bool moveElement(kmb::bodyIdType bodyId0,kmb::bodyIdType bodyId1,kmb::elementIdType elementId);

	kmb::bodyIdType getBodyCount(void) const;

	bodyIdType getBodyId(elementIdType elementId) const;


	virtual kmb::bodyIdType appendBody( kmb::Body* body, bool offset=true );
	virtual kmb::bodyIdType replaceBody( kmb::bodyIdType bodyId, kmb::Body* body, bool offset=true );



	virtual bool updateBody( kmb::bodyIdType bodyId );

	virtual void clearBody(bodyIdType bodyId);

	virtual void removeAllBodies(void);

	kmb::Body* getBodyPtr(bodyIdType bodyId);
	const kmb::Body* getBodyPtr(bodyIdType bodyId) const;

	size_t getElementCount(kmb::bodyIdType bodyId) const;
	size_t getElementCount(void) const;
	size_t getElementCountOfDim(int dim) const;

	int getDimension(kmb::bodyIdType bodyId) const;

	int getDegree(kmb::bodyIdType bodyId) const;

	bool isUniqueElementType(kmb::bodyIdType bodyId,kmb::elementType etype) const;

	kmb::ElementContainer::iterator findElement(elementIdType elementID,bodyIdType bodyID=kmb::Body::nullBodyId);
	kmb::ElementContainer::const_iterator findElement(elementIdType elementID,bodyIdType bodyID=kmb::Body::nullBodyId) const;

	virtual const char* getBodyName(bodyIdType bodyId) const;
	virtual void setBodyName(bodyIdType bodyId,const char* name);
	kmb::bodyIdType getBodyIdByName(const char* name) const;

	const char* getElementContainerType(bodyIdType bodyId) const;

	kmb::bodyIdType importBody(const kmb::MeshData& otherMesh,kmb::bodyIdType bodyId);



	virtual const std::multimap< std::string, kmb::DataBindings* >& getDataBindingsMap(void) const;
	kmb::datamap::iterator beginDataIterator();
	kmb::datamap::const_iterator beginDataIterator() const;
	kmb::datamap::iterator endDataIterator();
	kmb::datamap::const_iterator endDataIterator() const;

	virtual kmb::DataBindings* createDataBindings(
		const char* name,
		kmb::DataBindings::bindingMode bmode,
		kmb::PhysicalValue::valueType vtype,
		const char* stype="",
		kmb::bodyIdType targetBodyId=kmb::Body::nullBodyId);

	kmb::DataBindings* getDataBindingsPtr(const char* name,const char* stype=NULL);
	const kmb::DataBindings* getDataBindingsPtr(const char* name,const char* stype=NULL) const;

	bool setDataBindingsPtr(const char* name,kmb::DataBindings* data,const char* stype=NULL);

	virtual size_t getDataCount(const char* stype=NULL) const;
	bool hasData(const char* name,const char* stype=NULL) const;
	bool renameData(const char* oldname,const char* newname,const char* stype=NULL);
	bool deleteData(const char* name,const char* stype=NULL);
	bool clearData(const char* name,const char* stype=NULL);



	bool replaceData(const kmb::DataBindings* olddata, kmb::DataBindings* newdata, const char* name,const char* stype=NULL);

	size_t getIdCount(const char* name,const char* stype=NULL) const;

	void setDefaultSpecType(const char* specType);

	kmb::DataBindings::bindingMode getDataMode(const char* name, const char* stype=NULL) const;

	kmb::PhysicalValue::valueType getDataValueType(const char* name, const char* stype=NULL) const;

	std::string getDataSpecType(const char* name) const;

	bool setDataSpecType(const char* name,const char* oldstype,const char* newstype);

	const char* getDataContainerType(const char* name,const char* stype=NULL) const;

	virtual void addId(const char* name,kmb::idType id,const char* stype=NULL);
	virtual void addId(const char* name,kmb::Face f,const char* stype=NULL);


	virtual void setPhysicalValue(const char* name,kmb::PhysicalValue* val,const char* stype=NULL);
	virtual void setPhysicalValueAtId(const char* name,kmb::idType id,kmb::PhysicalValue* val,const char* stype=NULL);
	virtual void setPhysicalValueAtId(const char* name,kmb::Face f,kmb::PhysicalValue* val,const char* stype=NULL);

	virtual const kmb::PhysicalValue* getPhysicalValue(const char* name,const char* stype=NULL) const;
	virtual const kmb::PhysicalValue* getPhysicalValueAtId(const char* name,kmb::idType id,const char* stype=NULL) const;
	virtual const kmb::PhysicalValue* getPhysicalValueAtId(const char* name,kmb::Face f,const char* stype=NULL) const;
	virtual bool getPhysicalValueAtId(const char* name,kmb::idType id,double *val,const char* stype=NULL) const;
	virtual bool getPhysicalValueAtId(const char* name,kmb::Face f,double *val,const char* stype=NULL) const;

	void appendTargetData(const char* name, const char* stype=NULL);
	void appendTargetDataPtr(kmb::DataBindings* data);
	void clearTargetData(void);
	int getTargetDataNum(void) const;
	int getTargetDataDim(void) const;
	virtual void setMultiPhysicalValues(kmb::idType id, std::vector< kmb::PhysicalValue* > &values);
	virtual void setMultiPhysicalValues(kmb::idType id, double* values);
	virtual void setMultiPhysicalValues(kmb::idType id, long* values);
	virtual void setMultiPhysicalValues(double* values);
	virtual void getMultiPhysicalValues(kmb::idType id, double* values) const;

	std::string getUniqueDataName(std::string prefix,int num=0);


	void deriveTargetData(kmb::elementIdType elementId,kmb::elementIdType orgElementId);

	kmb::bodyIdType faceGroupToBody(const char* faceG,const char* stype=NULL);

	kmb::bodyIdType faceVariableToBody(const char* faceV,const char* elemV,const char* stype=NULL);

	size_t faceGroupToBody(const char* faceG,kmb::ElementContainer* body, const char* stype=NULL);
	size_t faceVariableToBody(const char* faceV,kmb::ElementContainer* body,const char* stype=NULL);

	void getNodeSetFromDataBindings(std::set<kmb::nodeIdType>&nodeSet,const char* name,const char* stype=NULL) const;

	void getBoundingBoxOfData(kmb::BoundingBox &bbox,const kmb::DataBindings* data) const;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif



	virtual kmb::elementIdType searchElement(kmb::bodyIdType bodyId,double x,double y,double z,double tolerance=0.0) const{ return kmb::Element::nullElementId; };



	virtual int getValueOnNearestNode(const char* name,double x,double y,double z,double *values,const char* stype=NULL) const { return -1; };

	virtual int getInterpolatedValueInBody(const char* name,kmb::bodyIdType bodyId,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const { return -1; };
	virtual int getInterpolatedValue(const char* name,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const { return -1; };
	virtual int getInterpolatedValueInData(const char* name,const char* target,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const{ return -1; };



	virtual int getInterpolatedValuesInBody(kmb::bodyIdType bodyId,double x,double y,double z,double *values) const { return -1; };
	virtual int getInterpolatedValues(double x,double y,double z,double *values) const { return -1; };
	virtual int getInterpolatedValuesInData(const char* target,double x,double y,double z,double *values,double tolerance=0.0) const{ return -1;};

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

};

}
