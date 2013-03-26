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
#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbPoint3DContainer.h"
#include <vector>








/**

Face クラス

localFaceId
 -1                                             : 要素全体
 0 から boundaryCount-1                         : 面
 boundaryCount から boundaryCount + edgeCount-1 : 辺

*/

namespace kmb{

class ElementContainer;
class MeshData;
class ElementBase;

class Face
{
private:
	kmb::elementIdType elementId;
	kmb::idType localFaceId;
public:
	Face(void);
	Face(kmb::elementIdType elementId, kmb::idType localFaceId);
	Face(const Face &f);
	virtual ~Face(void);

	bool operator<(const Face &other)const{
		return (elementId < other.elementId) ||
			(elementId==other.elementId &&
			 localFaceId < other.localFaceId);
	}
	bool operator==(const Face &other)const{
		return (elementId == other.elementId) && (localFaceId == other.localFaceId);
	}
	bool operator!=(const Face &other)const{
		return (elementId != other.elementId) || (localFaceId != other.localFaceId);
	}
	Face& operator=(const Face &other);

	kmb::elementIdType getElementId() const { return elementId; }
	kmb::idType getLocalFaceId() const { return localFaceId; }
	void setId(kmb::elementIdType elementId,kmb::idType localId);

	kmb::Element* createElement(const kmb::ElementContainer* body) const;
	kmb::Element* createElement(const kmb::MeshData* mesh) const;
	bool getFaceElement(const kmb::ElementContainer* body,kmb::ElementBase &element) const;
	bool getFaceElement(const kmb::MeshData* mesh,kmb::ElementBase &element) const;
	kmb::elementType getFaceElementType(const kmb::ElementContainer* body) const;
	kmb::elementType getFaceElementType(const kmb::MeshData* mesh) const;
	bool getFaceLinearElement(const kmb::ElementContainer* body,kmb::ElementBase &element) const;
	bool getFaceLinearElement(const kmb::MeshData* mesh,kmb::ElementBase &element) const;
	kmb::elementType getFaceLinearElementType(const kmb::ElementContainer* body) const;
	kmb::elementType getFaceLinearElementType(const kmb::MeshData* mesh) const;
	int getNodeCount(const kmb::ElementContainer* body) const;
	int getNodeCount(const kmb::MeshData* db) const;
private:
	kmb::Element* createElement(const kmb::ElementBase &parent) const;
	bool getFaceElement(const kmb::ElementBase &parent,kmb::ElementBase &output) const;
	kmb::elementType getFaceElementType(const kmb::ElementBase &parent) const;
	bool getFaceLinearElement(const kmb::ElementBase &parent,kmb::ElementBase &output) const;
	kmb::elementType getFaceLinearElementType(const kmb::ElementBase &parent) const;
	int getNodeCount(const kmb::ElementBase &parent) const;
};

}
