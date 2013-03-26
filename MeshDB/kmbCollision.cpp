/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Collision                                               #
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
#include <cmath>
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbGeometry4D.h"
#include "Common/kmbCalculator.h"
#include "MeshDB/kmbCollision.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementOctree.h"
#include "MeshDB/kmbSurfaceMatching.h"

kmb::Collision::Collision(kmb::MeshData* mesh)
: points(NULL)
, mesh(NULL)
, accuracy(1.0e-8)
{
	if( mesh ){
		this->points = mesh->getNodes();
		this->mesh = mesh;
	}
}

kmb::Collision::~Collision(void)
{
}

const char*
kmb::Collision::collisionTypeString( collisionType ctype )
{
	switch( ctype ){
	case kmb::Collision::COLLISION_ERROR: return "COLLISION_ERROR";
	case kmb::Collision::NOT_COLLIDE: return "NOT_COLLIDE";
	case kmb::Collision::TOPOLOGICAL_CONTACT: return "TOPOLOGICAL_CONTACT";
	case kmb::Collision::GEOMETRICAL_CONTACT: return "GEOMETRICAL_CONTACT";
	case kmb::Collision::DETECT: return "DETECT";
	default: return "UNKNOWN";
	};
}

void
kmb::Collision::setAccuracy(double acc)
{
	this->accuracy = acc;
}

double
kmb::Collision::getAccuracy(void) const
{
	return this->accuracy;
}














































double
kmb::Collision::distanceSqPtTri(kmb::nodeIdType nodeId,kmb::ElementBase& tri,double s[2]) const
{
	kmb::Point3D p0,q0,q1,q2;
	if( points &&
		points->getPoint( nodeId, p0 ) &&
		points->getPoint( tri.getCellId(0), q0 ) &&
		points->getPoint( tri.getCellId(1), q1 ) &&
		points->getPoint( tri.getCellId(2), q2 ) )
	{
		return p0.distanceSqToTriangle(q0,q1,q2,s);
	}
	return DBL_MAX;
}

double
kmb::Collision::distanceSqSegSeg(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,double &t1, double &t2) const
{


	kmb::Vector3D u0(p0,q0);
	kmb::Vector3D u1(p1,p0);
	kmb::Vector3D u2(q0,q1);
	kmb::Matrix2x2 mat(
		u1*u1, u1*u2,
		u2*u1, u2*u2);
	kmb::Vector2D v(-(u1*u0),-(u2*u0));
	kmb::Vector2D* t = mat.solve(v);
	if( t &&
		0.0 < t->x() && t->x() < 1.0 &&
		0.0 < t->y() && t->y() < 1.0 )
	{

		t1 = t->x();
		t2 = t->y();
		delete t;
		return (u0 + u1.scalar(t1) + u2.scalar(t2)).lengthSq();
	}else{

		kmb::Minimizer min;
		double tmp=0.0;
		if( min.update( p0.distanceSqToSegment( q0, q1, tmp ) ) ){
			t1 = 0.0;
			t2 = tmp;
		}
		if( min.update( p1.distanceSqToSegment( q0, q1, tmp ) ) ){
			t1 = 1.0;
			t2 = tmp;
		}
		if( min.update( q0.distanceSqToSegment( p0, p1, tmp ) ) ){
			t1 = tmp;
			t2 = 0.0;
		}
		if( min.update( q1.distanceSqToSegment( p0, p1, tmp ) ) ){
			t1 = tmp;
			t2 = 1.0;
		}
		if( t ){
			delete t;
		}
		return min.getMin();
	}
}

double
kmb::Collision::distanceSqSegSeg(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::nodeIdType b0,kmb::nodeIdType b1,double &t1, double &t2) const
{
	kmb::Point3D p0,p1,q0,q1;
	if( points &&
		points->getPoint( a0, p0 ) &&
		points->getPoint( a1, p1 ) &&
		points->getPoint( b0, q0 ) &&
		points->getPoint( b1, q1 ) )
	{
		return distanceSqSegSeg(p0,p1,q0,q1,t1,t2);
	}
	return DBL_MAX;
}

