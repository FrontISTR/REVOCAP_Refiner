/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Face                                                    #
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
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementContainer.h"

kmb::Face::Face(void)
: elementId( kmb::Element::nullElementId )
, localFaceId( -1 )
{
}

kmb::Face::~Face(void)
{
}

kmb::Face::Face(kmb::elementIdType elementId, kmb::idType localFaceId)
{
	this->elementId = elementId;
	this->localFaceId = localFaceId;
}

kmb::Face::Face(const kmb::Face &f)
{
	*this = f;
}

kmb::Face&
kmb::Face::operator=(const kmb::Face &other)
{
	this->elementId = other.elementId;
	this->localFaceId = other.localFaceId;
	return *this;
}

void
kmb::Face::setId(kmb::elementIdType elementId,kmb::idType localId)
{
	this->elementId = elementId;
	this->localFaceId = localId;
}

kmb::Element*
kmb::Face::createElement(const kmb::ElementContainer* body) const
{
	if( body ){
		kmb::ElementContainer::const_iterator elem = body->find( this->elementId );
		if( !elem.isFinished() ){
			return createElement( elem );
		}
	}
	return NULL;
}

kmb::Element*
kmb::Face::createElement(const kmb::MeshData* mesh) const
{
	if( mesh ){
		kmb::ElementContainer::const_iterator elem = mesh->findElement( this->elementId );
		if( !elem.isFinished() ){
			return createElement( elem );
		}
	}
	return NULL;
}

kmb::Element*
kmb::Face::createElement(const kmb::ElementBase &elem) const
{
	if( this->localFaceId < -1 ){
		return NULL;
	}else if( this->localFaceId == -1 ){
		const int len = elem.getNodeCount();
		kmb::nodeIdType* cell = new kmb::nodeIdType[len];
		for(int i=0;i<len;++i){
			cell[i] = elem.getCellId(i);
		}
		kmb::Element* clone = kmb::Element::create( elem.getType(), cell );
		if( clone ){
			return clone;
		}else{
			delete cell;
		}
		return NULL;
	}else if( this->localFaceId < elem.getBoundaryCount() ){
		const int len = elem.getBoundaryNodeCount( this->localFaceId );
		kmb::nodeIdType* cell = new kmb::nodeIdType[len];
		kmb::elementType etype = elem.getBoundaryElement( this->localFaceId, cell );
		kmb::Element* boundary = kmb::Element::create( etype, cell );
		if( boundary ){
			return boundary;
		}else{
			delete cell;
		}
	}else if( elem.getDimension() == 3 && this->localFaceId < elem.getBoundaryCount() + elem.getEdgeCount() ){
		const int edgeId = this->localFaceId - elem.getBoundaryCount();
		const int len = elem.getEdgeNodeCount( edgeId );
		kmb::nodeIdType* cell = new kmb::nodeIdType[len];
		kmb::elementType etype = elem.getEdgeElement( this->localFaceId, cell );
		kmb::Element* edge = kmb::Element::create( etype, cell );
		if( edge ){
			return edge;
		}else{
			delete cell;
		}
	}
	return NULL;
}

bool
kmb::Face::getFaceElement(const kmb::ElementContainer* body,kmb::ElementBase &element) const
{
	if( body ){
		kmb::ElementContainer::const_iterator elem = body->find( this->elementId );
		if( !elem.isFinished() ){
			return getFaceElement( elem, element );
		}
	}
	return false;
}

bool
kmb::Face::getFaceElement(const kmb::MeshData* mesh,kmb::ElementBase &element) const
{
	if( mesh ){
		kmb::ElementContainer::const_iterator elem = mesh->findElement( this->elementId );
		if( !elem.isFinished() ){
			return getFaceElement( elem, element );
		}
	}
	return false;
}

bool
kmb::Face::getFaceElement(const kmb::ElementBase &parent,kmb::ElementBase &output) const
{
	if( this->localFaceId < -1 ){
		return false;
	}else if( this->localFaceId == -1 ){
		const int len = parent.getNodeCount();
		for(int i=0;i<len;++i){
			output.setCellId(i,parent.getCellId(i));
		}
		return true;
	}else if( this->localFaceId < parent.getBoundaryCount() ){
		const int len = parent.getBoundaryNodeCount( this->localFaceId );
		for(int i=0;i<len;++i){
			output.setCellId(i,parent.getBoundaryCellId(this->localFaceId,i));
		}
		return true;
	}else if( parent.getDimension() == 3 && this->localFaceId < parent.getBoundaryCount() + parent.getEdgeCount() ){
		const int edgeId = this->localFaceId - parent.getBoundaryCount();
		const int len = parent.getEdgeNodeCount( edgeId );
		for(int i=0;i<len;++i){
			output.setCellId(i,parent.getEdgeCellId(edgeId,i));
		}
		return true;
	}
	return false;
}

kmb::elementType
kmb::Face::getFaceElementType(const kmb::ElementContainer* body) const
{
	if( body ){
		kmb::ElementContainer::const_iterator elem = body->find( this->elementId );
		if( !elem.isFinished() ){
			return getFaceElementType( elem );
		}
	}
	return kmb::UNKNOWNTYPE;
}

