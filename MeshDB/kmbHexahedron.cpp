/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Hexahedron                                              #
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
#include <cmath>
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbElementRelation.h"

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbOptimization.h"

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

kmb::Hexahedron::Hexahedron(
		kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3,
		kmb::nodeIdType n4, kmb::nodeIdType n5, kmb::nodeIdType n6, kmb::nodeIdType n7)
: kmb::Element(kmb::HEXAHEDRON)
{
	cell = new kmb::nodeIdType[8];
	cell[0] = n0;
	cell[1] = n1;
	cell[2] = n2;
	cell[3] = n3;
	cell[4] = n4;
	cell[5] = n5;
	cell[6] = n6;
	cell[7] = n7;
}

kmb::Hexahedron::Hexahedron(kmb::nodeIdType *ary)
: kmb::Element(kmb::HEXAHEDRON)
{
	cell = ary;
}

kmb::Hexahedron::~Hexahedron(void)
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

double
kmb::Hexahedron::checkShapeFunctionDomain(double s,double t,double u)
{
	kmb::Minimizer minimizer;
	minimizer.update( 1.0-s );
	minimizer.update( 1.0+s );
	minimizer.update( 1.0-t );
	minimizer.update( 1.0+t );
	minimizer.update( 1.0-u );
	minimizer.update( 1.0+u );
	return minimizer.getMin();
}

bool
kmb::Hexahedron::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	/*
	 * 要素座標を求めるためにニュートン法を行う
	 */
	class nr_local : public kmb::OptTargetVV {
	public:
		kmb::Point3D target;
		const kmb::Point3D* points;
		int getDomainDim(void) const { return 3; };
		int getRangeDim(void) const { return 3; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			double coeff[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Hexahedron::shapeFunction( t[0], t[1], t[2], coeff );
			double x=0.0,y=0.0,z=0.0;
			for(int i=0;i<8;++i){
				x += coeff[i] * points[i].x();
				y += coeff[i] * points[i].y();
				z += coeff[i] * points[i].z();
			}
			val.setRow(0,x-target[0]);
			val.setRow(1,y-target[1]);
			val.setRow(2,z-target[2]);
			return true;
		}
		bool df(const ColumnVector &t,Matrix &jac){
			for(int i=0;i<3;++i){

				jac.set(i,0,
					0.125 * (
					-(1.0-t[1])*(1.0-t[2])*points[0][i]
					+(1.0-t[1])*(1.0-t[2])*points[1][i]
					+(1.0+t[1])*(1.0-t[2])*points[2][i]
					-(1.0+t[1])*(1.0-t[2])*points[3][i]
					-(1.0-t[1])*(1.0+t[2])*points[4][i]
					+(1.0-t[1])*(1.0+t[2])*points[5][i]
					+(1.0+t[1])*(1.0+t[2])*points[6][i]
					-(1.0+t[1])*(1.0+t[2])*points[7][i] ) );

				jac.set(i,1,
					0.125 * (
					-(1.0-t[0])*(1.0-t[2])*points[0][i]
					-(1.0+t[0])*(1.0-t[2])*points[1][i]
					+(1.0+t[0])*(1.0-t[2])*points[2][i]
					+(1.0-t[0])*(1.0-t[2])*points[3][i]
					-(1.0-t[0])*(1.0+t[2])*points[4][i]
					-(1.0+t[0])*(1.0+t[2])*points[5][i]
					+(1.0+t[0])*(1.0+t[2])*points[6][i]
					+(1.0-t[0])*(1.0+t[2])*points[7][i] ) );

				jac.set(i,2,
					0.125 * (
					-(1.0-t[0])*(1.0-t[1])*points[0][i]
					-(1.0+t[0])*(1.0-t[1])*points[1][i]
					-(1.0+t[0])*(1.0+t[1])*points[2][i]
					-(1.0-t[0])*(1.0+t[1])*points[3][i]
					+(1.0-t[0])*(1.0-t[1])*points[4][i]
					+(1.0+t[0])*(1.0-t[1])*points[5][i]
					+(1.0+t[0])*(1.0+t[1])*points[6][i]
					+(1.0-t[0])*(1.0+t[1])*points[7][i] ) );
			}
			return true;
		}
		nr_local(const kmb::Point3D &t,const kmb::Point3D* pt)
		: target(t), points(pt){}
	};
	nr_local opt_obj(target,points);
	double min_t[3]  = {-1.0, -1.0, -1.0};
	double max_t[3]  = { 1.0,  1.0,  1.0};
	kmb::Optimization opt;
	bool res = opt.calcZero_NR( opt_obj, naturalCoords, min_t, max_t );
	return res;
}

bool
kmb::Hexahedron::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[8];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<8;++j){
			target.addCoordinate(i,points[j][i] * coeff[j]);
		}
	}
	return true;
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