double
kmb::Collision::distanceSqSegSeg(kmb::ElementBase& seg0,kmb::ElementBase& seg1,double &t1, double &t2) const
{
	kmb::Point3D p0,p1,q0,q1;
	if( points &&
		points->getPoint( seg0[0], p0 ) &&
		points->getPoint( seg0[1], p1 ) &&
		points->getPoint( seg1[0], q0 ) &&
		points->getPoint( seg1[1], q1 ) )
	{
		return distanceSqSegSeg(p0,p1,q0,q1,t1,t2);
	}
	return DBL_MAX;
}

double
kmb::Collision::distanceSegSeg(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::nodeIdType b0,kmb::nodeIdType b1) const
{
	double s=0.0, t=0.0;
	return sqrt( distanceSqSegSeg(a0,a1,b0,b1,s,t) );
}

double
kmb::Collision::distanceSegSeg(kmb::ElementBase& seg0,kmb::ElementBase& seg1) const
{
	double s=0.0, t=0.0;
	return sqrt( distanceSqSegSeg(seg0,seg1,s,t) );
}

double
kmb::Collision::distanceSqSegTri(kmb::Point3D& p0,kmb::Point3D& p1,
	kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,
	double &s,double t[2]) const
{



	kmb::Vector3D u0(p0,q0);
	kmb::Vector3D u1(p1,p0);
	kmb::Vector3D u2(q0,q1);
	kmb::Vector3D u3(q0,q2);
	kmb::Matrix3x3 mat(
		u1*u1, u1*u2, u1*u3,
		u2*u1, u2*u2, u2*u3,
		u3*u1, u3*u2, u3*u3);
	kmb::Vector3D v(-(u1*u0),-(u2*u0),-(u3*u0));
	kmb::Vector3D* tmp = mat.solve(v);
	if( tmp &&
		0.0 < tmp->x() && tmp->x() < 1.0 &&
		0.0 < tmp->y() && 0.0 < tmp->z() &&
		tmp->y() + tmp->z() < 1.0 )
	{

		s = tmp->getCoordinate(0);
		t[0] = tmp->getCoordinate(1);
		t[1] = tmp->getCoordinate(2);
		delete tmp;
		return (u0 + u1.scalar(s) + u2.scalar(t[0]) + u3.scalar(t[1])).lengthSq();
	}else{

		kmb::Minimizer min;

		double tt[2];
		if( min.update( p0.distanceSqToTriangle( q0, q1, q2, tt ) ) ){
			s = 0.0;
			t[0] = tt[0];
			t[1] = tt[1];
		}
		if( min.update( p1.distanceSqToTriangle( q0, q1, q2, tt ) ) ){
			s = 1.0;
			t[0] = tt[0];
			t[1] = tt[1];
		}

		double ss0,ss1;
		if( min.update( distanceSqSegSeg( p0, p1, q0, q1, ss0, ss1 ) ) ){
			s = ss0;
			t[0] = ss1;
			t[1] = 0.0;
		}
		if( min.update( distanceSqSegSeg( p0, p1, q1, q2, ss0, ss1 ) ) ){
			s = ss0;
			t[0] = 1.0 - ss1;
			t[1] = ss1;
		}
		if( min.update( distanceSqSegSeg( p0, p1, q0, q2, ss0, ss1 ) ) ){
			s = ss0;
			t[0] = 0.0;
			t[1] = ss1;
		}
		if( tmp ){
			delete tmp;
		}
		return min.getMin();
	}
}

double
kmb::Collision::distanceSqSegTri(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::ElementBase& tri,double &s,double t[2]) const
{
	kmb::Point3D p0,p1,q0,q1,q2;
	if( points &&
		points->getPoint( a0, p0 ) &&
		points->getPoint( a1, p1 ) &&
		points->getPoint( tri.getCellId(0), q0 ) &&
		points->getPoint( tri.getCellId(1), q1 ) &&
		points->getPoint( tri.getCellId(2), q2 ) )
	{
		return distanceSqSegTri( p0, p1, q0, q1, q2, s, t );
	}
	return DBL_MAX;
}

