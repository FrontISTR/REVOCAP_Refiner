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
// メッシュの位相に関する処理

#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshOperation.h"
#include "MeshDB/kmbMatching.h"

#include "Geometry/kmbFramedPlane.h"

//------------------- 近傍情報取得 ------------------------

void
kmb::MeshDB::getSurroundingElements(nodeIdType nodeId,std::vector<elementIdType>& elements,bool cacheOnly) const
{
	if( this->neighborInfo.getSize() > 0 ){
		this->neighborInfo.getSurroundingElements(nodeId,elements);
	}else if(cacheOnly == false){
		const kmb::bodyIdType bodyCount = this->getBodyCount();
		for( kmb::bodyIdType id = 0; id < bodyCount; ++id){
			const kmb::ElementContainer* body = this->getBodyPtr(id);
			if( body != NULL ){
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( eIter != body->end() )
				{
					if( eIter.include( nodeId ) )
					{
						elements.push_back( eIter.getId() );
					}
					++eIter;
				}
			}
		}
	}
}


//------------------- 三角形分割 --------------------------

void
kmb::MeshDB::triangulation(kmb::bodyIdType bodyId)
{
	if( meshOperation == NULL ){
		meshOperation = new kmb::MeshOperation(this);
	}
	this->meshOperation->triangulation(bodyId);
}

