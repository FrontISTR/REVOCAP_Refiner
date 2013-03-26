/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PatchOperation                                          #
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
#include "MeshDB/kmbPatchOperation.h"
#include "MeshDB/kmbMeshDB.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbFramedPlane.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbPolygon.h"
#include "MeshDB/kmbNodeMatchingInfo.h"
#include "MeshDB/kmbMatching.h"
#include "MeshDB/kmbPolygonPartitioner.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include <cmath>
#include <map>

kmb::PatchOperation::PatchOperation(void)
: points(NULL)
{
}

kmb::PatchOperation::PatchOperation(const kmb::Point3DContainer* nodes)
: points(nodes)
{
}

kmb::PatchOperation::~PatchOperation(void)
{
}

int
kmb::PatchOperation::uniformOrientation(kmb::MeshDB* mesh,kmb::bodyIdType bodyId,kmb::elementIdType elementId)
{
	int count = 0;
	if( mesh ){
		kmb::ElementContainer* patch = mesh->getBodyPtr(bodyId);
		if( patch && patch->isUniqueDim(2) ){

			if( elementId == kmb::Element::nullElementId ){
				elementId = patch->begin().getId();
			}
			kmb::NodeNeighborInfo neighborInfo;
			neighborInfo.appendCoboundary( patch );
			std::set< kmb::elementIdType > checked;

			std::set< kmb::elementIdType > candidate;
			kmb::elementIdType neighbor[6];

			while( true ){
				kmb::ElementContainer::iterator elem = patch->find(elementId);
				if( elem.isFinished() ){
					return count;
				}
				checked.insert( elementId );

				const int boundNum = elem.getBoundaryCount();

				for(int i=0;i<boundNum;++i){
					if( neighbor[i] != kmb::Element::nullElementId )
					{
						if( checked.find( neighbor[i] ) == checked.end() ){
							kmb::ElementContainer::iterator nei = patch->find( neighbor[i] );

							int i0,i1;
							if( !nei.isFinished() &&
								kmb::ElementRelation::getRelation( elem, i0, nei, i1 )
								== kmb::ElementRelation::ANTIADJACENT )
							{
								nei.reverse();
								++count;
							}
							checked.insert( neighbor[i] );
							candidate.insert( neighbor[i] );
						}
					}
				}
				candidate.erase( elementId );

				if( candidate.begin() != candidate.end() ){
					elementId = *(candidate.begin());
				}else{
					break;
				}
			}
		}
	}
	return count;
}


int
kmb::PatchOperation::edgeSubdivider
(kmb::MeshDB* mesh,kmb::bodyIdType edgeId, std::set<kmb::NodeMatchingInfo>& matchingInfo)
{
	int count = 0;
	if( mesh ){
		kmb::ElementContainer* edge = mesh->getBodyPtr( edgeId );
		if( edge && edge->isUniqueDim(1) )
		{

			kmb::elementIdType targetElementId = kmb::Element::nullElementId;
			kmb::ElementContainer::iterator seg;
			kmb::nodeIdType n0 = kmb::nullNodeId;
			kmb::nodeIdType n1 = kmb::nullNodeId;

			std::set<kmb::NodeMatchingInfo>::iterator mIter = matchingInfo.begin();
			while( mIter != matchingInfo.end() )
			{
				if( targetElementId != mIter->elementId ){
					targetElementId = mIter->elementId;
					seg = edge->find(targetElementId);

					if( !seg.isFinished() ){
						n0 = seg.getCellId(0);
						n1 = seg.getCellId(0);
					}
				}
				if( 0.0 < mIter->t && mIter->t < 1.0 )
				{
					n1 = mIter->nodeId;
					if( n0 != n1 ){
						kmb::nodeIdType divSeg[2] = {n0,n1};
						edge->addElement(kmb::SEGMENT,divSeg,mesh->generateElementId());
						++count;
					}

					n0 = n1;
				}
				++mIter;
				if( mIter == matchingInfo.end() || targetElementId != mIter->elementId ){

					n1 = seg.getCellId(1);
					if( n0 != n1 ){
						kmb::nodeIdType divSeg[2] = {n0,n1};
						edge->addElement(kmb::SEGMENT,divSeg,mesh->generateElementId());
					}
					edge->deleteElement(targetElementId);
				}
			}
		}
	}
	return count;
}