double
kmb::Collision::distanceSegTri(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::ElementBase& tri) const
{
	double t = 0.0;
	double s[2] = {0.0,0.0};
	return sqrt( distanceSqSegTri( a0, a1, tri, t, s ) );
}

double
kmb::Collision::distanceSqTriTri(
	kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,
	kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,
	double s[2],double t[2]) const
{


































		kmb::Minimizer min;

		double ss = 0.0;
		double tt[2] = {0.0, 0.0};
		if( min.update( distanceSqSegTri( p0, p1, q0, q1, q2, ss, tt ) ) ){
			s[0] = ss;
			s[1] = 0.0;
			t[0] = tt[0];
			t[1] = tt[1];
		}
		if( min.update( distanceSqSegTri( p1, p2, q0, q1, q2, ss, tt ) ) ){
			s[0] = 1.0-ss;
			s[1] = ss;
			t[0] = tt[0];
			t[1] = tt[1];
		}
		if( min.update( distanceSqSegTri( p0, p2, q0, q1, q2, ss, tt ) ) ){
			s[0] = 0.0;
			s[1] = ss;
			t[0] = tt[0];
			t[1] = tt[1];
		}
		if( min.update( distanceSqSegTri( q0, q1, p0, p1, p2, ss, tt ) ) ){
			s[0] = tt[0];
			s[1] = tt[1];
			t[0] = ss;
			t[1] = 0.0;
		}
		if( min.update( distanceSqSegTri( q1, q2, p0, p1, p2, ss, tt ) ) ){
			s[0] = tt[0];
			s[1] = tt[1];
			t[0] = 1.0-ss;
			t[1] = ss;
		}
		if( min.update( distanceSqSegTri( q0, q2, p0, p1, p2, ss, tt ) ) ){
			s[0] = tt[0];
			s[1] = tt[1];
			t[0] = 0.0;
			t[1] = ss;
		}



		return min.getMin();

}

double
kmb::Collision::distanceSqTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1,double s[2],double t[2]) const
{
	kmb::Point3D p0,p1,p2,q0,q1,q2;
	if( points &&
		points->getPoint( tri0.getCellId(0), p0 ) &&
		points->getPoint( tri0.getCellId(1), p1 ) &&
		points->getPoint( tri0.getCellId(2), p2 ) &&
		points->getPoint( tri1.getCellId(0), q0 ) &&
		points->getPoint( tri1.getCellId(1), q1 ) &&
		points->getPoint( tri1.getCellId(2), q2 ) )
	{
		return distanceSqTriTri( p0, p1, p2, q0, q1, q2, s, t );
	}
	return DBL_MAX;
}


double
kmb::Collision::distanceTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1) const
{
	double t[2] = {0.0,0.0};
	double s[2] = {0.0,0.0};
	return sqrt( distanceSqTriTri( tri0, tri1, t, s ) );
}

double
kmb::Collision::distanceSqPtQuad(kmb::Point3D& pt,kmb::ElementBase& quad) const
{
	kmb::Point3D t0,t1,t2,t3;
	if( points &&
		points->getPoint( quad.getCellId(0), t0 ) &&
		points->getPoint( quad.getCellId(1), t1 ) &&
		points->getPoint( quad.getCellId(2), t2 ) &&
		points->getPoint( quad.getCellId(3), t3 ) )
	{
		return kmb::Minimizer::getMin(
			pt.distanceSqToTriangle( t0, t1, t2 ),
			pt.distanceSqToTriangle( t2, t3, t0 ) );
	}
	return DBL_MAX;
}

