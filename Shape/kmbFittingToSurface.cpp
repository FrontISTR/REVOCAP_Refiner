/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : FittingToSurface                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "Shape/kmbFittingToSurface.h"
#include "Shape/kmbSurface3D.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbVector2WithIntBindings.h"

#include <cassert>

kmb::FittingToSurface::FittingToSurface(void)
: mesh(NULL)

, fittingData(NULL)
, threshold(1.0e-6)

{
}

kmb::FittingToSurface::~FittingToSurface(void)
{
}













void
kmb::FittingToSurface::setThreshold( double thresh )
{
	this->threshold = thresh;
}

































































void
kmb::FittingToSurface::clearFittingData()
{
	if( mesh == NULL ){
		assert(false);
		return;
	}
	if( fittingData )
	{
		fittingData->clear();
	}
}






















void
kmb::FittingToSurface::setMesh( kmb::MeshData* mesh )
{
	if( mesh ){
		this->mesh = mesh;
	}
}

int
kmb::FittingToSurface::appendSurface( kmb::Surface3D* surface )
{
	if( surface ){
		surfaces.push_back( surface );
		return static_cast<int>(surfaces.size()) - 1;
	}else{
		return -1;
	}
}




size_t
kmb::FittingToSurface::createFittingData( kmb::bodyIdType bodyId, const char* name )
{
	REVOCAP_Debug("createFittingData\n");
	size_t count = 0;

	kmb::DataBindings* data = NULL;
	const kmb::ElementContainer* body = NULL;
	if( mesh == NULL || (body=mesh->getBodyPtr(bodyId)) == NULL ){
		return 0;
	}
	REVOCAP_Debug("createFittingData mesh body not null\n");
	data = mesh->getDataBindingsPtr( name, "shape" );
	if( data ){
		if( data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
			data->getValueType() != kmb::PhysicalValue::VECTOR2WITHINT )
		{

			return 0;
		}
	}else{
		data = mesh->createDataBindings(
			name,
			kmb::DataBindings::NODEVARIABLE,
			kmb::PhysicalValue::VECTOR2WITHINT,
			"shape"
			);
	}
	fittingData = reinterpret_cast< kmb::Vector2WithIntBindings<kmb::nodeIdType>* >(data);
	double u=-1.0, v=-1.0;
	double thsq = this->threshold * this->threshold;
	REVOCAP_Debug("createFittingData fitting data not null\n");


	int k = 0;
	kmb::Point3D pt,pt2;
	for( std::vector<kmb::Surface3D*>::iterator sIter = surfaces.begin();
		sIter != surfaces.end();
		++sIter)
	{
		kmb::Surface3D* surf = *sIter;
		if( surf != NULL ){
			kmb::BoundingBox bbox;
			surf->getBoundingBox(bbox);
			if( body->getBoundingBox().distanceSq(bbox) < 0.01 * bbox.diameter() ){
				REVOCAP_Debug_3("createFittingData fitting %d => %d\n", bodyId, k);
				bbox.expand( 1.1 );
				kmb::ElementContainer::const_iterator eIter = body->begin();
				while( !eIter.isFinished() )
				{
					const int len = eIter.getNodeCount();
					for(int i=0;i<len;++i)
					{
						const kmb::nodeIdType nodeId = eIter.getCellId(i);
						if( !fittingData->hasId(nodeId,k) ){
							if( mesh->getNode( nodeId, pt ) && bbox.intersect(pt) != kmb::Region::OUTSIDE ){

								if( surf->getNearest( pt, u, v ) ){
									if( surf->getPoint( u, v, pt2 ) && pt.distanceSq(pt2) < thsq ){
										fittingData->setValue( nodeId, u, v, k );
										++count;
									}
								}else{
									REVOCAP_Debug_3("no fitting data (nodeId, surfaceId) = (%d %d)\n", nodeId, k);
								}
							}
						}
					}
					++eIter;
				}
			}
		}
		++k;
	}


/*
	this->surfaceId = surfaceId;
	long k = static_cast<long>(surfaceId);
	kmb::Point3D pt,pt2;
	kmb::BoundingBox bbox;
	surface->getBoundingBox( bbox );
	REVOCAP_Debug_3(" surface bounding box (%f, %f, %f) - (%f, %f, %f)\n",
		bbox.minX(), bbox.minY(), bbox.minZ(), bbox.maxX(), bbox.maxY(), bbox.maxZ() );
	bbox.expand( 1.1 );
	kmb::ElementContainer::const_iterator eIter = body->begin();
	while( !eIter.isFinished() )
	{
		const int len = eIter.getNodeCount();
		for(int i=0;i<len;++i)
		{
			const kmb::nodeIdType nodeId = eIter.getCellId(i);
			if( !fittingData->hasId(nodeId,k) ){
				if( mesh->getNode( nodeId, pt ) && bbox.intersect(pt) != kmb::Region::OUTSIDE ){

					if( surface->getNearest( pt, u, v ) ){
						if( surface->getPoint( u, v, pt2 ) && pt.distanceSq(pt2) < thsq ){
							REVOCAP_Debug_3("fitting data (nodeId, surfaceId) = (%d %d) => %f %f\n", nodeId, k, u, v);
							fittingData->setValue( nodeId, u, v, k );
							++count;
						}
					}else{
						REVOCAP_Debug_3("no fitting data (nodeId, surfaceId) = (%d %d)\n", nodeId, k);
					}
				}
			}
		}
		++eIter;
	}
*/
	return count;
}

#undef _DEBUG_
#include "Geometry/kmb_Debug.h"
