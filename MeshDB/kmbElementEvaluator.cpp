/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementEvaluator                                        #
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
#include <cfloat>

#include "MeshDB/kmbElementEvaluator.h"

#include "Common/kmbIntegration.h"

#include "Geometry/kmbSphere.h"
#include "Geometry/kmbCircle.h"
#include "Geometry/kmbGeometry.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbPoint3DContainer.h"

#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbSegment2.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbTriangle2.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbQuad2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbTetrahedron2.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbHexahedron2.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbPyramid2.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbWedge2.h"
#include "MeshDB/kmbFace.h"

kmb::ElementEvaluator::ElementEvaluator(const kmb::Point3DContainer* points)
: points(NULL)
, point2Ds(NULL)
{
	this->points = points;
}

kmb::ElementEvaluator::ElementEvaluator(const kmb::Point2DContainer* point2Ds)
: points(NULL)
, point2Ds(NULL)
{
	this->point2Ds = point2Ds;
}

kmb::ElementEvaluator::~ElementEvaluator(void)
{
}

double
kmb::ElementEvaluator::getAspectRatio(const kmb::ElementBase &eIter) const
{
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:

			{
				kmb::Node n0,n1,n2,n3;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 )
				{
					double inscribe = kmb::Sphere::getInscribedRadius( n0, n1, n2, n3 );
					double circum = kmb::Sphere::getCircumscribedRadius( n0, n1, n2, n3 );
					if( inscribe > 0.0 ){
						return circum / (inscribe*3.0);
					}
				}
			}
			break;
		case TRIANGLE:
		case TRIANGLE2:
			{
				kmb::Node n0,n1,n2;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 )
				{
					double inscribe = kmb::Circle3D::getInscribedRadius( n0, n1, n2 );
					double circum = kmb::Circle3D::getCircumscribedRadius( n0, n1, n2 );
					if( inscribe > 0.0 ){
						return circum / (inscribe*2.0);
					}
				}
			}
			break;
		case HEXAHEDRON:
		case HEXAHEDRON2:

			{
				kmb::Node n0,n1,n3,n4;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[3], n3 )
				 && points->getPoint( eIter[4], n4 )
				 )
				{
					double q01 = n0.distance(n1);
					double q03 = n0.distance(n3);
					double q04 = n0.distance(n4);
					kmb::BoundingBox1D bbox;
					bbox.update(q01);
					bbox.update(q03);
					bbox.update(q04);
					return bbox.getMax() / bbox.getMin();
				}
			}
			break;
		case QUAD:
		case QUAD2:

			{
				kmb::Node n0,n1,n3;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[3], n3 )
				 )
				{
					double q01 = n0.distance(n1);
					double q03 = n0.distance(n3);
					if( q01 > q03 ){
						return q01 / q03;
					}else{
						return q03 / q01;
					}
				}
			}
			break;
		default:
			break;
		}
	}
	return -1.0;
}

double
kmb::ElementEvaluator::getAspectRatio(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const
{
	kmb::Node p0,p1,p2,p3;
	if( points
	 &&	points->getPoint( n0, p0 )
	 && points->getPoint( n1, p1 )
	 && points->getPoint( n2, p2 )
	 && points->getPoint( n3, p3 )
	 )
	{
		double inscribe = kmb::Sphere::getInscribedRadius( p0, p1, p2, p3 );
		double circum = kmb::Sphere::getCircumscribedRadius( p0, p1, p2, p3 );
		if( inscribe > 0.0 ){
			return circum / (inscribe*3.0);
		}
	}
	return -1.0;
}

double
kmb::ElementEvaluator::getInscribedRadius(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const
{
	kmb::Node p0,p1,p2,p3;
	if( points
	 &&	points->getPoint( n0, p0 )
	 && points->getPoint( n1, p1 )
	 && points->getPoint( n2, p2 )
	 && points->getPoint( n3, p3 )
	 )
	{
		return kmb::Sphere::getInscribedRadius( p0, p1, p2, p3 );
	}else{
		return 0.0;
	}
}

double
kmb::ElementEvaluator::getCircumRadius(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const
{
	kmb::Node p0,p1,p2,p3;
	if( points
	 && points->getPoint( n0, p0 )
	 && points->getPoint( n1, p1 )
	 && points->getPoint( n2, p2 )
	 && points->getPoint( n3, p3 )
	 )
	{
		return kmb::Sphere::getCircumscribedRadius( p0, p1, p2, p3 );
	}else{
		return 0.0;
	}
}

double
kmb::ElementEvaluator::getEdgeLengthRatio(const kmb::ElementBase &eIter) const
{
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			{
				kmb::Node n0,n1,n2,n3;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 )
				{
					kmb::BoundingBox1D bbox;
					bbox.update( n0.distanceSq(n1) );
					bbox.update( n0.distanceSq(n2) );
					bbox.update( n0.distanceSq(n3) );
					bbox.update( n1.distanceSq(n2) );
					bbox.update( n1.distanceSq(n3) );
					bbox.update( n2.distanceSq(n3) );
					if( bbox.getMin() > 0.0 ){
						return bbox.getMax() / bbox.getMin();
					}
				}
			}
			break;
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			{
				kmb::Node n0,n1,n2,n3,n4,n5,n6,n7;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 && points->getPoint( eIter[4], n4 )
				 && points->getPoint( eIter[5], n5 )
				 && points->getPoint( eIter[6], n6 )
				 && points->getPoint( eIter[7], n7 )
				 )
				{
					kmb::BoundingBox1D bbox;
					bbox.update( n0.distanceSq(n1) );
					bbox.update( n0.distanceSq(n3) );
					bbox.update( n0.distanceSq(n4) );
					bbox.update( n1.distanceSq(n2) );
					bbox.update( n1.distanceSq(n5) );
					bbox.update( n2.distanceSq(n3) );
					bbox.update( n2.distanceSq(n6) );
					bbox.update( n3.distanceSq(n7) );
					bbox.update( n4.distanceSq(n5) );
					bbox.update( n4.distanceSq(n7) );
					bbox.update( n5.distanceSq(n6) );
					bbox.update( n6.distanceSq(n7) );
					if( bbox.getMin() > 0.0 ){
						return bbox.getMax() / bbox.getMin();
					}
				}
			}
			break;
		case kmb::WEDGE:
		case kmb::WEDGE2:
			{
				kmb::Node n0,n1,n2,n3,n4,n5;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 && points->getPoint( eIter[4], n4 )
				 && points->getPoint( eIter[5], n5 )
				 )
				{
					kmb::BoundingBox1D bbox;
					bbox.update( n1.distanceSq(n2) );
					bbox.update( n0.distanceSq(n2) );
					bbox.update( n0.distanceSq(n1) );
					bbox.update( n4.distanceSq(n5) );
					bbox.update( n3.distanceSq(n5) );
					bbox.update( n3.distanceSq(n4) );
					bbox.update( n0.distanceSq(n3) );
					bbox.update( n1.distanceSq(n4) );
					bbox.update( n2.distanceSq(n5) );
					if( bbox.getMin() > 0.0 ){
						return bbox.getMax() / bbox.getMin();
					}
				}
			}
			break;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			{
				kmb::Node n0,n1,n2,n3,n4;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 && points->getPoint( eIter[4], n4 )
				 )
				{
					kmb::BoundingBox1D bbox;
					bbox.update( n0.distanceSq(n1) );
					bbox.update( n0.distanceSq(n2) );
					bbox.update( n0.distanceSq(n3) );
					bbox.update( n0.distanceSq(n4) );
					bbox.update( n1.distanceSq(n2) );
					bbox.update( n2.distanceSq(n3) );
					bbox.update( n3.distanceSq(n4) );
					bbox.update( n1.distanceSq(n4) );
					if( bbox.getMin() > 0.0 ){
						return bbox.getMax() / bbox.getMin();
					}
				}
			}
			break;
		case TRIANGLE:
		case TRIANGLE2:
			{
				kmb::Node n0,n1,n2;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 )
				{
					kmb::BoundingBox1D bbox;
					bbox.update( n0.distanceSq(n1) );
					bbox.update( n0.distanceSq(n2) );
					bbox.update( n1.distanceSq(n2) );
					if( bbox.getMin() > 0.0 ){
						return bbox.getMax() / bbox.getMin();
					}
				}
			}
			break;
		case QUAD:
		case QUAD2:
			{
				kmb::Node n0,n1,n2,n3;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 )
				{
					kmb::BoundingBox1D bbox;
					bbox.update( n0.distanceSq(n1) );
					bbox.update( n1.distanceSq(n2) );
					bbox.update( n2.distanceSq(n3) );
					bbox.update( n3.distanceSq(n0) );
					if( bbox.getMin() > 0.0 ){
						return bbox.getMax() / bbox.getMin();
					}
				}
			}
			break;
		default:
			break;
		}
	}
	return -1.0;
}