double
kmb::Collision::distanceSqQuadQuad(kmb::ElementBase& q0,kmb::ElementBase& q1) const
{
	kmb::Minimizer min;
	kmb::nodeIdType triangles0[2][3];
	kmb::nodeIdType triangles1[2][3];
	q0.divideIntoTriangles( triangles0 );
	q1.divideIntoTriangles( triangles1 );
	kmb::Triangle tri00( triangles0[0][0], triangles0[0][1], triangles0[0][2] );
	kmb::Triangle tri01( triangles0[1][0], triangles0[1][1], triangles0[1][2] );
	kmb::Triangle tri10( triangles1[0][0], triangles1[0][1], triangles1[0][2] );
	kmb::Triangle tri11( triangles1[1][0], triangles1[1][1], triangles1[1][2] );
	double s0[2]={0.0,0.0},s1[2]={0.0,0.0};
	min.update( distanceSqTriTri(tri00,tri10,s0,s1) );
	min.update( distanceSqTriTri(tri00,tri11,s0,s1) );
	min.update( distanceSqTriTri(tri01,tri10,s0,s1) );
	min.update( distanceSqTriTri(tri01,tri11,s0,s1) );
	return min.getMin();
}

double
kmb::Collision::distanceQuadQuad(kmb::ElementBase& q0,kmb::ElementBase& q1) const
{
	return sqrt( distanceSqQuadQuad(q0,q1) );
}

double
kmb::Collision::distanceSqTriQuad(kmb::ElementBase& tri,kmb::ElementBase& q) const
{
	kmb::Minimizer min;
	kmb::nodeIdType triangles[2][3];
	q.divideIntoTriangles( triangles );
	kmb::Triangle tri0( triangles[0][0], triangles[0][1], triangles[0][2] );
	kmb::Triangle tri1( triangles[1][0], triangles[1][1], triangles[1][2] );
	double s0[2]={0.0,0.0},s1[2]={0.0,0.0};
	min.update( distanceSqTriTri(tri,tri0,s0,s1) );
	min.update( distanceSqTriTri(tri,tri1,s0,s1) );
	return min.getMin();
}

double
kmb::Collision::distanceTriQuad(kmb::ElementBase& tri,kmb::ElementBase& q) const
{
	return sqrt( distanceSqTriQuad(tri,q) );
}

double
kmb::Collision::distanceSq(kmb::ElementBase& elem0,kmb::ElementBase& elem1) const
{
	double s0[2]={0.0,0.0},s1[2]={0.0,0.0};
	switch( elem0.getLinearType() ){
	case kmb::TRIANGLE:
		switch( elem1.getLinearType() ){
		case kmb::TRIANGLE:
			return distanceSqTriTri(elem0,elem1,s0,s1);
		case kmb::QUAD:
			return distanceSqTriQuad(elem0,elem1);
		default:
			break;
		}
	case kmb::QUAD:
		switch( elem1.getLinearType() ){
		case kmb::TRIANGLE:
			return distanceSqTriQuad(elem1,elem0);
		case kmb::QUAD:
			return distanceSqQuadQuad(elem0,elem1);
		default:
			break;
		}
	default:
		break;
	}
	return DBL_MAX;
}

double
kmb::Collision::distance(kmb::ElementBase& elem0,kmb::ElementBase& elem1) const
{
	return sqrt( distanceSq(elem0,elem1) );
}

double
kmb::Collision::distanceSq(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements) const
{
	if( elements ){
		double s0[2]={0.0,0.0},s1[2]={0.0,0.0};
		switch( elem0.getLinearType() ){
		case kmb::TRIANGLE:
			switch( f.getFaceLinearElementType(elements) ){
			case kmb::TRIANGLE:
			{
				kmb::Triangle tri;
				f.getFaceLinearElement( elements, tri );
				return distanceSqTriTri(elem0,tri,s0,s1);
			}
			case kmb::QUAD:
			{
				kmb::Quad q;
				f.getFaceLinearElement( elements, q );
				return distanceSqTriQuad(elem0,q);
			}
			default:
				break;
			}
		case kmb::QUAD:
			switch( f.getFaceLinearElementType(elements) ){
			case kmb::TRIANGLE:
			{
				kmb::Triangle tri;
				f.getFaceLinearElement( elements, tri );
				return distanceSqTriQuad(tri,elem0);
			}
			case kmb::QUAD:
			{
				kmb::Quad q;
				f.getFaceLinearElement( elements, q );
				return distanceSqQuadQuad(elem0,q);
			}
			default:
				break;
			}
		default:
			break;
		}
	}
	return DBL_MAX;
}

