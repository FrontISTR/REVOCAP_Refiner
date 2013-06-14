/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Bucket                                                  #
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
#include "Geometry/kmbBucket.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Common/kmbCalculator.h"
#include <cmath>



kmb::Bucket3DGrid::Bucket3DGrid(void)
: kmb::Bucket<kmb::nodeIdType>()
, points(NULL)
{
}

kmb::Bucket3DGrid::Bucket3DGrid(const kmb::BoxRegion &box,int xnum,int ynum,int znum)
: kmb::Bucket<kmb::nodeIdType>(box,xnum,ynum,znum)
, points(NULL)
{
}

kmb::Bucket3DGrid::~Bucket3DGrid(void)
{
}

void
kmb::Bucket3DGrid::setContainer( const kmb::Point3DContainer* points )
{
	this->points = points;
}

int
kmb::Bucket3DGrid::append(kmb::nodeIdType nodeId)
{
	if( points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::Point3DContainer::const_iterator pIter = points->find(nodeId);
	if( !pIter.isFinished() ){
		kmb::Point3D pt;
		int i=0,j=0,k=0;
		if( pIter.getPoint( pt ) && getSubIndices(pt.x(),pt.y(),pt.z(),i,j,k) ){
			insert(i,j,k,nodeId);
			++count;
		}
	}
	return count;
}

int
kmb::Bucket3DGrid::appendAll(void)
{
	if( points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::Point3DContainer::const_iterator pIter = points->begin();
	while( !pIter.isFinished() ){
		kmb::Point3D pt;
		int i=0,j=0,k=0;
		kmb::nodeIdType nodeId = pIter.getId();
		if( pIter.getPoint( pt ) && getSubIndices(pt.x(),pt.y(),pt.z(),i,j,k) ){
			insert(i,j,k,nodeId);
			++count;
		}
		++pIter;
	}
	return count;
}

double
kmb::Bucket3DGrid::getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,kmb::nodeIdType &nearestId) const
{
	kmb::Minimizer minimizer;
	kmb::Point3D point;
	kmb::Bucket3DGrid::const_iterator pIter = this->begin(i,j,k);
	kmb::Bucket3DGrid::const_iterator endIter = this->end(i,j,k);
	while( pIter != endIter ){
		kmb::nodeIdType nodeId = pIter.get();
		if( this->points->getPoint( nodeId, point ) && minimizer.update( pt.distanceSq(point) ) ){
			nearestId = nodeId;
		}
		++pIter;
	}
	return sqrt( minimizer.getMin() );
}

size_t
kmb::Bucket3DGrid::getNearPoints(kmb::nodeIdType nodeId,double thresh,std::vector<kmb::nodeIdType>& nodeIds) const
{
	if( points == NULL ){
		return 0;
	}
	kmb::Point3D pt;
	points->getPoint(nodeId,pt);
	double x = pt.x();
	double y = pt.y();
	double z = pt.z();


	int i0,j0,k0;
	getSubIndices(x,y,z,i0,j0,k0);
	int i1=i0, j1=j0, k1=k0;
	kmb::BoxRegion region;
	getSubRegion(i0,j0,k0,region);
	if( region.maxX() -x < thresh ){
		i1++;
	}
	if( x - region.minX() < thresh ){
		i0--;
	}
	if( region.maxY() -y < thresh ){
		j1++;
	}
	if( y - region.minY() < thresh ){
		j0--;
	}
	if( region.maxZ() -z < thresh ){
		k1++;
	}
	if( z - region.minZ() < thresh ){
		k0--;
	}
	for(int i=i0;i<=i1;i++){
		for(int j=j0;j<=j1;j++){
			for(int k=k0;k<=k1;k++){
				kmb::Point3D targetPt;
				kmb::Bucket3DGrid::const_iterator pIter = this->begin(i,j,k);
				kmb::Bucket3DGrid::const_iterator endIter = this->end(i,j,k);
				while( pIter != endIter ){
					kmb::nodeIdType targetId = pIter.get();
					if( targetId != nodeId ){
						if( this->points->getPoint( targetId, targetPt ) && pt.distanceSq(targetPt) < thresh*thresh ){
							nodeIds.push_back( targetId );
						}
					}
					++pIter;
				}
			}
		}
	}

	return nodeIds.size();
}

double
kmb::Bucket3DGrid::getNearest(const double x,const double y,const double z,kmb::nodeIdType &nearestId) const
{
	kmb::Minimizer minimizer;
	kmb::Point3D pt(x,y,z);
	nearestId = kmb::nullNodeId;
	kmb::nodeIdType n0 = kmb::nullNodeId;

/*

	int i0=0,j0=0,k0=0;

	if( getSubIndices(x,y,z,i0,j0,k0) && this->getCount(i0,j0,k0) > 0){
		if( minimizer.update( getNearestInBucket(pt,i0,j0,k0,n0) ) ){
			nearestId = n0;
		}
	}

	int i1=0,j1=0,k1=0,i2=0,j2=0,k2=0;
	if( nearestId == kmb::nullNodeId ){

		i2 = xnum-1;
		j2 = ynum-1;
		k2 = znum-1;
	}else{
		double d = sqrt( minimizer.getMin() );
		getSubIndices(x-d,y-d,z-d,i1,j1,k1);
		getSubIndices(x+d,y+d,z+d,i2,j2,k2);
	}


	for(int i=i1;i<=i2;++i){
		for(int j=j1;j<=j2;++j){
			for(int k=k1;k<=k2;++k){
				if( i != i0 || j != j0 || k != k0 ){
					if( minimizer.update( getNearestInBucket(pt,i,j,k,n0) ) ){
						nearestId = n0;
					}
				}
			}
		}
	}
	return minimizer.getMin();
*/


	int imax=0,jmax=0,kmax=0,imin=0,jmin=0,kmin=0;


	int imax_ex=-1,jmax_ex=-1,kmax_ex=-1,imin_ex=xnum,jmin_ex=ynum,kmin_ex=znum;


	getSubIndices(x,y,z,imin,jmin,kmin);
	imax = imin+1; jmax = jmin+1; kmax = kmin+1;
	while( nearestId == kmb::nullNodeId ){
		if( minimizer.update( this->getNearestInBucket2(pt,imin,jmin,kmin,imax,jmax,kmax,imin_ex,jmin_ex,kmin_ex,imax_ex,jmax_ex,kmax_ex,n0) ) ){
			nearestId = n0;
		}
		imin_ex = imin; imax_ex = imax;
		jmin_ex = jmin; jmax_ex = jmax;
		kmin_ex = kmin; kmax_ex = kmax;
		bool breakFlag = true;
		if( imin > 0 ){ --imin; breakFlag = false; }
		if( jmin > 0 ){ --jmin; breakFlag = false; }
		if( kmin > 0 ){ --kmin; breakFlag = false; }
		if( imax < xnum-1 ){ ++imax; breakFlag = false; }
		if( jmax < ynum-1 ){ ++jmax; breakFlag = false; }
		if( kmax < znum-1 ){ ++kmax; breakFlag = false; }
		if( breakFlag ){

			break;
		}
	}

	if( nearestId == kmb::nullNodeId ){

		return minimizer.getMin();
	}else{
		double d = minimizer.getMin();
		getSubIndices(x-d,y-d,z-d,imin,jmin,kmin);
		getSubIndices(x+d,y+d,z+d,imax,jmax,kmax);
		if( minimizer.update( this->getNearestInBucket2(pt,imin,jmin,kmin,imax+1,jmax+1,kmax+1,imin_ex,jmin_ex,kmin_ex,imax_ex,jmax_ex,kmax_ex,n0) ) ){
			nearestId = n0;
		}
	}

	return minimizer.getMin();
}



double
kmb::Bucket3DGrid::getNearestInBucket2(const kmb::Point3D& pt,
	int imin,int jmin,int kmin,int imax,int jmax,int kmax,
	int imin_ex,int jmin_ex,int kmin_ex,int imax_ex,int jmax_ex,int kmax_ex,
	kmb::nodeIdType &nearestId) const
{
	nearestId = kmb::nullNodeId;
	kmb::nodeIdType n0 = kmb::nullNodeId;
	kmb::Minimizer minimizer;

	for(int i=imin;i<imax;++i){
		for(int j=jmin;j<jmax;++j){
			for(int k=kmin;k<kmax;++k){
				if( imin_ex <= i && i < imax_ex &&
					jmin_ex <= j && j < jmax_ex &&
					kmin_ex <= k && k < kmax_ex )
				{
					continue;
				}
				if( minimizer.update( getNearestInBucket(pt,i,j,k,n0) ) ){
					nearestId = n0;
				}
			}
		}
	}

	return minimizer.getMin();
}
