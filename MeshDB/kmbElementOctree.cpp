/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ElementOctree                                           #
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
#include "MeshDB/kmbElementOctree.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbTypes.h"
#include "Geometry/kmb_Calculator.h"

kmb::ElementOctree::ElementOctree(int layer,int thres)
: layer(0)
, thres(256)
, count(0)
, elementIds(NULL)
, children(NULL)
{
	this->layer = layer;
	this->thres = thres;
}

kmb::ElementOctree::~ElementOctree(void)
{
	clear();
}


void
kmb::ElementOctree::clear(void)
{
	if( children != NULL )
	{
		for( int i=0; i<8; ++i )
		{
			if( children[i] != NULL )
			{
				delete children[i];
				children[i] = NULL;
			}
		}
		delete[] children;
		children = NULL;
	}
	count = 0;
	if( elementIds ){
		delete[] elementIds;
		elementIds = NULL;
	}
	boundBox.initialize();
	center.zero();
}

int
kmb::ElementOctree::getLocalCount(void) const
{
	return count;
}

size_t
kmb::ElementOctree::getCount(void) const
{
	size_t sum = 0;
	if( children != NULL ){
		for(int i=0;i<8;++i){
			sum += children[i]->getCount();
		}
	}else{
		sum = count;
	}
	return sum;
}

void
kmb::ElementOctree::addElement(kmb::elementIdType elementId,kmb::BoundingBox* bbox,kmb::MeshData* mesh,double safetyRatio)
{
	if( children != NULL )
	{
		REVOCAP_Debug_3("ElementOctree::addElement 0 %d\n",elementId);
		bool bxmax = ( this->center.x() < bbox->getMax().x() );
		bool bymax = ( this->center.y() < bbox->getMax().y() );
		bool bzmax = ( this->center.z() < bbox->getMax().z() );
		bool bxmin = ( this->center.x() > bbox->getMin().x() );
		bool bymin = ( this->center.y() > bbox->getMin().y() );
		bool bzmin = ( this->center.z() > bbox->getMin().z() );

		if( bxmax && bymax && bzmax )	children[7]->addElement(elementId,bbox,mesh,safetyRatio);
		if( bxmax && bymax && bzmin )	children[6]->addElement(elementId,bbox,mesh,safetyRatio);
		if( bxmax && bymin && bzmax )	children[5]->addElement(elementId,bbox,mesh,safetyRatio);
		if( bxmax && bymin && bzmin )	children[4]->addElement(elementId,bbox,mesh,safetyRatio);
		if( bxmin && bymax && bzmax )	children[3]->addElement(elementId,bbox,mesh,safetyRatio);
		if( bxmin && bymax && bzmin )	children[2]->addElement(elementId,bbox,mesh,safetyRatio);
		if( bxmin && bymin && bzmax )	children[1]->addElement(elementId,bbox,mesh,safetyRatio);
		if( bxmin && bymin && bzmin )	children[0]->addElement(elementId,bbox,mesh,safetyRatio);
	}
	else if(elementIds != NULL)
	{
		REVOCAP_Debug_3("ElementOctree::addElement 1\n");
		elementIds[count] = elementId;
		++count;
		if( count == thres )
		{
			REVOCAP_Debug_3("ElementOctree::addElement 2\n");
			createChildren(mesh,safetyRatio);
		}
	}
	else if(count == 0)
	{
		REVOCAP_Debug_3("ElementOctree::addElement 3\n");
		elementIds = new kmb::elementIdType[thres];
		elementIds[count] = elementId;
		++count;
	}
}

void
kmb::ElementOctree::generateSearchCache(kmb::MeshData* mesh,kmb::bodyIdType bodyId,double safetyRatio)
{
	kmb::ElementContainer* elements = NULL;
	if( mesh != NULL &&
		(elements=mesh->getBodyPtr(bodyId))!=NULL &&
		elements->getCount() > 0 &&
		mesh->getNodeCount() > 0 &&
		mesh->getNodes() )
	{
		kmb::ElementEvaluator evaluator( mesh->getNodes() );
		REVOCAP_Debug("generateSearchCache 0\n");
		kmb::Point3D maxPt;
		elements->getMaxPoint( maxPt );
		kmb::Point3D minPt;
		elements->getMinPoint( minPt );
		if( maxPt.x() < minPt.x()
		 || maxPt.y() < minPt.y()
		 || maxPt.z() < minPt.z() )
		{
			elements->updateBoundingBox( mesh->getNodes() );
			elements->getMaxPoint( maxPt );
			elements->getMinPoint( minPt );
		}
		this->boundBox.update( minPt );
		this->boundBox.update( maxPt );
		REVOCAP_Debug("generateSearchCache 1\n");

		kmb::Point3D node;
		kmb::ElementContainer::iterator eIter = elements->begin();
		while( eIter != elements->end() )
		{
			kmb::elementIdType eid = eIter.getId();
			kmb::BoundingBox bbox;
			evaluator.getBoundingBox( eIter, bbox );
			bbox.expand( safetyRatio );
			addElement( eid, &bbox, mesh, safetyRatio );
			++eIter;
		}
		REVOCAP_Debug("generateSearchCache 3\n");
	}
}

