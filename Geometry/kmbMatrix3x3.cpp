/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matrix3x3                                               #
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
#include <cfloat>
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbIdTypes.h"
#include "Matrix/kmbMatrix_DoubleArray.h"

kmb::Matrix3x3::Matrix3x3(void)
: SquareMatrix(3)
{
	this->identity();
}

kmb::Matrix3x3::Matrix3x3(double m[9])
: SquareMatrix(3)
{
	for(int i=0;i<9;++i){
		this->m[i] = m[i];
	}
}

kmb::Matrix3x3::Matrix3x3(
		double m00,double m01,double m02,
		double m10,double m11,double m12,
		double m20,double m21,double m22)
: SquareMatrix(3)
{
	m[0] = m00;	m[3] = m01;	m[6] = m02;
	m[1] = m10;	m[4] = m11;	m[7] = m12;
	m[2] = m20;	m[5] = m21;	m[8] = m22;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

int
kmb::Matrix3x3::init(int rowSize, int colSize)
{
	return -1;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

int
kmb::Matrix3x3::getSize(void) const
{
	return 3;
}

double
kmb::Matrix3x3::get(int i,int j) const
{
	return m[i+j*3];
}

bool
kmb::Matrix3x3::set(int i,int j,double val)
{
	m[i+j*3] = val;
	return true;
}

bool
kmb::Matrix3x3::add(int i,int j,double val)
{
	m[i+j*3] += val;
	return true;
}

kmb::Matrix3x3
kmb::Matrix3x3::operator*(const Matrix3x3& other)
{
	double mat[9];
	for(int i=0;i<3;++i){
		mat[0+i] =
			m[i]*other.m[0] + m[3+i]*other.m[1] + m[6+i]*other.m[2];
		mat[3+i] =
			m[i]*other.m[3] + m[3+i]*other.m[4] + m[6+i]*other.m[5];
		mat[6+i] =
			m[i]*other.m[6] + m[3+i]*other.m[7] + m[6+i]*other.m[8];
	}
	return Matrix3x3(mat);
}

kmb::Vector2D
kmb::Matrix3x3::operator*(const Vector2D& vect)
{
	double x = m[0]*vect.x() + m[3]*vect.y() + m[6];
	double y = m[1]*vect.x() + m[4]*vect.y() + m[7];
	double w = m[2]*vect.x() + m[5]*vect.y() + m[8];

	if(w==0.0){
		w = 1.0;
	}else{
		w = 1.0/w;
	}

	return kmb::Vector2D(x*w,y*w);
}

void
kmb::Matrix3x3::convert(kmb::Tuple2D& tuple) const
{
	double x = m[0]*tuple.x() + m[3]*tuple.y() + m[6];
	double y = m[1]*tuple.x() + m[4]*tuple.y() + m[7];
	double w = m[2]*tuple.x() + m[5]*tuple.y() + m[8];

	if(w==0.0){
		w = 1.0;
	}else{
		w = 1.0/w;
	}
	tuple.setCoordinate( x*w, y*w );
}

void
kmb::Matrix3x3::convert(kmb::Tuple3D& tuple) const
{
	double x = m[0]*tuple.x() + m[3]*tuple.y() + m[6]*tuple.z();
	double y = m[1]*tuple.x() + m[4]*tuple.y() + m[7]*tuple.z();
	double z = m[2]*tuple.x() + m[5]*tuple.y() + m[8]*tuple.z();

	tuple.setCoordinate( x, y, z);
}

kmb::Vector3D
kmb::Matrix3x3::operator*(const Vector3D& vect)
{
	double x = m[0]*vect.x() + m[3]*vect.y() + m[6]*vect.z();
	double y = m[1]*vect.x() + m[4]*vect.y() + m[7]*vect.z();
	double z = m[2]*vect.x() + m[5]*vect.y() + m[8]*vect.z();

	return kmb::Vector3D(x,y,z);
}

kmb::Matrix3x3
kmb::Matrix3x3::operator+(const Matrix3x3& other)
{
	double mat[9];
	for(int i=0;i<9;++i){
		mat[i] = this->m[i] + other.m[i];
	}
	return Matrix3x3(mat);
}

kmb::Matrix3x3
kmb::Matrix3x3::operator-(const Matrix3x3& other)
{
	double mat[9];
	for(int i=0;i<9;++i){
		mat[i] = this->m[i] - other.m[i];
	}
	return Matrix3x3(mat);
}

bool
kmb::Matrix3x3::transpose(void)
{
	double tmp[9];
	for(int i=0;i<9;++i){
		tmp[i] = m[i];
	}
	for(int i=0;i<3;++i){
		m[i]	= tmp[3*i];
		m[3+i]	= tmp[3*i+1];
		m[6+i]	= tmp[3*i+2];
	}
	return true;
}

void
kmb::Matrix3x3::transpose(const Matrix3x3& other)
{
	for(int i=0;i<3;++i){
		m[i]	= other.m[3*i];
		m[3+i]	= other.m[3*i+1];
		m[6+i]	= other.m[3*i+2];
	}
}

bool
kmb::Matrix3x3::identity(void)
{
	m[0] = 1.0;	m[3] = 0.0;	m[6] = 0.0;
	m[1] = 0.0;	m[4] = 1.0;	m[7] = 0.0;
	m[2] = 0.0;	m[5] = 0.0;	m[8] = 1.0;
	return true;
}

bool
kmb::Matrix3x3::zero(void)
{
	for(int i=0;i<9;++i){
		this->m[i] = 0.0;
	}
	return true;
}

double
kmb::Matrix3x3::determinant(void) const
{
	return m[0]*m[4]*m[8] + m[2]*m[3]*m[7] + m[1]*m[5]*m[6]
		- m[0]*m[5]*m[7] - m[1]*m[3]*m[8] - m[2]*m[4]*m[6];
}

double
kmb::Matrix3x3::trace(void) const
{
	return m[0] + m[4] + m[8];
}

double
kmb::Matrix3x3::determinant(
	double m00,double m01,double m02,
	double m10,double m11,double m12,
	double m20,double m21,double m22)
{
	return m00*m11*m22 + m20*m01*m12 + m10*m21*m02
		- m00*m21*m12 - m10*m01*m22 - m20*m11*m02;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

double
kmb::Matrix3x3::trace(
	double m00,double m01,double m02,
	double m10,double m11,double m12,
	double m20,double m21,double m22)
{
	return m00 + m11 + m22;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

kmb::Matrix3x3
kmb::Matrix3x3::createRotation(double angle,const char* axis)
{
	double m[9];
	double c = cos(angle);
	double s = sin(angle);
	if( 'x' == axis[0] ){
		m[0] = 1.0;	m[3] = 0.0;	m[6] = 0.0;
		m[1] = 0.0;	m[4] = c;	m[7] = -s;
		m[2] = 0.0;	m[5] = s;	m[8] = c;
	}else if( 'y' == axis[0] ){
		m[0] = c;	m[3] = 0.0;	m[6] = s;
		m[1] = 0.0;	m[4] = 1.0;	m[7] = 0.0;
		m[2] = -s;	m[5] = 0.0;	m[8] = c;
	}else if( 'z' == axis[0] ){
		m[0] = c;	m[3] = -s;	m[6] = 0.0;
		m[1] = s;	m[4] = c;	m[7] = 0.0;
		m[2] = 0.0;	m[5] = 0.0;	m[8] = 1.0;
	}else{
		m[0] = 1.0;	m[3] = 0.0;	m[6] = 0.0;
		m[1] = 0.0;	m[4] = 1.0;	m[7] = 0.0;
		m[2] = 0.0;	m[5] = 0.0;	m[8] = 1.0;
	}
	return kmb::Matrix3x3(m);
}

kmb::Matrix3x3
kmb::Matrix3x3::createRotation(double angle,const Vector3D& axis)
{
	double m[9];
	double c = cos(angle);
	double s = sin(angle);
	double x = axis.x();
	double y = axis.y();
	double z = axis.z();
	m[0] = c+x*x*(1.0-c);	m[3] = x*y*(1.0-c)-z*s;	m[6] = x*z*(1.0-c)+y*s;
	m[1] = x*y*(1.0-c)+z*s;	m[4] = c+y*y*(1.0-c);	m[7] = y*z*(1.0-c)-x*s;
	m[2] = x*z*(1.0-c)-y*s;	m[5] = y*z*(1.0-c)+x*s;	m[8] = c+z*z*(1.0-c);

	return kmb::Matrix3x3(m);
}

kmb::Matrix3x3
kmb::Matrix3x3::createReflection(const kmb::Vector3D& axis)
{
	double m[9] =
		{1.0, 0.0, 0.0,
		 0.0, 1.0, 0.0,
		 0.0, 0.0, 1.0};
	kmb::Vector3D v = axis;
	v.normalize();
	for(int i=0;i<3;++i){
		for(int j=0;j<3;++j){
			m[i*3+j] -= 2.0 * v.getCoordinate(i) * v.getCoordinate(j);
		}
	}
	return kmb::Matrix3x3(m);
}

kmb::Matrix3x3
kmb::Matrix3x3::createSchmidtRotation(const Vector3D v0,const Vector3D v1,bool column)
{
	kmb::Vector3D u0 = v0;
	u0.normalize();
	kmb::Vector3D u1 = v1 - (v1*u0) * u0;
	u1.normalize();
	kmb::Vector3D u2 = u0 % u1;
	if( column ){
		return kmb::Matrix3x3(
			u0.x(), u1.x(), u2.x(),
			u0.y(), u1.y(), u2.y(),
			u0.z(), u1.z(), u2.z()
			);
	}else{
		return kmb::Matrix3x3(
			u0.x(), u0.y(), u0.z(),
			u1.x(), u1.y(), u1.z(),
			u2.x(), u2.y(), u2.z()
			);
	}
}

kmb::Vector3D*
kmb::Matrix3x3::solve(const Vector3D& b) const
{
	kmb::Vector3D* answer = new kmb::Vector3D();
	if( solve(b,*answer) ){
		return answer;
	}else{
		delete answer;
		return NULL;
	}
}

bool
kmb::Matrix3x3::solve(const Vector3D& b,Vector3D& x) const
{

	double coef[12] = {
		m[0],m[1],m[2],
		m[3],m[4],m[5],
		m[6],m[7],m[8],
		b.x(),b.y(),b.z()
	};

	kmb::Matrix_DoubleArray mat(3,4,coef);

	int ind = -1;
	double m = 0.0;

	for(int i=0;i<3;++i){
		double d = fabs(mat.get(i,0));
		if( d > m ){
			ind = i;
			m = d;
		}
	}
	if( m == 0.0 ){
		return false;
	}else if(ind != 0){
		mat.row_exchange(0,ind);
	}
	mat.row_multi(0, 1.0/mat.get(0,0));
	mat.row_transf(0, 1, -mat.get(1,0));
	mat.row_transf(0, 2, -mat.get(2,0));


	ind = -1;
	m = 0.0;
	for(int i=1;i<3;++i){
		double d = fabs(mat.get(i,1));
		if( d > m ){
			ind = i;
			m = d;
		}
	}
	if( m == 0.0 ){
		return false;
	}else if(ind != 1){
		mat.row_exchange(1,ind);
	}
	mat.row_multi(1, 1.0/mat.get(1,1));
	mat.row_transf(1, 2, -mat.get(2,1));
	mat.row_transf(1, 0, -mat.get(0,1));


	if( mat.get(2,2) == 0.0 ){
		return false;
	}
	mat.row_multi(2, 1.0/mat.get(2,2));
	mat.row_transf(2, 0, -mat.get(0,2));
	mat.row_transf(2, 1, -mat.get(1,2));

	x.setCoordinate(0,mat.get(0,3));
	x.setCoordinate(1,mat.get(1,3));
	x.setCoordinate(2,mat.get(2,3));

	return true;
}

bool
kmb::Matrix3x3::solveSafely(const Vector3D& b,Vector3D& x,double thresh) const
{

	double coef[12] = {
		m[0],m[1],m[2],
		m[3],m[4],m[5],
		m[6],m[7],m[8],
		b.x(),b.y(),b.z()
	};

	kmb::Matrix_DoubleArray mat(3,4,coef);

	int ind = -1;
	double m = 0.0;

	for(int i=0;i<3;++i){
		double d = fabs(mat.get(i,0));
		if( d > m ){
			ind = i;
			m = d;
		}
	}
	if( m < thresh ){
		return false;
	}else if(ind != 0){
		mat.row_exchange(0,ind);
	}
	mat.row_multi(0, 1.0/mat.get(0,0));
	mat.row_transf(0, 1, -mat.get(1,0));
	mat.row_transf(0, 2, -mat.get(2,0));


	ind = -1;
	m = 0.0;
	for(int i=1;i<3;++i){
		double d = fabs(mat.get(i,1));
		if( d > m ){
			ind = i;
			m = d;
		}
	}
	if( m < thresh ){
		return false;
	}else if(ind != 1){
		mat.row_exchange(1,ind);
	}
	mat.row_multi(1, 1.0/mat.get(1,1));
	mat.row_transf(1, 2, -mat.get(2,1));
	mat.row_transf(1, 0, -mat.get(0,1));


	if( fabs(mat.get(2,2)) < thresh ){
		return false;
	}
	mat.row_multi(2, 1.0/mat.get(2,2));
	mat.row_transf(2, 0, -mat.get(0,2));
	mat.row_transf(2, 1, -mat.get(1,2));

	x.setCoordinate(0,mat.get(0,3));
	x.setCoordinate(1,mat.get(1,3));
	x.setCoordinate(2,mat.get(2,3));

	return true;
}

kmb::Matrix3x3*
kmb::Matrix3x3::getInverse(void) const
{
	kmb::Matrix3x3* inv = NULL;
	double d = this->determinant();
	if( d != 0.0 ){
		d = 1.0/d;
		inv = new kmb::Matrix3x3(
			d*(m[4]*m[8]-m[7]*m[5]),
			d*(m[5]*m[6]-m[8]*m[3]),
			d*(m[3]*m[7]-m[4]*m[6]),

			d*(m[7]*m[2]-m[1]*m[8]),
			d*(m[0]*m[8]-m[2]*m[6]),
			d*(m[6]*m[1]-m[7]*m[0]),

			d*(m[1]*m[5]-m[4]*m[2]),
			d*(m[2]*m[3]-m[0]*m[5]),
			d*(m[0]*m[4]-m[3]*m[1]) );
	}
	return inv;
}
