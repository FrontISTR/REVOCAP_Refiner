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
#include "MeshDB/kmbMeshDB.h"

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbSegment2.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbTriangle2.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbQuad2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbTetrahedron2.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbHexahedron2.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbWedge2.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbPyramid2.h"
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbNodeNeighborFaceInfo.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbBodyOperation.h"

#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_Sphere.h"
#include "Geometry/kmb_Circle.h"
#include "Geometry/kmb_Point3DContainer.h"
#include "Geometry/kmb_Calculator.h"

#include "MeshDB/kmbTypes.h"
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <cstring>

#include <iostream>

void
kmb::MeshDB::updateBoundingBox(kmb::bodyIdType bodyId)
{
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body != NULL ){
		body->updateBoundingBox( this->getNodes() );
	}
}

const kmb::BoundingBox
kmb::MeshDB::getBoundingBox(kmb::bodyIdType bodyId) const
{
	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body != NULL ){
		return body->getBoundingBox();
	}
	return kmb::BoundingBox();
}

double
kmb::MeshDB::getAspectRatio(elementIdType elementId,kmb::bodyIdType bodyId)
{
	kmb::ElementContainer::iterator eIter = this->findElement(elementId,bodyId);
	return this->evaluator->getAspectRatio( eIter );
}







kmb::PhysicalValue*
kmb::MeshDB::getInterpolatedValue(const char* key,double x,double y,double z,double tolerance,const char* stype) const
{
	const kmb::DataBindings* bindings = this->getDataBindingsPtr(key,stype);
	if( bindings == NULL || bindings->getBindingMode() != kmb::DataBindings::NODEVARIABLE ){
		return NULL;
	}

	double maxCoeff[4] = {0.0,0.0,0.0,0.0};
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	kmb::ElementContainer::const_iterator nearestElement;
	kmb::bodyIdType bodyCount = this->getBodyCount();
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
		elementId = this->searchElement(bodyId,x,y,z,maxCoeff,tolerance);
		if( elementId != kmb::Element::nullElementId ){
			nearestElement = this->findElement( elementId, bodyId );
			break;
		}
	}
	if( !nearestElement.isFinished() ){
		const kmb::DataBindingsEach<kmb::nodeIdType>* nodeData
			= static_cast<const kmb::DataBindingsEach<kmb::nodeIdType>*>(bindings);
		return this->getInterpolatedValueInElement(nodeData,nearestElement,maxCoeff);
	}
	return NULL;
}




kmb::PhysicalValue*
kmb::MeshDB::getInterpolatedValueInData
(const char* key,const char* name,double x,double y,double z,double tolerance,const char* stype) const
{
	const kmb::DataBindings* bindings = this->getDataBindingsPtr(key,stype);
	if( bindings == NULL || bindings->getBindingMode() != kmb::DataBindings::NODEVARIABLE )
	{
		return NULL;
	}

	double maxCoeff[4] = {0.0,0.0,0.0,0.0};
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	kmb::ElementContainer::const_iterator nearestElement;
	elementId = this->searchElementInData(name,x,y,z,maxCoeff,tolerance);
	if( elementId != kmb::Element::nullElementId ){
		nearestElement = this->findElement( elementId, bindings->getTargetBodyId() );
	}
	if( !nearestElement.isFinished() ){
		const kmb::DataBindingsEach<kmb::nodeIdType>* nodeData
			= static_cast<const kmb::DataBindingsEach<kmb::nodeIdType>*>(bindings);
		return this->getInterpolatedValueInElement(nodeData,nearestElement,maxCoeff);
	}
	return NULL;
}

