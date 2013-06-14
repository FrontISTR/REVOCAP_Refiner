/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.7                          #
# Class Name : MeshBrep                                                #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/03/23     #
#                                                                      #
----------------------------------------------------------------------*/

#include "MeshDB/kmbMeshBrep.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbElementRelation.h"

kmb::MeshBrep::MeshBrep(const MeshData* _mesh)
	: mesh(_mesh), volumes(), surfaces(), reverseOrientation(false), neighborInfo(NULL)
{
}

kmb::MeshBrep::~MeshBrep(void)
{
	if( neighborInfo != NULL ){
		delete neighborInfo;
		neighborInfo = NULL;
	}
}

bool kmb::MeshBrep::getReverseOrientation(void) const
{
	return reverseOrientation;
}

void kmb::MeshBrep::setReverseOrientation(bool f)
{
	reverseOrientation = f;
}

int kmb::MeshBrep::appendVolume(kmb::bodyIdType parent,kmb::bodyIdType child)
{
	if( mesh == NULL || mesh->getDimension(parent) != 3 || mesh->getDimension(child) != 2 ){
		return 0;
	}
	bool flag = false;
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::iterator vLower = volumes.lower_bound(parent);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::iterator vUpper = volumes.upper_bound(parent);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::iterator vIter;
	for( vIter = vLower; vIter != vUpper; ++vIter )
	{
		if( vIter->second == child ){
			flag = true;
			break;
		}
	}
	if( flag ){
		return 0;
	}else{
		volumes.insert( std::pair< kmb::bodyIdType, kmb::bodyIdType >(parent,child) );
		return 1;
	}
}

int kmb::MeshBrep::appendSurface(kmb::bodyIdType parent,kmb::bodyIdType child)
{
	if( mesh == NULL || mesh->getDimension(parent) != 2 || mesh->getDimension(child) != 1 ){
		return 0;
	}
	bool flag = false;
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::iterator sLower = surfaces.lower_bound(parent);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::iterator sUpper = surfaces.upper_bound(parent);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::iterator sIter;
	for( sIter = sLower; sIter != sUpper; ++sIter )
	{
		if( sIter->second == child ){
			flag = true;
			break;
		}
	}
	if( flag ){
		return 0;
	}else{
		surfaces.insert( std::pair< kmb::bodyIdType, kmb::bodyIdType >(parent,child) );
		return 1;
	}
}

int kmb::MeshBrep::createVolumeInfo(void)
{
	if( mesh == NULL ){
		return 0;
	}
	if( neighborInfo == NULL ){
		neighborInfo = new kmb::NodeNeighborInfo();
	}
	neighborInfo->clear();
	size_t bCount = mesh->getBodyCount();

	for(kmb::bodyIdType b=0;b<bCount;++b){
		const kmb::ElementContainer* body = mesh->getBodyPtr(b);
		if( body && body->getDimension() == 3 ){
			neighborInfo->appendCoboundary( body );
		}
	}

	for(kmb::bodyIdType b=0;b<bCount;++b){
		const kmb::ElementContainer* body = mesh->getBodyPtr(b);
		if( body && body->getDimension() == 2 ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			kmb::nodeIdType n0 = eIter[0];
			kmb::bodyIdType parentId = kmb::Body::nullBodyId;
			kmb::NodeNeighbor::iterator beginIter = neighborInfo->beginIteratorAt(n0);
			kmb::NodeNeighbor::iterator endIter = neighborInfo->endIteratorAt(n0);
			kmb::NodeNeighbor::iterator nIter = beginIter;
			while( nIter != endIter ){
				kmb::elementIdType elementId = nIter->second;
				kmb::ElementContainer::const_iterator parentElement = mesh->findElement(elementId);
				int index=-1;
				int otherIndex=-1;
				kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation( eIter, index, parentElement, otherIndex );
				if( ( reverseOrientation == false && rel == kmb::ElementRelation::BOUNDARY ) ||
					( reverseOrientation == true && rel == kmb::ElementRelation::ANTIBOUNDARY ) )
				{
					parentId = mesh->getBodyId( elementId );
					this->appendVolume( parentId, b );
					break;
				}
				++nIter;
			}
			if( parentId == kmb::Body::nullBodyId ){

				nIter = beginIter;
				while( nIter != endIter ){
					kmb::elementIdType elementId = nIter->second;
					kmb::ElementContainer::const_iterator parentElement = mesh->findElement(elementId);
					int index=-1;
					int otherIndex=-1;
					kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation( eIter, index, parentElement, otherIndex );
					if( rel == kmb::ElementRelation::BOUNDARY || rel == kmb::ElementRelation::ANTIBOUNDARY )
					{
						parentId = mesh->getBodyId( elementId );
						this->appendVolume( parentId, b );
						break;
					}
					++nIter;
				}
			}
		}
	}
	return static_cast<int>( volumes.size() );
}

