/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : BoundaryExtractor                                       #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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
#include "MeshDB/kmbBoundaryExtractor.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbSurfaceOperation.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbDataBindings.h"

kmb::BoundaryExtractor::BoundaryExtractor(void)
: mesh(NULL)
, reverseMode(false)
{
}

kmb::BoundaryExtractor::~BoundaryExtractor(void)
{
}

void
kmb::BoundaryExtractor::clear(void)
{
	this->mesh = NULL;
	reverseMode = false;
	facemap.clear();
}

void
kmb::BoundaryExtractor::setMesh(kmb::MeshData* mesh)
{
	this->mesh = mesh;
}

void
kmb::BoundaryExtractor::setReverseMode(bool mode)
{
	this->reverseMode = mode;
}

bool
kmb::BoundaryExtractor::getReverseMode(void) const
{
	return reverseMode;
}

void
kmb::BoundaryExtractor::appendBody(kmb::bodyIdType bodyId)
{
	if( mesh == NULL ){
		return;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr( bodyId );
	if( body == NULL ){
		return;
	}
	appendElementContainer( body );
}

void
kmb::BoundaryExtractor::appendElementContainer(kmb::ElementContainer* body)
{
	if( body==NULL ){
		return;
	}
	kmb::ElementContainer::iterator eIter = body->begin();
	while( eIter != body->end() )
	{
		appendElement( eIter.getId(), eIter );
		++eIter;
	}
}

void
kmb::BoundaryExtractor::appendUnitBody(const kmb::ElementContainer* body)
{
	if( body==NULL ){
		return;
	}
	kmb::ElementContainer::const_iterator eIter = body->begin();
	while( eIter != body->end() )
	{
		appendElement( eIter.getId(), eIter, body );
		++eIter;
	}
}

void
kmb::BoundaryExtractor::appendData(const char* name)
{
	if( mesh == NULL ){
		return;
	}
	kmb::DataBindings* data = mesh->getDataBindingsPtr(name);
	if( data == NULL ){
		return;
	}
	appendData( data );
}

void
kmb::BoundaryExtractor::appendData(const kmb::DataBindings* data)
{
	if( mesh == NULL || data==NULL ){
		return;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr( data->getTargetBodyId() );
	if( body == NULL ){
		return;
	}
	switch( data->getBindingMode() ){
	case kmb::DataBindings::ELEMENTGROUP:
	case kmb::DataBindings::ELEMENTVARIABLE:
	{
		kmb::DataBindings::const_iterator eIter = data->begin();
		while( !eIter.isFinished() ){
			kmb::ElementContainer::iterator element = body->find( eIter.getId() );
			if( !element.isFinished() ){
				appendElement( eIter.getId(), element );
			}
			++eIter;
		}
		break;
	}
	case kmb::DataBindings::FACEGROUP:
	case kmb::DataBindings::FACEVARIABLE:
	{
		kmb::DataBindings::const_iterator fIter = data->begin();
		while( !fIter.isFinished() ){
			kmb::Face f;
			if( fIter.getFace(f) ){
				kmb::ElementContainer::iterator element = body->find( f.getElementId() );
				if( !element.isFinished() ){
					this->appendFaceMap( f.getElementId(), element, f.getLocalFaceId() );
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

void
kmb::BoundaryExtractor::eraseData(const char* name,bool reverse)
{
	if( mesh == NULL ){
		return;
	}
	kmb::DataBindings* data = mesh->getDataBindingsPtr(name);
	if( data == NULL ){
		return;
	}
	eraseData( data, reverse );
}

void
kmb::BoundaryExtractor::eraseData(const kmb::DataBindings* data,bool reverse)
{
	if( mesh == NULL || data==NULL ){
		return;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr( data->getTargetBodyId() );
	if( body == NULL ){
		return;
	}
	switch( data->getBindingMode() ){
	case kmb::DataBindings::ELEMENTGROUP:
	{
		kmb::DataBindings::const_iterator eIter = data->begin();
		while( !eIter.isFinished() ){
			kmb::ElementContainer::iterator element = body->find( eIter.getId() );
			if( !element.isFinished() ){
				const int boundNum = element.getBoundaryCount();
				for(int i=0;i<boundNum;++i){
					this->eraseFaceMap( element, i, reverse );
				}
			}
			++eIter;
		}
		break;
	}
	case kmb::DataBindings::FACEGROUP:
	{
		kmb::DataBindings::const_iterator fIter = data->begin();
		kmb::Face f;
		while( !fIter.isFinished() ){
			fIter.getFace(f);
			kmb::ElementContainer::iterator element = body->find( f.getElementId() );
			if( !element.isFinished() ){
				this->eraseFaceMap( element, f.getLocalFaceId(), reverse );
			}
			++fIter;
		}
		break;
	}
	default:
		break;
	}
}


void
kmb::BoundaryExtractor::appendElement(kmb::elementIdType elementId,kmb::ElementBase &element,const kmb::ElementContainer* elements)
{
	kmb::elementType etype = element.getType();
	if( ( mesh || elements ) && kmb::Element::getDimension( etype ) >= 2 ){

		const int boundNum = element.getBoundaryCount();
		for(int i=0;i<boundNum;++i)
		{
			kmb::nodeIdType sum = 0;
			int vertexNum = kmb::ElementBase::getBoundaryVertexCount( etype, i );
			for(int j=0;j<vertexNum;++j)
			{
				sum += element.getBoundaryCellId(i,j);
			}
			if( facemap.find(sum) == facemap.end() )
			{
				facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,i) ) );
				continue;
			}
			int index = -1;
			int otherIndex = -1;

			std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.lower_bound(sum);
			std::multimap< kmb::nodeIdType, kmb::Face >::iterator end = facemap.upper_bound(sum);
			switch(vertexNum){
				case 2:
				{
					kmb::nodeIdType a0 = element.getBoundaryCellId(i,0);
					kmb::nodeIdType a1 = element.getBoundaryCellId(i,1);
					while( fIter != end )
					{
						kmb::ElementContainer::const_iterator eIter0 =
							(elements) ? elements->find( fIter->second.getElementId() ) :
							static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
						kmb::idType id0 = fIter->second.getLocalFaceId();
						if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 2 )
						{
							kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
							kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
							kmb::ElementRelation::relationType rel = kmb::ElementRelation::getSegmentRelation( a0,a1,b0,b1,index,otherIndex );
							if( rel == kmb::ElementRelation::REVERSE )
							{

								facemap.erase( fIter );
								goto findElement2;
							}
							else if( reverseMode && rel == kmb::ElementRelation::EQUAL )
							{

								facemap.erase( fIter );
								goto findElement2;
							}
						}
						++fIter;
					}

					facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,i) ) );
findElement2:
					break;
				}
				case 3:
				{
					kmb::nodeIdType a0 = element.getBoundaryCellId(i,0);
					kmb::nodeIdType a1 = element.getBoundaryCellId(i,1);
					kmb::nodeIdType a2 = element.getBoundaryCellId(i,2);
					while( fIter != end )
					{
						kmb::ElementContainer::const_iterator eIter0 =
							(elements) ? elements->find( fIter->second.getElementId() ) :
							static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
						kmb::idType id0 = fIter->second.getLocalFaceId();
						if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 3 )
						{
							kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
							kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
							kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
							kmb::ElementRelation::relationType rel = kmb::ElementRelation::getTriangleRelation( a0,a1,a2,b0,b1,b2,index,otherIndex );
							if( rel == kmb::ElementRelation::REVERSE )
							{

								facemap.erase( fIter );
								goto findElement3;
							}
							else if( reverseMode && rel == kmb::ElementRelation::EQUAL )
							{

								facemap.erase( fIter );
								goto findElement3;
							}
						}
						++fIter;
					}

					facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,i) ) );
findElement3:
					break;
				}
				case 4:
				{
					kmb::nodeIdType a0 = element.getBoundaryCellId(i,0);
					kmb::nodeIdType a1 = element.getBoundaryCellId(i,1);
					kmb::nodeIdType a2 = element.getBoundaryCellId(i,2);
					kmb::nodeIdType a3 = element.getBoundaryCellId(i,3);
					while( fIter != end )
					{
						kmb::ElementContainer::const_iterator eIter0 =
							(elements) ? elements->find( fIter->second.getElementId() ) :
							static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
						kmb::idType id0 = fIter->second.getLocalFaceId();
						if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 4 )
						{
							kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
							kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
							kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
							kmb::nodeIdType b3 = eIter0.getBoundaryCellId(id0,3);
							kmb::ElementRelation::relationType rel = kmb::ElementRelation::getQuadRelation( a0,a1,a2,a3,b0,b1,b2,b3,index,otherIndex );
							if( rel == kmb::ElementRelation::REVERSE )
							{

								facemap.erase( fIter );
								goto findElement4;
							}
							else if( reverseMode && rel == kmb::ElementRelation::EQUAL )
							{

								facemap.erase( fIter );
								goto findElement4;
							}
						}
						++fIter;
					}

					facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,i) ) );