double
kmb::Collision::distance(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements) const
{
	return sqrt( distanceSq(elem0,f,elements) );
}





int
kmb::Collision::detectFaceGroup(const char* fg0,const char* fg1,double thres,const char* result) const
{
	if( mesh == NULL || points == NULL ){
		return 0;
	}
	kmb::DataBindings* faceGroup0 = mesh->getDataBindingsPtr( fg0 );
	kmb::DataBindings* faceGroup1 = mesh->getDataBindingsPtr( fg1 );
	kmb::DataBindings* resultGroup = mesh->getDataBindingsPtr( result );
	if( faceGroup0 && ( faceGroup0->getBindingMode() == kmb::DataBindings::FaceGroup ||
		  faceGroup0->getBindingMode() == kmb::DataBindings::FaceVariable ) &&
	    faceGroup1 && ( faceGroup1->getBindingMode() == kmb::DataBindings::FaceGroup ||
		  faceGroup1->getBindingMode() == kmb::DataBindings::FaceVariable ) &&
		resultGroup && ( resultGroup->getBindingMode() == kmb::DataBindings::FaceGroup ) )
	{
		size_t orgSize = resultGroup->getIdCount();
		kmb::Face f0,f1;
		kmb::Triangle tri0, tri1;
		kmb::Quad q0, q1;
		kmb::DataBindings::const_iterator f1Iter = faceGroup1->begin();
		while( !f1Iter.isFinished() ){
			if( f1Iter.getFace(f1) ){
				switch( f1.getFaceLinearElementType(mesh) ){
					case kmb::TRIANGLE:
					{
						f1.getFaceLinearElement(mesh,tri1);
						kmb::DataBindings::const_iterator f0Iter = faceGroup0->begin();
						while( !f0Iter.isFinished() ){
							if( f0Iter.getFace(f0) ){
								switch( f0.getFaceLinearElementType(mesh) ){
									case kmb::TRIANGLE:
										f0.getFaceLinearElement(mesh,tri0);
										if( distanceTriTri(tri0,tri1) < thres ){
											resultGroup->addId( f1 );
											goto foundFace;
										}
										break;
									case kmb::QUAD:
										f0.getFaceLinearElement(mesh,q0);
										if( distanceTriQuad(tri1,q0) < thres ){
											resultGroup->addId( f1 );
											goto foundFace;
										}
										break;
									default:
										break;
								}
							}
							++f0Iter;
						}
						break;
					}
					case kmb::QUAD:
					{
						f1.getFaceLinearElement(mesh,q1);
						kmb::DataBindings::const_iterator f0Iter = faceGroup0->begin();
						while( !f0Iter.isFinished() ){
							if( f0Iter.getFace(f0) ){
								switch( f0.getFaceLinearElementType(mesh) ){
									case kmb::TRIANGLE:
										f0.getFaceLinearElement(mesh,tri0);
										if( distanceTriQuad(tri0,q1) < thres ){
											resultGroup->addId( f1 );
											goto foundFace;
										}
										break;
									case kmb::QUAD:
										f0.getFaceLinearElement(mesh,q0);
										if( distanceQuadQuad(q0,q1) < thres ){
											resultGroup->addId( f1 );
											goto foundFace;
										}
										break;
									default:
										break;
								}
							}
							++f0Iter;
						}
						break;
					}
					default:
						break;
				}
			}
foundFace:
			++f1Iter;
		}
		return static_cast<int>( resultGroup->getIdCount() - orgSize );
	}
	return 0;
}





