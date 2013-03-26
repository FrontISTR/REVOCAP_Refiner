/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementBucket                                           #
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
#include "MeshDB/kmbElementBucket.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbDataBindings.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbGeometry4D.h"
#include "Common/kmbCalculator.h"

kmb::ElementBucket::ElementBucket(void)
: kmb::Bucket<kmb::elementIdType>()
, points(NULL)
, elements(NULL)
, coordMatrix(NULL)
, evaluator(NULL)
, margin(1.1)
{
}

kmb::ElementBucket::ElementBucket(const kmb::BoxRegion &box,int xnum,int ynum,int znum)
: kmb::Bucket<kmb::elementIdType>(box,xnum,ynum,znum)
, points(NULL)
, elements(NULL)
, coordMatrix(NULL)
, evaluator(NULL)
, margin(1.1)
{
}

kmb::ElementBucket::~ElementBucket(void)
{
	if( evaluator ){
		delete evaluator;
		evaluator = NULL;
	}
}

void
kmb::ElementBucket::setContainer(const kmb::Point3DContainer* points,const kmb::ElementContainer* elements,const kmb::Matrix4x4* coordMatrix)
{
	this->points = points;
	this->elements = elements;
	this->coordMatrix = coordMatrix;
	if( evaluator ){
		delete evaluator;
		evaluator = NULL;
	}
	this->evaluator = new kmb::ElementEvaluator(points);
}



