/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Matrix4x4                                               #
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
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbGeometry4D.h"
#include "Matrix/kmbMatrix_DoubleArray.h"

kmb::Matrix4x4::Matrix4x4(void)
: SquareMatrix(4)
{
	this->identity();
}

kmb::Matrix4x4::Matrix4x4(double m[16])
: SquareMatrix(4)
{
	for(int i=0;i<16;++i){
		this->m[i] = m[i];
	}
}

kmb::Matrix4x4::Matrix4x4(
		double m00,double m01,double m02,double m03,
		double m10,double m11,double m12,double m13,
		double m20,double m21,double m22,double m23,
		double m30,double m31,double m32,double m33)
: SquareMatrix(4)
{
	m[0] = m00;	m[4] = m01;	m[8] = m02;	m[12] = m03;
	m[1] = m10;	m[5] = m11;	m[9] = m12;	m[13] = m13;
	m[2] = m20;	m[6] = m21;	m[10]= m22;	m[14] = m23;
	m[3] = m30;	m[7] = m31;	m[11]= m32;	m[15] = m33;
}

kmb::Matrix4x4::Matrix4x4(const Matrix4x4 &other)
: SquareMatrix(4)
{
	*this = other;
}

kmb::Matrix4x4::Matrix4x4(const Matrix3x3 &other)
: SquareMatrix(4)
{
	*this = other;
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
kmb::Matrix4x4::init(int rowSize, int colSize)
{
	return -1;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

int
kmb::Matrix4x4::getSize(void) const
{
	return 4;
}

double
kmb::Matrix4x4::get(int i,int j) const
{
	return m[i+j*4];
}

bool
kmb::Matrix4x4::set(int i,int j,double val)
{
	m[i+j*4] = val;
	return true;
}

bool
kmb::Matrix4x4::add(int i,int j,double val)
{
	m[i+j*4] += val;
	return true;
}

kmb::Matrix4x4
kmb::Matrix4x4::operator*(const Matrix4x4& other)
{
	double mat[16];
	for(int i=0;i<4;++i){
		mat[0+i] =
			m[i]*other.m[0] + m[4+i]*other.m[1] + m[8+i]*other.m[2] + m[12+i]*other.m[3];
		mat[4+i] =
			m[i]*other.m[4] + m[4+i]*other.m[5] + m[8+i]*other.m[6] + m[12+i]*other.m[7];
		mat[8+i] =
			m[i]*other.m[8] + m[4+i]*other.m[9] + m[8+i]*other.m[10] + m[12+i]*other.m[11];
		mat[12+i] =
			m[i]*other.m[12] + m[4+i]*other.m[13] + m[8+i]*other.m[14] + m[12+i]*other.m[15];
	}
	return Matrix4x4(mat);
}

kmb::Vector3D
kmb::Matrix4x4::operator*(const Vector3D& vect)
{
	double x = m[0]*vect.x() + m[4]*vect.y() + m[8]*vect.z() + m[12];
	double y = m[1]*vect.x() + m[5]*vect.y() + m[9]*vect.z() + m[13];
	double z = m[2]*vect.x() + m[6]*vect.y() + m[10]*vect.z() + m[14];
	double w = m[3]*vect.x() + m[7]*vect.y() + m[11]*vect.z() + m[15];

	if(w==0.0){
		w = 1.0;
	}

	return kmb::Vector3D(x/w,y/w,z/w);
}

kmb::Vector4D
kmb::Matrix4x4::operator*(const Vector4D& vect)
{
	double x = m[0]*vect.x() + m[4]*vect.y() + m[8]*vect.z() + m[12]*vect.w();
	double y = m[1]*vect.x() + m[5]*vect.y() + m[9]*vect.z() + m[13]*vect.w();
	double z = m[2]*vect.x() + m[6]*vect.y() + m[10]*vect.z() + m[14]*vect.w();
	double w = m[3]*vect.x() + m[7]*vect.y() + m[11]*vect.z() + m[15]*vect.w();

	return kmb::Vector4D(x,y,z,w);
}

void
kmb::Matrix4x4::convert(double &x, double &y, double &z) const
{
	double x0 = m[0]*x + m[4]*y + m[8]*z + m[12];
	double y0 = m[1]*x + m[5]*y + m[9]*z + m[13];
	double z0 = m[2]*x + m[6]*y + m[10]*z + m[14];
	double w0 = m[3]*x + m[7]*y + m[11]*z + m[15];

	if(w0==0.0){
		w0 = 1.0;
	}else{
		w0 = 1.0/w0;
	}
	x = x0*w0;
	y = y0*w0;
	z = z0*w0;
}

void
kmb::Matrix4x4::convert(kmb::Tuple3D& tuple) const
{
	double x = m[0]*tuple.x() + m[4]*tuple.y() + m[8]*tuple.z() + m[12];
	double y = m[1]*tuple.x() + m[5]*tuple.y() + m[9]*tuple.z() + m[13];
	double z = m[2]*tuple.x() + m[6]*tuple.y() + m[10]*tuple.z() + m[14];
	double w = m[3]*tuple.x() + m[7]*tuple.y() + m[11]*tuple.z() + m[15];

	if(w==0.0){
		w = 1.0;
	}else{
		w = 1.0/w;
	}
	tuple.setCoordinate( x*w, y*w, z*w );
}

void
kmb::Matrix4x4::convert(kmb::Tuple4D& tuple) const
{
	double x = m[0]*tuple.x() + m[4]*tuple.y() + m[8]*tuple.z() + m[12]*tuple.w();
	double y = m[1]*tuple.x() + m[5]*tuple.y() + m[9]*tuple.z() + m[13]*tuple.w();
	double z = m[2]*tuple.x() + m[6]*tuple.y() + m[10]*tuple.z() + m[14]*tuple.w();
	double w = m[3]*tuple.x() + m[7]*tuple.y() + m[11]*tuple.z() + m[15]*tuple.w();

	tuple.setCoordinate( x, y, z, w );
}

kmb::Matrix4x4
kmb::Matrix4x4::operator+(const Matrix4x4& other)
{
	double mat[16];
	for(int i=0;i<16;++i){
		mat[i] = this->m[i] + other.m[i];
	}
	return Matrix4x4(mat);
}

kmb::Matrix4x4
kmb::Matrix4x4::operator-(const Matrix4x4& other)
{
	double mat[16];
	for(int i=0;i<16;++i){
		mat[i] = this->m[i] - other.m[i];
	}
	return Matrix4x4(mat);
}


kmb::Matrix4x4&
kmb::Matrix4x4::operator=(const Matrix4x4& other)
{
	for(int i=0;i<16;++i){
		this->m[i] = other.m[i];
	}
	return *this;
}

kmb::Matrix4x4&
kmb::Matrix4x4::operator=(const kmb::Matrix3x3& other)
{
	for(int i=0;i<3;++i){
		for(int j=0;j<3;++j){
			this->set(i,j,other.get(i,j));
		}
	}
	this->m[3] = 0.0;
	this->m[7] = 0.0;
	this->m[11] = 0.0;
	this->m[12] = 0.0;
	this->m[13] = 0.0;
	this->m[14] = 0.0;
	this->m[15] = 1.0;
	return *this;
}

bool
kmb::Matrix4x4::transpose()
{
	double tmp[16];
	for(int i=0;i<16;++i){
		tmp[i] = m[i];
	}
	for(int i=0;i<4;++i){
		m[i]	= tmp[4*i];
		m[4+i]	= tmp[4*i+1];
		m[8+i]	= tmp[4*i+2];
		m[12+i] = tmp[4*i+3];
	}
	return true;
}

void
kmb::Matrix4x4::transpose(const Matrix4x4& other)
{
	for(int i=0;i<4;++i){
		m[i]	= other.m[4*i];
		m[4+i]	= other.m[4*i+1];
		m[8+i]	= other.m[4*i+2];
		m[12+i] = other.m[4*i+3];
	}
}

bool
kmb::Matrix4x4::identity(void)
{
	m[0] = 1.0;	m[4] = 0.0;	m[8] = 0.0;	m[12] = 0.0;
	m[1] = 0.0;	m[5] = 1.0;	m[9] = 0.0;	m[13] = 0.0;
	m[2] = 0.0;	m[6] = 0.0;	m[10]= 1.0;	m[14] = 0.0;
	m[3] = 0.0;	m[7] = 0.0;	m[11]= 0.0;	m[15] = 1.0;
	return true;
}

bool
kmb::Matrix4x4::zero(void)
{
	for(int i=0;i<16;++i){
		this->m[i] = 0.0;
	}
	return true;
}

double
kmb::Matrix4x4::determinant(void) const
{

	double det = 0.0;
	if(m[3] != 0.0){
		kmb::Matrix3x3 m3 =
			kmb::Matrix3x3(
				m[4],m[8],m[12],
				m[5],m[9],m[13],
				m[6],m[10],m[14]);
		det -= m[3]*m3.determinant();
	}
	if(m[7] != 0.0){
		kmb::Matrix3x3 m7 =
			kmb::Matrix3x3(
				m[0],m[8],m[12],
				m[1],m[9],m[13],
				m[2],m[10],m[14]);
		det += m[7]*m7.determinant();
	}
	if(m[11] != 0.0){
		kmb::Matrix3x3 m11 =
			kmb::Matrix3x3(
				m[0],m[4],m[12],
				m[1],m[5],m[13],
				m[2],m[6],m[14]);
		det -= m[11]*m11.determinant();
	}
	if(m[15] != 0.0){
		kmb::Matrix3x3 m15 =
			kmb::Matrix3x3(
				m[0],m[4],m[8],
				m[1],m[5],m[9],
				m[2],m[6],m[10]);
		det += m[15]*m15.determinant();
	}
	return det;
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
kmb::Matrix4x4::trace(void) const
{
	return m[0] + m[5] + m[10] + m[15];
}

double
kmb::Matrix4x4::trace(
	double m00,double m01,double m02,double m03,
	double m10,double m11,double m12,double m13,
	double m20,double m21,double m22,double m23,
	double m30,double m31,double m32,double m33)
{
	return m00 + m11 + m22 + m33;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

double
kmb::Matrix4x4::determinant(
	double m00,double m01,double m02,double m03,
	double m10,double m11,double m12,double m13,
	double m20,double m21,double m22,double m23,
	double m30,double m31,double m32,double m33)
{

	double det = 0.0;
	if(m30 != 0.0){
		det -= m30 *
			kmb::Matrix3x3::determinant(
				m01,m02,m03,
				m11,m12,m13,
				m21,m22,m23);
	}
	if(m31 != 0.0){
		det += m31 *
			kmb::Matrix3x3::determinant(
				m00,m02,m03,
				m10,m12,m13,
				m20,m22,m23);
	}
	if(m32 != 0.0){
		det -= m32 *
			kmb::Matrix3x3::determinant(
				m00,m01,m03,
				m10,m11,m13,
				m20,m21,m23);
	}
	if(m33 != 0.0){
		det += m33 *
			kmb::Matrix3x3::determinant(
				m00,m01,m02,
				m10,m11,m12,
				m20,m21,m22);
	}
	return det;
}

kmb::Vector4D*
kmb::Matrix4x4::solve(const Vector4D& b) const
{
	kmb::Vector4D* answer = new kmb::Vector4D();
	if( solve(b,*answer) ){
		return answer;
	}else{
		delete answer;
		return NULL;
	}
}

bool
kmb::Matrix4x4::solve(const Vector4D& b,Vector4D& x) const
{
	double coef[20] = {
		m[0],m[1],m[2],m[3],
		m[4],m[5],m[6],m[7],
		m[8],m[9],m[10],m[11],
		m[12],m[13],m[14],m[15],
		b.x(),b.y(),b.z(),b.w()
	};

	kmb::Matrix_DoubleArray mat(4,5,coef);

	int ind = -1;
	double m = 0.0;

	for(int i=0;i<4;++i){
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
	mat.row_transf(0, 3, -mat.get(3,0));


	ind = -1;
	m = 0.0;
	for(int i=1;i<4;++i){
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
	mat.row_transf(1, 3, -mat.get(3,1));
	mat.row_transf(1, 0, -mat.get(0,1));


	ind = -1;
	m = 0.0;
	for(int i=2;i<4;++i){
		double d = fabs(mat.get(i,2));
		if( d > m ){
			ind = i;
			m = d;
		}
	}
	if( m == 0.0 ){
		return false;
	}else if(ind != 2){
		mat.row_exchange(2,ind);
	}
	mat.row_multi(2, 1.0/mat.get(2,2));
	mat.row_transf(2, 3, -mat.get(3,2));
	mat.row_transf(2, 0, -mat.get(0,2));
	mat.row_transf(2, 1, -mat.get(1,2));


	if( mat.get(3,3) == 0.0 ){
		return false;
	}
	mat.row_multi(3, 1.0/mat.get(3,3));
	mat.row_transf(3, 0, -mat.get(0,3));
	mat.row_transf(3, 1, -mat.get(1,3));
	mat.row_transf(3, 2, -mat.get(2,3));

	x.setCoordinate(0,mat.get(0,4));
	x.setCoordinate(1,mat.get(1,4));
	x.setCoordinate(2,mat.get(2,4));
	x.setCoordinate(3,mat.get(3,4));
	return true;
}

bool
kmb::Matrix4x4::solveSafely(const Vector4D& b,Vector4D& x,double thresh) const
{
	double coef[20] = {
		m[0],m[1],m[2],m[3],
		m[4],m[5],m[6],m[7],
		m[8],m[9],m[10],m[11],
		m[12],m[13],m[14],m[15],
		b.x(),b.y(),b.z(),b.w()
	};

	kmb::Matrix_DoubleArray mat(4,5,coef);

	int ind = -1;
	double m = 0.0;

	for(int i=0;i<4;++i){
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
	mat.row_transf(0, 3, -mat.get(3,0));


	ind = -1;
	m = 0.0;
	for(int i=1;i<4;++i){
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
	mat.row_transf(1, 3, -mat.get(3,1));
	mat.row_transf(1, 0, -mat.get(0,1));


	ind = -1;
	m = 0.0;
	for(int i=2;i<4;++i){
		double d = fabs(mat.get(i,2));
		if( d > m ){
			ind = i;
			m = d;
		}
	}
	if( m < thresh ){
		return false;
	}else if(ind != 1){
		mat.row_exchange(2,ind);
	}
	mat.row_multi(2, 1.0/mat.get(2,2));
	mat.row_transf(2, 3, -mat.get(3,2));
	mat.row_transf(2, 0, -mat.get(0,2));
	mat.row_transf(2, 1, -mat.get(1,2));


	if( fabs(mat.get(3,3)) < thresh ){
		return false;
	}
	mat.row_multi(3, 1.0/mat.get(3,3));
	mat.row_transf(3, 0, -mat.get(0,3));
	mat.row_transf(3, 1, -mat.get(1,3));
	mat.row_transf(3, 2, -mat.get(2,3));

	x.setCoordinate(0,mat.get(0,4));
	x.setCoordinate(1,mat.get(1,4));
	x.setCoordinate(2,mat.get(2,4));
	x.setCoordinate(3,mat.get(3,4));
	return true;
}

bool
kmb::Matrix4x4::isAffineType(void) const
{
	double thres = 1.0e-5;
	double a00 = this->m[0]*this->m[0] + this->m[1]*this->m[1] + this->m[2]*this->m[2];
	double a01 = this->m[0]*this->m[4] + this->m[1]*this->m[5] + this->m[2]*this->m[6];
	double a02 = this->m[0]*this->m[8] + this->m[1]*this->m[9] + this->m[2]*this->m[10];
	double a11 = this->m[4]*this->m[4] + this->m[5]*this->m[5] + this->m[6]*this->m[6];
	double a12 = this->m[4]*this->m[8] + this->m[5]*this->m[9] + this->m[6]*this->m[10];
	double a22 = this->m[8]*this->m[8] + this->m[9]*this->m[9] + this->m[10]*this->m[10];
	if( fabs(this->m[3]) < thres &&
		fabs(this->m[7]) < thres &&
		fabs(this->m[11]) < thres &&
		fabs(a00-1.0) < thres &&
		fabs(a11-1.0) < thres &&
		fabs(a22-1.0) < thres &&
		fabs(a01) < thres &&
		fabs(a02) < thres &&
		fabs(a12) < thres )
	{
		return true;
	}
	else
	{
		return false;
	}
}

kmb::Matrix4x4
kmb::Matrix4x4::createRotation(const Vector3D& axis, double angle)
{
	double m[16];
	double c = cos(angle);
	double s = sin(angle);
	double x = axis.x();
	double y = axis.y();
	double z = axis.z();
	m[0] = c+x*x*(1.0-c);	m[4] = x*y*(1.0-c)-z*s;	m[8] = x*z*(1.0-c)+y*s;
	m[1] = x*y*(1.0-c)+z*s;	m[5] = c+y*y*(1.0-c);	m[9] = y*z*(1.0-c)-x*s;
	m[2] = x*z*(1.0-c)-y*s;	m[6] = y*z*(1.0-c)+x*s;	m[10]= c+z*z*(1.0-c);

	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[3] = 0.0;	m[7] = 0.0;	m[11]= 0.0;	m[15] = 1.0;
	return kmb::Matrix4x4(m);
}

kmb::Matrix4x4*
kmb::Matrix4x4::createLookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up)
{
	kmb::Matrix4x4* lookat = NULL;
	Vector3D l = center - eye;
	l.normalize();
	Vector3D s = l % up;
	s.normalize();
	Vector3D u = s % l;
	lookat = new kmb::Matrix4x4(
		s.x(), u.x(), -l.x(), 0.0,
		s.y(), u.y(), -l.y(), 0.0,
		s.z(), u.z(), -l.z(), 0.0,
		-(eye*s), -(eye*u), (eye*l), 1.0 );
	return lookat;
}

kmb::Matrix4x4*
kmb::Matrix4x4::createAffine(const kmb::Matrix3x3& transform, const kmb::Vector3D& displace)
{
	kmb::Matrix4x4* mat = NULL;
	mat = new kmb::Matrix4x4(
		transform.get(0,0), transform.get(0,1), transform.get(0,2), displace[0],
		transform.get(1,0), transform.get(1,1), transform.get(1,2), displace[1],
		transform.get(2,0), transform.get(2,1), transform.get(2,2), displace[2],
		0.0, 0.0, 0.0, 1.0 );
	return mat;
}

bool
kmb::Matrix4x4::getLookAt(Vector3D& eye, Vector3D& center, Vector3D& up) const
{
	if( !isAffineType() ){
		return false;
	}
	eye.setCoordinate(
		-(m[12]*m[0] + m[13]*m[1] + m[14]*m[2]),
		-(m[12]*m[4] + m[13]*m[5] + m[14]*m[6]),
		-(m[12]*m[8] + m[13]*m[9] + m[14]*m[10]) );
	up.setCoordinate( m[1], m[5], m[9] );
	center.setCoordinate( eye.x()-m[0], eye.y()-m[1], eye.z()-m[2]);
	return true;
}

void
kmb::Matrix4x4::translate(double x,double y,double z)
{
	m[12] = x;
	m[13] = y;
	m[14] = z;
}

void
kmb::Matrix4x4::scale(double r)
{
	this->row_multi(0,r);
	this->row_multi(1,r);
	this->row_multi(2,r);
}
