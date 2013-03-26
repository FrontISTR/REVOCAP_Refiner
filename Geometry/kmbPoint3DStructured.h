/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DStructured                                       #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Copyright (C) 2012 Advancesoft corp.                            #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

namespace kmb{

class BoxRegion;
class Point3D;

class Point3DStructured
{
public:
	int divx,divy,divz;
	Point3DStructured(void);
	Point3DStructured(int _divx,int _divy,int _divz);
	virtual ~Point3DStructured(void);
	void initialize(int _divx,int _divy,int _divz);
	virtual bool getPoint(int i,int j,int k,kmb::Point3D &pt) const = 0;
	int getNodeCount(void) const;
};

class Point3DUniformStructured : public Point3DStructured
{
protected:
	kmb::BoxRegion* box;
public:
	Point3DUniformStructured(void);
	Point3DUniformStructured(double minx,double maxx,double miny,double maxy,double minz,double maxz,int _divx,int _divy,int _divz);
	virtual ~Point3DUniformStructured(void);
	void initialize(double minx,double maxx,double miny,double maxy,double minz,double maxz,int _divx,int _divy,int _divz);
	bool getPoint(int i,int j,int k,kmb::Point3D &pt) const;
};

class Point3DRectLinearStructured : public Point3DStructured
{
protected:
	double* xAxis;
	double* yAxis;
	double* zAxis;
public:
	Point3DRectLinearStructured(int _divx,int _divy,int _divz);
	virtual ~Point3DRectLinearStructured(void);
	void setXAxis(int i,double x);
	void setYAxis(int j,double y);
	void setZAxis(int k,double z);
	bool getPoint(int i,int j,int k,kmb::Point3D &pt) const;
};

}