int
kmb::Collision::detectSurfaceFaceGroup(kmb::bodyIdType bodyId0,const char* fg1,double thres,const char* result) const
{
	if( mesh == NULL || points == NULL ){
		return 0;
	}

	kmb::ElementContainer* surf0 = mesh->getBodyPtr( bodyId0 );
	kmb::DataBindings* faceGroup1 = mesh->getDataBindingsPtr( fg1 );
	kmb::DataBindings* resultGroup = mesh->getDataBindingsPtr( result );
	if( surf0 && ( surf0->getDimension() == 2 ) &&
	    faceGroup1 && ( faceGroup1->getBindingMode() == kmb::DataBindings::FaceGroup ||
		  faceGroup1->getBindingMode() == kmb::DataBindings::FaceVariable ) &&
		resultGroup &&( resultGroup->getBindingMode() == kmb::DataBindings::FaceGroup ) )
	{
		size_t orgSize = resultGroup->getIdCount();
		kmb::Face f1;
		kmb::Triangle tri1;
		kmb::Quad q1;
		double dist = 0.0;
		kmb::DataBindings::const_iterator f1Iter = faceGroup1->begin();
		while( !f1Iter.isFinished() ){
			if( f1Iter.getFace(f1) ){
				switch( f1.getFaceLinearElementType(mesh) ){
					case kmb::TRIANGLE:
					{
						f1.getFaceElement(mesh,tri1);
						kmb::ElementContainer::const_iterator eIter = surf0->begin();
						while( !eIter.isFinished() ){
							switch( eIter.getLinearType() ){
								case kmb::TRIANGLE:
									dist = distanceTriTri(eIter,tri1);
									if( dist < thres ){
										resultGroup->addId( f1 );
										goto foundFace;
									}
									break;
								case kmb::QUAD:
									dist = distanceTriQuad(tri1,eIter);
									if( dist < thres ){
										resultGroup->addId( f1 );
										goto foundFace;
									}
									break;
								default:
									break;
							}
							++eIter;
						}
						break;
					}
					case kmb::QUAD:
					{
						f1.getFaceLinearElement(mesh,q1);
						kmb::ElementContainer::const_iterator eIter = surf0->begin();
						while( !eIter.isFinished() ){
							switch( eIter.getLinearType() ){
								case kmb::TRIANGLE:
									dist = distanceTriQuad(eIter,q1);
									if( dist < thres ){
										resultGroup->addId( f1 );
										goto foundFace;
									}
									break;
								case kmb::QUAD:
									dist = distanceQuadQuad(eIter,q1);
									if( dist < thres ){
										resultGroup->addId( f1 );
										goto foundFace;
									}
									break;
								default:
									break;
							}
							++eIter;
						}
						break;
					}
					default:
						break;
				}
			}
foundFace:
			++f1Iter;
		}
		return static_cast<int>( resultGroup->getIdCount() - orgSize );
	}
	return 0;
}

int
kmb::Collision::detectSurfaceFaceGroupByNode(kmb::bodyIdType bodyId0,const char* fg1,int tCount,double thres,const char* result) const
{
	if( mesh == NULL || points == NULL ){
		return 0;
	}
	kmb::ElementContainer* surf0 = mesh->getBodyPtr( bodyId0 );
	kmb::DataBindings* faceGroup1 = mesh->getDataBindingsPtr( fg1 );
	kmb::DataBindings* resultGroup = mesh->getDataBindingsPtr( result );
	if( surf0 && ( surf0->getDimension() == 2 ) &&
	    faceGroup1 && ( faceGroup1->getBindingMode() == kmb::DataBindings::FaceGroup ||
		  faceGroup1->getBindingMode() == kmb::DataBindings::FaceVariable ) &&
		resultGroup &&( resultGroup->getBindingMode() == kmb::DataBindings::FaceGroup ) )
	{
		kmb::Element2DOctree octree;
		const kmb::ElementContainer* body = mesh->getBodyPtr(bodyId0);
		octree.setContainer( mesh->getNodes(), body );




		size_t orgSize = resultGroup->getIdCount();
		kmb::Face f1;
		kmb::Triangle tri1;
		kmb::Quad q1;
		kmb::Node node;



		double dist = 0.0;
		kmb::DataBindings::const_iterator f1Iter = faceGroup1->begin();
		while( !f1Iter.isFinished() ){
			if( f1Iter.getFace(f1) ){
				kmb::ElementContainer::const_iterator eIter = mesh->findElement( f1.getElementId() );
				int count = 0;
				int len = eIter.getBoundaryNodeCount( f1.getLocalFaceId() );
				for(int i=0;i<len;++i){
					kmb::nodeIdType nodeId = eIter.getBoundaryCellId( f1.getLocalFaceId(), i );
					mesh->getNode( nodeId, node );

					kmb::elementIdType nearest = kmb::Element::nullElementId;
					dist = octree.getNearest( node.x(), node.y(), node.z(), nearest );
					if( nearest != kmb::Element::nullElementId && dist < thres ){
						++count;
					}
				}
				if( count >= tCount ){
					resultGroup->addId( f1 );
				}
			}
			++f1Iter;
		}
		return static_cast<int>( resultGroup->getIdCount() - orgSize );
	}
	return 0;
}