void
kmb::MeshDB::getInterpolatedMultiValues(std::vector<kmb::PhysicalValue*> &values,double x,double y,double z,double tolerance) const
{
	double maxCoeff[4] = {0.0,0.0,0.0,0.0};
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	kmb::ElementContainer::const_iterator nearestElement;
	kmb::bodyIdType bodyCount = this->getBodyCount();
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
		elementId = this->searchElement(bodyId,x,y,z,maxCoeff,tolerance);
		if( elementId != kmb::Element::nullElementId ){
			nearestElement = this->findElement( elementId, bodyId );
			break;
		}
	}
	if( !nearestElement.isFinished() ){
		std::vector< kmb::DataBindings* >::const_iterator dIter = this->bindingsStack.begin();
		std::vector< kmb::DataBindings* >::const_iterator dEnd = this->bindingsStack.end();
		while( dIter != dEnd ){
			kmb::DataBindings* bindings = *dIter;
			if( bindings == NULL || bindings->getBindingMode() != kmb::DataBindings::NODEVARIABLE )
			{
				values.push_back(NULL);
			}else{
				kmb::DataBindingsEach<kmb::nodeIdType>* nodeData
					= static_cast<kmb::DataBindingsEach<kmb::nodeIdType>*>(bindings);
				kmb::PhysicalValue* val = this->getInterpolatedValueInElement(nodeData,nearestElement,maxCoeff);
				if( val != NULL ){
					values.push_back( val );
				}else{
					values.push_back(NULL);
				}
			}
			++dIter;
		}
	}
}

void
kmb::MeshDB::getInterpolatedMultiValuesInData(std::vector<kmb::PhysicalValue*> &values,const char* name,double x,double y,double z,double tolerance,const char* stype) const
{
	const kmb::DataBindings* data = this->getDataBindingsPtr(name,stype);
	if( data == NULL ){
		return;
	}
	double maxCoeff[4] = {0.0,0.0,0.0,0.0};
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	elementId = this->searchElementInData(name,x,y,z,maxCoeff,tolerance);
	if( elementId != kmb::Element::nullElementId ){
		kmb::ElementContainer::const_iterator nearestElement = this->findElement( elementId, data->getTargetBodyId() );
		if( !nearestElement.isFinished() ){
			std::vector< kmb::DataBindings* >::const_iterator dIter = this->bindingsStack.begin();
			std::vector< kmb::DataBindings* >::const_iterator dEnd = this->bindingsStack.end();
			while( dIter != dEnd ){
				kmb::DataBindings* bindings = *dIter;
				if( bindings == NULL || bindings->getBindingMode() != kmb::DataBindings::NODEVARIABLE )
				{
					values.push_back(NULL);
				}else{
					kmb::DataBindingsEach<kmb::nodeIdType>* nodeData
						= static_cast<kmb::DataBindingsEach<kmb::nodeIdType>*>(bindings);
					kmb::PhysicalValue* val = this->getInterpolatedValueInElement(nodeData,nearestElement,maxCoeff);
					if( val != NULL ){
						values.push_back( val );
					}else{
						values.push_back(NULL);
					}
				}
				++dIter;
			}
		}
	}
}

kmb::PhysicalValue*
kmb::MeshDB::getInterpolatedValueInElement(const kmb::DataBindingsEach<kmb::nodeIdType>* bindings,kmb::ElementBase &element,double* coeff) const
{
	kmb::PhysicalValue* val = NULL;
	if( coeff == NULL || bindings == NULL )
	{
		return val;
	}
	switch (element.getType() )
	{
	case kmb::TETRAHEDRON:
	case kmb::TETRAHEDRON2:
		{
			const int len = 4;
			double r[4];
			kmb::Tetrahedron::shapeFunction( coeff[0], coeff[1], coeff[2], r );

			double nodeVal[4][3];
			for(int i=0;i<len;++i){
				kmb::nodeIdType nodeID = element.getCellId(i);
				if( !bindings->getPhysicalValue(nodeID,nodeVal[i]) ){
					return NULL;
				}
			}
			switch( bindings->getValueType() )
			{
			case kmb::PhysicalValue::SCALAR:
				{
					double d = 0.0;
					for(int i=0;i<len;++i)
					{
						d += r[i] * nodeVal[i][0];
					}
					val = new kmb::ScalarValue( d );
				}
				break;
			case kmb::PhysicalValue::VECTOR3:
				{
					double d[3] = {0.0,0.0,0.0};
					for(int i=0;i<len;++i)
					{
						d[0] += r[i] * nodeVal[i][0];
						d[1] += r[i] * nodeVal[i][1];
						d[2] += r[i] * nodeVal[i][2];
					}
					val = new kmb::Vector3Value( d );
				}
				break;
			default:
				break;
			}
			return val;
		}
		break;
	case kmb::HEXAHEDRON:
		{
			const int len = 8;
			double r[8];
			kmb::Hexahedron::shapeFunction( coeff[0], coeff[1], coeff[2], r );

			double nodeVal[8][3];
			for(int i=0;i<len;++i){
				kmb::nodeIdType nodeID = element.getCellId(i);
				if( !bindings->getPhysicalValue(nodeID,nodeVal[i]) ){
					return NULL;
				}
			}
			switch( bindings->getValueType() )
			{
			case kmb::PhysicalValue::SCALAR:
				{
					double d = 0.0;
					for(int i=0;i<len;++i)
					{
						d += r[i] * nodeVal[i][0];
					}
					val = new kmb::ScalarValue( d );
				}
				break;
			case kmb::PhysicalValue::VECTOR3:
				{
					double d[3] = {0.0,0.0,0.0};
					for(int i=0;i<len;++i)
					{
						d[0] += r[i] * nodeVal[i][0];
						d[1] += r[i] * nodeVal[i][1];
						d[2] += r[i] * nodeVal[i][2];
					}
					val = new kmb::Vector3Value( d );
				}
				break;
			default:
				break;
			}
			return val;
		}
		break;
	default:
		break;
	}
	return val;
}

