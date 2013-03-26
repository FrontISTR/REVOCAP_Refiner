/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : NodeNeighborFaceInfo                                    #
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
#include "MeshDB/kmbNodeNeighborFaceInfo.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbQuad.h"

kmb::NodeNeighborFaceInfo::NodeNeighborFaceInfo(void)
{
}

kmb::NodeNeighborFaceInfo::~NodeNeighborFaceInfo(void)
{
}

void
kmb::NodeNeighborFaceInfo::clear(void)
{
	coboundaries.clear();
}

bool
kmb::NodeNeighborFaceInfo::append( kmb::nodeIdType nodeID, kmb::Face faceID )
{

	std::pair< NodeNeighborFace::iterator, NodeNeighborFace::iterator >
		bIterPair = coboundaries.equal_range( nodeID );
	NodeNeighborFace::iterator nIter = bIterPair.first;
	while( nIter != bIterPair.second )
	{
		if(nIter->second == faceID){

			return false;
		}
		++nIter;
	}
	coboundaries.insert( std::pair< kmb::nodeIdType, kmb::Face >(nodeID,faceID) );
	return true;
}

bool
kmb::NodeNeighborFaceInfo::erase( kmb::nodeIdType nodeID, kmb::Face faceID )
{

	std::pair< NodeNeighborFace::iterator, NodeNeighborFace::iterator >
		bIterPair = coboundaries.equal_range( nodeID );
	NodeNeighborFace::iterator nIter = bIterPair.first;
	while( nIter != bIterPair.second )
	{
		if(nIter->second == faceID){

			coboundaries.erase( nIter );
			return true;
		}
		++nIter;
	}
	return false;
}

bool
kmb::NodeNeighborFaceInfo::appendCoboundary( kmb::Face face, const kmb::ElementContainer* body )
{
	if( body == NULL )
		return false;
	kmb::ElementContainer::const_iterator element = body->find( face.getElementId() );
	if( element.isFinished() ){
		return false;
	}
	int i = static_cast<int>( face.getLocalFaceId() );
	int faceNodeCount = element.getBoundaryNodeCount(i);
	for(int j=0;j<faceNodeCount;++j){
		kmb::nodeIdType nodeId = element.getBoundaryCellId(i,j);
		append(nodeId,face);
	}
	return true;
}

bool
kmb::NodeNeighborFaceInfo::appendCoboundary( kmb::Face face, const kmb::MeshData* mesh )
{
	if( mesh == NULL )
		return false;
	kmb::ElementContainer::const_iterator element = mesh->findElement( face.getElementId() );
	if( element.isFinished() ){
		return false;
	}
	int i = static_cast<int>( face.getLocalFaceId() );
	int faceNodeCount = element.getBoundaryNodeCount(i);
	for(int j=0;j<faceNodeCount;++j){
		kmb::nodeIdType nodeId = element.getBoundaryCellId(i,j);
		append(nodeId,face);
	}
	return true;
}

bool
kmb::NodeNeighborFaceInfo::deleteCoboundary( kmb::Face face, const kmb::ElementContainer* body )
{
	if( body == NULL )
		return false;
	kmb::ElementContainer::const_iterator element = body->find( face.getElementId() );
	if( element.isFinished() ){
		return false;
	}
	const int faceLength = element.getBoundaryCount();
	for(int i=0;i<faceLength;++i){
		int faceNodeCount = element.getBoundaryNodeCount(i);
		for(int j=0;j<faceNodeCount;++j){
			kmb::nodeIdType nodeId = element.getBoundaryCellId(i,j);
			erase(nodeId,face);
		}
	}
	return true;
}

bool
kmb::NodeNeighborFaceInfo::appendCoboundary( const kmb::DataBindings* data, const kmb::ElementContainer* body )
{
	if( body == NULL || data == NULL )
		return false;
	bool retVal = false;

	switch( data->getBindingMode() ){
	case kmb::DataBindings::FaceVariable:
	case kmb::DataBindings::FaceGroup:
		{
			kmb::DataBindings::const_iterator dIter = data->begin();
			kmb::DataBindings::const_iterator dEnd = data->end();
			kmb::Face face;
			while( dIter != dEnd ){
				if( dIter.getFace( face ) ){
					appendCoboundary( face, body );
				}
				++dIter;
			}
			retVal = true;
			break;
		}
	default:
		break;
	}
	return retVal;
}

bool
kmb::NodeNeighborFaceInfo::appendCoboundary( const kmb::DataBindings* data, const kmb::MeshData* mesh )
{
	if( mesh == NULL || data == NULL )
		return false;
	bool retVal = false;
	const kmb::ElementContainer* body = mesh->getBodyPtr( data->getTargetBodyId() );

	switch( data->getBindingMode() ){
	case kmb::DataBindings::FaceVariable:
	case kmb::DataBindings::FaceGroup:
		{
			kmb::DataBindings::const_iterator dIter = data->begin();
			kmb::DataBindings::const_iterator dEnd = data->end();
			kmb::Face face;
			while( dIter != dEnd ){
				if( dIter.getFace( face ) ){
					if( body ){
						appendCoboundary( face, body );
					}else{
						appendCoboundary( face, mesh );
					}
				}
				++dIter;
			}
			retVal = true;
			break;
		}
	default:
		break;
	}
	return retVal;
}

