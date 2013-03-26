/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Geometry2D                                              #
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

#include "Matrix/kmbMatrix.h"

namespace kmb{

class Tuple2D
{
public:
	Tuple2D(void);
	Tuple2D(const double x, const double y);
	Tuple2D(const Tuple2D &other);
	virtual ~Tuple2D(void);
	double x() const{ return v[0]; }
	double y() const{ return v[1]; }
	void x(double x){ v[0] = x; }
	void y(double y){ v[1] = y; }
	double getCoordinate(int i) const { return v[i]; }
	void setCoordinate(int i,double v){ this->v[i] = v; }
	void setCoordinate(double x,double y){
		this->v[0] = x;
		this->v[1] = y;
	}
	void set(const Tuple2D& other){
		this->v[0] = other.v[0];
		this->v[1] = other.v[1];
	}
	void addCoordinate(const double x,const double y);
	void scale(const double r);
	void zero(void);
	bool isZero(void) const{ return (v[0]==0.0 && v[1]==0.0); }
	double& operator[](const int i){ return v[i]; }
	double operator[](const int i) const { return v[i]; }
	Tuple2D& operator=(const Tuple2D& other);
protected:
	double v[2];
};

class Vector2D;

class Point2D : public Tuple2D
{
public:
	Point2D(void) : Tuple2D(){};
	Point2D(const double x, const double y)
		: Tuple2D(x,y){};
	Point2D(const Tuple2D &other)
		: Tuple2D(other){};
	Point2D operator+(const Vector2D& other);
	Point2D operator-(const Vector2D& other);
	Point2D& operator+=(const Vector2D& other);
	Point2D& operator-=(const Vector2D& other);
	bool operator==(const Point2D& other) const;
	double distance(const Point2D& other) const;
	double distance(double x,double y) const;
	double distanceSq(const Point2D& other) const;
	double distanceSq(double x,double y) const;

	double distanceSqToSegment(const Point2D& a,const Point2D& b) const;


	double distanceSqToSegment(const Point2D& a,const Point2D& b,double& t) const;
	double distanceToSegment(const Point2D& a,const Point2D& b) const;

	Point2D dividingPoint(const Point2D& other,double m,double n) const;



#ifndef REVOCAP_SUPPORT_RUBY
	static double distance(const Point2D& a,const Point2D& b);
	static double distanceSq(const Point2D& a,const Point2D& b);
#endif


	static double area(const Point2D& a,const Point2D& b,const Point2D &c);
	static Point2D getCenter(const Point2D& a,const Point2D& b);
	static Point2D getCenter(const Point2D& a,const Point2D& b,const Point2D &c);


	static double angle(const Point2D &a,const Point2D &b,const Point2D &c);

	static double angle2(const Point2D &a,const Point2D &b,const Point2D &c);
	static double cos(const Point2D &a,const Point2D &b,const Point2D &c);
	static double sin(const Point2D &a,const Point2D &b,const Point2D &c);

	static void calcMinorCoordinate( const kmb::Point2D& a, const kmb::Point2D& b, const kmb::Point2D& c, const kmb::Point2D& x, double coordinate[3]);
	static Point2D infinity;

	static bool intesectSegments(const kmb::Point2D& a0, const kmb::Point2D& a1, const kmb::Point2D& b0, const kmb::Point2D& b1);
};

class Vector2D : public Tuple2D
{
public:
	Vector2D(void) : Tuple2D(){};
	Vector2D(const double x, const double y)
		: Tuple2D(x,y){};


	Vector2D(const Point2D& p,const Point2D& q);
	Vector2D(const Tuple2D &other)
		: Tuple2D(other){};
	virtual ~Vector2D(void){};
	Vector2D operator+(const Vector2D& other){
		return Vector2D(v[0]+other.v[0],v[1]+other.v[1]);
	}
	Vector2D operator-(const Vector2D& other){
		return Vector2D(v[0]-other.v[0],v[1]-other.v[1]);
	}

	Vector2D scalar(const double s) const;
	Vector2D operator*(const double s) const{
		return Vector2D(s*v[0],s*v[1]);
	}
	friend Vector2D operator*(const double scalar,const Vector2D& vect){
		return vect * scalar;
	}

	double operator*(const Vector2D& other) const{
		return v[0]*other.v[0] + v[1]*other.v[1];
	}

	double operator%(const Vector2D& other) const{
		return v[0]*other.v[1] - v[1]*other.v[0];
	}
	double lengthSq() const;
	double length() const;
	double abs(void) const;
	double normalize();
	void rotate(double angle);

	static double cos(const Vector2D &v0,const Vector2D &v1);
	static double sin(const Vector2D &v0,const Vector2D &v1);
	static double angle(const Vector2D &v0,const Vector2D &v1);
	static double angle2(const Vector2D &v0,const Vector2D &v1);
	static double abs(const double v[3]);
};















class Matrix2x2 : public SquareMatrix
{
public:
	Matrix2x2(void);
	Matrix2x2(double m[4]);
	Matrix2x2(
		double m00,double m01,
		double m10,double m11);
	virtual ~Matrix2x2(void){};
	virtual const char* getContainerType(void) const{
		return "double_array_2x2";
	}
	int init(int rowSize, int colSize);
	int getSize(void) const;
	double get(int i,int j) const;
	bool set(int i,int j,double val);
	bool add(int i,int j,double val);

	bool identity(void);
	bool zero(void);
	bool transpose(void);
	void transpose(const Matrix2x2& other);
	static Matrix2x2 createRotation(double angle);

	static Matrix2x2 createSchmidtRotation(const Vector2D v0,bool column=true);
	double determinant(void) const;
	double trace(void) const;
	static double determinant(
		double m00,double m01,
		double m10,double m11);
	static double trace(
		double m00,double m01,
		double m10,double m11);

	Matrix2x2* getInverse(void) const;

	Vector2D* solve(const Vector2D& b) const;
	bool solve(const Vector2D& b,Vector2D& x) const;
	bool solveSafely(const Vector2D& b,Vector2D& x,double thresh=1.0e-6) const;


	Vector2D operator*(const Vector2D& vect);
	Matrix2x2 operator*(const Matrix2x2& other);

	Matrix2x2 operator+(const Matrix2x2& other);
	Matrix2x2 operator-(const Matrix2x2& other);
protected:
	double m[4];
};

}
