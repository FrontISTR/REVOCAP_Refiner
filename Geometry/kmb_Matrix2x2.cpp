/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Matrix2x2                                               #
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
#include <cfloat>
#include "Geometry/kmb_Geometry2D.h"
#include "Geometry/kmb_idTypes.h"

kmb::Matrix2x2::Matrix2x2(void)
: SquareMatrix(2)
{
	this->identity();
}

kmb::Matrix2x2::Matrix2x2(double m[4])
: SquareMatrix(2)
{
	for(int i=0;i<4;++i){
		this->m[i] = m[i];
	}
}

kmb::Matrix2x2::Matrix2x2(
		  double m00,double m01,
		  double m10,double m11)
: SquareMatrix(2)
{
	m[0] = m00;	m[2] = m01;
	m[1] = m10;	m[3] = m11;
}

int
kmb::Matrix2x2::getSize(void) const
{
	return 2;
}

double
kmb::Matrix2x2::get(int i,int j) const
{
	return m[i+j*2];
}

bool
kmb::Matrix2x2::set(int i,int j,double val)
{
	m[i+j*2] = val;
	return true;
}

bool
kmb::Matrix2x2::identity(void)
{
	m[0] = 1.0;	m[2] = 0.0;
	m[1] = 0.0;	m[3] = 1.0;
	return true;
}

bool
kmb::Matrix2x2::zero(void)
{
	m[0] = 0.0;	m[2] = 0.0;
	m[1] = 0.0;	m[3] = 0.0;
	return true;
}

bool
kmb::Matrix2x2::transpose(void)
{
	double tmp[4];
	for(int i=0;i<4;++i){
		tmp[i] = m[i];
	}
	for(int i=0;i<2;++i){
		m[i]	= tmp[2*i];
		m[2+i]	= tmp[2*i+1];
	}
	return true;
}

void
kmb::Matrix2x2::transpose(const kmb::Matrix2x2& other)
{
	for(int i=0;i<2;++i){
		m[i]	= other.m[2*i];
		m[2+i]	= other.m[2*i+1];
	}
}

double
kmb::Matrix2x2::determinant(void) const
{
	return m[0]*m[3] - m[1]*m[2];
}

double
kmb::Matrix2x2::trace(void) const
{
	return m[0] + m[3];
}

kmb::Matrix2x2
kmb::Matrix2x2::createRotation(double angle)
{
	double c = cos(angle);
	double s = sin(angle);
	return kmb::Matrix2x2(c,-s,s,c);
}

kmb::Matrix2x2
kmb::Matrix2x2::createSchmidtRotation(const kmb::Vector2D v0,bool column)
{
	kmb::Vector2D u0 = v0;
	u0.normalize();
	if( column ){
		return kmb::Matrix2x2(
			u0.x(), -u0.y(),
			u0.y(), u0.x()
			);
	}else{
		return kmb::Matrix2x2(
			u0.x(), u0.y(),
			-u0.y(), u0.x()
			);
	}
}

kmb::Vector2D
kmb::Matrix2x2::operator*(const kmb::Vector2D& vect)
{
	double x = m[0]*vect.x() + m[2]*vect.y();
	double y = m[1]*vect.x() + m[3]*vect.y();

	return kmb::Vector2D(x,y);
}

kmb::Matrix2x2
kmb::Matrix2x2::operator*(const kmb::Matrix2x2& other)
{
	double mat[4];
	for(int i=0;i<2;++i){
		mat[0+i] =
			m[i]*other.m[0] + m[2+i]*other.m[1];
		mat[2+i] =
			m[i]*other.m[2] + m[2+i]*other.m[3];
	}
	return kmb::Matrix2x2(mat);
}

kmb::Matrix2x2
kmb::Matrix2x2::operator+(const kmb::Matrix2x2& other)
{
	double mat[4];
	for(int i=0;i<4;++i){
		mat[i] = this->m[i] + other.m[i];
	}
	return kmb::Matrix2x2(mat);
}

kmb::Matrix2x2
kmb::Matrix2x2::operator-(const kmb::Matrix2x2& other)
{
	double mat[4];
	for(int i=0;i<4;++i){
		mat[i] = this->m[i] + other.m[i];
	}
	return kmb::Matrix2x2(mat);
}

kmb::Vector2D*
kmb::Matrix2x2::solve(const Vector2D& b) const
{
	double d = this->determinant();
	if( d != 0.0 ){
		return new kmb::Vector2D(
					(m[3]*b.x()-m[2]*b.y()) / d,
					(-m[1]*b.x()+m[0]*b.y()) / d );
	}else{
		return NULL;
	}
}

bool
kmb::Matrix2x2::solve(const Vector2D& b,Vector2D& x) const
{
	double d = this->determinant();
	if( d != 0.0 ){
		x.setCoordinate(0, (m[3]*b.x()-m[2]*b.y()) / d );
		x.setCoordinate(1, (-m[1]*b.x()+m[0]*b.y()) / d );
		return true;
	}else{
		return false;
	}
}

kmb::Matrix2x2*
kmb::Matrix2x2::getInverse(void) const
{
	kmb::Matrix2x2* inv = NULL;
	double d = this->determinant();
	if( d != 0.0 ){
		d = 1.0/d;
		inv = new kmb::Matrix2x2(d*m[3],-d*m[2],-d*m[1],d*m[0]);
	}
	return inv;
}
