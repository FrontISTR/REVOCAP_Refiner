/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : FittingToSurface                                        #
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
#include "Shape/kmbMiddleNodeManagerWithShape.h"
#include "Shape/kmbSurface3D.h"
#include "MeshDB/kmbVector2WithIntBindings.h"
#include "MeshDB/kmbNodeMapperBindings.h"

kmb::MiddleNodeManagerWithShape::MiddleNodeManagerWithShape(void)
: kmb::MiddleNodeManager()
, mappingToSurface(NULL)
, surfaces(NULL)
, nearestFlag(false)
{
}

kmb::MiddleNodeManagerWithShape::~MiddleNodeManagerWithShape(void)
{
}

void
kmb::MiddleNodeManagerWithShape::setNearestFlag(bool b)
{
	this->nearestFlag = b;
}

void
kmb::MiddleNodeManagerWithShape::setMappingData(kmb::Vector2WithIntBindings<kmb::nodeIdType>* mapping)
{
	this->mappingToSurface = mapping;
}

void
kmb::MiddleNodeManagerWithShape::setSurfaces(const std::vector< kmb::Surface3D* >* surfaces)
{
	this->surfaces = surfaces;
}

size_t
kmb::MiddleNodeManagerWithShape::replaceNodeIds( const std::map<kmb::nodeIdType,kmb::nodeIdType> &nodeMapper )
{
	if( mappingToSurface == NULL ){
		return 0;
	}
	return mappingToSurface->replaceIds( nodeMapper );
}

kmb::nodeIdType
kmb::MiddleNodeManagerWithShape::createMiddleNode(kmb::nodeIdType n0, kmb::nodeIdType n1)
{
	if( mappingToSurface ){
		std::vector< kmb::Vector2WithIntBindings<nodeIdType>::valueType > v0;
		std::vector< kmb::Vector2WithIntBindings<nodeIdType>::valueType > v1;
		kmb::Surface3D* surface = NULL;
		kmb::Point3D pt;
		double u,v;

		if( mappingToSurface->isCommonIntval(n0,n1,v0,v1) > 0 )
		{
			if( nearestFlag ){
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				for(unsigned int i=0;i<v0.size();++i){
					if( (surface = surfaces->at(v0[i].l)) != NULL && surface->getMiddlePointByNearest( v0[i].u, v0[i].v, v1[i].u, v1[i].v, u, v, pt ) ){
						if( nodeId == kmb::nullNodeId ){
							nodeId = this->points->addPoint( pt );
						}


						mappingToSurface->setValue(nodeId, u, v, v0[i].l );
					}
				}
				if( nodeId != kmb::nullNodeId ){
					return nodeId;
				}
			}else{
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				for(unsigned int i=0;i<v0.size();++i){
					if( (surface = surfaces->at(v0[i].l)) != NULL && surface->getMiddlePoint( v0[i].u, v0[i].v, v1[i].u, v1[i].v, u, v, pt ) ){
						if( nodeId == kmb::nullNodeId ){
							nodeId = this->points->addPoint( pt );
						}


						mappingToSurface->setValue(nodeId, u, v, v0[i].l );
					}
				}
				if( nodeId != kmb::nullNodeId ){
					return nodeId;
				}
			}
		}else{

		}
	}

	return kmb::MiddleNodeManager::createMiddleNode(n0,n1);
}


kmb::nodeIdType
kmb::MiddleNodeManagerWithShape::createMiddleNode4(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3)
{
	if( mappingToSurface ){
		double u0[2] = {0.0,0.0}, u1[2] = {0.0,0.0}, u2[2] = {0.0,0.0}, u3[2] = {0.0,0.0};
		long index02 = 0L, index13 = 0L;
		kmb::Surface3D* surface = NULL;
		kmb::Point3D pt;

		if( mappingToSurface->isCommonIntval(n0,n2,index02,u0,u2) &&
			mappingToSurface->isCommonIntval(n1,n3,index13,u1,u3) &&
			index02 == index13 &&
			(surface = surfaces->at(index02)) != NULL &&
			surface->getPoint( 0.25*(u0[0] + u1[0] + u2[0] + u3[0]), 0.25*(u0[1] + u1[1] + u2[1] + u3[1]), pt ) )
		{
			kmb::nodeIdType nodeId =this->points->addPoint( pt );
			mappingToSurface->setValue(nodeId, 0.25*(u0[0] + u1[0] + u2[0] + u3[0]), 0.25*(u0[1] + u1[1] + u2[1] + u3[1]), index02 );
			return nodeId;
		}
	}
	return kmb::MiddleNodeManager::createMiddleNode4(n0,n1,n2,n3);
}
