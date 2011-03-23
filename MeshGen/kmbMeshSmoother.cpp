/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : MeshSmoother                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshGen/kmbMeshSmoother.h"

#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbBoundaryExtractor.h"

kmb::MeshSmoother::MeshSmoother(kmb::MeshData* mesh,kmb::bodyIdType bodyId)
: points(NULL)
, elements(NULL)
, boundary(NULL)
, evaluator(NULL)
, neighborInfo(NULL)
, neighborInfoSurf(NULL)
, shiftPoints(NULL)
{
	if( mesh ){
		this->points = mesh->getNodes();
		this->elements = mesh->getBodyPtr(bodyId);
		this->evaluator = new kmb::ElementEvaluator( this->points );
		this->neighborInfo = new kmb::NodeNeighborInfo();
		this->neighborInfo->appendCoboundary( this->elements );
	}
}

kmb::MeshSmoother::~MeshSmoother(void)
{
	if( this->evaluator ){
		delete this->evaluator;
		this->evaluator = NULL;
	}
	if( this->neighborInfo ){
		delete this->neighborInfo;
		this->neighborInfo = NULL;
	}
	if( this->neighborInfoSurf ){
		delete this->neighborInfoSurf;
		this->neighborInfoSurf = NULL;
	}
	if( this->boundary ){
		delete this->boundary;
		this->boundary = NULL;
	}
	if( this->shiftPoints ){
		delete this->shiftPoints;
		this->shiftPoints = NULL;
	}
}

void
kmb::MeshSmoother::init(void)
{
	if( shiftPoints ){
		delete shiftPoints;
		shiftPoints = NULL;
	}
	shiftPoints = new kmb::Point3DContainerMap();
}

void
kmb::MeshSmoother::smoothingAtNode(kmb::nodeIdType nodeId)
{
	if( neighborInfo && elements && shiftPoints && points ){
		kmb::Point3D point(0.0,0.0,0.0);
		double x=0.0,y=0.0,z=0.0;
		std::set< kmb::nodeIdType > neighbors;
		neighborInfo->getNodeNeighbor(nodeId,neighbors,elements);
		std::set< kmb::nodeIdType >::iterator nIter = neighbors.begin();
		while( nIter != neighbors.end() ){
			if( points->getXYZ( *nIter, x, y, z ) ){
				point.addCoordinate(x,y,z);
			}
			++nIter;
		}
		if( neighbors.size() > 0 ){
			point.scale( 1.0/neighbors.size() );
			shiftPoints->addPoint( point, nodeId );
		}
	}
}

void
kmb::MeshSmoother::smoothingOnFaceAtNode(kmb::nodeIdType nodeId)
{
	if( elements && shiftPoints && points ){

		if( neighborInfoSurf == NULL ){
			kmb::BoundaryExtractor bext;
			bext.appendUnitBody( this->elements );
			boundary = new kmb::ElementContainerMap();
			bext.getBoundary( this->elements, boundary );
			neighborInfoSurf = new kmb::NodeNeighborInfo();
			neighborInfoSurf->appendCoboundary( boundary );
		}
		kmb::Point3D point(0.0,0.0,0.0);
		double x=0.0,y=0.0,z=0.0;
		std::set< kmb::nodeIdType > neighbors;
		neighborInfoSurf->getNodeNeighbor(nodeId,neighbors,boundary);
		std::set< kmb::nodeIdType >::iterator nIter = neighbors.begin();
		while( nIter != neighbors.end() ){
			if( points->getXYZ( *nIter, x, y, z ) ){
				point.addCoordinate(x,y,z);
			}
			++nIter;
		}
		if( neighbors.size() > 0 ){
			point.scale( 1.0/neighbors.size() );
			shiftPoints->addPoint( point, nodeId );
		}
	}
}

void
kmb::MeshSmoother::commit(void)
{
	if( shiftPoints && points ){
		double x=0.0, y=0.0, z=0.0;
		kmb::Point3DContainer::iterator pIter = shiftPoints->begin();
		while( !pIter.isFinished() ){
			pIter.getXYZ( x, y, z );
			points->updateCoordinate( pIter.getId(), x, y, z );
			++pIter;
		}
	}
}