double
kmb::Collision::getNearestFace(kmb::ElementBase& q0,const char* fg,kmb::Face &nearestFace,int &index) const
{
	kmb::Minimizer minimizer;
	if( mesh == NULL || points == NULL ){
		return minimizer.getMin();
	}
	kmb::DataBindings* faceGroup = mesh->getDataBindingsPtr( fg );
	if( faceGroup && ( faceGroup->getBindingMode() == kmb::DataBindings::FaceGroup ||
		  faceGroup->getBindingMode() == kmb::DataBindings::FaceVariable ) )
	{
		kmb::Face f;
		kmb::Quad q;
		kmb::Triangle t;
		double d = 0.0;
		int i = -1;
		kmb::DataBindings::const_iterator fIter = faceGroup->begin();
		if( q0.getLinearType() == kmb::TRIANGLE ){
			while( !fIter.isFinished() ){
				if( fIter.getFace(f) ){
					switch( f.getFaceLinearElementType(mesh) ){
					case kmb::TRIANGLE:
						f.getFaceLinearElement(mesh,t);
						d = distanceByNodeTri(q0,t,i);
						if( d > 0 && minimizer.update(d) ){
							index = i;
							nearestFace = f;
						}
						break;
					default:
						break;
					}
				}
				++fIter;
			}
		}else if( q0.getLinearType() == kmb::QUAD ){
			while( !fIter.isFinished() ){
				if( fIter.getFace(f) ){
					switch( f.getFaceLinearElementType(mesh) ){
					case kmb::QUAD:
						f.getFaceLinearElement(mesh,q);
						d = distanceByNodeQuad(q0,q,i);
						if( d > 0 && minimizer.update(d) ){
							index = i;
							nearestFace = f;
						}
						break;
					default:
						break;
					}
				}
				++fIter;
			}
		}
	}
	return minimizer.getMin();
}

double
kmb::Collision::distanceByNodeTri(kmb::ElementBase& t0,kmb::ElementBase& t1,int &index) const
{
	kmb::Minimizer minimizer;
	if( mesh == NULL || points == NULL )
	{
		return minimizer.getMin();
	}
	index = -1;
	for(int i=0;i<3;++i){
		double d =
			sqrt( points->distanceSq( t0.getCellId((0+i)%3), t1.getCellId(0) ) ) +
			sqrt( points->distanceSq( t0.getCellId((1+i)%3), t1.getCellId(1) ) ) +
			sqrt( points->distanceSq( t0.getCellId((2+i)%3), t1.getCellId(2) ) );
		if( minimizer.update( d ) ){
			index = i;
		}
	}
	return minimizer.getMin();
}

