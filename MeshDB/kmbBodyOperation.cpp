/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BodyOperation                                           #
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
#include <cstdlib>

#include "MeshDB/kmbBodyOperation.h"
#include "MeshDB/kmbElementContainer.h"

kmb::BodyOperation::BodyOperation(const Point3DContainer* nodes)
: points(nodes)
{
}

kmb::BodyOperation::~BodyOperation(void)
{
}

size_t
kmb::BodyOperation::getEdgesOfBody( const kmb::ElementContainer* body, kmb::ElementContainer* result) const
{
	if( body->getCount() == 0 || !body->isUniqueDim(2) ){
		return 0;
	}
	std::set< std::pair<kmb::nodeIdType,kmb::nodeIdType> > nodePair;
	kmb::ElementContainer::const_iterator eIter = body->begin();
	while( !eIter.isFinished() )
	{
		const int len = eIter.getBoundaryCount();
		for(int i=0;i<len;++i){
			kmb::nodeIdType n0 = eIter.getBoundaryCellId(i,0);
			kmb::nodeIdType n1 = eIter.getBoundaryCellId(i,1);
			if( n0 < n1 ){
				nodePair.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(n0,n1) );
			}else{
				nodePair.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(n1,n0) );
			}
		}
		++eIter;
	}
	if( result ){
		kmb::nodeIdType nodes[2];
		std::set< std::pair<kmb::nodeIdType,kmb::nodeIdType> >::iterator nIter = nodePair.begin();
		while( nIter != nodePair.end() ){
			nodes[0] = nIter->first;
			nodes[1] = nIter->second;
			result->addElement( kmb::SEGMENT, nodes );
			++nIter;
		}
	}
	return nodePair.size();
}

size_t
kmb::BodyOperation::getEdgesIntersectPlane(const kmb::ElementContainer* body,const kmb::Plane* plane,kmb::ElementContainer* result) const
{
	if( body && plane ){
		return 0;
	}
	std::set< std::pair<kmb::nodeIdType,kmb::nodeIdType> > nodePair;
	kmb::Point3D p0,p1;
	kmb::ElementContainer::const_iterator eIter = body->begin();
	while( !eIter.isFinished() )
	{
		const int degree = eIter.getDegree();
		const int len = eIter.getNodeCount();


		for(int i0=0;i0<len-1;i0++){
			for(int i1=i0+1;i1<len;i1++){
				if( abs(eIter.isConnected(i0,i1)) == degree )
				{
					kmb::nodeIdType n0 = eIter.getCellId(i0);
					kmb::nodeIdType n1 = eIter.getCellId(i1);
					if( points->getPoint(n0,p0) && points->getPoint(n1,p1) ){
						if( plane->evaluate( p0 ) * plane->evaluate( p1 ) < 0 )
						{

							if( n0 < n1 ){
								nodePair.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(n0,n1) );
							}else{
								nodePair.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(n1,n0) );
							}
						}
					}
				}
			}
		}
		++eIter;
	}
	if( result ){
		kmb::nodeIdType nodes[2];
		std::set< std::pair<kmb::nodeIdType,kmb::nodeIdType> >::iterator nIter = nodePair.begin();
		while( nIter != nodePair.end() ){
			nodes[0] = nIter->first;
			nodes[1] = nIter->second;
			result->addElement( kmb::SEGMENT, nodes );
			++nIter;
		}
	}
	return nodePair.size();
}
