/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.7                          #
# Class Name : MeshOperation                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/05/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbMeshOperation.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbMeshBrepInfo.h"
#include "MeshDB/kmbNodeMapperBindings.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbDataBindings.h"
#include "Geometry/kmbBucket.h"
#include "Geometry/kmbBoundingBox.h"
#include "Geometry/kmbPoint3DContainerMArray.h"

#include <cstring>
#include <cstdlib>

kmb::MeshOperation::MeshOperation(MeshData* _mesh)
	: mesh(_mesh)
{
}

kmb::MeshOperation::~MeshOperation(void)
{
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
	// そもそも領域が1以下
	if( volCount <= 1 ){
		return 0;
	}
	// 節点番号を2重化する必要がある場合の対応
	kmb::MeshBrepInfo* brep = kmb::MeshBrepInfo::create3DModelWithBoundary(mesh,true);
	kmb::Point3DContainer* nodes = new kmb::Point3DContainerMArray();
	nodes->initialize( mesh->getNodeCount() );
	std::vector< kmb::bodyIdType > volumes;
	brep->getVolumes(volumes);
	kmb::Point3D pt;
	for( std::vector< kmb::bodyIdType >::iterator vIter = volumes.begin(); vIter < volumes.end(); ++vIter)
	{
		// 旧番号 => 新番号
		std::map< kmb::nodeIdType, kmb::nodeIdType > nodeMapperLocal;
		kmb::bodyIdType volumeId = *vIter;
		kmb::ElementContainer* body = mesh->getBodyPtr(volumeId);
		if( body ){
			kmb::ElementContainer::iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				int nCount = eIter.getNodeCount();
				for(int i=0;i<nCount;++i){
					// Local で最初に出てきた節点かどうか
					std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator n2Iter = nodeMapperLocal.find(eIter[i]);
					if( n2Iter == nodeMapperLocal.end() ){
						mesh->getNode(eIter[i],pt);
						kmb::nodeIdType dnodeId = nodes->addPoint(pt.x(),pt.y(),pt.z());
						nodeMapperLocal.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(eIter[i],dnodeId) );
					}
				}
				++eIter;
			}
		}
		kmb::ElementContainer* vol = mesh->getBodyPtr(volumeId);
		if( vol ){
			vol->replaceNodeId( nodeMapperLocal );
		}
		std::vector< kmb::bodyIdType > surfaces;
		brep->getChildren(volumeId,surfaces);
		for( std::vector< kmb::bodyIdType >::iterator sIter = surfaces.begin(); sIter < surfaces.end(); ++sIter){
			kmb::bodyIdType surfaceId = *sIter;
			kmb::ElementContainer* surf = mesh->getBodyPtr(surfaceId);
			if( surf ){
				surf->replaceNodeId( nodeMapperLocal );
			}
		}
	}
	// node container を入れ替え
	mesh->replaceNodes( nodes );
	delete brep;
	return 0;
}

size_t kmb::MeshOperation::uniteNodes(double thresh)
{
	if( mesh == NULL || mesh->getNodes() == NULL ){
		return 0;
	}
	// 新しい nodeId と古い nodeId の対応関係を覚えておく
	// idmap[oldid] = newid
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
		bucket.getNearPoints( nodeId, thresh, nodeIds );
		// 小さい節点番号に一致させる
		std::vector< kmb::nodeIdType >::iterator nIter = nodeIds.begin();
		while( nIter != nodeIds.end() ){
		  if( *nIter < nearId ){
		    nearId = *nIter;
		  }
		  ++nIter;
		}
		if( nodeId != nearId ){
			idmap.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(nodeId,nearId) );
			std::cout << nodeId << " => " << nearId << std::endl;
		}
		++pIter;
	}

	// 要素情報の更新
	kmb::bodyIdType bCount = static_cast<kmb::bodyIdType>(mesh->getBodyCount());
	for(kmb::bodyIdType bodyId=0;bodyId<bCount;++bodyId)
	{
		kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
		if( body ){
			body->replaceNodeId( idmap );
		}
	}

	// 物理量の更新
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
			// mesh に登録されていない data は削除する
			delete data;
		}
	}
	return count;
}


// 親要素の検索
kmb::elementIdType kmb::MeshOperation::getParentElement(const kmb::ElementBase &elem,const kmb::ElementContainer* parent,const kmb::NodeNeighborInfo &neighbors)
{
	if( parent == NULL ){
		return kmb::Element::nullElementId;
	}
	// 最初の節点番号で調べてよい
	int i0,i1;
	kmb::nodeIdType nodeId = elem[0];
	kmb::NodeNeighbor::const_iterator nIter = neighbors.beginIteratorAt(nodeId);
	kmb::NodeNeighbor::const_iterator endIter = neighbors.endIteratorAt(nodeId);
	while( nIter != endIter ){
		kmb::elementIdType elementId = nIter->second;
		kmb::ElementContainer::const_iterator pelem = parent->find(elementId);
		if( kmb::ElementRelation::getRelation(elem,i0,pelem,i1) == kmb::ElementRelation::BOUNDARY ){
			return elementId;
		}
		++nIter;
	}
	return kmb::Element::nullElementId;
}

