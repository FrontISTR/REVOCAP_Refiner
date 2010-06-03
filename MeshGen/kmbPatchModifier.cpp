/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : AFTetraMesher                                           #
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
#include "MeshGen/kmbPatchModifier.h"

#include "Geometry/kmb_Point3DContainer.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbNodeNeighborInfo.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbQuad.h"

kmb::PatchModifier::PatchModifier(kmb::Point3DContainer* points,kmb::ElementContainer* patch)
: points(NULL)
, patch(NULL)
, evaluator(NULL)
, neighborInfo(NULL)
{
	this->points = points;
	this->evaluator = new kmb::ElementEvaluator( points );
	this->neighborInfo = new kmb::NodeNeighborInfo();
	if( patch && patch->getDimension() == 2 ){
		this->patch = patch;
		this->neighborInfo->appendCoboundary( this->patch );
	}
}

kmb::PatchModifier::PatchModifier(kmb::MeshData* mesh,kmb::bodyIdType bodyId)
: points(NULL)
, patch(NULL)
, evaluator(NULL)
, neighborInfo(NULL)
{
	if( mesh ){
		this->points = mesh->getNodes();
		kmb::ElementContainer* body = mesh->getBodyPtr(bodyId);
		if( body && body->getDimension() == 2 ){
			this->patch = body;
		}
		this->evaluator = new kmb::ElementEvaluator( this->points );
		this->neighborInfo = new kmb::NodeNeighborInfo();
		this->neighborInfo->appendCoboundary( this->patch );
	}
}

kmb::PatchModifier::~PatchModifier(void)
{
	if( this->evaluator ){
		delete this->evaluator;
		this->evaluator = NULL;
	}
	if( this->neighborInfo ){
		delete this->neighborInfo;
		this->neighborInfo = NULL;
	}
}

int
kmb::PatchModifier::getNeighborElements(kmb::elementIdType elementId,kmb::elementIdType* neighbors) const
{
	if( patch && neighborInfo ){
		return neighborInfo->getNeighborElements( elementId, neighbors, patch );
	}else{
		return -1;
	}
}

kmb::elementIdType
kmb::PatchModifier::getNeighborElement(kmb::Face f) const
{
	kmb::Face adj;
	if( patch && neighborInfo ){
		neighborInfo->getAdjacentFace(f,adj,patch);
	}
	return adj.getElementId();
}