int
kmb::PatchOperation::subdivideEdge
(kmb::MeshDB* mesh,kmb::bodyIdType targetId,kmb::bodyIdType edgeId,double tolerance)
{
	int count = 0;
	if( mesh ){
		kmb::ElementContainer* edge = mesh->getBodyPtr( edgeId );
		kmb::ElementContainer* targetEdge = mesh->getBodyPtr( targetId );

		if( edge && edge->isUniqueDim(1) && targetEdge && targetEdge->isUniqueDim(1) )
		{

			kmb::Node node;
			std::set<kmb::nodeIdType> nodeSet;
			edge->getNodesOfBody( nodeSet );
			std::set< kmb::NodeMatchingInfo > matchingInfo;

			std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
			while( nIter != nodeSet.end() ){
				kmb::nodeIdType nodeId = (*nIter);
				mesh->getNode( nodeId, node );

				double t = 0.0;
				kmb::elementIdType elemId = kmb::Element::nullElementId;
				double dist = kmb::Matching::getDistanceEdgeToNodeWithParam(mesh->getNodes(),targetEdge,&node,elemId,t);
				if( dist < tolerance && elemId != kmb::Element::nullElementId ){
					matchingInfo.insert( kmb::NodeMatchingInfo(elemId,nodeId,t) );
				}
				++nIter;
			}
			count += edgeSubdivider(mesh,targetId,matchingInfo);
		}
	}
	return count;
}

kmb::bodyIdType
kmb::PatchOperation::subdivideByEdge
(kmb::MeshDB* mesh,kmb::bodyIdType edgeId,double tolerance,double globalTolerance)
{
	kmb::bodyIdType matchingId = kmb::Body::nullBodyId;
	if( mesh ){
		kmb::ElementContainer* edge = mesh->getBodyPtr( edgeId );
		kmb::ElementContainer* targetEdge = NULL;
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		if( edge && edge->isUniqueDim(1) )
		{

			kmb::Node node;
			std::set<kmb::nodeIdType> nodeSet;
			edge->getNodesOfBody( nodeSet );
			for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
				targetEdge = mesh->getBodyPtr(bodyId);
				if( targetEdge && targetEdge->isUniqueDim(1) )
				{
					kmb::AverageCalculator ave;
					std::set< kmb::NodeMatchingInfo > matchingInfo;
					std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
					while( nIter != nodeSet.end() ){
						kmb::nodeIdType nodeId = (*nIter);
						mesh->getNode( nodeId, node );

						double t = 0.0;
						kmb::elementIdType elemId = kmb::Element::nullElementId;
						double dist = kmb::Matching::getDistanceEdgeToNodeWithParam(mesh->getNodes(),targetEdge,&node,elemId,t);
						if( dist < tolerance ){
							matchingInfo.insert( kmb::NodeMatchingInfo(elemId,nodeId,t) );
						}
						ave.add( dist );
						++nIter;
					}
					if( ave.getAverage() < globalTolerance ){
						matchingId = bodyId;
					}
					edgeSubdivider(mesh,bodyId,matchingInfo);
				}
			}
		}
	}
	return matchingId;
}

