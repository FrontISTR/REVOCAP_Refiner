/*----------------------------------------------------------------------
#                                                                      #
# Software Name : Advance/REVOCAP version 3.2                          #
# Class Name : MeshBrepInfo                                            #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/09/23     #
#                                                                      #
----------------------------------------------------------------------*/

#include "MeshDB/kmbMeshBrepInfo.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbBoundaryExtractor.h"
#include "MeshDB/kmbSurfaceOperation.h"

#include <sstream>
#include <cstdlib>

kmb::MeshBrepItem::MeshBrepItem(void)
	: bodyId(kmb::Body::nullBodyId)
	, faceGroup("")
	, dim(-1)
	, outlineId(kmb::Body::nullBodyId)
	, parent(NULL)
	, children()
{
}

kmb::MeshBrepItem::~MeshBrepItem(void)
{
}

kmb::bodyIdType kmb::MeshBrepItem::getBodyId(void) const
{
	return bodyId;
}

const char* kmb::MeshBrepItem::getGroup(void) const
{
	return faceGroup.c_str();
}


kmb::MeshBrepInfo::MeshBrepInfo(void)
: topDim(0)
, mapperById()
, mapperByGroup()
, volumes()
, surfaces()
, edges()
{
}

kmb::MeshBrepInfo::~MeshBrepInfo(void)
{
	volumes.clear();
	surfaces.clear();
	edges.clear();
	std::map< kmb::bodyIdType, kmb::MeshBrepItem* >::const_iterator bIter = mapperById.begin();
	while( bIter != mapperById.end() ){
		delete bIter->second;
		++bIter;
	}
	mapperById.clear();
	std::map< std::string, kmb::MeshBrepItem* >::const_iterator sIter = mapperByGroup.begin();
	while( sIter != mapperByGroup.end() ){
		delete sIter->second;
		++sIter;
	}
	mapperByGroup.clear();
}

int kmb::MeshBrepInfo::getTopDim(void) const
{
	return topDim;
}

size_t kmb::MeshBrepInfo::getCount(void) const
{
	return volumes.size() + surfaces.size() + edges.size();
}

size_t kmb::MeshBrepInfo::getVolumeCount(void) const
{
	return volumes.size();
}

size_t kmb::MeshBrepInfo::getSurfaceCount(void) const
{
	return surfaces.size();
}

size_t kmb::MeshBrepInfo::getEdgeCount(void) const
{
	return edges.size();
}

kmb::MeshBrepInfo* kmb::MeshBrepInfo::create3DModel(kmb::MeshData *mesh,double angle,bool ridge)
{
	kmb::MeshBrepInfo* brep = new kmb::MeshBrepInfo();
	kmb::bodyIdType bodyCount = mesh->getBodyCount();
	kmb::BoundaryExtractor bext;
	kmb::SurfaceOperation surfOp;
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;bodyId++){
		if( mesh->getDimension(bodyId) == 3 ){
			brep->topDim = 3;
			kmb::MeshBrepItem* item = new kmb::MeshBrepItem();
			item->bodyId = bodyId;
			item->dim = 3;
			brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(bodyId,item) );
			brep->volumes.push_back(item);
			bext.clear();
			bext.setMesh(mesh);
			bext.appendBody(bodyId);
			if( angle > 0.0 ){
				kmb::bodyIdType surface = bext.getBoundary(bodyId);
				surfOp.setMesh(mesh);
				if( ridge ){
					std::map< kmb::bodyIdType, kmb::bodyIdType > subsurfaces;
					surfOp.divideSurfaceWithRidge(surface,angle,subsurfaces);
					std::map< kmb::bodyIdType, kmb::bodyIdType >::iterator sIter = subsurfaces.begin();
					while( sIter != subsurfaces.end() ){
						kmb::MeshBrepItem* child = new kmb::MeshBrepItem();
						child->bodyId = sIter->first;
						child->outlineId = sIter->second;
						child->parent = item;
						child->dim = 2;
						item->children.push_back(child);
						brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(sIter->first,child));
						brep->surfaces.push_back(child);
						++sIter;
					}
					item->outlineId = surface;
				}else{
					std::vector< kmb::bodyIdType > subsurfaces;
					surfOp.divideSurface(surface,angle,subsurfaces);
					std::vector< kmb::bodyIdType >::iterator sIter = subsurfaces.begin();
					while( sIter != subsurfaces.end() ){
						kmb::MeshBrepItem* child = new kmb::MeshBrepItem();
						child->bodyId = *sIter;
						child->parent = item;
						child->dim = 2;
						item->children.push_back(child);
						brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(*sIter,child));
						brep->surfaces.push_back(child);
						++sIter;
					}
				}
			}else{
				kmb::bodyIdType surface = bext.getBoundary(bodyId);
				kmb::MeshBrepItem* child = new kmb::MeshBrepItem();
				child->bodyId = surface;
				item->children.push_back(child);
				child->parent = item;
				child->dim = 2;
				brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(surface,child));
				brep->surfaces.push_back(child);
			}
		}
	}
	return brep;
}