double
kmb::ElementEvaluator::getMaxEdgeLength(const kmb::ElementBase &eIter) const
{
	if( points != NULL ){
		kmb::Maximizer max;
		kmb::Node n0,n1;
		int len = eIter.getEdgeCount();
		for(int i=0;i<len;++i){
			if( points->getPoint( eIter.getEdgeCellId(i,0), n0 ) &&
				points->getPoint( eIter.getEdgeCellId(i,1), n1 ) )
			{
				max.update( n0.distance(n1) );
			}
		}
		return max.getMax();
	}
	return -1.0;
}

double
kmb::ElementEvaluator::getAverageEdgeLength(const kmb::ElementBase &eIter) const
{
	if( points != NULL ){
		kmb::AverageCalculator ave;
		kmb::Node n0,n1;
		int len = eIter.getEdgeCount();
		for(int i=0;i<len;++i){
			if( points->getPoint( eIter.getEdgeCellId(i,0), n0 ) &&
				points->getPoint( eIter.getEdgeCellId(i,1), n1 ) )
			{
				ave.add( n0.distance(n1) );
			}
		}
		return ave.getAverage();
	}
	return -1.0;
}

double
kmb::ElementEvaluator::getMinAngleCos(const kmb::ElementBase &eIter) const
{
	kmb::Minimizer min;
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			{
				kmb::Node n0,n1,n2;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 )
				{
					min.update( kmb::Point3D::cos(n0,n1,n2) );
					min.update( kmb::Point3D::cos(n1,n2,n0) );
					min.update( kmb::Point3D::cos(n2,n0,n1) );
				}
			}
			break;
		case kmb::QUAD:
		case kmb::QUAD2:
			{
				kmb::Node n0,n1,n2,n3;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 )
				{
					min.update( kmb::Point3D::cos(n0,n1,n2) );
					min.update( kmb::Point3D::cos(n1,n2,n3) );
					min.update( kmb::Point3D::cos(n2,n3,n0) );
					min.update( kmb::Point3D::cos(n3,n0,n1) );
				}
			}
			break;
		default:
			break;
		}
	}
	return min.getMin();
}

double
kmb::ElementEvaluator::getMaxAngleCos(const kmb::ElementBase &eIter) const
{
	kmb::Maximizer max;
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			{
				kmb::Node n0,n1,n2;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 )
				{
					max.update( kmb::Point3D::cos(n0,n1,n2) );
					max.update( kmb::Point3D::cos(n1,n2,n0) );
					max.update( kmb::Point3D::cos(n2,n0,n1) );
				}
			}
			break;
		case kmb::QUAD:
		case kmb::QUAD2:
			{
				kmb::Node n0,n1,n2,n3;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 )
				 )
				{
					max.update( kmb::Point3D::cos(n0,n1,n2) );
					max.update( kmb::Point3D::cos(n1,n2,n3) );
					max.update( kmb::Point3D::cos(n2,n3,n0) );
					max.update( kmb::Point3D::cos(n3,n0,n1) );
				}
			}
			break;
		default:
			break;
		}
	}
	return max.getMax();
}

