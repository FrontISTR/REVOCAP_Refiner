/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Tetrahedron                                             #
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
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"
#include "Geometry/kmbSphere.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

/********************************************************************************
=begin

=== 1次四面体要素 (TETRAHEDRON)

接続行列

	{ 0, 1, 1, 1},
	{ 1, 0, 1, 1},
	{ 1, 1, 0, 1},
	{ 1, 1, 1, 0}

面

	{ 1, 2, 3},
	{ 0, 3, 2},
	{ 0, 1, 3},
	{ 0, 2, 1}

辺

	{ 1, 2},
	{ 0, 2},
	{ 0, 1},
	{ 0, 3},
	{ 1, 3},
	{ 2, 3}

=end

形状関数 : [s,t,u] 空間での向きを合わせるために、順番注意
0 : 1-s-t-u   => (s,t,u) = ( 0, 0, 0)
1 : s         => (s,t,u) = ( 1, 0, 0)
2 : t         => (s,t,u) = ( 0, 1, 0)
3 : u         => (s,t,u) = ( 0, 0, 1)

*********************************************************************************/
const int kmb::Tetrahedron::nodeCount = 4;








const int kmb::Tetrahedron::connectionTable[4][4] =
{
	{ 0, 1, 1, 1},
	{ 1, 0, 1, 1},
	{ 1, 1, 0, 1},
	{ 1, 1, 1, 0}
};

const int kmb::Tetrahedron::faceTable[4][4] =
{
	{ 1, 2, 3, -1},
	{ 0, 3, 2, -1},
	{ 0, 1, 3, -1},
	{ 0, 2, 1, -1}
};

const int kmb::Tetrahedron::edgeTable[6][2] =
{
	{ 1, 2},
	{ 0, 2},
	{ 0, 1},
	{ 0, 3},
	{ 1, 3},
	{ 2, 3}
};

bool
kmb::Tetrahedron::isEquivalent(int index[4])
{
	const int len = kmb::Element::getNodeCount(kmb::TETRAHEDRON);

	for(int i=0;i<len;++i){
		if(index[i] < 0 || len <= index[i]){
			return false;
		}
	}

	for(int i=0;i<len;++i){
		for(int j=0;j<len;++j){
			if( kmb::Tetrahedron::connectionTable[i][j]
			!= kmb::Tetrahedron::connectionTable[ index[i] ][ index[j] ] )
			{
				return false;
			}
		}
	}
	return true;
}

kmb::Tetrahedron::Tetrahedron(void)
: kmb::Element(kmb::TETRAHEDRON)
{
	cell = new kmb::nodeIdType[4];
}

kmb::Tetrahedron::Tetrahedron(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3)
: kmb::Element(kmb::TETRAHEDRON)
{
	cell = new kmb::nodeIdType[4];
	cell[0] = n0;
	cell[1] = n1;
	cell[2] = n2;
	cell[3] = n3;
}

kmb::Tetrahedron::Tetrahedron(kmb::nodeIdType *ary)
: kmb::Element(kmb::TETRAHEDRON)
{
	cell = ary;
}

kmb::Tetrahedron::~Tetrahedron(void)
{
}

void
kmb::Tetrahedron::shapeFunction(double s,double t,double u,double* coeff)
{
	coeff[0] = 1.0-s-t-u;
	coeff[1] = s;
	coeff[2] = t;
	coeff[3] = u;
}

double
kmb::Tetrahedron::checkShapeFunctionDomain(double s,double t,double u)
{
	kmb::Minimizer minimizer;
	minimizer.update( 1.0-s-t-u );
	minimizer.update( s );
	minimizer.update( t );
	minimizer.update( u );
	return minimizer.getMin();
}

bool
kmb::Tetrahedron::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	double coords[4] = {0.0,0.0,0.0,0.0};
	kmb::Point3D::calcMinorCoordinate( points[0], points[1], points[2], points[3], target, coords);
	double sum = coords[0] + coords[1] + coords[2] + coords[3];
	if( sum != 0.0 ){
		naturalCoords[0] = coords[1] / sum;
		naturalCoords[1] = coords[2] / sum;
		naturalCoords[2] = coords[3] / sum;
		return true;
	}
	return false;
}

bool
kmb::Tetrahedron::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[4];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<4;++j){
			target.addCoordinate(i,points[j].getCoordinate(i) * coeff[j]);
		}
	}
	return true;
}