double
kmb::Hexahedron::jacobian(double s, double t,double u,const kmb::Point3D* points)
{
	double jxs = 0.125 * (
		( - points[0].x() + points[1].x() + points[2].x() - points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() )
		+ u * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ t * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ t * u * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jys = 0.125 * (
		( - points[0].y() + points[1].y() + points[2].y() - points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() )
		+ u * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ t * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ t * u * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzs = 0.125 * (
		( - points[0].z() + points[1].z() + points[2].z() - points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() )
		+ u * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ t * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ t * u * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxt = 0.125 * (
		( - points[0].x() - points[1].x() + points[2].x() + points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ u * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ u * s * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyt = 0.125 * (
		( - points[0].y() - points[1].y() + points[2].y() + points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ u * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ u * s * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzt = 0.125 * (
		( - points[0].z() - points[1].z() + points[2].z() + points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ u * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ u * s * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxu = 0.125 * (
		( - points[0].x() - points[1].x() - points[2].x() - points[3].x() + points[4].x() + points[5].x() + points[6].x() + points[7].x() ) +
		+ t * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ s * t * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyu = 0.125 * (
		( - points[0].y() - points[1].y() - points[2].y() - points[3].y() + points[4].y() + points[5].y() + points[6].y() + points[7].y() ) +
		+ t * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ s * t * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzu = 0.125 * (
		( - points[0].z() - points[1].z() - points[2].z() - points[3].z() + points[4].z() + points[5].z() + points[6].z() + points[7].z() ) +
		+ t * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ s * t * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	return
		( jxs * jyt * jzu + jys * jzt * jxu + jzs * jxt * jyu ) -
		( jxs * jzt * jyu + jys * jxt * jzu + jzs * jyt * jxu );
}



double
kmb::Hexahedron::jacobian_ds(double s, double t,double u,const kmb::Point3D* points)
{
	double jxs = 0.125 * (
		( - points[0].x() + points[1].x() + points[2].x() - points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() )
		+ u * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ t * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ t * u * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jys = 0.125 * (
		( - points[0].y() + points[1].y() + points[2].y() - points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() )
		+ u * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ t * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ t * u * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzs = 0.125 * (
		( - points[0].z() + points[1].z() + points[2].z() - points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() )
		+ u * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ t * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ t * u * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxt = 0.125 * (
		( - points[0].x() - points[1].x() + points[2].x() + points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ u * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ u * s * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyt = 0.125 * (
		( - points[0].y() - points[1].y() + points[2].y() + points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ u * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ u * s * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzt = 0.125 * (
		( - points[0].z() - points[1].z() + points[2].z() + points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ u * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ u * s * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxu = 0.125 * (
		( - points[0].x() - points[1].x() - points[2].x() - points[3].x() + points[4].x() + points[5].x() + points[6].x() + points[7].x() ) +
		+ t * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ s * t * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyu = 0.125 * (
		( - points[0].y() - points[1].y() - points[2].y() - points[3].y() + points[4].y() + points[5].y() + points[6].y() + points[7].y() ) +
		+ t * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ s * t * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzu = 0.125 * (
		( - points[0].z() - points[1].z() - points[2].z() - points[3].z() + points[4].z() + points[5].z() + points[6].z() + points[7].z() ) +
		+ t * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ s * t * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxts = 0.125 * (
		( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ u * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyts = 0.125 * (
		( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ u * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzts = 0.125 * (
		( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ u * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxus = 0.125 * (
		( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ t * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyus = 0.125 * (
		( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ t * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzus = 0.125 * (
		( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ t * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	return
		( jxs * jyts * jzu + jys * jzts * jxu + jzs * jxts * jyu ) -
		( jxs * jzts * jyu + jys * jxts * jzu + jzs * jyts * jxu ) +
		( jxs * jyt * jzus + jys * jzt * jxus + jzs * jxt * jyus ) -
		( jxs * jzt * jyus + jys * jxt * jzus + jzs * jyt * jxus );
}

double
kmb::Hexahedron::jacobian_dt(double s, double t,double u,const kmb::Point3D* points)
{
	double jxs = 0.125 * (
		( - points[0].x() + points[1].x() + points[2].x() - points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() )
		+ u * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ t * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ t * u * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jys = 0.125 * (
		( - points[0].y() + points[1].y() + points[2].y() - points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() )
		+ u * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ t * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ t * u * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzs = 0.125 * (
		( - points[0].z() + points[1].z() + points[2].z() - points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() )
		+ u * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ t * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ t * u * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxt = 0.125 * (
		( - points[0].x() - points[1].x() + points[2].x() + points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ u * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ u * s * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyt = 0.125 * (
		( - points[0].y() - points[1].y() + points[2].y() + points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ u * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ u * s * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzt = 0.125 * (
		( - points[0].z() - points[1].z() + points[2].z() + points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ u * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ u * s * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxu = 0.125 * (
		( - points[0].x() - points[1].x() - points[2].x() - points[3].x() + points[4].x() + points[5].x() + points[6].x() + points[7].x() ) +
		+ t * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ s * t * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyu = 0.125 * (
		( - points[0].y() - points[1].y() - points[2].y() - points[3].y() + points[4].y() + points[5].y() + points[6].y() + points[7].y() ) +
		+ t * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ s * t * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzu = 0.125 * (
		( - points[0].z() - points[1].z() - points[2].z() - points[3].z() + points[4].z() + points[5].z() + points[6].z() + points[7].z() ) +
		+ t * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ s * t * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxst = 0.125 * (
		( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ u * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyst = 0.125 * (
		( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ u * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzst = 0.125 * (
		( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ u * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );


	double jxut = 0.125 * (
		( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyut = 0.125 * (
		( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzut = 0.125 * (
		( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	return
		( jxst * jyt * jzu + jyst * jzt * jxu + jzst * jxt * jyu ) -
		( jxst * jzt * jyu + jyst * jxt * jzu + jzst * jyt * jxu ) +
		( jxs * jyt * jzut + jys * jzt * jxut + jzs * jxt * jyut ) -
		( jxs * jzt * jyut + jys * jxt * jzut + jzs * jyt * jxut );
}

double
kmb::Hexahedron::jacobian_du(double s, double t,double u,const kmb::Point3D* points)
{
	double jxs = 0.125 * (
		( - points[0].x() + points[1].x() + points[2].x() - points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() )
		+ u * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ t * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ t * u * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jys = 0.125 * (
		( - points[0].y() + points[1].y() + points[2].y() - points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() )
		+ u * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ t * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ t * u * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzs = 0.125 * (
		( - points[0].z() + points[1].z() + points[2].z() - points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() )
		+ u * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ t * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ t * u * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxt = 0.125 * (
		( - points[0].x() - points[1].x() + points[2].x() + points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ u * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() + points[2].x() - points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) +
		+ u * s * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyt = 0.125 * (
		( - points[0].y() - points[1].y() + points[2].y() + points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ u * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() + points[2].y() - points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) +
		+ u * s * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzt = 0.125 * (
		( - points[0].z() - points[1].z() + points[2].z() + points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ u * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() + points[2].z() - points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) +
		+ u * s * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxu = 0.125 * (
		( - points[0].x() - points[1].x() - points[2].x() - points[3].x() + points[4].x() + points[5].x() + points[6].x() + points[7].x() ) +
		+ t * ( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ s * t * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jyu = 0.125 * (
		( - points[0].y() - points[1].y() - points[2].y() - points[3].y() + points[4].y() + points[5].y() + points[6].y() + points[7].y() ) +
		+ t * ( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ s * t * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzu = 0.125 * (
		( - points[0].z() - points[1].z() - points[2].z() - points[3].z() + points[4].z() + points[5].z() + points[6].z() + points[7].z() ) +
		+ t * ( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ s * t * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxsu = 0.125 * (
		( points[0].x() - points[1].x() - points[2].x() + points[3].x() - points[4].x() + points[5].x() + points[6].x() - points[7].x() ) +
		+ t * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );

	double jysu = 0.125 * (
		( points[0].y() - points[1].y() - points[2].y() + points[3].y() - points[4].y() + points[5].y() + points[6].y() - points[7].y() ) +
		+ t * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );

	double jzsu = 0.125 * (
		( points[0].z() - points[1].z() - points[2].z() + points[3].z() - points[4].z() + points[5].z() + points[6].z() - points[7].z() ) +
		+ t * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	double jxtu = 0.125 * (
		( points[0].x() + points[1].x() - points[2].x() - points[3].x() - points[4].x() - points[5].x() + points[6].x() + points[7].x() ) +
		+ s * ( - points[0].x() + points[1].x() - points[2].x() + points[3].x() + points[4].x() - points[5].x() + points[6].x() - points[7].x() ) );


	double jytu = 0.125 * (
		( points[0].y() + points[1].y() - points[2].y() - points[3].y() - points[4].y() - points[5].y() + points[6].y() + points[7].y() ) +
		+ s * ( - points[0].y() + points[1].y() - points[2].y() + points[3].y() + points[4].y() - points[5].y() + points[6].y() - points[7].y() ) );


	double jztu = 0.125 * (
		( points[0].z() + points[1].z() - points[2].z() - points[3].z() - points[4].z() - points[5].z() + points[6].z() + points[7].z() ) +
		+ s * ( - points[0].z() + points[1].z() - points[2].z() + points[3].z() + points[4].z() - points[5].z() + points[6].z() - points[7].z() ) );

	return
		( jxsu * jyt * jzu + jysu * jzt * jxu + jzsu * jxt * jyu ) -
		( jxsu * jzt * jyu + jysu * jxt * jzu + jzsu * jyt * jxu ) +
		( jxs * jytu * jzu + jys * jztu * jxu + jzs * jxtu * jyu ) -
		( jxs * jztu * jyu + jys * jxtu * jzu + jzs * jytu * jxu );
}
