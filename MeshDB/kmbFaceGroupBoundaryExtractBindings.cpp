/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FaceGroupBoundaryExtractBindings                        #
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
#include "MeshDB/kmbFaceGroupBoundaryExtractBindings.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbElementRelation.h"

kmb::FaceGroupBoundaryExtractBindings::FaceGroupBoundaryExtractBindings(void)
: kmb::DataBindings()
, mesh(NULL)
{
}

kmb::FaceGroupBoundaryExtractBindings::~FaceGroupBoundaryExtractBindings(void)
{
}

const char*
kmb::FaceGroupBoundaryExtractBindings::getContainerType(void) const
{
	return "FaceGroupBoundaryExtractBindings";
}

void
kmb::FaceGroupBoundaryExtractBindings::clear(void)
{
	facemap.clear();
}

size_t
kmb::FaceGroupBoundaryExtractBindings::getIdCount(void) const
{
	return facemap.size();
}

bool
kmb::FaceGroupBoundaryExtractBindings::addId(kmb::Face f)
{
	if( mesh ){
		kmb::ElementContainer::const_iterator element = mesh->findElement(f.getElementId());
		if( !element.isFinished() ){
			kmb::elementType etype = element.getType();
			kmb::idType faceId = f.getLocalFaceId();
			kmb::nodeIdType sum = 0;
			int vertexNum = kmb::ElementBase::getBoundaryVertexCount( etype, static_cast<int>(faceId) );
			for(int j=0;j<vertexNum;++j)
			{
				sum += element.getBoundaryCellId(static_cast<int>(faceId),j);
			}
			facemap.insert( std::pair<kmb::nodeIdType,kmb::Face>(sum,f) );
			return true;
		}
	}
	return false;
}

bool
kmb::FaceGroupBoundaryExtractBindings::hasId(kmb::Face f) const
{
	if( mesh ){
		kmb::ElementContainer::const_iterator element = mesh->findElement(f.getElementId());
		if( !element.isFinished() ){
			kmb::elementType etype = element.getType();
			kmb::idType faceId = f.getLocalFaceId();
			kmb::nodeIdType sum = 0;
			int vertexNum = kmb::ElementBase::getBoundaryVertexCount( etype, static_cast<int>(faceId) );
			for(int j=0;j<vertexNum;++j)
			{
				sum += element.getBoundaryCellId(static_cast<int>(faceId),j);
			}
			std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator fIter = facemap.lower_bound(sum);
			std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator end = facemap.upper_bound(sum);
			while( fIter != end )
			{
				if( fIter->second == f ){
					return true;
				}
				++fIter;
			}
		}
	}
	return false;
}

bool
kmb::FaceGroupBoundaryExtractBindings::deleteId(kmb::Face f)
{
	if( mesh ){
		kmb::ElementContainer::const_iterator element = mesh->findElement(f.getElementId());
		if( !element.isFinished() ){
			kmb::elementType etype = element.getType();
			kmb::idType faceId = f.getLocalFaceId();
			kmb::nodeIdType sum = 0;
			int vertexNum = kmb::ElementBase::getBoundaryVertexCount( etype, static_cast<int>(faceId) );
			for(int j=0;j<vertexNum;++j)
			{
				sum += element.getBoundaryCellId(static_cast<int>(faceId),j);
			}
			std::multimap< kmb::nodeIdType, kmb::Face >::iterator fIter = facemap.lower_bound(sum);
			std::multimap< kmb::nodeIdType, kmb::Face >::iterator end = facemap.upper_bound(sum);
			while( fIter != end )
			{
				if( fIter->second == f ){
					facemap.erase( fIter );
					return true;
				}
				++fIter;
			}
		}
	}
	return false;
}

bool
kmb::FaceGroupBoundaryExtractBindings::replaceId(kmb::Face old_f,kmb::Face new_f)
{
	if( deleteId(old_f) ){
		return addId(new_f);
	}
	return false;
}

kmb::DataBindings::iterator
kmb::FaceGroupBoundaryExtractBindings::begin(void)
{
	if( facemap.size() == 0 ){
		return kmb::DataBindings::iterator(NULL);
	}
	kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE* _it = NULL;
	_it = new kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE();
	_it->fIter = this->facemap.begin();
	_it->endIter = this->facemap.end();
	return kmb::DataBindings::iterator(_it);
}

kmb::DataBindings::const_iterator
kmb::FaceGroupBoundaryExtractBindings::begin(void) const
{
	if( facemap.size() == 0 ){
		return kmb::DataBindings::iterator(NULL);
	}
	kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE* _it = NULL;
	_it = new kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE();
	_it->fIter = this->facemap.begin();
	_it->endIter = this->facemap.end();
	return kmb::DataBindings::iterator(_it);
}

/*---------------- iterator ---------------------------*/

bool
kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE::getFace(kmb::Face &f) const
{
	f = fIter->second;
	return true;
}

