/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : StructuredCell                                          #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Copyright (C) 2012 Advancesoft corp.                            #
#                                                                      #
----------------------------------------------------------------------*/

#pragma once

#include "Common/kmbCommon.h"
#include "Geometry/kmbIdTypes.h"
#include "MeshDB/kmbTypes.h"

namespace kmb{

class Point3D;
class Point3DStructured;
class Hexahedron;





class StructuredCell
{
protected:
	kmb::Point3DStructured* points;
public:
	StructuredCell(kmb::Point3DStructured* _points);
	virtual ~StructuredCell(void);

	bool getPoint(int i,int j,int k,kmb::Point3D& pt) const;
	bool getPoint(kmb::nodeIdType nodeId,kmb::Point3D& pt) const;
	bool getHexahedron(int i,int j,int k,kmb::Hexahedron &hexa) const;

	bool getIndices(kmb::nodeIdType nodeId,int &i,int &j,int &k) const;
	kmb::nodeIdType getNodeId(int i,int j,int k) const;
	kmb::elementIdType getElementId(int i,int j,int k) const;
};

}
