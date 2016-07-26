/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FaceBucketArea                                          #
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

#include "MeshDB/kmbFaceBucketArea.h"

#include "Geometry/kmbBoundingBox.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbGeometry3D.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbMeshData.h"

#include <cmath>

kmb::FaceBucketArea::FaceBucketArea(void)
: kmb::Bucket< std::pair<kmb::Face,double> >()
, mesh(NULL)
, faceGroup(NULL)
, bodyId(kmb::Body::nullBodyId)
, displace(NULL)
, faceArea(NULL)
{
}

kmb::FaceBucketArea::~FaceBucketArea(void)
{
	if( faceArea ){
		delete faceArea;
		faceArea = NULL;
	}
}

void
kmb::FaceBucketArea::setContainer(const kmb::MeshData* mesh,const kmb::DataBindings* faceGroup,const kmb::DataBindings* displace)
{
	if( mesh && faceGroup ){
		this->mesh = mesh;
		this->faceGroup = faceGroup;
		this->bodyId = faceGroup->getTargetBodyId();
	}
	if( displace ){
		this->displace = displace;
	}
	if( faceArea ){
		delete faceArea;
		faceArea = NULL;
	}
	faceArea = kmb::DataBindings::createDataBindings(kmb::DataBindings::FaceVariable,kmb::PhysicalValue::Scalar,"");
}

