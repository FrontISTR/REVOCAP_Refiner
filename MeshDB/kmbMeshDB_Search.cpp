/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshDB                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/


#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementOctree.h"
#include "MeshDB/kmbElementBucket.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbHexahedron.h"
#include "Common/kmbCalculator.h"

#include <cmath>
#include <cstring>

size_t
kmb::MeshDB::generateElementSearchCache( kmb::bodyIdType bodyId, const char* cacheType )
{
	kmb::Body* body = this->getBodyPtr( bodyId );
	kmb::Point3DContainer* points = this->getNodes();
	if( body != NULL && points != NULL )
	{
		if( strcmp("octree",cacheType) == 0 ){

			this->elementOctree.setContainer( points, body );
			this->elementOctree.appendAll();
			return this->elementOctree.getCount();
		}else if( strcmp("bucket",cacheType) == 0 ){

			this->elementBucket.setContainer( points, body );
			kmb::BoundingBox bbox = this->getBoundingBox();
			bbox.expandDiameter(1.2);
			this->elementBucket.setRegionGrid( bbox, this->getElementCount(bodyId) );
			this->elementBucket.appendAll();
			return this->elementBucket.getCount();
		}
	}
	return 0;
}

void
kmb::MeshDB::clearElementSearchCache(void)
{
	this->elementOctree.clear();
	this->elementBucket.clear();
}

kmb::elementIdType
kmb::MeshDB::searchElement(kmb::bodyIdType bodyId,double x,double y,double z,double tolerance) const
{
	kmb::elementIdType findElementId = kmb::Element::nullElementId;
	const kmb::Body* body = this->getBodyPtr( bodyId );
	if( body == NULL || node3Ds == NULL || evaluator == NULL ){
		return kmb::Element::nullElementId;
	}

	if( this->elementBucket.getCount() > 0 ){

		findElementId = this->elementBucket.searchElementInBody(body,x,y,z,tolerance);
		return findElementId;
	}else if( this->elementOctree.getCount() > 0 ){

		double dist = this->elementOctree.getNearest(x,y,z,findElementId);
		if( dist <= tolerance ){
			return findElementId;
		}else{
			return kmb::Element::nullElementId;
		}
	}else{

		kmb::Minimizer minimizer;
		kmb::Node n0,n1,n2,n3;
		kmb::Point3D pt(x,y,z);

		if( body->isUniqueDim(1) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				kmb::elementIdType eid = eIter.getId();
				if( this->getNode( eIter.getCellId(0), n0 ) &&
					this->getNode( eIter.getCellId(1), n1 ) )
				{
					double d = pt.distanceSqToSegment( n0, n1 );
					if( minimizer.update( d ) ){
						findElementId = eid;
					}
				}
				++eIter;
			}
			if( minimizer.getMin() <= tolerance*tolerance ){
				return findElementId;
			}else{
				return kmb::Element::nullElementId;
			}
		}else if( body->isUniqueDim(2) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				kmb::elementIdType eid = eIter.getId();
				switch( eIter.getType() )
				{
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					if( this->getNode( eIter.getCellId(0), n0 ) &&
						this->getNode( eIter.getCellId(1), n1 ) &&
						this->getNode( eIter.getCellId(2), n2 ) )
					{
						double d = pt.distanceSqToTriangle( n0, n1, n2 );
						if( minimizer.update( d ) ){
							findElementId = eid;
						}
					}
					break;
				case QUAD:
				case QUAD2:
					if( this->getNode( eIter.getCellId(0), n0 ) &&
						this->getNode( eIter.getCellId(1), n1 ) &&
						this->getNode( eIter.getCellId(2), n2 ) &&
						this->getNode( eIter.getCellId(3), n3 ) )
					{
						double d = kmb::Minimizer::getMin(
								pt.distanceSqToTriangle( n0, n1, n2 ),
								pt.distanceSqToTriangle( n2, n3, n0 ) );
						if( minimizer.update( d ) ){
							findElementId = eid;
						}
					}
					break;
				default:
					break;
				}
				++eIter;
			}
			if( minimizer.getMin() <= tolerance*tolerance ){
				return findElementId;
			}else{
				return kmb::Element::nullElementId;
			}
		}else if( body->isUniqueDim(3) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){


				double dist = 0.0;
				if( evaluator->getMinInnerVolume(eIter,x,y,z) >= 0.0 ){
					dist = 0.0;
				}else{
					dist = evaluator->getDistanceSqOnBoundary(eIter,x,y,z);
				}
				if( minimizer.update( dist ) ){
					findElementId = eIter.getId();
				}
				++eIter;
			}
			if( minimizer.getMin() <= tolerance*tolerance ){
				return findElementId;
			}else{
				return kmb::Element::nullElementId;
			}
		}
		return findElementId;
	}
}