void
kmb::ElementOctree::createChildren(kmb::MeshData* mesh,double safetyRatio)
{
	REVOCAP_Debug_3("createChildren 0 %d\n",layer);
	if( children == NULL )
	{
		this->boundBox.getCenter(center);
		children = new kmb::ElementOctree*[8];
		for(int i=0;i<8;++i)
		{


			children[i] = new kmb::ElementOctree(layer+1,thres+thres/2);
		}
		REVOCAP_Debug_3("createChildren 1 %f %f %f\n",center.x(),center.y(),center.z());
		const kmb::Point3D maxPt = boundBox.getMax();
		const kmb::Point3D minPt = boundBox.getMin();
		children[7]->boundBox.update( maxPt.x(), maxPt.y(), maxPt.z() );
		children[7]->boundBox.update( center.x(), center.y(), center.z() );
		children[6]->boundBox.update( maxPt.x(), maxPt.y(), minPt.z() );
		children[6]->boundBox.update( center.x(), center.y(), center.z() );
		children[5]->boundBox.update( maxPt.x(), minPt.y(), maxPt.z() );
		children[5]->boundBox.update( center.x(), center.y(), center.z() );
		children[4]->boundBox.update( maxPt.x(), minPt.y(), minPt.z() );
		children[4]->boundBox.update( center.x(), center.y(), center.z() );
		children[3]->boundBox.update( minPt.x(), maxPt.y(), maxPt.z() );
		children[3]->boundBox.update( center.x(), center.y(), center.z() );
		children[2]->boundBox.update( minPt.x(), maxPt.y(), minPt.z() );
		children[2]->boundBox.update( center.x(), center.y(), center.z() );
		children[1]->boundBox.update( minPt.x(), minPt.y(), maxPt.z() );
		children[1]->boundBox.update( center.x(), center.y(), center.z() );
		children[0]->boundBox.update( minPt.x(), minPt.y(), minPt.z() );
		children[0]->boundBox.update( center.x(), center.y(), center.z() );
		REVOCAP_Debug_3("createChildren 2\n");

		kmb::Node node;
		for(int i=0;i<count;++i)
		{
			kmb::ElementContainer::const_iterator elem = mesh->findElement( this->elementIds[i] );
			if( !elem.isFinished() ){
				kmb::BoundingBox bbox;
				const int len = elem.getNodeCount();
				for(int j=0;j<len;++j)
				{
					if( mesh->getNode( elem.getCellId(j), node ) ){
						bbox.update( node );
					}
				}
				bbox.expand( safetyRatio );
				addElement( elementIds[i], &bbox, mesh, safetyRatio );
			}
		}
		REVOCAP_Debug_3("createChildren 3\n");
		this->count = -1;
		delete[] elementIds;

		elementIds = NULL;
	}
}

int
kmb::ElementOctree::getChildIndex(double x,double y,double z) const
{
	return
		( ( x > center.x() ) ? 4 : 0 ) +
		( ( y > center.y() ) ? 2 : 0 ) +
		( ( z > center.z() ) ? 1 : 0 ) ;
}

double
kmb::ElementOctree::getDistanceFromChildCell(double x,double y,double z,int i) const
{
	int index = getChildIndex(x,y,z) ^ i;
	switch( index )
	{
	case 0:
		return 0.0;
	case 1:
		return fabs(center.z() - z);
	case 2:
		return fabs(center.y() - y);
	case 3:
		return center.distance( center.x(), y, z );
	case 4:
		return fabs(center.x() - x);
	case 5:
		return center.distance( x, center.y(), z );
	case 6:
		return center.distance( x, y, center.z() );
	case 7:
		return center.distance( x, y, z );
	}
	return 0.0;
}

