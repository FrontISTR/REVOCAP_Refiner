/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Element                                                 #
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
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbSegment2.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbTriangle2.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbQuad2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbTetrahedron2.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbPyramid2.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbWedge2.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbHexahedron2.h"
#include "MeshDB/kmbQuadInterface.h"
#include "MeshDB/kmbTriInterface.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"
#include "Common/kmbCalculator.h"

kmb::elementIdType kmb::Element::nullElementId = -1;

const int kmb::Element::MAX_NODE_COUNT = 20;
const int kmb::Element::MAX_NODE_COUNT_DIM2 = 8;

const std::string
kmb::ElementBase::getTypeString(elementType type)
{
	switch(type){
		case kmb::UNKNOWNTYPE:
			return std::string("UNKNOWNTYPE");
		case kmb::SEGMENT:
			return std::string("SEGMENT");
		case kmb::SEGMENT2:
			return std::string("SEGMENT2");
		case kmb::TRIANGLE:
			return std::string("TRIANGLE");
		case kmb::TRIANGLE2:
			return std::string("TRIANGLE2");
		case kmb::QUAD:
			return std::string("QUAD");
		case kmb::QUAD2:
			return std::string("QUAD2");
		case kmb::TETRAHEDRON:
			return std::string("TETRAHEDRON");
		case kmb::TETRAHEDRON2:
			return std::string("TETRAHEDRON2");
		case kmb::PYRAMID:
			return std::string("PYRAMID");
		case kmb::PYRAMID2:
			return std::string("PYRAMID2");
		case kmb::WEDGE:
			return std::string("WEDGE");
		case kmb::WEDGE2:
			return std::string("WEDGE2");
		case kmb::HEXAHEDRON:
			return std::string("HEXAHEDRON");
		case kmb::HEXAHEDRON2:
			return std::string("HEXAHEDRON2");
		case kmb::QUADRILATERAL_INTERFACE:
			return std::string("QUAD_INTERFACE");
		case kmb::TRIANGLE_INTERFACE:
			return std::string("TRI_INTERFACE");
		default:
			return std::string("OTHERTYPE");
	}
}

kmb::elementType
kmb::ElementBase::getType(std::string str)
{
	if(str == "SEGMENT")
		return kmb::SEGMENT;
	else if(str == "SEGMENT2")
		return kmb::SEGMENT2;
	else if(str == "TRIANGLE" || str == "TRI")
		return kmb::TRIANGLE;
	else if(str == "TRIANGLE2" || str == "TRI2")
		return kmb::TRIANGLE2;
	else if(str == "QUAD" || str == "QUADRILATERAL")
		return kmb::QUAD;
	else if(str == "QUAD2" || str == "QUADRILATERAL2")
		return kmb::QUAD2;
	else if(str == "TETRAHEDRON")
		return kmb::TETRAHEDRON;
	else if(str == "TETRAHEDRON2")
		return kmb::TETRAHEDRON2;
	else if(str == "PYRAMID")
		return kmb::PYRAMID;
	else if(str == "PYRAMID2")
		return kmb::PYRAMID2;
	else if(str == "WEDGE")
		return kmb::WEDGE;
	else if(str == "WEDGE2")
		return kmb::WEDGE2;
	else if(str == "HEXAHEDRON")
		return kmb::HEXAHEDRON;
	else if(str == "HEXAHEDRON2")
		return kmb::HEXAHEDRON2;
	else if(str == "QUAD_INTERFACE")
		return kmb::QUADRILATERAL_INTERFACE;
	else if(str == "TRI_INTERFACE")
		return kmb::TRIANGLE_INTERFACE;
	else if(str == "OTHERTYPE")
		return kmb::OTHERTYPE;
	else
		return kmb::UNKNOWNTYPE;
}

int
kmb::ElementBase::getNodeCount(elementType type)
{
	switch(type){
		case kmb::SEGMENT:		return kmb::Segment::nodeCount;
		case kmb::SEGMENT2:		return kmb::Segment2::nodeCount;
		case kmb::TRIANGLE:		return kmb::Triangle::nodeCount;
		case kmb::TRIANGLE2:	return kmb::Triangle2::nodeCount;
		case kmb::QUAD:			return kmb::Quad::nodeCount;
		case kmb::QUAD2:		return kmb::Quad2::nodeCount;
		case kmb::TETRAHEDRON:	return kmb::Tetrahedron::nodeCount;
		case kmb::TETRAHEDRON2:	return kmb::Tetrahedron2::nodeCount;
		case kmb::PYRAMID:		return kmb::Pyramid::nodeCount;
		case kmb::PYRAMID2:		return kmb::Pyramid2::nodeCount;
		case kmb::WEDGE:		return kmb::Wedge::nodeCount;
		case kmb::WEDGE2:		return kmb::Wedge2::nodeCount;
		case kmb::HEXAHEDRON:	return kmb::Hexahedron::nodeCount;
		case kmb::HEXAHEDRON2:	return kmb::Hexahedron2::nodeCount;
		case kmb::QUADRILATERAL_INTERFACE:	return kmb::QuadInterface::nodeCount;
		case kmb::TRIANGLE_INTERFACE:	return kmb::TriInterface::nodeCount;
		case kmb::VERTEX:		return 1;
		default:				return -1;
	}
}

int
kmb::ElementBase::getVertexCount(elementType type)
{
	switch(type){
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return kmb::Segment::nodeCount;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			return kmb::Triangle::nodeCount;
		case kmb::QUAD:
		case kmb::QUAD2:
			return kmb::Quad::nodeCount;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			return kmb::Tetrahedron::nodeCount;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			return kmb::Pyramid::nodeCount;
		case kmb::WEDGE:
		case kmb::WEDGE2:
			return kmb::Wedge::nodeCount;
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			return kmb::Hexahedron::nodeCount;
		case kmb::QUADRILATERAL_INTERFACE:
			return kmb::QuadInterface::nodeCount;
		case kmb::TRIANGLE_INTERFACE:
			return kmb::TriInterface::nodeCount;
		case kmb::VERTEX:
			return 1;
		default:
			return -1;
	}
}

kmb::elementType
kmb::ElementBase::getLinearType(kmb::elementType type)
{
	switch(type){
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return kmb::SEGMENT;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			return kmb::TRIANGLE;
		case kmb::QUAD:
		case kmb::QUAD2:
			return kmb::QUAD;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			return kmb::TETRAHEDRON;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			return kmb::PYRAMID;
		case kmb::WEDGE:
		case kmb::WEDGE2:
			return kmb::WEDGE;
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			return kmb::HEXAHEDRON;
		case kmb::QUADRILATERAL_INTERFACE:
			return kmb::QUADRILATERAL_INTERFACE;
		case kmb::TRIANGLE_INTERFACE:
			return kmb::TRIANGLE_INTERFACE;
		default:
			return kmb::UNKNOWNTYPE;
	}
}

kmb::elementType
kmb::ElementBase::getSecondType(kmb::elementType type)
{
	switch(type){
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return kmb::SEGMENT2;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			return kmb::TRIANGLE2;
		case kmb::QUAD:
		case kmb::QUAD2:
			return kmb::QUAD2;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			return kmb::TETRAHEDRON2;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			return kmb::PYRAMID2;
		case kmb::WEDGE:
		case kmb::WEDGE2:
			return kmb::WEDGE2;
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			return kmb::HEXAHEDRON2;
		default:
			return kmb::UNKNOWNTYPE;
	}
}