kmb::DataBindings::_iterator*
kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE::operator++(void)
{
	++fIter;
	if( fIter != endIter ){
		return this;
	}else{
		return NULL;
	}
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif
#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif
kmb::DataBindings::_iterator*
kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE::operator++(int n)
{
	fIter++;
	if( fIter != endIter ){
		return this;
	}else{
		return NULL;
	}
}
#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

kmb::DataBindings::_iterator*
kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE::clone(void)
{
	kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE* _it = NULL;
	_it = new kmb::FaceGroupBoundaryExtractBindings::_iteratorFGBE();
	_it->fIter = this->fIter;
	_it->endIter = this->endIter;
	return _it;
}



void
kmb::FaceGroupBoundaryExtractBindings::setMesh(kmb::MeshData* mesh)
{
	this->mesh = mesh;
}

size_t
kmb::FaceGroupBoundaryExtractBindings::appendBody(kmb::bodyIdType bodyId)
{
	size_t count = 0;
	if( mesh == NULL ){
		return count;
	}
	kmb::ElementContainer* body = mesh->getBodyPtr( bodyId );
	if( body == NULL ){
		return count;
	}

	kmb::ElementContainer::const_iterator eIter = body->begin();
	while( !eIter.isFinished() ){
		kmb::elementIdType elementId = eIter.getId();
		int bCount = eIter.getBoundaryCount();
		for(int i=0;i<bCount;++i){
			kmb::idType localId = static_cast<kmb::idType>(i);
			addId( kmb::Face(elementId,localId) );
		}
		++eIter;
	}
	return count;
}

kmb::Face
kmb::FaceGroupBoundaryExtractBindings::getNeighbor(kmb::Face f,bool reverse) const
{
	kmb::Face otherFace;
	if( mesh==NULL ){
		return otherFace;
	}
	kmb::ElementContainer::const_iterator element = mesh->findElement(f.getElementId());
	if( element.isFinished() ){
		return otherFace;
	}
	kmb::elementType etype = element.getType();
	kmb::idType faceId = f.getLocalFaceId();
	kmb::nodeIdType sum = 0;
	int vertexNum = kmb::ElementBase::getBoundaryVertexCount( etype, static_cast<int>(faceId) );
	for(int j=0;j<vertexNum;++j)
	{
		sum += element.getBoundaryCellId(static_cast<int>(faceId),j);
	}
	int index = -1;
	int otherIndex = -1;

	std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator fIter = facemap.lower_bound(sum);
	std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator end = facemap.upper_bound(sum);
	switch(vertexNum){
		case 2:{
			kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
			kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
			while( fIter != end )
			{
				kmb::elementIdType elemId0 = fIter->second.getElementId();
				kmb::ElementContainer::const_iterator eIter0 = mesh->findElement( elemId0 );
				kmb::idType id0 = fIter->second.getLocalFaceId();
				if( !eIter0.isFinished() && eIter0.getBoundaryVertexCount(id0) == 2 )
				{
					kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
					kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
					kmb::ElementRelation::relationType rel = kmb::ElementRelation::getSegmentRelation( a0,a1,b0,b1,index,otherIndex );
					if( rel== kmb::ElementRelation::REVERSE )
					{
						otherFace.setId( elemId0, id0 );
						break;
					}
					else if( reverse && rel == kmb::ElementRelation::EQUAL )
					{
						otherFace.setId( elemId0, id0 );
						break;
					}
				}
				++fIter;
			}
			break;
		}
		case 3:{
			kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
			kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
			kmb::nodeIdType a2 = element.getBoundaryCellId(faceId,2);
			while( fIter != end )
			{
				kmb::elementIdType elemId0 = fIter->second.getElementId();
				kmb::ElementContainer::const_iterator eIter0 = mesh->findElement( elemId0 );
				kmb::idType id0 = fIter->second.getLocalFaceId();
				if( !eIter0.isFinished() && eIter0.getBoundaryVertexCount(id0) == 3 )
				{
					kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
					kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
					kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
					kmb::ElementRelation::relationType rel = kmb::ElementRelation::getTriangleRelation( a0,a1,a2,b0,b1,b2,index,otherIndex );
					if( rel== kmb::ElementRelation::REVERSE )
					{
						otherFace.setId( elemId0, id0 );
						break;
					}
					else if( reverse && rel == kmb::ElementRelation::EQUAL )
					{
						otherFace.setId( elemId0, id0 );
						break;
					}
				}
				++fIter;
			}
			break;
		}
		case 4:{
			kmb::nodeIdType a0 = element.getBoundaryCellId(faceId,0);
			kmb::nodeIdType a1 = element.getBoundaryCellId(faceId,1);
			kmb::nodeIdType a2 = element.getBoundaryCellId(faceId,2);
			kmb::nodeIdType a3 = element.getBoundaryCellId(faceId,3);
			while( fIter != end )
			{
				kmb::elementIdType elemId0 = fIter->second.getElementId();
				kmb::ElementContainer::const_iterator eIter0 = mesh->findElement( elemId0 );
				kmb::idType id0 = fIter->second.getLocalFaceId();
				if( !eIter0.isFinished() && kmb::ElementBase::getBoundaryVertexCount( eIter0.getType(), id0 ) == 4 )
				{
					kmb::nodeIdType b0 = eIter0.getBoundaryCellId(id0,0);
					kmb::nodeIdType b1 = eIter0.getBoundaryCellId(id0,1);
					kmb::nodeIdType b2 = eIter0.getBoundaryCellId(id0,2);
					kmb::nodeIdType b3 = eIter0.getBoundaryCellId(id0,3);
					kmb::ElementRelation::relationType rel = kmb::ElementRelation::getQuadRelation( a0,a1,a2,a3,b0,b1,b2,b3,index,otherIndex );
					if( rel== kmb::ElementRelation::REVERSE )
					{
						otherFace.setId( elemId0, id0 );
						break;
					}
					else if( reverse && rel == kmb::ElementRelation::EQUAL )
					{
						otherFace.setId( elemId0, id0 );
						break;
					}
				}
				++fIter;
			}
			break;
		}
		default:
			break;
	}
	return otherFace;
}