findElement4:
					break;
				}
				default:
					break;
			}
		}
	}
}


void
kmb::BoundaryExtractor::appendFaceMap(kmb::elementIdType elementId,kmb::ElementBase &element,kmb::idType faceId,const kmb::ElementContainer* elements)
{
	kmb::elementType etype = element.getType();
	if( ( mesh || elements ) && kmb::Element::getDimension( etype ) >= 2 ){

		kmb::nodeIdType sum = 0;
		int vertexNum = kmb::ElementBase::getBoundaryVertexCount( etype, static_cast<int>(faceId) );
		for(int j=0;j<vertexNum;++j)
		{
			sum += element.getBoundaryCellId(static_cast<int>(faceId),j);
		}
		if( facemap.find(sum) == facemap.end() )
		{

			facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,faceId) ) );
			return;
		}
		int index = -1;
		int otherIndex = -1;

		std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.lower_bound(sum);
		std::multimap< kmb::nodeIdType, kmb::Face >::iterator end = facemap.upper_bound(sum);
		switch(vertexNum){
			case 2:
			{
				kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
				kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
				while( fIter != end )
				{
					kmb::ElementContainer::const_iterator eIter0 =
						(elements) ? elements->find( fIter->second.getElementId() ) :
						static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
					kmb::idType id0 = fIter->second.getLocalFaceId();
					if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 2 )
					{
						kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
						kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
						kmb::ElementRelation::relationType rel = kmb::ElementRelation::getSegmentRelation( a0,a1,b0,b1,index,otherIndex );
						if( rel== kmb::ElementRelation::REVERSE )
						{

							facemap.erase( fIter );
							goto findElement2;
						}
						else if( reverseMode && rel == kmb::ElementRelation::EQUAL )
						{

							facemap.erase( fIter );
							goto findElement2;
						}
					}
					++fIter;
				}

				facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,faceId) ) );
