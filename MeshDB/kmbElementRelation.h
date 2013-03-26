/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementRelation                                         #
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

#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElement.h"

#include <map>

namespace kmb{

class Face;

class ElementRelation
{
public:

	enum relationType{
		UNKNOWNRELATION = -1,

		EQUAL,
		REVERSE,
		ADJACENT,
		ANTIADJACENT,

		BOUNDARY,
		ANTIBOUNDARY,

		COBOUNDARY,
		ANTICOBOUNDARY,

		COMMONEDGE,
		COMMONNODE,
		NOCOMMON,
		OTHERRELATION
	};

	static std::string	getRelationTypeString(kmb::ElementRelation::relationType rel);



	static kmb::ElementRelation::relationType
		getRelation(const kmb::ElementBase &eIter0, int &index0,
					const kmb::ElementBase &eIter1, int &index1 );


	static kmb::ElementRelation::relationType
		getSegmentRelation
			(int a0,int a1,int b0,int b1,int &aIndex,int &bIndex);

	static kmb::ElementRelation::relationType
		getTriangleRelation
			(int a0,int a1,int a2,int b0,int b1,int b2,int &aIndex,int &bIndex);

	static kmb::ElementRelation::relationType
		getQuadRelation
			(int a0,int a1,int a2,int a3,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);

	static kmb::ElementRelation::relationType
		getTriangleQuadRelation
			(int a0,int a1,int a2,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);

	static kmb::ElementRelation::relationType
		getTetrahedronRelation
			(int a0,int a1,int a2,int a3,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);



	static kmb::ElementRelation::relationType
		getTriangleTetrahedronRelation
			(int a0,int a1,int a2,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex);


	static bool isAdjacent(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );


	static bool clone( const kmb::ElementBase &eIter, kmb::nodeIdType *clonedCell );
	static bool clone( const kmb::ElementBase &eIter, kmb::nodeIdType *clonedCell, std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper );

	static bool reverse( const kmb::ElementBase &eIter, kmb::nodeIdType *reversedCell );
	static bool reverse( const kmb::ElementBase &eIter, kmb::nodeIdType *reversedCell, std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper );

	static kmb::elementType common( const kmb::ElementBase &eIter0, const kmb::ElementBase &eIter1, kmb::nodeIdType *commonCell );
	static kmb::elementType common( kmb::Face f0, kmb::Face f1, const kmb::ElementContainer* elements, kmb::nodeIdType *commonCell );

protected:


	static kmb::ElementRelation::relationType getRelationForSegment
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );


	static kmb::ElementRelation::relationType getRelationForTriangle
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );
	static kmb::ElementRelation::relationType getRelationForQuad
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );
	static kmb::ElementRelation::relationType getRelationForTetrahedron
		(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 );
};

}
