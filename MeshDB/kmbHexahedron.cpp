/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Hexahedron                                              #
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
#include <cmath>
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbElementRelation.h"

#include "Geometry/kmb_Calculator.h"

/********************************************************************************
=begin

=== 1次六面体要素 (HEXAHEDRON)

接続行列

	{ 0, 1, 0, 1, 1, 0, 0, 0},
	{ 1, 0, 1, 0, 0, 1, 0, 0},
	{ 0, 1, 0, 1, 0, 0, 1, 0},
	{ 1, 0, 1, 0, 0, 0, 0, 1},
	{ 1, 0, 0, 0, 0, 1, 0, 1},
	{ 0, 1, 0, 0, 1, 0, 1, 0},
	{ 0, 0, 1, 0, 0, 1, 0, 1},
	{ 0, 0, 0, 1, 1, 0, 1, 0}

面

	{ 3, 2, 1, 0},
	{ 4, 5, 6, 7},
	{ 1, 5, 4, 0},
	{ 1, 2, 6, 5},
	{ 3, 7, 6, 2},
	{ 4, 7, 3, 0}

辺

	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 0, 3},
	{ 4, 5},
	{ 5, 6},
	{ 6, 7},
	{ 4, 7},
	{ 0, 4},
	{ 1, 5},
	{ 2, 6},
	{ 3, 7}

=end

旧バージョン
	{ 3, 2, 1, 0},
	{ 4, 5, 6, 7},
	{ 0, 1, 5, 4},
	{ 1, 2, 6, 5},
	{ 2, 3, 7, 6},
	{ 0, 4, 7, 3}


形状関数：
0 : 1/8(1-s)(1-t)(1-u) => (s,t,u) = (-1,-1,-1)
1 : 1/8(1+s)(1-t)(1-u) => (s,t,u) = ( 1,-1,-1)
2 : 1/8(1+s)(1+t)(1-u) => (s,t,u) = ( 1, 1,-1)
3 : 1/8(1-s)(1+t)(1-u) => (s,t,u) = (-1, 1,-1)
4 : 1/8(1-s)(1-t)(1+u) => (s,t,u) = (-1,-1, 1)
5 : 1/8(1+s)(1-t)(1+u) => (s,t,u) = ( 1,-1, 1)
6 : 1/8(1+s)(1+t)(1+u) => (s,t,u) = ( 1, 1, 1)
7 : 1/8(1-s)(1+t)(1+u) => (s,t,u) = (-1, 1, 1)

*********************************************************************************/
const int kmb::Hexahedron::nodeCount = 8;


const int kmb::Hexahedron::connectionTable[8][8] =
{
	{ 0, 1, 0, 1, 1, 0, 0, 0},
	{ 1, 0, 1, 0, 0, 1, 0, 0},
	{ 0, 1, 0, 1, 0, 0, 1, 0},
	{ 1, 0, 1, 0, 0, 0, 0, 1},
	{ 1, 0, 0, 0, 0, 1, 0, 1},
	{ 0, 1, 0, 0, 1, 0, 1, 0},
	{ 0, 0, 1, 0, 0, 1, 0, 1},
	{ 0, 0, 0, 1, 1, 0, 1, 0}
};



const int kmb::Hexahedron::faceTable[6][4] =
{
	{ 3, 2, 1, 0},
	{ 4, 5, 6, 7},
	{ 1, 5, 4, 0},
	{ 1, 2, 6, 5},
	{ 3, 7, 6, 2},
	{ 4, 7, 3, 0}
};

const int kmb::Hexahedron::edgeTable[12][2] =
{
	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 0, 3},
	{ 4, 5},
	{ 5, 6},
	{ 6, 7},
	{ 4, 7},
	{ 0, 4},
	{ 1, 5},
	{ 2, 6},
	{ 3, 7}
};

bool
kmb::Hexahedron::isEquivalent(int index[8])
{
	const int len = kmb::Element::getNodeCount(kmb::HEXAHEDRON);

	for(int i=0;i<len;++i){
		if(index[i] < 0 || len <= index[i]){
			return false;
		}
	}

	for(int i=0;i<len;++i){
		for(int j=0;j<len;++j){
			if( kmb::Hexahedron::connectionTable[i][j]
				!= kmb::Hexahedron::connectionTable[ index[i] ][ index[j] ] )
			{
				return false;
			}
		}
	}
	return true;
}

