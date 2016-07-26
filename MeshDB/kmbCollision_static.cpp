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
#include "MeshDB/kmbCollision.h"
#include "Common/kmbCalculator.h"
#include "Common/kmbTolerance.h"

double kmb::Collision::testSegSeg(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,double &t1, double &t2)
{
	// p0 + t1(p1-p0) と q0 + t2(q1-q0) の距離の最小化
	// u0 + t1*u1 + t2*u2 の長さの最小化
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
		// 内部で最小を取る
		t1 = t->x();
		t2 = t->y();
		delete t;
		return (u0 + u1.scalar(t1) + u2.scalar(t2)).lengthSq();
	}else{
		// 境界で最小を取る
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

double kmb::Collision::testSegTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,double &s,double t[2])
{
	// t1 = s t2 = t[0] t3 = t[1]
	// p0 + t1(p1-p0) と q0 + t2(q1-q0) + t3(q2-q0) と の距離の最小化
	// u0 + t1*u1 + t2*u2 + t3*u3 の長さの最小化
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
		// 内部で最小を取る
		s = tmp->getCoordinate(0);
		t[0] = tmp->getCoordinate(1);
		t[1] = tmp->getCoordinate(2);
		delete tmp;
		return (u0 + u1.scalar(s) + u2.scalar(t[0]) + u3.scalar(t[1])).lengthSq();
	}else{
		// 境界で最小を取る
		kmb::Minimizer min;
		// 三角形と点
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
		// 辺と辺
		double ss0,ss1;
		if( min.update( testSegSeg( p0, p1, q0, q1, ss0, ss1 ) ) ){
			s = ss0;
			t[0] = ss1;
			t[1] = 0.0;
		}
		if( min.update( testSegSeg( p0, p1, q1, q2, ss0, ss1 ) ) ){
			s = ss0;
			t[0] = 1.0 - ss1;
			t[1] = ss1;
		}
		if( min.update( testSegSeg( p0, p1, q0, q2, ss0, ss1 ) ) ){
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
kmb::Collision::testTriTri(
	kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,
	kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,
	double s[2],double t[2])
{
	// 境界で最小を取る
	kmb::Minimizer min;
	// 三角形と辺
	double ss = 0.0;
	double tt[2] = {0.0, 0.0};
	if( min.update( kmb::Collision::testSegTri( p0, p1, q0, q1, q2, ss, tt ) ) ){
		s[0] = ss;
		s[1] = 0.0;
		t[0] = tt[0];
		t[1] = tt[1];
	}
	if( min.update( kmb::Collision::testSegTri( p1, p2, q0, q1, q2, ss, tt ) ) ){
		s[0] = 1.0-ss;
		s[1] = ss;
		t[0] = tt[0];
		t[1] = tt[1];
	}
	if( min.update( kmb::Collision::testSegTri( p0, p2, q0, q1, q2, ss, tt ) ) ){
		s[0] = 0.0;
		s[1] = ss;
		t[0] = tt[0];
		t[1] = tt[1];
	}
	if( min.update( kmb::Collision::testSegTri( q0, q1, p0, p1, p2, ss, tt ) ) ){
		s[0] = tt[0];
		s[1] = tt[1];
		t[0] = ss;
		t[1] = 0.0;
	}
	if( min.update( kmb::Collision::testSegTri( q1, q2, p0, p1, p2, ss, tt ) ) ){
		s[0] = tt[0];
		s[1] = tt[1];
		t[0] = 1.0-ss;
		t[1] = ss;
	}
	if( min.update( kmb::Collision::testSegTri( q0, q2, p0, p1, p2, ss, tt ) ) ){
		s[0] = tt[0];
		s[1] = tt[1];
		t[0] = 0.0;
		t[1] = ss;
	}
	return min.getMin();
}

// 厳しめに判定する
kmb::Collision::collisionType
kmb::Collision::detectSegTri2(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2)
{
	double tolerance = kmb::Tolerance::geometric;
	kmb::BoundingBox bbox;
	bbox.update(p0);
	bbox.update(p1);
	bbox.update(q0);
	bbox.update(q1);
	bbox.update(q2);
	double r = bbox.range();
	double v = r*r*r;
	double volumes[5] = {
		kmb::Point3D::volume(p0,p1,q0,q1),
		kmb::Point3D::volume(p0,p1,q1,q2),
		kmb::Point3D::volume(p0,p1,q2,q0),
		kmb::Point3D::volume(p0,q0,q1,q2),
		kmb::Point3D::volume(q0,q1,q2,p1)
	};
	int checkPosi = 0;
	int checkZero = 0;
	int checkNega = 0;

	for(int i=0;i<5;++i){
		if( volumes[i] > tolerance*v ){
			checkPosi++;
		}else if( volumes[i] < -tolerance*v ){
			checkNega++;
		}else{
			checkZero++;
		}
	}
	if( checkPosi == 5 || checkNega == 5 ){
		return kmb::Collision::kCollision;
	}else if( checkPosi + checkZero == 5 || checkNega + checkZero == 5 ){
		return kmb::Collision::kGeometricalContact;
	}else{
		return kmb::Collision::kNotCollision;
	}
}

// 安全よりに判定する
kmb::Collision::collisionType
kmb::Collision::detectSegTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2)
{
	double tolerance = kmb::Tolerance::geometric;
	double ss = 0.0;
	double tt[2] = {0.0, 0.0};
	double dist = kmb::Collision::testSegTri( p0, p1, q0, q1, q2, ss, tt );
	kmb::BoundingBox bbox;
	bbox.update(p0);
	bbox.update(p1);
	bbox.update(q0);
	bbox.update(q1);
	bbox.update(q2);
	double r = bbox.range();
	if( dist < r*r*tolerance ){
		if( 0.0-tolerance < tt[0] && 0.0-tolerance < tt[1] && tt[0] + tt[1] < 1.0+tolerance && 0.0-tolerance < ss && ss < 1.0+tolerance ){
			return kmb::Collision::kCollision;
		}
		return kmb::Collision::kGeometricalContact;
	}
	return kmb::Collision::kNotCollision;
}

kmb::Collision::collisionType
kmb::Collision::detectTriTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2)
{
	kmb::Collision::collisionType colType = kmb::Collision::NOT_COLLIDE;

	switch( detectSegTri( p0, p1, q0, q1, q2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( detectSegTri( p1, p2, q0, q1, q2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( detectSegTri( p2, p0, q0, q1, q2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( detectSegTri( q0, q1, p0, p1, p2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( detectSegTri( q1, q2, p0, p1, p2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( detectSegTri( q2, q0, p0, p1, p2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	return colType;
}

kmb::Collision::collisionType
kmb::Collision::detectTriTri2(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2)
{
	kmb::Collision::collisionType colType = kmb::Collision::NOT_COLLIDE;

	switch( kmb::Collision::detectSegTri2( p0, p1, q0, q1, q2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( kmb::Collision::detectSegTri2( p1, p2, q0, q1, q2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( kmb::Collision::detectSegTri2( p2, p0, q0, q1, q2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( kmb::Collision::detectSegTri2( q0, q1, p0, p1, p2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( kmb::Collision::detectSegTri2( q1, q2, p0, p1, p2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	switch( kmb::Collision::detectSegTri2( q2, q0, p0, p1, p2 ) ){
		case kmb::Collision::DETECT:
			return kmb::Collision::DETECT;
		case kmb::Collision::GEOMETRICAL_CONTACT:
			colType = kmb::Collision::GEOMETRICAL_CONTACT;
			break;
		default:
			break;
	}

	return colType;
}
