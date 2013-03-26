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
#include "MeshDB/kmbNodeEvaluator.h"

#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbSphere.h"
#include "Geometry/kmbCircle.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Common/kmbCalculator.h"

#include "MeshDB/kmbTypes.h"

#include <cstring>

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
kmb::MeshDB::getAspectRatio(elementIdType elementId,kmb::bodyIdType bodyId) const
{
	kmb::ElementContainer::const_iterator eIter = this->findElement(elementId,bodyId);
	return this->evaluator->getAspectRatio( eIter );
}

int
kmb::MeshDB::getEdgeCountOfBody(kmb::bodyIdType bodyId) const
{
	const kmb::ElementContainer* body = this->getBodyPtr( bodyId );
	if( body ){
		kmb::BodyOperation bodyOp( this->getNodes() );
		return static_cast<int>( bodyOp.getEdgesOfBody( body ) );
	}else{
		return 0;
	}
}

int
kmb::MeshDB::getNodeCountOfBody(kmb::bodyIdType bodyId) const
{
	const kmb::ElementContainer* body = this->getBodyPtr( bodyId );
	if( body ){
		std::set< kmb::nodeIdType > nodes;
		body->getNodesOfBody( nodes );
		return static_cast<int>(nodes.size());
	}else{
		return 0;
	}
}

