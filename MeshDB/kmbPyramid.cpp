/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Pyramid                                                 #
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
#include "MeshDB/kmbPyramid.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElementRelation.h"

#include "Matrix/kmbMatrix.h"
#include "Matrix/kmbVector.h"
#include "Geometry/kmbOptimization.h"

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

double
kmb::Pyramid::checkShapeFunctionDomain(double s,double t,double u)
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
kmb::Pyramid::getNaturalCoordinates(const kmb::Point3D &target,const kmb::Point3D* points,double naturalCoords[3])
{
	if( points == NULL ){
		return false;
	}
	/*
	 * pyramidの要素座標を求めるためにニュートン法を行う
	 */
	class nr_local : public kmb::OptTargetVV {
	public:
		kmb::Point3D target;
		const kmb::Point3D* points;
		int getDomainDim(void) const { return 3; };
		int getRangeDim(void) const { return 3; };
		bool f(const kmb::ColumnVector &t,kmb::ColumnVector &val){
			double coeff[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
			kmb::Pyramid::shapeFunction( t[0], t[1], t[2], coeff );
			double x=0.0,y=0.0,z=0.0;
			for(int i=0;i<5;++i){
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

				jac.set(i,0,0.125 * (
					-(1.0-t[1])*(1.0-t[2])*points[1][i]
					+(1.0-t[1])*(1.0-t[2])*points[2][i]
					+(1.0+t[1])*(1.0-t[2])*points[3][i]
					-(1.0+t[1])*(1.0-t[2])*points[4][i] ) );

				jac.set(i,1,0.125 * (
					-(1.0-t[0])*(1.0-t[2])*points[1][i]
					-(1.0+t[0])*(1.0-t[2])*points[2][i]
					+(1.0+t[0])*(1.0-t[2])*points[3][i]
					+(1.0-t[0])*(1.0-t[2])*points[4][i] ) );

				jac.set(i,2,0.125 * (
					+4.0*points[0].getCoordinate(i)
					-(1.0-t[0])*(1.0-t[1])*points[1][i]
					-(1.0+t[0])*(1.0-t[1])*points[2][i]
					-(1.0+t[0])*(1.0+t[1])*points[3][i]
					-(1.0-t[0])*(1.0+t[1])*points[4][i] ) );
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
kmb::Pyramid::getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,kmb::Point3D &target)
{
	if( points == NULL ){
		return false;
	}
	double coeff[5];
	shapeFunction( naturalCoords[0], naturalCoords[1], naturalCoords[2], coeff );
	target.zero();
	for(int i=0;i<3;++i){
		for(int j=0;j<5;++j){
			target.addCoordinate(i,points[j].getCoordinate(i) * coeff[j]);
		}
	}
	return true;
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