int
kmb::MeshDB::getEdgeCountOfBody(kmb::bodyIdType bodyID)
{
	kmb::ElementContainer* body = this->getBodyPtr( bodyID );
	if( body ){
		kmb::BodyOperation bodyOp( this->getNodes() );
		return static_cast<int>( bodyOp.getEdgesOfBody( body ) );
	}else{
		return 0;
	}
}

int
kmb::MeshDB::getNodeCountOfBody(kmb::bodyIdType bodyID)
{
	kmb::ElementContainer* body = this->getBodyPtr( bodyID );
	if( body ){
		std::set< kmb::nodeIdType > nodes;
		body->getNodesOfBody( nodes );
		return static_cast<int>(nodes.size());
	}else{
		return 0;
	}
}

int
kmb::MeshDB::getNaturalCoordinates(kmb::bodyIdType bodyId,kmb::elementIdType elementId,double x,double y,double z,double* values,double margin)
{
	kmb::ElementContainer::iterator element = this->findElement(elementId,bodyId);
	if( !element.isFinished() && values != NULL ){
		switch( element.getType() ){
			case kmb::TETRAHEDRON:
			case kmb::TETRAHEDRON2:
			case kmb::HEXAHEDRON:
			case kmb::PYRAMID:
			case kmb::WEDGE:
				{
					if( evaluator->getNaturalCoordinates( element, x, y, z, values, margin ) ){
						return 3;
					}
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

bool
kmb::MeshDB::getPhysicalCoordinates(bodyIdType bodyId,elementIdType elementId,double s,double t,double u,double* values)
{
	kmb::ElementContainer::iterator element = this->findElement(elementId,bodyId);
	if( !element.isFinished() && values != NULL ){
		switch( element.getType() ){
			case kmb::TETRAHEDRON:
			case kmb::TETRAHEDRON2:
			case kmb::HEXAHEDRON:
			case kmb::PYRAMID:
			case kmb::WEDGE:
				{
					if( evaluator->getPhysicalCoordinates( element, s, t, u, values) ){
						return true;
					}
				}
				break;
			default:
				break;
		}
	}
	return false;
}

void
kmb::MeshDB::shapeFunction( kmb::elementType etype, double* naturalCoords, double* values )
{
	switch( etype )
	{
	case kmb::SEGMENT:
		kmb::Segment::shapeFunction(
			naturalCoords[0],
			values );
		break;
	case kmb::SEGMENT2:
		kmb::Segment2::shapeFunction(
			naturalCoords[0],
			values );
		break;
	case kmb::TRIANGLE:
		kmb::Triangle::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			values );
		break;
	case kmb::TRIANGLE2:
		kmb::Triangle2::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			values );
		break;
	case kmb::QUAD:
		kmb::Quad::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			values );
		break;
	case kmb::QUAD2:
		kmb::Quad2::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			values );
		break;
	case kmb::TETRAHEDRON:
		kmb::Tetrahedron::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	case kmb::TETRAHEDRON2:
		kmb::Tetrahedron2::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	case kmb::HEXAHEDRON:
		kmb::Hexahedron::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	case kmb::HEXAHEDRON2:
		kmb::Hexahedron2::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	case kmb::WEDGE:
		kmb::Wedge::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	case kmb::WEDGE2:
		kmb::Wedge2::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	case kmb::PYRAMID2:
		kmb::Pyramid2::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	case kmb::PYRAMID:
		kmb::Pyramid::shapeFunction(
			naturalCoords[0],
			naturalCoords[1],
			naturalCoords[2],
			values );
		break;
	default:
		break;
	}
}