kmb::Hexahedron::Hexahedron(void)
: kmb::Element(kmb::HEXAHEDRON)
{
	cell = new kmb::nodeIdType[8];
}

kmb::Hexahedron::Hexahedron(kmb::nodeIdType *ary)
: kmb::Element(kmb::HEXAHEDRON)
{
	cell = ary;
}

kmb::Hexahedron::
~Hexahedron(void)
{
}

int
kmb::Hexahedron::divideIntoTetrahedrons(const kmb::ElementBase* element,kmb::nodeIdType tetrahedrons[][4])
{
	if( element == NULL ||
		( element->getType() != kmb::HEXAHEDRON && element->getType() != kmb::HEXAHEDRON2 ) ){
		return 0;
	}
	int num = 0;


	bool b0 = element->getIndexMinNodeIdOfFace( 0 )%2 == 1;
	bool b1 = element->getIndexMinNodeIdOfFace( 1 )%2 == 1;
	bool b2 = element->getIndexMinNodeIdOfFace( 2 )%2 == 1;
	bool b3 = element->getIndexMinNodeIdOfFace( 3 )%2 == 1;
	bool b4 = element->getIndexMinNodeIdOfFace( 4 )%2 == 1;
	bool b5 = element->getIndexMinNodeIdOfFace( 5 )%2 == 1;







































































	if( !b0 && !b1 && !b2 && !b3 && !b4 && !b5 )
	{

		num = 5;
		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(1);
		tetrahedrons[0][2] = element->getCellId(3);
		tetrahedrons[0][3] = element->getCellId(4);

		tetrahedrons[1][0] = element->getCellId(1);
		tetrahedrons[1][1] = element->getCellId(5);
		tetrahedrons[1][2] = element->getCellId(6);
		tetrahedrons[1][3] = element->getCellId(4);

		tetrahedrons[2][0] = element->getCellId(1);
		tetrahedrons[2][1] = element->getCellId(2);
		tetrahedrons[2][2] = element->getCellId(3);
		tetrahedrons[2][3] = element->getCellId(6);

		tetrahedrons[3][0] = element->getCellId(7);
		tetrahedrons[3][1] = element->getCellId(6);
		tetrahedrons[3][2] = element->getCellId(4);
		tetrahedrons[3][3] = element->getCellId(3);

		tetrahedrons[4][0] = element->getCellId(1);
		tetrahedrons[4][1] = element->getCellId(3);
		tetrahedrons[4][2] = element->getCellId(4);
		tetrahedrons[4][3] = element->getCellId(6);
	}
	else if( b0 && b1 && b2 && b3 && b4 && b5 )
	{

		num = 5;
		tetrahedrons[0][0] = element->getCellId(0);
		tetrahedrons[0][1] = element->getCellId(1);
		tetrahedrons[0][2] = element->getCellId(2);
		tetrahedrons[0][3] = element->getCellId(5);

		tetrahedrons[1][0] = element->getCellId(2);
		tetrahedrons[1][1] = element->getCellId(5);
		tetrahedrons[1][2] = element->getCellId(6);
		tetrahedrons[1][3] = element->getCellId(7);

		tetrahedrons[2][0] = element->getCellId(0);
		tetrahedrons[2][1] = element->getCellId(4);
		tetrahedrons[2][2] = element->getCellId(5);
		tetrahedrons[2][3] = element->getCellId(7);

		tetrahedrons[3][0] = element->getCellId(3);
		tetrahedrons[3][1] = element->getCellId(0);
		tetrahedrons[3][2] = element->getCellId(2);
		tetrahedrons[3][3] = element->getCellId(7);

		tetrahedrons[4][0] = element->getCellId(0);
		tetrahedrons[4][1] = element->getCellId(5);
		tetrahedrons[4][2] = element->getCellId(2);
		tetrahedrons[4][3] = element->getCellId(7);
	}
	else
	{
		num = 0;
		kmb::Wedge* w0 = NULL;
		kmb::Wedge* w1 = NULL;
		if( !b0 &&  b1 )
		{

			w0 = new kmb::Wedge(
				element->getCellId(0),element->getCellId(1),element->getCellId(3),
				element->getCellId(4),element->getCellId(5),element->getCellId(7));
			w1 = new kmb::Wedge(
				element->getCellId(1),element->getCellId(2),element->getCellId(3),
				element->getCellId(5),element->getCellId(6),element->getCellId(7));
		}
		else if( b0 && !b1 )
		{

			w0 = new kmb::Wedge(
				element->getCellId(0),element->getCellId(1),element->getCellId(2),
				element->getCellId(4),element->getCellId(5),element->getCellId(6));
			w1 = new kmb::Wedge(
				element->getCellId(0),element->getCellId(2),element->getCellId(3),
				element->getCellId(4),element->getCellId(6),element->getCellId(7));
		}
		else if( !b2 && b4 )
		{

			w0 = new kmb::Wedge(
				element->getCellId(4),element->getCellId(5),element->getCellId(1),
				element->getCellId(7),element->getCellId(6),element->getCellId(2));
			w1 = new kmb::Wedge(
				element->getCellId(4),element->getCellId(1),element->getCellId(0),
				element->getCellId(7),element->getCellId(2),element->getCellId(3));
		}
		else if( b2 && !b4 )
		{

			w0 = new kmb::Wedge(
				element->getCellId(0),element->getCellId(5),element->getCellId(1),
				element->getCellId(3),element->getCellId(6),element->getCellId(2));
			w1 = new kmb::Wedge(
				element->getCellId(0),element->getCellId(4),element->getCellId(5),
				element->getCellId(3),element->getCellId(7),element->getCellId(6));
		}
		else if( !b3 && b5 )
		{

			w0 = new kmb::Wedge(
				element->getCellId(1),element->getCellId(6),element->getCellId(2),
				element->getCellId(0),element->getCellId(7),element->getCellId(3));
			w1 = new kmb::Wedge(
				element->getCellId(1),element->getCellId(5),element->getCellId(6),
				element->getCellId(0),element->getCellId(4),element->getCellId(7));
		}
		else if( b3 && !b5 )
		{

			w0 = new kmb::Wedge(
				element->getCellId(2),element->getCellId(5),element->getCellId(6),
				element->getCellId(3),element->getCellId(4),element->getCellId(7));
			w1 = new kmb::Wedge(
				element->getCellId(1),element->getCellId(5),element->getCellId(2),
				element->getCellId(0),element->getCellId(4),element->getCellId(3));
		}
		else
		{
			num = 0;
		}
		if( w0 ){
			if( kmb::Wedge::divideIntoTetrahedrons( w0, &tetrahedrons[num] ) == 3 )
			{
				num += 3;
			}
			delete w0;
		}
		if( w1 ){
			if( kmb::Wedge::divideIntoTetrahedrons( w1, &tetrahedrons[num] ) == 3 )
			{
				num += 3;
			}
			delete w1;
		}
	}
	return num;
}

























































































































