int
kmb::NodeNeighborFaceInfo::getFaceNeighbor
( kmb::Face face, kmb::Face *neighbors, const kmb::ElementContainer* elements ) const
{
	if( elements == NULL )
		return 0;

	kmb::Element* element = face.createElement( elements );
	if( element == NULL )
		return 0;

	int count = 0;
	const int boundaryNum = element->getBoundaryCount();


	if( element->getDimension() == 1 )
	{
		for(int i=0;i<2;++i)
		{
			neighbors[i] = kmb::Face( kmb::Element::nullElementId, -1 );
			const kmb::nodeIdType nodeID = element->getCellId(i);

			std::pair< NodeNeighborFace::const_iterator, NodeNeighborFace::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighborFace::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count].getElementId() == kmb::Element::nullElementId )
			{
				kmb::Face coFace = eIter->second;
				++eIter;

				if( face != coFace ){
					neighbors[count] = coFace;
					++count;
				}
			}
		}
	}else{
		int index = -1;
		int otherIndex = -1;
		for(int i=0;i<boundaryNum;++i){
			neighbors[i] = kmb::Face( kmb::Element::nullElementId, -1 );

			kmb::nodeIdType nodeID = element->getBoundaryCellId(i,0);

			std::pair< NodeNeighborFace::const_iterator, NodeNeighborFace::const_iterator >
				eIterPair = coboundaries.equal_range( nodeID );
			NodeNeighborFace::const_iterator eIter = eIterPair.first;
			while( eIter != eIterPair.second &&
				neighbors[count].getElementId() == kmb::Element::nullElementId )
			{
				kmb::Face coFace = eIter->second;
				++eIter;

				if( face != coFace ){
					kmb::Element* coElement = coFace.createElement( elements );
					if( coElement != NULL ){


						kmb::ElementRelation::relationType rel =
							kmb::ElementRelation::getRelation( *element, index, *coElement, otherIndex );
						if( index == i && rel == kmb::ElementRelation::ADJACENT ){
							neighbors[count] = coFace;
							++count;
						}
						delete coElement;
					}
				}
			}
		}
	}
	delete element;
	return count;
}

bool
kmb::NodeNeighborFaceInfo::getFaceNeighborByIndex( kmb::Face face, int index, const kmb::ElementContainer* elements, kmb::Face &neighborFace ) const
{
	if( elements == NULL ){
		return false;
	}

	kmb::ElementContainer::const_iterator elem = elements->find( face.getElementId() );
	if( elem.isFinished() ){
		return false;
	}

	kmb::elementType etype = face.getFaceElementType( elements );
	if( index < 0 || kmb::Element::getBoundaryCount( etype ) <= index ){
		return false;
	}

	if( etype == kmb::SEGMENT ){


		const kmb::nodeIdType nodeId = elem.getBoundaryCellId( face.getLocalFaceId(), index );
		std::pair< kmb::NodeNeighborFace::const_iterator, kmb::NodeNeighborFace::const_iterator >
			eIterPair = coboundaries.equal_range( nodeId );
		kmb::NodeNeighborFace::const_iterator eIter = eIterPair.first;
		while( eIter != eIterPair.second ){
			kmb::Face coFace = eIter->second;
			++eIter;

			if( face != coFace ){
				neighborFace = coFace;
				return true;
			}
		}
	}else if( etype == kmb::TRIANGLE ){



		kmb::Triangle tri;
		face.getFaceLinearElement( elements, tri );

		const kmb::nodeIdType nodeId = tri.getBoundaryCellId( index, 0 );
		std::pair< kmb::NodeNeighborFace::const_iterator, kmb::NodeNeighborFace::const_iterator >
			eIterPair = coboundaries.equal_range( nodeId );
		kmb::NodeNeighborFace::const_iterator eIter = eIterPair.first;
		int i0,i1;
		while( eIter != eIterPair.second ){
			kmb::Face coFace = eIter->second;
			++eIter;

			if( face != coFace ){
				kmb::Element* coElement = coFace.createElement( elements );
				if( coElement != NULL ){
					kmb::ElementRelation::relationType rel =
						kmb::ElementRelation::getRelation( tri, i0, *coElement, i1 );
					if( index == i0 && rel == kmb::ElementRelation::ADJACENT ){
						neighborFace = coFace;
						delete coElement;
						return true;
					}
					delete coElement;
				}
			}
		}
	}else if( etype == kmb::QUAD ){
		kmb::Quad quad;
		face.getFaceLinearElement( elements, quad );

		const kmb::nodeIdType nodeId = quad.getBoundaryCellId( index, 0 );
		std::pair< kmb::NodeNeighborFace::const_iterator, kmb::NodeNeighborFace::const_iterator >
			eIterPair = coboundaries.equal_range( nodeId );
		kmb::NodeNeighborFace::const_iterator eIter = eIterPair.first;
		int i0,i1;
		while( eIter != eIterPair.second )
		{
			kmb::Face coFace = eIter->second;
			++eIter;

			if( face != coFace ){
				kmb::Element* coElement = coFace.createElement( elements );
				if( coElement != NULL ){
					kmb::ElementRelation::relationType rel =
						kmb::ElementRelation::getRelation( quad, i0, *coElement, i1 );
					if( index == i0 && rel == kmb::ElementRelation::ADJACENT ){
						neighborFace = coFace;
						delete coElement;
						return true;
					}
					delete coElement;
				}
			}
		}
	}
	return false;
}

size_t
kmb::NodeNeighborFaceInfo::getFaceCountAroundNode(nodeIdType nodeID) const
{
	return this->coboundaries.count(nodeID);
}

size_t
kmb::NodeNeighborFaceInfo::getSize(void) const
{
	return this->coboundaries.size();
}
