/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Wedge                                                   #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElementRelation.h"

/********************************************************************************
=begin

=== 1次三角柱要素 (WEDGE)

接続行列

	{ 0, 1, 1, 1, 0, 0},
	{ 1, 0, 1, 0, 1, 0},
	{ 1, 1, 0, 0, 0, 1},
	{ 1, 0, 0, 0, 1, 1},
	{ 0, 1, 0, 1, 0, 1},
	{ 0, 0, 1, 1, 1, 0}

面情報

	{ 0, 2, 1},
	{ 3, 4, 5},
	{ 0, 1, 4, 3},
	{ 1, 2, 5, 4},
	{ 2, 0, 3, 5}

辺

	{ 1, 2},
	{ 0, 2},
	{ 0, 1},
	{ 4, 5},
	{ 3, 5},
	{ 3, 4},
	{ 0, 3},
	{ 1, 4},
	{ 2, 5}

=end

形状関数 serendipity 0 <= s,t, <= 1, -1 <= u <= 1
0 : 1/2(1-s-t)(1-u)       => (s,t,u) = ( 0, 0,-1)
1 : 1/2s(1-u)             => (s,t,u) = ( 1, 0,-1)
2 : 1/2t(1-u)             => (s,t,u) = ( 0, 1,-1)
3 : 1/2(1-s-t)(1+u)       => (s,t,u) = ( 0, 0, 1)
4 : 1/2s(1+u)             => (s,t,u) = ( 1, 0, 1)
5 : 1/2t(1+u)             => (s,t,u) = ( 0, 1, 1)

*********************************************************************************/
const int kmb::Wedge::nodeCount = 6;


const int kmb::Wedge::connectionTable[6][6] =
{
	{ 0, 1, 1, 1, 0, 0},
	{ 1, 0, 1, 0, 1, 0},
	{ 1, 1, 0, 0, 0, 1},
	{ 1, 0, 0, 0, 1, 1},
	{ 0, 1, 0, 1, 0, 1},
	{ 0, 0, 1, 1, 1, 0}
};




const int kmb::Wedge::faceTable[5][4] =
{
	{ 0, 2, 1,-1},
	{ 3, 4, 5,-1},
	{ 0, 1, 4, 3},
	{ 1, 2, 5, 4},
	{ 2, 0, 3, 5}
};

const int kmb::Wedge::edgeTable[9][2] =
{
	{ 1, 2},
	{ 0, 2},
	{ 0, 1},
	{ 4, 5},
	{ 3, 5},
	{ 3, 4},
	{ 0, 3},
	{ 1, 4},
	{ 2, 5}
};

bool
kmb::Wedge::isEquivalent(int index[6])
{
	const int len = kmb::Element::getNodeCount(kmb::WEDGE);

	for(int i=0;i<len;++i){
		if(index[i] < 0 || len <= index[i]){
			return false;
		}
	}

	for(int i=0;i<len;++i){
		for(int j=0;j<len;++j){
			if( kmb::Wedge::connectionTable[i][j]
				!= kmb::Wedge::connectionTable[ index[i] ][ index[j] ] )
			{
				return false;
			}
		}
	}
	return true;
}

kmb::Wedge::Wedge(void)
: kmb::Element(kmb::WEDGE)
{
	cell = new kmb::nodeIdType[6];
}

kmb::Wedge::Wedge(kmb::nodeIdType* ary)
: kmb::Element(kmb::WEDGE)
{
	cell = ary;
}

kmb::Wedge::Wedge(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3, kmb::nodeIdType n4, kmb::nodeIdType n5)
: kmb::Element(kmb::WEDGE)
{
	cell = new kmb::nodeIdType[6];
	cell[0] = n0;
	cell[1] = n1;
	cell[2] = n2;
	cell[3] = n3;
	cell[4] = n4;
	cell[5] = n5;
}


kmb::Wedge::~Wedge(void)
{
}

void
kmb::Wedge::shapeFunction(double s,double t,double u,double* coeff)
{
	coeff[0] = 0.5*(1.0-s-t)*(1.0-u);
	coeff[1] = 0.5*s*(1.0-u);
	coeff[2] = 0.5*t*(1.0-u);
	coeff[3] = 0.5*(1.0-s-t)*(1.0+u);
	coeff[4] = 0.5*s*(1.0+u);
	coeff[5] = 0.5*t*(1.0+u);
}