findElement2:
				break;
			}
			case 3:
			{
				kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
				kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
				kmb::nodeIdType a2 = element.getBoundaryCellId(faceId,2);
				while( fIter != end )
				{
					kmb::ElementContainer::const_iterator eIter0 =
						(elements) ? elements->find( fIter->second.getElementId() ) :
						static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
					kmb::idType id0 = fIter->second.getLocalFaceId();
					if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 3 )
					{
						kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
						kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
						kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
						kmb::ElementRelation::relationType rel = kmb::ElementRelation::getTriangleRelation( a0,a1,a2,b0,b1,b2,index,otherIndex );
						if( rel == kmb::ElementRelation::REVERSE )
						{

							facemap.erase( fIter );
							goto findElement3;
						}
						else if( reverseMode && rel == kmb::ElementRelation::EQUAL )
						{

							facemap.erase( fIter );
							goto findElement3;
						}
					}
					++fIter;
				}

				facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,faceId) ) );
findElement3:
				break;
			}
			case 4:
			{
				kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
				kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
				kmb::nodeIdType a2 = element.getBoundaryCellId(faceId,2);
				kmb::nodeIdType a3 = element.getBoundaryCellId(faceId,3);
				while( fIter != end )
				{
					kmb::ElementContainer::const_iterator eIter0 =
						(elements) ? elements->find( fIter->second.getElementId() ) :
						static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
					kmb::idType id0 = fIter->second.getLocalFaceId();
					if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 4 )
					{
						kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
						kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
						kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
						kmb::nodeIdType b3 = eIter0.getBoundaryCellId(id0,3);
						kmb::ElementRelation::relationType rel = kmb::ElementRelation::getQuadRelation( a0,a1,a2,a3,b0,b1,b2,b3,index,otherIndex );
						if( rel == kmb::ElementRelation::REVERSE )
						{

							facemap.erase( fIter );
							goto findElement4;
						}
						else if( reverseMode && rel == kmb::ElementRelation::EQUAL )
						{

							facemap.erase( fIter );
							goto findElement4;
						}
					}
					++fIter;
				}

				facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,kmb::Face(elementId,faceId) ) );
