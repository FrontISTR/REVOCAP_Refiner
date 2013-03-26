/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshSmoother                                            #
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
#include "MeshGen/kmbMeshSmoother.h"

#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbBoundaryExtractor.h"
#include "MeshDB/kmbDataBindings.h"

typedef std::multimap< kmb::nodeIdType, kmb::nodeIdType > NodeConnectTable;


kmb::MeshSmoother::MeshSmoother(kmb::MeshData* mesh)
: mesh(NULL)
, shiftPoints(NULL)
, nodeConnectTable()
, relax(0.0)
{
	this->mesh = mesh;
}

kmb::MeshSmoother::~MeshSmoother(void)
{
	if( this->shiftPoints ){
		delete this->shiftPoints;
		this->shiftPoints = NULL;
	}
}

void
kmb::MeshSmoother::clear(void)
{
	nodeConnectTable.clear();
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

bool
kmb::MeshSmoother::appendConnectNode(kmb::nodeIdType n0,kmb::nodeIdType n1)
{
	std::pair< NodeConnectTable::iterator, NodeConnectTable::iterator > range = nodeConnectTable.equal_range(n0);
	NodeConnectTable::iterator nIter = range.first;
	while( nIter != range.second ){
		if( nIter->second == n1 ){
			return false;
		}
		++nIter;
	}
	nodeConnectTable.insert( std::pair< kmb::nodeIdType, kmb::nodeIdType >(n0,n1) );
	return true;
}

size_t
kmb::MeshSmoother::appendBody(kmb::bodyIdType bodyId)
{
	const kmb::ElementContainer* body = this->mesh->getBodyPtr(bodyId);
	return appendBody(body);
}

size_t
kmb::MeshSmoother::appendBody(const kmb::ElementContainer* body)
{
	if( body == NULL ){
		return 0;
	}

	kmb::DataBindings* nodeGroup =
		kmb::DataBindings::createDataBindings(
			kmb::DataBindings::NodeGroup,
			kmb::PhysicalValue::None,"");

	kmb::BoundaryExtractor bext;
	bext.appendUnitBody(body);
	bext.getBoundaryNodeGroup(body,nodeGroup);
	size_t orgSize = nodeConnectTable.size();
	kmb::ElementContainer::const_iterator eIter = body->begin();
	while( !eIter.isFinished() ){
		int len = eIter.getEdgeCount();
		for(int i=0;i<len;++i){
			kmb::nodeIdType n0 = eIter.getEdgeCellId(i,0);
			kmb::nodeIdType n1 = eIter.getEdgeCellId(i,1);
			if( eIter.getDegree() == 2 ){
				kmb::nodeIdType n2 = eIter.getEdgeCellId(i,2);
				if( !nodeGroup->hasId(n0) ){
					appendConnectNode(n0,n2);
				}
				if( !nodeGroup->hasId(n1) ){
					appendConnectNode(n1,n2);
				}
				if( !nodeGroup->hasId(n2) ){
					appendConnectNode(n2,n0);
					appendConnectNode(n2,n1);
				}
			}else{
				if( !nodeGroup->hasId(n0) ){
					appendConnectNode(n0,n1);
				}
				if( !nodeGroup->hasId(n1) ){
					appendConnectNode(n1,n0);
				}
			}
		}
		++eIter;
	}
	delete nodeGroup;
	return nodeConnectTable.size() - orgSize;
}

int
kmb::MeshSmoother::smoothingNode(kmb::nodeIdType nodeId)
{
	if(mesh==NULL){
		return 0;
	}
	kmb::Point3DContainer* points = mesh->getNodes();
	if( points == NULL || shiftPoints == NULL ){
		return 0;
	}
	kmb::Point3D point(0.0,0.0,0.0);
	double x,y,z;
	std::pair< NodeConnectTable::iterator, NodeConnectTable::iterator > range = nodeConnectTable.equal_range(nodeId);
	if( range.first == range.second ){
		return 0;
	}
	int count = 0;
	NodeConnectTable::iterator nIter = range.first;
	while( nIter != range.second ){
		kmb::nodeIdType n0 = nIter->second;
		points->getXYZ(n0,x,y,z);
		point.addCoordinate(x,y,z);
		++nIter;
		++count;
	}
	if( count > 0 ){
		point.scale( (1.0-relax)/count );
		points->getXYZ(nodeId,x,y,z);
		point.addCoordinate( relax*x, relax*y, relax*z );
		shiftPoints->addPoint( point, nodeId );
		return 1;
	}
	return 0;
}

int
kmb::MeshSmoother::smoothingAllNode(void)
{
	if(mesh==NULL){
		return 0;
	}
	kmb::Point3DContainer* points = mesh->getNodes();
	if( points == NULL || shiftPoints == NULL ){
		return 0;
	}
	kmb::Point3D point(0.0,0.0,0.0);
	double x,y,z;
	int shiftCount = 0;
	std::multimap<kmb::nodeIdType,kmb::nodeIdType>::iterator nIter = nodeConnectTable.begin();
	while( nIter != nodeConnectTable.end() ){
		int count = 0;
		point.zero();
		kmb::nodeIdType nodeId = nIter->first;
		std::multimap<kmb::nodeIdType,kmb::nodeIdType>::iterator upperIter = nodeConnectTable.upper_bound(nodeId);
		while( nIter != upperIter ){
			kmb::nodeIdType n0 = nIter->second;
			points->getXYZ(n0,x,y,z);
			point.addCoordinate(x,y,z);
			++nIter;
			++count;
		}
		if( count > 0 ){
			point.scale( (1.0-relax)/count );
			points->getXYZ(nodeId,x,y,z);
			point.addCoordinate( relax*x, relax*y, relax*z );
			shiftPoints->addPoint( point, nodeId );
			++shiftCount;
		}else{
			++nIter;
		}
	}
	return shiftCount;
}

int
kmb::MeshSmoother::smoothingNodeGroup(const char* name,const char* stype)
{
	if(mesh==NULL){
		return 0;
	}
	kmb::Point3DContainer* points = mesh->getNodes();
	if( points == NULL || shiftPoints == NULL ){
		return 0;
	}
	kmb::DataBindings* data = mesh->getDataBindingsPtr(name,stype);
	if( data == NULL || data->getBindingMode() != kmb::DataBindings::NodeGroup ){
		return 0;
	}
	int shiftCount = 0;
	kmb::Point3D point(0.0,0.0,0.0);
	double x,y,z;
	kmb::DataBindings::iterator dIter = data->begin();
	while( !dIter.isFinished() ){
		kmb::nodeIdType nodeId = static_cast<kmb::nodeIdType>(dIter.getId());
		std::pair< NodeConnectTable::iterator, NodeConnectTable::iterator > range = nodeConnectTable.equal_range(nodeId);
		int count = 0;
		point.zero();
		NodeConnectTable::iterator nIter = range.first;
		while( nIter != range.second ){
			kmb::nodeIdType n0 = nIter->second;
			points->getXYZ(n0,x,y,z);
			point.addCoordinate(x,y,z);
			++nIter;
			++count;
		}
		if( count > 0 ){
			point.scale( (1.0-relax)/count );
			points->getXYZ(nodeId,x,y,z);
			point.addCoordinate( relax*x, relax*y, relax*z );
			shiftPoints->addPoint( point, nodeId );
			++shiftCount;
		}
		++dIter;
	}
	return shiftCount;
}

void
kmb::MeshSmoother::commit(void)
{
	kmb::Point3DContainer* points = mesh->getNodes();
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