kmb::elementIdType
kmb::MeshDB::searchElementInData(const char* name,double x,double y,double z,double tolerance) const
{
	kmb::elementIdType findElementId = kmb::Element::nullElementId;
	const kmb::DataBindings* data = this->getDataBindingsPtr(name);
	if( data == NULL ||
		!(data->getBindingMode() == kmb::DataBindings::ElementGroup ||
		data->getBindingMode() == kmb::DataBindings::FaceGroup ) )
	{
		return findElementId;
	}
	kmb::bodyIdType bodyId = data->getTargetBodyId();
	const kmb::Body* body = this->getBodyPtr( bodyId );
	if( body == NULL || node3Ds == NULL || evaluator == NULL ){
		return kmb::Element::nullElementId;
	}
	if( this->elementBucket.getCount() > 0 ){

		findElementId = this->elementBucket.searchElementInData(data,body,x,y,z,tolerance);
	}else if( data->getBindingMode() == kmb::DataBindings::ElementGroup ){

		kmb::Maximizer maximizer;
		kmb::Minimizer minimizer;
		kmb::Node n0,n1,n2,n3;
		kmb::Point3D pt(x,y,z);

		if( body->isUniqueDim(1) ){
			kmb::DataBindings::const_iterator dIter = data->begin();
			while( !dIter.isFinished() ){
				kmb::elementIdType eid = dIter.getId();
				kmb::ElementContainer::const_iterator eIter = body->find( eid );
				if( !eIter.isFinished() ){
					if( this->getNode( eIter.getCellId(0), n0 ) &&
						this->getNode( eIter.getCellId(1), n1 ) )
					{
						double d = pt.distanceSqToSegment( n0, n1 );
						if( minimizer.update( d ) ){
							findElementId = eid;
						}
					}
				}

				++dIter;
			}
		}else if( body->isUniqueDim(2) ){
			kmb::DataBindings::const_iterator dIter = data->begin();
			while( !dIter.isFinished() ){
				kmb::elementIdType eid = dIter.getId();
				kmb::ElementContainer::const_iterator eIter = body->find( eid );
				if( !eIter.isFinished() ){
					switch( eIter.getType() )
					{
					case kmb::TRIANGLE:
					case kmb::TRIANGLE2:
						if( this->getNode( eIter.getCellId(0), n0 ) &&
							this->getNode( eIter.getCellId(1), n1 ) &&
							this->getNode( eIter.getCellId(2), n2 ) )
						{
							double d = pt.distanceSqToTriangle( n0, n1, n2 );
							if( minimizer.update( d ) ){
								findElementId = eid;
							}
						}
						break;
					case QUAD:
					case QUAD2:
						if( this->getNode( eIter.getCellId(0), n0 ) &&
							this->getNode( eIter.getCellId(1), n1 ) &&
							this->getNode( eIter.getCellId(2), n2 ) &&
							this->getNode( eIter.getCellId(3), n3 ) )
						{
							double d = kmb::Minimizer::getMin(
									pt.distanceSqToTriangle( n0, n1, n2 ),
									pt.distanceSqToTriangle( n2, n3, n0 ) );
							if( minimizer.update( d ) ){
								findElementId = eid;
							}
						}
						break;
					default:
						break;
					}
				}
				++dIter;
			}
		}else if( body->isUniqueDim(3) ){
			kmb::DataBindings::const_iterator dIter = data->begin();
			while( !dIter.isFinished() ){
				kmb::elementIdType eid = dIter.getId();
				kmb::ElementContainer::const_iterator eIter = body->find( eid );
				if( !eIter.isFinished() ){

					if( maximizer.update( evaluator->getMinInnerVolume( eIter, x, y, z ) ) ){
						findElementId = eIter.getId();
					}
				}
				++dIter;
			}
			if( maximizer.getMax() < tolerance ){
				findElementId = kmb::Element::nullElementId;
			}
		}
	}else if( data->getBindingMode() == kmb::DataBindings::FaceGroup ){

		kmb::Minimizer minimizer;
		kmb::Node n0,n1,n2,n3;
		kmb::Point3D pt(x,y,z);
		kmb::Face f;

		if( body->isUniqueDim(1) ){
			kmb::DataBindings::const_iterator dIter = data->begin();
			while( !dIter.isFinished() ){
				dIter.getFace(f);
				kmb::elementIdType eid = f.getElementId();
				kmb::idType localId = f.getLocalFaceId();
				kmb::ElementContainer::const_iterator eIter = body->find( eid );
				if( !eIter.isFinished() ){
					if( this->getNode( eIter.getBoundaryCellId(localId,0), n0 ) )
					{
						double d = pt.distanceSq( n0 );
						if( minimizer.update( d ) ){
							findElementId = eid;
						}
					}
				}
				++dIter;
			}
		}else if( body->isUniqueDim(2) ){
			kmb::DataBindings::const_iterator dIter = data->begin();
			while( !dIter.isFinished() ){
				dIter.getFace(f);
				kmb::elementIdType eid = f.getElementId();
				kmb::idType localId = f.getLocalFaceId();
				kmb::ElementContainer::const_iterator eIter = body->find( eid );
				if( !eIter.isFinished() ){
					if( this->getNode( eIter.getBoundaryCellId(localId,0), n0 ) &&
						this->getNode( eIter.getBoundaryCellId(localId,1), n1 ) )
					{
						double d = pt.distanceSqToSegment( n0, n1 );
						if( minimizer.update( d ) ){
							findElementId = eid;
						}
					}
				}
				++dIter;
			}
		}else if( body->isUniqueDim(3) ){
			double innerVolume = 0.0;
			kmb::DataBindings::const_iterator dIter = data->begin();
			while( !dIter.isFinished() ){
				dIter.getFace(f);
				kmb::elementIdType eid = f.getElementId();
				kmb::idType localId = f.getLocalFaceId();
				kmb::ElementContainer::const_iterator eIter = body->find( eid );
				if( !eIter.isFinished() ){
					switch( eIter.getBoundaryType(localId) )
					{
					case kmb::TRIANGLE:
					case kmb::TRIANGLE2:
						if( this->getNode( eIter.getBoundaryCellId(localId,0), n0 ) &&
							this->getNode( eIter.getBoundaryCellId(localId,1), n1 ) &&
							this->getNode( eIter.getBoundaryCellId(localId,2), n2 ) )
						{
							double d = pt.distanceSqToTriangle( n0, n1, n2 );
							if( minimizer.update( d ) ){
								findElementId = eid;
								innerVolume = evaluator->getMinInnerVolume( eIter, x, y, z );
							}
						}
						break;
					case QUAD:
					case QUAD2:
						if( this->getNode( eIter.getBoundaryCellId(localId,0), n0 ) &&
							this->getNode( eIter.getBoundaryCellId(localId,1), n1 ) &&
							this->getNode( eIter.getBoundaryCellId(localId,2), n2 ) &&
							this->getNode( eIter.getBoundaryCellId(localId,3), n3 ) )
						{
							double d = kmb::Minimizer::getMin(
									pt.distanceSqToTriangle( n0, n1, n2 ),
									pt.distanceSqToTriangle( n2, n3, n0 ) );
							if( minimizer.update( d ) ){
								findElementId = eid;
								innerVolume = evaluator->getMinInnerVolume( eIter, x, y, z );
							}
						}
						break;
					default:
						break;
					}
				}
				++dIter;
			}
			if( innerVolume < tolerance ){
				findElementId = kmb::Element::nullElementId;
			}
		}
	}
	return findElementId;
}