int
kmb::PatchOperation::subdivideEdgeWithPatch
(kmb::MeshDB* mesh,kmb::bodyIdType targetId,kmb::bodyIdType targetPatchId,kmb::bodyIdType edgeId,double tolerance)
{
	int count = 0;
	if( mesh ){
		kmb::ElementContainer* edge = mesh->getBodyPtr( edgeId );
		kmb::ElementContainer* targetEdge = mesh->getBodyPtr(targetId);

		if( edge && edge->isUniqueDim(1) &&
			targetEdge && targetEdge->isUniqueDim(1) )
		{

			kmb::Node node;
			std::set<kmb::nodeIdType> nodeSet;
			edge->getNodesOfBody( nodeSet );
			std::set< kmb::NodeMatchingInfo > matchingInfo;
			std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
			while( nIter != nodeSet.end() ){
				kmb::nodeIdType nodeId = (*nIter);
				mesh->getNode( nodeId, node );

				double t = 0.0;
				kmb::elementIdType elemId = kmb::Element::nullElementId;
				double dist = kmb::Matching::getDistanceEdgeToNodeWithParam(mesh->getNodes(),targetEdge,&node,elemId,t);
				if( dist < tolerance ){
					matchingInfo.insert( kmb::NodeMatchingInfo(elemId,nodeId,t) );
				}
				++nIter;
			}
			count += triangleSubdivider(mesh,targetId,targetPatchId,matchingInfo);
			count += edgeSubdivider(mesh,targetId,matchingInfo);
		}
	}
	return count;
}

kmb::bodyIdType
kmb::PatchOperation::subdivideByEdgeWithPatch
(kmb::MeshDB* mesh,kmb::bodyIdType edgeId,double tolerance,double globalTolerance)
{
	kmb::bodyIdType matchingId = kmb::Body::nullBodyId;
	if( mesh ){
		kmb::ElementContainer* edge = mesh->getBodyPtr( edgeId );
		kmb::ElementContainer* targetEdge = NULL;
		kmb::bodyIdType bodyCount = mesh->getBodyCount();
		if( edge && edge->isUniqueDim(1) )
		{

			std::set<kmb::nodeIdType> nodeSet;
			edge->getNodesOfBody( nodeSet );
			for(kmb::bodyIdType bodyId=0;bodyId<bodyCount;++bodyId){
				targetEdge = mesh->getBodyPtr(bodyId);
				if( targetEdge && targetEdge->isUniqueDim(1) )
				{
					kmb::AverageCalculator ave;
					kmb::Node node;
					std::set< kmb::NodeMatchingInfo > matchingInfo;
					std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
					while( nIter != nodeSet.end() ){
						kmb::nodeIdType nodeId = (*nIter);
						mesh->getNode( nodeId, node );

						double t = 0.0;
						kmb::elementIdType elemId = kmb::Element::nullElementId;
						double dist = kmb::Matching::getDistanceEdgeToNodeWithParam(mesh->getNodes(),targetEdge,&node,elemId,t);
						if( dist < tolerance ){
							matchingInfo.insert( kmb::NodeMatchingInfo(elemId,nodeId,t) );
						}
						ave.add( dist );
						++nIter;
					}
					if( ave.getAverage() < globalTolerance ){
						matchingId = bodyId;
					}
					for(kmb::bodyIdType i = 0; i < bodyCount; ++i){
						triangleSubdivider(mesh,bodyId,i,matchingInfo);
					}
					edgeSubdivider(mesh,bodyId,matchingInfo);
				}
			}
		}
	}
	return matchingId;
}