bool
kmb::Wedge::getNaturalCoordinates(const double physicalCoords[3],const kmb::Point3D* points,double naturalCoords[3],double margin)
{
	if( points == NULL ){
		return false;
	}
	/*
	 * wedgeの要素座標を求めるためにニュートン法を行う
	 */
	const int len = 6;
	double d = 0.0;
	double thres = 1.0e-10;
	kmb::BoundingBox bbox;
	for(int i=0;i<len;++i){
		bbox.update( points[i] );
	}
	bbox.expand( margin );
	if( bbox.intersect(physicalCoords[0],physicalCoords[1],physicalCoords[2]) == kmb::BoxRegion::OUTSIDE ){
		return false;
	}

	for(int j=0;j<10;++j){
		for(int i=0;i<50;++i){
			d = kmb::Wedge::newtonMethod(physicalCoords,points,naturalCoords);
			if( d < thres &&
				 0.0 <= naturalCoords[0] && naturalCoords[0] <= 1.0 &&
				 0.0 <= naturalCoords[1] && naturalCoords[1] <= 1.0 &&
				-1.0 <= naturalCoords[2] && naturalCoords[2] <= 1.0 )
			{
				return true;
			}
		}
		if( d < thres &&
			-margin <= naturalCoords[0] && naturalCoords[0] <= 1.0 + margin &&
			-margin <= naturalCoords[1] && naturalCoords[1] <= 1.0 + margin &&
			-margin-1.0 <= naturalCoords[2] && naturalCoords[2] <= 1.0 + margin )
		{
			return true;
		}


		naturalCoords[0] = -0.01*j*naturalCoords[1];
		naturalCoords[1] = -0.01*j*naturalCoords[2];
		naturalCoords[2] = -0.01*j*naturalCoords[0];
	}
	return false;
}

bool
kmb::Wedge::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,double physicalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	double coeff[6];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	for(int i=0;i<3;++i){
		physicalCoords[i] = 0.0;
		for(int j=0;j<6;++j){
			physicalCoords[i] += points[j].getCoordinate(i) * coeff[j];
		}
	}
	return true;
}

double
kmb::Wedge::newtonMethod(const double physicalCoords[3], const kmb::Point3D* points, double naturalCoords[3])
{
	double retVal = DBL_MAX;
	if( points == NULL ){
		return retVal;
	}
	double q[3];
	if( !getPhysicalCoordinates( naturalCoords, points, q ) ){
		return retVal;
	}
	double ds[3] = {0.0,0.0,0.0};
	double dt[3] = {0.0,0.0,0.0};
	double du[3] = {0.0,0.0,0.0};

	for(int i=0;i<3;++i){
		ds[i] = 0.5 * (
		-(1.0-naturalCoords[2])*points[0].getCoordinate(i)
		+(1.0-naturalCoords[2])*points[1].getCoordinate(i)
		-(1.0+naturalCoords[2])*points[3].getCoordinate(i)
		+(1.0+naturalCoords[2])*points[4].getCoordinate(i) );
		dt[i] = 0.5 * (
		-(1.0-naturalCoords[2])*points[0].getCoordinate(i)
		+(1.0-naturalCoords[2])*points[2].getCoordinate(i)
		-(1.0+naturalCoords[2])*points[3].getCoordinate(i)
		+(1.0+naturalCoords[2])*points[5].getCoordinate(i) );
		du[i] = 0.5 * (
		-(1.0-naturalCoords[0]-naturalCoords[1])*points[0].getCoordinate(i)
		-naturalCoords[0]*points[1].getCoordinate(i)
		-naturalCoords[1]*points[2].getCoordinate(i)
		+(1.0-naturalCoords[0]-naturalCoords[1])*points[3].getCoordinate(i)
		+naturalCoords[0]*points[4].getCoordinate(i)
		+naturalCoords[1]*points[5].getCoordinate(i) );
	}
	kmb::Matrix3x3 mat(
		ds[0],dt[0],du[0],
		ds[1],dt[1],du[1],
		ds[2],ds[2],du[2]);
	kmb::Vector3D v( q[0]-physicalCoords[0], q[1]-physicalCoords[1], q[2]-physicalCoords[2] );
	kmb::Vector3D* sol = mat.solve(v);
	if( sol != NULL ){
		naturalCoords[0] -= sol->x();
		naturalCoords[1] -= sol->y();
		naturalCoords[2] -= sol->z();
		retVal = sol->length();
		delete sol;
	}
	return retVal;
}