/*
kmb::elementIdType
kmb::MeshDB::searchElement(kmb::bodyIdType bodyId,double x,double y,double z,double* maxCoeff,double tolerance) const
{
	kmb::elementIdType findElementId = kmb::Element::nullElementId;

	kmb::Maximizer maximizer;
	kmb::Minimizer minimizer;
	double coeff[5] = {-1.0, -1.0, -1.0, -1.0, -1.0};
	kmb::Node n0,n1,n2,n3;
	kmb::Point3D pt(x,y,z);
	double toleranceSq = tolerance * tolerance;

	const kmb::Body* body = this->getBodyPtr( bodyId );
	if( body != NULL && node3Ds != NULL ){
		findElementId = this->elementOctree.searchElement(x,y,z,this->node3Ds,body,coeff,tolerance);
		if( findElementId != kmb::Element::nullElementId ){
			for(int i=0;i<5;++i){
				maxCoeff[i] = coeff[i];
			}
		}else if( body->isUniqueDim(1) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() )
			{
				kmb::elementIdType eid = eIter.getId();
				if( this->getNode( eIter.getCellId(0), n0 ) &&
					this->getNode( eIter.getCellId(1), n1 ) )
				{
					double d = pt.distanceSqToSegment( n0, n1 );
					if( d < toleranceSq && minimizer.update( d ) ){
						findElementId = eid;
						maxCoeff[0] = sqrt( minimizer.getMin() );
						maxCoeff[1] = pt.distance( n0 );
						maxCoeff[2] = pt.distance( n1 );
					}
				}
				++eIter;
			}
		}else if( body->isUniqueDim(2) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() )
			{
				kmb::elementIdType eid = eIter.getId();
				switch( eIter.getType() )
				{
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					if( this->getNode( eIter.getCellId(0), n0 ) &&
						this->getNode( eIter.getCellId(1), n1 ) &&
						this->getNode( eIter.getCellId(2), n2 ) )
					{
						double d = pt.distanceSqToTriangle( n0, n1, n2 );
						if( d < toleranceSq && minimizer.update( d ) ){
							findElementId = eid;
							maxCoeff[0] = sqrt( minimizer.getMin() );
							maxCoeff[1] = pt.distance( n0 );
							maxCoeff[2] = pt.distance( n1 );
							maxCoeff[3] = pt.distance( n2 );
						}
					}
					break;
				case QUAD:
				case QUAD2:
					if( this->getNode( eIter.getCellId(0), n0 ) &&
						this->getNode( eIter.getCellId(1), n1 ) &&
						this->getNode( eIter.getCellId(2), n2 ) &&
						this->getNode( eIter.getCellId(3), n3 ) )
					{
						double d = kmb::Minimizer::getMin(
								pt.distanceSqToTriangle( n0, n1, n2 ),
								pt.distanceSqToTriangle( n2, n3, n0 ) );
						if( d < toleranceSq && minimizer.update( d ) ){
							findElementId = eid;
							maxCoeff[0] = sqrt( minimizer.getMin() );
							maxCoeff[1] = pt.distance( n0 );
							maxCoeff[2] = pt.distance( n1 );
							maxCoeff[3] = pt.distance( n2 );
							maxCoeff[4] = pt.distance( n3 );
						}
					}
					break;
				default:
					break;
				}
				++eIter;
			}
		}else if( body->isUniqueType( kmb::TETRAHEDRON ) || body->isUniqueType( kmb::TETRAHEDRON2 ) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			double r[4];
			while( !eIter.isFinished() )
			{
				kmb::elementIdType eid = eIter.getId();
				if( evaluator && evaluator->getNaturalCoordinates( eIter, x, y, z, coeff) ){
					kmb::Tetrahedron::shapeFunction( coeff[0], coeff[1], coeff[2], r );
					minimizer.initialize();
					minimizer.update( r[0] );
					minimizer.update( r[1] );
					minimizer.update( r[2] );
					minimizer.update( r[3] );
					if( maximizer.update( minimizer.getMin() ) ){
						findElementId = eid;
						for(int i=0;i<3;++i){
							maxCoeff[i] = coeff[i];
						}
						if( maximizer.getMax() >= 0.0 ){
							goto findElement;
						}
					}
				}
				++eIter;
			}

			if( maximizer.getMax() <= tolerance ){
				findElementId = kmb::Element::nullElementId;
			}
		}else if( body->isUniqueType( kmb::HEXAHEDRON ) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() )
			{
				kmb::elementIdType eid = eIter.getId();
				coeff[0] = 0.0; coeff[1] = 0.0; coeff[2] = 0.0;
				if( evaluator && evaluator->getNaturalCoordinates( eIter, x, y, z, coeff, 1.0-tolerance ) ){
					minimizer.initialize();
					minimizer.update( 1 - fabs(coeff[0]) );
					minimizer.update( 1 - fabs(coeff[1]) );
					minimizer.update( 1 - fabs(coeff[2]) );
					if( maximizer.update( minimizer.getMin() ) ){
						findElementId = eid;
						for(int i=0;i<3;++i){
							maxCoeff[i] = coeff[i];
						}
						if( maximizer.getMax() >= 0.0 ){
							goto findElement;
						}
					}
				}
				++eIter;
			}

			if( maximizer.getMax() <= tolerance ){
				findElementId = kmb::Element::nullElementId;
			}
		}
	}
findElement:
	return findElementId;
}
*/
/*
kmb::elementIdType
kmb::MeshDB::searchElementWithNormal(kmb::bodyIdType bodyId,double x,double y,double z,kmb::Vector3D &normal,double* maxCoeff,double tolerance) const
{
	kmb::elementIdType findElementId = kmb::Element::nullElementId;

	kmb::Maximizer maximizer;
	kmb::Minimizer minimizer;
	double coeff[5] = {-1.0, -1.0, -1.0, -1.0, -1.0};
	kmb::Node n0,n1,n2,n3;
	kmb::Point3D pt(x,y,z);
	double toleranceSq = tolerance * tolerance;
	kmb::Vector3D vect;

	const kmb::Body* body = this->getBodyPtr( bodyId );
	if( body != NULL && node3Ds != NULL && body->isUniqueDim(2) && this->evaluator ){
		findElementId = this->elementOctree.searchElementWithNormal(x,y,z,normal,this->node3Ds,body,coeff,tolerance);
		if( findElementId != kmb::Element::nullElementId ){
			for(int i=0;i<5;++i){
				maxCoeff[i] = coeff[i];
			}
		}else if( body->isUniqueType( kmb::TRIANGLE ) || body->isUniqueType( kmb::TRIANGLE2 ) ){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() )
			{
				kmb::elementIdType eid = eIter.getId();
				if( evaluator->getNormalVector( eIter, vect ) &&
					vect*normal > 0 &&
					this->getNode( eIter.getCellId(0), n0 ) &&
					this->getNode( eIter.getCellId(1), n1 ) &&
					this->getNode( eIter.getCellId(2), n2 ) )
				{
					double d = pt.distanceSqToTriangle( n0, n1, n2 );
					if( d < toleranceSq && minimizer.update( d ) ){
						findElementId = eid;
						maxCoeff[0] = sqrt( minimizer.getMin() );
						maxCoeff[1] = pt.distance( n0 );
						maxCoeff[2] = pt.distance( n1 );
						maxCoeff[3] = pt.distance( n2 );
					}
				}
				++eIter;
			}



		}else if( body->isUniqueType( kmb::QUAD ) || body->isUniqueType( kmb::QUAD2 )){
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() )
			{
				kmb::elementIdType eid = eIter.getId();
				if( evaluator->getNormalVector( eIter, vect ) &&
					vect*normal > 0 &&
					this->getNode( eIter.getCellId(0), n0 ) &&
					this->getNode( eIter.getCellId(1), n1 ) &&
					this->getNode( eIter.getCellId(1), n2 ) &&
					this->getNode( eIter.getCellId(2), n3 ) )
				{
					double d = kmb::Minimizer::getMin(
							pt.distanceSqToTriangle( n0, n1, n2 ),
							pt.distanceSqToTriangle( n2, n3, n0 ) );
					if( d < toleranceSq && minimizer.update( d ) ){
						findElementId = eid;
						maxCoeff[0] = sqrt( minimizer.getMin() );
						maxCoeff[1] = pt.distance( n0 );
						maxCoeff[2] = pt.distance( n1 );
						maxCoeff[3] = pt.distance( n2 );
						maxCoeff[4] = pt.distance( n3 );
					}
				}
				++eIter;
			}
		}
	}
	return findElementId;
}
*/
/*
kmb::elementIdType
kmb::MeshDB::searchElementInData(const char* name,double x,double y,double z,double* maxCoeff,double tolerance,const char* stype) const
{
	kmb::elementIdType findElementId = kmb::Element::nullElementId;

	kmb::Maximizer maximizer;
	kmb::Minimizer minimizer;
	double coeff[4] = {0.0, 0.0, 0.0, 0.0};

	const kmb::DataBindings* group = this->getDataBindingsPtr(name,stype);
	if( group != NULL )
	{
		switch( group->getBindingMode() ){
		case kmb::DataBindings::ElementGroup:
		{
			kmb::bodyIdType bodyId = group->getTargetBodyId();
			kmb::DataBindings::const_iterator eIter = group->begin();
			while( !eIter.isFinished() ){
				kmb::ElementContainer::const_iterator elem = this->findElement( static_cast< kmb::elementIdType >(eIter.getId()), bodyId );
				if( !elem.isFinished() ){
					switch( elem.getType() )
					{
					case kmb::TETRAHEDRON:
					case kmb::TETRAHEDRON2:
						{
							evaluator->getNaturalCoordinates( elem, x, y, z, coeff );
							minimizer.initialize();
							for(int i=0;i<4;++i){
								minimizer.update( coeff[i] );
							}
							if( maximizer.update( minimizer.getMin() ) ){
								findElementId = static_cast< kmb::elementIdType >(eIter.getId());
								for(int i=0;i<4;++i){
									maxCoeff[i] = coeff[i];
								}
								if( maximizer.getMax() >= 0.0 ){
									goto findElement;
								}
							}
						}
						break;
					case kmb::HEXAHEDRON:
						{
							coeff[0] = 0.0;	coeff[1] = 0.0;	coeff[2] = 0.0;
							if( evaluator->getNaturalCoordinates( elem, x, y, z, coeff, 1.0-tolerance) ){
								minimizer.initialize();
								for(int i=0;i<3;++i){
									minimizer.update( 1- fabs(coeff[i]) );
								}
								if( maximizer.update( minimizer.getMin() ) ){
									findElementId = static_cast< kmb::elementIdType >(eIter.getId());
									for(int i=0;i<3;++i){
										maxCoeff[i] = coeff[i];
									}
									if( maximizer.getMax() >= 0.0 ){
										goto findElement;
									}
								}
							}
						}
						break;
					default:
						break;
					}
				}
				++eIter;
			}
			break;
		}
		case kmb::DataBindings::FaceGroup:
		{
			kmb::bodyIdType bodyId = group->getTargetBodyId();
			kmb::DataBindings::const_iterator fIter = group->begin();
			kmb::Face f;
			while( !fIter.isFinished() ){
				if( fIter.getFace( f ) ){
					kmb::ElementContainer::const_iterator elem = this->findElement(f.getElementId(),bodyId);
					if( !elem.isFinished() ){
						switch( elem.getType() )
						{
						case kmb::TETRAHEDRON:
						case kmb::TETRAHEDRON2:
							{
								evaluator->getNaturalCoordinates( elem, x, y, z, coeff );
								minimizer.initialize();
								for(int i=0;i<4;++i){
									minimizer.update( coeff[i] );
								}
								if( maximizer.update( minimizer.getMin() ) ){
									findElementId = f.getElementId();
									for(int i=0;i<4;++i){
										maxCoeff[i] = coeff[i];
									}
									if( maximizer.getMax() >= 0.0 ){
										goto findElement;
									}
								}
							}
							break;
						case kmb::HEXAHEDRON:
							{
								coeff[0] = 0.0;	coeff[1] = 0.0;	coeff[2] = 0.0;
								if( evaluator->getNaturalCoordinates( elem, x, y, z, coeff, 1.0-tolerance ) ){
									minimizer.initialize();
									for(int i=0;i<3;++i){
										minimizer.update( 1- fabs(coeff[i]) );
									}
									if( maximizer.update( minimizer.getMin() ) ){
										findElementId = f.getElementId();
										for(int i=0;i<3;++i){
											maxCoeff[i] = coeff[i];
										}
										if( maximizer.getMax() >= 0.0 ){
											goto findElement;
										}
									}
								}
							}
							break;
						default:
							break;
						}
					}
				}
				++fIter;
			}
			break;
		}
		default:
			break;
		}
	}
	if( maximizer.getMax() <= tolerance ){
		findElementId = kmb::Element::nullElementId;
	}
findElement:
	return findElementId;
}
*/

