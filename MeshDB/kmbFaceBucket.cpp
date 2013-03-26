/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FaceBucket                                              #
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

#include "MeshDB/kmbFaceBucket.h"

#include "Geometry/kmbBoundingBox.h"
#include "Common/kmbCalculator.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbMeshData.h"

#include <cmath>

kmb::FaceBucket::FaceBucket(void)
: kmb::Bucket<kmb::Face>()
, mesh(NULL)
, faceGroup(NULL)
, bodyId(kmb::Body::nullBodyId)
{
}

kmb::FaceBucket::~FaceBucket(void)
{
}

void
kmb::FaceBucket::setContainer(const kmb::MeshData* mesh,const kmb::DataBindings* faceGroup)
{
	if( mesh==NULL || faceGroup==NULL || faceGroup->getBindingMode() != kmb::DataBindings::FaceGroup ){
		return;
	}
	this->mesh = mesh;
	this->faceGroup = faceGroup;
	this->bodyId = faceGroup->getTargetBodyId();
}


void
kmb::FaceBucket::setAutoBucketSize(void)
{
	kmb::BoundingBox bbox;
	mesh->getBoundingBoxOfData( bbox, faceGroup );

	bbox.expandDiameter( 1.5 );
	this->setRegion( bbox );


	double range[3] = { bbox.rangeX(), bbox.rangeY(), bbox.rangeZ() };
	int minIndex = 0;
	if( range[0] > range[1] ){
		if( range[1] > range[2] ){
			minIndex = 2;
		}else{
			minIndex = 1;
		}
	}else{
		if( range[0] > range[2] ){
			minIndex = 2;
		}else{
			minIndex = 0;
		}
	}
	int num = static_cast<int>( bbox.rangeX() * bbox.rangeY() * bbox.rangeZ() / range[minIndex] );
	int div = static_cast<int>( faceGroup->getIdCount() / num );


	switch(minIndex){
	case 0:
		this->setGridSize( div, static_cast<int>(div * range[1] / range[0]), static_cast<int>(div * range[2] / range[0]) );
		break;
	case 1:
		this->setGridSize( static_cast<int>(div * range[0] / range[1]), div, static_cast<int>(div * range[2] / range[1]) );
		break;
	case 2:
		this->setGridSize( static_cast<int>(div * range[0] / range[2]), static_cast<int>(div * range[2] / range[0]), div );
		break;
	}
}

int
kmb::FaceBucket::appendAll(void)
{

	int _count = 0;
	kmb::BoundingBox _bbox;
	kmb::Node node;
	kmb::Face f;
	kmb::bodyIdType bodyId = faceGroup->getTargetBodyId();
	kmb::DataBindings::const_iterator fIter = faceGroup->begin();
	while( !fIter.isFinished() ){
		if( fIter.getFace( f ) ){
			kmb::ElementContainer::const_iterator elem = mesh->findElement( f.getElementId(), bodyId );
			if( !elem.isFinished() ){
				_bbox.initialize();
				const int len = elem.getBoundaryNodeCount(f.getLocalFaceId());
				for(int i=0;i<len;++i)
				{
					if( mesh->getNode( elem.getBoundaryCellId(f.getLocalFaceId(),i), node ) ){
						_bbox.update( node );
					}
				}

				int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
				getSubIndices( _bbox.getMin().x(), _bbox.getMin().y(), _bbox.getMin().z(), i0, j0, k0 );
				getSubIndices( _bbox.getMax().x(), _bbox.getMax().y(), _bbox.getMax().z(), i1, j1, k1 );
				for(int i=i0;i<=i1;++i){
					for(int j=j0;j<=j1;++j){
						for(int k=k0;k<=k1;++k){
							insert( i,j,k,f );
						}
					}
				}
			}
		}
		++fIter;
	}
	return _count;
}