int
kmb::PatchOperation::triangleSubdivider
(kmb::MeshDB* mesh,kmb::bodyIdType edgeId, kmb::bodyIdType patchId, std::set<kmb::NodeMatchingInfo>& matchingInfo)
{
	int counter = 0;
	if( mesh == NULL )
		return counter;
	kmb::ElementContainer* edge = mesh->getBodyPtr( edgeId );
	kmb::ElementContainer* patch = mesh->getBodyPtr( patchId );
	if( edge == NULL || !edge->isUniqueDim(1) ||
		patch == NULL || !patch->isUniqueType( kmb::TRIANGLE ) ){
		return counter;
	}

	kmb::NodeNeighborInfo neighborInfo;
	neighborInfo.appendCoboundary(patch);

	kmb::elementIdType targetElementId = kmb::Element::nullElementId;
	kmb::ElementContainer::iterator seg;
	kmb::elementIdType triangleId = kmb::Element::nullElementId;
	kmb::ElementContainer::iterator tri;
	int faceId = -1;
	kmb::nodeIdType n0 = kmb::nullNodeId;
	kmb::nodeIdType n1 = kmb::nullNodeId;
	kmb::nodeIdType n2 = kmb::nullNodeId;
	bool orient = false;
	std::set<kmb::NodeMatchingInfo>::iterator mIter = matchingInfo.begin();
	while( mIter != matchingInfo.end() )
	{

		if( targetElementId != mIter->elementId ){
			targetElementId = mIter->elementId;
			seg = edge->find( targetElementId );
			if( seg.getType() == kmb::SEGMENT )
			{

				faceId = -1;
				n0 = seg.getCellId(0);
				n1 = seg.getCellId(1);
				kmb::NodeNeighbor::iterator tIter = neighborInfo.beginIteratorAt(n0);
				while( tIter != neighborInfo.endIteratorAt(n0) ){
					kmb::ElementContainer::iterator elem = patch->find( tIter->second );
					if( elem.getType() == kmb::TRIANGLE ){
						for(int i=0;i<3;++i){
							if( elem.getBoundaryCellId(i,0) == seg.getCellId(0) &&
								elem.getBoundaryCellId(i,1) == seg.getCellId(1) )
							{
								n0 = seg.getCellId(0);
								n1 = seg.getCellId(1);
								n2 = elem.getCellId(i);
								tri = elem;
								triangleId = tIter->second;
								faceId = i;
								orient = true;
								break;
							}else
							if( elem.getBoundaryCellId(i,0) == seg.getCellId(1) &&
								elem.getBoundaryCellId(i,1) == seg.getCellId(0) )
							{
								n0 = seg.getCellId(1);
								n1 = seg.getCellId(0);
								n2 = elem.getCellId(i);
								tri = elem;
								triangleId = tIter->second;
								faceId = i;
								orient = false;
								break;
							}
						}
					}
					if( faceId != -1 ){
						break;
					}
					++tIter;
				}
			}
		}
		if( faceId != -1 && 0.0 < mIter->t && mIter->t < 1.0 ){
			n1 = mIter->nodeId;
			if( n0 != n1 ){
				kmb::nodeIdType insTri[3] = {n2,n0,n1};
				patch->addElement( kmb::TRIANGLE, insTri, mesh->generateElementId() );
				++counter;
			}
			n0 = n1;
		}
		++mIter;
		if( faceId != -1 && (mIter == matchingInfo.end() || targetElementId != mIter->elementId ) ){

			n1 = (orient)? seg.getCellId(1) : seg.getCellId(0);
			if( n0 != n1 ){
				kmb::nodeIdType insTri[3] = {n2,n0,n1};
				patch->addElement( kmb::TRIANGLE, insTri, mesh->generateElementId() );
				++counter;
			}
			patch->deleteElement( triangleId );
		}
	}
	return counter;
}









kmb::nodeIdType
duplicatePointOnSection( kmb::MeshDB* mesh, kmb::nodeIdType nodeId, std::map< kmb::nodeIdType, kmb::nodeIdType > &duplicatedNodes )
{
	if( mesh == NULL )
		return kmb::nullNodeId;
	std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator it = duplicatedNodes.find( nodeId );

	if( it == duplicatedNodes.end() ){
		kmb::nodeIdType dupNodeId = mesh->duplicatePoint( nodeId );
		duplicatedNodes.insert( std::pair<kmb::nodeIdType,kmb::nodeIdType>(nodeId,dupNodeId) );
		return dupNodeId;
	}else{
		return it->second;
	}

}
















kmb::nodeIdType
getDividedNodeId
(const kmb::nodeIdType n0,const kmb::nodeIdType n1,
 const double m0,const double m1,
 std::map< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType > &dividedNodes,
 kmb::MeshDB* mesh)
{
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	if( mesh == NULL ){
		return nodeId;
	}
	kmb::Point3D p0, p1;
	if( mesh && mesh->getNode(n0,p0) && mesh->getNode(n1,p1) ){
		std::map< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType >::iterator
			it = dividedNodes.find( std::pair<kmb::nodeIdType,kmb::nodeIdType>(n0,n1) );
		if( it == dividedNodes.end() ){

			nodeId = mesh->insertNode( p0.dividingPoint( p1, m0, m1 ) );
			dividedNodes.insert( std::pair< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType>
				(std::pair<kmb::nodeIdType,kmb::nodeIdType>(n0,n1), nodeId ) );
		}else{
			nodeId = it->second;
		}
	}
	return nodeId;
}

