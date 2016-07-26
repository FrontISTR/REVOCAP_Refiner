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
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbNodeMapperBindings.h"
#include "MeshDB/kmbMeshOperation.h"

#include <map>
#include <cstring>

// nodeID を initId から始まるように連続化する
// 省略すれば 0 から順に埋めていく
// ただし順序は保存しない
// 非常に長時間かかることがあるので注意


void
kmb::MeshDB::nodeIdDefragmentation(nodeIdType initId)
{
	if( node3Ds == NULL ||
		strcmp( node3Ds->getContainerType(), kmb::Point3DContainerMap::CONTAINER_TYPE ) != 0 )
	{
		return;
	}

	// 近傍情報のクリア
	this->neighborInfo.clear();

	// 新しい nodeId と古い nodeId の対応関係を覚えておく
	// idmap[oldid] = newid
	std::map<kmb::nodeIdType,kmb::nodeIdType> idmap;

	static_cast< kmb::Point3DContainerMap*>(this->node3Ds)->idDefragment(initId,idmap);

	// 変更がなければこれで終わり
	if( idmap.size() == 0 ){
		return;
	}

	// 要素情報の更新
	for(std::vector<kmb::ElementContainer*>::iterator cIter = this->bodies.begin();
		cIter != this->bodies.end();
		++cIter)
	{
		kmb::ElementContainer* body = (*cIter);
		if( body ){
			kmb::ElementContainer::iterator eIter = body->begin();
			while( eIter != body->end() ){
				int len = eIter.getNodeCount();
				for(int i=0;i<len;++i){
					nodeIdType oldId = eIter.getCellId(i);
					if( idmap.find(oldId) != idmap.end() ){
						eIter.setCellId(i,idmap[oldId]);
					}
				}
				++eIter;
			}
		}
	}

	// 物理量の更新
	std::multimap< std::string, kmb::DataBindings*>::iterator bIter = this->bindings.begin();
	while( bIter != this->bindings.end() )
	{
		kmb::DataBindings* d = bIter->second;
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
		++bIter;
	}
}

bool
kmb::MeshDB::replaceNodeId(kmb::nodeIdType oldId, kmb::nodeIdType newId)
{
	if( this->node3Ds
		&& strcmp( node3Ds->getContainerType(), kmb::Point3DContainerMap::CONTAINER_TYPE ) == 0
		&& static_cast< kmb::Point3DContainerMap*>(this->node3Ds)->replaceId(oldId,newId) ){
		// 古い Id の周辺要素を調べる
		std::vector<elementIdType> coboundary;
		// 逆対応が登録されていなかったら cache なしバージョンを使う
		getSurroundingElements(oldId,coboundary,false);

		std::vector<elementIdType>::iterator eIter = coboundary.begin();
		while ( eIter != coboundary.end() )
		{
			kmb::elementIdType eid = (*eIter);
			kmb::ElementContainer::iterator e = this->findElement(eid);
			if( !e.isFinished() )
			{
				e.replaceNodeId( oldId, newId );
			}
			++eIter;
		}
		// 物理量についても変換する
		std::multimap< std::string, kmb::DataBindings*>::iterator iterBind = this->bindings.begin();
		while( iterBind != this->bindings.end() )
		{
			kmb::DataBindings* data = iterBind->second;
			if( data != NULL && (
				data->getBindingMode() == kmb::DataBindings::NodeVariable ||
				data->getBindingMode() == kmb::DataBindings::NodeGroup ) )
			{
				data->replaceId( oldId, newId );
			}
			++iterBind;
		}
		return true;
	}else{
		return false;
	}
}

// 使われていない節点を削除する
// 削除した節点の個数を返す
int
kmb::MeshDB::deleteUselessNodes(void)
{
	int count = 0;
	kmb::NodeNeighborInfo neighborInfo;
	// すべての Body の近傍情報
	neighborInfo.appendCoboundary( this );

	if( this->node3Ds
		&& strcmp( node3Ds->getContainerType(), kmb::Point3DContainerMap::CONTAINER_TYPE ) == 0 )
	{
		kmb::Point3DContainer::iterator nIter = this->node3Ds->begin();
		while( nIter != this->node3Ds->end() )
		{
			kmb::nodeIdType nodeId = nIter.getId();
			++nIter;
			if( neighborInfo.getElementCountAroundNode( nodeId ) == 0 ){
				reinterpret_cast< kmb::Point3DContainerMap*>(this->node3Ds)->deleteId( nodeId );
				++count;
			}
		}
		reinterpret_cast< kmb::Point3DContainerMap*>(this->node3Ds)->updateMinMaxId();
	}
	return count;
}