kmb::elementType
kmb::Face::getFaceElementType(const kmb::MeshData* mesh) const
{
	if( mesh ){
		kmb::ElementContainer::const_iterator elem = mesh->findElement( this->elementId );
		if( !elem.isFinished() ){
			return getFaceElementType( elem );
		}
	}
	return kmb::UNKNOWNTYPE;
}

kmb::elementType
kmb::Face::getFaceElementType(const kmb::ElementBase &parent) const
{
	if( this->localFaceId < -1 ){
		return kmb::UNKNOWNTYPE;
	}else if( this->localFaceId == -1 ){
		return parent.getType();
	}else if( this->localFaceId < parent.getBoundaryCount() ){
		return parent.getBoundaryType( this->localFaceId );
	}else if( parent.getDimension() == 3 && this->localFaceId < parent.getBoundaryCount() + parent.getEdgeCount() ){
		const int edgeId = this->localFaceId - parent.getBoundaryCount();
		return parent.getEdgeType( edgeId );
	}
	return kmb::UNKNOWNTYPE;
}

bool
kmb::Face::getFaceLinearElement(const kmb::ElementContainer* body,kmb::ElementBase &element) const
{
	if( body ){
		kmb::ElementContainer::const_iterator elem = body->find( this->elementId );
		if( !elem.isFinished() ){
			return getFaceLinearElement( elem, element );
		}
	}
	return false;
}

bool
kmb::Face::getFaceLinearElement(const kmb::MeshData* mesh,kmb::ElementBase &element) const
{
	if( mesh ){
		kmb::ElementContainer::const_iterator elem = mesh->findElement( this->elementId );
		if( !elem.isFinished() ){
			return getFaceLinearElement( elem, element );
		}
	}
	return false;
}

bool
kmb::Face::getFaceLinearElement(const kmb::ElementBase &parent,kmb::ElementBase &output) const
{
	if( this->localFaceId < -1 ){
		return false;
	}else if( this->localFaceId == -1 ){
		const int len = parent.getVertexCount();
		for(int i=0;i<len;++i){
			output.setCellId(i,parent.getCellId(i));
		}
		return true;
	}else if( this->localFaceId < parent.getBoundaryCount() ){
		const int len = parent.getBoundaryVertexCount( this->localFaceId );
		for(int i=0;i<len;++i){
			output.setCellId(i,parent.getBoundaryCellId(this->localFaceId,i));
		}
		return true;
	}else if( parent.getDimension() == 3 && this->localFaceId < parent.getBoundaryCount() + parent.getEdgeCount() ){
		const int edgeId = this->localFaceId - parent.getBoundaryCount();
		const int len = parent.getEdgeVertexCount( edgeId );
		for(int i=0;i<len;++i){
			output.setCellId(i,parent.getEdgeCellId(edgeId,i));
		}
		return true;
	}
	return false;
}

kmb::elementType
kmb::Face::getFaceLinearElementType(const kmb::ElementContainer* body) const
{
	if( body ){
		kmb::ElementContainer::const_iterator elem = body->find( this->elementId );
		if( !elem.isFinished() ){
			return getFaceLinearElementType( elem );
		}
	}
	return kmb::UNKNOWNTYPE;
}

kmb::elementType
kmb::Face::getFaceLinearElementType(const kmb::MeshData* mesh) const
{
	if( mesh ){
		kmb::ElementContainer::const_iterator elem = mesh->findElement( this->elementId );
		if( !elem.isFinished() ){
			return getFaceLinearElementType( elem );
		}
	}
	return kmb::UNKNOWNTYPE;
}

kmb::elementType
kmb::Face::getFaceLinearElementType(const kmb::ElementBase &parent) const
{
	return kmb::Element::getLinearType( getFaceElementType( parent ) );
}

int
kmb::Face::getNodeCount(const kmb::ElementContainer* body) const
{
	if( body ){
		kmb::ElementContainer::const_iterator eIter = body->find( this->elementId );
		if( !eIter.isFinished() ){
			return eIter.getBoundaryNodeCount( this->localFaceId );
		}
	}
	return 0;
}

int
kmb::Face::getNodeCount(const kmb::MeshData* mesh) const
{
	if( mesh ){
		kmb::ElementContainer::const_iterator eIter = mesh->findElement( this->elementId );
		if( !eIter.isFinished() ){
			return eIter.getBoundaryNodeCount( this->localFaceId );
		}
	}
	return 0;
}

int
kmb::Face::getNodeCount(const kmb::ElementBase &parent) const
{
	if( this->localFaceId < -1 ){
		return 0;
	}else if( this->localFaceId == -1 ){
		return parent.getNodeCount();
	}else if( this->localFaceId < parent.getBoundaryCount() ){
		return parent.getBoundaryNodeCount( this->localFaceId );
	}else if( parent.getDimension() == 3 && this->localFaceId < parent.getBoundaryCount() + parent.getEdgeCount() ){
		const int edgeId = this->localFaceId - parent.getBoundaryCount();
		return parent.getEdgeNodeCount( edgeId );
	}
	return 0;
}