double
kmb::Collision::distanceByNodeTri(kmb::ElementBase& t0,kmb::Face &f,const kmb::ElementContainer* elements,int &index) const
{
	kmb::Minimizer minimizer;
	if( mesh == NULL || points == NULL || elements == NULL )
	{
		return minimizer.getMin();
	}
	if( f.getFaceLinearElementType(elements) == kmb::TRIANGLE )
	{
		kmb::Triangle t1;
		f.getFaceLinearElement( elements, t1 );
		index = -1;
		for(int i=0;i<3;++i){
			double d =
				sqrt( points->distanceSq( t0.getCellId((0+i)%3), t1.getCellId(0) ) ) +
				sqrt( points->distanceSq( t0.getCellId((1+i)%3), t1.getCellId(1) ) ) +
				sqrt( points->distanceSq( t0.getCellId((2+i)%3), t1.getCellId(2) ) );
			if( minimizer.update( d ) ){
				index = i;
			}
		}
	}
	return minimizer.getMin();
}

double
kmb::Collision::distanceByNodeQuad(kmb::ElementBase& q0,kmb::ElementBase& q1,int &index) const
{
	kmb::Minimizer minimizer;
	if( mesh == NULL || points == NULL )
	{
		return minimizer.getMin();
	}
	index = -1;
	for(int i=0;i<4;++i){
		double d =
			sqrt( points->distanceSq( q0.getCellId((0+i)%4), q1.getCellId(0) ) ) +
			sqrt( points->distanceSq( q0.getCellId((1+i)%4), q1.getCellId(1) ) ) +
			sqrt( points->distanceSq( q0.getCellId((2+i)%4), q1.getCellId(2) ) ) +
			sqrt( points->distanceSq( q0.getCellId((3+i)%4), q1.getCellId(3) ) );
		if( minimizer.update( d ) ){
			index = i;
		}
	}
	return minimizer.getMin();
}

double
kmb::Collision::distanceByNodeQuad(kmb::ElementBase& q0,kmb::Face &f,const kmb::ElementContainer* elements,int &index) const
{
	kmb::Minimizer minimizer;
	if( mesh == NULL || points == NULL || elements == NULL )
	{
		return minimizer.getMin();
	}
	if( f.getFaceLinearElementType(elements) == kmb::QUAD )
	{
		kmb::Quad q1;
		f.getFaceLinearElement( elements, q1 );
		index = -1;
		for(int i=0;i<4;++i){
			double d =
				sqrt( points->distanceSq( q0.getCellId((0+i)%4), q1.getCellId(0) ) ) +
				sqrt( points->distanceSq( q0.getCellId((1+i)%4), q1.getCellId(1) ) ) +
				sqrt( points->distanceSq( q0.getCellId((2+i)%4), q1.getCellId(2) ) ) +
				sqrt( points->distanceSq( q0.getCellId((3+i)%4), q1.getCellId(3) ) );
			if( minimizer.update( d ) ){
				index = i;
			}
		}
	}
	return minimizer.getMin();
}

double
kmb::Collision::distanceByNode(kmb::ElementBase& elem0,kmb::ElementBase& elem1,int &index) const
{
	switch( elem0.getLinearType() ){
	case kmb::TRIANGLE:
		switch( elem1.getLinearType() ){
		case kmb::TRIANGLE:
			return distanceByNodeTri(elem0,elem1,index);
		default:
			break;
		}
	case kmb::QUAD:
		switch( elem1.getLinearType() ){
		case kmb::QUAD:
			return distanceByNodeQuad(elem0,elem1,index);
		default:
			break;
		}
	default:
		break;
	}
	return DBL_MAX;
}

double
kmb::Collision::distanceByNode(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements,int &index) const
{
	if( elements ){
		switch( elem0.getLinearType() ){
		case kmb::TRIANGLE:
			switch( f.getFaceLinearElementType(elements) ){
			case kmb::TRIANGLE:
			{
				kmb::Triangle tri;
				f.getFaceLinearElement( elements, tri );
				return distanceByNodeTri(elem0,tri,index);
			}
			default:
				break;
			}
		case kmb::QUAD:
			switch( f.getFaceLinearElementType(elements) ){
			case kmb::QUAD:
			{
				kmb::Quad q;
				f.getFaceLinearElement( elements, q );
				return distanceByNodeQuad(elem0,q,index);
			}
			default:
				break;
			}
		default:
			break;
		}
	}
	return DBL_MAX;
}
