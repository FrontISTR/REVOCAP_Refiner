/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FittingToSurface                                        #
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
#include "Shape/kmbMiddleNodeManagerWithShape.h"
#include "Shape/kmbSurface3D.h"
#include "MeshDB/kmbVector2WithIntBindings.h"
#include "MeshDB/kmbNodeMapperBindings.h"
#include "MeshGen/kmbMeshSmoother.h"
#include "Common/kmbCalculator.h"

kmb::MiddleNodeManagerWithShape::MiddleNodeManagerWithShape(void)
: kmb::MiddleNodeManager()
, mappingToSurface(NULL)
, surfaces(NULL)
, nearestFlag(false)
, iterMax(1000)
, epsilon(1.0e-8)
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
kmb::MiddleNodeManagerWithShape::setiterMax(int m)
{
	this->iterMax = m;
}

void
kmb::MiddleNodeManagerWithShape::setEpsilon(double e)
{
	this->epsilon = e;
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

int
kmb::MiddleNodeManagerWithShape::getPointOnSurface(kmb::nodeIdType nodeId,kmb::Point3D &pt) const
{
	if( mappingToSurface == NULL ){
		return 0;
	}
	size_t count = mappingToSurface->countId(nodeId);
	int c = 0;
	double u,v;
	long l;
	kmb::Point3D q;
	pt.zero();
	for(size_t i=0;i<count;++i){
		if( mappingToSurface->getValues(nodeId,u,v,l,i) ){
			++c;
			surfaces->at(l)->getPoint(u,v,q);
			pt += q;
		}
	}
	if( c > 0 ){
		pt.scale( 1.0 / c );
	}
	return c;
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
					if( (surface = surfaces->at(v0[i].l)) != NULL ){
						surface->setEpsilon(epsilon);
						surface->setIterMax(iterMax);
						if( surface->getMiddlePointByNearest( v0[i].u, v0[i].v, v1[i].u, v1[i].v, u, v, pt ) ){
							if( nodeId == kmb::nullNodeId ){
								nodeId = this->points->addPoint( pt );
							}
							mappingToSurface->setValue(nodeId, u, v, v0[i].l );
						}
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

/*

			if( nearestFlag ){
				kmb::Node p0, p1, pt0, pt1;

				double u0(0.0), v0(0.0), u1(0.0), v1(0.0);

				double uu0(0.0), vv0(0.0), uu1(0.0), vv1(0.0);
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				if( points && points->getPoint(n0,p0) && points->getPoint(n1,p1) ){

					pt = kmb::Point3D::getCenter(p0,p1);
					long surfIndex0(-1), surfIndex1(-1);

					kmb::Minimizer minimizer;
					long fittingSurfIndex(-1);
					minimizer.update( 0.5 * p0.distance(p1) );
					if( mappingToSurface->getValues(n0,u0,v0,surfIndex0) ){

						if( (surface = surfaces->at(surfIndex0)) != NULL ){
							surface->setEpsilon(epsilon);
							surface->setIterMax(iterMax);
							if( surface->getNearest(pt,uu0,vv0) ){

								surface->getPoint(uu0,vv0,pt0);
								if( minimizer.update( pt.distance(pt0) ) ){
									fittingSurfIndex = 0;
								}
							}
						}
					}
					if( mappingToSurface->getValues(n1,u1,v1,surfIndex1) ){

						if( (surface = surfaces->at(surfIndex1)) != NULL ){
							surface->setEpsilon(epsilon);
							surface->setIterMax(iterMax);
							if( surface->getNearest(pt,uu1,vv1) ){

								surface->getPoint(uu1,vv1,pt1);
								if( minimizer.update( pt.distance(pt1) ) ){
									fittingSurfIndex = 1;
								}
							}
						}
					}
					if( fittingSurfIndex == 0 ){
						nodeId = this->points->addPoint( pt0 );
						mappingToSurface->setValue(nodeId, uu0, vv0, fittingSurfIndex );
						return nodeId;
					}else if( fittingSurfIndex == 1 ){
						nodeId = this->points->addPoint( pt1 );
						mappingToSurface->setValue(nodeId, uu1, vv1, fittingSurfIndex );
						return nodeId;
					}else{
					}
				}
			}
*/
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