int
kmb::PatchModifier::uniformOrientation(kmb::elementIdType elementId)
{
	int count = 0;
	if( patch && neighborInfo ){

		if( elementId == kmb::Element::nullElementId ){
			elementId = patch->begin().getId();
		}
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
			neighborInfo->getNeighborElements(elementId,neighbor,patch);
			for(int i=0;i<boundNum;++i){
				if( neighbor[i] != kmb::Element::nullElementId ){
					if( checked.find( neighbor[i] ) == checked.end() ){
						kmb::ElementContainer::iterator nei = patch->find( neighbor[i] );

						int i0,i1;
						if( !nei.isFinished() &&
							kmb::ElementRelation::getRelation( elem, i0, nei, i1 ) == kmb::ElementRelation::ANTIADJACENT )
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
	return count;
}

bool
kmb::PatchModifier::triangleEdgeSwap(kmb::elementIdType tri0, kmb::elementIdType tri1 )
{
	bool swapped = false;
	if( patch && neighborInfo ){
		kmb::ElementContainer::iterator e0 = patch->find( tri0 );
		kmb::ElementContainer::iterator e1 = patch->find( tri1 );
		if( !e0.isFinished() && !e1.isFinished() )
		{
			neighborInfo->deleteCoboundary( tri0, e0 );
			neighborInfo->deleteCoboundary( tri1, e1 );
			swapped = kmb::Triangle::edgeSwap( e0, e1 );
			neighborInfo->appendCoboundary( tri0, e0 );
			neighborInfo->appendCoboundary( tri1, e1 );
		}
	}
	return swapped;
}

bool
kmb::PatchModifier::quadEdgeSwap(kmb::elementIdType quad0, kmb::elementIdType quad1, bool orientation)
{
	bool swapped = false;
	if( patch && neighborInfo ){
		kmb::ElementContainer::iterator e0 = patch->find( quad0 );
		kmb::ElementContainer::iterator e1 = patch->find( quad1 );
		if( !e0.isFinished() && !e1.isFinished() )
		{
			neighborInfo->deleteCoboundary( quad0, e0 );
			neighborInfo->deleteCoboundary( quad1, e1 );
			swapped = kmb::Quad::edgeSwap( e0, e1, orientation );
			neighborInfo->appendCoboundary( quad0, e0 );
			neighborInfo->appendCoboundary( quad1, e1 );
		}
	}
	return swapped;
}

kmb::elementIdType
kmb::PatchModifier::quadTrisect(kmb::elementIdType quad0, kmb::elementIdType quad1, bool orientation)
{
	kmb::elementIdType elementId = kmb::Element::nullElementId;
	bool swapped = false;
	if( patch && neighborInfo )
	{
		kmb::ElementContainer::iterator e0 = patch->find( quad0 );
		kmb::ElementContainer::iterator e1 = patch->find( quad1 );
		if( !e0.isFinished() && e0.getType() == kmb::QUAD &&
			!e1.isFinished() && e1.getType() == kmb::QUAD )
		{
			neighborInfo->deleteCoboundary( quad0, e0 );
			neighborInfo->deleteCoboundary( quad1, e1 );

			unsigned int rel = 0;
			unsigned int flag = 0x0001;
			for(int i=0;i<4;++i){
				for(int j=0;j<4;++j){
					if( e0.getCellId(i) == e1.getCellId(j) )	rel |= flag;
					flag = flag << 1;
				}
			}
			REVOCAP_Debug_3("quadTrisect flag = %#6x\n", rel);
			kmb::nodeIdType nodes[7]
				= {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};
			kmb::nodeIdType quadNodes[4]
				= {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};

			switch( rel ){
			case 0x0081:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e1.getCellId(1);
				nodes[2] = e1.getCellId(2);
				nodes[3] = e0.getCellId(1);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x0012:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e1.getCellId(2);
				nodes[2] = e1.getCellId(3);
				nodes[3] = e0.getCellId(1);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x0024:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e1.getCellId(3);
				nodes[2] = e1.getCellId(0);
				nodes[3] = e0.getCellId(1);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x0048:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e1.getCellId(0);
				nodes[2] = e1.getCellId(1);
				nodes[3] = e0.getCellId(1);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x0810:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e1.getCellId(1);
				nodes[3] = e1.getCellId(2);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x0120:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e1.getCellId(2);
				nodes[3] = e1.getCellId(3);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x0240:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e1.getCellId(3);
				nodes[3] = e1.getCellId(0);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x0480:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e1.getCellId(0);
				nodes[3] = e1.getCellId(1);
				nodes[4] = e0.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x8100:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e1.getCellId(1);
				nodes[4] = e1.getCellId(2);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x1200:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e1.getCellId(2);
				nodes[4] = e1.getCellId(3);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x2400:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e1.getCellId(3);
				nodes[4] = e1.getCellId(0);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x4800:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e1.getCellId(0);
				nodes[4] = e1.getCellId(1);
				nodes[5] = e0.getCellId(3);
				swapped = true;
				break;
			case 0x1008:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e0.getCellId(3);
				nodes[4] = e1.getCellId(1);
				nodes[5] = e1.getCellId(2);
				swapped = true;
				break;
			case 0x2001:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e0.getCellId(3);
				nodes[4] = e1.getCellId(2);
				nodes[5] = e1.getCellId(3);
				swapped = true;
				break;
			case 0x4002:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e0.getCellId(3);
				nodes[4] = e1.getCellId(3);
				nodes[5] = e1.getCellId(0);
				swapped = true;
				break;
			case 0x8004:
				nodes[0] = e0.getCellId(0);
				nodes[1] = e0.getCellId(1);
				nodes[2] = e0.getCellId(2);
				nodes[3] = e0.getCellId(3);
				nodes[4] = e1.getCellId(0);
				nodes[5] = e1.getCellId(1);
				swapped = true;
				break;
			default:
				break;
			}
			if( swapped ){

				kmb::Point3D center(0.0,0.0,0.0);
				kmb::Point3D point;
				for(int i=0;i<6;++i){
					if( points->getPoint(nodes[i],point) ){
						center += point;
					}
				}
				center.scale( 1.0/6.0 );
				nodes[6] = points->addPoint( center );
				if( orientation ){

					e0.setCellId(0, nodes[0]);
					e0.setCellId(1, nodes[1]);
					e0.setCellId(2, nodes[2]);
					e0.setCellId(3, nodes[6]);
					e1.setCellId(0, nodes[2]);
					e1.setCellId(1, nodes[3]);
					e1.setCellId(2, nodes[4]);
					e1.setCellId(3, nodes[6]);
					quadNodes[0] = nodes[4];
					quadNodes[1] = nodes[5];
					quadNodes[2] = nodes[0];
					quadNodes[3] = nodes[6];
					elementId = patch->addElement(kmb::QUAD,quadNodes);
				}else{

					e0.setCellId(0, nodes[5]);
					e0.setCellId(1, nodes[0]);
					e0.setCellId(2, nodes[1]);
					e0.setCellId(3, nodes[6]);
					e1.setCellId(0, nodes[1]);
					e1.setCellId(1, nodes[2]);
					e1.setCellId(2, nodes[3]);
					e1.setCellId(3, nodes[6]);
					quadNodes[0] = nodes[3];
					quadNodes[1] = nodes[4];
					quadNodes[2] = nodes[5];
					quadNodes[3] = nodes[6];
					elementId = patch->addElement(kmb::QUAD,quadNodes);
				}
			}
		}
	}
	return elementId;
}

bool
kmb::PatchModifier::edgeContract(kmb::nodeIdType node0, kmb::nodeIdType node1)
{
	if( patch && neighborInfo ){
		if( patch->isUniqueType(kmb::TRIANGLE) ){

			std::vector< kmb::elementIdType > elements1;
			neighborInfo->getSurroundingElements(node1, elements1);

			std::vector< kmb::elementIdType >::iterator eIter = elements1.begin();
			while( eIter != elements1.end() )
			{
				kmb::elementIdType elemId = (*eIter);
				kmb::ElementContainer::iterator element = patch->find( elemId );
				if( !element.isFinished() ){
					if( element.include( node0 ) ){

						neighborInfo->deleteCoboundary( elemId, element );
						patch->deleteElement( elemId );
					}else{

						neighborInfo->deleteCoboundary( elemId, element );
						element.replaceNodeId( node1, node0 );

						neighborInfo->appendCoboundary( elemId, element );
					}
				}
				++eIter;
			}
			return true;
		}
	}
	return false;
}

double
kmb::PatchModifier::getAngleByIndex(kmb::elementIdType elementId,int index) const
{
	kmb::ElementContainer::const_iterator elem = patch->find(elementId);
	return evaluator->getAngleByIndex( elem, index );
}