/*
void
kmb::FaceBucket::setup(const kmb::MeshData* mesh,const kmb::DataBindings* faceGroup)
{
	if( mesh==NULL || faceGroup==NULL || faceGroup->getBindingMode() != kmb::DataBindings::FaceGroup ){
		return;
	}
	this->mesh = mesh;
	this->bodyId = faceGroup->getTargetBodyId();
	kmb::BoundingBox bbox;
	mesh->getBoundingBoxOfData( bbox, faceGroup );

	bbox.expandDiameter( 1.5 );
	this->setRegion( bbox );


	double range[3] = { bbox.rangeX(), bbox.rangeY(), bbox.rangeZ() };
	int minIndex = 0;
	if( range[0] > range[1] ){
		if( range[1] > range[2] ){
			minIndex = 2;
		}else{
			minIndex = 1;
		}
	}else{
		if( range[0] > range[2] ){
			minIndex = 2;
		}else{
			minIndex = 0;
		}
	}
	int num = static_cast<int>( bbox.rangeX() * bbox.rangeY() * bbox.rangeZ() / range[minIndex] );
	int div = static_cast<int>( faceGroup->getIdCount() / num );


	switch(minIndex){
	case 0:
		this->setGridSize( div, static_cast<int>(div * range[1] / range[0]), static_cast<int>(div * range[2] / range[0]) );
		break;
	case 1:
		this->setGridSize( static_cast<int>(div * range[0] / range[1]), div, static_cast<int>(div * range[2] / range[1]) );
		break;
	case 2:
		this->setGridSize( static_cast<int>(div * range[0] / range[2]), static_cast<int>(div * range[2] / range[0]), div );
		break;
	}


	kmb::Node node;
	kmb::Face f;
	kmb::bodyIdType bodyId = faceGroup->getTargetBodyId();
	kmb::DataBindings::const_iterator fIter = faceGroup->begin();
	while( !fIter.isFinished() ){
		if( fIter.getFace( f ) ){
			kmb::ElementContainer::const_iterator elem = mesh->findElement( f.getElementId(), bodyId );
			if( !elem.isFinished() ){
				bbox.initialize();
				const int len = elem.getBoundaryNodeCount(f.getLocalFaceId());
				for(int i=0;i<len;++i)
				{
					if( mesh->getNode( elem.getBoundaryCellId(f.getLocalFaceId(),i), node ) ){
						bbox.update( node );
					}
				}

				int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
				getSubIndices( bbox.getMin().x(), bbox.getMin().y(), bbox.getMin().z(), i0, j0, k0 );
				getSubIndices( bbox.getMax().x(), bbox.getMax().y(), bbox.getMax().z(), i1, j1, k1 );
				for(int i=i0;i<=i1;++i){
					for(int j=j0;j<=j1;++j){
						for(int k=k0;k<=k1;++k){
							insert( i,j,k,f );
						}
					}
				}
			}
		}
		++fIter;
	}
}
*/

bool
kmb::FaceBucket::getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,double &dist,kmb::Face &f) const
{
	if( mesh == NULL || this->getCount(i,j,k) == 0 ){
		return false;
	}
	kmb::Minimizer minimizer;
	kmb::Node n0, n1, n2, n3;
	kmb::Bucket< kmb::Face >::const_iterator fIter = this->begin(i,j,k);
	kmb::Bucket< kmb::Face >::const_iterator endIter = this->end(i,j,k);
	while( fIter != endIter ){
		const kmb::Face f0 = fIter.get();
		kmb::ElementContainer::const_iterator elem = mesh->findElement( f0.getElementId(), bodyId );
		if( !elem.isFinished() ){
			kmb::idType localId = f0.getLocalFaceId();
			const int len = elem.getBoundaryVertexCount(localId);
			switch(len){
			case 3:
				mesh->getNode(elem.getBoundaryCellId(localId,0),n0);
				mesh->getNode(elem.getBoundaryCellId(localId,1),n1);
				mesh->getNode(elem.getBoundaryCellId(localId,2),n2);
				if( minimizer.update(pt.distanceSqToTriangle(n0,n1,n2)) )
				{
					f = f0;
				}
				break;
			case 4:
				mesh->getNode(elem.getBoundaryCellId(localId,0),n0);
				mesh->getNode(elem.getBoundaryCellId(localId,1),n1);
				mesh->getNode(elem.getBoundaryCellId(localId,2),n2);
				mesh->getNode(elem.getBoundaryCellId(localId,3),n3);
				if( minimizer.update(kmb::Minimizer::getMin(
					pt.distanceSqToTriangle( n0, n1, n2 ),
					pt.distanceSqToTriangle( n2, n3, n0 ) ) ) )
				{
					f = f0;
				}
				break;
			default:
				break;
			}
		}
		++fIter;
	}
	dist = minimizer.getMin();
	return true;
}