int
kmb::Wedge::divideIntoTetrahedrons(const kmb::ElementBase* element,kmb::nodeIdType tetrahedrons[][4])
{
	if( element == NULL ||
		( element->getType() != kmb::WEDGE && element->getType() != kmb::WEDGE2 ) ){
		return 0;
	}
	int num = 3;











	bool n2 = element->getIndexMinNodeIdOfFace( 2 )%2 == 1;
	bool n3 = element->getIndexMinNodeIdOfFace( 3 )%2 == 1;
	bool n4 = element->getIndexMinNodeIdOfFace( 4 )%2 == 1;

	if( !n2 && !n3 && n4 )
	{

		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(4);
		tetrahedrons[0][2] = element->getCellId(5);
		tetrahedrons[0][3] = element->getCellId(3);

		tetrahedrons[1][0] = element->getCellId(0);
		tetrahedrons[1][1] = element->getCellId(1);
		tetrahedrons[1][2] = element->getCellId(5);
		tetrahedrons[1][3] = element->getCellId(4);

		tetrahedrons[2][0] = element->getCellId(0);
		tetrahedrons[2][1] = element->getCellId(1);
		tetrahedrons[2][2] = element->getCellId(2);
		tetrahedrons[2][3] = element->getCellId(5);
	}
	else if( !n2 && n3 && n4 )
	{

		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(4);
		tetrahedrons[0][2] = element->getCellId(5);
		tetrahedrons[0][3] = element->getCellId(3);

		tetrahedrons[1][0] = element->getCellId(0);
		tetrahedrons[1][1] = element->getCellId(1);
		tetrahedrons[1][2] = element->getCellId(2);
		tetrahedrons[1][3] = element->getCellId(4);

		tetrahedrons[2][0] = element->getCellId(0);
		tetrahedrons[2][1] = element->getCellId(4);
		tetrahedrons[2][2] = element->getCellId(2);
		tetrahedrons[2][3] = element->getCellId(5);
	}
	else if( !n2 && n3 && !n4 )
	{

		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(4);
		tetrahedrons[0][2] = element->getCellId(2);
		tetrahedrons[0][3] = element->getCellId(3);

		tetrahedrons[1][0] = element->getCellId(0);
		tetrahedrons[1][1] = element->getCellId(1);
		tetrahedrons[1][2] = element->getCellId(2);
		tetrahedrons[1][3] = element->getCellId(4);

		tetrahedrons[2][0] = element->getCellId(2);
		tetrahedrons[2][1] = element->getCellId(3);
		tetrahedrons[2][2] = element->getCellId(4);
		tetrahedrons[2][3] = element->getCellId(5);
	}
	else if( n2 && !n3 && n4 )
	{

		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(1);
		tetrahedrons[0][2] = element->getCellId(5);
		tetrahedrons[0][3] = element->getCellId(3);

		tetrahedrons[1][0] = element->getCellId(1);
		tetrahedrons[1][1] = element->getCellId(5);
		tetrahedrons[1][2] = element->getCellId(3);
		tetrahedrons[1][3] = element->getCellId(4);

		tetrahedrons[2][0] = element->getCellId(0);
		tetrahedrons[2][1] = element->getCellId(1);
		tetrahedrons[2][2] = element->getCellId(2);
		tetrahedrons[2][3] = element->getCellId(5);
	}
	else if( n2 && !n3 && !n4 )
	{

		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(1);
		tetrahedrons[0][2] = element->getCellId(2);
		tetrahedrons[0][3] = element->getCellId(3);

		tetrahedrons[1][0] = element->getCellId(1);
		tetrahedrons[1][1] = element->getCellId(5);
		tetrahedrons[1][2] = element->getCellId(3);
		tetrahedrons[1][3] = element->getCellId(4);

		tetrahedrons[2][0] = element->getCellId(1);
		tetrahedrons[2][1] = element->getCellId(5);
		tetrahedrons[2][2] = element->getCellId(2);
		tetrahedrons[2][3] = element->getCellId(3);
	}
	else if( n2 && n3 && !n4 )
	{

		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(1);
		tetrahedrons[0][2] = element->getCellId(2);
		tetrahedrons[0][3] = element->getCellId(3);

		tetrahedrons[1][0] = element->getCellId(1);
		tetrahedrons[1][1] = element->getCellId(4);
		tetrahedrons[1][2] = element->getCellId(2);
		tetrahedrons[1][3] = element->getCellId(3);

		tetrahedrons[2][0] = element->getCellId(2);
		tetrahedrons[2][1] = element->getCellId(3);
		tetrahedrons[2][2] = element->getCellId(4);
		tetrahedrons[2][3] = element->getCellId(5);
	}
	else
	{
		num = 0;
	}
	return num;
}


































































































































































































