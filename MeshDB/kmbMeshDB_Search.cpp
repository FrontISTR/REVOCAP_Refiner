/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : MeshDB                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbHexahedron.h"
#include "Geometry/kmb_Calculator.h"

#include <cmath>

size_t
kmb::MeshDB::generateElementSearchCache( kmb::bodyIdType bodyId )
{
	kmb::Body* body = this->getBodyPtr( bodyId );
	if( body != NULL )
	{

		this->elementOctree.generateSearchCache(this,bodyId);
	}
	return this->elementOctree.getCount();
}

void
kmb::MeshDB::clearElementSearchCache(void)
{
	this->elementOctree.clear();
}




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
				coeff[0] = 0.0;	coeff[1] = 0.0;	coeff[2] = 0.0;
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
		case kmb::DataBindings::ELEMENTGROUP:
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
		case kmb::DataBindings::FACEGROUP:
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

size_t
kmb::MeshDB::generateNodeSearchCache(void)
{
	this->nodeOctree.generateSearchCache( this->node3Ds );
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
		this->nodeOctree.generateSearchCache( this->node3Ds, nodeSet );
	}
	return this->nodeOctree.getCount();
}

size_t
kmb::MeshDB::generateNodeSearchCacheOfData( const char* name, const char* stype )
{
	std::set< kmb::nodeIdType > nodeSet;
	this->getNodeSetFromDataBindings( nodeSet, name, stype );
	this->nodeOctree.generateSearchCache( this->node3Ds, nodeSet );
	return this->nodeOctree.getCount();
}

void
kmb::MeshDB::appendNodeSearchCache(kmb::nodeIdType nodeId)
{
	this->nodeOctree.appendSearchCache( this->node3Ds, nodeId );
}

void
kmb::MeshDB::clearNodeSearchCache(void)
{
	this->nodeOctree.clear();
}

double
kmb::MeshDB::getNearestNode(double x,double y,double z,kmb::nodeIdType &nearestId)
{
	if( this->nodeOctree.getCount() != 0 ){
		return this->nodeOctree.getNearestPoint(x,y,z,this->node3Ds,nearestId);
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
kmb::MeshDB::getNodesInRegion(kmb::Region* region,std::set<kmb::nodeIdType>& selectedNodes)
{
	if( this->node3Ds && this->nodeOctree.getCount() != 0 ){
		this->nodeOctree.getNodesInRegion(region,this->node3Ds,selectedNodes);
	}else if( this->node3Ds && region ){
		kmb::Point3D pt;
		kmb::Point3DContainer::iterator nIter = node3Ds->begin();
		while( nIter != node3Ds->end() )
		{
			if( nIter.getPoint(pt) && region->intersect(pt) != kmb::Region::OUTSIDE ){
				selectedNodes.insert( nIter.getId() );
			}
			++nIter;
		}
	}
}
