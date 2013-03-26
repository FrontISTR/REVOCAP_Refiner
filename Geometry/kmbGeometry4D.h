/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Geometry4D                                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "Geometry/kmbIdTypes.h"
#include "Matrix/kmbMatrix.h"

namespace kmb{

class Tuple4D
{
public:
	Tuple4D(void);
	Tuple4D(const double x, const double y, const double z,const double w);
	Tuple4D(const Tuple4D &other);
	virtual ~Tuple4D(void){};
	double x() const{ return v[0]; }
	double y() const{ return v[1]; }
	double z() const{ return v[2]; }
	double w() const{ return v[3]; }
	void x(const double x){ v[0] = x; }
	void y(const double y){ v[1] = y; }
	void z(const double z){ v[2] = z; }
	void w(const double w){ v[3] = w; }
	double getCoordinate(const int i) const{ return v[i]; }
	void setCoordinate(const int i,const double v){ this->v[i] = v; }
	void setCoordinate(const double x,const double y,const double z,const double w){
		this->v[0] = x;
		this->v[1] = y;
		this->v[2] = z;
		this->v[3] = w;
	}
	void set(const Tuple4D& other){
		this->v[0] = other.v[0];
		this->v[1] = other.v[1];
		this->v[2] = other.v[2];
		this->v[3] = other.v[3];
	}
	void addCoordinate(const double x,const double y,const double z,const double w);
	void scale(const double r);
	void zero(void);
	bool isZero(void) const{ return (v[0]==0.0 && v[1]==0.0 && v[2]==0.0 && v[3]==0.0); }
	double& operator[](const int i){ return v[i]; }
	double operator[](const int i) const { return v[i]; }
	Tuple4D& operator=(const Tuple4D& other);
protected:
	double v[4];
};

class Vector4D;

class Point4D : public Tuple4D
{
public:
	Point4D(void) : Tuple4D(){};
	Point4D(const double x, const double y, const double z, const double w)
		: Tuple4D(x,y,z,w){};
	Point4D(const Tuple4D &other)
		: Tuple4D(other){};
	Point4D operator+(const Vector4D& other) const;
	Point4D operator-(const Vector4D& other) const;
	Point4D& operator+=(const Vector4D& other);
	Point4D& operator-=(const Vector4D& other);
	virtual bool operator==(const Point4D& other) const;

	double distance(const Point4D& other) const;
	double distance(double x,double y,double z,double w) const;
	double distanceSq(const Point4D& other) const;
	double distanceSq(double x,double y,double z,double w) const;


#ifndef REVOCAP_SUPPORT_RUBY
	static double distance(const Point4D& a,const Point4D& b);
	static double distanceSq(const Point4D& a,const Point4D& b);
#endif
	static Point4D infinity;
};

class Vector4D : public Tuple4D
{
public:
	Vector4D(void) : Tuple4D(){};
	Vector4D(const double x, const double y, const double z,const double w) : Tuple4D(x,y,z,w){};
	Vector4D(const Tuple4D &other) : Tuple4D(other){};
	virtual ~Vector4D(void){};
	Vector4D operator+(const Vector4D& other) const{
		return Vector4D(v[0]+other.v[0],v[1]+other.v[1],v[2]+other.v[2],v[3]+other.v[3]);
	}
	Vector4D operator-(const Vector4D& other) const{
		return Vector4D(v[0]-other.v[0],v[1]-other.v[1],v[2]-other.v[2],v[3]-other.v[3]);
	}
	Vector4D& operator+=(const Vector4D& other){
		v[0] += other.v[0];
		v[1] += other.v[1];
		v[2] += other.v[2];
		v[3] += other.v[3];
		return *this;
	}
	Vector4D& operator-=(const Vector4D& other){
		v[0] -= other.v[0];
		v[1] -= other.v[1];
		v[2] -= other.v[2];
		v[3] -= other.v[3];
		return *this;
	}

	Vector4D scalar(const double s) const;
	Vector4D operator*(const double s) const{
		return Vector4D(s*v[0],s*v[1],s*v[2],s*v[3]);
	}
	friend Vector4D operator*(const double scalar,const Vector4D& vect){
		return vect * scalar;
	}

	double operator*(const Vector4D& other) const{
		return v[0]*other.v[0] + v[1]*other.v[1] + v[2]*other.v[2] + v[3]*other.v[3];
	}
	double lengthSq(void) const;
	double length(void) const;
	double abs(void) const;
	double normalize();
};

class Matrix3x3;
class Vector3D;
class Tuple3D;








class Matrix4x4 : public SquareMatrix
{
public:
	Matrix4x4(void);
	Matrix4x4(double m[16]);





	Matrix4x4(
		double m00,double m01,double m02,double m03,
		double m10,double m11,double m12,double m13,
		double m20,double m21,double m22,double m23,
		double m30,double m31,double m32,double m33);
	Matrix4x4(const Matrix4x4 &other);
	Matrix4x4(const Matrix3x3 &other);

	virtual ~Matrix4x4(void){};
	virtual const char* getContainerType(void) const{
		return "double_array_4x4";
	}
	int init(int rowSize, int colSize);
	int getSize(void) const;
	double get(int i,int j) const;
	bool set(int i,int j,double val);
	bool add(int i,int j,double val);

	bool identity(void);
	bool zero(void);

	bool transpose(void);
	void transpose(const Matrix4x4& other);

	double determinant(void) const;
	double trace(void) const;
	static double determinant(
		double m00,double m01,double m02,double m03,
		double m10,double m11,double m12,double m13,
		double m20,double m21,double m22,double m23,
		double m30,double m31,double m32,double m33);
	static double trace(
		double m00,double m01,double m02,double m03,
		double m10,double m11,double m12,double m13,
		double m20,double m21,double m22,double m23,
		double m30,double m31,double m32,double m33);

	Vector4D* solve(const Vector4D& b) const;
	bool solve(const Vector4D& b,Vector4D& x) const;
	bool solveSafely(const Vector4D& b,Vector4D& x,double thresh=1.0e-6) const;



	void convert(Tuple3D& tuple) const;
	void convert(Tuple4D& tuple) const;
	void convert(double &x, double &y, double &z) const;
	Vector3D operator*(const Vector3D& vect);
	Vector4D operator*(const Vector4D& vect);
	Matrix4x4 operator*(const Matrix4x4& other);

	Matrix4x4 operator+(const Matrix4x4& other);
	Matrix4x4 operator-(const Matrix4x4& other);

	Matrix4x4& operator=(const Matrix4x4& other);
	Matrix4x4& operator=(const Matrix3x3& other);


	bool isAffineType(void) const;

	static Matrix4x4 createRotation(const Vector3D& axis, double angle);
	static Matrix4x4* createLookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up);
	static Matrix4x4* createAffine(const Matrix3x3& transform, const Vector3D& displace);
	bool getLookAt(Vector3D& eye, Vector3D& center, Vector3D& up) const;


	void translate(double x,double y,double z);

	void scale(double r);
protected:
	double m[16];
};


}