bool
kmb::FaceBucket::getNearest(double x,double y,double z,double &dist,kmb::Face &f) const
{
	if( mesh == NULL ){
		return false;
	}
	kmb::Minimizer minimizer;
	kmb::Point3D pt(x,y,z);
	kmb::Node n0, n1, n2, n3;
	int i0=0,j0=0,k0=0;
	getSubIndices( x, y, z, i0, j0, k0 );
	double d = 0.0;
	kmb::Face f0;
	if( getNearestInBucket(pt,i0,j0,k0,d,f0) ){
		minimizer.update( d );
		f = f0;
		int i1=0,j1=0,k1=0,i2=0,j2=0,k2=0;
		getSubIndices( x-d, y-d, z-d, i1, j1, k1 );
		getSubIndices( x+d, y+d, z+d, i2, j2, k2 );

		for(int i=i1;i<=i2;++i){
			for(int j=j1;j<=j2;++j){
				for(int k=k1;k<=k2;++k){
					if( i != i0 || j != j0 || k != k0 ){
						if( getNearestInBucket(pt,i,j,k,d,f0) && minimizer.update(d) ){
							f = f0;
						}
					}
				}
			}
		}
	}else{


		kmb::BoxRegion box;
		kmb::Bucket< kmb::Face >::const_iterator fIter = this->begin();
		kmb::Bucket< kmb::Face >::const_iterator endIter = this->end();
		while( fIter != endIter ){
			if( getSubRegion( fIter.getIndex(), box ) && box.distanceSq( pt ) > minimizer.getMin() ){
				++fIter;
				continue;
			}
			f0 = fIter.get();
			kmb::ElementContainer::const_iterator elem = mesh->findElement( f0.getElementId(), bodyId );
			if( !elem.isFinished() ){
				kmb::idType localId = f0.getLocalFaceId();
				const int len = elem.getBoundaryVertexCount(localId);
				switch(len){
				case 3:
					mesh->getNode(elem.getBoundaryCellId(localId,0),n0);
					mesh->getNode(elem.getBoundaryCellId(localId,1),n1);
					mesh->getNode(elem.getBoundaryCellId(localId,2),n2);
					if( minimizer.update(pt.distanceSqToTriangle(n0,n1,n2)) )
					{
						f = f0;
					}
					break;
				case 4:
					mesh->getNode(elem.getBoundaryCellId(localId,0),n0);
					mesh->getNode(elem.getBoundaryCellId(localId,1),n1);
					mesh->getNode(elem.getBoundaryCellId(localId,2),n2);
					mesh->getNode(elem.getBoundaryCellId(localId,3),n3);
					if( minimizer.update(kmb::Minimizer::getMin(
						pt.distanceSqToTriangle( n0, n1, n2 ),
						pt.distanceSqToTriangle( n2, n3, n0 ) ) ) )
					{
						f = f0;
					}
					break;
				default:
					break;
				}
			}
			++fIter;
		}
	}
	dist = sqrt( minimizer.getMin() );
	return true;
}
