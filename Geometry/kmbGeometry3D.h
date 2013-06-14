/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Geometry3D                                              #
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
#pragma once

#include "Geometry/kmbIdTypes.h"
#include "Matrix/kmbMatrix.h"

namespace kmb{

class Tuple3D
{
public:
	Tuple3D(void);
	Tuple3D(const double x, const double y, const double z);
	Tuple3D(const Tuple3D &other);
	virtual ~Tuple3D(void){};
	double x() const{ return v[0]; }
	double y() const{ return v[1]; }
	double z() const{ return v[2]; }
	void x(const double x){ v[0] = x; }
	void y(const double y){ v[1] = y; }
	void z(const double z){ v[2] = z; }
	double getCoordinate(const int i) const{ return v[i]; }
	void setCoordinate(const int i,const double v){ this->v[i] = v; }
	void setCoordinate(const double x,const double y,const double z){
		this->v[0] = x;
		this->v[1] = y;
		this->v[2] = z;
	}

	void set(const Tuple3D& other){
		this->v[0] = other.v[0];
		this->v[1] = other.v[1];
		this->v[2] = other.v[2];
	}
	void addCoordinate(const int i,const double v){ this->v[i] += v; }
	void addCoordinate(const double x,const double y,const double z);
	void scale(const double r);
	void zero(void);
	bool isZero(void) const{ return (v[0]==0.0 && v[1]==0.0 && v[2]==0.0); }
	double& operator[](const int i){ return v[i]; }
	double operator[](const int i) const { return v[i]; }
	Tuple3D& operator=(const Tuple3D& other);
protected:
	double v[3];
};

class Vector3D;

class Point3D : public Tuple3D
{
public:
	Point3D(void) : Tuple3D(){};
	Point3D(const double x, const double y, const double z) : Tuple3D(x,y,z){};
	Point3D(const Tuple3D &other);
	virtual ~Point3D(void);
	Point3D operator+(const Vector3D& other) const;
	Point3D operator-(const Vector3D& other) const;
	Point3D& operator+=(const Vector3D& other);
	Point3D& operator-=(const Vector3D& other);
	virtual bool operator==(const Point3D& other) const;
	Point3D& operator=(const Point3D& other);

	Point3D dividingPoint(const Point3D& other,double m,double n) const;


	Point3D proportionalPoint(const Point3D& other,double t) const;


	double distance(const Point3D& other) const;
	double distance(double x,double y,double z) const;
	double distanceSq(const Point3D& other) const;
	double distanceSq(double x,double y,double z) const;


	double distanceSqToSegment(const Point3D& a,const Point3D& b) const;
	double distanceSqToSegment(const Point3D& a,const Point3D& b,double& t) const;
	double distanceToSegment(const Point3D& a,const Point3D& b) const;

	double distanceSqToTriangle(const Point3D& a,const Point3D& b,const Point3D& c,double* t=NULL) const;
	double distanceToTriangle(const Point3D& a,const Point3D& b,const Point3D& c) const;


	bool getFootOfPerpendicular( const Point3D &origin, const Vector3D &u, const Vector3D &v, double &a, double &b) const;



#ifndef REVOCAP_SUPPORT_RUBY
	static double distance(const Point3D& a,const Point3D& b);
	static double distanceSq(const Point3D& a,const Point3D& b);
#endif

	static double volume(const Point3D& a,const Point3D& b,const Point3D &c,const Point3D &d);

	static bool positive(const Point3D& a,const Point3D& b,const Point3D &c,const Point3D &x);

	static double area(const Point3D& a,const Point3D& b,const Point3D &c);
	static Vector3D areaVector(const Point3D& a,const Point3D& b,const Point3D &c);

	static Point3D getCenter(const Point3D& a,const Point3D& b);
	static Point3D getCenter(const Point3D& a,const Point3D& b,const Point3D &c);


	static double angle(const Point3D &a,const Point3D &b,const Point3D &c);
	static double cos(const Point3D &a,const Point3D &b,const Point3D &c);
	static double sin(const Point3D &a,const Point3D &b,const Point3D &c);

	static void calcMinorCoordinate( const kmb::Point3D& a, const kmb::Point3D& b, const kmb::Point3D& c, const kmb::Point3D& d, const kmb::Point3D& x, double coordinate[4]);