kmb::elementIdType
kmb::ElementOctree::searchElement(double x,double y,double z, const kmb::Point3DContainer* points, const kmb::ElementContainer* elements, double* coeff,double tolerance) const
{
	REVOCAP_Debug_3("kmb::ElementOctree::searchElement %f %f %f in %f\n",x,y,z,tolerance);
	kmb::elementIdType elemId = kmb::Element::nullElementId;

	if( points == NULL || elements == NULL )
	{
		REVOCAP_Debug("kmb::ElementOctree::searchElement NULL return\n");
		return elemId;
	}

	kmb::ElementEvaluator evaluator(points);
	if( children != NULL )
	{
		int childIndex = getChildIndex(x,y,z);

		if( elements->isUniqueDim(3) ){
			elemId = children[ childIndex ]->searchElement( x, y, z, points, elements, coeff, tolerance );
		}else{
			elemId = children[ childIndex ]->searchElement( x, y, z, points, elements, coeff, tolerance );
			if( elemId != kmb::Element::nullElementId ){
				tolerance = coeff[0];
			}
			for(int i=0;i<8;++i){
				if( i != childIndex && getDistanceFromChildCell(x,y,z,i) < tolerance ){
					kmb::elementIdType tmpElemId
						= children[i]->searchElement( x, y, z, points, elements, coeff, tolerance );

					if( tmpElemId != kmb::Element::nullElementId ){
						elemId = tmpElemId;
						tolerance = coeff[0];
					}
				}
			}
		}
	}
	else if( elementIds != NULL )
	{
		kmb::elementIdType candidateId = kmb::Element::nullElementId;
		kmb::Minimizer minimizer;
		kmb::Node pt(x,y,z),n0,n1,n2,n3;
		double toleranceSq = tolerance * tolerance;
		for(int i=0;i<count;++i )
		{
			kmb::ElementContainer::const_iterator element = elements->find( elementIds[i] );
			switch( element.getType() )
			{
			case kmb::SEGMENT:
			case kmb::SEGMENT2:
				if( points->getPoint( element.getCellId(0), n0 ) &&
					points->getPoint( element.getCellId(1), n1 ) )
				{
					double d = pt.distanceSqToSegment( n0, n1 );
					if( d < toleranceSq && minimizer.update( d ) ){
						elemId = elementIds[i];
						coeff[0] = sqrt( minimizer.getMin() );
						coeff[1] = pt.distance( n0 );
						coeff[2] = pt.distance( n1 );
					}
				}
				break;
			case kmb::TRIANGLE:
			case kmb::TRIANGLE2:




				if( points->getPoint( element.getCellId(0), n0 ) &&
					points->getPoint( element.getCellId(1), n1 ) &&
					points->getPoint( element.getCellId(2), n2 ) )
				{
					double d = pt.distanceSqToTriangle( n0, n1, n2 );
					if( d < toleranceSq && minimizer.update( d ) ){
						elemId = elementIds[i];
						coeff[0] = sqrt( minimizer.getMin() );
						coeff[1] = pt.distance( n0 );
						coeff[2] = pt.distance( n1 );
						coeff[3] = pt.distance( n2 );
					}
				}
				break;
			case kmb::QUAD:
			case kmb::QUAD2:
				if( points->getPoint( element.getCellId(0), n0 ) &&
					points->getPoint( element.getCellId(1), n1 ) &&
					points->getPoint( element.getCellId(2), n2 ) &&
					points->getPoint( element.getCellId(3), n3 ) )
				{
					double d012 = pt.distanceSqToTriangle( n0, n1, n2 );
					double d230 = pt.distanceSqToTriangle( n2, n3, n0 );
					double d = kmb::Minimizer::getMin( d012, d230 );
					if( d < toleranceSq && minimizer.update( d ) ){
						elemId = elementIds[i];
						coeff[0] = sqrt( minimizer.getMin() );
						coeff[1] = pt.distance( n0 );
						coeff[2] = pt.distance( n1 );
						coeff[3] = pt.distance( n2 );
						coeff[4] = pt.distance( n3 );
					}
				}
				break;
			case kmb::TETRAHEDRON:
			case kmb::TETRAHEDRON2:
				evaluator.getNaturalCoordinates( element, x, y, z, coeff );
				minimizer.initialize();
				minimizer.update( coeff[0] );
				minimizer.update( coeff[1] );
				minimizer.update( coeff[2] );
				minimizer.update( coeff[3] );
				if( minimizer.getMin() >= 0.0 ){
					elemId = elementIds[i];
					return elemId;
				}else if( tolerance < 0.0 && minimizer.getMin() >= tolerance ){
					candidateId = elementIds[i];
					tolerance = minimizer.getMin();
				}
				break;
			case kmb::HEXAHEDRON:
				evaluator.getNaturalCoordinates( element, x, y, z, coeff );
				minimizer.initialize();
				minimizer.update( 1 - fabs(coeff[0]) );
				minimizer.update( 1 - fabs(coeff[1]) );
				minimizer.update( 1 - fabs(coeff[2]) );
				if( minimizer.getMin() >= 0.0 ){
					elemId = elementIds[i];
					return elemId;
				}else if( tolerance < 0.0 && minimizer.getMin() >= tolerance ){
					candidateId = elementIds[i];
					tolerance = minimizer.getMin();
				}
				break;
			default:
				break;
			}
		}


		if( elemId == kmb::Element::nullElementId &&
			candidateId != kmb::Element::nullElementId )
		{
			elemId = candidateId;

			kmb::ElementContainer::const_iterator element = elements->find( elemId );
			evaluator.getNaturalCoordinates( element, x, y, z, coeff );
		}
	}
	else
	{
		REVOCAP_Debug("kmb::ElementOctree::searchElement NULL octree\n");
	}
	return elemId;
}