void
kmb::FaceBucketArea::setAutoBucketSize(void)
{
	if( mesh==NULL || faceGroup==NULL || faceGroup->getBindingMode() != kmb::DataBindings::FaceGroup ){
		return;
	}
	kmb::BoundingBox bbox;
	mesh->getBoundingBoxOfData( bbox, faceGroup );
	// 薄板の場合に厚みが出るようにするため
	bbox.expandDiameter( 1.5 );
	this->setRegion( bbox );

	// 一番短いところを 1 としたときの bucket の個数を求める
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
	// minIndex のところを div 分割
	// 他のところはほぼ同じ長さになるように分割
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
kmb::FaceBucketArea::appendAll(void)
{
	int count = 0;
	double totalArea = 0.0;
	double totalFaceArea = 0.0;
	kmb::Face f;
	kmb::BoundingBox bbox;
	kmb::BoxRegion box;
	kmb::nodeIdType n0, n1, n2, n3;
	kmb::Point3D q0, q1, q2, q3;
	double a;
	double v0[3], v1[3], v2[3], v3[3];
	kmb::bodyIdType bodyId = faceGroup->getTargetBodyId();
	kmb::DataBindings::const_iterator fIter = faceGroup->begin();
	while( !fIter.isFinished() ){
		if( fIter.getFace( f ) ){
			kmb::ElementContainer::const_iterator elem = mesh->findElement( f.getElementId(), bodyId );
			if( !elem.isFinished() ){
				kmb::idType localId = f.getLocalFaceId();
				switch( elem.getBoundaryType(localId) ){
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					n0 = elem.getBoundaryCellId(localId,0);
					n1 = elem.getBoundaryCellId(localId,1);
					n2 = elem.getBoundaryCellId(localId,2);
					if(
						mesh->getNode( n0, q0 ) &&
						mesh->getNode( n1, q1 ) &&
						mesh->getNode( n2, q2 ) )
					{
						if( displace ){
							displace->getPhysicalValue(n0,v0);
							displace->getPhysicalValue(n1,v1);
							displace->getPhysicalValue(n2,v2);
							q0.addCoordinate( v0[0], v0[1], v0[2] );
							q1.addCoordinate( v1[0], v1[1], v1[2] );
							q2.addCoordinate( v2[0], v2[1], v2[2] );
						}
						bbox.initialize();
						bbox.update(q0);
						bbox.update(q1);
						bbox.update(q2);
						bbox.expand(1.1);
						int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
						getSubIndices( bbox.getMin().x(), bbox.getMin().y(), bbox.getMin().z(), i0, j0, k0 );
						getSubIndices( bbox.getMax().x(), bbox.getMax().y(), bbox.getMax().z(), i1, j1, k1 );
						for(int i=i0;i<=i1;++i){
							for(int j=j0;j<=j1;++j){
								for(int k=k0;k<=k1;++k){
									getSubRegion(i,j,k,box);
									double area = box.intersectArea(q0,q1,q2);
									if( area != 0.0 ){
										++count;
										insert( i,j,k, std::pair<kmb::Face,double>(f,area) );
									}
									totalArea += area;
								}
							}
						}
						a = kmb::Point3D::area(q0,q1,q2);
						faceArea->setPhysicalValue(f,&a);
						totalFaceArea += a;
					}
					break;
				case kmb::QUAD:
				case kmb::QUAD2:
					n0 = elem.getBoundaryCellId(localId,0);
					n1 = elem.getBoundaryCellId(localId,1);
					n2 = elem.getBoundaryCellId(localId,2);
					n3 = elem.getBoundaryCellId(localId,3);
					if(
						mesh->getNode( n0, q0 ) &&
						mesh->getNode( n1, q1 ) &&
						mesh->getNode( n2, q2 ) &&
						mesh->getNode( n3, q3 ) )
					{
						if( displace ){
							displace->getPhysicalValue(n0,v0);
							displace->getPhysicalValue(n1,v1);
							displace->getPhysicalValue(n2,v2);
							displace->getPhysicalValue(n3,v3);
							q0.addCoordinate( v0[0], v0[1], v0[2] );
							q1.addCoordinate( v1[0], v1[1], v1[2] );
							q2.addCoordinate( v2[0], v2[1], v2[2] );
							q3.addCoordinate( v3[0], v3[1], v3[2] );
						}
						bbox.initialize();
						bbox.update(q0);
						bbox.update(q1);
						bbox.update(q2);
						bbox.update(q3);
						bbox.expand(1.1);
						int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
						getSubIndices( bbox.getMin().x(), bbox.getMin().y(), bbox.getMin().z(), i0, j0, k0 );
						getSubIndices( bbox.getMax().x(), bbox.getMax().y(), bbox.getMax().z(), i1, j1, k1 );
						for(int i=i0;i<=i1;++i){
							for(int j=j0;j<=j1;++j){
								for(int k=k0;k<=k1;++k){
									getSubRegion(i,j,k,box);
									double area = box.intersectArea(q0,q1,q2) + box.intersectArea(q0,q2,q3);
									if( area != 0.0 ){
										++count;
										insert( i,j,k, std::pair<kmb::Face,double>(f,area) );
									}
									totalArea += area;
								}
							}
						}
						a = kmb::Point3D::area(q0,q1,q2) + kmb::Point3D::area(q0,q2,q3);
						faceArea->setPhysicalValue(f,&a);
						totalFaceArea += a;
					}
					break;
				default:
					break;
				}
			}
		}
		++fIter;
	}
	std::cout << "REVOCAP FaceBucketArea count = "<< count << ", total bucket area = " << totalArea << ", total face area = " << totalFaceArea << std::endl;
	return count;
}

// これを使うと三角形に分割して複数登録されてしまうことに注意
// すなわち、Face と Grid の交差が四角形ならば三角形に分割して2つ登録されてしまう。
int
kmb::FaceBucketArea::appendSubBucket(int i0,int i1,int j0,int j1,int k0,int k1,const kmb::Point3D &a,const kmb::Point3D &b,const kmb::Point3D &c,const kmb::Face &f)
{
	if( i0 == i1 && j0 == j1 && k0 == k1 ){
		double area = kmb::Point3D::area(a,b,c);
		std::pair<kmb::Face,double> t(f,area);
		insert( i0,j0,k0, t );
		return 1;
	}
	// 一番大きなところで分割
	int which = -1;
	if( i1-i0 >= j1-j0 ){
		if( i1-i0 >= k1-k0 ){
			// i の幅が最大
			which = 0;
		}else{
			// k の幅が最大
			which = 2;
		}
	}else{
		if( j1-j0 >= k1-k0 ){
			// j の幅が最大
			which = 1;
		}else{
			// k の幅が最大
			which = 2;
		}
	}
	kmb::Point3D p, q;
	switch( which ){
	case 0:
		{
			// i の幅が最大
			int i = (i0 + i1)/2;
			// i と i+1 の境界の x
			double x = getSubRegionMaxX(i,j0,k0);
			switch( kmb::PlaneYZ::getIntersectionTriangle(x,a,b,c,p,q) ){
			case -2:
				return appendSubBucket(i0,i,j0,j1,k0,k1,a,b,c,f);
			case -1:
				return appendSubBucket(i+1,i1,j0,j1,k0,k1,a,b,c,f);
			case 0:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,c,q,f) +
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,q,p,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,p,q,f);
			case 1:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,a,q,f) +
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,q,p,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,p,q,f);
			case 2:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,b,q,f) +
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,q,p,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,p,q,f);
			case 3:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,p,q,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,c,q,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,q,p,f);
			case 4:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,p,q,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,a,q,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,q,p,f);
			case 5:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,p,q,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,b,q,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,q,p,f);
			case 6:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,p,c,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,b,p,f);
			case 7:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,p,a,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,c,p,f);
			case 8:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,p,b,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,a,p,f);
			case 9:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,b,p,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,p,c,f);
			case 10:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,c,p,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,p,a,f);
			case 11:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,a,p,f) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,p,b,f);
			default:
				return 0;
			}
		}
	case 1:
		{
			// j の幅が最大
			int j = (j0 + j1)/2;
			// j と j+1 の境界の y
			double y = getSubRegionMaxY(i0,j,k0);
			switch( kmb::PlaneZX::getIntersectionTriangle(y,a,b,c,p,q) ){
			case -2:
				return appendSubBucket(i0,i1,j0,j,k0,k1,a,b,c,f);
			case -1:
				return appendSubBucket(i0,i1,j+1,j1,k0,k1,a,b,c,f);
			case 0:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,c,q,f) +
					appendSubBucket(i0,i1,j0, j, k0,k1,b,q,p,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,p,q,f);
			case 1:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,a,q,f) +
					appendSubBucket(i0,i1,j0, j, k0,k1,c,q,p,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,p,q,f);
			case 2:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,b,q,f) +
					appendSubBucket(i0,i1,j0, j, k0,k1,a,q,p,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,p,q,f);
			case 3:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,p,q,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,c,q,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,q,p,f);
			case 4:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,p,q,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,a,q,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,q,p,f);
			case 5:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,p,q,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,b,q,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,q,p,f);
			case 6:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,p,c,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,b,p,f);
			case 7:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,p,a,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,c,p,f);
			case 8:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,p,b,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,a,p,f);
			case 9:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,b,p,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,p,c,f);
			case 10:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,c,p,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,p,a,f);
			case 11:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,a,p,f) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,p,b,f);
			default:
				return 0;
			}
		}
	case 2:
		{
			// k の幅が最大
			int k = (k0 + k1)/2;
			// k と k+1 の境界の z
			double z = getSubRegionMaxZ(i0,j0,k);
			switch( kmb::PlaneXY::getIntersectionTriangle(z,a,b,c,p,q) ){
			case -2:
				return appendSubBucket(i0,i1,j0,j1,k0, k, a,b,c,f);
			case -1:
				return appendSubBucket(i0,i1,j0,j1,k+1,k1,a,b,c,f);
			case 0:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,c,q,f) +
					appendSubBucket(i0,i1,j0,j1,k0, k, b,q,p,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,p,q,f);
			case 1:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,a,q,f) +
					appendSubBucket(i0,i1,j0,j1,k0, k, c,q,p,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,p,q,f);
			case 2:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,b,q,f) +
					appendSubBucket(i0,i1,j0,j1,k0, k, a,q,p,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,p,q,f);
			case 3:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,p,q,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,c,q,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,q,p,f);
			case 4:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,p,q,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,a,q,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,q,p,f);
			case 5:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,p,q,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,b,q,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,q,p,f);
			case 6:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,p,c,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,b,p,f);
			case 7:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,p,a,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,c,p,f);
			case 8:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,p,b,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,a,p,f);
			case 9:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,b,p,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,p,c,f);
			case 10:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,c,p,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,p,a,f);
			case 11:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,a,p,f) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,p,b,f);
			default:
				return 0;
			}
		}
	default:
		return 0;
	}
}