int
kmb::ElementBucket::append(kmb::elementIdType elementId)
{
	if( elements == NULL || points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::ElementContainer::const_iterator eIter = elements->find(elementId);
	if( !eIter.isFinished() ){
		kmb::BoundingBox box;
		kmb::Point3D pt;
		int len = eIter.getNodeCount();
		bool flag = true;
		for(int i=0;i<len;++i){
			if( points->getPoint( eIter[i], pt ) ){
				if( coordMatrix ){
					coordMatrix->convert( pt );
				}
				box.update(pt);
			}else{
				flag = false;
			}
		}
		if( flag ){
			box.expand(margin);
			int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
			getSubIndices( box.getMin().x(), box.getMin().y(), box.getMin().z(), i0, j0, k0 );
			getSubIndices( box.getMax().x(), box.getMax().y(), box.getMax().z(), i1, j1, k1 );
			for(int i=i0;i<=i1;++i){
				for(int j=j0;j<=j1;++j){
					for(int k=k0;k<=k1;++k){
						++count;
						insert( i,j,k,elementId );
					}
				}
			}
		}
	}
	return count;
}

int
kmb::ElementBucket::appendAll(void)
{
	if( elements == NULL || points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::BoundingBox box;
	kmb::Point3D pt;
	kmb::ElementContainer::const_iterator eIter = elements->begin();
	while( !eIter.isFinished() ){
		box.initialize();
		int len = eIter.getNodeCount();
		bool flag = true;
		for(int i=0;i<len;++i){
			if( points->getPoint( eIter[i], pt ) ){
				if( coordMatrix ){
					coordMatrix->convert( pt );
				}
				box.update(pt);
			}else{
				flag = false;
			}
		}
		if( flag ){
			box.expand(margin);
			int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
			getSubIndices( box.getMin().x(), box.getMin().y(), box.getMin().z(), i0, j0, k0 );
			getSubIndices( box.getMax().x(), box.getMax().y(), box.getMax().z(), i1, j1, k1 );
			for(int i=i0;i<=i1;++i){
				for(int j=j0;j<=j1;++j){
					for(int k=k0;k<=k1;++k){
						++count;
						insert( i,j,k,eIter.getId() );
					}
				}
			}
		}
		++eIter;
	}
	return count;
}

int
kmb::ElementBucket::appendAllElement(const kmb::ElementContainer* body)
{
	if( body == NULL || points == NULL ){
		return 0;
	}
	this->elements = body;
	int count = 0;
	kmb::BoundingBox box;
	kmb::Point3D pt;
	kmb::ElementContainer::const_iterator eIter = elements->begin();
	while( !eIter.isFinished() ){
		box.initialize();
		int len = eIter.getNodeCount();
		bool flag = true;
		for(int i=0;i<len;++i){
			if( points->getPoint( eIter[i], pt ) ){
				if( coordMatrix ){
					coordMatrix->convert( pt );
				}
				box.update(pt);
			}else{
				flag = false;
			}
		}
		if( flag ){
			box.expand(margin);
			int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
			getSubIndices( box.getMin().x(), box.getMin().y(), box.getMin().z(), i0, j0, k0 );
			getSubIndices( box.getMax().x(), box.getMax().y(), box.getMax().z(), i1, j1, k1 );
			for(int i=i0;i<=i1;++i){
				for(int j=j0;j<=j1;++j){
					for(int k=k0;k<=k1;++k){
						++count;
						insert( i,j,k,eIter.getId() );
					}
				}
			}
		}
		++eIter;
	}
	return count;
}

kmb::elementIdType
kmb::ElementBucket::searchElement(double x,double y,double z,double tolerance) const
{
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	int i0=0,j0=0,k0=0;
	if( getSubIndices(x,y,z,i0,j0,k0) == false ){
		return elementId;
	}
	kmb::Maximizer maximizer;
	kmb::ElementBucket::const_iterator eIter = begin(i0,j0,k0);
	kmb::ElementBucket::const_iterator endIter = end(i0,j0,k0);
	while( eIter != endIter ){
		kmb::ElementContainer::const_iterator elem = elements->find( eIter.get() );
		if( !elem.isFinished() ){

			if( maximizer.update( evaluator->getMinInnerVolume( elem, x, y, z ) ) ){
				elementId = elem.getId();
			}
		}
		++eIter;
	}
	if( maximizer.getMax() < tolerance ){
		elementId = kmb::Element::nullElementId;
	}
	return elementId;
}

kmb::elementIdType
kmb::ElementBucket::searchElementInBody(const kmb::ElementContainer* body,double x,double y,double z,double tolerance) const
{
	if( body == NULL ){
		return kmb::Element::nullElementId;
	}
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	int i0=0,j0=0,k0=0;
	if( getSubIndices(x,y,z,i0,j0,k0) == false ){
		return elementId;
	}
	kmb::Minimizer minimizer;
	kmb::ElementBucket::const_iterator eIter = begin(i0,j0,k0);
	kmb::ElementBucket::const_iterator endIter = end(i0,j0,k0);
	while( eIter != endIter ){
		kmb::ElementContainer::const_iterator elem = body->find( eIter.get() );
		if( !elem.isFinished() ){

			double dist = 0.0;
			if( evaluator->getMinInnerVolume(elem,x,y,z) >= 0.0 ){
				dist = 0.0;
			}else{
				dist = evaluator->getDistanceSqOnBoundary(elem,x,y,z);
			}
			if( minimizer.update( dist ) ){
				elementId = elem.getId();
			}
		}
		++eIter;
	}

	if( minimizer.getMin() > tolerance*tolerance ){
		elementId = kmb::Element::nullElementId;
	}
	return elementId;
}

kmb::elementIdType
kmb::ElementBucket::searchElementInData(const kmb::DataBindings* data,const kmb::ElementContainer* body,double x,double y,double z,double tolerance) const
{
	if( body == NULL || data == NULL ||
		( data->getBindingMode() != kmb::DataBindings::ElementGroup && data->getBindingMode() != kmb::DataBindings::FaceGroup ) )
	{
		return kmb::Element::nullElementId;
	}

	kmb::elementIdType elementId = kmb::Element::nullElementId;
	int i0=0,j0=0,k0=0;
	if( getSubIndices(x,y,z,i0,j0,k0) == false ){
		return elementId;
	}
	kmb::Minimizer minimizer;
	kmb::ElementBucket::const_iterator eIter = begin(i0,j0,k0);
	kmb::ElementBucket::const_iterator endIter = end(i0,j0,k0);
	if( data->getBindingMode() == kmb::DataBindings::ElementGroup ){
		while( eIter != endIter ){
			if( data->hasId( eIter.get() ) ){
				kmb::ElementContainer::const_iterator elem = body->find( eIter.get() );
				if( !elem.isFinished() ){

					double dist = 0.0;
					if( evaluator->getMinInnerVolume(elem,x,y,z) >= 0.0 ){
						dist = 0.0;
					}else{
						dist = evaluator->getDistanceSqOnBoundary(elem,x,y,z);
					}
					if( minimizer.update( dist ) ){
						elementId = elem.getId();
					}
				}
			}
			++eIter;
		}
	}else if( data->getBindingMode() == kmb::DataBindings::FaceGroup ){
		while( eIter != endIter ){
			kmb::ElementContainer::const_iterator elem = body->find( eIter.get() );
			if( !elem.isFinished() ){
				int faceCount = elem.getBoundaryCount();
				bool flag = false;
				for(int i=0;i<faceCount;++i){
					kmb::Face f(eIter.get(),i);
					if( data->hasId( f ) ){
						flag = true;
						break;
					}
				}
				if( flag ){

					double dist = 0.0;
					if( evaluator->getMinInnerVolume(elem,x,y,z) >= 0.0 ){
						dist = 0.0;
					}else{
						dist = evaluator->getDistanceSqOnBoundary(elem,x,y,z);
					}
					if( minimizer.update( dist ) ){
						elementId = elem.getId();
					}
				}
			}
			++eIter;
		}
	}
	if( minimizer.getMin() > tolerance*tolerance ){
		elementId = kmb::Element::nullElementId;
	}
	return elementId;
}
