/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Triangle                                                #
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
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbQuad.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbSegment.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"
#include "Geometry/kmbCircle.h"

/********************************************************************************
=begin

=== 1次三角形要素 (TRIANGLE)

接続行列

	{  0, 1,-1},
	{ -1, 0, 1},
	{  1,-1, 0}

辺

	{ 1, 2},
	{ 2, 0},
	{ 0, 1},

=end

接続行列の (i,j) 成分は
 [i,j] が辺のときに 1
 [j,i] が辺のときに -1
とする

形状関数
0 : 1-s-t => (s,t) = ( 0, 0)
1 : s     => (s,t) = ( 1, 0)
2 : t     => (s,t) = ( 0, 1)

*********************************************************************************/

const int kmb::Triangle::nodeCount = 3;

const int kmb::Triangle::connectionTable[3][3] =
{
	{  0, 1,-1},
	{ -1, 0, 1},
	{  1,-1, 0}
};

const int kmb::Triangle::faceTable[3][2] =
{
	{ 1, 2},
	{ 2, 0},
	{ 0, 1},
};

kmb::Triangle::Triangle(void)
: kmb::Element(kmb::TRIANGLE)
{
	cell = new kmb::nodeIdType[3];
}

kmb::Triangle::Triangle(kmb::nodeIdType *ary)
: kmb::Element(kmb::TRIANGLE)
{
	cell = ary;
}

kmb::Triangle::Triangle(kmb::nodeIdType i0,kmb::nodeIdType i1,kmb::nodeIdType i2)
: kmb::Element(kmb::TRIANGLE)
{
	cell = new kmb::nodeIdType[3];
	cell[0] = i0;
	cell[1] = i1;
	cell[2] = i2;
}

kmb::Triangle::~Triangle(void)
{
}

void
kmb::Triangle::shapeFunction(double s,double t,double* coeff)
{
	coeff[0] = 1.0-s-t;
	coeff[1] = s;
	coeff[2] = t;
}

double
kmb::Triangle::checkShapeFunctionDomain(double s,double t)
{
	return kmb::Minimizer::getMin( 1.0-s-t, s, t );
}

bool
kmb::Triangle::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[2])
{
	if( points == NULL ){
		return false;
	}
	double coords[3] = {0.0,0.0,0.0};
	kmb::Vector3D normal = kmb::Point3D::areaVector( points[0], points[1], points[2] );
	coords[0] = normal * kmb::Point3D::areaVector( target, points[1], points[2] );
	coords[1] = normal * kmb::Point3D::areaVector( target, points[2], points[0] );
	coords[2] = normal * kmb::Point3D::areaVector( target, points[0], points[1] );
	double sum = coords[0] + coords[1] + coords[2];
	if( sum != 0.0 ){
		naturalCoords[0] = coords[1] / sum;
		naturalCoords[1] = coords[2] / sum;
		return true;
	}
	return false;
}

bool
kmb::Triangle::getPhysicalCoordinates(const double naturalCoords[2],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[3] = { 0.0, 0.0, 0.0 };
 	shapeFunction( naturalCoords[0], naturalCoords[1], coeff );
	target.zero();
	double c = 0.0;
	for(int i=0;i<3;++i){
		c = 0.0;
		for(int j=0;j<3;++j){
			c += points[j].getCoordinate(i) * coeff[j];
		}
		target.setCoordinate(i,c);
	}
	return true;
}

bool
kmb::Triangle::edgeSwap(kmb::ElementBase &triangle0,kmb::ElementBase &triangle1)
{
	bool swapped = false;
	if( triangle0.getType() == kmb::TRIANGLE && triangle1.getType() == kmb::TRIANGLE )
	{

		kmb::nodeIdType nodes[4]
			= {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};
		int count = 0;
		int indices[3] ={0,0,0};
		for(int i=0;i<3;++i){
			indices[i] = triangle1.indexOf( triangle0.getCellId(i) );
			if( indices[i] != -1 ){
				++count;
			}
		}

		if( count == 2 )
		{
			if( indices[0] == -1 )
			{
				int i_next = ( indices[1]+1 == 3) ? 0 : indices[1]+1;
				nodes[0] = triangle0.getCellId( 0 );
				nodes[1] = triangle0.getCellId( 1 );
				nodes[2] = triangle1.getCellId( i_next );
				nodes[3] = triangle0.getCellId( 2 );
			}
			else if( indices[1] == -1 )
			{
				int i_next = ( indices[2]+1 == 3) ? 0 : indices[2]+1;
				nodes[0] = triangle0.getCellId( 1 );
				nodes[1] = triangle0.getCellId( 2 );
				nodes[2] = triangle1.getCellId( i_next );
				nodes[3] = triangle0.getCellId( 0 );
			}
			else if( indices[2] == -1 )
			{
				int i_next = ( indices[0]+1 == 3) ? 0 : indices[0]+1;
				nodes[0] = triangle0.getCellId( 2 );
				nodes[1] = triangle0.getCellId( 0 );
				nodes[2] = triangle1.getCellId( i_next );
				nodes[3] = triangle0.getCellId( 1 );
			}
			triangle0.setCellId(0, nodes[0]);
			triangle0.setCellId(1, nodes[1]);
			triangle0.setCellId(2, nodes[2]);
			triangle1.setCellId(0, nodes[2]);
			triangle1.setCellId(1, nodes[3]);
			triangle1.setCellId(2, nodes[0]);
			swapped = true;
		}
	}
	return swapped;
}

int
kmb::Triangle::isCoincident(kmb::nodeIdType t00,kmb::nodeIdType t01,kmb::nodeIdType t02,kmb::nodeIdType t10,kmb::nodeIdType t11,kmb::nodeIdType t12)
{
	unsigned int rel = 0;
	if( t00 == t10 )	rel |= 0x100;
	if( t00 == t11 )	rel |= 0x200;
	if( t00 == t12 )	rel |= 0x400;
	if( t01 == t10 )	rel |= 0x010;
	if( t01 == t11 )	rel |= 0x020;
	if( t01 == t12 )	rel |= 0x040;
	if( t02 == t10 )	rel |= 0x001;
	if( t02 == t11 )	rel |= 0x002;
	if( t02 == t12 )	rel |= 0x004;
	switch( rel ){

	case 0x124:
	case 0x241:
	case 0x412:
		return 1;

	case 0x421:
	case 0x214:
	case 0x142:
		return -1;
	default:
		return 0;
	}
}

bool
kmb::Triangle::isCoincidentNodes(kmb::nodeIdType t0,kmb::nodeIdType t1,kmb::nodeIdType t2)
{
	return (t0 == t1) || (t1 == t2) || (t2 == t0);
}