int kmb::MeshOperation::triangulation(kmb::bodyIdType bodyId)
{
	if( mesh == NULL ){
		return -1;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
	if( body == NULL ){
		return -1;
	}
	int count = 0;
	// iterator の中でコンテナの中身を替えたくないから、
	// 一度 elementId をキャッシュする
	std::vector<kmb::elementIdType> elementIds;

	kmb::ElementContainer::iterator eIter = body->begin();
	while( eIter != body->end() ){
		elementIds.push_back( eIter.getId() );
		++eIter;
	}

	kmb::nodeIdType triangles[2][3];
	kmb::nodeIdType tetrahedrons[6][4];

	std::vector<kmb::elementIdType>::iterator iter = elementIds.begin();
	while(iter != elementIds.end())
	{
		kmb::elementIdType elementId = (*iter);
		kmb::ElementContainer::iterator e = body->find(elementId);
		if( !e.isFinished() && e.getDimension() == 2){
			const int num = e.divideIntoTriangles( triangles );
			++count;
			for(int i=0;i<num;++i)
			{
				mesh->insertElement( bodyId, kmb::TRIANGLE, triangles[i] ); // MeshDB を通して登録する（ ElementId を発行するため）
			}
			body->deleteElement(elementId);
		}else if( !e.isFinished() && e.getDimension() == 3 ){
			const int num = e.divideIntoTetrahedrons(tetrahedrons);
			++count;
			for(int i=0;i<num;++i)
			{
				mesh->insertElement( bodyId, kmb::TETRAHEDRON, tetrahedrons[i] );
			}
			body->deleteElement(elementId);
		}
		++iter;
	}
	return count;
}

bool kmb::MeshOperation::isParent(const kmb::ElementContainer* child,const kmb::ElementContainer* parent,const kmb::NodeNeighborInfo &neighbors)
{
	if( parent == NULL || child == NULL ){
		return false;
	}
	kmb::ElementContainer::const_iterator eIter = child->begin();
	if( !eIter.isFinished() ){
		// 最初の節点番号で調べてよい
		int i0,i1;
		kmb::nodeIdType nodeId = eIter[0];
		kmb::NodeNeighbor::const_iterator nIter = neighbors.beginIteratorAt(nodeId);
		kmb::NodeNeighbor::const_iterator endIter = neighbors.endIteratorAt(nodeId);
		while( nIter != endIter ){
			kmb::elementIdType elementId = nIter->second;
			kmb::ElementContainer::const_iterator pelem = parent->find(elementId);
			kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation(eIter,i0,pelem,i1);
			if( rel == kmb::ElementRelation::BOUNDARY || rel == kmb::ElementRelation::ANTIBOUNDARY ){
				return true;
			}
			++nIter;
		}
	}
	return false;
}

int kmb::MeshOperation::surface2FaceGroup(const kmb::ElementContainer* child,const kmb::ElementContainer* parent,const kmb::NodeNeighborInfo &neighborInfo,kmb::DataBindings* faceGroup)
{
	if( child == NULL || parent == NULL || faceGroup == NULL ){
		return 0;
	}
	if( faceGroup->getBindingMode() != kmb::DataBindings::FaceGroup ){
		return 0;
	}
	int count = 0;
	int index = -1;
	int otherindex = -1;
	kmb::ElementContainer::const_iterator eIter = child->begin();
	while( !eIter.isFinished() )
	{
		kmb::nodeIdType n0 = eIter[0];
		kmb::NodeNeighbor::const_iterator neiIter = neighborInfo.beginIteratorAt(n0);
		kmb::NodeNeighbor::const_iterator neiEnd = neighborInfo.endIteratorAt(n0);
		while( neiIter != neiEnd ){
			kmb::ElementContainer::const_iterator parentElement = parent->find( neiIter->second );
			if( !parentElement.isFinished() ){
				kmb::ElementRelation::relationType rel =
					kmb::ElementRelation::getRelation( parentElement, index, eIter, otherindex );
				if( rel == kmb::ElementRelation::COBOUNDARY ){
					faceGroup->addId( kmb::Face(neiIter->second,index) );
					++count;
					goto findFace;
				}
			}
			++neiIter;
		}
findFace:
		++eIter;
	}
	return count;
}