double
kmb::ElementEvaluator::getMinAngleTri(const kmb::ElementBase &eIter) const
{
	kmb::Maximizer max;
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TETRAHEDRON:
			{
				kmb::Triangle tri;
				for(int i=0;i<4;++i){
					if( eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::TETRAHEDRON2:
			{
				kmb::Triangle2 tri;
				for(int i=0;i<4;++i){
					if( eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::PYRAMID:
		case kmb::WEDGE:
			{
				kmb::Triangle tri;
				int len = eIter.getBoundaryCount();
				for(int i=0;i<len;++i){
					if( eIter.getBoundaryType(i) == kmb::TRIANGLE && eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::PYRAMID2:
		case kmb::WEDGE2:
			{
				kmb::Triangle2 tri;
				int len = eIter.getBoundaryCount();
				for(int i=0;i<len;++i){
					if( eIter.getBoundaryType(i) == kmb::TRIANGLE2 && eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			{
				max.update( this->getMaxAngleCos(eIter) );
			}
			break;
		default:
			break;
		}
	}
	if( max.getMax() <= 1.0 ){
		return COS2ANGLE( max.getMax() );
	}else{
		return DBL_MAX;
	}
}

double
kmb::ElementEvaluator::getMinAngle(const kmb::ElementBase &eIter) const
{
	kmb::Maximizer max;
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TETRAHEDRON:
			{
				kmb::Triangle tri;
				for(int i=0;i<4;++i){
					if( eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::TETRAHEDRON2:
			{
				kmb::Triangle2 tri;
				for(int i=0;i<4;++i){
					if( eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::HEXAHEDRON:
			{
				kmb::Quad q;
				for(int i=0;i<6;++i){
					if( eIter.getBoundaryElement(i,q) ){
						max.update( this->getMaxAngleCos(q) );
					}
				}
			}
			break;
		case kmb::HEXAHEDRON2:
			{
				kmb::Quad2 q;
				for(int i=0;i<6;++i){
					if( eIter.getBoundaryElement(i,q) ){
						max.update( this->getMaxAngleCos(q) );
					}
				}
			}
			break;
		case kmb::PYRAMID:
		case kmb::WEDGE:
			{
				kmb::Triangle tri;
				kmb::Quad q;
				int len = eIter.getBoundaryCount();
				for(int i=0;i<len;++i){
					if( eIter.getBoundaryType(i) == kmb::QUAD && eIter.getBoundaryElement(i,q) ){
						max.update( this->getMaxAngleCos(q) );
					}else if( eIter.getBoundaryType(i) == kmb::TRIANGLE && eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::PYRAMID2:
		case kmb::WEDGE2:
			{
				kmb::Triangle2 tri;
				kmb::Quad2 q;
				int len = eIter.getBoundaryCount();
				for(int i=0;i<len;++i){
					if( eIter.getBoundaryType(i) == kmb::QUAD2 && eIter.getBoundaryElement(i,q) ){
						max.update( this->getMaxAngleCos(q) );
					}else if( eIter.getBoundaryType(i) == kmb::TRIANGLE2 && eIter.getBoundaryElement(i,tri) ){
						max.update( this->getMaxAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
			{
				max.update( this->getMaxAngleCos(eIter) );
			}
			break;
		default:
			break;
		}
	}
	if( max.getMax() <= 1.0 ){
		return COS2ANGLE( max.getMax() );
	}else{
		return DBL_MAX;
	}
}

double
kmb::ElementEvaluator::getMaxAngle(const kmb::ElementBase &eIter) const
{
	kmb::Minimizer min;
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TETRAHEDRON:
			{
				kmb::Triangle tri;
				for(int i=0;i<4;++i){
					if( eIter.getBoundaryElement(i,tri) ){
						min.update( this->getMinAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::TETRAHEDRON2:
			{
				kmb::Triangle2 tri;
				for(int i=0;i<4;++i){
					if( eIter.getBoundaryElement(i,tri) ){
						min.update( this->getMinAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::HEXAHEDRON:
			{
				kmb::Quad q;
				for(int i=0;i<6;++i){
					if( eIter.getBoundaryElement(i,q) ){
						min.update( this->getMinAngleCos(q) );
					}
				}
			}
			break;
		case kmb::HEXAHEDRON2:
			{
				kmb::Quad2 q;
				for(int i=0;i<6;++i){
					if( eIter.getBoundaryElement(i,q) ){
						min.update( this->getMinAngleCos(q) );
					}
				}
			}
			break;
		case kmb::PYRAMID:
		case kmb::WEDGE:
			{
				kmb::Triangle tri;
				kmb::Quad q;
				int len = eIter.getBoundaryCount();
				for(int i=0;i<len;++i){
					if( eIter.getBoundaryType(i) == kmb::QUAD && eIter.getBoundaryElement(i,q) ){
						min.update( this->getMinAngleCos(q) );
					}else if( eIter.getBoundaryType(i) == kmb::TRIANGLE && eIter.getBoundaryElement(i,tri) ){
						min.update( this->getMinAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::PYRAMID2:
		case kmb::WEDGE2:
			{
				kmb::Triangle2 tri;
				kmb::Quad2 q;
				int len = eIter.getBoundaryCount();
				for(int i=0;i<len;++i){
					if( eIter.getBoundaryType(i) == kmb::QUAD2 && eIter.getBoundaryElement(i,q) ){
						min.update( this->getMinAngleCos(q) );
					}else if( eIter.getBoundaryType(i) == kmb::TRIANGLE2 && eIter.getBoundaryElement(i,tri) ){
						min.update( this->getMinAngleCos(tri) );
					}
				}
			}
			break;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
			{
				min.update( this->getMinAngleCos(eIter) );
			}
			break;
		default:
			break;
		}
	}
	if( min.getMin() >= -1.0 ){
		return COS2ANGLE( min.getMin() );
	}else{
		return -DBL_MAX;
	}
}

double
kmb::ElementEvaluator::getVolume(const kmb::ElementBase &eIter) const
{
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			{
				kmb::Node n0,n1,n2,n3;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 && points->getPoint( eIter[3], n3 ) )
				{
					return kmb::Point3D::volume( n0, n1, n2, n3 );
				}
				break;
			}
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
		case kmb::WEDGE:
		case kmb::WEDGE2:
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			{
				kmb::Node n0,n1,n2,n3;
				kmb::nodeIdType tetrahedrons[6][4];
				for(int i=0;i>6;++i){
					for(int j=0;j<4;++j){
						tetrahedrons[i][j] = kmb::nullNodeId;
					}
				}
				const int len = eIter.divideIntoTetrahedrons(tetrahedrons);
				double volume = 0.0;
				for(int i=0;i<len;++i){
					if( points->getPoint( tetrahedrons[i][0], n0 )
					 && points->getPoint( tetrahedrons[i][1], n1 )
					 && points->getPoint( tetrahedrons[i][2], n2 )
					 && points->getPoint( tetrahedrons[i][3], n3 ) )
					{
						volume += kmb::Point3D::volume( n0, n1, n2, n3 );
					}
				}
				return volume;
			}
		default:
			break;
		}
	}
	return 0.0;
}

double
kmb::ElementEvaluator::getVolume
(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const
{
	if( points != NULL ){
		kmb::Node p0,p1,p2,p3;
		if( points->getPoint( n0, p0 )
		 && points->getPoint( n1, p1 )
		 && points->getPoint( n2, p2 )
		 && points->getPoint( n3, p3 ) )
		{
			return kmb::Point3D::volume( p0, p1, p2, p3 );
		}
	}
	return 0.0;
}

double
kmb::ElementEvaluator::getSharpness(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const
{
	if( points != NULL ){
		kmb::Node p0,p1,p2,p3;
		if( points->getPoint( n0, p0 )
		 && points->getPoint( n1, p1 )
		 && points->getPoint( n2, p2 )
		 && points->getPoint( n3, p3 ) )
		{
			kmb::Vector3D v30(p3,p0);
			kmb::Vector3D v31(p3,p1);
			kmb::Vector3D v32(p3,p2);
			v30.normalize();
			v31.normalize();
			v32.normalize();
			return kmb::Vector3D::triple(v30,v31,v32);
		}
	}
	return 0.0;
}

double
kmb::ElementEvaluator::getArea(const kmb::ElementBase &eIter) const
{
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case TRIANGLE:
		case TRIANGLE2:
			{
				kmb::Point3D n0,n1,n2;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 && points->getPoint( eIter[2], n2 )
				 )
				{
					return kmb::Point3D::area( n0, n1, n2 );
				}else{
					return -DBL_MAX;
				}
			}
		case QUAD:
		case QUAD2:
			{
				kmb::Point3D a,b,c,d;
				if( points->getPoint( eIter[0], a )
				 && points->getPoint( eIter[1], b )
				 && points->getPoint( eIter[2], c )
				 && points->getPoint( eIter[3], d )
				)
				{
					return 0.5 * (
						kmb::Point3D::area( a, b, c ) +
						kmb::Point3D::area( a, c, d ) +
						kmb::Point3D::area( b, c, d ) +
						kmb::Point3D::area( b, d, a ) );
				}else{
					return -DBL_MAX;
				}
			}
		default:
			break;
		}
	}else if( point2Ds != NULL ){
		switch( eIter.getType() )
		{
		case TRIANGLE:
		case TRIANGLE2:
			{
				kmb::Point2D n0,n1,n2;
				if( point2Ds->getPoint( eIter[0], n0 )
				 && point2Ds->getPoint( eIter[1], n1 )
				 && point2Ds->getPoint( eIter[2], n2 )
				 )
				{
					return kmb::Point2D::area( n0, n1, n2 );
				}else{
					return -DBL_MAX;
				}
			}
		case QUAD:
		case QUAD2:
			{
				kmb::Point2D a,b,c,d;
				if( point2Ds->getPoint( eIter[0], a )
				 && point2Ds->getPoint( eIter[1], b )
				 && point2Ds->getPoint( eIter[2], c )
				 && point2Ds->getPoint( eIter[3], d )
				)
				{
					return
						kmb::Point2D::area( a, b, c ) +
						kmb::Point2D::area( a, c, d );
				}else{
					return -DBL_MAX;
				}
			}
		default:
			break;
		}
	}
	return 0.0;
}

double
kmb::ElementEvaluator::getArea(const kmb::ElementBase &eIter,kmb::idType localFaceId) const
{
	if( points != NULL ){
		switch( eIter.getBoundaryType(localFaceId) )
		{
		case TRIANGLE:
		case TRIANGLE2:
			{
				kmb::Point3D n0,n1,n2;
				if( points->getPoint( eIter.getBoundaryCellId(localFaceId,0), n0 )
				 && points->getPoint( eIter.getBoundaryCellId(localFaceId,1), n1 )
				 && points->getPoint( eIter.getBoundaryCellId(localFaceId,2), n2 )
				 )
				{
					return kmb::Point3D::area( n0, n1, n2 );
				}else{
					return -DBL_MAX;
				}
			}
		case QUAD:
		case QUAD2:
			{
				kmb::Point3D a,b,c,d;
				if( points->getPoint( eIter.getBoundaryCellId(localFaceId,0), a )
				 && points->getPoint( eIter.getBoundaryCellId(localFaceId,1), b )
				 && points->getPoint( eIter.getBoundaryCellId(localFaceId,2), c )
				 && points->getPoint( eIter.getBoundaryCellId(localFaceId,3), d )
				)
				{
					return 0.5 * (
						kmb::Point3D::area( a, b, c ) +
						kmb::Point3D::area( a, c, d ) +
						kmb::Point3D::area( b, c, d ) +
						kmb::Point3D::area( b, d, a ) );
				}else{
					return -DBL_MAX;
				}
			}
		default:
			break;
		}
	}else if( point2Ds != NULL ){
		switch( eIter.getBoundaryType(localFaceId) )
		{
		case TRIANGLE:
		case TRIANGLE2:
			{
				kmb::Point2D n0,n1,n2;
				if( point2Ds->getPoint( eIter.getBoundaryCellId(localFaceId,0), n0 )
				 && point2Ds->getPoint( eIter.getBoundaryCellId(localFaceId,1), n1 )
				 && point2Ds->getPoint( eIter.getBoundaryCellId(localFaceId,2), n2 )
				 )
				{
					return kmb::Point2D::area( n0, n1, n2 );
				}else{
					return -DBL_MAX;
				}
			}
		case QUAD:
		case QUAD2:
			{
				kmb::Point2D a,b,c,d;
				if( point2Ds->getPoint( eIter.getBoundaryCellId(localFaceId,0), a )
				 && point2Ds->getPoint( eIter.getBoundaryCellId(localFaceId,1), b )
				 && point2Ds->getPoint( eIter.getBoundaryCellId(localFaceId,2), c )
				 && point2Ds->getPoint( eIter.getBoundaryCellId(localFaceId,3), d )
				)
				{
					return
						kmb::Point2D::area( a, b, c ) +
						kmb::Point2D::area( a, c, d );
				}else{
					return -DBL_MAX;
				}
			}
		default:
			break;
		}
	}
	return 0.0;
}

double
kmb::ElementEvaluator::getLength(const kmb::ElementBase &eIter) const
{
	if( points != NULL ){
		switch( eIter.getType() )
		{
		case SEGMENT:
		case SEGMENT2:
			{
				kmb::Node n0,n1;
				if( points->getPoint( eIter[0], n0 )
				 && points->getPoint( eIter[1], n1 )
				 )
				{
					return n0.distance( n1 );
				}else{
					return -DBL_MAX;
				}
			}
		default:
			break;
		}
	}else if( point2Ds != NULL ){
		switch( eIter.getType() )
		{
		case SEGMENT:
		case SEGMENT2:
			{
				kmb::Point2D n0,n1;
				if( point2Ds->getPoint( eIter[0], n0 )
				 && point2Ds->getPoint( eIter[1], n1 )
				 )
				{
					return n0.distance( n1 );
				}else{
					return -DBL_MAX;
				}
			}
		default:
			break;
		}
	}
	return 0.0;
}

double
kmb::ElementEvaluator::getAngle(const kmb::ElementBase &eIter,kmb::nodeIdType nodeId) const
{
	int index = eIter.indexOf( nodeId );
	return kmb::ElementEvaluator::getAngleByIndex(eIter,index);
}

double
kmb::ElementEvaluator::getAngleByIndex(const kmb::ElementBase &eIter,int index) const
{
	double angle = -PI;
	if( points != NULL ){
		kmb::Point3D p0;
		kmb::Point3D p1;
		kmb::Point3D p2;
		switch( eIter.getType() )
		{
		case TRIANGLE:
		case TRIANGLE2:
			switch(index){
			case 0:
				if( points->getPoint( eIter[2], p0 ) &&
					points->getPoint( eIter[0], p1 ) &&
					points->getPoint( eIter[1], p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter[0], p0 ) &&
					points->getPoint( eIter[1], p1 ) &&
					points->getPoint( eIter[2], p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter[1], p0 ) &&
					points->getPoint( eIter[2], p1 ) &&
					points->getPoint( eIter[0], p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			default:
				break;
			}
			break;
		case QUAD:
		case QUAD2:
			switch(index){
			case 0:
				if( points->getPoint( eIter[3], p0 ) &&
					points->getPoint( eIter[0], p1 ) &&
					points->getPoint( eIter[1], p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter[0], p0 ) &&
					points->getPoint( eIter[1], p1 ) &&
					points->getPoint( eIter[2], p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter[1], p0 ) &&
					points->getPoint( eIter[2], p1 ) &&
					points->getPoint( eIter[3], p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 3:
				if( points->getPoint( eIter[2], p0 ) &&
					points->getPoint( eIter[3], p1 ) &&
					points->getPoint( eIter[0], p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			default:
				break;
			}
		default:
			break;
		}
	}else if( point2Ds != NULL ){
		kmb::Point2D p0;
		kmb::Point2D p1;
		kmb::Point2D p2;
		switch( eIter.getType() )
		{
		case TRIANGLE:
		case TRIANGLE2:
			switch(index){
			case 0:
				if( point2Ds->getPoint( eIter[2], p0 ) &&
					point2Ds->getPoint( eIter[0], p1 ) &&
					point2Ds->getPoint( eIter[1], p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter[0], p0 ) &&
					point2Ds->getPoint( eIter[1], p1 ) &&
					point2Ds->getPoint( eIter[2], p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter[1], p0 ) &&
					point2Ds->getPoint( eIter[2], p1 ) &&
					point2Ds->getPoint( eIter[0], p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			default:
				break;
			}
			break;
		case QUAD:
		case QUAD2:
			switch(index){
			case 0:
				if( point2Ds->getPoint( eIter[3], p0 ) &&
					point2Ds->getPoint( eIter[0], p1 ) &&
					point2Ds->getPoint( eIter[1], p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter[0], p0 ) &&
					point2Ds->getPoint( eIter[1], p1 ) &&
					point2Ds->getPoint( eIter[2], p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter[1], p0 ) &&
					point2Ds->getPoint( eIter[2], p1 ) &&
					point2Ds->getPoint( eIter[3], p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 3:
				if( point2Ds->getPoint( eIter[2], p0 ) &&
					point2Ds->getPoint( eIter[3], p1 ) &&
					point2Ds->getPoint( eIter[0], p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
	return angle;
}

double
kmb::ElementEvaluator::getCosByIndex(const kmb::ElementBase &eIter,int index) const
{
	double c = -1.0;
	if( points != NULL ){
		kmb::Point3D p0;
		kmb::Point3D p1;
		kmb::Point3D p2;
		switch( eIter.getType() )
		{
		case TRIANGLE:
		case TRIANGLE2:
			switch(index){
			case 0:
				if( points->getPoint( eIter[2], p0 ) &&
					points->getPoint( eIter[0], p1 ) &&
					points->getPoint( eIter[1], p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter[0], p0 ) &&
					points->getPoint( eIter[1], p1 ) &&
					points->getPoint( eIter[2], p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter[1], p0 ) &&
					points->getPoint( eIter[2], p1 ) &&
					points->getPoint( eIter[0], p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			default:
				break;
			}
			break;
		case QUAD:
		case QUAD2:
			switch(index){
			case 0:
				if( points->getPoint( eIter[3], p0 ) &&
					points->getPoint( eIter[0], p1 ) &&
					points->getPoint( eIter[1], p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter[0], p0 ) &&
					points->getPoint( eIter[1], p1 ) &&
					points->getPoint( eIter[2], p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter[1], p0 ) &&
					points->getPoint( eIter[2], p1 ) &&
					points->getPoint( eIter[3], p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 3:
				if( points->getPoint( eIter[2], p0 ) &&
					points->getPoint( eIter[3], p1 ) &&
					points->getPoint( eIter[0], p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			default:
				break;
			}
		default:
			break;
		}
	}else if( point2Ds != NULL ){
		kmb::Point2D p0;
		kmb::Point2D p1;
		kmb::Point2D p2;
		switch( eIter.getType() )
		{
		case TRIANGLE:
		case TRIANGLE2:
			switch(index){
			case 0:
				if( point2Ds->getPoint( eIter[2], p0 ) &&
					point2Ds->getPoint( eIter[0], p1 ) &&
					point2Ds->getPoint( eIter[1], p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter[0], p0 ) &&
					point2Ds->getPoint( eIter[1], p1 ) &&
					point2Ds->getPoint( eIter[2], p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter[1], p0 ) &&
					point2Ds->getPoint( eIter[2], p1 ) &&
					point2Ds->getPoint( eIter[0], p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			default:
				break;
			}
			break;
		case QUAD:
		case QUAD2:
			switch(index){
			case 0:
				if( point2Ds->getPoint( eIter[3], p0 ) &&
					point2Ds->getPoint( eIter[0], p1 ) &&
					point2Ds->getPoint( eIter[1], p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter[0], p0 ) &&
					point2Ds->getPoint( eIter[1], p1 ) &&
					point2Ds->getPoint( eIter[2], p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter[1], p0 ) &&
					point2Ds->getPoint( eIter[2], p1 ) &&
					point2Ds->getPoint( eIter[3], p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 3:
				if( point2Ds->getPoint( eIter[2], p0 ) &&
					point2Ds->getPoint( eIter[3], p1 ) &&
					point2Ds->getPoint( eIter[0], p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
	return c;
}

bool
kmb::ElementEvaluator::getBoundingBox(const kmb::ElementBase &element,kmb::BoundingBox &bbox) const
{
	const int nodeCount = element.getNodeCount();
	kmb::Point3D node;
	for(int i=0;i<nodeCount;++i){
		if( points->getPoint( element[i], node ) ){
			bbox.update( node );
		}
	}
	return true;
}

double
kmb::ElementEvaluator::getBoundingBoxRadius(const kmb::ElementBase &element) const
{
	kmb::BoundingBox bbox;
	const int nodeCount = element.getNodeCount();
	kmb::Point3D node;
	for(int i=0;i<nodeCount;++i){
		if( points->getPoint( element[i], node ) ){
			bbox.update( node );
		}
	}
	return 0.5 * bbox.diameter();
}

bool
kmb::ElementEvaluator::getNormalVector(const kmb::ElementBase &element, kmb::Vector3D &vect) const
{
	if( points != NULL ){
		kmb::Point3D a;
		kmb::Point3D b;
		kmb::Point3D c;
		kmb::Point3D d;
		switch( element.getType() )
		{
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			if( points->getPoint( element[0], a ) &&
				points->getPoint( element[1], b ) &&
				points->getPoint( element[2], c )  )
			{
				vect = kmb::Point3D::calcNormalVector( a, b, c );
				return true;
			}
			break;
		case kmb::QUAD:
		case kmb::QUAD2:
			if( points->getPoint( element[0], a) &&
				points->getPoint( element[1], b) &&
				points->getPoint( element[2], c) &&
				points->getPoint( element[3], d) )
			{
				kmb::Vector3D n0 = kmb::Point3D::calcNormalVector( a, b, c );
				kmb::Vector3D n1 = kmb::Point3D::calcNormalVector( a, c, d );
				vect = n0 + n1;
				vect.normalize();
				return true;
			}
		default:
			break;
		}
	}
	return false;
}

bool
kmb::ElementEvaluator::getNormalVector(const Face &face, const kmb::ElementContainer* elements, kmb::Vector3D &vect) const
{
	if( elements && points )
	{
		kmb::ElementContainer::const_iterator element = elements->find( face.getElementId() );
		if( element.getDimension() == 3 ){
			int index = static_cast<int>( face.getLocalFaceId() );
			const int size = element.getBoundaryVertexCount(index);
			if( size == 3 ){
				kmb::Point3D a,b,c;
				if( points->getPoint( element.getBoundaryCellId(index, 0 ), a ) &&
					points->getPoint( element.getBoundaryCellId(index, 1 ), b ) &&
					points->getPoint( element.getBoundaryCellId(index, 2 ), c ) ){
					vect = kmb::Point3D::calcNormalVector( a, b, c );
					return true;
				}
			}else if( size == 4 ){
				kmb::Point3D a,b,c,d;
				if( points->getPoint( element.getBoundaryCellId(index, 0 ), a ) &&
					points->getPoint( element.getBoundaryCellId(index, 1 ), b ) &&
					points->getPoint( element.getBoundaryCellId(index, 2 ), c ) &&
					points->getPoint( element.getBoundaryCellId(index, 3 ), d ) ){
					kmb::Vector3D n0 = kmb::Point3D::calcNormalVector( a, b, c );
					kmb::Vector3D n1 = kmb::Point3D::calcNormalVector( a, c, d );
					vect = n0 + n1;
					vect.normalize();
					return true;
				}
			}
		}
	}
	return false;
}

bool
kmb::ElementEvaluator::getNormalVectorOfFace(const kmb::ElementBase &element, int index, kmb::Vector3D &vect) const
{
	if( points != NULL ){
		kmb::Point3D a;
		kmb::Point3D b;
		kmb::Point3D c;
		kmb::Point3D d;
		switch( element.getBoundaryType(index) )
		{
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			if( points->getPoint( element.getBoundaryCellId(index,0), a ) &&
				points->getPoint( element.getBoundaryCellId(index,1), b ) &&
				points->getPoint( element.getBoundaryCellId(index,2), c )  )
			{
				vect = kmb::Point3D::calcNormalVector( a, b, c );
				return true;
			}
			break;
		case kmb::QUAD:
		case kmb::QUAD2:
			if( points->getPoint( element.getBoundaryCellId(index,0), a) &&
				points->getPoint( element.getBoundaryCellId(index,1), b) &&
				points->getPoint( element.getBoundaryCellId(index,2), c) &&
				points->getPoint( element.getBoundaryCellId(index,3), d) )
			{
				kmb::Vector3D n0 = kmb::Point3D::calcNormalVector( a, b, c );
				kmb::Vector3D n1 = kmb::Point3D::calcNormalVector( a, c, d );
				vect = n0 + n1;
				vect.normalize();
				return true;
			}
		default:
			break;
		}
	}
	return false;
}

double
kmb::ElementEvaluator::getAngleBetweenElements(const kmb::ElementBase &element0,const kmb::ElementBase &element1) const
{
	double angle = 0.0;
	kmb::Vector3D v0, v1;
	if( getNormalVector(element0,v0) && getNormalVector(element1,v1) ){
		angle = kmb::Vector3D::angle(v0,v1);
	}
	return angle;
}

double
kmb::ElementEvaluator::getCosBetweenElements(const kmb::ElementBase &element0,const kmb::ElementBase &element1) const
{
	double c = 0.0;
	kmb::Vector3D v0, v1;
	if( getNormalVector(element0,v0) && getNormalVector(element1,v1) ){
		c = - kmb::Vector3D::cos(v0,v1);
	}
	return c;
}

bool
kmb::ElementEvaluator::getNaturalCoordinates(const kmb::ElementBase &element,const double x,const double y,const double z,double* coords) const
{
	if( points == NULL ){
		return false;
	}
	switch( element.getType() )
	{
	case kmb::TETRAHEDRON:
		{
			kmb::Point3D pt[4];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<4;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Tetrahedron::getNaturalCoordinates( target, pt, coords );
		}
	case kmb::TETRAHEDRON2:
		{
			kmb::Point3D pt[10];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<10;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Tetrahedron2::getNaturalCoordinates( target, pt, coords );
		}
	case kmb::PYRAMID:
		{
			kmb::Point3D pt[5];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<5;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Pyramid::getNaturalCoordinates( target, pt, coords );
		}
	case kmb::WEDGE:
		{
			kmb::Point3D pt[6];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<6;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Wedge::getNaturalCoordinates( target, pt, coords );
		}
	case kmb::HEXAHEDRON:
		{
			kmb::Point3D pt[8];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<8;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Hexahedron::getNaturalCoordinates( target, pt, coords );
		}
	case kmb::HEXAHEDRON2:
		{
			kmb::Point3D pt[20];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<20;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Hexahedron::getNaturalCoordinates( target, pt, coords );
		}
	default:
		break;
	}
	return false;
}

double
kmb::ElementEvaluator::checkShapeFunctionDomain(const kmb::ElementBase &element,double s,double t,double u)
{
	switch( element.getType() )
	{
	case kmb::TETRAHEDRON:
		return kmb::Tetrahedron::checkShapeFunctionDomain(s,t,u);
	case kmb::TETRAHEDRON2:
		return kmb::Tetrahedron2::checkShapeFunctionDomain(s,t,u);
	case kmb::HEXAHEDRON:
		return kmb::Hexahedron::checkShapeFunctionDomain(s,t,u);
	case kmb::HEXAHEDRON2:
		return kmb::Hexahedron2::checkShapeFunctionDomain(s,t,u);
	case kmb::WEDGE:
		return kmb::Wedge::checkShapeFunctionDomain(s,t,u);
	case kmb::WEDGE2:
		return kmb::Wedge2::checkShapeFunctionDomain(s,t,u);
	case kmb::PYRAMID:
		return kmb::Pyramid::checkShapeFunctionDomain(s,t,u);
	case kmb::PYRAMID2:
		return kmb::Pyramid2::checkShapeFunctionDomain(s,t,u);
	case kmb::TRIANGLE:
		return kmb::Triangle::checkShapeFunctionDomain(s,t);
	case kmb::TRIANGLE2:
		return kmb::Triangle2::checkShapeFunctionDomain(s,t);
	case kmb::QUAD:
		return kmb::Triangle::checkShapeFunctionDomain(s,t);
	case kmb::QUAD2:
		return kmb::Triangle2::checkShapeFunctionDomain(s,t);
	case kmb::SEGMENT:
		return kmb::Segment::checkShapeFunctionDomain(s);
	case kmb::SEGMENT2:
		return kmb::Segment2::checkShapeFunctionDomain(s);
	default:
		break;
	}
	return -DBL_MAX;
}

double
kmb::ElementEvaluator::getWeightElement(const kmb::ElementBase &element,const double x,const double y,const double z,double* weights) const
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	double coords[3] = {0.0,0.0,0.0};
	switch( element.getType() )
	{
	case kmb::TETRAHEDRON:
		{
			kmb::Point3D pt[4];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<4;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return -DBL_MAX;
				}
			}
			coords[0] = 0.25;
			coords[1] = 0.25;
			coords[2] = 0.25;
			if( kmb::Tetrahedron::getNaturalCoordinates( target, pt, coords ) ){
				kmb::Tetrahedron::shapeFunction(coords[0], coords[1], coords[2], weights);
				return kmb::Tetrahedron::checkShapeFunctionDomain(coords[0], coords[1], coords[2]);
			}
			break;
		}
	case kmb::TETRAHEDRON2:
		{
			kmb::Point3D pt[10];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<10;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return -DBL_MAX;
				}
			}
			coords[0] = 0.25;
			coords[1] = 0.25;
			coords[2] = 0.25;
			if( kmb::Tetrahedron2::getNaturalCoordinates( target, pt, coords ) ){
				kmb::Tetrahedron2::shapeFunction(coords[0], coords[1], coords[2], weights);
				return kmb::Tetrahedron2::checkShapeFunctionDomain(coords[0], coords[1], coords[2]);
			}
			break;
		}
	case kmb::PYRAMID:
		{
			kmb::Point3D pt[5];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<5;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return -DBL_MAX;
				}
			}
			if( kmb::Pyramid::getNaturalCoordinates( target, pt, coords ) ){
				kmb::Pyramid::shapeFunction(coords[0], coords[1], coords[2], weights);
				return kmb::Pyramid::checkShapeFunctionDomain(coords[0], coords[1], coords[2]);
			}
			break;
		}
	case kmb::WEDGE:
		{
			kmb::Point3D pt[6];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<6;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return -DBL_MAX;
				}
			}
			if( kmb::Wedge::getNaturalCoordinates( target, pt, coords ) ){
				kmb::Wedge::shapeFunction(coords[0], coords[1], coords[2], weights);
				return kmb::Wedge::checkShapeFunctionDomain(coords[0], coords[1], coords[2]);
			}
			break;
		}
	case kmb::HEXAHEDRON:
		{
			kmb::Point3D pt[8];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<8;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return -DBL_MAX;
				}
			}
			if( kmb::Hexahedron::getNaturalCoordinates( target, pt, coords ) ){
				kmb::Hexahedron::shapeFunction(coords[0], coords[1], coords[2], weights);
				return kmb::Hexahedron::checkShapeFunctionDomain(coords[0], coords[1], coords[2]);
			}
			break;
		}
	case kmb::HEXAHEDRON2:
		{
			kmb::Point3D pt[20];
			kmb::Point3D target(x,y,z);
			for(int i=0;i<20;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return -DBL_MAX;
				}
			}
			if( kmb::Hexahedron2::getNaturalCoordinates( target, pt, coords ) ){
				kmb::Hexahedron2::shapeFunction(coords[0], coords[1], coords[2], weights);
				return kmb::Hexahedron2::checkShapeFunctionDomain(coords[0], coords[1], coords[2]);
			}
			break;
		}
	default:
		break;
	}
	return -DBL_MAX;
}

double
kmb::ElementEvaluator::getWeightElementFace(const kmb::ElementBase &element,int index,const double x,const double y,const double z,double* weights) const
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	double coords[2] = {0.0,0.0};
	switch( element.getBoundaryType(index) )
	{
	case kmb::TRIANGLE:
	{
		kmb::Point3D pt[3];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<3;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return -DBL_MAX;
			}
		}
		coords[0] = 0.25;
		coords[1] = 0.25;
		if( kmb::Triangle::getNaturalCoordinates( target, pt, coords ) ){
			kmb::Triangle::shapeFunction(coords[0], coords[1], weights);
			return kmb::Triangle::checkShapeFunctionDomain(coords[0], coords[1]);
		}
		break;
	}
	case kmb::TRIANGLE2:
	{
		kmb::Point3D pt[6];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<6;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return -DBL_MAX;
			}
		}
		coords[0] = 0.25;
		coords[1] = 0.25;
		if( kmb::Triangle2::getNaturalCoordinates( target, pt, coords ) ){
			kmb::Triangle2::shapeFunction(coords[0], coords[1], weights);
			return kmb::Triangle2::checkShapeFunctionDomain(coords[0], coords[1]);
		}
		break;
	}
	case kmb::QUAD:
	{
		kmb::Point3D pt[4];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<4;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return -DBL_MAX;
			}
		}
		coords[0] = 0.0;
		coords[1] = 0.0;
		if( kmb::Quad::getNaturalCoordinates( target, pt, coords ) ){
			kmb::Quad::shapeFunction(coords[0], coords[1], weights);
			return kmb::Quad::checkShapeFunctionDomain(coords[0], coords[1]);
		}
		break;
	}
	case kmb::QUAD2:
	{
		kmb::Point3D pt[8];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<8;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return -DBL_MAX;
			}
		}
		coords[0] = 0.0;
		coords[1] = 0.0;
		if( kmb::Quad2::getNaturalCoordinates( target, pt, coords ) ){
			kmb::Quad2::shapeFunction(coords[0], coords[1], weights);
			return kmb::Quad2::checkShapeFunctionDomain(coords[0], coords[1]);
		}
		break;
	}
	default:
		break;
	}
	return -DBL_MAX;
}

bool
kmb::ElementEvaluator::getNaturalCoordinatesOfFace(const kmb::ElementBase &element,int index,const double x,const double y,const double z,double* retvals) const
{
	if( points == NULL ){
		return false;
	}
	switch( element.getBoundaryType(index) )
	{
	case kmb::TRIANGLE:
	{
		kmb::Point3D pt[3];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<3;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return false;
			}
		}
		return kmb::Triangle::getNaturalCoordinates( target, pt, retvals );
	}
	case kmb::TRIANGLE2:
	{
		kmb::Point3D pt[6];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<6;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return false;
			}
		}
		return kmb::Triangle2::getNaturalCoordinates( target, pt, retvals );
	}
	case kmb::QUAD:
	{
		kmb::Point3D pt[4];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<4;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return false;
			}
		}
		return kmb::Quad::getNaturalCoordinates( target, pt, retvals );
	}
	case kmb::QUAD2:
	{
		kmb::Point3D pt[8];
		kmb::Point3D target(x,y,z);
		for(int i=0;i<8;++i){
			if( !points->getPoint( element.getBoundaryCellId(index,i), pt[i] ) ){
				return false;
			}
		}
		return kmb::Quad2::getNaturalCoordinates( target, pt, retvals );
	}
	default:
		break;
	}
	return false;
}

bool
kmb::ElementEvaluator::getPhysicalCoordinates(const kmb::ElementBase &element,const double s,const double t,const double u,kmb::Point3D &target) const
{
	if( points == NULL ){
		return false;
	}
	switch( element.getType() )
	{
	case kmb::TETRAHEDRON:
		{
			kmb::Point3D pt[4];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<4;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Tetrahedron::getPhysicalCoordinates( natCoords, pt, target );
		}
	case kmb::TETRAHEDRON2:
		{
			kmb::Point3D pt[10];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<10;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Tetrahedron2::getPhysicalCoordinates( natCoords, pt, target );
		}
	case kmb::PYRAMID:
		{
			kmb::Point3D pt[5];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<5;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Pyramid::getPhysicalCoordinates( natCoords, pt, target );
		}
	case kmb::WEDGE:
		{
			kmb::Point3D pt[6];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<6;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Wedge::getPhysicalCoordinates( natCoords, pt, target );
		}
	case kmb::HEXAHEDRON:
		{
			kmb::Point3D pt[8];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<8;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Hexahedron::getPhysicalCoordinates( natCoords, pt, target );
		}
	case kmb::HEXAHEDRON2:
		{
			kmb::Point3D pt[20];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<20;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			return kmb::Hexahedron2::getPhysicalCoordinates( natCoords, pt, target );
		}
	default:
		break;
	}
	return false;
}


bool
kmb::ElementEvaluator::getMinMaxJacobian(const kmb::ElementBase &element, double &min, double &max) const
{
	if( points == NULL ){
		return false;
	}
	kmb::BoundingBox1D bbox;
	switch( element.getType() )
	{
	case kmb::HEXAHEDRON:
		{
			kmb::Point3D pt[8];
			for(int i=0;i<8;++i){
				if( !points->getPoint( element[i], pt[i] ) ){
					return false;
				}
			}
			for(int i=0;i<3;++i){
				for(int j=0;j<3;++j){
					for(int k=0;k<3;++k){
						bbox.update( kmb::Hexahedron::jacobian(
							kmb::Integration::Gauss_Point3[i],
							kmb::Integration::Gauss_Point3[j],
							kmb::Integration::Gauss_Point3[k], pt) );
					}
				}
			}
			for(int i=0;i<1;++i){
				double s = -1.0 + i*2.0;
				for(int j=0;j<1;++j){
					double t = -1.0 + j*2.0;
					for(int k=0;k<1;++k){
						double u = -1.0 + k*2.0;
						bbox.update( kmb::Hexahedron::jacobian(s,t,u,pt) );
					}
				}
			}
			min = bbox.getMin();
			max = bbox.getMax();
			return true;
		}
	default:
		break;
	}
	return false;
}

double
kmb::ElementEvaluator::getMinInnerVolume(const kmb::ElementBase &element,const double x,const double y,const double z) const
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	kmb::Minimizer minimizer;
	kmb::Point3D p0,p1,p2,p3;
	p0.setCoordinate(x,y,z);

	int len = element.getBoundaryCount();
	for(int i=0;i<len;++i){
		switch( element.getBoundaryType(i) )
		{
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p1 ) &&
				points->getPoint( element.getBoundaryCellId(i,1), p2 ) &&
				points->getPoint( element.getBoundaryCellId(i,2), p3 ) )
			{
				minimizer.update( kmb::Point3D::volume( p0, p1, p2, p3 ) );
			}
			break;
		case kmb::QUAD:
		case kmb::QUAD2:
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p1 ) &&
				points->getPoint( element.getBoundaryCellId(i,1), p2 ) &&
				points->getPoint( element.getBoundaryCellId(i,2), p3 ) )
			{
				minimizer.update( kmb::Point3D::volume( p0, p1, p2, p3 ) );
			}
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p1 ) &&
				points->getPoint( element.getBoundaryCellId(i,2), p2 ) &&
				points->getPoint( element.getBoundaryCellId(i,3), p3 ) )
			{
				minimizer.update( kmb::Point3D::volume( p0, p1, p2, p3 ) );
			}
			break;
		default:
			minimizer.update(-DBL_MAX);
			break;
		}
	}
	return minimizer.getMin();
}

double
kmb::ElementEvaluator::getDistanceSqOnBoundary(const kmb::ElementBase &element,const double x,const double y,const double z) const
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	kmb::Minimizer minimizer;
	kmb::Point3D q,p0,p1,p2;
	q.setCoordinate(x,y,z);

	int len = element.getBoundaryCount();
	for(int i=0;i<len;++i){
		switch( element.getBoundaryType(i) )
		{
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p0 ) &&
				points->getPoint( element.getBoundaryCellId(i,1), p1 ) &&
				points->getPoint( element.getBoundaryCellId(i,2), p2 ) )
			{
				minimizer.update( q.distanceSqToTriangle(p0,p1,p2) );
			}
			break;
		case kmb::QUAD:
		case kmb::QUAD2:
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p0 ) &&
				points->getPoint( element.getBoundaryCellId(i,1), p1 ) &&
				points->getPoint( element.getBoundaryCellId(i,2), p2 ) )
			{
				minimizer.update( q.distanceSqToTriangle(p0,p1,p2) );
			}
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p0 ) &&
				points->getPoint( element.getBoundaryCellId(i,2), p1 ) &&
				points->getPoint( element.getBoundaryCellId(i,3), p2 ) )
			{
				minimizer.update( q.distanceSqToTriangle(p0,p1,p2) );
			}
			break;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p0 ) &&
				points->getPoint( element.getBoundaryCellId(i,1), p1 ) )
			{
				minimizer.update( q.distanceSqToSegment(p0,p1) );
			}
			break;
		case kmb::VERTEX:
			if(
				points->getPoint( element.getBoundaryCellId(i,0), p0 ) )
			{
				minimizer.update( q.distanceSq(p1) );
			}
			break;
		default:
			break;
		}
	}
	return minimizer.getMin();
}

double
kmb::ElementEvaluator::getDistanceSq(const kmb::ElementBase &element,const double x,const double y,const double z) const
{
	if( points == NULL ){
		return -DBL_MAX;
	}
	kmb::Point3D pt(x,y,z);

	switch( element.getType() )
	{
	case kmb::TRIANGLE:
	case kmb::TRIANGLE2:
	{
		kmb::Point3D q[3];
		if( points->getPoint( element[0], q[0] ) &&
			points->getPoint( element[1], q[1] ) &&
			points->getPoint( element[2], q[2] ) )
		{
			return pt.distanceSqToTriangle(q[0],q[1],q[2]);
		}
		break;
	}
	case kmb::QUAD:
	case kmb::QUAD2:
	{
		kmb::Point3D q[4];
		if( points->getPoint( element[0], q[0] ) &&
			points->getPoint( element[1], q[1] ) &&
			points->getPoint( element[2], q[2] ) &&
			points->getPoint( element[3], q[3] ) )
		{
			return kmb::Minimizer::getMin(
				pt.distanceSqToTriangle( q[0], q[1], q[2] ),
				pt.distanceSqToTriangle( q[2], q[3], q[0] ) );
		}
		break;
	}
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
	{
		kmb::Point3D q[2];
		if(
			points->getPoint( element[0], q[0] ) &&
			points->getPoint( element[1], q[1] ) )
		{
			return pt.distanceSqToSegment(q[0],q[1]);
		}
		break;
	}
	default:
		break;
	}
	return DBL_MAX;
}

int
kmb::ElementEvaluator::getDuplicationNodeIdCount(const kmb::ElementBase &element) const
{
	int count = 0;
	int len = element.getNodeCount();
	for(int i=0;i<len;++i){
		for(int j=i+1;j<len;++j){
			if( element[i] == element[j] ){
				++count;
			}
		}
	}
	return count;
}