kmb::MeshBrepInfo* kmb::MeshBrepInfo::create3DFaceModel(kmb::MeshData *mesh,double angle,bool ridge)
{
	kmb::MeshBrepInfo* brep = new kmb::MeshBrepInfo();
	kmb::bodyIdType bodyCount = mesh->getBodyCount();
	kmb::BoundaryExtractor bext;
	kmb::SurfaceOperation surfOp;
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;bodyId++){
		if( mesh->getDimension(bodyId) == 3 ){
			brep->topDim = 3;
			kmb::MeshBrepItem* item = new kmb::MeshBrepItem();
			item->bodyId = bodyId;
			item->dim = 3;
			brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(bodyId,item) );
			brep->volumes.push_back(item);
			bext.clear();
			bext.setMesh(mesh);
			bext.appendBody(bodyId);
			if( angle > 0.0 ){
				std::stringstream ss;
				ss << "Face" << bodyId << "_";
				bext.getBoundaryFace(bodyId,ss.str().c_str());
				surfOp.setMesh(mesh);
				if( ridge ){
					std::map< std::string, kmb::bodyIdType > subfaces;
					surfOp.divideFaceGroupWithRidge(ss.str(),angle,subfaces);
					std::map< std::string, kmb::bodyIdType >::iterator sIter = subfaces.begin();
					while( sIter != subfaces.end() ){
						if( sIter->first == ss.str() ){
							item->outlineId = sIter->second;
						}else{
							kmb::MeshBrepItem* child = new kmb::MeshBrepItem();
							child->faceGroup = sIter->first;
							child->outlineId = sIter->second;
							child->parent = item;
							child->dim = 3;
							item->children.push_back(child);
							brep->mapperByGroup.insert( std::pair< std::string, kmb::MeshBrepItem* >(sIter->first,child));
							brep->surfaces.push_back(child);
						}
						++sIter;
					}
				}else{
					std::vector< std::string > subfaces;
					surfOp.divideFaceGroup(ss.str(),angle,subfaces);
					std::vector< std::string >::iterator sIter = subfaces.begin();
					while( sIter != subfaces.end() ){
						kmb::MeshBrepItem* child = new kmb::MeshBrepItem();
						child->faceGroup = *sIter;
						child->parent = item;
						child->dim = 3;
						item->children.push_back(child);
						brep->mapperByGroup.insert( std::pair< std::string, kmb::MeshBrepItem* >(*sIter,child));
						brep->surfaces.push_back(child);
						++sIter;
					}
				}
				mesh->deleteData(ss.str().c_str());
			}else{
				std::stringstream ss;
				ss << "Face" << bodyId;
				bext.getBoundaryFace(bodyId,ss.str().c_str());
				kmb::MeshBrepItem* child = new kmb::MeshBrepItem();
				child->faceGroup = ss.str();
				item->children.push_back(child);
				child->parent = item;
				child->dim = 3;
				brep->mapperByGroup.insert( std::pair< std::string, kmb::MeshBrepItem* >(ss.str(),child));
				brep->surfaces.push_back(child);
			}
		}
	}
	return brep;
}