int
kmb::ElementBase::getDiagIndex(kmb::elementType type,int index)
{
	switch(type){
	case kmb::UNKNOWNTYPE:
		return -1;
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
		switch(index){
		case 0:	return 1;
		case 1:	return 0;
		default:	return -1;
		}
	case kmb::TRIANGLE:
		return -1;
	case kmb::TRIANGLE2:
		switch(index){
		case 0:	return 3;
		case 1:	return 4;
		case 2:	return 5;
		case 3:	return 0;
		case 4:	return 1;
		case 5:	return 2;
		default:	return -1;
		}
	case kmb::QUAD:
	case kmb::QUAD2:
		switch(index){
		case 0:	return 2;
		case 1:	return 3;
		case 2:	return 0;
		case 3:	return 1;
		case 4:	return 6;
		case 5:	return 7;
		case 6:	return 4;
		case 7:	return 5;
		default:	return -1;
		}
	case kmb::TETRAHEDRON:
	case kmb::TETRAHEDRON2:
		return -1;
	case kmb::PYRAMID:
	case kmb::PYRAMID2:
		return -1;
	case kmb::WEDGE:
		return -1;
	case kmb::WEDGE2:
		switch(index){
		case 0:	return 9;
		case 1:	return 10;
		case 2:	return 11;
		case 3:	return 6;
		case 4:	return 7;
		case 5:	return 8;
		case 6:	return 3;
		case 7:	return 4;
		case 8:	return 5;
		case 9:	return 0;
		case 10:	return 1;
		case 11:	return 2;
		default:	return -1;
		}
	case kmb::HEXAHEDRON:
	case kmb::HEXAHEDRON2:
		switch(index){
		case 0:	return 6;
		case 1:	return 7;
		case 2:	return 4;
		case 3:	return 5;
		case 4:	return 2;
		case 5:	return 3;
		case 6:	return 0;
		case 7:	return 1;
		case 8:	return 14;
		case 9:	return 15;
		case 10:	return 12;
		case 11:	return 13;
		case 12:	return 10;
		case 13:	return 11;
		case 14:	return 8;
		case 15:	return 9;
		case 16:	return 18;
		case 17:	return 19;
		case 18:	return 16;
		case 19:	return 17;
		default:	return -1;
		}
	default:
		return -2;
	}
}


int
kmb::ElementBase::getBoundaryCount(kmb::elementType type)
{
	switch(type){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return 2;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			return 3;
		case kmb::QUAD:
		case kmb::QUAD2:
			return 4;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			return 4;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			return 5;
		case kmb::WEDGE:
		case kmb::WEDGE2:
			return 5;
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			return 6;
		case kmb::QUADRILATERAL_INTERFACE:
			return 5;
		case kmb::TRIANGLE_INTERFACE:
			return 5;
		default:
			return -2;
	}
}

int
kmb::ElementBase::getEdgeCount(kmb::elementType type)
{
	switch( type ){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return 1;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
			return 3;
		case kmb::QUAD:
		case kmb::QUAD2:
			return 4;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			return 6;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			return 8;
		case kmb::WEDGE:
		case kmb::WEDGE2:
			return 9;
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			return 12;
		case kmb::QUADRILATERAL_INTERFACE:
			return 16;
		case kmb::TRIANGLE_INTERFACE:
			return 12;
		default:
			return -2;
	}
}

kmb::elementType
kmb::ElementBase::getBoundaryType(kmb::elementType type,int index)
{
	const int len = getBoundaryCount(type);
	if( index < -1 || len <= index ){
		return kmb::UNKNOWNTYPE;
	}else if( index == -1 ){

		return type;
	}
	switch(type){
		case kmb::UNKNOWNTYPE:
			return kmb::UNKNOWNTYPE;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return kmb::VERTEX;
		case kmb::TRIANGLE:
		case kmb::QUAD:
			return kmb::SEGMENT;
		case kmb::TRIANGLE2:
		case kmb::QUAD2:
			return kmb::SEGMENT2;
		case kmb::TETRAHEDRON:
			return kmb::TRIANGLE;
		case kmb::TETRAHEDRON2:
			return kmb::TRIANGLE2;
		case kmb::PYRAMID:
			if(kmb::Pyramid::faceTable[index][3]==-1){
				return kmb::TRIANGLE;
			}else{
				return kmb::QUAD;
			}
		case kmb::PYRAMID2:
			if(kmb::Pyramid2::faceTable[index][7]==-1){
				return kmb::TRIANGLE2;
			}else{
				return kmb::QUAD2;
			}
		case kmb::WEDGE:
			if(kmb::Wedge::faceTable[index][3]==-1){
				return kmb::TRIANGLE;
			}else{
				return kmb::QUAD;
			}
		case kmb::WEDGE2:
			if(kmb::Wedge2::faceTable[index][7]==-1){
				return kmb::TRIANGLE2;
			}else{
				return kmb::QUAD2;
			}
		case kmb::HEXAHEDRON:
			return kmb::QUAD;
		case kmb::HEXAHEDRON2:
			return kmb::QUAD2;
		case kmb::QUADRILATERAL_INTERFACE:
			return kmb::QUAD;
		case kmb::TRIANGLE_INTERFACE:
			return kmb::TRIANGLE;
		default:
			return kmb::OTHERTYPE;
	}
}

int
kmb::ElementBase::getBoundaryNodeCount(kmb::elementType type,int index)
{
	switch(type){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return 1;
		case kmb::TRIANGLE:
		case kmb::QUAD:
			return 2;
		case kmb::TRIANGLE2:
		case kmb::QUAD2:
			return 3;
		case kmb::TETRAHEDRON:
			return 3;
		case kmb::TETRAHEDRON2:
			return 6;
		case kmb::PYRAMID:
			if(kmb::Pyramid::faceTable[index][3]==-1){
				return 3;
			}else{
				return 4;
			}
		case kmb::PYRAMID2:
			if(kmb::Pyramid2::faceTable[index][7]==-1){
				return 6;
			}else{
				return 8;
			}
		case kmb::WEDGE:
			if(kmb::Wedge::faceTable[index][3]==-1){
				return 3;
			}else{
				return 4;
			}
		case kmb::WEDGE2:
			if(kmb::Wedge2::faceTable[index][7]==-1){
				return 6;
			}else{
				return 8;
			}
		case kmb::HEXAHEDRON:
			return 4;
		case kmb::HEXAHEDRON2:
			return 8;
		case kmb::QUADRILATERAL_INTERFACE:
			return 4;
		case kmb::TRIANGLE_INTERFACE:
			return 3;
		default:
			return -2;
	}
}

int
kmb::ElementBase::getBoundaryVertexCount(kmb::elementType type,int index)
{
	switch(type){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return 1;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
			return 2;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			return 3;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			if(kmb::Pyramid::faceTable[index][3]==-1){
				return 3;
			}else{
				return 4;
			}
		case kmb::WEDGE:
		case kmb::WEDGE2:
			if(kmb::Wedge::faceTable[index][3]==-1){
				return 3;
			}else{
				return 4;
			}
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			return 4;
		case kmb::QUADRILATERAL_INTERFACE:
			return 4;
		case kmb::TRIANGLE_INTERFACE:
			return 3;
		default:
			return -2;
	}
}