kmb::elementIdType
kmb::ElementOctree::searchElementWithNormal(double x,double y,double z, const Vector3D &normal, const kmb::Point3DContainer* points, const kmb::ElementContainer* elements, double* coeff,double tolerance) const
{
	kmb::elementIdType elemId = kmb::Element::nullElementId;

	if( points == NULL || elements == NULL || !elements->isUniqueDim(2))
	{
		return elemId;
	}

	if( children != NULL )
	{
		int childIndex = getChildIndex(x,y,z);

		elemId = children[ childIndex ]->searchElementWithNormal( x, y, z, normal, points, elements, coeff, tolerance );
		if( elemId != kmb::Element::nullElementId ){
			tolerance = coeff[0];
		}
		for(int i=0;i<8;++i){
			if( i != childIndex && getDistanceFromChildCell(x,y,z,i) < tolerance ){
				kmb::elementIdType tmpElemId
					= children[i]->searchElementWithNormal( x, y, z, normal, points, elements, coeff, tolerance );

				if( tmpElemId != kmb::Element::nullElementId ){
					elemId = tmpElemId;
					tolerance = coeff[0];
				}
			}
		}
	}
	else if( elementIds != NULL )
	{
		kmb::ElementEvaluator evaluator(points);
		kmb::Vector3D vect;
		kmb::Minimizer minimizer;
		kmb::Node pt(x,y,z),n0,n1,n2,n3;
		double toleranceSq = tolerance * tolerance;
		for(int i=0;i<count;++i )
		{
			kmb::ElementContainer::const_iterator element = elements->find( elementIds[i] );

			if( !element.isFinished() && evaluator.getNormalVector( element, vect ) && vect*normal > 0 ){
				switch( element.getType() )
				{
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					if( points->getPoint( element.getCellId(0), n0 ) &&
						points->getPoint( element.getCellId(1), n1 ) &&
						points->getPoint( element.getCellId(2), n2 ) )
					{
						double d = pt.distanceSqToTriangle( n0, n1, n2 );
						if( d < toleranceSq && minimizer.update( d ) ){
							elemId = elementIds[i];
							coeff[0] = sqrt( minimizer.getMin() );
							coeff[1] = pt.distance( n0 );
							coeff[2] = pt.distance( n1 );
							coeff[3] = pt.distance( n2 );
						}
					}
					break;
				case kmb::QUAD:
				case kmb::QUAD2:
					if( points->getPoint( element.getCellId(0), n0 ) &&
						points->getPoint( element.getCellId(1), n1 ) &&
						points->getPoint( element.getCellId(1), n2 ) &&
						points->getPoint( element.getCellId(2), n3 ) )
					{
						double d = kmb::Minimizer::getMin(
								pt.distanceSqToTriangle( n0, n1, n2 ),
								pt.distanceSqToTriangle( n2, n3, n0 ) );
						if( d < toleranceSq && minimizer.update( d ) ){
							elemId = elementIds[i];
							coeff[0] = sqrt( minimizer.getMin() );
							coeff[1] = pt.distance( n0 );
							coeff[2] = pt.distance( n1 );
							coeff[3] = pt.distance( n2 );
							coeff[4] = pt.distance( n3 );
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}
	return elemId;
}