double kmb::FaceBucketArea::getArea(kmb::Face f) const
{
	double area=0.0;
	if( faceArea ){
		faceArea->getPhysicalValue(f,&area);
	}
	return area;
}

bool
kmb::FaceBucketArea::getNearestInBucket(const kmb::Point3D& pt,int i,int j,int k,double &dist,kmb::Face &f) const
{
	if( mesh == NULL || this->getCount(i,j,k) == 0 ){
		return false;
	}
	kmb::Minimizer minimizer;
	kmb::Node n0, n1, n2, n3;
	kmb::Bucket< std::pair<kmb::Face,double> >::const_iterator fIter = this->begin(i,j,k);
	kmb::Bucket< std::pair<kmb::Face,double> >::const_iterator endIter = this->end(i,j,k);
	while( fIter != endIter ){
		const kmb::Face f0 = fIter.get().first;
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
kmb::FaceBucketArea::getNearest(double x,double y,double z,double &dist,kmb::Face &f) const
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
		getSubIndices( x-dist, y-dist, z-dist, i1, j1, k1 );
		getSubIndices( x+dist, y+dist, z+dist, i2, j2, k2 );
		// (i0,j0,k0) 以外を探す
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
		// (i0,j0,k0) の Bucket に Face がない
		// 全部調べる
		kmb::BoxRegion box;
		kmb::Bucket< std::pair<kmb::Face,double> >::const_iterator fIter = this->begin();
		kmb::Bucket< std::pair<kmb::Face,double> >::const_iterator endIter = this->end();
		while( fIter != endIter ){
			if( getSubRegion( fIter.getIndex(), box ) && box.distanceSq( pt ) > minimizer.getMin() ){
				++fIter;
				continue;
			}
			f0 = fIter.get().first;
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