/*
 * 立方体の要素座標を求めるためにニュートン法を行う
 * 1/8 (1+s)*(1+t)*(1+u)
 * 1/8 (1-s)*(1+t)*(1+u)
 * 1/8 (1-s)*(1-t)*(1+u)
 * 1/8 (1+s)*(1-t)*(1+u)
 * 1/8 (1+s)*(1+t)*(1-u)
 * 1/8 (1-s)*(1+t)*(1-u)
 * 1/8 (1-s)*(1-t)*(1-u)
 * 1/8 (1+s)*(1-t)*(1-u)
 */
































































bool
kmb::Hexahedron::getNaturalCoordinates(const double physicalCoords[3],const kmb::Point3D* points,double naturalCoords[3],double margin)
{

	if( points == NULL ){
		return false;
	}
	/*
	 * 立方体の要素座標を求めるためにニュートン法を行う
	 */
	double d = 0.0;
	double thres = 1.0e-10;
	kmb::BoundingBox bbox;
	for(int i=0;i<8;++i){
		bbox.update( points[i] );
	}
	bbox.expand( margin );
	if( bbox.intersect(physicalCoords[0],physicalCoords[1],physicalCoords[2]) == kmb::BoxRegion::OUTSIDE ){
		return false;
	}

	for(int j=0;j<10;++j){
		for(int i=0;i<50;++i){
			d = kmb::Hexahedron::newtonMethod(physicalCoords,points,naturalCoords);
			if( d < thres &&
				fabs(naturalCoords[0]) <= 1.0 &&
				fabs(naturalCoords[1]) <= 1.0 &&
				fabs(naturalCoords[2]) <= 1.0 )
			{
				return true;
			}
		}
		if( d < thres &&
			fabs(naturalCoords[0]) <= margin &&
			fabs(naturalCoords[1]) <= margin &&
			fabs(naturalCoords[2]) <= margin )
		{
			return true;
		}


		naturalCoords[0] = -0.01*j*naturalCoords[1];
		naturalCoords[1] = -0.01*j*naturalCoords[2];
		naturalCoords[2] = -0.01*j*naturalCoords[0];
	}
	return false;
}

