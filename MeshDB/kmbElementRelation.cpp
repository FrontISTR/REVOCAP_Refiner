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
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbElementRelation.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbHexahedron.h"

std::string
kmb::ElementRelation::getRelationTypeString(kmb::ElementRelation::relationType rel)
{
	switch(rel){
		case UNKNOWNRELATION:
			return std::string("UNKNOWNRELATION");
		case EQUAL:
			return std::string("EQUAL");
		case REVERSE:
			return std::string("REVERSE");
		case ADJACENT:
			return std::string("ADJACENT");
		case ANTIADJACENT:
			return std::string("ANTIADJACENT");
		case BOUNDARY:
			return std::string("BOUNDARY");
		case ANTIBOUNDARY:
			return std::string("ANTIBOUNDARY");
		case COBOUNDARY:
			return std::string("COBOUNDARY");
		case ANTICOBOUNDARY:
			return std::string("ANTICOBOUNDARY");
		case COMMONEDGE:
			return std::string("COMMONEDGE");
		case COMMONNODE:
			return std::string("COMMONNODE");
		case NOCOMMON:
			return std::string("NOCOMMON");
		case OTHERRELATION:
		default:
			return std::string("OTHERRELATION");
	};
}


kmb::ElementRelation::relationType
kmb::ElementRelation::getRelation(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 )
{
	index0 = -1;
	index1 = -1;
	kmb::ElementRelation::relationType ret = kmb::ElementRelation::UNKNOWNRELATION;
	if( eIter0.getType() == kmb::UNKNOWNTYPE || eIter1.getType() == kmb::UNKNOWNTYPE ){
		return ret;
	}
	switch( eIter0.getType() )
	{
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
		ret = kmb::ElementRelation::getRelationForSegment( eIter0, index0, eIter1, index1 );
		break;
	case kmb::TRIANGLE:
	case kmb::TRIANGLE2:
		ret = kmb::ElementRelation::getRelationForTriangle( eIter0, index0, eIter1, index1 );
		break;
	case kmb::QUAD:
	case kmb::QUAD2:
		ret = kmb::ElementRelation::getRelationForQuad( eIter0, index0, eIter1, index1 );
		break;
	case kmb::TETRAHEDRON:
	case kmb::TETRAHEDRON2:
		ret = kmb::ElementRelation::getRelationForTetrahedron( eIter0, index0, eIter1, index1 );
		break;
	case kmb::PYRAMID:
	case kmb::PYRAMID2:
	case kmb::WEDGE:
	case kmb::WEDGE2:
	case kmb::HEXAHEDRON:
	case kmb::HEXAHEDRON2:
		{
			switch( eIter1.getType() ){
			case kmb::SEGMENT:
			case kmb::SEGMENT2:
				ret = kmb::ElementRelation::getRelationForSegment( eIter1, index1, eIter0, index0 );
				break;
			case kmb::TRIANGLE:
			case kmb::TRIANGLE2:
				{
					kmb::ElementRelation::relationType r =
						kmb::ElementRelation::getRelationForTriangle( eIter1, index1, eIter0, index0 );
					switch( r ){
					case kmb::ElementRelation::BOUNDARY:
						ret = kmb::ElementRelation::COBOUNDARY;
						break;
					case kmb::ElementRelation::ANTIBOUNDARY:
						ret = kmb::ElementRelation::ANTICOBOUNDARY;
						break;
					default:
						ret = r;
						break;
					}
				}
				break;
			case kmb::QUAD:
			case kmb::QUAD2:
				{
					kmb::ElementRelation::relationType r =
						kmb::ElementRelation::getRelationForQuad( eIter1, index1, eIter0, index0 );
					switch( r ){
					case kmb::ElementRelation::BOUNDARY:
						ret = kmb::ElementRelation::COBOUNDARY;
						break;
					case kmb::ElementRelation::ANTIBOUNDARY:
						ret = kmb::ElementRelation::ANTICOBOUNDARY;
						break;
					default:
						ret = r;
						break;
					}
				}
				break;
			case kmb::TETRAHEDRON:
			case kmb::TETRAHEDRON2:
				ret = kmb::ElementRelation::getRelationForTetrahedron( eIter1, index1, eIter0, index0 );
				break;
			case kmb::PYRAMID:
			case kmb::PYRAMID2:
			case kmb::WEDGE:
			case kmb::WEDGE2:
			case kmb::HEXAHEDRON:
			case kmb::HEXAHEDRON2:
				{
					int indices[8];
					int count = 0;
					const int vertexCount = eIter0.getVertexCount();
					for(int i=0;i<vertexCount;++i){
						indices[i] = eIter1.indexOf( eIter0.getCellId(i) );
						if( indices[i] >= eIter1.getVertexCount() ){
							indices[i] = -1;
						}
						if( indices[i] >= 0 ){
							++count;
						}
					}
					switch(count){
					case 0:
						ret = kmb::ElementRelation::NOCOMMON;
						break;
					case 1:
						ret = kmb::ElementRelation::COMMONNODE;
						break;
					case 2:
						{
							ret = kmb::ElementRelation::OTHERRELATION;
							for(int i0=0;i0<8;++i0){
								for(int i1=i0+1;i1<8;++i1){
									if( eIter0.isConnected(i0,i1) != 0 &&
										eIter1.isConnected(indices[i0],indices[i1]) != 0 )
									{
										ret = kmb::ElementRelation::COMMONEDGE;
										goto END_EDGE_CHECK;
									}
								}
							}
						}
						END_EDGE_CHECK:
						break;
					case 3:
					case 4:
						{

							const int boundaryCount = eIter0.getBoundaryCount();
							ret = kmb::ElementRelation::OTHERRELATION;
							for(int i=0;i<boundaryCount;++i){
								const int vertexCount0 = eIter0.getBoundaryVertexCount(i);
								int res = 0;
								switch( eIter0.getType() ){
								case kmb::PYRAMID:
								case kmb::PYRAMID2:
									if( vertexCount0 == 3 ){
										res = eIter1.isFace(
											indices[ kmb::Pyramid::faceTable[i][0] ],
											indices[ kmb::Pyramid::faceTable[i][1] ],
											indices[ kmb::Pyramid::faceTable[i][2] ], index1 );
									}else if( vertexCount0 == 4 ){
										res = eIter1.isFace(
											indices[ kmb::Pyramid::faceTable[i][0] ],
											indices[ kmb::Pyramid::faceTable[i][1] ],
											indices[ kmb::Pyramid::faceTable[i][2] ],
											indices[ kmb::Pyramid::faceTable[i][3] ], index1 );
									}
									break;
								case kmb::WEDGE:
								case kmb::WEDGE2:
									if( vertexCount0 == 3 ){
										res = eIter1.isFace(
											indices[ kmb::Wedge::faceTable[i][0] ],
											indices[ kmb::Wedge::faceTable[i][1] ],
											indices[ kmb::Wedge::faceTable[i][2] ], index1 );
									}else if( vertexCount0 == 4 ){
										res = eIter1.isFace(
											indices[ kmb::Wedge::faceTable[i][0] ],
											indices[ kmb::Wedge::faceTable[i][1] ],
											indices[ kmb::Wedge::faceTable[i][2] ],
											indices[ kmb::Wedge::faceTable[i][3] ], index1 );
									}
									break;
								case kmb::HEXAHEDRON:
								case kmb::HEXAHEDRON2:
									if( vertexCount0 == 4 ){
										res = eIter1.isFace(
											indices[ kmb::Hexahedron::faceTable[i][0] ],
											indices[ kmb::Hexahedron::faceTable[i][1] ],
											indices[ kmb::Hexahedron::faceTable[i][2] ],
											indices[ kmb::Hexahedron::faceTable[i][3] ], index1 );
									}
									break;
								default:
									break;
								}
								if(res == 1){
									index0 = i;
									ret = kmb::ElementRelation::ANTIADJACENT;
									break;
								}else if(res == -1){
									index0 = i;
									ret = kmb::ElementRelation::ADJACENT;
									break;
								}
							}
						}
						break;
					case 5:
						ret = kmb::ElementRelation::OTHERRELATION;
						if( ( eIter0.getType() == kmb::PYRAMID || eIter0.getType() == kmb::PYRAMID2 ) &&
							( eIter1.getType() == kmb::PYRAMID || eIter1.getType() == kmb::PYRAMID2 ) )
						{
							int i0 = -1;
							int i1 = -1;
							if( eIter0.getCellId(0) == eIter1.getCellId(0) ){
								ret =
									kmb::ElementRelation::getQuadRelation(
										eIter0.getCellId(1), eIter0.getCellId(2), eIter0.getCellId(3), eIter0.getCellId(4),
										eIter1.getCellId(1), eIter1.getCellId(2), eIter1.getCellId(3), eIter1.getCellId(4),
										i0,i1);
							}
						}
						break;
					case 6:
						ret = kmb::ElementRelation::OTHERRELATION;
						if( ( eIter0.getType() == kmb::WEDGE || eIter0.getType() == kmb::WEDGE2 ) &&
							( eIter1.getType() == kmb::WEDGE || eIter1.getType() == kmb::WEDGE2 ) )
						{
							if( kmb::Wedge::isEquivalent( indices ) ){
								int ind1 = -1;
								switch( eIter1.isFace(indices[0],indices[2],indices[1],ind1) ){
								case 1:
									ret = kmb::ElementRelation::EQUAL;
									break;
								case -1:
									ret = kmb::ElementRelation::REVERSE;
									break;
								default:
									break;
								}
							}
						}
						break;
					case 8:
						ret = kmb::ElementRelation::OTHERRELATION;
						if( ( eIter0.getType() == kmb::HEXAHEDRON || eIter0.getType() == kmb::HEXAHEDRON2 ) &&
							( eIter1.getType() == kmb::HEXAHEDRON || eIter1.getType() == kmb::HEXAHEDRON2 ) )
						{
							if( kmb::Hexahedron::isEquivalent( indices ) ){
								int ind1 = -1;
								switch( eIter1.isFace(indices[3],indices[2],indices[1],indices[0],ind1) ){
								case 1:
									ret = kmb::ElementRelation::EQUAL;
									break;
								case -1:
									ret = kmb::ElementRelation::REVERSE;
									break;
								default:
									break;
								}
							}
						}
						break;
					default:
						ret = kmb::ElementRelation::OTHERRELATION;
						break;
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return ret;
}

bool
kmb::ElementRelation::isAdjacent(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 )
{
	index0 = -1;
	index1 = -1;
	bool ret = true;
	if( eIter0.getType() == kmb::TETRAHEDRON && eIter1.getType() == kmb::TETRAHEDRON ){
		unsigned int rel = 0;
		if( eIter0.getCellId(0) == eIter1.getCellId(0) )	rel |= 0x1000;
		if( eIter0.getCellId(0) == eIter1.getCellId(1) )	rel |= 0x2000;
		if( eIter0.getCellId(0) == eIter1.getCellId(2) )	rel |= 0x4000;
		if( eIter0.getCellId(0) == eIter1.getCellId(3) )	rel |= 0x8000;
		if( eIter0.getCellId(1) == eIter1.getCellId(0) )	rel |= 0x0100;
		if( eIter0.getCellId(1) == eIter1.getCellId(1) )	rel |= 0x0200;
		if( eIter0.getCellId(1) == eIter1.getCellId(2) )	rel |= 0x0400;
		if( eIter0.getCellId(1) == eIter1.getCellId(3) )	rel |= 0x0800;
		if( eIter0.getCellId(2) == eIter1.getCellId(0) )	rel |= 0x0010;
		if( eIter0.getCellId(2) == eIter1.getCellId(1) )	rel |= 0x0020;
		if( eIter0.getCellId(2) == eIter1.getCellId(2) )	rel |= 0x0040;
		if( eIter0.getCellId(2) == eIter1.getCellId(3) )	rel |= 0x0080;
		if( eIter0.getCellId(3) == eIter1.getCellId(0) )	rel |= 0x0001;
		if( eIter0.getCellId(3) == eIter1.getCellId(1) )	rel |= 0x0002;
		if( eIter0.getCellId(3) == eIter1.getCellId(2) )	rel |= 0x0004;
		if( eIter0.getCellId(3) == eIter1.getCellId(3) )	rel |= 0x0008;
		switch( rel )
		{





























			case 0x0248:
			case 0x0482:
			case 0x0824:
				index0 = 0;	index1 = 0;	ret = false;	break;

			case 0x0284:
			case 0x0842:
			case 0x0428:
				index0 = 0;	index1 = 0;	ret = true;		break;

			case 0x0418:
			case 0x0184:
			case 0x0841:
				index0 = 0;	index1 = 1;	ret = false;	break;

			case 0x0481:
			case 0x0814:
			case 0x0148:
				index0 = 0;	index1 = 1;	ret = true;		break;

			case 0x0128:
			case 0x0281:
			case 0x0812:
				index0 = 0;	index1 = 2;	ret = false;	break;

			case 0x0182:
			case 0x0821:
			case 0x0218:
				index0 = 0;	index1 = 2;	ret = true;		break;

			case 0x0421:
			case 0x0214:
			case 0x0142:
				index0 = 0;	index1 = 3;	ret = false;	break;

			case 0x0412:
			case 0x0124:
			case 0x0241:
				index0 = 0;	index1 = 3;	ret = true;		break;


			case 0x2084:
			case 0x8042:
			case 0x4028:
				index0 = 1;	index1 = 0;	ret = false;	break;

			case 0x2048:
			case 0x4082:
			case 0x8024:
				index0 = 1;	index1 = 0;	ret = true;		break;

			case 0x1048:
			case 0x4081:
			case 0x8014:
				index0 = 1;	index1 = 1;	ret = false;	break;

			case 0x1084:
			case 0x4018:
			case 0x8041:
				index0 = 1;	index1 = 1;	ret = true;		break;

			case 0x1082:
			case 0x8021:
			case 0x2018:
				index0 = 1;	index1 = 2;	ret = false;	break;

			case 0x1028:
			case 0x8012:
			case 0x2081:
				index0 = 1;	index1 = 2;	ret = true;		break;

			case 0x1024:
			case 0x2041:
			case 0x4012:
				index0 = 1;	index1 = 3;	ret = false;	break;

			case 0x1042:
			case 0x2014:
			case 0x4021:
				index0 = 1;	index1 = 3;	ret = true;		break;


			case 0x2408:
			case 0x4802:
			case 0x8204:
				index0 = 2;	index1 = 0;	ret = false;	break;

			case 0x4208:
			case 0x8402:
			case 0x2804:
				index0 = 2;	index1 = 0;	ret = true;		break;

			case 0x1804:
			case 0x8401:
			case 0x4108:
				index0 = 2;	index1 = 1;	ret = false;	break;

			case 0x8104:
			case 0x4801:
			case 0x1408:
				index0 = 2;	index1 = 1;	ret = true;		break;

			case 0x1208:
			case 0x2801:
			case 0x8102:
				index0 = 2;	index1 = 2;	ret = false;	break;

			case 0x2108:
			case 0x8201:
			case 0x1802:
				index0 = 2;	index1 = 2;	ret = true;		break;

			case 0x1402:
			case 0x4201:
			case 0x2104:
				index0 = 2;	index1 = 3;	ret = false;	break;

			case 0x4102:
			case 0x2401:
			case 0x1204:
				index0 = 2;	index1 = 3;	ret = true;		break;


			case 0x2840:
			case 0x8420:
			case 0x4280:
				index0 = 3;	index1 = 0;	ret = false;	break;

			case 0x2480:
			case 0x8240:
			case 0x4820:
				index0 = 3;	index1 = 0;	ret = true;		break;

			case 0x1480:
			case 0x4810:
			case 0x8140:
				index0 = 3;	index1 = 1;	ret = false;	break;

			case 0x1840:
			case 0x4180:
			case 0x8410:
				index0 = 3;	index1 = 1;	ret = true;		break;

			case 0x1820:
			case 0x8210:
			case 0x2180:
				index0 = 3;	index1 = 2;	ret = false;	break;

			case 0x1280:
			case 0x8120:
			case 0x2810:
				index0 = 3;	index1 = 2;	ret = true;		break;

			case 0x1240:
			case 0x2410:
			case 0x4120:
				index0 = 3;	index1 = 3;	ret = false;	break;

			case 0x1420:
			case 0x2140:
			case 0x4210:
				index0 = 3;	index1 = 3;	ret = true;		break;
			default:
				break;
		}
	}
	return ret;
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getSegmentRelation
(int a0,int a1,int b0,int b1,int &aIndex,int &bIndex)
{
	aIndex = -1;
	bIndex = -1;
	unsigned int rel = 0;
	if( a0 == b0 ) rel |= 0x01;
	if( a0 == b1 ) rel |= 0x02;
	if( a1 == b0 ) rel |= 0x10;
	if( a1 == b1 ) rel |= 0x20;
	switch( rel ){
	case 0x21:
		return kmb::ElementRelation::EQUAL;
	case 0x12:
		return kmb::ElementRelation::REVERSE;
	case 0x00:
		return kmb::ElementRelation::NOCOMMON;
	case 0x01:
		aIndex = 0;
		bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x02:
		aIndex = 0;
		bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x10:
		aIndex = 1;
		bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x20:
		aIndex = 1;
		bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	default:
		break;
	}
	return kmb::ElementRelation::UNKNOWNRELATION;
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getTriangleRelation
(int a0,int a1,int a2,int b0,int b1,int b2,int &aIndex,int &bIndex)
{
	aIndex = -1;
	bIndex = -1;
	unsigned int rel = 0;
	if( a0 == b0 )	rel |= 0x100;
	if( a0 == b1 )	rel |= 0x200;
	if( a0 == b2 )	rel |= 0x400;
	if( a1 == b0 )	rel |= 0x010;
	if( a1 == b1 )	rel |= 0x020;
	if( a1 == b2 )	rel |= 0x040;
	if( a2 == b0 )	rel |= 0x001;
	if( a2 == b1 )	rel |= 0x002;
	if( a2 == b2 )	rel |= 0x004;
	switch( rel ){

		case 0x124:
		case 0x241:
		case 0x412:
			return kmb::ElementRelation::EQUAL;

		case 0x421:
		case 0x214:
		case 0x142:
			return kmb::ElementRelation::REVERSE;

		case 0x120:
			aIndex = 2; bIndex = 2;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x240:
			aIndex = 2; bIndex = 0;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x410:
			aIndex = 2; bIndex = 1;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x012:
			aIndex = 0; bIndex = 2;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x024:
			aIndex = 0; bIndex = 0;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x041:
			aIndex = 0; bIndex = 1;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x201:
			aIndex = 1; bIndex = 2;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x402:
			aIndex = 1; bIndex = 0;
			return kmb::ElementRelation::ANTIADJACENT;
		case 0x104:
			aIndex = 1; bIndex = 1;
			return kmb::ElementRelation::ANTIADJACENT;

		case 0x210:
			aIndex = 2; bIndex = 2;
			return kmb::ElementRelation::ADJACENT;
		case 0x420:
			aIndex = 2; bIndex = 0;
			return kmb::ElementRelation::ADJACENT;
		case 0x140:
			aIndex = 2; bIndex = 1;
			return kmb::ElementRelation::ADJACENT;
		case 0x021:
			aIndex = 0; bIndex = 2;
			return kmb::ElementRelation::ADJACENT;
		case 0x042:
			aIndex = 0; bIndex = 0;
			return kmb::ElementRelation::ADJACENT;
		case 0x014:
			aIndex = 0; bIndex = 1;
			return kmb::ElementRelation::ADJACENT;
		case 0x102:
			aIndex = 1; bIndex = 2;
			return kmb::ElementRelation::ADJACENT;
		case 0x204:
			aIndex = 1; bIndex = 0;
			return kmb::ElementRelation::ADJACENT;
		case 0x401:
			aIndex = 1; bIndex = 1;
			return kmb::ElementRelation::ADJACENT;
		case 0:
			return kmb::ElementRelation::NOCOMMON;
		default:
			return kmb::ElementRelation::COMMONNODE;
	}
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getQuadRelation
(int a0,int a1,int a2,int a3,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex)
{
	aIndex = -1;
	bIndex = -1;
	unsigned int rel = 0;
	int count = 0;
	if( a0 == b0 ){	rel |= 0x1000;	++count;	}
	if( a0 == b1 ){	rel |= 0x2000;	++count;	}
	if( a0 == b2 ){	rel |= 0x4000;	++count;	}
	if( a0 == b3 ){	rel |= 0x8000;	++count;	}
	if( a1 == b0 ){	rel |= 0x0100;	++count;	}
	if( a1 == b1 ){	rel |= 0x0200;	++count;	}
	if( a1 == b2 ){	rel |= 0x0400;	++count;	}
	if( a1 == b3 ){	rel |= 0x0800;	++count;	}
	if( a2 == b0 ){	rel |= 0x0010;	++count;	}
	if( a2 == b1 ){	rel |= 0x0020;	++count;	}
	if( a2 == b2 ){	rel |= 0x0040;	++count;	}
	if( a2 == b3 ){	rel |= 0x0080;	++count;	}
	if( a3 == b0 ){	rel |= 0x0001;	++count;	}
	if( a3 == b1 ){	rel |= 0x0002;	++count;	}
	if( a3 == b2 ){	rel |= 0x0004;	++count;	}
	if( a3 == b3 ){	rel |= 0x0008;	++count;	}
	if( count == 0 ){
		return kmb::ElementRelation::NOCOMMON;
	}else if( count == 1 ){
		return kmb::ElementRelation::COMMONNODE;
	}
	switch( rel ){

	case 0x1248:
	case 0x2481:
	case 0x4812:
	case 0x8124:
		return kmb::ElementRelation::EQUAL;

	case 0x1842:
	case 0x8421:
	case 0x4218:
	case 0x2184:
		return kmb::ElementRelation::REVERSE;

	case 0x1200:
		aIndex = 0;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x2400:
		aIndex = 0;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x4800:
		aIndex = 0;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x8100:
		aIndex = 0;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0120:
		aIndex = 1;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0240:
		aIndex = 1;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0480:
		aIndex = 1;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0810:
		aIndex = 1;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0012:
		aIndex = 2;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0024:
		aIndex = 2;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0048:
		aIndex = 2;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x0081:
		aIndex = 2;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x2001:
		aIndex = 3;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x4002:
		aIndex = 3;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x8004:
		aIndex = 3;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x1008:
		aIndex = 3;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x2100:
		aIndex = 0;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x4200:
		aIndex = 0;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x8400:
		aIndex = 0;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;
	case 0x1800:
		aIndex = 0;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	case 0x0210:
		aIndex = 1;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x0420:
		aIndex = 1;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x0840:
		aIndex = 1;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;
	case 0x0180:
		aIndex = 1;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	case 0x0021:
		aIndex = 2;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x0042:
		aIndex = 2;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x0084:
		aIndex = 2;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;
	case 0x0018:
		aIndex = 2;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	case 0x1002:
		aIndex = 3;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x2004:
		aIndex = 3;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x4008:
		aIndex = 3;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;
	case 0x8001:
		aIndex = 3;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	default:
		return kmb::ElementRelation::OTHERRELATION;
	}
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getTriangleQuadRelation
(int a0,int a1,int a2,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex)
{
	aIndex = -1;
	bIndex = -1;
	unsigned int rel = 0;
	int count = 0;
	if( a0 == b0 ){	rel |= 0x100;	++count;	}
	if( a0 == b1 ){	rel |= 0x200;	++count;	}
	if( a0 == b2 ){	rel |= 0x400;	++count;	}
	if( a0 == b3 ){	rel |= 0x800;	++count;	}
	if( a1 == b0 ){	rel |= 0x010;	++count;	}
	if( a1 == b1 ){	rel |= 0x020;	++count;	}
	if( a1 == b2 ){	rel |= 0x040;	++count;	}
	if( a1 == b3 ){	rel |= 0x080;	++count;	}
	if( a2 == b0 ){	rel |= 0x001;	++count;	}
	if( a2 == b1 ){	rel |= 0x002;	++count;	}
	if( a2 == b2 ){	rel |= 0x004;	++count;	}
	if( a2 == b3 ){	rel |= 0x008;	++count;	}
	if( count == 0 ){
		return kmb::ElementRelation::NOCOMMON;
	}else if( count == 1 ){
		return kmb::ElementRelation::COMMONNODE;
	}else if( count == 3 ){
		return kmb::ElementRelation::OTHERRELATION;
	}
	switch( rel ){

	case 0x012:
		aIndex = 0;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x024:
		aIndex = 0;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x048:
		aIndex = 0;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x081:
		aIndex = 0;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x201:
		aIndex = 1;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x402:
		aIndex = 1;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x804:
		aIndex = 1;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x108:
		aIndex = 1;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x120:
		aIndex = 2;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x240:
		aIndex = 2;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x480:
		aIndex = 2;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;
	case 0x810:
		aIndex = 2;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x021:
		aIndex = 0;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x042:
		aIndex = 0;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x084:
		aIndex = 0;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;
	case 0x018:
		aIndex = 0;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	case 0x102:
		aIndex = 1;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x204:
		aIndex = 1;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x408:
		aIndex = 1;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;
	case 0x801:
		aIndex = 1;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	case 0x210:
		aIndex = 2;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;
	case 0x420:
		aIndex = 2;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;
	case 0x840:
		aIndex = 2;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;
	case 0x180:
		aIndex = 2;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	default:
		return kmb::ElementRelation::OTHERRELATION;
	}
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getTetrahedronRelation
(int a0,int a1,int a2,int a3,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex)
{
	aIndex = -1;
	bIndex = -1;
	unsigned int rel = 0;
	int count = 0;
	if( a0 == b0 ){	rel |= 0x1000;	++count;	}
	if( a0 == b1 ){	rel |= 0x2000;	++count;	}
	if( a0 == b2 ){	rel |= 0x4000;	++count;	}
	if( a0 == b3 ){	rel |= 0x8000;	++count;	}
	if( a1 == b0 ){	rel |= 0x0100;	++count;	}
	if( a1 == b1 ){	rel |= 0x0200;	++count;	}
	if( a1 == b2 ){	rel |= 0x0400;	++count;	}
	if( a1 == b3 ){	rel |= 0x0800;	++count;	}
	if( a2 == b0 ){	rel |= 0x0010;	++count;	}
	if( a2 == b1 ){	rel |= 0x0020;	++count;	}
	if( a2 == b2 ){	rel |= 0x0040;	++count;	}
	if( a2 == b3 ){	rel |= 0x0080;	++count;	}
	if( a3 == b0 ){	rel |= 0x0001;	++count;	}
	if( a3 == b1 ){	rel |= 0x0002;	++count;	}
	if( a3 == b2 ){	rel |= 0x0004;	++count;	}
	if( a3 == b3 ){	rel |= 0x0008;	++count;	}
	if( count == 0 ){
		return kmb::ElementRelation::NOCOMMON;
	}else if( count == 1 ){
		return kmb::ElementRelation::COMMONNODE;
	}else if( count == 2 ){
		return kmb::ElementRelation::COMMONEDGE;
	}
	switch( rel ){

	case 0x1248:
	case 0x1482:
	case 0x1824:
	case 0x2418:
	case 0x2184:
	case 0x2841:
	case 0x4128:
	case 0x4281:
	case 0x4812:
	case 0x8421:
	case 0x8214:
	case 0x8142:
		return kmb::ElementRelation::EQUAL;

	case 0x1284:
	case 0x1428:
	case 0x1842:
	case 0x2481:
	case 0x2148:
	case 0x2814:
	case 0x4182:
	case 0x4218:
	case 0x4821:
	case 0x8412:
	case 0x8241:
	case 0x8124:
		return kmb::ElementRelation::REVERSE;

	case 0x0248:
	case 0x0482:
	case 0x0824:
		aIndex = 0;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x0284:
	case 0x0842:
	case 0x0428:
		aIndex = 0;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;

	case 0x0418:
	case 0x0184:
	case 0x0841:
		aIndex = 0;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x0481:
	case 0x0814:
	case 0x0148:
		aIndex = 0;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;

	case 0x0128:
	case 0x0281:
	case 0x0812:
		aIndex = 0;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x0182:
	case 0x0821:
	case 0x0218:
		aIndex = 0;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;

	case 0x0421:
	case 0x0214:
	case 0x0142:
		aIndex = 0;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x0412:
	case 0x0124:
	case 0x0241:
		aIndex = 0;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;

	case 0x2084:
	case 0x8042:
	case 0x4028:
		aIndex = 1;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x2048:
	case 0x4082:
	case 0x8024:
		aIndex = 1;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;

	case 0x1048:
	case 0x4081:
	case 0x8014:
		aIndex = 1;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x1084:
	case 0x4018:
	case 0x8041:
		aIndex = 1;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;

	case 0x1082:
	case 0x8021:
	case 0x2018:
		aIndex = 1;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x1028:
	case 0x8012:
	case 0x2081:
		aIndex = 1;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;

	case 0x1024:
	case 0x2041:
	case 0x4012:
		aIndex = 1;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x1042:
	case 0x2014:
	case 0x4021:
		aIndex = 1;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;

	case 0x2408:
	case 0x4802:
	case 0x8204:
		aIndex = 2;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x4208:
	case 0x8402:
	case 0x2804:
		aIndex = 2;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;

	case 0x1804:
	case 0x8401:
	case 0x4108:
		aIndex = 2;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x8104:
	case 0x4801:
	case 0x1408:
		aIndex = 2;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;

	case 0x1208:
	case 0x2801:
	case 0x8102:
		aIndex = 2;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x2108:
	case 0x8201:
	case 0x1802:
		aIndex = 2;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;

	case 0x1402:
	case 0x4201:
	case 0x2104:
		aIndex = 2;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x4102:
	case 0x2401:
	case 0x1204:
		aIndex = 2;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;

	case 0x2840:
	case 0x8420:
	case 0x4280:
		aIndex = 3;	bIndex = 0;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x2480:
	case 0x8240:
	case 0x4820:
		aIndex = 3;	bIndex = 0;
		return kmb::ElementRelation::ADJACENT;

	case 0x1480:
	case 0x4810:
	case 0x8140:
		aIndex = 3;	bIndex = 1;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x1840:
	case 0x4180:
	case 0x8410:
		aIndex = 3;	bIndex = 1;
		return kmb::ElementRelation::ADJACENT;

	case 0x1820:
	case 0x8210:
	case 0x2180:
		aIndex = 3;	bIndex = 2;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x1280:
	case 0x8120:
	case 0x2810:
		aIndex = 3;	bIndex = 2;
		return kmb::ElementRelation::ADJACENT;

	case 0x1240:
	case 0x2410:
	case 0x4120:
		aIndex = 3;	bIndex = 3;
		return kmb::ElementRelation::ANTIADJACENT;

	case 0x1420:
	case 0x2140:
	case 0x4210:
		aIndex = 3;	bIndex = 3;
		return kmb::ElementRelation::ADJACENT;
	default:
		break;
	}
	return kmb::ElementRelation::OTHERRELATION;
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getTriangleTetrahedronRelation
(int a0,int a1,int a2,int b0,int b1,int b2,int b3,int &aIndex,int &bIndex)
{
	aIndex = -1;
	bIndex = -1;
	unsigned int rel = 0;
	int count = 0;
	if( a0 == b0 ){	rel |= 0x100;	++count;	}
	if( a0 == b1 ){	rel |= 0x200;	++count;	}
	if( a0 == b2 ){	rel |= 0x400;	++count;	}
	if( a0 == b3 ){	rel |= 0x800;	++count;	}
	if( a1 == b0 ){	rel |= 0x010;	++count;	}
	if( a1 == b1 ){	rel |= 0x020;	++count;	}
	if( a1 == b2 ){	rel |= 0x040;	++count;	}
	if( a1 == b3 ){	rel |= 0x080;	++count;	}
	if( a2 == b0 ){	rel |= 0x001;	++count;	}
	if( a2 == b1 ){	rel |= 0x002;	++count;	}
	if( a2 == b2 ){	rel |= 0x004;	++count;	}
	if( a2 == b3 ){	rel |= 0x008;	++count;	}
	if( count == 0 ){
		return kmb::ElementRelation::NOCOMMON;
	}
	switch( rel ){

	case 0x248:
	case 0x824:
	case 0x482:
		bIndex = 0;
		return kmb::ElementRelation::BOUNDARY;
	case 0x284:
	case 0x842:
	case 0x428:
		bIndex = 0;
		return kmb::ElementRelation::ANTIBOUNDARY;

	case 0x184:
	case 0x418:
	case 0x841:
		bIndex = 1;
		return kmb::ElementRelation::BOUNDARY;
	case 0x148:
	case 0x481:
	case 0x814:
		bIndex = 1;
		return kmb::ElementRelation::ANTIBOUNDARY;

	case 0x128:
	case 0x812:
	case 0x281:
		bIndex = 2;
		return kmb::ElementRelation::BOUNDARY;
	case 0x182:
	case 0x821:
	case 0x218:
		bIndex = 2;
		return kmb::ElementRelation::ANTIBOUNDARY;

	case 0x142:
	case 0x214:
	case 0x421:
		bIndex = 3;
		return kmb::ElementRelation::BOUNDARY;
	case 0x124:
	case 0x241:
	case 0x412:
		bIndex = 3;
		return kmb::ElementRelation::ANTIBOUNDARY;

	case 0x024:
	case 0x042:
		aIndex = 0;
		bIndex = 0;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x014:
	case 0x041:
		aIndex = 0;
		bIndex = 1;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x012:
	case 0x021:
		aIndex = 0;
		bIndex = 2;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x018:
	case 0x081:
		aIndex = 0;
		bIndex = 3;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x028:
	case 0x082:
		aIndex = 0;
		bIndex = 4;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x048:
	case 0x084:
		aIndex = 0;
		bIndex = 5;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x402:
	case 0x204:
		aIndex = 1;
		bIndex = 0;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x401:
	case 0x104:
		aIndex = 1;
		bIndex = 1;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x201:
	case 0x102:
		aIndex = 1;
		bIndex = 2;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x801:
	case 0x108:
		aIndex = 1;
		bIndex = 3;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x802:
	case 0x208:
		aIndex = 1;
		bIndex = 4;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x804:
	case 0x408:
		aIndex = 1;
		bIndex = 5;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x240:
	case 0x420:
		aIndex = 2;
		bIndex = 0;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x140:
	case 0x410:
		aIndex = 2;
		bIndex = 1;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x120:
	case 0x210:
		aIndex = 2;
		bIndex = 2;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x180:
	case 0x810:
		aIndex = 2;
		bIndex = 3;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x280:
	case 0x820:
		aIndex = 2;
		bIndex = 4;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x480:
	case 0x840:
		aIndex = 2;
		bIndex = 5;
		return kmb::ElementRelation::COMMONEDGE;

	case 0x100:
		aIndex = 0;
		bIndex = 0;
		return kmb::ElementRelation::COMMONNODE;

	case 0x200:
		aIndex = 0;
		bIndex = 1;
		return kmb::ElementRelation::COMMONNODE;

	case 0x400:
		aIndex = 0;
		bIndex = 2;
		return kmb::ElementRelation::COMMONNODE;

	case 0x800:
		aIndex = 0;
		bIndex = 3;
		return kmb::ElementRelation::COMMONNODE;

	case 0x010:
		aIndex = 1;
		bIndex = 0;
		return kmb::ElementRelation::COMMONNODE;

	case 0x020:
		aIndex = 1;
		bIndex = 1;
		return kmb::ElementRelation::COMMONNODE;

	case 0x040:
		aIndex = 1;
		bIndex = 2;
		return kmb::ElementRelation::COMMONNODE;

	case 0x080:
		aIndex = 1;
		bIndex = 3;
		return kmb::ElementRelation::COMMONNODE;

	case 0x001:
		aIndex = 2;
		bIndex = 0;
		return kmb::ElementRelation::COMMONNODE;

	case 0x002:
		aIndex = 2;
		bIndex = 1;
		return kmb::ElementRelation::COMMONNODE;

	case 0x004:
		aIndex = 2;
		bIndex = 2;
		return kmb::ElementRelation::COMMONNODE;

	case 0x008:
		aIndex = 2;
		bIndex = 3;
		return kmb::ElementRelation::COMMONNODE;
	default:
		break;
	}
	return kmb::ElementRelation::OTHERRELATION;
}

bool
kmb::ElementRelation::clone( const kmb::ElementBase &eIter, kmb::nodeIdType *clonedCell )
{
	bool retVal = false;
	if( clonedCell == NULL ){
		return retVal;
	}
	const int len = eIter.getNodeCount();
	for(int i=0;i<len;++i){
		clonedCell[i] = eIter.getCellId(i);
	}
	return retVal;
}

bool
kmb::ElementRelation::clone( const kmb::ElementBase &eIter, kmb::nodeIdType *clonedCell, std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper )
{
	bool retVal = false;
	if( clonedCell == NULL ){
		return retVal;
	}
	const int len = eIter.getNodeCount();
	for(int i=0;i<len;++i){
		kmb::nodeIdType nodeId = eIter.getCellId(i);
		std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator nIter = nodeMapper.find( nodeId );
		if( nIter != nodeMapper.end() ){
			clonedCell[i] = nIter->second;
		}else{
			clonedCell[i] = nodeId;
		}
	}
	retVal = true;
	return retVal;
}

bool
kmb::ElementRelation::reverse( const kmb::ElementBase &eIter, kmb::nodeIdType *reversedCell )
{
	bool retVal = false;
	if( reversedCell == NULL ){
		return retVal;
	}
	switch( eIter.getType() )
	{
	case kmb::SEGMENT:
		reversedCell[0] = eIter.getCellId(1);
		reversedCell[1] = eIter.getCellId(0);
		retVal = true;
		break;
	case kmb::SEGMENT2:
		reversedCell[0] = eIter.getCellId(1);
		reversedCell[1] = eIter.getCellId(0);

		reversedCell[2] = eIter.getCellId(2);
		retVal = true;
		break;
	case kmb::TRIANGLE:
		reversedCell[0] = eIter.getCellId(2);
		reversedCell[1] = eIter.getCellId(1);
		reversedCell[2] = eIter.getCellId(0);
		retVal = true;
		break;
	case kmb::TRIANGLE2:
		reversedCell[0] = eIter.getCellId(2);
		reversedCell[1] = eIter.getCellId(1);
		reversedCell[2] = eIter.getCellId(0);

		reversedCell[3] = eIter.getCellId(5);
		reversedCell[4] = eIter.getCellId(4);
		reversedCell[5] = eIter.getCellId(3);
		retVal = true;
		break;
	case kmb::QUAD:
		reversedCell[0] = eIter.getCellId(3);
		reversedCell[1] = eIter.getCellId(2);
		reversedCell[2] = eIter.getCellId(1);
		reversedCell[3] = eIter.getCellId(0);
		retVal = true;
		break;
	case kmb::QUAD2:
		reversedCell[0] = eIter.getCellId(3);
		reversedCell[1] = eIter.getCellId(2);
		reversedCell[2] = eIter.getCellId(1);
		reversedCell[3] = eIter.getCellId(0);

		reversedCell[4] = eIter.getCellId(6);
		reversedCell[5] = eIter.getCellId(5);
		reversedCell[6] = eIter.getCellId(4);
		reversedCell[7] = eIter.getCellId(7);
		retVal = true;
		break;
	case kmb::TETRAHEDRON:
		reversedCell[0] = eIter.getCellId(1);
		reversedCell[1] = eIter.getCellId(0);
		reversedCell[2] = eIter.getCellId(2);
		reversedCell[3] = eIter.getCellId(3);
		retVal = true;
		break;
	case kmb::TETRAHEDRON2:
		reversedCell[0] = eIter.getCellId(1);
		reversedCell[1] = eIter.getCellId(0);
		reversedCell[2] = eIter.getCellId(2);
		reversedCell[3] = eIter.getCellId(3);

		reversedCell[4] = eIter.getCellId(5);
		reversedCell[5] = eIter.getCellId(4);
		reversedCell[6] = eIter.getCellId(6);
		reversedCell[7] = eIter.getCellId(8);
		reversedCell[8] = eIter.getCellId(7);
		reversedCell[9] = eIter.getCellId(9);
		retVal = true;
		break;
	case kmb::WEDGE:
		reversedCell[0] = eIter.getCellId(0);
		reversedCell[1] = eIter.getCellId(2);
		reversedCell[2] = eIter.getCellId(1);
		reversedCell[3] = eIter.getCellId(3);
		reversedCell[4] = eIter.getCellId(5);
		reversedCell[5] = eIter.getCellId(4);
		retVal = true;
		break;
	case kmb::WEDGE2:
		reversedCell[0] = eIter.getCellId(0);
		reversedCell[1] = eIter.getCellId(2);
		reversedCell[2] = eIter.getCellId(1);
		reversedCell[3] = eIter.getCellId(3);
		reversedCell[4] = eIter.getCellId(5);
		reversedCell[5] = eIter.getCellId(4);

		reversedCell[6] = eIter.getCellId(6);
		reversedCell[7] = eIter.getCellId(8);
		reversedCell[8] = eIter.getCellId(7);
		reversedCell[9] = eIter.getCellId(9);
		reversedCell[10] = eIter.getCellId(11);
		reversedCell[11] = eIter.getCellId(10);
		reversedCell[12] = eIter.getCellId(12);
		reversedCell[13] = eIter.getCellId(14);
		reversedCell[14] = eIter.getCellId(13);
		retVal = true;
		break;
	case kmb::PYRAMID:
		reversedCell[0] = eIter.getCellId(0);
		reversedCell[1] = eIter.getCellId(1);
		reversedCell[2] = eIter.getCellId(4);
		reversedCell[3] = eIter.getCellId(3);
		reversedCell[4] = eIter.getCellId(2);
		retVal = true;
		break;
	case kmb::PYRAMID2:
		reversedCell[0] = eIter.getCellId(0);
		reversedCell[1] = eIter.getCellId(1);
		reversedCell[2] = eIter.getCellId(4);
		reversedCell[3] = eIter.getCellId(3);
		reversedCell[4] = eIter.getCellId(2);

		reversedCell[5] = eIter.getCellId(5);
		reversedCell[6] = eIter.getCellId(8);
		reversedCell[7] = eIter.getCellId(7);
		reversedCell[8] = eIter.getCellId(6);
		reversedCell[9] = eIter.getCellId(12);
		reversedCell[10] = eIter.getCellId(11);
		reversedCell[11] = eIter.getCellId(10);
		reversedCell[12] = eIter.getCellId(9);
		retVal = true;
		break;
	case kmb::HEXAHEDRON:
		reversedCell[0] = eIter.getCellId(0);
		reversedCell[1] = eIter.getCellId(3);
		reversedCell[2] = eIter.getCellId(2);
		reversedCell[3] = eIter.getCellId(1);
		reversedCell[4] = eIter.getCellId(4);
		reversedCell[5] = eIter.getCellId(7);
		reversedCell[6] = eIter.getCellId(6);
		reversedCell[7] = eIter.getCellId(5);
		retVal = true;
		break;
	case kmb::HEXAHEDRON2:
		reversedCell[0] = eIter.getCellId(0);
		reversedCell[1] = eIter.getCellId(3);
		reversedCell[2] = eIter.getCellId(2);
		reversedCell[3] = eIter.getCellId(1);
		reversedCell[4] = eIter.getCellId(4);
		reversedCell[5] = eIter.getCellId(7);
		reversedCell[6] = eIter.getCellId(6);
		reversedCell[7] = eIter.getCellId(5);

		reversedCell[8] = eIter.getCellId(11);
		reversedCell[9] = eIter.getCellId(10);
		reversedCell[10] = eIter.getCellId(9);
		reversedCell[11] = eIter.getCellId(8);

		reversedCell[12] = eIter.getCellId(15);
		reversedCell[13] = eIter.getCellId(14);
		reversedCell[14] = eIter.getCellId(13);
		reversedCell[15] = eIter.getCellId(12);

		reversedCell[16] = eIter.getCellId(16);
		reversedCell[17] = eIter.getCellId(19);
		reversedCell[18] = eIter.getCellId(18);
		reversedCell[19] = eIter.getCellId(17);
		retVal = true;
		break;
	default:
		break;
	}
	return retVal;
}

bool
kmb::ElementRelation::reverse( const kmb::ElementBase &eIter, kmb::nodeIdType *reversedCell, std::map< kmb::nodeIdType, kmb::nodeIdType >& nodeMapper )
{
	if( reverse( eIter, reversedCell ) ){
		const int len = eIter.getNodeCount();
		for(int i=0;i<len;++i){
			std::map< kmb::nodeIdType, kmb::nodeIdType >::iterator nIter = nodeMapper.find( reversedCell[i] );
			if( nIter != nodeMapper.end() ){
				reversedCell[i] = nIter->second;
			}
		}
		return true;
	}
	return false;
}

kmb::elementType
kmb::ElementRelation::common( const kmb::ElementBase &eIter0, const kmb::ElementBase &eIter1, kmb::nodeIdType *commonCell )
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( commonCell == NULL ){
		return etype;
	}
	int common[4] = {-1,-1,-1,-1};
	int counter = 0;
	switch( eIter0.getType() ){
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
			{
				const int len0 = eIter0.getVertexCount();
				const int len1 = eIter1.getVertexCount();
				for(int i=0;i<len0;++i){
					for(int j=0;j<len1;++j){
						if( eIter0.getCellId(i) == eIter1.getCellId(j) ){
							common[i] = j;
							++counter;
						}
					}
				}
				if( counter == 2 ){
					for(int i0=0;i0<len0;++i0){
						int i1 = (i0+1) % len0;
						if( common[i0] != -1 && common[i1] != -1 &&
							( common[i0] - common[i1] + len1 ) % len1 == 1 )
						{
							commonCell[0] = eIter0.getCellId(i0);
							commonCell[1] = eIter0.getCellId(i1);
							etype = kmb::SEGMENT;
							break;
						}
					}
				}
			}
			break;
		default:
			break;
	}
	return etype;
}

kmb::elementType
kmb::ElementRelation::common( kmb::Face f0, kmb::Face f1, const kmb::ElementContainer* elements, kmb::nodeIdType *commonCell )
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( commonCell == NULL || elements == NULL ){
		return etype;
	}
	kmb::ElementContainer::const_iterator eIter0 = elements->find( f0.getElementId() );
	kmb::ElementContainer::const_iterator eIter1 = elements->find( f1.getElementId() );
	int common[4] = {-1,-1,-1,-1};
	int counter = 0;
	switch( eIter0.getBoundaryType( f0.getLocalFaceId() ) ){
		case kmb::TRIANGLE:
		case kmb::TRIANGLE2:
		case kmb::QUAD:
		case kmb::QUAD2:
			{
				const int len0 = eIter0.getBoundaryVertexCount( f0.getLocalFaceId() );
				const int len1 = eIter1.getBoundaryVertexCount( f1.getLocalFaceId() );
				for(int i=0;i<len0;++i){
					for(int j=0;j<len1;++j){
						if( eIter0.getBoundaryCellId( f0.getLocalFaceId(), i ) == eIter1.getBoundaryCellId( f1.getLocalFaceId(), j ) ){
							common[i] = j;
							++counter;
						}
					}
				}
				if( counter == 2 ){
					for(int i0=0;i0<len0;++i0){
						int i1 = (i0+1) % len0;
						if( common[i0] != -1 && common[i1] != -1 &&
							( common[i0] - common[i1] + len1 ) % len1 == 1 )
						{
							commonCell[0] = eIter0.getBoundaryCellId( f0.getLocalFaceId(), i0 );
							commonCell[1] = eIter0.getBoundaryCellId( f0.getLocalFaceId(), i1 );
							etype = kmb::SEGMENT;
							break;
						}
					}
				}
			}
			break;
		default:
			break;
	}
	return etype;
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getRelationForSegment
(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 )
{
	index0 = -1;
	index1 = -1;
	kmb::ElementRelation::relationType ret = kmb::ElementRelation::UNKNOWNRELATION;
	if( eIter1.getType() == kmb::UNKNOWNTYPE ){
		return ret;
	}
	if( eIter1.getDimension() == 1 ){
		ret = kmb::ElementRelation::getSegmentRelation(
			eIter0.getCellId(0), eIter0.getCellId(1),
			eIter1.getCellId(0), eIter1.getCellId(1),
			index0, index1);
	}else{
		int indices[2];
		int count = 0;
		for(int i=0;i<2;++i){
			indices[i] = eIter1.indexOf( eIter0.getCellId(i) );
			if( indices[i] >= eIter1.getVertexCount() ){
				indices[i] = -1;
			}
			if( indices[i] >= 0 ){
				++count;
			}
		}
		switch(count){
		case 2:
			{
				int conn = eIter1.isConnected(indices[0],indices[1]);
				switch(eIter1.getType()){
				case kmb::TRIANGLE:
				case kmb::TRIANGLE2:
				case kmb::QUAD:
				case kmb::QUAD2:
					if( conn == 1 )
						ret = kmb::ElementRelation::BOUNDARY;
					else if( conn == -1 )
						ret = kmb::ElementRelation::ANTIBOUNDARY;
					else
						ret = kmb::ElementRelation::OTHERRELATION;
					break;
				default:

					if( conn != 0 )
						ret = kmb::ElementRelation::COMMONEDGE;
					else
						ret = kmb::ElementRelation::OTHERRELATION;
					break;
				}
			}
			break;
		case 1:
			ret = kmb::ElementRelation::COMMONNODE;
			break;
		case 0:
			ret = kmb::ElementRelation::NOCOMMON;
			break;
		default:
			break;
		}
	}
	return ret;
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getRelationForTriangle
(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 )
{
	index0 = -1;
	index1 = -1;
	kmb::ElementRelation::relationType ret = kmb::ElementRelation::UNKNOWNRELATION;
	if( eIter1.getType() == kmb::UNKNOWNTYPE ){
		return ret;
	}
	switch( eIter1.getType() )
	{
	case SEGMENT:
	case SEGMENT2:
		{
			switch( kmb::ElementRelation::getRelationForSegment( eIter1, index1, eIter0, index0 ) )
			{
			case kmb::ElementRelation::BOUNDARY:
				ret = kmb::ElementRelation::COBOUNDARY;
				break;
			case kmb::ElementRelation::ANTIBOUNDARY:
				ret = kmb::ElementRelation::ANTICOBOUNDARY;
				break;
			case kmb::ElementRelation::COMMONNODE:
				ret = kmb::ElementRelation::COMMONNODE;
				break;
			case kmb::ElementRelation::NOCOMMON:
				ret = kmb::ElementRelation::NOCOMMON;
				break;
			default:
				ret = kmb::ElementRelation::OTHERRELATION;
				break;
			}
		}
		break;
	case TRIANGLE:
	case TRIANGLE2:
		ret = kmb::ElementRelation::getTriangleRelation(
				eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2),
				eIter1.getCellId(0), eIter1.getCellId(1), eIter1.getCellId(2),
				index0, index1 );
		break;
	case TETRAHEDRON:
	case TETRAHEDRON2:

		ret = kmb::ElementRelation::getTriangleTetrahedronRelation(
				eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2),
				eIter1.getCellId(0), eIter1.getCellId(1), eIter1.getCellId(2), eIter1.getCellId(3),
				index0, index1 );
		break;
	default:
		{
			ret = kmb::ElementRelation::OTHERRELATION;
			int indices[3];
			int count = 0;
			for(int i=0;i<3;++i){
				indices[i] = eIter1.indexOf( eIter0.getCellId(i) );
				if( indices[i] >= eIter1.getVertexCount() ){
					indices[i] = -1;
				}
				if( indices[i] >= 0 ){
					++count;
				}
			}
			switch( count ){
			case 2:
				{
					switch( eIter1.getType() ){
					case kmb::QUAD:
					case kmb::QUAD2:
						{
							if( indices[0] == -1 ){
								index0 = 0;
								for(int i=0;i<4;++i){
									if( indices[1] == kmb::Quad::faceTable[i][0] &&
										indices[2] == kmb::Quad::faceTable[i][1] ){
										index1 = i;
										ret = kmb::ElementRelation::ANTIADJACENT;
										break;
									}else if(
										indices[1] == kmb::Quad::faceTable[i][1] &&
										indices[2] == kmb::Quad::faceTable[i][0] ){
										index1 = i;
										ret = kmb::ElementRelation::ADJACENT;
										break;
									}
								}
							}else if( indices[1] == -1 ){
								index0 = 1;
								for(int i=0;i<4;++i){
									if( indices[2] == kmb::Quad::faceTable[i][0] &&
										indices[0] == kmb::Quad::faceTable[i][1] ){
										index1 = i;
										ret = kmb::ElementRelation::ANTIADJACENT;
										break;
									}else if(
										indices[2] == kmb::Quad::faceTable[i][1] &&
										indices[0] == kmb::Quad::faceTable[i][0] ){
										index1 = i;
										ret = kmb::ElementRelation::ADJACENT;
										break;
									}
								}
							}else if( indices[2] == -1 ){
								index0 = 2;
								for(int i=0;i<4;++i){
									if( indices[0] == kmb::Quad::faceTable[i][0] &&
										indices[1] == kmb::Quad::faceTable[i][1] ){
										index1 = i;
										ret = kmb::ElementRelation::ANTIADJACENT;
										break;
									}else if(
										indices[0] == kmb::Quad::faceTable[i][1] &&
										indices[1] == kmb::Quad::faceTable[i][0] ){
										index1 = i;
										ret = kmb::ElementRelation::ADJACENT;
										break;
									}
								}
							}
						}
						break;
					default:
						{
							if( indices[0] == -1 ){
								index0 = 0;
								if( eIter1.isConnected( indices[1], indices[2] ) == 1 ){
									ret = kmb::ElementRelation::COMMONEDGE;
								}
							}else if( indices[1] == -1 ){
								index0 = 1;
								if( eIter1.isConnected( indices[2], indices[0] ) == 1 ){
									ret = kmb::ElementRelation::COMMONEDGE;
								}
							}else if( indices[2] == -1 ){
								index0 = 2;
								if( eIter1.isConnected( indices[0], indices[1] ) == 1 ){
									ret = kmb::ElementRelation::COMMONEDGE;
								}
							}
						}
						break;
					}
					break;
				}
			case 1:
				ret = kmb::ElementRelation::COMMONNODE;
				for(int i=0;i<3;++i){
					if( indices[i] >= 0 ){
						index0 = i;
					}
				}
				break;
			case 0:
				ret = kmb::ElementRelation::NOCOMMON;
				break;
			case 3:
				{
					int ind0 = -1;
					int ind1 = -1;
					switch( eIter1.getType() ){
					case WEDGE:
					case WEDGE2:
						{

							for(int i=0;i<2;++i){
								kmb::ElementRelation::relationType r =
									kmb::ElementRelation::getTriangleRelation(
										eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2),
										eIter1.getCellId(kmb::Wedge::faceTable[i][0]),
										eIter1.getCellId(kmb::Wedge::faceTable[i][1]),
										eIter1.getCellId(kmb::Wedge::faceTable[i][2]),
										ind0,ind1);
								if( r == kmb::ElementRelation::EQUAL ){
									index1 = i;
									ret = kmb::ElementRelation::BOUNDARY;
									break;
								}else if( r == kmb::ElementRelation::REVERSE ){
									index1 = i;
									ret = kmb::ElementRelation::ANTIBOUNDARY;
									break;
								}
							}
						}
						break;
					case PYRAMID:
					case PYRAMID2:
						{

							for(int i=0;i<4;++i){
								kmb::ElementRelation::relationType r =
									kmb::ElementRelation::getTriangleRelation(
										eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2),
										eIter1.getCellId(kmb::Pyramid::faceTable[i][0]),
										eIter1.getCellId(kmb::Pyramid::faceTable[i][1]),
										eIter1.getCellId(kmb::Pyramid::faceTable[i][2]),
										ind0,ind1);
								if( r == kmb::ElementRelation::EQUAL ){
									index1 = i;
									ret = kmb::ElementRelation::BOUNDARY;
									break;
								}else if( r == kmb::ElementRelation::REVERSE ){
									index1 = i;
									ret = kmb::ElementRelation::ANTIBOUNDARY;
									break;
								}
							}
						}
						break;
					default:

						ret = kmb::ElementRelation::OTHERRELATION;
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	return ret;
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getRelationForQuad
(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 )
{
	index0 = -1;
	index1 = -1;
	kmb::ElementRelation::relationType ret = kmb::ElementRelation::UNKNOWNRELATION;
	if( eIter1.getType() == kmb::UNKNOWNTYPE ){
		return ret;
	}
	switch( eIter1.getType() ){
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
		switch( kmb::ElementRelation::getRelationForSegment( eIter1, index1, eIter0, index0 ) ){
		case kmb::ElementRelation::BOUNDARY:
			ret = kmb::ElementRelation::COBOUNDARY;
			break;
		case kmb::ElementRelation::ANTIBOUNDARY:
			ret = kmb::ElementRelation::ANTICOBOUNDARY;
			break;
		case kmb::ElementRelation::COMMONNODE:
			ret = kmb::ElementRelation::COMMONNODE;
			break;
		case kmb::ElementRelation::NOCOMMON:
			ret = kmb::ElementRelation::NOCOMMON;
			break;
		default:
			ret = kmb::ElementRelation::OTHERRELATION;
			break;
		}
		break;
	case kmb::TRIANGLE:
	case kmb::TRIANGLE2:
		ret = kmb::ElementRelation::getRelationForTriangle( eIter1, index1, eIter0, index0 );
		break;
	case kmb::QUAD:
	case kmb::QUAD2:
		ret = kmb::ElementRelation::getQuadRelation(
				eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2), eIter0.getCellId(3),
				eIter1.getCellId(0), eIter1.getCellId(1), eIter1.getCellId(2), eIter1.getCellId(3),
				index0, index1);
		break;
	default:
		{
			int indices[4];
			int count = 0;
			for(int i=0;i<4;++i){
				indices[i] = eIter1.indexOf( eIter0.getCellId(i) );
				if( indices[i] >= eIter1.getVertexCount() ){
					indices[i] = -1;
				}
				if( indices[i] >= 0 ){
					++count;
				}
			}
			switch(count){
			case 2:
				{
					ret = kmb::ElementRelation::OTHERRELATION;
					for(int i=0;i<4;++i){
						int i0 = indices[ kmb::Quad::faceTable[i][0] ];
						int i1 = indices[ kmb::Quad::faceTable[i][1] ];
						if( i0 >= 0 && i1 >= 0 && eIter1.isConnected(i0,i1) ){
							index0 = i;
							ret = kmb::ElementRelation::COMMONEDGE;
						}
					}
				}
				break;
			case 4:
				{
					ret = kmb::ElementRelation::OTHERRELATION;
					int ind0 = -1;
					int ind1 = -1;
					switch( eIter1.getType() ){
					case kmb::WEDGE:
					case kmb::WEDGE2:
					{

						for(int i=2;i<5;++i){
							kmb::ElementRelation::relationType r = kmb::ElementRelation::getQuadRelation(
								eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2), eIter0.getCellId(3),
								eIter1.getCellId(kmb::Wedge::faceTable[i][0]),
								eIter1.getCellId(kmb::Wedge::faceTable[i][1]),
								eIter1.getCellId(kmb::Wedge::faceTable[i][2]),
								eIter1.getCellId(kmb::Wedge::faceTable[i][3]),
								ind0,ind1);
							if( r == kmb::ElementRelation::EQUAL ){
								index1 = i;
								ret = kmb::ElementRelation::BOUNDARY;
								break;
							}else if( r == kmb::ElementRelation::REVERSE ){
								index1 = i;
								ret = kmb::ElementRelation::ANTIBOUNDARY;
								break;
							}
						}
						break;
					}
					case kmb::PYRAMID:
					case kmb::PYRAMID2:
					{

						kmb::ElementRelation::relationType r =
							kmb::ElementRelation::getQuadRelation(
							eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2), eIter0.getCellId(3),
							eIter1.getCellId(kmb::Pyramid::faceTable[4][0]),
							eIter1.getCellId(kmb::Pyramid::faceTable[4][1]),
							eIter1.getCellId(kmb::Pyramid::faceTable[4][2]),
							eIter1.getCellId(kmb::Pyramid::faceTable[4][3]),
							ind0,ind1);
						if( r == kmb::ElementRelation::EQUAL ){
							index1 = 4;
							ret = kmb::ElementRelation::BOUNDARY;
						}else if( r == kmb::ElementRelation::REVERSE ){
							index1 = 4;
							ret = kmb::ElementRelation::ANTIBOUNDARY;
						}
						break;
					}
					case kmb::HEXAHEDRON:
					case kmb::HEXAHEDRON2:
					{

						for(int i=0;i<6;++i){
							kmb::ElementRelation::relationType r =
								kmb::ElementRelation::getQuadRelation(
								eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2), eIter0.getCellId(3),
								eIter1.getCellId(kmb::Hexahedron::faceTable[i][0]),
								eIter1.getCellId(kmb::Hexahedron::faceTable[i][1]),
								eIter1.getCellId(kmb::Hexahedron::faceTable[i][2]),
								eIter1.getCellId(kmb::Hexahedron::faceTable[i][3]),
								ind0,ind1);
							if( r == kmb::ElementRelation::EQUAL ){
								index1 = i;
								ret = kmb::ElementRelation::BOUNDARY;
								break;
							}else if( r == kmb::ElementRelation::REVERSE ){
								index1 = i;
								ret = kmb::ElementRelation::ANTIBOUNDARY;
								break;
							}
						}
						break;
					}
					default:
						break;
					}
				}
				break;
			case 1:
				ret = kmb::ElementRelation::COMMONNODE;
				break;
			case 3:
				ret = kmb::ElementRelation::OTHERRELATION;
				break;
			case 0:
				ret = kmb::ElementRelation::NOCOMMON;
				break;
			default:
				break;
			}
		}
		break;
	}
	return ret;
}

kmb::ElementRelation::relationType
kmb::ElementRelation::getRelationForTetrahedron(const kmb::ElementBase &eIter0, int &index0, const kmb::ElementBase &eIter1, int &index1 )
{
	index0 = -1;
	index1 = -1;
	kmb::ElementRelation::relationType ret = kmb::ElementRelation::UNKNOWNRELATION;
	if( eIter1.getType() == kmb::UNKNOWNTYPE ){
		return ret;
	}
	switch( eIter1.getType() ){
	case kmb::SEGMENT:
	case kmb::SEGMENT2:
		ret = kmb::ElementRelation::getRelationForSegment( eIter1, index1, eIter0, index0 );
		break;
	case kmb::TRIANGLE:
	case kmb::TRIANGLE2:
		{
			kmb::ElementRelation::relationType r = kmb::ElementRelation::getRelationForTriangle( eIter1, index1, eIter0, index0 );
			switch( r ){
			case kmb::ElementRelation::BOUNDARY:
				ret = kmb::ElementRelation::COBOUNDARY;
				break;
			case kmb::ElementRelation::ANTIBOUNDARY:
				ret = kmb::ElementRelation::ANTICOBOUNDARY;
				break;
			default:
				ret = r;
				break;
			}
			break;
		}
	case kmb::QUAD:
	case kmb::QUAD2:
		{
			kmb::ElementRelation::relationType r = kmb::ElementRelation::getRelationForQuad( eIter1, index1, eIter0, index0 );
			switch( r ){
			case kmb::ElementRelation::BOUNDARY:
				ret = kmb::ElementRelation::COBOUNDARY;
				break;
			case kmb::ElementRelation::ANTIBOUNDARY:
				ret = kmb::ElementRelation::ANTICOBOUNDARY;
				break;
			default:
				ret = r;
				break;
			}
			break;
		}
	case kmb::TETRAHEDRON:
	case kmb::TETRAHEDRON2:
		ret = kmb::ElementRelation::getTetrahedronRelation(
			eIter0.getCellId(0), eIter0.getCellId(1), eIter0.getCellId(2), eIter0.getCellId(3),
			eIter1.getCellId(0), eIter1.getCellId(1), eIter1.getCellId(2), eIter1.getCellId(3),
			index0, index1);
		break;
	default:
		{

			int indices[4];
			int count = 0;
			for(int i=0;i<4;++i){
				indices[i] = eIter1.indexOf( eIter0[i] );
				if( indices[i] >= eIter1.getVertexCount() ){
					indices[i] = -1;
				}
				if( indices[i] >= 0 ){
					++count;
				}
			}
			switch(count){
			case 0:
				ret = kmb::ElementRelation::NOCOMMON;
				break;
			case 1:
				ret = kmb::ElementRelation::COMMONNODE;
				break;
			case 2:
				{
					if( indices[0] >= 0 && indices[1] >= 0 && eIter1.isConnected(indices[0],indices[1]) != 0 ){
						ret = kmb::ElementRelation::COMMONEDGE;
					}else if( indices[0] >= 0 && indices[2] >= 0 && eIter1.isConnected(indices[0],indices[2]) != 0 ){
						ret = kmb::ElementRelation::COMMONEDGE;
					}else if( indices[0] >= 0 && indices[3] >= 0 && eIter1.isConnected(indices[0],indices[3]) != 0 ){
						ret = kmb::ElementRelation::COMMONEDGE;
					}else if( indices[1] >= 0 && indices[2] >= 0 && eIter1.isConnected(indices[1],indices[2]) != 0 ){
						ret = kmb::ElementRelation::COMMONEDGE;
					}else if( indices[1] >= 0 && indices[3] >= 0 && eIter1.isConnected(indices[1],indices[3]) != 0 ){
						ret = kmb::ElementRelation::COMMONEDGE;
					}else if( indices[2] >= 0 && indices[3] >= 0 && eIter1.isConnected(indices[2],indices[3]) != 0 ){
						ret = kmb::ElementRelation::COMMONEDGE;
					}else{
						ret = kmb::ElementRelation::OTHERRELATION;
					}
				}
				break;
			case 3:
				{
					ret = kmb::ElementRelation::OTHERRELATION;

					for(int i=0;i<4;++i){
						if( indices[i] == -1 ){
							int res = eIter1.isFace(
								indices[ kmb::Tetrahedron::faceTable[i][0] ],
								indices[ kmb::Tetrahedron::faceTable[i][1] ],
								indices[ kmb::Tetrahedron::faceTable[i][2] ],
								index1);
							switch( res ){
							case 1:
								index0 = i;
								ret = kmb::ElementRelation::ANTIADJACENT;
								break;
							case -1:
								index0 = i;
								ret = kmb::ElementRelation::ADJACENT;
								break;
							default:
								break;
							}
							break;
						}
					}
				}
				break;
			case 4:
				{
					ret = kmb::ElementRelation::OTHERRELATION;
				}
				break;
			default:
				break;
			}
		}
		break;
	}
	return ret;
}