kmb::MeshBrepInfo* kmb::MeshBrepInfo::create3DModelWithBoundary(kmb::MeshData *mesh,bool reverse)
{
	kmb::MeshBrepInfo* brep = new kmb::MeshBrepInfo();
	kmb::bodyIdType bodyCount = mesh->getBodyCount();
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;bodyId++){
		if( mesh->getDimension(bodyId) == 2 ){
			brep->topDim = 2;
			kmb::MeshBrepItem* item = new kmb::MeshBrepItem();
			item->bodyId = bodyId;
			item->dim = 2;
			brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(bodyId,item) );
			brep->surfaces.push_back(item);
		}
	}
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;bodyId++){
		if( mesh->getDimension(bodyId) == 3 ){
			brep->topDim = 3;
			kmb::MeshBrepItem* item = new kmb::MeshBrepItem();
			item->bodyId = bodyId;
			item->dim = 3;
			brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(bodyId,item) );
			brep->volumes.push_back(item);
			// 親子関係の構築
			kmb::BoundaryExtractor bext;
			bext.setReverseMode(reverse);
			bext.setMesh(mesh);
			bext.appendBody(bodyId);
			kmb::Face f;
			for(kmb::bodyIdType surfaceId=0;surfaceId<bodyCount;surfaceId++){
				if( mesh->getDimension(surfaceId) == 2 ){
					kmb::ElementContainer* body = mesh->getBodyPtr(surfaceId);
					if( body != NULL ){
						// 最初の要素だけで判定する
						kmb::ElementContainer::const_iterator eIter = body->begin();
						if( !eIter.isFinished() && bext.getBoundaryFace(eIter,f) ){
							item->children.push_back( brep->mapperById.find(surfaceId)->second );
						}
					}
				}
			}
		}
	}
	return brep;
}

kmb::MeshBrepInfo* kmb::MeshBrepInfo::createPatchModel(kmb::MeshData *mesh,bool outline)
{
	kmb::MeshBrepInfo* brep = new kmb::MeshBrepInfo();
	kmb::bodyIdType bodyCount = mesh->getBodyCount();
	for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;bodyId++){
		if( mesh->getDimension(bodyId) == 2 ){
			brep->topDim = 2;
			kmb::MeshBrepItem* item = new kmb::MeshBrepItem();
			item->bodyId = bodyId;
			item->dim = 2;
			brep->mapperById.insert( std::pair< kmb::bodyIdType, kmb::MeshBrepItem* >(bodyId,item) );
			brep->surfaces.push_back(item);
		}
	}
	return brep;
}

kmb::MeshBrepInfo* kmb::MeshBrepInfo::createSurfaceModel(kmb::MeshData *mesh,double angle,bool ridge)
{
	return NULL;
}

const kmb::MeshBrepItem* kmb::MeshBrepInfo::getParent(kmb::bodyIdType bodyId) const
{
	std::map< kmb::bodyIdType, kmb::MeshBrepItem* >::const_iterator bIter = mapperById.find(bodyId);
	if( bIter != mapperById.end() ){
		return bIter->second->parent;
	}else{
		return NULL;
	}
}

const kmb::MeshBrepItem* kmb::MeshBrepInfo::getParent(const char* faceGroup) const
{
	std::map< std::string, kmb::MeshBrepItem* >::const_iterator bIter = mapperByGroup.find(faceGroup);
	if( bIter != mapperByGroup.end() ){
		return bIter->second->parent;
	}else{
		return NULL;
	}
}

kmb::bodyIdType kmb::MeshBrepInfo::getOutline(kmb::bodyIdType bodyId) const
{
	std::map< kmb::bodyIdType, kmb::MeshBrepItem* >::const_iterator bIter = mapperById.find(bodyId);
	if( bIter != mapperById.end() ){
		return bIter->second->outlineId;
	}else{
		return kmb::Body::nullBodyId;
	}
}

