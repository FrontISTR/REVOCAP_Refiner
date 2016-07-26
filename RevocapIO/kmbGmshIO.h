/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 2.0                          #
# Class Name : GmshIO                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2015/09/24     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

#pragma once
#include "MeshDB/kmbTypes.h"
#include <fstream>

namespace kmb{

class MeshData;

class GmshIO{
protected:
	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;
public:
#ifndef REVOCAP_SUPPORT_RUBY
	enum gmshFormat{
		k2_2,
		kUndefined
	};
	enum gmshElementType{
		kLine=1,
		kTriangle=2,
		kQuadrangle=3,
		kTetrahedron=4,
		kHexahedron=5,
		kPrism=6,
		kPyramid=7,
		kLine3=8,
		kTriangle6=9,
		kQuadrangle9=10,
		kTetrahedron10=11,
		kHexahedron27=12,
		kPrism18=13,
		kPyramid14=14,
		kPoint=15,
		kQuadrangle8=16,
		kHexahedron20=17,
		kPrism15=18,
		kPyramid13=19,
		kTriangle9=20,
		kTriangle10=21,
		kIncompleteTriangle12=22,
		kTriangle15=23,
		kIncompleteTriangle15=24,
		kTriangle21=25,
		kEdge4=26,
		kEdge5=27,
		kEdge6=28,
		kTetrahedron20=29,
		kTetrahedron35=30,
		kTetrahedron56=31,
		kHexahedron64=92,
		kHexahedron125=93,
		kUndefinedElement=-1
	};
#endif
	GmshIO();
	int loadMeshFromFile(const char* filename,kmb::MeshData* mesh);
protected:
	gmshFormat getVersion(std::ifstream &input) const;
	int readNodes_2_2(std::ifstream &input,kmb::MeshData* mesh);
	int readElements_2_2(std::ifstream &input,kmb::MeshData* mesh);
};

}