void
kmb::Hexahedron::shapeFunction(double s,double t,double u,double* coeff)
{
	coeff[0] = 0.125*(1.0-s)*(1.0-t)*(1.0-u);
	coeff[1] = 0.125*(1.0+s)*(1.0-t)*(1.0-u);
	coeff[2] = 0.125*(1.0+s)*(1.0+t)*(1.0-u);
	coeff[3] = 0.125*(1.0-s)*(1.0+t)*(1.0-u);
	coeff[4] = 0.125*(1.0-s)*(1.0-t)*(1.0+u);
	coeff[5] = 0.125*(1.0+s)*(1.0-t)*(1.0+u);
	coeff[6] = 0.125*(1.0+s)*(1.0+t)*(1.0+u);
	coeff[7] = 0.125*(1.0-s)*(1.0+t)*(1.0+u);
}

bool
kmb::Hexahedron::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,double physicalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	double coeff[8];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	for(int i=0;i<3;++i){
		physicalCoords[i] = 0.0;
		for(int j=0;j<8;++j){
			physicalCoords[i] += points[j].getCoordinate(i) * coeff[j];
		}
	}
	return true;
}

double
kmb::Hexahedron::newtonMethod(const double physicalCoords[3], const kmb::Point3D* nodes, double naturalCoords[3])
{
	double retVal = DBL_MAX;
	if( nodes == NULL ){
		return retVal;
	}
	double q[3];
	if( !getPhysicalCoordinates( naturalCoords, nodes, q ) ){
		return retVal;
	}
	double ds[3] = {0.0,0.0,0.0};
	double dt[3] = {0.0,0.0,0.0};
	double du[3] = {0.0,0.0,0.0};

	ds[0] = 0.125 * (
		-(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*nodes[0].x()
		+(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*nodes[1].x()
		+(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*nodes[2].x()
		-(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*nodes[3].x()
		-(1.0-naturalCoords[1])*(1.0+naturalCoords[2])*nodes[4].x()
		+(1.0-naturalCoords[1])*(1.0+naturalCoords[2])*nodes[5].x()
		+(1.0+naturalCoords[1])*(1.0+naturalCoords[2])*nodes[6].x()
		-(1.0+naturalCoords[1])*(1.0+naturalCoords[2])*nodes[7].x() );
	ds[1] = 0.125 * (
		-(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*nodes[0].y()
		+(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*nodes[1].y()
		+(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*nodes[2].y()
		-(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*nodes[3].y()
		-(1.0-naturalCoords[1])*(1.0+naturalCoords[2])*nodes[4].y()
		+(1.0-naturalCoords[1])*(1.0+naturalCoords[2])*nodes[5].y()
		+(1.0+naturalCoords[1])*(1.0+naturalCoords[2])*nodes[6].y()
		-(1.0+naturalCoords[1])*(1.0+naturalCoords[2])*nodes[7].y() );
	ds[2] = 0.125 * (
		-(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*nodes[0].z()
		+(1.0-naturalCoords[1])*(1.0-naturalCoords[2])*nodes[1].z()
		+(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*nodes[2].z()
		-(1.0+naturalCoords[1])*(1.0-naturalCoords[2])*nodes[3].z()
		-(1.0-naturalCoords[1])*(1.0+naturalCoords[2])*nodes[4].z()
		+(1.0-naturalCoords[1])*(1.0+naturalCoords[2])*nodes[5].z()
		+(1.0+naturalCoords[1])*(1.0+naturalCoords[2])*nodes[6].z()
		-(1.0+naturalCoords[1])*(1.0+naturalCoords[2])*nodes[7].z() );

	dt[0] = 0.125 * (
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*nodes[0].x()
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*nodes[1].x()
		+(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*nodes[2].x()
		+(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*nodes[3].x()
		-(1.0-naturalCoords[0])*(1.0+naturalCoords[2])*nodes[4].x()
		-(1.0+naturalCoords[0])*(1.0+naturalCoords[2])*nodes[5].x()
		+(1.0+naturalCoords[0])*(1.0+naturalCoords[2])*nodes[6].x()
		+(1.0-naturalCoords[0])*(1.0+naturalCoords[2])*nodes[7].x() );
	dt[1] = 0.125 * (
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*nodes[0].y()
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*nodes[1].y()
		+(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*nodes[2].y()
		+(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*nodes[3].y()
		-(1.0-naturalCoords[0])*(1.0+naturalCoords[2])*nodes[4].y()
		-(1.0+naturalCoords[0])*(1.0+naturalCoords[2])*nodes[5].y()
		+(1.0+naturalCoords[0])*(1.0+naturalCoords[2])*nodes[6].y()
		+(1.0-naturalCoords[0])*(1.0+naturalCoords[2])*nodes[7].y() );
	dt[2] = 0.125 * (
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*nodes[0].z()
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*nodes[1].z()
		+(1.0+naturalCoords[0])*(1.0-naturalCoords[2])*nodes[2].z()
		+(1.0-naturalCoords[0])*(1.0-naturalCoords[2])*nodes[3].z()
		-(1.0-naturalCoords[0])*(1.0+naturalCoords[2])*nodes[4].z()
		-(1.0+naturalCoords[0])*(1.0+naturalCoords[2])*nodes[5].z()
		+(1.0+naturalCoords[0])*(1.0+naturalCoords[2])*nodes[6].z()
		+(1.0-naturalCoords[0])*(1.0+naturalCoords[2])*nodes[7].z() );

	du[0] = 0.125 * (
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[1])*nodes[0].x()
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[1])*nodes[1].x()
		-(1.0+naturalCoords[0])*(1.0+naturalCoords[1])*nodes[2].x()
		-(1.0-naturalCoords[0])*(1.0+naturalCoords[1])*nodes[3].x()
		+(1.0-naturalCoords[0])*(1.0-naturalCoords[1])*nodes[4].x()
		+(1.0+naturalCoords[0])*(1.0-naturalCoords[1])*nodes[5].x()
		+(1.0+naturalCoords[0])*(1.0+naturalCoords[1])*nodes[6].x()
		+(1.0-naturalCoords[0])*(1.0+naturalCoords[1])*nodes[7].x() );
	du[1] = 0.125 * (
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[1])*nodes[0].y()
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[1])*nodes[1].y()
		-(1.0+naturalCoords[0])*(1.0+naturalCoords[1])*nodes[2].y()
		-(1.0-naturalCoords[0])*(1.0+naturalCoords[1])*nodes[3].y()
		+(1.0-naturalCoords[0])*(1.0-naturalCoords[1])*nodes[4].y()
		+(1.0+naturalCoords[0])*(1.0-naturalCoords[1])*nodes[5].y()
		+(1.0+naturalCoords[0])*(1.0+naturalCoords[1])*nodes[6].y()
		+(1.0-naturalCoords[0])*(1.0+naturalCoords[1])*nodes[7].y() );
	du[2] = 0.125 * (
		-(1.0-naturalCoords[0])*(1.0-naturalCoords[1])*nodes[0].z()
		-(1.0+naturalCoords[0])*(1.0-naturalCoords[1])*nodes[1].z()
		-(1.0+naturalCoords[0])*(1.0+naturalCoords[1])*nodes[2].z()
		-(1.0-naturalCoords[0])*(1.0+naturalCoords[1])*nodes[3].z()
		+(1.0-naturalCoords[0])*(1.0-naturalCoords[1])*nodes[4].z()
		+(1.0+naturalCoords[0])*(1.0-naturalCoords[1])*nodes[5].z()
		+(1.0+naturalCoords[0])*(1.0+naturalCoords[1])*nodes[6].z()
		+(1.0-naturalCoords[0])*(1.0+naturalCoords[1])*nodes[7].z() );
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
kmb::Hexahedron::isOppositeFace(const kmb::ElementBase* hexa,const kmb::ElementBase* quad0,const kmb::ElementBase* quad1)
{
	if( hexa == NULL || hexa->getType() != kmb::HEXAHEDRON ||
		quad0 == NULL || quad0->getType() != kmb::QUAD ||
		quad1 == NULL || quad1->getType() != kmb::QUAD ){
		return -1;
	}




	int corr[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
	for(int i=0;i<4;++i){
		int index0 = hexa->indexOf( quad0->getCellId(i) );
		int index1 = hexa->indexOf( quad1->getCellId(i) );
		if( index0 >= 0 && index1 >= 0 ){
			corr[index0] = i;
			corr[index1] = 4+i;
		}else{
			return -1;
		}
	}

	if( corr[0] < 4 ){
		return (corr[6]-corr[0]+2)%4;
	}else{
		return (corr[0]-corr[6]+2)%4;
	}
}
