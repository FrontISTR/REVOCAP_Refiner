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
#include "MeshGen/kmbPatchOperation.h"
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
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshGen/kmbPolygonPartitioner.h"
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
			// 最初の elementId を与える
			if( elementId == kmb::Element::nullElementId ){
				elementId = patch->begin().getId();
			}
			kmb::NodeNeighborInfo neighborInfo;
			neighborInfo.appendCoboundary( patch );
			std::set< kmb::elementIdType > checked;

			std::set< kmb::elementIdType > candidate;   // まだすべての近傍を調べ終わっていない要素
			kmb::elementIdType neighbor[6];

			while( true ){
				kmb::ElementContainer::iterator elem = patch->find(elementId);
				if( elem.isFinished() ){
					return count;
				}
				checked.insert( elementId );
				// 近傍取得
				const int boundNum = elem.getBoundaryCount();
				for(int i=0;i<boundNum;++i){
					if( neighbor[i] != kmb::Element::nullElementId )
					{
						if( checked.find( neighbor[i] ) == checked.end() ){
							kmb::ElementContainer::iterator nei = patch->find( neighbor[i] );
							// チェックされていなければ、自分優先
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

// subdivide body of edgeId by NodeMatchingInfo
int
kmb::PatchOperation::edgeSubdivider
(kmb::MeshDB* mesh,kmb::bodyIdType edgeId, std::set<kmb::NodeMatchingInfo>& matchingInfo)
{
	int count = 0;
	if( mesh ){
		kmb::ElementContainer* edge = mesh->getBodyPtr( edgeId );
		if( edge && edge->isUniqueDim(1) )
		{
			// initialize parameters
			kmb::elementIdType targetElementId = kmb::Element::nullElementId;
			kmb::ElementContainer::iterator seg;
			kmb::nodeIdType n0 = kmb::nullNodeId;
			kmb::nodeIdType n1 = kmb::nullNodeId;
			// iterate node matching info
			std::set<kmb::NodeMatchingInfo>::iterator mIter = matchingInfo.begin();
			while( mIter != matchingInfo.end() )
			{
				if( targetElementId != mIter->elementId ){
					targetElementId = mIter->elementId;
					seg = edge->find(targetElementId);
					// initialize node id
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
					// insert last segment
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
			// iterate node of edge1
			kmb::Node node;
			std::set<kmb::nodeIdType> nodeSet;
			edge->getNodesOfBody( nodeSet );
			std::set< kmb::NodeMatchingInfo > matchingInfo;
			// get matching info
			std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
			while( nIter != nodeSet.end() ){
				kmb::nodeIdType nodeId = (*nIter);
				mesh->getNode( nodeId, node );
				// get nearest segment in targetEdge from node of edge
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
			// iterate node of edge1
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
						// get nearest segment in targetEdge from node of edge
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
			// iterate node of edge1
			kmb::Node node;
			std::set<kmb::nodeIdType> nodeSet;
			edge->getNodesOfBody( nodeSet );
			std::set< kmb::NodeMatchingInfo > matchingInfo;
			std::set<kmb::nodeIdType>::iterator nIter = nodeSet.begin();
			while( nIter != nodeSet.end() ){
				kmb::nodeIdType nodeId = (*nIter);
				mesh->getNode( nodeId, node );
				// get nearest segment in targetEdge from node of edge
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
			// iterate node of edge1
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
						// get nearest segment in targetEdge from node of edge
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
	// calc node neighbor
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
		// changes target segment
		if( targetElementId != mIter->elementId ){
			targetElementId = mIter->elementId;
			seg = edge->find( targetElementId );
			if( seg.getType() == kmb::SEGMENT )
			{
				// search triangle whose face is seg
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
								n2 = elem.getCellId(i);  // elem = [n0,n1,n2]
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
								n2 = elem.getCellId(i);  // elem = [n0,n1,n2]
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
			// insert last triangle
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

//------------------- 平面によるパッチの分割 --------------------------//
// local function
//
// 既に二重化した nodeId を duplicatedNodes に記憶
// 既に二重化されていればその結果を返し、
// 二重化されていなければ mesh に nodeId を二重化して登録
//

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

// local function
//
// n0 正領域の点
// n1 負領域の点
// n0 と n1 を m0 : m1 に内分した点を mesh に追加して n とし、
// dividedNodes[ std::pair(n0,n1) ] = n 
// とする
// すでに dividedNodes に登録されていたら何もしない
// 戻り値は n 
//

// 面グループを平面で分割する
// n0 正領域
// n1 負領域

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
			// ない場合は作る
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

	// 負領域の点と、正領域の点の組から断面上の点を求める
	std::map< std::pair<kmb::nodeIdType,kmb::nodeIdType>, kmb::nodeIdType > dividedNodes;

	// 断面上の節点を二重化するときに
	// 正領域の断面上の節点と負領域の断面上の節点の対応を記憶する
	std::map< kmb::nodeIdType, kmb::nodeIdType > posnegNodes;

	// 断面にふたをするために正領域の断面上の SEGMENT を覚えておく
	kmb::ElementContainerMap positiveSegments;


	// 平面を横切っている辺の検索
	kmb::Point3D p0,p1,p2;
	kmb::ElementContainer::iterator eIter = target->begin();
	while( eIter != target->end() )
	{
		kmb::elementIdType elementId = eIter.getId();
		// 三角形きめうち
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
				// z z z 何もしない
			}
			if( eval0 >= 0 && eval1 >= 0 && eval2 >= 0)
			{
				// pz pz pz
				kmb::nodeIdType tri[3] = {n0,n1,n2};
				positive->addElement( kmb::TRIANGLE, tri, elementId);
				// zero が2個ある時は SEGMENT 登録
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
				// nz nz nz
				kmb::nodeIdType tri[3] = {n0,n1,n2};
				if( duplicateFlag ){
					// 必要なら二重化
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
				// p p n
				kmb::nodeIdType n20 = getDividedNodeId(n0,n2,eval0,-eval2,dividedNodes,mesh);
				kmb::nodeIdType n12 = getDividedNodeId(n1,n2,eval1,-eval2,dividedNodes,mesh);
				// 新しい要素を登録
				kmb::nodeIdType tri0[3] = {n0,n1,n20};
				positive->addElement(kmb::TRIANGLE,tri0,mesh->generateElementId());
				kmb::nodeIdType tri1[3] = {n1,n12,n20};
				positive->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				if( capFlag ){
					// SEGMENT は正領域の三角形の辺とは同じ向きにする
					// polygon partitioner の向きとあわせるため 
					kmb::nodeIdType seg[2] = {n12,n20};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}
				// negative は elementId を再利用
				kmb::nodeIdType tri2[3] = {n12,n2,n20};
				negative->addElement(kmb::TRIANGLE,tri2,elementId);
			}
			else if( eval0 > 0 && eval1 < 0 && eval2 > 0 )
			{
				// p n p
				kmb::nodeIdType n01 = getDividedNodeId(n0,n1,eval0,-eval1,dividedNodes,mesh);
				kmb::nodeIdType n12 = getDividedNodeId(n2,n1,eval2,-eval1,dividedNodes,mesh);
				// 新しい要素を登録
				kmb::nodeIdType tri0[3] = {n0,n01,n2};
				positive->addElement(kmb::TRIANGLE,tri0,mesh->generateElementId());
				kmb::nodeIdType tri1[3] = {n01,n12,n2};
				positive->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n01,n12};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}
				// negative は elementId を再利用
				kmb::nodeIdType tri2[3] = {n01,n1,n12};
				negative->addElement(kmb::TRIANGLE,tri2,elementId);
			}
			else if( eval0 < 0 && eval1 > 0 && eval2 > 0 )
			{
				// n p p
				kmb::nodeIdType n01 = getDividedNodeId(n1,n0,eval1,-eval0,dividedNodes,mesh);
				kmb::nodeIdType n20 = getDividedNodeId(n2,n0,eval2,-eval0,dividedNodes,mesh);
				// 新しい要素を登録
				kmb::nodeIdType tri0[3] = {n01,n1,n2};
				positive->addElement(kmb::TRIANGLE,tri0,mesh->generateElementId());
				kmb::nodeIdType tri1[3] = {n01,n2,n20};
				positive->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n20,n01};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}
				// negative は elementId を再利用
				kmb::nodeIdType tri2[3] = {n0,n01,n20};
				negative->addElement(kmb::TRIANGLE,tri2,elementId);
			}
			else if( eval0 < 0 && eval1 < 0 && eval2 > 0 )
			{
				// n n p
				kmb::nodeIdType n12 = getDividedNodeId(n2,n1,eval2,-eval1,dividedNodes,mesh);
				kmb::nodeIdType n20 = getDividedNodeId(n2,n0,eval2,-eval0,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n20,n12,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n20,n12};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n0,n1,n20};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				kmb::nodeIdType tri2[3] = {n1,n12,n20};
				negative->addElement(kmb::TRIANGLE,tri2,mesh->generateElementId());
			}
			else if( eval0 < 0 && eval1 > 0 && eval2 < 0 )
			{
				// n p n
				kmb::nodeIdType n01 = getDividedNodeId(n1,n0,eval1,-eval0,dividedNodes,mesh);
				kmb::nodeIdType n12 = getDividedNodeId(n1,n2,eval1,-eval2,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n01,n1,n12};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n12,n01};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n0,n01,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				kmb::nodeIdType tri2[3] = {n01,n12,n2};
				negative->addElement(kmb::TRIANGLE,tri2,mesh->generateElementId());
			}
			else if( eval0 > 0 && eval1 < 0 && eval2 < 0 )
			{
				// p n n
				kmb::nodeIdType n01 = getDividedNodeId(n0,n1,eval0,-eval1,dividedNodes,mesh);
				kmb::nodeIdType n20 = getDividedNodeId(n0,n2,eval0,-eval2,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n0,n01,n20};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n01,n20};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n01,n1,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
				kmb::nodeIdType tri2[3] = {n01,n2,n20};
				negative->addElement(kmb::TRIANGLE,tri2,mesh->generateElementId());
			}
			else if( eval0 == 0.0 && eval1 < 0 && eval2 > 0 )
			{
				// z n p
				kmb::nodeIdType n12 = getDividedNodeId(n2,n1,eval2,-eval1,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n0,n12,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n0,n12};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n0 = duplicatePointOnSection( mesh, n0, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n0,n1,n12};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 == 0.0 && eval1 > 0 && eval2 < 0 )
			{
				// z p n
				kmb::nodeIdType n12 = getDividedNodeId(n1,n2,eval1,-eval2,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n0,n1,n12};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n12,n0};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n0 = duplicatePointOnSection( mesh, n0, posnegNodes );
					n12 = duplicatePointOnSection( mesh, n12, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n0,n12,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 > 0 && eval1 == 0 && eval2 < 0 )
			{
				// p z n
				kmb::nodeIdType n20 = getDividedNodeId(n0,n2,eval0,-eval2,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n0,n1,n20};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n1,n20};
					positiveSegments.addElement( kmb::SEGMENT,seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n1 = duplicatePointOnSection( mesh, n1, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n1,n2,n20};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 < 0 && eval1 == 0 && eval2 > 0 )
			{
				// n z p
				kmb::nodeIdType n20 = getDividedNodeId(n2,n0,eval2,-eval0,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n1,n2,n20};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n20,n1};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n1 = duplicatePointOnSection( mesh, n1, posnegNodes );
					n20 = duplicatePointOnSection( mesh, n20, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n0,n1,n20};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 < 0 && eval1 > 0 && eval2 == 0 )
			{
				// n p z
				kmb::nodeIdType n01 = getDividedNodeId(n1,n0,eval1,-eval0,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n01,n1,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n2,n01};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n2 = duplicatePointOnSection( mesh, n2, posnegNodes );
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
				}
				// 新しい要素を登録
				kmb::nodeIdType tri1[3] = {n0,n01,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
			else if( eval0 > 0 && eval1 < 0 && eval2 == 0 )
			{
				// p n z
				kmb::nodeIdType n01 = getDividedNodeId(n0,n1,eval0,-eval1,dividedNodes,mesh);
				// positive は elementId を再利用
				kmb::nodeIdType tri0[3] = {n0,n01,n2};
				positive->addElement(kmb::TRIANGLE,tri0,elementId);
				if( capFlag ){
					kmb::nodeIdType seg[2] = {n01,n2};
					positiveSegments.addElement( kmb::SEGMENT, seg );
				}
				if( duplicateFlag ){
					// 必要なら二重化
					n2 = duplicatePointOnSection( mesh, n2, posnegNodes );
					n01 = duplicatePointOnSection( mesh, n01, posnegNodes );
				}
				// negative に新しい要素を登録
				kmb::nodeIdType tri1[3] = {n01,n1,n2};
				negative->addElement(kmb::TRIANGLE,tri1,mesh->generateElementId());
			}
		}
	}
	target->clear();
	if( capFlag ){
		// posnegNodes の key となっている nodeId について 断面に射影
		// そのままの Id で Point2DContainer に登録
		// polygonpartioner で positiveSegments を三角形化
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
			// その三角形を positive と negative に追加
			// positive には polygon partitioner で三角形化した向きとは逆にする
			// 接している三角形同士の辺の向きは互いに逆だから
			// negative には nodeId を posnegNodes で変換して向きは
			// polygon partitioner のまま追加する
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