int
kmb::MeshDB::getNaturalCoordinates(kmb::bodyIdType bodyId,kmb::elementIdType elementId,double x,double y,double z,double* values) const
{
	kmb::ElementContainer::const_iterator element = this->findElement(elementId,bodyId);
	if( !element.isFinished() && values != NULL ){
		switch( element.getType() ){
			case kmb::TETRAHEDRON:
			case kmb::TETRAHEDRON2:
			case kmb::HEXAHEDRON:
			case kmb::HEXAHEDRON2:
			case kmb::PYRAMID:
			case kmb::WEDGE:
				{
					if( evaluator->getNaturalCoordinates( element, x, y, z, values ) ){
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
kmb::MeshDB::getPhysicalCoordinates(bodyIdType bodyId,elementIdType elementId,double s,double t,double u,kmb::Point3D &target) const
{
	kmb::ElementContainer::const_iterator element = this->findElement(elementId,bodyId);
	if( !element.isFinished() ){
		switch( element.getType() ){
			case kmb::TETRAHEDRON:
			case kmb::TETRAHEDRON2:
			case kmb::HEXAHEDRON:
			case kmb::PYRAMID:
			case kmb::WEDGE:
				{
					if( evaluator->getPhysicalCoordinates( element, s, t, u, target) ){
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
			kmb::DataBindings* data = this->createDataBindings("NormalVectorOnNode",kmb::DataBindings::NodeVariable,kmb::PhysicalValue::Vector3,"MeshProperty");
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
				data = this->createDataBindings("AspectRatio",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
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
				data = this->createDataBindings("ElementVolume",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
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
				data = this->createDataBindings("MaxEdgeLength",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
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
				data = this->createDataBindings("AverageEdgeLength",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
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
				data = this->createDataBindings("EdgeLengthRatio",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
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
				data = this->createDataBindings("MinAngle",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
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
				data = this->createDataBindings("MaxAngle",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
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
				data = this->createDataBindings("ConcaveElement",kmb::DataBindings::ElementGroup,kmb::PhysicalValue::None,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementGroup ||
				data->getValueType() != kmb::PhysicalValue::None )
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
	else if( strcmp(name,"MinJacobian")==0 ){
		kmb::ElementContainer* body = this->getBodyPtr(bodyId);
		kmb::Point3DContainer* nodes = this->getNodes();
		kmb::ElementEvaluator evaluator(nodes);
		if( body && body->getDimension() >= 2 ){
			kmb::DataBindings* data = this->getDataBindingsPtr("MinJacobian","MeshProperty");
			if( data == NULL ){
				data = this->createDataBindings("MinJacobian",kmb::DataBindings::ElementVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
			}
			if( data == NULL ||
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar )
			{
				return false;
			}
			kmb::ElementContainer::iterator eIter = body->begin();
			double min=0.0, max=0.0;
			while( !eIter.isFinished() ){
				if( evaluator.getMinMaxJacobian( eIter, min, max ) ){
					data->setValue( eIter.getId(), min );
				}
				++eIter;
			}
			return true;
		}
	}
	else if( strcmp(name,"Curvature")==0 ){
		kmb::NodeEvaluator nevaluator;
		nevaluator.setMesh(this);
		this->createDataBindings("Curvature",kmb::DataBindings::NodeVariable,kmb::PhysicalValue::Scalar,"MeshProperty");
		nevaluator.calcCurvature("Curvature","MeshProperty");
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
			data->getBindingMode() != kmb::DataBindings::ElementVariable &&
			data->getValueType() != kmb::PhysicalValue::Scalar )
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
			data->getBindingMode() != kmb::DataBindings::ElementVariable &&
			data->getValueType() != kmb::PhysicalValue::Scalar )
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



kmb::nodeIdType
kmb::MeshDB::getCornerNodeIdOfSurface(kmb::bodyIdType bodyId,kmb::Vector3D dir) const
{
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	if( dir.isZero() ){
		dir.setCoordinate(1.0, 1.0, 1.0);
	}
	kmb::Minimizer minimizer;
	kmb::Point3D point;
	const kmb::ElementContainer* elements = this->getBodyPtr(bodyId);
	if( elements && elements->getDimension() == 2 ){
		for( kmb::ElementContainer::const_iterator eIter = elements->begin();
			!eIter.isFinished(); ++eIter)
		{
			switch( eIter.getType() )
			{
			case kmb::TRIANGLE:
			case kmb::TRIANGLE2:
				for(int i=0;i<3;++i){
					if( getNode( eIter[i], point ) &&
						minimizer.update( dir.x()*point.x() + dir.y()*point.y() + dir.z()*point.z() ) ){
						nodeId = eIter[i];
					}
				}
				break;
			case kmb::QUAD:
			case kmb::QUAD2:
				for(int i=0;i<4;++i){
					if( getNode( eIter[i], point ) &&
						minimizer.update( dir.x()*point.x() + dir.y()*point.y() + dir.z()*point.z() ) ){
						nodeId = eIter[i];
					}
				}
				break;
			default:
				break;
			}
		}
	}
	return nodeId;
}

kmb::nodeIdType
kmb::MeshDB::getCornerNodeIdOfFaceGroup(const char* faceGroup,kmb::Vector3D dir) const
{
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	if( dir.isZero() ){
		dir.setCoordinate(1.0, 1.0, 1.0);
	}
	kmb::Minimizer minimizer;
	kmb::Point3D point;
	const kmb::DataBindings* data = this->getDataBindingsPtr(faceGroup);
	const kmb::ElementContainer* elements = NULL;
	kmb::Face f;
	if( data &&
		( data->getBindingMode() == kmb::DataBindings::FaceGroup || data->getBindingMode() == kmb::DataBindings::FaceVariable ) &&
		( elements = this->getBodyPtr( data->getTargetBodyId() ) ) != NULL )
	{
		for( kmb::DataBindings::const_iterator fIter = data->begin();
			!fIter.isFinished(); ++fIter )
		{
			if( fIter.getFace(f) ){
				kmb::ElementContainer::const_iterator eIter = elements->find( f.getElementId() );
				kmb::idType i = f.getLocalFaceId();
				switch( eIter.getBoundaryType(i) )
				{
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					for(int j=0;j<3;++j){
						if( getNode( eIter.getBoundaryCellId(i,j), point ) &&
							minimizer.update( dir.x()*point.x() + dir.y()*point.y() + dir.z()*point.z() ) ){
							nodeId = eIter.getBoundaryCellId(i,j);
						}
					}
					break;
				case kmb::QUAD:
				case kmb::QUAD2:
					for(int j=0;j<4;++j){
						if( getNode( eIter.getBoundaryCellId(i,j), point ) &&
							minimizer.update( dir.x()*point.x() + dir.y()*point.y() + dir.z()*point.z() ) ){
							nodeId = eIter.getBoundaryCellId(i,j);
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}
	return nodeId;
}