kmb::bodyIdType kmb::MeshBrepInfo::getOutline(const char* faceGroup) const
{
	std::map< std::string, kmb::MeshBrepItem* >::const_iterator bIter = mapperByGroup.find(faceGroup);
	if( bIter != mapperByGroup.end() ){
		return bIter->second->outlineId;
	}else{
		return kmb::Body::nullBodyId;
	}
}

const std::vector< const kmb::MeshBrepItem* >*
kmb::MeshBrepInfo::getChildren(kmb::bodyIdType bodyId) const
{
	std::map< kmb::bodyIdType, kmb::MeshBrepItem* >::const_iterator bIter = mapperById.find(bodyId);
	if( bIter != mapperById.end() ){
		return &bIter->second->children;
	}else{
		return NULL;
	}
}

const std::vector< const kmb::MeshBrepItem* >*
kmb::MeshBrepInfo::getChildren(const char* faceGroup) const
{
	std::map< std::string, kmb::MeshBrepItem* >::const_iterator bIter = mapperByGroup.find(faceGroup);
	if( bIter != mapperByGroup.end() ){
		return &bIter->second->children;
	}else{
		return NULL;
	}
}

size_t kmb::MeshBrepInfo::getChildren(kmb::bodyIdType bodyId,std::vector<kmb::bodyIdType>& children) const
{
	std::map< kmb::bodyIdType, kmb::MeshBrepItem* >::const_iterator bIter = mapperById.find(bodyId);
	if( bIter != mapperById.end() ){
		size_t count=0;
		std::vector< const kmb::MeshBrepItem* >::const_iterator cIter = bIter->second->children.begin();
		while( cIter != bIter->second->children.end() ){
			kmb::bodyIdType child = (*cIter)->getBodyId();
			if( child != kmb::Body::nullBodyId ){
				children.push_back( child );
				++count;
			}
			++cIter;
		}
		return count;
	}else{
		return 0;
	}
}

const std::vector< const kmb::MeshBrepItem* >* kmb::MeshBrepInfo::getVolumes(void) const
{
	return &volumes;
}

const std::vector< const kmb::MeshBrepItem* >* kmb::MeshBrepInfo::getSurfaces(void) const
{
	return &surfaces;
}

const std::vector< const kmb::MeshBrepItem* >* kmb::MeshBrepInfo::getEdges(void) const
{
	return &edges;
}

size_t kmb::MeshBrepInfo::getVolumes(std::vector<kmb::bodyIdType>& bodyIds) const
{
	std::vector< const kmb::MeshBrepItem* >::const_iterator cIter = volumes.begin();
	size_t count=0;
	while( cIter != volumes.end() ){
		kmb::bodyIdType child = (*cIter)->getBodyId();
		if( child != kmb::Body::nullBodyId ){
			bodyIds.push_back( child );
			++count;
		}
		++cIter;
	}
	return count;
}

size_t kmb::MeshBrepInfo::getSurfaces(std::vector<kmb::bodyIdType>& bodyIds) const
{
	std::vector< const kmb::MeshBrepItem* >::const_iterator cIter = surfaces.begin();
	size_t count=0;
	while( cIter != surfaces.end() ){
		kmb::bodyIdType child = (*cIter)->getBodyId();
		if( child != kmb::Body::nullBodyId ){
			bodyIds.push_back( child );
			++count;
		}
		++cIter;
	}
	return count;
}

size_t kmb::MeshBrepInfo::getEdges(std::vector<kmb::bodyIdType>& bodyIds) const
{
	std::vector< const kmb::MeshBrepItem* >::const_iterator cIter = edges.begin();
	size_t count=0;
	while( cIter != edges.end() ){
		kmb::bodyIdType child = (*cIter)->getBodyId();
		if( child != kmb::Body::nullBodyId ){
			bodyIds.push_back( child );
			++count;
		}
		++cIter;
	}
	return count;
}
