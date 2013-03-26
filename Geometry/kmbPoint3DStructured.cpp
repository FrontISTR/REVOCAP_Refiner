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

#include "Geometry/kmbPoint3DStructured.h"

#include "Common/kmbCommon.h"
#include "Geometry/kmbBoxRegion.h"

kmb::Point3DStructured::Point3DStructured(void)
	: divx(0), divy(0), divz(0)
{
}

kmb::Point3DStructured::Point3DStructured(int _divx,int _divy,int _divz)
	: divx(_divx), divy(_divy), divz(_divz)
{
}

kmb::Point3DStructured::~Point3DStructured(void)
{
}

void kmb::Point3DStructured::initialize(int _divx,int _divy,int _divz)
{
	divx = _divx;
	divy = _divy;
	divz = _divz;
}

int kmb::Point3DStructured::getNodeCount(void) const
{
	return (divx+1)*(divy+1)*(divz+1);
}

kmb::Point3DUniformStructured::Point3DUniformStructured(void)
	: kmb::Point3DStructured(), box(NULL)
{
}

kmb::Point3DUniformStructured::Point3DUniformStructured(double minx,double maxx,double miny,double maxy,double minz,double maxz,int _divx,int _divy,int _divz)

	: kmb::Point3DStructured(_divx,_divy,_divz), box(new BoxRegion(minx,miny,minz,maxx,maxy,maxz))
{

}

kmb::Point3DUniformStructured::~Point3DUniformStructured(void)
{
	if(box){
		delete box;
		box = NULL;
	}
}

void kmb::Point3DUniformStructured::initialize(double minx,double maxx,double miny,double maxy,double minz,double maxz,int _divx,int _divy,int _divz)
{
	if(box){
		delete box;
		box = NULL;
	}
	box = new BoxRegion(minx,miny,minz,maxx,maxy,maxz);
	divx = _divx;
	divy = _divy;
	divz = _divz;
}

bool kmb::Point3DUniformStructured::getPoint(int i,int j,int k,kmb::Point3D &pt) const
{
	if( box == NULL ||
		i < 0 || divx < i ||
		j < 0 || divy < j ||
		k < 0 || divz < k )
	{
		return false;
	}
	pt.setCoordinate(
		box->minX() + i * box->rangeX() / divx,
		box->minY() + j * box->rangeY() / divy,
		box->minZ() + k * box->rangeZ() / divz);
	return true;
}

kmb::Point3DRectLinearStructured::Point3DRectLinearStructured(int _divx,int _divy,int _divz)
	: kmb::Point3DStructured(_divx,_divy,_divz), xAxis(new double[_divx+1]), yAxis(new double[_divy+1]), zAxis(new double[_divz+1])
{
}

kmb::Point3DRectLinearStructured::~Point3DRectLinearStructured(void)
{
	if( xAxis ){
		delete[] xAxis;
		xAxis = NULL;
	}
	if( yAxis ){
		delete[] yAxis;
		yAxis = NULL;
	}
	if( zAxis ){
		delete[] zAxis;
		zAxis = NULL;
	}
}

void kmb::Point3DRectLinearStructured::setXAxis(int i,double x)
{
	xAxis[i] = x;
}

void kmb::Point3DRectLinearStructured::setYAxis(int j,double y)
{
	yAxis[j] = y;
}

void kmb::Point3DRectLinearStructured::setZAxis(int k,double z)
{
	zAxis[k] = z;
}

bool kmb::Point3DRectLinearStructured::getPoint(int i,int j,int k,kmb::Point3D &pt) const
{
	if( xAxis == NULL || i < 0 || divx < i ||
		yAxis == NULL || j < 0 || divy < j ||
		zAxis == NULL || k < 0 || divz < k )
	{
		return false;
	}
	pt.setCoordinate( xAxis[i], yAxis[j], zAxis[k] );
	return true;
}
