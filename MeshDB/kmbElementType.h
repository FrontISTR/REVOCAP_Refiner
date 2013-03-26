/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementType                                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #                                                                      #
----------------------------------------------------------------------*/
#pragma once

namespace kmb{

enum elementType{
	UNKNOWNTYPE = -1,
	SEGMENT,
	SEGMENT2,
	TRIANGLE,
	TRIANGLE2,
	QUADRILATERAL,
	QUADRILATERAL2,
	TETRAHEDRON,
	TETRAHEDRON2,
	WEDGE,
	WEDGE2,
	PYRAMID,
	PYRAMID2,
	HEXAHEDRON,
	HEXAHEDRON2,
	ELEMENT_TYPE_NUM,
	TRIANGLE_INTERFACE,
	QUADRILATERAL_INTERFACE,
	VERTEX,
	POLYGON,
	POLYHEDRON,
	TWO_DIM_ONLY,
	THREE_DIM_ONLY,
	MIXED,
	OTHERTYPE,
	QUAD = QUADRILATERAL,
	QUAD2 = QUADRILATERAL2
};

}
