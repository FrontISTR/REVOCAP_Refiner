/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DStructured                                       #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Copyright (C) 2012 Advancesoft corp.                            #
#                                                                      #
----------------------------------------------------------------------*/

#include "MeshDB/kmbStructuredCell.h"

#include "Common/kmbCommon.h"
#include "Geometry/kmbPoint3DStructured.h"
#include "MeshDB/kmbHexahedron.h"

kmb::StructuredCell::StructuredCell(kmb::Point3DStructured* _points)
	: points(_points)
{
}


kmb::StructuredCell::~StructuredCell(void)
{
}

bool kmb::StructuredCell::getPoint(int i,int j,int k,kmb::Point3D& pt) const
{
	if( points ){
		return points->getPoint(i,j,k,pt);
	}else{
		return false;
	}
}

bool kmb::StructuredCell::getPoint(kmb::nodeIdType nodeId,kmb::Point3D& pt) const
{
	if( points ){
		int i(0),j(0),k(0);
		getIndices(nodeId,i,j,k);
		return points->getPoint(i,j,k,pt);
	}else{
		return false;
	}
}

bool kmb::StructuredCell::getHexahedron(int i,int j,int k,kmb::Hexahedron &hexa) const
{
	if( points == NULL ||
		i < 0 || points->divx <= i ||
		j < 0 || points->divy <= j ||
		k < 0 || points->divz <= k )
	{
		return false;
	}else{
		hexa[0] = getNodeId(i,  j,  k);
		hexa[1] = getNodeId(i+1,j,  k);
		hexa[2] = getNodeId(i+1,j+1,k);
		hexa[3] = getNodeId(i,  j+1,k);
		hexa[4] = getNodeId(i,  j,  k+1);
		hexa[5] = getNodeId(i+1,j,  k+1);
		hexa[6] = getNodeId(i+1,j+1,k+1);
		hexa[7] = getNodeId(i,  j+1,k+1);
		return true;
	}
}

bool kmb::StructuredCell::getIndices(kmb::nodeIdType nodeId,int &i,int &j,int &k) const
{
	if( points == NULL || nodeId < 0 || points->getNodeCount() <= nodeId ){
		return false;
	}else{
		i = nodeId / ((points->divy+1) * (points->divz+1));
		int qi = nodeId - i*((points->divy+1) * (points->divz+1));
		j = qi / (points->divz+1);
		k = qi - j * (points->divz+1);
		return true;
	}
}

kmb::nodeIdType kmb::StructuredCell::getNodeId(int i,int j,int k) const
{
	if( points ){
		return static_cast<kmb::nodeIdType>(i * (points->divy+1) * (points->divz+1) + j * (points->divz+1) + k );
	}else{
		return kmb::nullNodeId;
	}
}

kmb::elementIdType kmb::StructuredCell::getElementId(int i,int j,int k) const
{
	if( points ){
		return static_cast<kmb::elementIdType>(i * (points->divy) * (points->divz) + j * (points->divz) + k );
	}else{
		return kmb::Element::nullElementId;
	}
}