size_t
kmb::MeshDB::generateNodeSearchCache(void)
{
	this->nodeOctree.setContainer( this->node3Ds );
	this->nodeOctree.setRegion( this->getBoundingBox() );
	this->nodeOctree.appendAll();
	return this->nodeOctree.getCount();
}

size_t
kmb::MeshDB::generateNodeSearchCacheOfBody( kmb::bodyIdType bodyId )
{
	kmb::Body* body = this->getBodyPtr( bodyId );
	if( body != NULL )
	{
		std::set< kmb::nodeIdType > nodeSet;
		body->getNodesOfBody( nodeSet );
		this->nodeOctree.setContainer( this->node3Ds );
		std::set< kmb::nodeIdType >::iterator nIter = nodeSet.begin();
		while( nIter != nodeSet.end() ){
			kmb::nodeIdType nodeId = *nIter;
			this->nodeOctree.append( nodeId );
			++nIter;
		}
	}
	return this->nodeOctree.getCount();
}

size_t
kmb::MeshDB::generateNodeSearchCacheOfData( const char* name, const char* stype )
{
	std::set< kmb::nodeIdType > nodeSet;
	this->getNodeSetFromDataBindings( nodeSet, name, stype );
	this->nodeOctree.setContainer( this->node3Ds );
	std::set< kmb::nodeIdType >::iterator nIter = nodeSet.begin();
	while( nIter != nodeSet.end() ){
		kmb::nodeIdType nodeId = *nIter;
		this->nodeOctree.append( nodeId );
		++nIter;
	}
	return this->nodeOctree.getCount();
}