kmb::elementType
kmb::ElementBase::getEdgeType(kmb::elementType type,int index)
{
	const int len = getEdgeCount(type);
	if( index < 0 || len <= index ){
		return kmb::UNKNOWNTYPE;
	}
	switch(type){
		case kmb::UNKNOWNTYPE:
			return kmb::UNKNOWNTYPE;
		case kmb::SEGMENT:
		case kmb::TRIANGLE:
		case kmb::QUAD:
		case kmb::TETRAHEDRON:
		case kmb::PYRAMID:
		case kmb::WEDGE:
		case kmb::HEXAHEDRON:
		case kmb::QUADRILATERAL_INTERFACE:
		case kmb::TRIANGLE_INTERFACE:
			return kmb::SEGMENT;
		case kmb::SEGMENT2:
		case kmb::TRIANGLE2:
		case kmb::QUAD2:
		case kmb::TETRAHEDRON2:
		case kmb::PYRAMID2:
		case kmb::WEDGE2:
		case kmb::HEXAHEDRON2:
			return kmb::SEGMENT2;
		default:
			return kmb::OTHERTYPE;
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

int
kmb::ElementBase::getEdgeNodeCount(kmb::elementType type,int index)
{
	switch( type ){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::SEGMENT:
		case kmb::TRIANGLE:
		case kmb::QUAD:
		case kmb::TETRAHEDRON:
		case kmb::PYRAMID:
		case kmb::WEDGE:
		case kmb::HEXAHEDRON:
		case kmb::QUADRILATERAL_INTERFACE:
		case kmb::TRIANGLE_INTERFACE:
			return 2;
		case kmb::SEGMENT2:
		case kmb::TRIANGLE2:
		case kmb::QUAD2:
		case kmb::TETRAHEDRON2:
		case kmb::PYRAMID2:
		case kmb::WEDGE2:
		case kmb::HEXAHEDRON2:
			return 3;
		default:
			return -2;
	}
}

int
kmb::ElementBase::getEdgeVertexCount(kmb::elementType type,int index)
{
	switch( type ){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
		case kmb::WEDGE:
		case kmb::WEDGE2:
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
		case kmb::QUADRILATERAL_INTERFACE:
		case kmb::TRIANGLE_INTERFACE:
			return 2;
		default:
			return -2;
	}
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

int
kmb::ElementBase::getDimension(kmb::elementType type)
{
	switch(type){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::VERTEX:
			return 0;
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return 1;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
			return 2;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
		case kmb::WEDGE:
		case kmb::WEDGE2:
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
		case kmb::QUADRILATERAL_INTERFACE:
		case kmb::TRIANGLE_INTERFACE:
			return 3;
		default:
			return -2;
	}
}

int
kmb::ElementBase::getDegree(kmb::elementType type)
{
	switch(type){
		case kmb::UNKNOWNTYPE:
			return -1;
		case kmb::VERTEX:
			return 0;
		case kmb::SEGMENT:
		case kmb::TRIANGLE:
		case kmb::QUAD:
		case kmb::TETRAHEDRON:
		case kmb::PYRAMID:
		case kmb::WEDGE:
		case kmb::HEXAHEDRON:
		case kmb::QUADRILATERAL_INTERFACE:
		case kmb::TRIANGLE_INTERFACE:
			return 1;
		case kmb::SEGMENT2:
		case kmb::TRIANGLE2:
		case kmb::QUAD2:
		case kmb::TETRAHEDRON2:
		case kmb::PYRAMID2:
		case kmb::WEDGE2:
		case kmb::HEXAHEDRON2:
			return 2;
		default:
			return -2;
	}
}



int
kmb::ElementBase::getNodeCount(void) const
{
	return kmb::ElementBase::getNodeCount( getType() );
}

int
kmb::ElementBase::getVertexCount(void) const
{
	return kmb::ElementBase::getVertexCount( getType() );
}

const std::string
kmb::ElementBase::getTypeString(void) const
{
	return kmb::ElementBase::getTypeString( getType() );
}

kmb::elementType
kmb::ElementBase::getLinearType(void) const
{
	return kmb::ElementBase::getLinearType( getType() );
}

kmb::elementType
kmb::ElementBase::getSecondType(void) const
{
	return kmb::ElementBase::getSecondType( getType() );
}

int
kmb::ElementBase::getDiagIndex(int index) const
{
	return kmb::ElementBase::getDiagIndex( getType(), index );
}

int
kmb::ElementBase::getBoundaryCount(void) const
{
	return kmb::ElementBase::getBoundaryCount( getType() );
}

int
kmb::ElementBase::getEdgeCount(void) const
{
	return kmb::ElementBase::getEdgeCount( getType() );
}

kmb::elementType
kmb::ElementBase::getBoundaryType(int index) const
{
	return kmb::ElementBase::getBoundaryType( getType(), index );
}

kmb::elementType
kmb::ElementBase::getEdgeType(int index) const
{
	return kmb::ElementBase::getEdgeType( getType(), index );
}

int
kmb::ElementBase::getBoundaryNodeCount(int index) const
{
	return kmb::ElementBase::getNodeCount( getBoundaryType(index) );
}

int
kmb::ElementBase::getBoundaryVertexCount(int index) const
{
	return kmb::ElementBase::getVertexCount( getBoundaryType(index) );
}

int
kmb::ElementBase::getEdgeNodeCount(int index) const
{
	return kmb::ElementBase::getNodeCount( getEdgeType(index) );
}

int
kmb::ElementBase::getEdgeVertexCount(int index) const
{
	return kmb::ElementBase::getVertexCount( getEdgeType(index) );
}

int
kmb::ElementBase::getDimension(void) const
{
	return kmb::ElementBase::getDimension( getType() );
}

int
kmb::ElementBase::getDegree(void) const
{
	return kmb::ElementBase::getDegree( getType() );
}

bool
kmb::ElementBase::include(kmb::nodeIdType nodeId) const
{
	unsigned int len = this->getNodeCount();
	for(unsigned int i=0;i<len;++i){
		if( getCellId(i) == nodeId ){
			return true;
		}
	}
	return false;
}

bool
kmb::ElementBase::includeVertex(kmb::nodeIdType nodeId) const
{
	unsigned int len = this->getVertexCount();
	for(unsigned int i=0;i<len;++i){
		if( getCellId(i) == nodeId ){
			return true;
		}
	}
	return false;
}

int
kmb::ElementBase::indexOf(kmb::nodeIdType nodeId) const
{
	unsigned int len = this->getNodeCount();
	for(unsigned int i=0;i<len;++i){
		if( getCellId(i) == nodeId ){
			return i;
		}
	}
	return -1;
}

int
kmb::ElementBase::countCommonNode(kmb::ElementBase& other) const
{
	int count = 0;
	int len = other.getNodeCount();
	for(int i=0;i<len;++i){
		if( include( other.getCellId(i) ) ){
			++count;
		}
	}
	return count;
}

bool
kmb::ElementBase::replaceNodeId(kmb::nodeIdType oldNodeId,kmb::nodeIdType newNodeId)
{
	const int len = this->getNodeCount();
	for(int i=0;i<len;++i){
		if( this->getCellId(i) == oldNodeId ){
			return this->setCellId(i,newNodeId);
		}
	}
	return false;
}

int
kmb::ElementBase::replaceNodeId(std::map<nodeIdType,nodeIdType> &idmap)
{
	int counter = 0;
	const int len = this->getNodeCount();
	for(int i=0;i<len;++i){
		std::map<nodeIdType,nodeIdType>::iterator nIter = idmap.find( this->getCellId(i) );
		if( nIter != idmap.end() ){
			if( this->setCellId(i,nIter->second) ){
				++counter;
			}
		}
	}
	return counter;
}

kmb::nodeIdType
kmb::ElementBase::getBoundaryCellId(int index,int i) const
{
	if( 0 <= i && 0 <= index && index < getBoundaryCount() && i < getBoundaryNodeCount(index) ){
		switch( getType() )
		{
		case SEGMENT:
		case SEGMENT2:
			return getCellId( index );
		case TRIANGLE:
			return getCellId( kmb::Triangle::faceTable[index][i] );
		case TRIANGLE2:
			return getCellId( kmb::Triangle2::faceTable[index][i] );
		case QUAD:
			return getCellId( kmb::Quad::faceTable[index][i] );
		case QUAD2:
			return getCellId( kmb::Quad2::faceTable[index][i] );
		case TETRAHEDRON:
			return getCellId( kmb::Tetrahedron::faceTable[index][i] );
		case TETRAHEDRON2:
			return getCellId( kmb::Tetrahedron2::faceTable[index][i] );
		case WEDGE:
			return getCellId( kmb::Wedge::faceTable[index][i] );
		case WEDGE2:
			return getCellId( kmb::Wedge2::faceTable[index][i] );
		case PYRAMID:
			return getCellId( kmb::Pyramid::faceTable[index][i] );
		case PYRAMID2:
			return getCellId( kmb::Pyramid2::faceTable[index][i] );
		case HEXAHEDRON:
			return getCellId( kmb::Hexahedron::faceTable[index][i] );
		case HEXAHEDRON2:
			return getCellId( kmb::Hexahedron2::faceTable[index][i] );
		case QUADRILATERAL_INTERFACE:
			return getCellId( kmb::QuadInterface::faceTable[index][i] );
		case TRIANGLE_INTERFACE:
			return getCellId( kmb::TriInterface::faceTable[index][i] );
		default:
			break;
		}
	}else if( index == -1 && 0 <= i && i < getNodeCount() ){
		return getCellId(i);
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::ElementBase::getEdgeCellId(int index,int i) const
{

	if( 0 <= i && 0 <= index && index < getEdgeCount() && i < getDegree()+1 ){
		switch( getType() )
		{
		case SEGMENT:
		case SEGMENT2:
			return getCellId( i );
		case TRIANGLE:
			return getCellId( kmb::Triangle::faceTable[index][i] );
		case TRIANGLE2:
			return getCellId( kmb::Triangle2::faceTable[index][i] );
		case QUAD:
			return getCellId( kmb::Quad::faceTable[index][i] );
		case QUAD2:
			return getCellId( kmb::Quad2::faceTable[index][i] );
		case TETRAHEDRON:
			return getCellId( kmb::Tetrahedron::edgeTable[index][i] );
		case TETRAHEDRON2:
			return getCellId( kmb::Tetrahedron2::edgeTable[index][i] );
		case WEDGE:
			return getCellId( kmb::Wedge::edgeTable[index][i] );
		case WEDGE2:
			return getCellId( kmb::Wedge2::edgeTable[index][i] );
		case PYRAMID:
			return getCellId( kmb::Pyramid::edgeTable[index][i] );
		case PYRAMID2:
			return getCellId( kmb::Pyramid2::edgeTable[index][i] );
		case HEXAHEDRON:
			return getCellId( kmb::Hexahedron::edgeTable[index][i] );
		case HEXAHEDRON2:
			return getCellId( kmb::Hexahedron2::edgeTable[index][i] );
		case QUADRILATERAL_INTERFACE:
			return getCellId( kmb::QuadInterface::edgeTable[index][i] );
		case TRIANGLE_INTERFACE:
			return getCellId( kmb::TriInterface::edgeTable[index][i] );
		default:
			break;
		}
	}
	return kmb::nullNodeId;
}

kmb::elementType
kmb::ElementBase::getBoundaryElement(int index,kmb::nodeIdType* cell) const
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( cell != NULL && 0<= index && index < getBoundaryCount() ){
		etype = getBoundaryType( index );
		const int len = getBoundaryNodeCount( index );
		for(int i=0;i<len;++i){
			cell[i] = getBoundaryCellId(index,i);
		}
	}
	return etype;
}

bool
kmb::ElementBase::getBoundaryElement(int index,kmb::ElementBase& elem) const
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( 0<= index && index < getBoundaryCount() ){
		etype = getBoundaryType( index );
		if( etype == elem.getType() ){
			const int len = getBoundaryNodeCount( index );
			for(int i=0;i<len;++i){
				elem.setCellId(i,this->getBoundaryCellId(index,i));
			}
			return true;
		}
	}
	return false;
}

kmb::elementType
kmb::ElementBase::getEdgeElement(int index,kmb::nodeIdType* cell) const
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( cell != NULL && 0<= index && index < getEdgeCount() ){
		etype = getEdgeType( index );
		const int len = getEdgeNodeCount( index );
		for(int i=0;i<len;++i){
			cell[i] = getEdgeCellId(index,i);
		}
	}
	return etype;
}

bool
kmb::ElementBase::getEdgeElement(int index,kmb::ElementBase& elem) const
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( 0<= index && index < getEdgeCount() ){
		etype = getEdgeType( index );
		if( etype == elem.getType() ){
			const int len = getEdgeNodeCount( index );
			for(int i=0;i<len;++i){
				elem.setCellId(i,this->getEdgeCellId(index,i));
			}
			return true;
		}
	}
	return false;
}

int
kmb::ElementBase::isConnected(int index0,int index1) const
{
	const int len = getNodeCount();
	if( 0 <= index0 && index0 < len &&
		0 <= index1 && index1 < len )
	{
		switch( getType() )
		{
		case SEGMENT:
			return kmb::Segment::connectionTable[index0][index1];
		case SEGMENT2:
			return kmb::Segment2::connectionTable[index0][index1];
		case TRIANGLE:
			return kmb::Triangle::connectionTable[index0][index1];
		case TRIANGLE2:
			return kmb::Triangle2::connectionTable[index0][index1];
		case QUAD:
			return kmb::Quad::connectionTable[index0][index1];
		case QUAD2:
			return kmb::Quad2::connectionTable[index0][index1];
		case TETRAHEDRON:
			return kmb::Tetrahedron::connectionTable[index0][index1];
		case TETRAHEDRON2:
			return kmb::Tetrahedron2::connectionTable[index0][index1];
		case WEDGE:
			return kmb::Wedge::connectionTable[index0][index1];
		case WEDGE2:
			return kmb::Wedge2::connectionTable[index0][index1];
		case PYRAMID:
			return kmb::Pyramid::connectionTable[index0][index1];
		case PYRAMID2:
			return kmb::Pyramid2::connectionTable[index0][index1];
		case HEXAHEDRON:
			return kmb::Hexahedron::connectionTable[index0][index1];
		case HEXAHEDRON2:
			return kmb::Hexahedron2::connectionTable[index0][index1];
		case QUADRILATERAL_INTERFACE:
			return kmb::QuadInterface::connectionTable[index0][index1];
		case TRIANGLE_INTERFACE:
			return kmb::TriInterface::connectionTable[index0][index1];
		default:
			break;
		}
	}
	return 0;
}

int
kmb::ElementBase::isFace(const kmb::ElementBase &element, int &faceIndex) const
{
	const int vcount = element.getVertexCount();
	switch( vcount )
	{
	case 2:
		{
			int index0 = this->indexOf( element.getCellId(0) );
			int index1 = this->indexOf( element.getCellId(1) );
			return this->isFace( index0, index1, faceIndex );
		}
	case 3:
		{
			int index0 = this->indexOf( element.getCellId(0) );
			int index1 = this->indexOf( element.getCellId(1) );
			int index2 = this->indexOf( element.getCellId(2) );
			return this->isFace( index0, index1, index2, faceIndex );
		}
	case 4:
		{
			int index0 = this->indexOf( element.getCellId(0) );
			int index1 = this->indexOf( element.getCellId(1) );
			int index2 = this->indexOf( element.getCellId(1) );
			int index3 = this->indexOf( element.getCellId(1) );
			return this->isFace( index0, index1, index2, index3, faceIndex );
		}
	default:
		break;
	}
	return 0;
}

int
kmb::ElementBase::isFace(int index0,int index1,int &faceIndex) const
{
	const int nodeCount = getNodeCount();
	if( 0 <= index0 && index0 < nodeCount &&
		0 <= index1 && index1 < nodeCount )
	{
		const int len = this->getBoundaryCount();
		switch( getType() )
		{
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		{
			for(int i=0;i<len;++i){
				if( kmb::Triangle::faceTable[i][0] == index0 && kmb::Triangle::faceTable[i][1] == index1 ){
					faceIndex = i;
					return 1;
				}
				else if( kmb::Triangle::faceTable[i][0] == index1 && kmb::Triangle::faceTable[i][1] == index0 ){
					faceIndex = i;
					return -1;
				}
			}
			break;
		}
		case kmb::QUAD:
		case kmb::QUAD2:
		{
			for(int i=0;i<len;++i){
				if( kmb::Quad::faceTable[i][0] == index0 && kmb::Quad::faceTable[i][1] == index1 ){
					faceIndex = i;
					return 1;
				}
				else if( kmb::Quad::faceTable[i][0] == index1 && kmb::Quad::faceTable[i][1] == index0 ){
					faceIndex = i;
					return -1;
				}
			}
			break;
		}
		default:
			break;
		}
	}
	return 0;
}

int
kmb::ElementBase::isFace(int index0,int index1,int index2,int &faceIndex) const
{
	int ret = 0;
	const int nodeCount = getNodeCount();
	if( 0 <= index0 && index0 < nodeCount &&
		0 <= index1 && index1 < nodeCount &&
		0 <= index2 && index2 < nodeCount )
	{
		int aInd = -1;
		int bInd = -1;
		const int len = this->getBoundaryCount();
		switch( getType() )
		{
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 3 ){
						switch( kmb::ElementRelation::getTriangleRelation(
								index0,index1,index2,
								kmb::Tetrahedron::faceTable[i][0],
								kmb::Tetrahedron::faceTable[i][1],
								kmb::Tetrahedron::faceTable[i][2],aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		case kmb::WEDGE:
		case kmb::WEDGE2:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 3 ){
						switch( kmb::ElementRelation::getTriangleRelation(
								index0,index1,index2,
								kmb::Wedge::faceTable[i][0],
								kmb::Wedge::faceTable[i][1],
								kmb::Wedge::faceTable[i][2],aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 3 ){
						switch( kmb::ElementRelation::getTriangleRelation(
								index0,index1,index2,
								kmb::Pyramid::faceTable[i][0],
								kmb::Pyramid::faceTable[i][1],
								kmb::Pyramid::faceTable[i][2],aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		case kmb::TRIANGLE_INTERFACE:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 3 ){
						switch( kmb::ElementRelation::getTriangleRelation(
								index0,index1,index2,
								kmb::TriInterface::faceTable[i][0],
								kmb::TriInterface::faceTable[i][1],
								kmb::TriInterface::faceTable[i][2],aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		default:
			break;
		}
	}
	return ret;
}

int
kmb::ElementBase::isFace(int index0,int index1,int index2,int index3,int &faceIndex) const
{
	int ret = 0;
	const int nodeCount = getNodeCount();
	if( 0 <= index0 && index0 < nodeCount &&
		0 <= index1 && index1 < nodeCount &&
		0 <= index2 && index2 < nodeCount &&
		0 <= index3 && index3 < nodeCount )
	{
		int aInd = -1;
		int bInd = -1;
		const int len = this->getBoundaryCount();
		switch( getType() )
		{
		case kmb::WEDGE:
		case kmb::WEDGE2:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 4 ){
						switch( kmb::ElementRelation::getQuadRelation(
								index0,index1,index2,index3,
								kmb::Wedge::faceTable[i][0],
								kmb::Wedge::faceTable[i][1],
								kmb::Wedge::faceTable[i][2],
								kmb::Wedge::faceTable[i][3],
								aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 4 ){
						switch( kmb::ElementRelation::getQuadRelation(
								index0,index1,index2,index3,
								kmb::Pyramid::faceTable[i][0],
								kmb::Pyramid::faceTable[i][1],
								kmb::Pyramid::faceTable[i][2],
								kmb::Pyramid::faceTable[i][3],
								aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 4 ){
						switch( kmb::ElementRelation::getQuadRelation(
								index0,index1,index2,index3,
								kmb::Hexahedron::faceTable[i][0],
								kmb::Hexahedron::faceTable[i][1],
								kmb::Hexahedron::faceTable[i][2],
								kmb::Hexahedron::faceTable[i][3],
								aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		case kmb::QUADRILATERAL_INTERFACE:
			{
				for(int i=0;i<len;++i){
					int vertexCount = this->getBoundaryVertexCount(i);
					if( vertexCount == 4 ){
						switch( kmb::ElementRelation::getQuadRelation(
								index0,index1,index2,index3,
								kmb::QuadInterface::faceTable[i][0],
								kmb::QuadInterface::faceTable[i][1],
								kmb::QuadInterface::faceTable[i][2],
								kmb::QuadInterface::faceTable[i][3],
								aInd,bInd)){
						case kmb::ElementRelation::EQUAL:
							faceIndex = i;
							return 1;
						case kmb::ElementRelation::REVERSE:
							faceIndex = i;
							return -1;
						default:
							break;
						}
					}
				}
				break;
			}
		default:
			break;
		}
	}
	return ret;
}


int
kmb::ElementBase::getFaceIndex(int index0,int index1) const
{
	const int nodeCount = getNodeCount();
	int faceIndex = -1;
	if( 0 <= index0 && index0 < nodeCount &&
		0 <= index1 && index1 < nodeCount )
	{
		switch( getType() )
		{
		case kmb::SEGMENT:
		case kmb::SEGMENT2:
			return -1;
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
			if( isFace(index0,index1,faceIndex) != 0 ){
				return faceIndex;
			}
			break;
		case kmb::TETRAHEDRON:
		case kmb::TETRAHEDRON2:
			switch(index0){
			case 0:
				switch(index1){
				case 1:		return 6;
				case 2:		return 5;
				case 3:		return 7;
				default:	break;
				}
			case 1:
				switch(index1){
				case 0:		return 6;
				case 2:		return 4;
				case 3:		return 8;
				default:	break;
				}
			case 2:
				switch(index1){
				case 0:		return 5;
				case 1:		return 4;
				case 3:		return 9;
				default:	break;
				}
			case 3:
				switch(index1){
				case 0:		return 7;
				case 1:		return 8;
				case 2:		return 9;
				default:	break;
				}
			default:
				break;
			}
			break;
		case kmb::WEDGE:
		case kmb::WEDGE2:
			switch(index0){
			case 0:
				switch(index1){
				case 1:		return 7;
				case 2:		return 6;
				case 3:		return 11;
				case 4:		return 2;
				case 5:		return 4;
				default:	break;
				}
			case 1:
				switch(index1){
				case 0:		return 7;
				case 2:		return 5;
				case 3:		return 2;
				case 4:		return 12;
				case 5:		return 3;
				default:	break;
				}
			case 2:
				switch(index1){
				case 0:		return 6;
				case 1:		return 5;
				case 3:		return 4;
				case 4:		return 3;
				case 5:		return 13;
				default:	break;
				}
			case 3:
				switch(index1){
				case 0:		return 11;
				case 1:		return 2;
				case 2:		return 4;
				case 4:		return 10;
				case 5:		return 9;
				default:	break;
				}
			case 4:
				switch(index1){
				case 0:		return 2;
				case 1:		return 12;
				case 2:		return 3;
				case 3:		return 10;
				case 5:		return 8;
				default:	break;
				}
			case 5:
				switch(index1){
				case 0:		return 4;
				case 1:		return 3;
				case 2:		return 13;
				case 3:		return 9;
				case 4:		return 8;
				default:	break;
				}
			default:
				break;
			}
			break;
		case kmb::PYRAMID:
		case kmb::PYRAMID2:
			switch(index0){
			case 0:
				switch(index1){
				case 1:		return 5;
				case 2:		return 6;
				case 3:		return 7;
				case 4:		return 8;
				default:	break;
				}
			case 1:
				switch(index1){
				case 0:		return 5;
				case 2:		return 9;
				case 3:		return 4;
				case 4:		return 12;
				default:	break;
				}
			case 2:
				switch(index1){
				case 0:		return 6;
				case 1:		return 9;
				case 3:		return 10;
				case 4:		return 4;
				default:	break;
				}
			case 3:
				switch(index1){
				case 0:		return 7;
				case 1:		return 4;
				case 2:		return 10;
				case 4:		return 11;
				default:	break;
				}
			case 4:
				switch(index1){
				case 0:		return 8;
				case 1:		return 12;
				case 2:		return 4;
				case 3:		return 11;
				default:	break;
				}
			default:
				break;
			}
			break;
		case kmb::HEXAHEDRON:
		case kmb::HEXAHEDRON2:
			switch(index0){
			case 0:
				switch(index1){
				case 1:		return 6;
				case 2:		return 0;
				case 3:		return 9;
				case 4:		return 14;
				case 5:		return 2;
				case 6:		return -1;
				case 7:		return 5;
				default:	break;
				}
			case 1:
				switch(index1){
				case 0:		return 6;
				case 2:		return 7;
				case 3:		return 0;
				case 4:		return 2;
				case 5:		return 15;
				case 6:		return 3;
				case 7:		return -1;
				default:	break;
				}
			case 2:
				switch(index1){
				case 0:		return 0;
				case 1:		return 7;
				case 3:		return 8;
				case 4:		return -1;
				case 5:		return 3;
				case 6:		return 16;
				case 7:		return 4;
				default:	break;
				}
			case 3:
				switch(index1){
				case 0:		return 9;
				case 1:		return 0;
				case 2:		return 8;
				case 4:		return 5;
				case 5:		return -1;
				case 6:		return 4;
				case 7:		return 17;
				default:	break;
				}
			case 4:
				switch(index1){
				case 0:		return 14;
				case 1:		return 2;
				case 2:		return -1;
				case 3:		return 5;
				case 5:		return 10;
				case 6:		return 1;
				case 7:		return 13;
				default:	break;
				}
			case 5:
				switch(index1){
				case 0:		return 2;
				case 1:		return 15;
				case 2:		return 3;
				case 3:		return -1;
				case 4:		return 10;
				case 6:		return 11;
				case 7:		return 1;
				default:	break;
				}
			case 6:
				switch(index1){
				case 0:		return -1;
				case 1:		return 3;
				case 2:		return 16;
				case 3:		return 4;
				case 4:		return 1;
				case 5:		return 11;
				case 7:		return 12;
				default:	break;
				}
			case 7:
				switch(index1){
				case 0:		return 5;
				case 1:		return -1;
				case 2:		return 4;
				case 3:		return 17;
				case 4:		return 13;
				case 5:		return 1;
				case 6:		return 12;
				default:	break;
				}
			default:
				break;
			}
			break;
		case kmb::QUADRILATERAL_INTERFACE:
			return kmb::QuadInterface::faceIndexTable[index0][index1];
		case kmb::TRIANGLE_INTERFACE:
			return kmb::TriInterface::faceIndexTable[index0][index1];
		default:
			return -1;
		}
	}
	return -1;
}

bool
kmb::ElementBase::reverse(void)
{
	bool result = false;
	switch( getType() )
	{
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
		{
			kmb::nodeIdType tmp[2];
			for(int i=0;i<2;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[1]) &&
				this->setCellId(1,tmp[0])
				);
			break;
		}
	case kmb::TRIANGLE:
		{
			kmb::nodeIdType tmp[3];
			for(int i=0;i<3;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[2]) &&
				this->setCellId(2,tmp[0])
				);
			break;
		}
	case kmb::TRIANGLE2:
		{
			kmb::nodeIdType tmp[6];
			for(int i=0;i<6;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[2]) &&
				this->setCellId(2,tmp[0]) &&
				this->setCellId(3,tmp[5]) &&
				this->setCellId(5,tmp[3])
				);
			break;
		}
	case kmb::QUAD:
		{
			kmb::nodeIdType tmp[4];
			for(int i=0;i<4;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[3]) &&
				this->setCellId(1,tmp[2]) &&
				this->setCellId(2,tmp[1]) &&
				this->setCellId(3,tmp[0])
				);
			break;
		}
	case kmb::QUAD2:
		{
			kmb::nodeIdType tmp[8];
			for(int i=0;i<8;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[3]) &&
				this->setCellId(1,tmp[2]) &&
				this->setCellId(2,tmp[1]) &&
				this->setCellId(3,tmp[0]) &&
				this->setCellId(4,tmp[6]) &&
				this->setCellId(6,tmp[4])
				);
			break;
		}
	case kmb::TETRAHEDRON:
		{
			kmb::nodeIdType tmp[4];
			for(int i=0;i<4;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[1]) &&
				this->setCellId(1,tmp[0])
				);
			break;
		}
	case kmb::TETRAHEDRON2:
		{
			kmb::nodeIdType tmp[10];
			for(int i=0;i<10;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[1]) &&
				this->setCellId(1,tmp[0]) &&
				this->setCellId(4,tmp[5]) &&
				this->setCellId(5,tmp[4]) &&
				this->setCellId(7,tmp[8]) &&
				this->setCellId(8,tmp[7])
				);
			break;
		}
	case kmb::PYRAMID:
		{
			kmb::nodeIdType tmp[5];
			for(int i=0;i<5;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(2,tmp[4]) &&
				this->setCellId(4,tmp[2])
				);
			break;
		}
	case kmb::PYRAMID2:
		{
			kmb::nodeIdType tmp[13];
			for(int i=0;i<13;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(2,tmp[4]) &&
				this->setCellId(4,tmp[2]) &&
				this->setCellId(6,tmp[8]) &&
				this->setCellId(8,tmp[6]) &&
				this->setCellId(9,tmp[12]) &&
				this->setCellId(10,tmp[11]) &&
				this->setCellId(11,tmp[10]) &&
				this->setCellId(12,tmp[9])
				);
			break;
		}
	case kmb::WEDGE:
		{
			kmb::nodeIdType tmp[6];
			for(int i=0;i<6;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(1,tmp[2]) &&
				this->setCellId(2,tmp[1]) &&
				this->setCellId(4,tmp[5]) &&
				this->setCellId(5,tmp[4])
				);
			break;
		}
	case kmb::WEDGE2:
		{
			kmb::nodeIdType tmp[15];
			for(int i=0;i<15;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(1,tmp[2]) &&
				this->setCellId(2,tmp[1]) &&
				this->setCellId(4,tmp[5]) &&
				this->setCellId(5,tmp[4]) &&
				this->setCellId(6,tmp[6]) &&
				this->setCellId(7,tmp[8]) &&
				this->setCellId(8,tmp[7]) &&
				this->setCellId(9,tmp[9]) &&
				this->setCellId(10,tmp[11]) &&
				this->setCellId(11,tmp[10]) &&
				this->setCellId(13,tmp[14]) &&
				this->setCellId(14,tmp[13])
				);
			break;
		}
	case kmb::HEXAHEDRON:
		{
			kmb::nodeIdType tmp[8];
			for(int i=0;i<8;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(1,tmp[3]) &&
				this->setCellId(3,tmp[1]) &&
				this->setCellId(5,tmp[7]) &&
				this->setCellId(7,tmp[5])
				);
			break;
		}
	case kmb::HEXAHEDRON2:
		{
			kmb::nodeIdType tmp[20];
			for(int i=0;i<20;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(1,tmp[3]) &&
				this->setCellId(3,tmp[1]) &&
				this->setCellId(5,tmp[7]) &&
				this->setCellId(7,tmp[5]) &&
				this->setCellId(8,tmp[11]) &&
				this->setCellId(9,tmp[10]) &&
				this->setCellId(10,tmp[9]) &&
				this->setCellId(11,tmp[8]) &&
				this->setCellId(12,tmp[15]) &&
				this->setCellId(13,tmp[14]) &&
				this->setCellId(14,tmp[13]) &&
				this->setCellId(15,tmp[12]) &&
				this->setCellId(17,tmp[19]) &&
				this->setCellId(19,tmp[17])
				);
			break;
		}
	case kmb::QUADRILATERAL_INTERFACE:
		{
			kmb::nodeIdType tmp[9];
			for(int i=0;i<9;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(1,tmp[3]) &&
				this->setCellId(3,tmp[1]) &&
				this->setCellId(4,tmp[7]) &&
				this->setCellId(5,tmp[6]) &&
				this->setCellId(6,tmp[5]) &&
				this->setCellId(7,tmp[4])
				);
			break;
		}
	case kmb::TRIANGLE_INTERFACE:
		{
			kmb::nodeIdType tmp[6];
			for(int i=0;i<6;++i){
				tmp[i] = this->getCellId(i);
			}
			result = (
				this->setCellId(0,tmp[2]) &&
				this->setCellId(2,tmp[0]) &&
				this->setCellId(3,tmp[5]) &&
				this->setCellId(5,tmp[3])
				);
			break;
		}
	default:
		break;
	}
	return result;
}

bool
kmb::ElementBase::reverse( kmb::elementType etype, kmb::nodeIdType* nodes )
{
	bool result = false;
	switch( etype )
	{
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
		{
			kmb::nodeIdType tmp[2];
			for(int i=0;i<2;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[1];
			nodes[1] = tmp[0];
			result = true;
			break;
		}
	case kmb::TRIANGLE:
		{
			kmb::nodeIdType tmp[3];
			for(int i=0;i<3;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[2];
			nodes[2] = tmp[0];
			result = true;
			break;
		}
	case kmb::TRIANGLE2:
		{
			kmb::nodeIdType tmp[6];
			for(int i=0;i<6;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[2];
			nodes[2] = tmp[0];
			nodes[3] = tmp[5];
			nodes[5] = tmp[3];
			result = true;
			break;
		}
	case kmb::QUAD:
		{
			kmb::nodeIdType tmp[4];
			for(int i=0;i<4;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[3];
			nodes[1] = tmp[2];
			nodes[2] = tmp[1];
			nodes[3] = tmp[0];
			result = true;
			break;
		}
	case kmb::QUAD2:
		{
			kmb::nodeIdType tmp[8];
			for(int i=0;i<8;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[3];
			nodes[1] = tmp[2];
			nodes[2] = tmp[1];
			nodes[3] = tmp[0];
			nodes[4] = tmp[6];
			nodes[6] = tmp[4];
			result = true;
			break;
		}
	case kmb::TETRAHEDRON:
		{
			kmb::nodeIdType tmp[4];
			for(int i=0;i<4;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[1];
			nodes[1] = tmp[0];
			result = true;
			break;
		}
	case kmb::TETRAHEDRON2:
		{
			kmb::nodeIdType tmp[10];
			for(int i=0;i<10;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[1];
			nodes[1] = tmp[0];
			nodes[4] = tmp[5];
			nodes[5] = tmp[4];
			nodes[7] = tmp[8];
			nodes[8] = tmp[7];
			result = true;
			break;
		}
	case kmb::PYRAMID:
		{
			kmb::nodeIdType tmp[5];
			for(int i=0;i<5;++i){
				tmp[i] = nodes[i];
			}
			nodes[2] = tmp[4];
			nodes[4] = tmp[2];
			result = true;
			break;
		}
	case kmb::PYRAMID2:
		{
			kmb::nodeIdType tmp[13];
			for(int i=0;i<13;++i){
				tmp[i] = nodes[i];
			}
			nodes[2] = tmp[4];
			nodes[4] = tmp[2];
			nodes[6] = tmp[8];
			nodes[8] = tmp[6];
			nodes[9] = tmp[12];
			nodes[10] = tmp[11];
			nodes[11] = tmp[10];
			nodes[12] = tmp[9];
			result = true;
			break;
		}
	case kmb::WEDGE:
		{
			kmb::nodeIdType tmp[6];
			for(int i=0;i<6;++i){
				tmp[i] = nodes[i];
			}
			nodes[1] = tmp[2];
			nodes[2] = tmp[1];
			nodes[4] = tmp[5];
			nodes[5] = tmp[4];
			result = true;
			break;
		}
	case kmb::WEDGE2:
		{
			kmb::nodeIdType tmp[15];
			for(int i=0;i<15;++i){
				tmp[i] = nodes[i];
			}
			nodes[1] = tmp[2];
			nodes[2] = tmp[1];
			nodes[4] = tmp[5];
			nodes[5] = tmp[4];
			nodes[7] = tmp[8];
			nodes[8] = tmp[7];
			nodes[10] = tmp[11];
			nodes[11] = tmp[10];
			nodes[13] = tmp[14];
			nodes[14] = tmp[13];
			result = true;
			break;
		}
	case kmb::HEXAHEDRON:
		{
			kmb::nodeIdType tmp[8];
			for(int i=0;i<8;++i){
				tmp[i] = nodes[i];
			}
			nodes[1] = tmp[3];
			nodes[3] = tmp[1];
			nodes[5] = tmp[7];
			nodes[7] = tmp[5];
			result = true;
			break;
		}
	case kmb::HEXAHEDRON2:
		{
			kmb::nodeIdType tmp[20];
			for(int i=0;i<20;++i){
				tmp[i] = nodes[i];
			}
			nodes[1] = tmp[3];
			nodes[3] = tmp[1];
			nodes[5] = tmp[7];
			nodes[7] = tmp[5];
			nodes[8] = tmp[11];
			nodes[9] = tmp[10];
			nodes[10] = tmp[9];
			nodes[11] = tmp[8];
			nodes[12] = tmp[15];
			nodes[13] = tmp[14];
			nodes[14] = tmp[13];
			nodes[15] = tmp[12];
			nodes[17] = tmp[19];
			nodes[19] = tmp[17];
			result = true;
			break;
		}
	case kmb::QUADRILATERAL_INTERFACE:
		{
			kmb::nodeIdType tmp[9];
			for(int i=0;i<9;++i){
				tmp[i] = nodes[i];
			}
			nodes[1] = tmp[3];
			nodes[3] = tmp[1];
			nodes[4] = tmp[7];
			nodes[5] = tmp[6];
			nodes[6] = tmp[5];
			nodes[7] = tmp[4];
			result = true;
			break;
		}
	case kmb::TRIANGLE_INTERFACE:
		{
			kmb::nodeIdType tmp[6];
			for(int i=0;i<6;++i){
				tmp[i] = nodes[i];
			}
			nodes[0] = tmp[2];
			nodes[2] = tmp[0];
			nodes[3] = tmp[5];
			nodes[5] = tmp[3];
			result = true;
			break;
		}
	default:
		break;
	}
	return result;
}

int
kmb::ElementBase::getIndexMinNodeIdOfFace(int faceIndex) const
{
	int minIndex = 0;
	kmb::nodeIdType minId = this->getBoundaryCellId(faceIndex,0);
	const int len = this->getBoundaryVertexCount(faceIndex);
	for(int i=1;i<len;++i)
	{
		kmb::nodeIdType nodeId = this->getBoundaryCellId(faceIndex,i);
		if( minId > nodeId ){
			minIndex = i;
			minId = nodeId;
		}
	}
	return minIndex;
}

int
kmb::ElementBase::getIndexMinNodeId(void) const
{
	int minIndex = 0;
	kmb::nodeIdType minId = this->getCellId(0);
	const int len = this->getVertexCount();
	for(int i=1;i<len;++i)
	{
		kmb::nodeIdType nodeId = this->getCellId(i);
		if( minId > nodeId ){
			minIndex = i;
			minId = nodeId;
		}
	}
	return minIndex;
}

int
kmb::ElementBase::divideIntoTriangles(kmb::nodeIdType triangles[][3]) const
{
	int num = 0;
	switch( getType() )
	{
	case kmb::TRIANGLE:
	case kmb::TRIANGLE2:
		{
			for(int i=0;i<3;++i){
				triangles[0][i] = this->getCellId(i);
			}
			num = 1;
			break;
		}
	case kmb::QUAD:
	case kmb::QUAD2:
		{
			triangles[0][0] = this->getCellId(0);
			triangles[0][1] = this->getCellId(1);
			triangles[0][2] = this->getCellId(2);
			triangles[1][0] = this->getCellId(0);
			triangles[1][1] = this->getCellId(2);
			triangles[1][2] = this->getCellId(3);
			num = 2;
			break;
		}
	default:
		break;
	}
	return num;
}

int
kmb::ElementBase::divideIntoTetrahedrons(kmb::nodeIdType tetrahedrons[][4]) const
{
	int num = 0;
	switch( getType() )
	{
	case kmb::TETRAHEDRON:
	case kmb::TETRAHEDRON2:
		{
			for(int i=0;i<4;++i){
				tetrahedrons[0][i] = this->getCellId(i);
			}
			num = 1;
			break;
		}
	case kmb::PYRAMID:
	case kmb::PYRAMID2:
		num = kmb::Pyramid::divideIntoTetrahedrons( this, tetrahedrons );
		break;
	case kmb::WEDGE:
	case kmb::WEDGE2:
		num = kmb::Wedge::divideIntoTetrahedrons( this, tetrahedrons );
		break;
	case kmb::HEXAHEDRON:
	case kmb::HEXAHEDRON2:
		num = kmb::Hexahedron::divideIntoTetrahedrons( this, tetrahedrons );
		break;
	default:
		break;
	}
	return num;
}


kmb::nodeIdType
kmb::ElementBase::nextNode(kmb::nodeIdType nodeId) const
{
	switch( getType() )
	{
	case kmb::TRIANGLE:
	case kmb::TRIANGLE2:
		if		( nodeId == getCellId(0) )	{	return getCellId(1);	}
		else if	( nodeId == getCellId(1) )	{	return getCellId(2);	}
		else if	( nodeId == getCellId(2) )	{	return getCellId(0);	}
		break;
	case kmb::QUAD:
	case kmb::QUAD2:
		if		( nodeId == getCellId(0) )	{	return getCellId(1);	}
		else if	( nodeId == getCellId(1) )	{	return getCellId(2);	}
		else if	( nodeId == getCellId(2) )	{	return getCellId(3);	}
		else if	( nodeId == getCellId(3) )	{	return getCellId(0);	}
		break;
	default:
		break;
	}
	return kmb::nullNodeId;
}

kmb::nodeIdType
kmb::ElementBase::previousNode(kmb::nodeIdType nodeId) const
{
	switch( getType() )
	{
	case kmb::TRIANGLE:
	case kmb::TRIANGLE2:
		if		( nodeId == getCellId(0) )	{	return getCellId(2);	}
		else if	( nodeId == getCellId(1) )	{	return getCellId(0);	}
		else if	( nodeId == getCellId(2) )	{	return getCellId(1);	}
		break;
	case kmb::QUAD:
	case kmb::QUAD2:
		if		( nodeId == getCellId(0) )	{	return getCellId(3);	}
		else if	( nodeId == getCellId(1) )	{	return getCellId(0);	}
		else if	( nodeId == getCellId(2) )	{	return getCellId(1);	}
		else if	( nodeId == getCellId(3) )	{	return getCellId(2);	}
		break;
	default:
		break;
	}
	return kmb::nullNodeId;
}



kmb::Element*
kmb::Element::create(kmb::elementType type, kmb::nodeIdType* ary)
{
	kmb::Element* element = NULL;
	if( ary ==NULL ){
		int ncount = kmb::Element::getNodeCount(type);
		ary = new kmb::nodeIdType[ ncount ];
		for(int i=0;i<ncount;++i){
			ary[i] = kmb::nullNodeId;
		}
	}
	switch(type){
		case SEGMENT:
			element = new kmb::Segment(ary);
			break;
		case SEGMENT2:
			element = new kmb::Segment2(ary);
			break;
		case TRIANGLE:
			element = new kmb::Triangle(ary);
			break;
		case TRIANGLE2:
			element = new kmb::Triangle2(ary);
			break;
		case QUAD:
			element = new kmb::Quad(ary);
			break;
		case QUAD2:
			element = new kmb::Quad2(ary);
			break;
		case TETRAHEDRON:
			element = new kmb::Tetrahedron(ary);
			break;
		case TETRAHEDRON2:
			element = new kmb::Tetrahedron2(ary);
			break;
		case PYRAMID:
			element = new kmb::Pyramid(ary);
			break;
		case PYRAMID2:
			element = new kmb::Pyramid2(ary);
			break;
		case WEDGE:
			element = new kmb::Wedge(ary);
			break;
		case WEDGE2:
			element = new kmb::Wedge2(ary);
			break;
		case HEXAHEDRON:
			element = new kmb::Hexahedron(ary);
			break;
		case HEXAHEDRON2:
			element = new kmb::Hexahedron2(ary);
			break;
		case kmb::QUADRILATERAL_INTERFACE:
			element = new kmb::QuadInterface(ary);
			break;
		case kmb::TRIANGLE_INTERFACE:
			element = new kmb::TriInterface(ary);
			break;
		default:
			break;
	}
	return element;
}

kmb::Element::Element(kmb::elementType t)
: type(t)
, cell(NULL)
{
	if( t == kmb::UNKNOWNTYPE ){
		cell = new kmb::nodeIdType[ kmb::Element::MAX_NODE_COUNT ];
	}
}

kmb::Element::~Element(void)
{
	if(cell)	delete[] cell;
}

kmb::Element*
kmb::Element::clone() const
{
	int len = this->getNodeCount();
	kmb::nodeIdType* ary = NULL;
	ary = new kmb::nodeIdType[len];
	for(int i=0;i<len;++i){
		ary[i] = this->getCellId(i);
	}
	return kmb::Element::create(this->type,ary);
}

kmb::Element*
kmb::Element::reverseClone() const
{
	kmb::Element* elem = this->clone();
	elem->reverse();
	return elem;
}

kmb::Element*
kmb::Element::clone( std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper ) const
{
	int len = this->getNodeCount();
	kmb::nodeIdType* ary = NULL;
	ary = new kmb::nodeIdType[len];
	for(int i=0;i<len;++i){
		kmb::nodeIdType nodeID = this->getCellId(i);
		if( nodeMapper.find( nodeID ) != nodeMapper.end() ){
			ary[i] = nodeMapper[ nodeID ];
		}else{
			ary[i] = nodeID;
		}
	}
	return kmb::Element::create(this->type,ary);
}

kmb::Element*
kmb::Element::reverseClone( std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper ) const
{
	kmb::Element* elem = this->clone( nodeMapper );
	elem->reverse();
	return elem;
}



bool
kmb::Element::setCellId(int index,kmb::nodeIdType id)
{
	cell[index] = id;
	return true;
}

kmb::nodeIdType
kmb::Element::getCellId(int index) const
{
	return cell[index];
}

kmb::nodeIdType
kmb::Element::operator[](const int i) const
{
	return cell[i];
}

kmb::nodeIdType&
kmb::Element::operator[](const int i)
{
	return cell[i];
}



kmb::elementType
kmb::Element::getType() const
{
	return type;
}
