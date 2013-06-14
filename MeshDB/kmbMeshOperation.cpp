/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.7                          #
# Class Name : MeshOperation                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/05/23     #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbMeshOperation.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbMeshBrep.h"
#include "MeshDB/kmbNodeMapperBindings.h"
#include "Geometry/kmbBucket.h"
#include "Geometry/kmbBoundingBox.h"
#include "Geometry/kmbPoint3DContainerMArray.h"

kmb::MeshOperation::MeshOperation(MeshData* _mesh)
	: mesh(_mesh)
{
}

kmb::MeshOperation::~MeshOperation(void)
{
}

size_t kmb::MeshOperation::replaceNodeIdOfBody(kmb::bodyIdType bodyId,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper)
{
	if( mesh == NULL ){
		return 0;
	}
	size_t count = 0;
	kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			count += eIter.replaceNodeId(nodeMapper);
			++eIter;
		}
	}
	return count;
}

size_t kmb::MeshOperation::nodeSplitByVolumes(void)
{
	if( mesh == NULL ){
		return 0;
	}
	kmb::bodyIdType bodyCount = static_cast<kmb::bodyIdType>(mesh->getBodyCount());
	int volCount = 0;
	for(kmb::bodyIdType bodyId=0; bodyId < bodyCount; ++bodyId){
		kmb::Body* body = mesh->getBodyPtr(bodyId);
		if( body && body->getDimension() == 3 ){
			++volCount;
		}
	}

	if( volCount <= 1 ){
		return 0;
	}

	kmb::MeshBrep* brep = new kmb::MeshBrep(mesh);
	int vres = brep->createVolumeInfo();



	kmb::Point3DContainer* nodes = new kmb::Point3DContainerMArray();
	nodes->initialize( mesh->getNodeCount() );
	std::vector< kmb::bodyIdType > volumes;
	brep->getVolumes(volumes);
	kmb::Point3D pt;
	for( std::vector< kmb::bodyIdType >::iterator vIter = volumes.begin(); vIter < volumes.end(); ++vIter)
	{

		std::map< kmb::nodeIdType, kmb::nodeIdType > nodeMapperLocal;
		kmb::bodyIdType volumeId = *vIter;
		kmb::ElementContainer* body = mesh->getBodyPtr(volumeId);
		if( body ){
			kmb::ElementContainer::iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				int nCount = eIter.getNodeCount();
				for(int i=0;i<nCount;++i){

					std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator n2Iter = nodeMapperLocal.find(eIter[i]);
					if( n2Iter == nodeMapperLocal.end() ){
						mesh->getNode(eIter[i],pt);
						kmb::nodeIdType dnodeId = nodes->addPoint(pt.x(),pt.y(),pt.z());
						nodeMapperLocal.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(eIter[i],dnodeId) );
/*
						std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator n1Iter = nodeMapperAll.find(eIter[i]);
						if( n1Iter == nodeMapperAll.end() ){

							nodeMapperLocal.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(eIter[i],eIter[i]) );
						}else{


							mesh->getNode(eIter[i],pt);
							kmb::nodeIdType dnodeId = mesh->insertNode(pt.x(),pt.y(),pt.z());
							nodeMapperLocal.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(eIter[i],dnodeId) );
						}
*/
					}
				}
				++eIter;
			}
		}
		replaceNodeIdOfBody(volumeId,nodeMapperLocal);
		std::vector< kmb::bodyIdType > surfaces;
		brep->getChildren(volumeId,surfaces);
		for( std::vector< kmb::bodyIdType >::iterator sIter = surfaces.begin(); sIter < surfaces.end(); ++sIter){
			kmb::bodyIdType surfaceId = *sIter;
			replaceNodeIdOfBody(surfaceId,nodeMapperLocal);
		}






	}

	mesh->replaceNodes( nodes );
	delete brep;
	return 0;
}

