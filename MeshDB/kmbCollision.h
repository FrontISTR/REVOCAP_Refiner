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
#pragma once

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbFace.h"

namespace kmb{

class Point3D;
class Point3DContainer;
class ElementBase;
class MeshData;

class Collision
{
protected:
	const Point3DContainer* points;
	MeshData* mesh;
	double accuracy;
public:
	Collision(kmb::MeshData* mesh);
	virtual ~Collision(void);
	void setAccuracy(double acc);
	double getAccuracy(void) const;

	double distanceSqPtTri(kmb::nodeIdType nodeId,kmb::ElementBase& tri,double s[2]) const;
	double distanceSqPtQuad(kmb::Point3D& pt,kmb::ElementBase& quad) const;

	double distanceSqSegSeg(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::nodeIdType b0,kmb::nodeIdType b1,double &t1, double &t2) const;
	double distanceSqSegSeg(kmb::ElementBase& seg0,kmb::ElementBase& seg1,double &t1, double &t2) const;
	double distanceSegSeg(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::nodeIdType b0,kmb::nodeIdType b1) const;
	double distanceSegSeg(kmb::ElementBase& seg0,kmb::ElementBase& seg1) const;

	double distanceSqSegTri(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::ElementBase& tri,double &s,double t[2]) const;
	double distanceSegTri(kmb::nodeIdType a0,kmb::nodeIdType a1,kmb::ElementBase& tri) const;

	double distanceSqTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1,double s[2],double t[2]) const;
	double distanceTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1) const;


	double distanceSqQuadQuad(kmb::ElementBase& q0,kmb::ElementBase& q1) const;
	double distanceQuadQuad(kmb::ElementBase& q0,kmb::ElementBase& q1) const;

	double distanceSqTriQuad(kmb::ElementBase& tri,kmb::ElementBase& q) const;
	double distanceTriQuad(kmb::ElementBase& tri,kmb::ElementBase& q) const;

	double distanceSq(kmb::ElementBase& elem0,kmb::ElementBase& elem1) const;
	double distance(kmb::ElementBase& elem0,kmb::ElementBase& elem1) const;
	double distanceSq(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements) const;
	double distance(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements) const;

	enum collisionType{
		COLLISION_ERROR = -2,
		NOT_COLLIDE = -1,
		TOPOLOGICAL_CONTACT = 0,
		GEOMETRICAL_CONTACT = 1,
		DETECT = 2
	};
	static const char* collisionTypeString( collisionType ctype );

	collisionType detectTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1) const;
	collisionType detectTriTetra(kmb::ElementBase& tri,kmb::ElementBase& tetra) const;




	double distanceByNodeTri(kmb::ElementBase& t0,kmb::ElementBase& t1,int &index) const;
	double distanceByNodeTri(kmb::ElementBase& t0,kmb::Face &f,const kmb::ElementContainer* elements,int &index) const;



	double distanceByNodeQuad(kmb::ElementBase& q0,kmb::ElementBase& q1,int &index) const;
	double distanceByNodeQuad(kmb::ElementBase& q0,kmb::Face &f,const kmb::ElementContainer* elements,int &index) const;

	double distanceByNode(kmb::ElementBase& elem0,kmb::ElementBase& elem1,int &index) const;
	double distanceByNode(kmb::ElementBase& elem0,kmb::Face& f,const kmb::ElementContainer* elements,int &index) const;




	int detectFaceGroup(const char* fg0,const char* fg1,double thres,const char* result) const;



	int detectSurfaceFaceGroup(kmb::bodyIdType bodyId0,const char* fg1,double thres,const char* result) const;



	int detectSurfaceFaceGroupByNode(kmb::bodyIdType bodyId0,const char* fg1,int tCount,double thres,const char* result) const;




	double getNearestFace(kmb::ElementBase& q0,const char* fg,kmb::Face &nearestFace,int &index) const;


	double distanceSqSegSeg(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& q0,kmb::Point3D& q1,double &t1, double &t2) const;
	double distanceSqSegTri(kmb::Point3D& p0,kmb::Point3D& p1,
		kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,
		double &s,double t[2]) const;
	double distanceSqTriTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,
		kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2,
		double s[2],double t[2]) const;


	collisionType detectTriTri(kmb::Point3D& p0,kmb::Point3D& p1,kmb::Point3D& p2,
		kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2) const;
	collisionType detectSegTri(kmb::Point3D& p0,kmb::Point3D& p1,
		kmb::Point3D& q0,kmb::Point3D& q1,kmb::Point3D& q2) const;
};

}
