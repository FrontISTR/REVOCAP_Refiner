/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Wedge                                                   #
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
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Geometry/kmbOptimization.h"

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

形状関数 serendipity 0 <= s,t, <= 1, -1 <= u <= 1, s+t <= 1
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

double
kmb::Wedge::checkShapeFunctionDomain(double s,double t,double u)
{

	kmb::Minimizer minimizer;
	minimizer.update( s );
	minimizer.update( t );
	minimizer.update( 1.0-s-t );
	minimizer.update( 1.0-u );
	minimizer.update( 1.0+u );
	return minimizer.getMin();
}

bool
kmb::Wedge::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	/*
	 * wedgeの要素座標を求めるためにニュートン法を行う
	 */
	class nr_local : public kmb::OptTargetVV {
	public:
		kmb::Point3D target;
		const kmb::Point3D* points;
		int getDomainDim(void) const { return 3; };
		int getRangeDim(void) const { return 3; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			double coeff[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Wedge::shapeFunction( t[0], t[1], t[2], coeff );
			double x=0.0,y=0.0,z=0.0;
			for(int i=0;i<6;++i){
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

				jac.set(i,0,0.5 * (
					-(1.0-t[2])*points[0][i]
					+(1.0-t[2])*points[1][i]
					-(1.0+t[2])*points[3][i]
					+(1.0+t[2])*points[4][i] ) );

				jac.set(i,1,0.5 * (
					-(1.0-t[2])*points[0][i]
					+(1.0-t[2])*points[2][i]
					-(1.0+t[2])*points[3][i]
					+(1.0+t[2])*points[5][i] ) );

				jac.set(i,2,0.5 * (
					-(1.0-t[0]-t[1])*points[0][i]
					-t[0]*points[1][i]
					-t[1]*points[2][i]
					+(1.0-t[0]-t[1])*points[3][i]
					+t[0]*points[4][i]
					+t[1]*points[5][i] ) );
			}
			return true;
		}
		nr_local(const kmb::Point3D &t,const kmb::Point3D* pt)
		: target(t), points(pt){}
	};
	nr_local opt_obj(target,points);
	double min_t[3]  = { 0.0,  0.0, -1.0};
	double max_t[3]  = { 1.0,  1.0,  1.0};
	kmb::Optimization opt;
	bool res = opt.calcZero_NR( opt_obj, naturalCoords, min_t, max_t );
	return res;
}

bool
kmb::Wedge::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[6];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<6;++j){
			target.addCoordinate(i,points[j].getCoordinate(i) * coeff[j]);
		}
	}
	return true;
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