bool
kmb::MeshDB::replaceNodeIdOfElement
(kmb::bodyIdType bodyId,kmb::elementIdType elementId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId)
{
	kmb::ElementContainer::iterator element = this->findElement(elementId,bodyId);
	if( !element.isFinished() ){
		return element.replaceNodeId(oldNodeId,newNodeId);
	}
	return false;
}

bool
kmb::MeshDB::reverseElement(kmb::elementIdType elementID,kmb::bodyIdType bodyID)
{
	kmb::ElementContainer::iterator elem = this->findElement(elementID,bodyID);
	return elem.reverse();
}

// 置き換えの個数を返す
int
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId)
{
	int count = 0;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		count = body->replaceNodeId(oldNodeId,newNodeId);
	}
	return count;
}

// 置き換えの個数を返す
int
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,std::map<kmb::nodeIdType,kmb::nodeIdType>& nodeMapper)
{
	int count = 0;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		count = body->replaceNodeId(nodeMapper);
	}
	return count;
}

int
kmb::MeshDB::replaceNodeIdOfBody(kmb::bodyIdType bodyId,const char* coupleName)
{
	int count = 0;
	kmb::nodeIdType orgNodeId = kmb::nullNodeId;
	kmb::nodeIdType newNodeId = kmb::nullNodeId;
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	kmb::DataBindings* data = this->getDataBindingsPtr(coupleName);
	long l = 0L;
	if( body && data &&
		strcmp("NodeMapperBindings",data->getContainerType()) == 0 )
	{
		kmb::ElementContainer::iterator eIter = body->begin();
		while( !eIter.isFinished() ){
			int len = eIter.getNodeCount();
			for(int i=0;i<len;++i){
				orgNodeId = eIter[i];
				if( data->hasId( orgNodeId ) && data->getPhysicalValue( orgNodeId, &l ) ){
					newNodeId = static_cast<kmb::nodeIdType>(l);
					eIter.setCellId(i,newNodeId);
				}
			}
			++eIter;
		}
	}
	return count;
}

void
kmb::MeshDB::reverseBody(kmb::bodyIdType bodyId)
{
	kmb::ElementContainer* body = this->getBodyPtr(bodyId);
	if( body ){
		kmb::ElementContainer::iterator eIter = body->begin();
		while( eIter != body->end() )
		{
			eIter.reverse();
			++eIter;
		}
	}
}

size_t kmb::MeshDB::countCommonNode(kmb::bodyIdType bodyId0,kmb::bodyIdType bodyId1) const
{
	const kmb::ElementContainer* body0 = this->getBodyPtr(bodyId0);
	const kmb::ElementContainer* body1 = this->getBodyPtr(bodyId1);
	if( body0 == NULL || body1 == NULL ){
		return 0;
	}
	// 節点集合を調べておく
	kmb::DataBindings* nodeGroup0 = kmb::DataBindings::createDataBindings(kmb::DataBindings::NodeGroup,kmb::PhysicalValue::None,"",bodyId0);
	kmb::DataBindings* nodeGroup1 = kmb::DataBindings::createDataBindings(kmb::DataBindings::NodeGroup,kmb::PhysicalValue::None,"",bodyId0);
	kmb::ElementContainer::const_iterator eIter0 = body0->begin();
	while( !eIter0.isFinished() ){
		int len = eIter0.getNodeCount();
		for(int i=0;i<len;++i){
			nodeGroup0->addId( eIter0[i] );
		}
		++eIter0;
	}
	kmb::ElementContainer::const_iterator eIter1 = body1->begin();
	while( !eIter1.isFinished() ){
		int len = eIter1.getNodeCount();
		for(int i=0;i<len;++i){
			nodeGroup1->addId( eIter1[i] );
		}
		++eIter1;
	}
	size_t count = 0;
	kmb::DataBindings::const_iterator dIter = nodeGroup1->begin();
	while( !dIter.isFinished() ){
		if( nodeGroup0->hasId( dIter.getId() ) ){
			++count;
		}
		++dIter;
	}
	delete nodeGroup0;
	delete nodeGroup1;
	return count;
}
