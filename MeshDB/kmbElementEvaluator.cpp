/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ElementEvaluator                                        #
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
#include <cmath>
#include <cfloat>

#include "MeshDB/kmbElementEvaluator.h"

#include "Geometry/kmb_Sphere.h"
#include "Geometry/kmb_Circle.h"
#include "Geometry/kmb_Geometry.h"
#include "Geometry/kmb_Calculator.h"
#include "Geometry/kmb_Point2DContainer.h"
#include "Geometry/kmb_Point3DContainer.h"

#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbTriangle2.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbQuad2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbTetrahedron2.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbWedge.h"
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
				 && points->getPoint( eIter.getCellId(4), n4 )
				 && points->getPoint( eIter.getCellId(5), n5 )
				 && points->getPoint( eIter.getCellId(6), n6 )
				 && points->getPoint( eIter.getCellId(7), n7 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
				 && points->getPoint( eIter.getCellId(4), n4 )
				 && points->getPoint( eIter.getCellId(5), n5 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
				 && points->getPoint( eIter.getCellId(4), n4 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 && points->getPoint( eIter.getCellId(3), n3 ) )
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 && points->getPoint( eIter.getCellId(2), n2 )
				 )
				{
					return kmb::Point3D::area( n0, n1, n2 );
				}else{
					return -DBL_MAX;
				}
			}
			break;
		case QUAD:
		case QUAD2:
			{
				kmb::Point3D a,b,c,d;
				if( points->getPoint( eIter.getCellId(0), a )
				 && points->getPoint( eIter.getCellId(1), b )
				 && points->getPoint( eIter.getCellId(2), c )
				 && points->getPoint( eIter.getCellId(3), d )
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
			break;
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
				if( point2Ds->getPoint( eIter.getCellId(0), n0 )
				 && point2Ds->getPoint( eIter.getCellId(1), n1 )
				 && point2Ds->getPoint( eIter.getCellId(2), n2 )
				 )
				{
					return kmb::Point2D::area( n0, n1, n2 );
				}else{
					return -DBL_MAX;
				}
			}
			break;
		case QUAD:
		case QUAD2:
			{
				kmb::Point2D a,b,c,d;
				if( point2Ds->getPoint( eIter.getCellId(0), a )
				 && point2Ds->getPoint( eIter.getCellId(1), b )
				 && point2Ds->getPoint( eIter.getCellId(2), c )
				 && point2Ds->getPoint( eIter.getCellId(3), d )
				)
				{
					return
						kmb::Point2D::area( a, b, c ) +
						kmb::Point2D::area( a, c, d );
				}else{
					return -DBL_MAX;
				}
			}
			break;
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
				if( points->getPoint( eIter.getCellId(0), n0 )
				 && points->getPoint( eIter.getCellId(1), n1 )
				 )
				{
					return n0.distance( n1 );
				}else{
					return -DBL_MAX;
				}
			}
			break;
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
				if( point2Ds->getPoint( eIter.getCellId(0), n0 )
				 && point2Ds->getPoint( eIter.getCellId(1), n1 )
				 )
				{
					return n0.distance( n1 );
				}else{
					return -DBL_MAX;
				}
			}
			break;
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
				if( points->getPoint( eIter.getCellId(2), p0 ) &&
					points->getPoint( eIter.getCellId(0), p1 ) &&
					points->getPoint( eIter.getCellId(1), p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter.getCellId(0), p0 ) &&
					points->getPoint( eIter.getCellId(1), p1 ) &&
					points->getPoint( eIter.getCellId(2), p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter.getCellId(1), p0 ) &&
					points->getPoint( eIter.getCellId(2), p1 ) &&
					points->getPoint( eIter.getCellId(0), p2 ) )
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
				if( points->getPoint( eIter.getCellId(3), p0 ) &&
					points->getPoint( eIter.getCellId(0), p1 ) &&
					points->getPoint( eIter.getCellId(1), p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter.getCellId(0), p0 ) &&
					points->getPoint( eIter.getCellId(1), p1 ) &&
					points->getPoint( eIter.getCellId(2), p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter.getCellId(1), p0 ) &&
					points->getPoint( eIter.getCellId(2), p1 ) &&
					points->getPoint( eIter.getCellId(3), p2 ) )
				{
					angle = kmb::Point3D::angle(p0,p1,p2);
				}
				break;
			case 3:
				if( points->getPoint( eIter.getCellId(2), p0 ) &&
					points->getPoint( eIter.getCellId(3), p1 ) &&
					points->getPoint( eIter.getCellId(0), p2 ) )
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
				if( point2Ds->getPoint( eIter.getCellId(2), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter.getCellId(0), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter.getCellId(1), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p2 ) )
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
				if( point2Ds->getPoint( eIter.getCellId(3), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter.getCellId(0), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter.getCellId(1), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(3), p2 ) )
				{
					angle = kmb::Point2D::angle(p0,p1,p2);
				}
				break;
			case 3:
				if( point2Ds->getPoint( eIter.getCellId(2), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(3), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p2 ) )
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
				if( points->getPoint( eIter.getCellId(2), p0 ) &&
					points->getPoint( eIter.getCellId(0), p1 ) &&
					points->getPoint( eIter.getCellId(1), p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter.getCellId(0), p0 ) &&
					points->getPoint( eIter.getCellId(1), p1 ) &&
					points->getPoint( eIter.getCellId(2), p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter.getCellId(1), p0 ) &&
					points->getPoint( eIter.getCellId(2), p1 ) &&
					points->getPoint( eIter.getCellId(0), p2 ) )
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
				if( points->getPoint( eIter.getCellId(3), p0 ) &&
					points->getPoint( eIter.getCellId(0), p1 ) &&
					points->getPoint( eIter.getCellId(1), p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( points->getPoint( eIter.getCellId(0), p0 ) &&
					points->getPoint( eIter.getCellId(1), p1 ) &&
					points->getPoint( eIter.getCellId(2), p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( points->getPoint( eIter.getCellId(1), p0 ) &&
					points->getPoint( eIter.getCellId(2), p1 ) &&
					points->getPoint( eIter.getCellId(3), p2 ) )
				{
					c = kmb::Point3D::cos(p0,p1,p2);
				}
				break;
			case 3:
				if( points->getPoint( eIter.getCellId(2), p0 ) &&
					points->getPoint( eIter.getCellId(3), p1 ) &&
					points->getPoint( eIter.getCellId(0), p2 ) )
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
				if( point2Ds->getPoint( eIter.getCellId(2), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter.getCellId(0), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter.getCellId(1), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p2 ) )
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
				if( point2Ds->getPoint( eIter.getCellId(3), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 1:
				if( point2Ds->getPoint( eIter.getCellId(0), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(1), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 2:
				if( point2Ds->getPoint( eIter.getCellId(1), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(2), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(3), p2 ) )
				{
					c = kmb::Point2D::cos(p0,p1,p2);
				}
				break;
			case 3:
				if( point2Ds->getPoint( eIter.getCellId(2), p0 ) &&
					point2Ds->getPoint( eIter.getCellId(3), p1 ) &&
					point2Ds->getPoint( eIter.getCellId(0), p2 ) )
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
		if( points->getPoint( element.getCellId(i), node ) ){
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
		if( points->getPoint( element.getCellId(i), node ) ){
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
			if( points->getPoint( element.getCellId(0), a ) &&
				points->getPoint( element.getCellId(1), b ) &&
				points->getPoint( element.getCellId(2), c )  )
			{
				vect = kmb::Point3D::calcNormalVector( a, b, c );
				return true;
			}
			break;
		case kmb::QUAD:
		case kmb::QUAD2:
			if( points->getPoint( element.getCellId(0), a) &&
				points->getPoint( element.getCellId(1), b) &&
				points->getPoint( element.getCellId(2), c) &&
				points->getPoint( element.getCellId(3), d) )
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
kmb::ElementEvaluator::getNaturalCoordinates(const kmb::ElementBase &element,const double x,const double y,const double z,double* retvals,double margin) const
{
	if( points == NULL ){
		return false;
	}
	switch( element.getType() )
	{
	case kmb::TETRAHEDRON:
		{
			kmb::Point3D pt[4];
			double physCoords[3] = { x, y, z };
			if( points->getPoint( element.getCellId(0), pt[0] )
				&& points->getPoint( element.getCellId(1), pt[1] )
				&& points->getPoint( element.getCellId(2), pt[2] )
				&& points->getPoint( element.getCellId(3), pt[3] ) )
			{
				return kmb::Tetrahedron::getNaturalCoordinates( physCoords, pt, retvals, margin );
			}
			break;
		}
	case kmb::TETRAHEDRON2:
		{
			kmb::Point3D pt[10];
			double physCoords[3] = { x, y, z };
			for(int i=0;i<10;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			return kmb::Tetrahedron2::getNaturalCoordinates( physCoords, pt, retvals, margin );
			break;
		}
	case kmb::PYRAMID:
		{
			kmb::Point3D pt[5];
			double physCoords[3] = { x, y, z };
			for(int i=0;i<5;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			return kmb::Pyramid::getNaturalCoordinates( physCoords, pt, retvals, margin );
			break;
		}
	case kmb::WEDGE:
		{
			kmb::Point3D pt[6];
			double physCoords[3] = { x, y, z };
			for(int i=0;i<6;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			return kmb::Wedge::getNaturalCoordinates( physCoords, pt, retvals, margin );
			break;
		}
	case kmb::HEXAHEDRON:
		{
			kmb::Point3D pt[8];
			double physCoords[3] = { x, y, z };
			for(int i=0;i<8;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			return kmb::Hexahedron::getNaturalCoordinates( physCoords, pt, retvals, margin );
			break;
		}
	default:
		break;
	}
	return false;
}

bool
kmb::ElementEvaluator::getPhysicalCoordinates(const kmb::ElementBase &element,const double s,const double t,const double u,double* retvals) const
{
	if( points == NULL || retvals == NULL ){
		return false;
	}
	switch( element.getType() )
	{
	case kmb::TETRAHEDRON:
		{
			kmb::Point3D pt[4];
			double natCoords[3] = { s, t, u };
			if( points->getPoint( element.getCellId(0), pt[0] )
				&& points->getPoint( element.getCellId(1), pt[1] )
				&& points->getPoint( element.getCellId(2), pt[2] )
				&& points->getPoint( element.getCellId(3), pt[3] ) )
			{
				kmb::Tetrahedron::getPhysicalCoordinates( natCoords, pt, retvals );
				return true;
			}
			break;
		}
	case kmb::TETRAHEDRON2:
		{
			kmb::Point3D pt[10];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<10;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			kmb::Tetrahedron2::getPhysicalCoordinates( natCoords, pt, retvals );
			return true;
			break;
		}
	case kmb::PYRAMID:
		{
			kmb::Point3D pt[5];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<5;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			kmb::Pyramid::getPhysicalCoordinates( natCoords, pt, retvals );
			return true;
			break;
		}
	case kmb::WEDGE:
		{
			kmb::Point3D pt[6];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<6;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			kmb::Wedge::getPhysicalCoordinates( natCoords, pt, retvals );
			return true;
			break;
		}
	case kmb::HEXAHEDRON:
		{
			kmb::Point3D pt[8];
			double natCoords[3] = { s, t, u };
			for(int i=0;i<8;++i){
				if( !points->getPoint( element.getCellId(i), pt[i] ) ){
					return false;
				}
			}
			kmb::Hexahedron::getPhysicalCoordinates( natCoords, pt, retvals );
			return true;
			break;
		}
	default:
		break;
	}
	return false;
}
