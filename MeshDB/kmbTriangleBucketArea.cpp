/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : TriangleBucketArea                                      #
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
#include "MeshDB/kmbTriangleBucketArea.h"
#include "MeshDB/kmbElementContainer.h"
#include "Geometry/kmb_Geometry4D.h"

kmb::TriangleBucketArea::TriangleBucketArea(void)
: kmb::Bucket< std::pair<kmb::elementIdType,double> >()
, points(NULL)
, elements(NULL)
, coordMatrix(NULL)
{
}

kmb::TriangleBucketArea::TriangleBucketArea(const kmb::BoxRegion &box,int xnum,int ynum,int znum)
: kmb::Bucket< std::pair<kmb::elementIdType,double> >(box,xnum,ynum,znum)
, points(NULL)
, elements(NULL)
, coordMatrix(NULL)
{
}

kmb::TriangleBucketArea::~TriangleBucketArea(void)
{
}

void
kmb::TriangleBucketArea::setContainer(const kmb::Point3DContainer* points,const kmb::ElementContainer* elements,const kmb::Matrix4x4* coordMatrix)
{
	this->points = points;
	this->elements = elements;
	this->coordMatrix = coordMatrix;
}

int
kmb::TriangleBucketArea::append(kmb::elementIdType elementId)
{
	if( elements == NULL || points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::ElementContainer::const_iterator eIter = elements->find(elementId);
	if( !eIter.isFinished() && eIter.getType() == kmb::TRIANGLE ){
		kmb::BoundingBox box;
		kmb::Point3D a,b,c;
		if( points->getPoint( eIter.getCellId(0), a ) &&
			points->getPoint( eIter.getCellId(1), b ) &&
			points->getPoint( eIter.getCellId(2), c ) )
		{
			if( coordMatrix ){
				coordMatrix->convert( a );
				coordMatrix->convert( b );
				coordMatrix->convert( c );
			}
			box.update(a);
			box.update(b);
			box.update(c);
			box.expand(1.1);
			int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
			getSubIndices( box.getMin().x(), box.getMin().y(), box.getMin().z(), i0, j0, k0 );
			getSubIndices( box.getMax().x(), box.getMax().y(), box.getMax().z(), i1, j1, k1 );
			for(int i=i0;i<=i1;++i){
				for(int j=j0;j<=j1;++j){
					for(int k=k0;k<=k1;++k){
						getSubRegion(i,j,k,box);
						double area = box.intersectArea(a,b,c);
						if( area != 0.0 ){
							++count;
							insert( i,j,k, std::pair<kmb::elementIdType,double>(elementId,area) );
						}
					}
				}
			}
		}
	}
	return count;
}

int
kmb::TriangleBucketArea::appendAll(void)
{
	if( elements == NULL || points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::BoundingBox bbox;
	kmb::BoxRegion box;
	kmb::Point3D a,b,c;
	kmb::ElementContainer::const_iterator eIter = elements->begin();
	while( !eIter.isFinished() ){
		if( eIter.getType() == kmb::TRIANGLE &&
			points->getPoint( eIter.getCellId(0), a ) &&
			points->getPoint( eIter.getCellId(1), b ) &&
			points->getPoint( eIter.getCellId(2), c ) )
		{
			if( coordMatrix ){
				coordMatrix->convert( a );
				coordMatrix->convert( b );
				coordMatrix->convert( c );
			}
			bbox.initialize();
			bbox.update(a);
			bbox.update(b);
			bbox.update(c);
			bbox.expand(1.1);
			int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
			getSubIndices( bbox.getMin().x(), bbox.getMin().y(), bbox.getMin().z(), i0, j0, k0 );
			getSubIndices( bbox.getMax().x(), bbox.getMax().y(), bbox.getMax().z(), i1, j1, k1 );
			for(int i=i0;i<=i1;++i){
				for(int j=j0;j<=j1;++j){
					for(int k=k0;k<=k1;++k){
						getSubRegion(i,j,k,box);
						double area = box.intersectArea(a,b,c);
						if( area != 0.0 ){
							++count;
							insert( i,j,k,std::pair<kmb::elementIdType,double>(eIter.getId(),area) );
						}
					}
				}
			}
		}
		++eIter;
	}
	return count;
}