	static Vector3D calcNormalVector( const kmb::Point3D& a, const kmb::Point3D& b, const kmb::Point3D& c );
	static Vector3D calcNormalVector(
		const double x0, const double y0, const double z0,
		const double x1, const double y1, const double z1,
		const double x2, const double y2, const double z2);
	static Vector3D calcNormalVector(
		const double x0, const double y0, const double z0,
		const double x1, const double y1, const double z1,
		const double x2, const double y2, const double z2,
		const double x3, const double y3, const double z3);
	static Point3D infinity;
};

class Vector3D : public Tuple3D
{
public:
	Vector3D(void) : Tuple3D(){};
	Vector3D(const double x, const double y, const double z)
		: Tuple3D(x,y,z){};


	Vector3D(const Point3D& p,const Point3D& q);
	Vector3D(const Tuple3D &other) : Tuple3D(other){};
	virtual ~Vector3D(void){};
	Vector3D operator+(const Vector3D& other) const{
		return Vector3D(v[0]+other.v[0],v[1]+other.v[1],v[2]+other.v[2]);
	}
	Vector3D operator-(const Vector3D& other) const{
		return Vector3D(v[0]-other.v[0],v[1]-other.v[1],v[2]-other.v[2]);
	}
	Vector3D& operator+=(const Vector3D& other){
		v[0] += other.v[0];
		v[1] += other.v[1];
		v[2] += other.v[2];
		return *this;
	}
	Vector3D& operator-=(const Vector3D& other){
		v[0] -= other.v[0];
		v[1] -= other.v[1];
		v[2] -= other.v[2];
		return *this;
	}

	Vector3D scalar(const double s) const;
	Vector3D operator*(const double s) const{
		return Vector3D(s*v[0],s*v[1],s*v[2]);
	}
	friend Vector3D operator*(const double scalar,const Vector3D& vect){
		return vect * scalar;
	}

	double operator*(const Vector3D& other) const{
		return v[0]*other.v[0] + v[1]*other.v[1] + v[2]*other.v[2];
	}

	Vector3D operator%(const Vector3D& other) const{
		return Vector3D(
			v[1]*other.v[2] - v[2]*other.v[1],
			v[2]*other.v[0] - v[0]*other.v[2],
			v[0]*other.v[1] - v[1]*other.v[0]);
	}
	double lengthSq(void) const;
	double length(void) const;
	double abs(void) const;
	double normalize();

	static double triple(const Vector3D &v0,const Vector3D &v1,const Vector3D &v2);
	static double cos(const Vector3D &v0,const Vector3D &v1);
	static double sin(const Vector3D &v0,const Vector3D &v1);
	static double angle(const Vector3D &v0,const Vector3D &v1);
	static double abs(const double v[3]);
	static double inner(const double v0[3],const double v1[3]);
};











class Tuple2D;
class Vector2D;

class Matrix3x3 : public SquareMatrix
{
public:
	Matrix3x3(void);

	Matrix3x3(double m[9]);
	Matrix3x3(
		double m00,double m01,double m02,
		double m10,double m11,double m12,
		double m20,double m21,double m22);
	virtual ~Matrix3x3(void){};
	virtual const char* getContainerType(void) const{
		return "double_array_3x3";
	}
	int init(int rowSize, int colSize);
	int getSize(void) const;
	double get(int i,int j) const;
	bool set(int i,int j,double val);
	bool add(int i,int j,double val);

	bool identity(void);
	bool zero(void);
	bool transpose(void);
	void transpose(const Matrix3x3& other);
	static Matrix3x3 createRotation(double angle,const char* axis="z");
	static Matrix3x3 createRotation(double angle,const Vector3D& axis);
	static Matrix3x3 createReflection(const Vector3D& axis);

	static Matrix3x3 createSchmidtRotation(const Vector3D v0,const Vector3D v1,bool column=true);
	double determinant(void) const;
	double trace(void) const;
	static double determinant(
		double m00,double m01,double m02,
		double m10,double m11,double m12,
		double m20,double m21,double m22);
	static double trace(
		double m00,double m01,double m02,
		double m10,double m11,double m12,
		double m20,double m21,double m22);

	Matrix3x3* getInverse(void) const;

	Vector3D* solve(const Vector3D& b) const;
	bool solve(const Vector3D& b,Vector3D& x) const;
	bool solveSafely(const Vector3D& b,Vector3D& x,double thresh=1.0e-6) const;


	void convert(Tuple2D& tuple) const;
	void convert(Tuple3D& tuple) const;
	Vector2D operator*(const Vector2D& vect);
	Vector3D operator*(const Vector3D& vect);
	Matrix3x3 operator*(const Matrix3x3& other);

	Matrix3x3 operator+(const Matrix3x3& other);
	Matrix3x3 operator-(const Matrix3x3& other);
protected:
	double m[9];
};

}