bool
kmb::MeshDB::calcMeshProperty(const char* name,kmb::bodyIdType bodyId)
{
	if( strcmp(name,"NormalVectorOnNode")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		if( body && body->isUniqueDim(2) ){
			kmb::NodeNeighborInfo neighborInfo;
			neighborInfo.appendCoboundary( body );
			std::vector< kmb::elementIdType > elements;
			kmb::DataBindingsEach< kmb::nodeIdType >* data = reinterpret_cast< kmb::DataBindingsEach< kmb::nodeIdType >* >(
				this->createDataBindings("NormalVectorOnNode",kmb::DataBindings::NODEVARIABLE,kmb::PhysicalValue::VECTOR3,"MeshProperty") );
			kmb::Point3DContainer::iterator nIter = nodes->begin();
			while( !nIter.isFinished() ){
				elements.clear();
				neighborInfo.getSurroundingElements( nIter.getId(), elements );
				if( elements.size() > 0 ){
					kmb::Vector3D normal(0.0,0.0,0.0);
					std::vector<kmb::elementIdType >::iterator eIter = elements.begin();
					while( eIter != elements.end() ){
						kmb::ElementContainer::iterator element = body->find( *eIter );
						kmb::Vector3D v(0.0,0.0,0.0);
						this->evaluator->getNormalVector( element, v );
						normal += v;
						++eIter;
					}
					normal.normalize();
					data->setPhysicalValue( nIter.getId(), new kmb::Vector3Value( normal.x(), normal.y(), normal.z() ) );
				}
				++nIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"AspectRatio")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body && body->isUniqueDim(3) ){
			kmb::DataBindings* data = this->getDataBindingsPtr("AspectRatio","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("AspectRatio",kmb::DataBindings::ELEMENTVARIABLE,kmb::PhysicalValue::SCALAR,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
				data->getValueType() != kmb::PhysicalValue::SCALAR )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double ratio = 0.0;
			while( !eIter.isFinished() ){
				ratio = evaluator.getAspectRatio( eIter );
				data->setPhysicalValue( eIter.getId(), &ratio );
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"ElementVolume")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body && body->isUniqueDim(3) ){
			kmb::DataBindings* data = this->getDataBindingsPtr("ElementVolume","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("ElementVolume",kmb::DataBindings::ELEMENTVARIABLE,kmb::PhysicalValue::SCALAR,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
				data->getValueType() != kmb::PhysicalValue::SCALAR )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double vol = 0.0;
			while( !eIter.isFinished() ){
				vol = evaluator.getVolume( eIter );
				data->setPhysicalValue( eIter.getId(), &vol );
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"MaxEdgeLength")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body ){
			kmb::DataBindings* data = this->getDataBindingsPtr("MaxEdgeLength","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("MaxEdgeLength",kmb::DataBindings::ELEMENTVARIABLE,kmb::PhysicalValue::SCALAR,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
				data->getValueType() != kmb::PhysicalValue::SCALAR )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double vol = 0.0;
			while( !eIter.isFinished() ){
				vol = evaluator.getMaxEdgeLength( eIter );
				data->setPhysicalValue( eIter.getId(), &vol );
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"AverageEdgeLength")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body ){
			kmb::DataBindings* data = this->getDataBindingsPtr("AverageEdgeLength","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("AverageEdgeLength",kmb::DataBindings::ELEMENTVARIABLE,kmb::PhysicalValue::SCALAR,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
				data->getValueType() != kmb::PhysicalValue::SCALAR )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double vol = 0.0;
			while( !eIter.isFinished() ){
				vol = evaluator.getAverageEdgeLength( eIter );
				data->setPhysicalValue( eIter.getId(), &vol );
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"EdgeLengthRatio")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body ){
			kmb::DataBindings* data = this->getDataBindingsPtr("EdgeLengthRatio","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("EdgeLengthRatio",kmb::DataBindings::ELEMENTVARIABLE,kmb::PhysicalValue::SCALAR,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
				data->getValueType() != kmb::PhysicalValue::SCALAR )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double vol = 0.0;
			while( !eIter.isFinished() ){
				vol = evaluator.getEdgeLengthRatio( eIter );
				data->setPhysicalValue( eIter.getId(), &vol );
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"MinAngle")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body && body->getDimension() >= 2 ){
			kmb::DataBindings* data = this->getDataBindingsPtr("MinAngle","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("MinAngle",kmb::DataBindings::ELEMENTVARIABLE,kmb::PhysicalValue::SCALAR,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
				data->getValueType() != kmb::PhysicalValue::SCALAR )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double ang = 0.0;
			while( !eIter.isFinished() ){
				ang = evaluator.getMinAngle( eIter );
				data->setPhysicalValue( eIter.getId(), &ang );
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"MaxAngle")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body && body->getDimension() >= 2 ){
			kmb::DataBindings* data = this->getDataBindingsPtr("MaxAngle","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("MaxAngle",kmb::DataBindings::ELEMENTVARIABLE,kmb::PhysicalValue::SCALAR,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
				data->getValueType() != kmb::PhysicalValue::SCALAR )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double ang = 0.0;
			while( !eIter.isFinished() ){
				ang = evaluator.getMaxAngle( eIter );
				data->setPhysicalValue( eIter.getId(), &ang );
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"ConcaveElement")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body && body->getDimension() >= 2 ){
			kmb::DataBindings* data = this->getDataBindingsPtr("ConcaveElement","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("ConcaveElement",kmb::DataBindings::ELEMENTGROUP,kmb::PhysicalValue::NONE,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ELEMENTGROUP ||
				data->getValueType() != kmb::PhysicalValue::NONE )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				if( evaluator.getConcaveInQuad( eIter ) != -1 ){
					data->addId( eIter.getId() );
				}
				++eIter;
			}
			return true;
		}
	}
	return false;
}