size_t kmb::MeshOperation::uniteNodes(double thresh)
{
	if( mesh == NULL || mesh->getNodes() == NULL ){
		return 0;
	}


	std::map<kmb::nodeIdType,kmb::nodeIdType> idmap;
	kmb::BoundingBox bbox = mesh->getBoundingBox();
	bbox.expand(1.2);
	kmb::Bucket3DGrid bucket;
	bucket.setRegionGrid( bbox, mesh->getNodeCount() / 1000 + 1 );
	bucket.setContainer( mesh->getNodes() );
	bucket.appendAll();

	std::vector< kmb::nodeIdType > nodeIds;
	kmb::Point3DContainer::iterator pIter = mesh->getNodes()->begin();
	while( !pIter.isFinished() ){
		nodeIds.clear();
		kmb::nodeIdType nodeId = pIter.getId();
		kmb::nodeIdType nearId = pIter.getId();
		bucket.getNearPoints( nodeId, 1.0e-8, nodeIds );

		for(int i=0;i<nodeIds.size();i++){
			if( nodeIds[i] < nearId ){
				nearId = nodeIds[i];
			}
		}
		if( nodeId != nearId ){
			idmap.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(nodeId,nearId) );
			std::cout << nodeId << " => " << nearId << std::endl;
		}
		++pIter;
	}


	kmb::bodyIdType bCount = static_cast<kmb::bodyIdType>(mesh->getBodyCount());
	for(kmb::bodyIdType bodyId=0;bodyId<bCount;++bodyId)
	{
		replaceNodeIdOfBody(bodyId,idmap);
	}


	kmb::datamap::iterator dIter = mesh->beginDataIterator();
	while( dIter != mesh->endDataIterator() )
	{
		kmb::DataBindings* d = dIter->second;
		if( d != NULL ){
			switch(d->getBindingMode()){
				case kmb::DataBindings::NodeGroup:
				{
					if( strcmp( "DataBindingsGroup", d->getContainerType() ) == 0 ){
						reinterpret_cast<kmb::DataBindingsGroup<kmb::nodeIdType>*>(d)->replaceId( idmap );
					}
					break;
				}
				case kmb::DataBindings::NodeVariable:
				{
					if( strcmp( "DataVariable", d->getContainerType() ) == 0 ){
						d->replaceNodeId( idmap );
					}
					break;
				}
				default:
					break;
			}
		}
		++dIter;
	}
	return idmap.size();
}

int kmb::MeshOperation::duplicateNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName)
{
	if( mesh == NULL || mesh->getNodes() == NULL ){
		return 0;
	}
	int count = 0;
	kmb::nodeIdType orgNodeId = kmb::nullNodeId;
	kmb::nodeIdType newNodeId = kmb::nullNodeId;
	long l;
	kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
	kmb::Point3DContainer* points = mesh->getNodes();
	kmb::DataBindings* data = NULL;
	if( body && points ){
		if( coupleName ){
			data = mesh->getDataBindingsPtr(coupleName);
			if( data == NULL ){
				data = new kmb::NodeMapperBindings<kmb::nodeIdType>();
				mesh->setDataBindingsPtr(coupleName,data);
			}
		}else{
			data = new kmb::NodeMapperBindings<kmb::nodeIdType>();
		}
		kmb::ElementContainer::iterator eIter = body->begin();
		while( !eIter.isFinished() ){
			int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				orgNodeId = eIter[i];
				if( !data->hasId( orgNodeId ) ){
					++count;
					newNodeId = points->duplicatePoint( orgNodeId );
					eIter.setCellId(i,newNodeId);
					l = static_cast<long>(newNodeId);
					data->setPhysicalValue( orgNodeId, &l );
				}else if( data->getPhysicalValue( orgNodeId, &l ) ){
					newNodeId = static_cast<kmb::nodeIdType>(l);
					eIter.setCellId(i,newNodeId);
				}
			}
			++eIter;
		}
		if( data && coupleName == NULL ){

			delete data;
		}
	}
	return count;
}
