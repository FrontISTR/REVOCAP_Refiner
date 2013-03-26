/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeEvaluator                                           #
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
#include "MeshDB/kmbNodeEvaluator.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbDataBindings.h"

kmb::NodeEvaluator::NodeEvaluator(void)
: mesh(NULL)
{
}

kmb::NodeEvaluator::~NodeEvaluator(void)
{
}

void
kmb::NodeEvaluator::setMesh(kmb::MeshData* mesh)
{
	this->mesh = mesh;
}

int
kmb::NodeEvaluator::calcCurvature(const char* name,const char* stype)
{
	if( mesh == NULL ){
		return -1;
	}
	kmb::DataBindings* data = mesh->getDataBindingsPtr(name,stype);
	if( data == NULL ||
		data->getBindingMode() != kmb::DataBindings::NodeVariable ||
		data->getValueType() != kmb::PhysicalValue::Scalar )
	{
		return -1;
	}
	kmb::Point3DContainer* points = mesh->getNodes();
	if( points == NULL ){
		return -1;
	}
	double angle(0.0);
	kmb::bodyIdType bCount = mesh->getBodyCount();
	for(kmb::bodyIdType bodyId=0;bodyId<bCount;++bodyId){
		kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
		if( body == NULL || body->getDimension() != 2 )
		{
			continue;
		}
		kmb::ElementContainer::iterator eIter = body->begin();
		kmb::Node n0,n1,n2,n3;
		while( !eIter.isFinished() ){
			switch( eIter.getType() ){
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
					if( points->getPoint( eIter[0], n0 ) &&
						points->getPoint( eIter[1], n1 ) &&
						points->getPoint( eIter[2], n2 ) )
					{
						if( data->getPhysicalValue( eIter[0], &angle ) ){
							angle -= kmb::Point3D::angle(n2,n0,n1);
						}else{
							angle = -kmb::Point3D::angle(n2,n0,n1);
						}
						data->setPhysicalValue( eIter[0], &angle );

						if( data->getPhysicalValue( eIter[1], &angle ) ){
							angle -= kmb::Point3D::angle(n0,n1,n2);
						}else{
							angle = -kmb::Point3D::angle(n0,n1,n2);
						}
						data->setPhysicalValue( eIter[1], &angle );

						if( data->getPhysicalValue( eIter[2], &angle ) ){
							angle -= kmb::Point3D::angle(n1,n2,n0);
						}else{
							angle = -kmb::Point3D::angle(n1,n2,n0);
						}
						data->setPhysicalValue( eIter[2], &angle );
					}
					break;
				case kmb::QUAD:
				case kmb::QUAD2:
					if( points->getPoint( eIter[0], n0 ) &&
						points->getPoint( eIter[1], n1 ) &&
						points->getPoint( eIter[2], n2 ) &&
						points->getPoint( eIter[3], n3 ) )
					{
						if( data->getPhysicalValue( eIter[0], &angle ) ){
							angle -= kmb::Point3D::angle(n3,n0,n1);
						}else{
							angle = -kmb::Point3D::angle(n3,n0,n1);
						}
						data->setPhysicalValue( eIter[0], &angle );

						if( data->getPhysicalValue( eIter[1], &angle ) ){
							angle -= kmb::Point3D::angle(n0,n1,n2);
						}else{
							angle = -kmb::Point3D::angle(n0,n1,n2);
						}
						data->setPhysicalValue( eIter[1], &angle );

						if( data->getPhysicalValue( eIter[2], &angle ) ){
							angle -= kmb::Point3D::angle(n1,n2,n3);
						}else{
							angle = -kmb::Point3D::angle(n1,n2,n3);
						}
						data->setPhysicalValue( eIter[2], &angle );

						if( data->getPhysicalValue( eIter[3], &angle ) ){
							angle -= kmb::Point3D::angle(n2,n3,n0);
						}else{
							angle = -kmb::Point3D::angle(n2,n3,n0);
						}
						data->setPhysicalValue( eIter[3], &angle );
					}
					break;
				default:
					break;
			}
			++eIter;
		}
	}

	kmb::Point3DContainer::iterator pIter = points->begin();
	while( !pIter.isFinished() ){
		if( data->getPhysicalValue( pIter.getId(), &angle ) ){
			angle += 2.0 * PI;
			data->setPhysicalValue( pIter.getId(), &angle );
		}
		++pIter;
	}
	return static_cast<int>( data->getIdCount() );
}