double
kmb::MeshDB::getElementArea(kmb::elementIdType triId,kmb::bodyIdType bodyId) const
{
	kmb::ElementContainer::const_iterator element = this->findElement(triId,bodyId);
	if( !element.isFinished() && element.getDimension() == 2 && evaluator ){
		return this->evaluator->getArea( element );
	}else{
		return 0.0;
	}
}

double
kmb::MeshDB::getElementVolume(kmb::elementIdType elementId,kmb::bodyIdType bodyId) const
{
	kmb::ElementContainer::const_iterator element = this->findElement(elementId,bodyId);
	if( !element.isFinished() && element.getDimension() == 3 && evaluator ){
		return this->evaluator->getVolume( element );
	}else{
		return 0.0;
	}
}


double
kmb::MeshDB::getArea(kmb::bodyIdType bodyId) const
{
	double area = 0.0;

	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body && body->isUniqueDim(2) ){
		const kmb::DataBindings* data = this->getDataBindingsPtr("ElementArea","MeshProperty");
		if( data != NULL &&
			data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE &&
			data->getValueType() != kmb::PhysicalValue::SCALAR )
		{
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				double v = 0.0;
				data->getPhysicalValue( eIter.getId(), &v );
				area += v;
				++eIter;
			}
		}else{
			const kmb::Point3DContainer* nodes = this->getNodes();
			if( nodes ){
				kmb::ElementEvaluator evaluator(nodes);
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( !eIter.isFinished() ){
					area += evaluator.getArea( eIter );
					++eIter;
				}
			}
		}
	}
	return area;
}

double
kmb::MeshDB::getVolume(kmb::bodyIdType bodyId) const
{
	double vol = 0.0;

	const kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body && body->isUniqueDim(3) ){
		const kmb::DataBindings* data = this->getDataBindingsPtr("ElementVolume","MeshProperty");
		if( data != NULL &&
			data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE &&
			data->getValueType() != kmb::PhysicalValue::SCALAR )
		{
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				double v = 0.0;
				data->getPhysicalValue( eIter.getId(), &v );
				vol += v;
				++eIter;
			}
		}else{
			const kmb::Point3DContainer* nodes = this->getNodes();
			if( nodes ){
				kmb::ElementEvaluator evaluator(nodes);
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( !eIter.isFinished() ){
					vol += evaluator.getVolume( eIter );
					++eIter;
				}
			}
		}
	}
	return vol;
}