bool
kmb::PatchOperation::divideByPlane
(kmb::MeshDB* mesh,const kmb::bodyIdType targetId,const kmb::FramedPlane &plane,
 kmb::bodyIdType &positiveID,kmb::bodyIdType &negativeID,
 bool capFlag,bool duplicateFlag,double tolerance)
{
	positiveID = kmb::Body::nullBodyId;
	negativeID = kmb::Body::nullBodyId;
	kmb::ElementContainer* target = NULL;
	const kmb::Point3DContainer* points = NULL;
	bool retVal = true;
	if( mesh == NULL ||
		(points=mesh->getNodes()) == NULL ||
		(target=mesh->getBodyPtr(targetId)) == NULL ||
		!target->isUniqueType(kmb::TRIANGLE) )
	{
		retVal &= false;
		return retVal;
	}
	kmb::Body* positive = NULL;
	positive = new kmb::ElementContainerMap();
	kmb::Body* negative = NULL;
	negative = new kmb::ElementContainerMap();


	std::map< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType > dividedNodes;



	std::map< kmb::nodeIdType, kmb::nodeIdType > posnegNodes;


	kmb::ElementContainerMap positiveSegments;



	kmb::Point3D p0,p1,p2;
	kmb::ElementContainer::iterator eIter = target->begin();
	while( eIter != target->end() )
	{
		kmb::elementIdType elementId = eIter.getId();

		kmb::nodeIdType n0 = eIter.getCellId(0);
		kmb::nodeIdType n1 = eIter.getCellId(1);
		kmb::nodeIdType n2 = eIter.getCellId(2);
		++eIter;

		if( mesh->getNode(n0,p0) &&
			mesh->getNode(n1,p1) &&
			mesh->getNode(n2,p2) )
		{
			double eval0 = plane.evaluate( p0 );
			double eval1 = plane.evaluate( p1 );
			double eval2 = plane.evaluate( p2 );
			if( fabs(eval0) < tolerance ) eval0 = 0.0;
			if( fabs(eval1) < tolerance ) eval1 = 0.0;
			if( fabs(eval2) < tolerance ) eval2 = 0.0;
			if( eval0 == 0.0 && eval1 == 0.0 && eval2 == 0.0)
			{

			}
			if( eval0 >= 0 && eval1 >= 0 && eval2 >= 0)
			{

				kmb::nodeIdType tri[3] = {n0,n1,n2};
				positive->addElement( kmb::TRIANGLE, tri, elementId);

				if( capFlag && eval0 == 0.0 && eval1 == 0.0 ){
					kmb::nodeIdType seg[2] = {n0,n1};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( capFlag && eval1 == 0.0 && eval2 == 0.0 ){
					kmb::nodeIdType seg[2] = {n1,n2};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( capFlag && eval2 == 0.0 && eval0 == 0.0 ){
					kmb::nodeIdType seg[2] = {n2,n0};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
			}
			else if( eval0 <= 0 && eval1 <= 0 && eval2 <= 0 )
			{

				kmb::nodeIdType tri[3] = {n0,n1,n2};
				if( duplicateFlag ){

					if( eval0 == 0.0 ){
						kmb::nodeIdType nn0 = duplicatePointOnSection( mesh, n0, posnegNodes );
						tri[0] = nn0;

					}
					if( eval1 == 0.0 ){
						kmb::nodeIdType nn1 = duplicatePointOnSection( mesh, n1, posnegNodes );
						tri[1] = nn1;

					}
					if( eval2 == 0.0 ){
						kmb::nodeIdType nn2 = duplicatePointOnSection( mesh, n2, posnegNodes );
						tri[2] = nn2;

					}
				}
				negative->addElement( kmb::TRIANGLE, tri, elementId);
			}
			else if( eval0 > 0 && eval1 > 0 && eval2 < 0 )
			{

				kmb::nodeIdType n20 = getDividedNodeId(n0,n2,eval0,-eval2,dividedNodes,mesh);
				kmb::nodeIdType n12 = getDividedNodeId(n1,n2,eval1,-eval2,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n0,n1,n20};
				positive->addElement(kmb::TRIANGLE,tri0,mesh->generateElementId());
				kmb::nodeIdType tri1[3] = {n1,n12,n20};
				positive->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				if( capFlag ){


					kmb::nodeIdType seg[2] = {n12,n20};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){

					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}

				kmb::nodeIdType tri2[3] = {n12,n2,n20};
				negative->addElement(kmb::TRIANGLE,tri2,elementId);
			}
			else if( eval0 > 0 && eval1 < 0 && eval2 > 0 )
			{

				kmb::nodeIdType n01 = getDividedNodeId(n0,n1,eval0,-eval1,dividedNodes,mesh);
				kmb::nodeIdType n12 = getDividedNodeId(n2,n1,eval2,-eval1,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n0,n01,n2};
				positive->addElement(kmb::TRIANGLE,tri0,mesh->generateElementId());
				kmb::nodeIdType tri1[3] = {n01,n12,n2};
				positive->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n01,n12};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}

				kmb::nodeIdType tri2[3] = {n01,n1,n12};
				negative->addElement(kmb::TRIANGLE,tri2,elementId);
			}
			else if( eval0 < 0 && eval1 > 0 && eval2 > 0 )
			{

				kmb::nodeIdType n01 = getDividedNodeId(n1,n0,eval1,-eval0,dividedNodes,mesh);
				kmb::nodeIdType n20 = getDividedNodeId(n2,n0,eval2,-eval0,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n01,n1,n2};
				positive->addElement(kmb::TRIANGLE,tri0,mesh->generateElementId());
				kmb::nodeIdType tri1[3] = {n01,n2,n20};
				positive->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n20,n01};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}

				kmb::nodeIdType tri2[3] = {n0,n01,n20};
				negative->addElement(kmb::TRIANGLE,tri2,elementId);
			}
			else if( eval0 < 0 && eval1 < 0 && eval2 > 0 )
			{

				kmb::nodeIdType n12 = getDividedNodeId(n2,n1,eval2,-eval1,dividedNodes,mesh);
				kmb::nodeIdType n20 = getDividedNodeId(n2,n0,eval2,-eval0,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n20,n12,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n20,n12};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n0,n1,n20};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				kmb::nodeIdType tri2[3] = {n1,n12,n20};
				negative->addElement(kmb::TRIANGLE,tri2,mesh->generateElementId());
			}
			else if( eval0 < 0 && eval1 > 0 && eval2 < 0 )
			{

				kmb::nodeIdType n01 = getDividedNodeId(n1,n0,eval1,-eval0,dividedNodes,mesh);
				kmb::nodeIdType n12 = getDividedNodeId(n1,n2,eval1,-eval2,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n01,n1,n12};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n12,n01};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n0,n01,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				kmb::nodeIdType tri2[3] = {n01,n12,n2};
				negative->addElement(kmb::TRIANGLE,tri2,mesh->generateElementId());
			}
			else if( eval0 > 0 && eval1 < 0 && eval2 < 0 )
			{

				kmb::nodeIdType n01 = getDividedNodeId(n0,n1,eval0,-eval1,dividedNodes,mesh);
				kmb::nodeIdType n20 = getDividedNodeId(n0,n2,eval0,-eval2,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n0,n01,n20};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n01,n20};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n01,n1,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				kmb::nodeIdType tri2[3] = {n01,n2,n20};
				negative->addElement(kmb::TRIANGLE,tri2,mesh->generateElementId());
			}
			else if( eval0 == 0.0 && eval1 < 0 && eval2 > 0 )
			{

				kmb::nodeIdType n12 = getDividedNodeId(n2,n1,eval2,-eval1,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n0,n12,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n0,n12};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n0 = duplicatePointOnSection( mesh, n0, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n0,n1,n12};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 == 0.0 && eval1 > 0 && eval2 < 0 )
			{

				kmb::nodeIdType n12 = getDividedNodeId(n1,n2,eval1,-eval2,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n0,n1,n12};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n12,n0};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n0 = duplicatePointOnSection( mesh, n0, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n0,n12,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 > 0 && eval1 == 0 && eval2 < 0 )
			{

				kmb::nodeIdType n20 = getDividedNodeId(n0,n2,eval0,-eval2,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n0,n1,n20};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n1,n20};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){

					n1 = duplicatePointOnSection( mesh, n1, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n1,n2,n20};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 < 0 && eval1 == 0 && eval2 > 0 )
			{

				kmb::nodeIdType n20 = getDividedNodeId(n2,n0,eval2,-eval0,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n1,n2,n20};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n20,n1};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){

					n1 = duplicatePointOnSection( mesh, n1, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n0,n1,n20};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 < 0 && eval1 > 0 && eval2 == 0 )
			{

				kmb::nodeIdType n01 = getDividedNodeId(n1,n0,eval1,-eval0,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n01,n1,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n2,n01};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){

					n2 = duplicatePointOnSection( mesh, n2, posnegNodes );
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n0,n01,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 > 0 && eval1 < 0 && eval2 == 0 )
			{

				kmb::nodeIdType n01 = getDividedNodeId(n0,n1,eval0,-eval1,dividedNodes,mesh);

				kmb::nodeIdType tri0[3] = {n0,n01,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n01,n2};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){

					n2 = duplicatePointOnSection( mesh, n2, posnegNodes );
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
				}

				kmb::nodeIdType tri1[3] = {n01,n1,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
		}
	}
	target->clear();
	if( capFlag ){



		kmb::PolygonPartitioner partitioner;
		kmb::ElementContainerMap triangles;
		kmb::Point3D point;

		kmb::Point2DContainerMap point2Ds;
		std::set< kmb::nodeIdType > nodeSet;
		positiveSegments.getNodesOfBody( nodeSet );
		std::set< kmb::nodeIdType >::iterator nIter = nodeSet.begin();
		while( nIter != nodeSet.end() )
		{
			kmb::nodeIdType nodeId = (*nIter);
			++nIter;
			if( points->getPoint(nodeId,point) ){
				kmb::Point2D uv = plane.transformTo2D( point );
				point2Ds.addPoint( uv, nodeId );
			}
		}
		partitioner.setPoints( &point2Ds );
		partitioner.setInitialPolygon( &positiveSegments );
		retVal &= partitioner.partition( triangles );
		if( triangles.getCount() > 0 ){





			kmb::ElementContainerMap::iterator tIter = triangles.begin();
			while( tIter != triangles.end() )
			{
				kmb::nodeIdType n0 = tIter.getCellId(0);
				kmb::nodeIdType n1 = tIter.getCellId(1);
				kmb::nodeIdType n2 = tIter.getCellId(2);
				kmb::nodeIdType tripos[3] = {n0,n2,n1};
				positive->addElement(kmb::TRIANGLE,tripos,mesh->generateElementId());
				if( duplicateFlag ){
					kmb::nodeIdType neg0 = posnegNodes[ n0 ];
					kmb::nodeIdType neg1 = posnegNodes[ n1 ];
					kmb::nodeIdType neg2 = posnegNodes[ n2 ];
					kmb::nodeIdType trineg[3] = {neg0,neg1,neg2};
					negative->addElement(kmb::TRIANGLE,trineg,mesh->generateElementId());
				}else{
					kmb::nodeIdType trineg[3] = {n0,n1,n2};
					negative->addElement(kmb::TRIANGLE,trineg,mesh->generateElementId());
				}
				++tIter;
			}
		}else{
			retVal &= false;
		}
	}
	if( positive->getCount() > 0 ){
		positiveID = mesh->appendBody( positive, false );
	}else{
		retVal &= false;
		delete positive;
	}
	if( negative->getCount() > 0 ){
		negativeID = mesh->appendBody( negative, false );
	}else{
		retVal &= false;
		delete negative;
	}
	return retVal;
}