void
kmb::MeshDB::appendNodeSearchCache(kmb::nodeIdType nodeId)
{
	this->nodeOctree.append( nodeId );
}

void
kmb::MeshDB::clearNodeSearchCache(void)
{
	this->nodeOctree.clear();
}

double
kmb::MeshDB::getNearestNode(double x,double y,double z,kmb::nodeIdType &nearestId) const
{
	if( this->nodeOctree.getCount() != 0 ){
		return this->nodeOctree.getNearest(x,y,z,nearestId);
	}else if( this->node3Ds ){
		kmb::Point3D result;
		return this->node3Ds->getNearest(x,y,z,result,nearestId);
	}else{
		return DBL_MAX;
	}
}


/*
double
kmb::MeshDB::getNearestElement(kmb::bodyIdType bodyId,double x,double y,double z,kmb::elementIdType &nearestId)
{
	kmb::Point3D pt(x,y,z);
	kmb::Body* body = this->getBodyPtr( bodyId );
	kmb::Minimizer min;
	kmb::Point3D n0,n1,n2;
	if( body != NULL )
	{
		if( body->isUniqueType( kmb::TRIANGLE ) ){
			kmb::ElementContainer::iterator eIter = body->begin();
			while( eIter != body->end() ){
				kmb::Element* triangle = eIter.getElement();
				if( triangle ){
					if( this->getNode( triangle->getCellId(0), n0 ) &&
						this->getNode( triangle->getCellId(1), n1 ) &&
						this->getNode( triangle->getCellId(2), n2 ) &&
						min.update( pt.distanceSqToTriangle( n0, n1, n2 ) ) ){
						nearestId = eIter.getId();
					}
				}
				++eIter;
			}
		}
		if( body->isUniqueType( kmb::SEGMENT ) ){
			kmb::ElementContainer::iterator eIter = body->begin();
			while( eIter != body->end() ){
				kmb::Element* segment = eIter.getElement();
				if( segment ){
					if( this->getNode( segment->getCellId(0), n0 ) &&
						this->getNode( segment->getCellId(1), n1 ) &&
						min.update( pt.distanceSqToSegment( n0, n1 ) ) ){
						nearestId = eIter.getId();
					}
				}
				++eIter;
			}
		}
	}
	return sqrt( min.getMin() );
}
*/

void
kmb::MeshDB::getNodesInRegion(const kmb::Region* region,std::set<kmb::nodeIdType>& selectedNodes) const
{
	if( this->node3Ds && this->nodeOctree.getCount() != 0 ){
		this->nodeOctree.getNodesInRegion(region,selectedNodes);
	}else if( this->node3Ds && region ){
		kmb::Point3D pt;
		kmb::Point3DContainer::const_iterator nIter = node3Ds->begin();
		while( nIter != node3Ds->end() )
		{
			if( nIter.getPoint(pt) && region->intersect(pt) != kmb::Region::OUTSIDE ){
				selectedNodes.insert( nIter.getId() );
			}
			++nIter;
		}
	}
}
