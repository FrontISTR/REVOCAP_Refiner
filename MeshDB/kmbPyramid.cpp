/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Pyramid                                                 #
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
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElementRelation.h"

/********************************************************************************
=begin

=== 1次四角錐要素 (PYRAMID)

接続行列

	{ 0, 1, 1, 1, 1},
	{ 1, 0, 1, 0, 1},
	{ 1, 1, 0, 1, 0},
	{ 1, 0, 1, 0, 1},
	{ 1, 1, 0, 1, 0}

面

	{ 0, 1, 2},
	{ 0, 2, 3},
	{ 0, 3, 4},
	{ 0, 4, 1},
	{ 4, 3, 2, 1}

辺

	{ 0, 1},
	{ 0, 2},
	{ 0, 3},
	{ 0, 4},
	{ 1, 2},
	{ 2, 3},
	{ 3, 4},
	{ 1, 4}

=end

形状関数 (-1 <= s,t,u <= 1)
0 : 1/2(1+u)                     => (s,t,u) = (  0,  0,  1)
1 : 1/8(1-s)(1-t)(1-u)           => (s,t,u) = ( -1, -1, -1)
2 : 1/8(1+s)(1-t)(1-u)           => (s,t,u) = (  1, -1, -1)
3 : 1/8(1+s)(1+t)(1-u)           => (s,t,u) = (  1,  1, -1)
4 : 1/8(1-s)(1+t)(1-u)           => (s,t,u) = ( -1,  1, -1)

*********************************************************************************/
const int kmb::Pyramid::nodeCount = 5;


const int kmb::Pyramid::connectionTable[5][5] =
{
	{ 0, 1, 1, 1, 1},
	{ 1, 0, 1, 0, 1},
	{ 1, 1, 0, 1, 0},
	{ 1, 0, 1, 0, 1},
	{ 1, 1, 0, 1, 0}
};












const int kmb::Pyramid::faceTable[5][4] =
{
	{ 0, 1, 2,-1},
	{ 0, 2, 3,-1},
	{ 0, 3, 4,-1},
	{ 0, 4, 1,-1},
	{ 4, 3, 2, 1}
};

const int kmb::Pyramid::edgeTable[8][2] =
{
	{ 0, 1},
	{ 0, 2},
	{ 0, 3},
	{ 0, 4},
	{ 1, 2},
	{ 2, 3},
	{ 3, 4},
	{ 1, 4}
};

bool
kmb::Pyramid::isEquivalent(int index[5])
{
	const int len = kmb::Element::getNodeCount(kmb::PYRAMID);

	for(int i=0;i<len;++i){
		if(index[i] < 0 || len <= index[i]){
			return false;
		}
	}

	for(int i=0;i<len;++i){
		for(int j=0;j<len;++j){
			if( kmb::Pyramid::connectionTable[i][j]
				!= kmb::Pyramid::connectionTable[ index[i] ][ index[j] ] )
			{
				return false;
			}
		}
	}
	return true;
}

kmb::Pyramid::Pyramid(void)
: kmb::Element(kmb::PYRAMID)
{
	cell = new kmb::nodeIdType[6];
}

kmb::Pyramid::Pyramid(kmb::nodeIdType* ary)
: kmb::Element(kmb::PYRAMID)
{
	cell = ary;
}

kmb::Pyramid::~Pyramid(void)
{
}

void
kmb::Pyramid::shapeFunction(double s,double t,double u,double* coeff)
{
	coeff[0]  = 0.5*(1.0+u);
	coeff[1]  = 0.125*(1.0-s)*(1.0-t)*(1.0-u);
	coeff[2]  = 0.125*(1.0+s)*(1.0-t)*(1.0-u);
	coeff[3]  = 0.125*(1.0+s)*(1.0+t)*(1.0-u);
	coeff[4]  = 0.125*(1.0-s)*(1.0+t)*(1.0-u);
}

bool
kmb::Pyramid::getNaturalCoordinates(const double physicalCoords[3],const kmb::Point3D* points,double naturalCoords[3],double margin)
{
	if( points == NULL ){
		return false;
	}
	/*
	 * pyramidの要素座標を求めるためにニュートン法を行う
	 */
	const int len = 5;
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
			d = kmb::Pyramid::newtonMethod(physicalCoords,points,naturalCoords);
			if( d < thres &&
				-1.0 <= naturalCoords[0] && naturalCoords[0] <= 1.0 &&
				-1.0 <= naturalCoords[1] && naturalCoords[1] <= 1.0 &&
				-1.0 <= naturalCoords[2] && naturalCoords[2] <= 1.0 )
			{
				return true;
			}
		}
		if( d < thres &&
			-margin-1.0 <= naturalCoords[0] && naturalCoords[0] <= 1.0 + margin &&
			-margin-1.0 <= naturalCoords[1] && naturalCoords[1] <= 1.0 + margin &&
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
kmb::Pyramid::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,double physicalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	double coeff[5];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	for(int i=0;i<3;++i){
		physicalCoords[i] = 0.0;
		for(int j=0;j<5;++j){
			physicalCoords[i] += points[j].getCoordinate(i) * coeff[j];
		}
	}
	return true;
}

double
kmb::Pyramid::newtonMethod(const double physicalCoords[3], const kmb::Point3D* points, double naturalCoords[3])
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
		ds[i] = 0.125 * (
		-(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*points[1].getCoordinate(i)
		+(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*points[2].getCoordinate(i)
		+(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*points[3].getCoordinate(i)
		-(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*points[4].getCoordinate(i) );
		dt[i] = 0.125 * (
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*points[1].getCoordinate(i)
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*points[2].getCoordinate(i)
		+(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*points[3].getCoordinate(i)
		+(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*points[4].getCoordinate(i) );
		du[i] = 0.125 * (
		+4.0*points[0].getCoordinate(i)
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[1])*points[1].getCoordinate(i)
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[1])*points[2].getCoordinate(i)
		-(1.0+naturalCoords[0])*(1.0+naturalCoords[1])*points[3].getCoordinate(i)
		-(1.0-naturalCoords[0])*(1.0+naturalCoords[1])*points[4].getCoordinate(i) );
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
kmb::Pyramid::divideIntoTetrahedrons(const kmb::ElementBase* element,kmb::nodeIdType tetrahedrons[][4])
{
	if( element == NULL ||
		( element->getType() != kmb::PYRAMID && element->getType() != kmb::PYRAMID2 ) ){
		return 0;
	}
	int num = 2;


	bool n4 = element->getIndexMinNodeIdOfFace( 4 )%2 == 1;

	if( !n4 )
	{

		tetrahedrons[0][0] = element->getCellId(1);
		tetrahedrons[0][1] = element->getCellId(2);
		tetrahedrons[0][2] = element->getCellId(4);
		tetrahedrons[0][3] = element->getCellId(0);
		tetrahedrons[1][0] = element->getCellId(2);
		tetrahedrons[1][1] = element->getCellId(3);
		tetrahedrons[1][2] = element->getCellId(4);
		tetrahedrons[1][3] = element->getCellId(0);
	}
	else
	{

		tetrahedrons[0][0] = element->getCellId(1);
		tetrahedrons[0][1] = element->getCellId(2);
		tetrahedrons[0][2] = element->getCellId(3);
		tetrahedrons[0][3] = element->getCellId(0);
		tetrahedrons[1][0] = element->getCellId(1);
		tetrahedrons[1][1] = element->getCellId(3);
		tetrahedrons[1][2] = element->getCellId(4);
		tetrahedrons[1][3] = element->getCellId(0);
	}
	return num;
}


















































































































































































