int kmb::MeshBrep::createSurfaceInfo(void)
{
	if( mesh == NULL ){
		return 0;
	}
	if( neighborInfo == NULL ){
		neighborInfo = new kmb::NodeNeighborInfo();
	}
	neighborInfo->clear();
	size_t bCount = mesh->getBodyCount();

	for(kmb::bodyIdType b=0;b<bCount;++b){
		const kmb::ElementContainer* body = mesh->getBodyPtr(b);
		if( body && body->getDimension() == 2 ){
			neighborInfo->appendCoboundary( body );
		}
	}

	for(kmb::bodyIdType b=0;b<bCount;++b){
		const kmb::ElementContainer* body = mesh->getBodyPtr(b);
		if( body && body->getDimension() == 1 ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			kmb::nodeIdType n0 = eIter[0];
			kmb::bodyIdType parentId = kmb::Body::nullBodyId;
			kmb::NodeNeighbor::iterator beginIter = neighborInfo->beginIteratorAt(n0);
			kmb::NodeNeighbor::iterator endIter = neighborInfo->endIteratorAt(n0);
			kmb::NodeNeighbor::iterator nIter = beginIter;
			while( nIter != endIter ){
				kmb::elementIdType elementId = nIter->second;
				kmb::ElementContainer::const_iterator parentElement = mesh->findElement(parentId);
				int index=-1;
				int otherIndex=-1;
				if( kmb::ElementRelation::getRelation( eIter, index, parentElement, otherIndex )
					== kmb::ElementRelation::BOUNDARY )
				{
					parentId = mesh->getBodyId( elementId );
					this->appendSurface( parentId, b );
					break;
				}
				++nIter;
			}
			if( parentId == kmb::Body::nullBodyId ){

				nIter = beginIter;
				while( nIter != endIter ){
					kmb::elementIdType elementId = nIter->second;
					kmb::ElementContainer::const_iterator parentElement = mesh->findElement(elementId);
					int index=-1;
					int otherIndex=-1;
					kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation( eIter, index, parentElement, otherIndex );
					if( rel == kmb::ElementRelation::BOUNDARY || rel == kmb::ElementRelation::ANTIBOUNDARY )
					{
						parentId = mesh->getBodyId( elementId );
						this->appendSurface( parentId, b );
						break;
					}
					++nIter;
				}
			}
		}
	}
	return static_cast<int>( surfaces.size() );
}

size_t kmb::MeshBrep::getChildren(kmb::bodyIdType bodyId,std::vector<kmb::bodyIdType>& children) const
{
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator vLower = volumes.lower_bound(bodyId);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator vUpper = volumes.upper_bound(bodyId);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator vIter;
	for( vIter = vLower; vIter != vUpper; ++vIter ){
		children.push_back( vIter->second );
	}
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator sLower = surfaces.lower_bound(bodyId);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator sUpper = surfaces.upper_bound(bodyId);
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator sIter;
	for( sIter = sLower; sIter != sUpper; ++sIter ){
		children.push_back( sIter->second );
	}
	return children.size();
}

size_t kmb::MeshBrep::getVolumes(std::vector<kmb::bodyIdType>& bodyIds) const
{
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator vBegin = volumes.begin();
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator vEnd = volumes.end();
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator vIter;
	for( vIter = vBegin; vIter != vEnd; vIter = volumes.upper_bound(vIter->first) ){
		bodyIds.push_back( vIter->first );
	}
	return bodyIds.size();
}

size_t kmb::MeshBrep::getSurfaces(std::vector<kmb::bodyIdType>& bodyIds) const
{
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator sBegin = surfaces.begin();
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator sEnd = surfaces.end();
	std::multimap< kmb::bodyIdType, kmb::bodyIdType >::const_iterator sIter;
	for( sIter = sBegin; sIter != sEnd; sIter = surfaces.upper_bound(sIter->first) ){
		bodyIds.push_back( sIter->first );
	}
	return bodyIds.size();
}