findElement4:
				break;
			}
			default:
				break;
		}
	}
}

void
kmb::BoundaryExtractor::eraseFaceMap(kmb::ElementBase &element,kmb::idType faceId,bool reverse,const kmb::ElementContainer* elements)
{
	kmb::elementType etype = element.getType();
	if( ( mesh || elements ) && kmb::Element::getDimension( etype ) >= 2 ){

		kmb::nodeIdType sum = 0;
		int vertexNum = kmb::ElementBase::getBoundaryVertexCount( etype, static_cast<int>(faceId) );
		for(int j=0;j<vertexNum;++j)
		{
			sum += element.getBoundaryCellId(static_cast<int>(faceId),j);
		}
		if( facemap.find(sum) == facemap.end() )
		{

			return;
		}

		int index = -1;
		int otherIndex = -1;
		std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.lower_bound(sum);
		std::multimap< kmb::nodeIdType, kmb::Face >::iterator end = facemap.upper_bound(sum);
		switch(vertexNum){
			case 2:
			{
				kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
				kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
				while( fIter != end )
				{
					kmb::ElementContainer::const_iterator eIter0 =
						(elements) ? elements->find( fIter->second.getElementId() ) :
						static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
					kmb::idType id0 = fIter->second.getLocalFaceId();
					if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 2 )
					{
						kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
						kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
						kmb::ElementRelation::relationType rel = kmb::ElementRelation::getSegmentRelation( a0,a1,b0,b1,index,otherIndex );
						if( reverse && rel == kmb::ElementRelation::REVERSE )
						{

							facemap.erase( fIter );
							return;
						}
						if( !reverse && rel == kmb::ElementRelation::EQUAL )
						{

							facemap.erase( fIter );
							return;
						}
					}
					++fIter;
				}
				break;
			}
			case 3:
			{
				kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
				kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
				kmb::nodeIdType a2 = element.getBoundaryCellId(faceId,2);
				while( fIter != end )
				{
					kmb::ElementContainer::const_iterator eIter0 =
						(elements) ? elements->find( fIter->second.getElementId() ) :
						static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
					kmb::idType id0 = fIter->second.getLocalFaceId();
					if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 3 )
					{
						kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
						kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
						kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
						kmb::ElementRelation::relationType rel = kmb::ElementRelation::getTriangleRelation( a0,a1,a2,b0,b1,b2,index,otherIndex );
						if( reverse && rel == kmb::ElementRelation::REVERSE )
						{

							facemap.erase( fIter );
							return;
						}
						if( !reverse && rel == kmb::ElementRelation::EQUAL )
						{

							facemap.erase( fIter );
							return;
						}
					}
					++fIter;
				}
				break;
			}
			case 4:
			{
				kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
				kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
				kmb::nodeIdType a2 = element.getBoundaryCellId(faceId,2);
				kmb::nodeIdType a3 = element.getBoundaryCellId(faceId,3);
				while( fIter != end )
				{
					kmb::ElementContainer::const_iterator eIter0 =
						(elements) ? elements->find( fIter->second.getElementId() ) :
						static_cast<const kmb::MeshData*>(mesh)->findElement( fIter->second.getElementId() );
					kmb::idType id0 = fIter->second.getLocalFaceId();
					if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 4 )
					{
						kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
						kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
						kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
						kmb::nodeIdType b3 = eIter0.getBoundaryCellId(id0,3);
						kmb::ElementRelation::relationType rel = kmb::ElementRelation::getQuadRelation( a0,a1,a2,a3,b0,b1,b2,b3,index,otherIndex );
						if( reverse && rel == kmb::ElementRelation::REVERSE )
						{

							facemap.erase( fIter );
							return;
						}
						if( !reverse && rel == kmb::ElementRelation::EQUAL )
						{

							facemap.erase( fIter );
							return;
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
}

bool
kmb::BoundaryExtractor::isClosed(kmb::bodyIdType bodyId)
{
	kmb::ElementContainer* body = NULL;
	if( mesh == NULL ||
		(body = mesh->getBodyPtr( bodyId )) == NULL)
	{
		return false;
	}

	std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.begin();
	while( fIter != facemap.end() )
	{
		kmb::ElementContainer::iterator eIter = body->find( fIter->second.getElementId() );
		if( !eIter.isFinished() ){
			return false;
		}
		++fIter;
	}
	return true;
}

kmb::bodyIdType
kmb::BoundaryExtractor::getBoundary(kmb::bodyIdType bodyId)
{
	if( mesh == NULL ){
		return kmb::Body::nullBodyId;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr( bodyId );
	if( body == NULL ){
		return kmb::Body::nullBodyId;
	}
	kmb::nodeIdType* cell = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT_DIM2 ];

	size_t bCount = 0;
	{
		std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.begin();
		while( fIter != facemap.end() )
		{
			if( ! body->find( fIter->second.getElementId() ).isFinished() ){
				++bCount;
			}
			++fIter;
		}
	}

	kmb::bodyIdType boundaryId = mesh->beginElement( bCount );
	{
		std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.begin();
		while( fIter != facemap.end() )
		{
			kmb::elementIdType orgElementId = fIter->second.getElementId();
			kmb::ElementContainer::iterator eIter = body->find( orgElementId );
			if( !eIter.isFinished() ){
				kmb::elementType etype = eIter.getBoundaryElement( fIter->second.getLocalFaceId(), cell );
				kmb::elementIdType elementId = mesh->addElement( etype, cell );
				mesh->deriveTargetData( elementId, orgElementId );
			}
			++fIter;
		}
	}
	mesh->endElement();

	delete[] cell;
	return boundaryId;
}

kmb::bodyIdType
kmb::BoundaryExtractor::getBoundary(void)
{
	if( mesh == NULL ){
		return kmb::Body::nullBodyId;
	}
	kmb::nodeIdType* cell = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT_DIM2 ];

	kmb::bodyIdType boundaryId = mesh->beginElement( facemap.size() );
	{
		std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.begin();
		while( fIter != facemap.end() )
		{
			kmb::elementIdType orgElementId = fIter->second.getElementId();
			kmb::ElementContainer::const_iterator eIter = mesh->findElement( orgElementId );
			if( !eIter.isFinished() ){
				kmb::elementType etype = eIter.getBoundaryElement( fIter->second.getLocalFaceId(), cell );
				kmb::elementIdType elementId = mesh->addElement( etype, cell );
				mesh->deriveTargetData( elementId, orgElementId );
			}
			++fIter;
		}
	}
	mesh->endElement();
	delete[] cell;
	return boundaryId;
}

int
kmb::BoundaryExtractor::getBoundaryComponents(kmb::bodyIdType bodyId,kmb::bodyIdType* &boundaryIds)
{
	if( mesh == NULL ){
		return 0;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr( bodyId );
	if( body == NULL ){
		return 0;
	}

	kmb::nodeIdType cell[8];
	kmb::ElementContainerMap* boundaryBody = NULL;
	boundaryBody = new kmb::ElementContainerMap();
	std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.begin();
	while( fIter != facemap.end() )
	{
		kmb::ElementContainer::iterator eIter = body->find( fIter->second.getElementId() );
		if( !eIter.isFinished() ){
			kmb::elementType etype = eIter.getBoundaryElement( fIter->second.getLocalFaceId(), cell );
			boundaryBody->addElement( etype, cell );
		}
		++fIter;
	}

	kmb::SurfaceOperation surfOp;
	std::set<kmb::ElementContainer*> components;
	surfOp.decomposeByElementContainer( boundaryBody, components );
	int i = 0;
	int size = static_cast<int>( components.size() );
	boundaryIds = new kmb::bodyIdType[ size ];
	std::set<kmb::ElementContainer*>::iterator cIter = components.begin();

	while( cIter != components.end() )
	{
		kmb::ElementContainer* container = (*cIter);
		++cIter;
		if( container != NULL ){
			boundaryIds[i] = mesh->appendBody( container );
		}else{
			boundaryIds[i] = kmb::Body::nullBodyId;
		}
		++i;
	}
	components.clear();
	delete boundaryBody;
	return size;
}

bool
kmb::BoundaryExtractor::getBoundaryFace(kmb::bodyIdType bodyId,const char* name)
{
	kmb::ElementContainer* body = NULL;
	kmb::DataBindings* bface = NULL;
	if( mesh == NULL ||
		(body=mesh->getBodyPtr( bodyId )) == NULL ||
		name == NULL )
	{
		return false;
	}


	bface = mesh->getDataBindingsPtr( name );
	if( bface == NULL ){

		bface = mesh->createDataBindings(name,kmb::DataBindings::FACEGROUP,kmb::PhysicalValue::NONE,"Brep",bodyId);
	}else if( bface->getBindingMode() != kmb::DataBindings::FACEGROUP ){

		return false;
	}

	std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.begin();
	while( fIter != facemap.end() )
	{
		if( body->includeElement( fIter->second.getElementId() ) ){
			bface->addId( fIter->second );
		}
		++fIter;
	}

	return true;
}

size_t
kmb::BoundaryExtractor::getBoundary(const kmb::ElementContainer* parent,kmb::ElementContainer* boundary)
{
	if( parent == NULL || boundary == NULL ){
		return 0;
	}
	size_t boundaryElementCount = facemap.size();
	boundary->initialize( boundaryElementCount );
	std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.begin();
	kmb::nodeIdType nodes[8];
	while( fIter != facemap.end() )
	{
		kmb::ElementContainer::const_iterator eIter = parent->find( fIter->second.getElementId() );
		int faceIndex = fIter->second.getLocalFaceId();
		if( !eIter.isFinished() ){
			kmb::elementType etype = eIter.getType();
			int boundaryNodeCount = kmb::ElementBase::getBoundaryNodeCount( etype, faceIndex );
			for(int i=0;i<boundaryNodeCount;++i){
				nodes[i] = eIter.getBoundaryCellId( faceIndex, i );
			}
			boundary->addElement( eIter.getBoundaryType(faceIndex), nodes );
		}
		++fIter;
	}
	return boundaryElementCount;
}
