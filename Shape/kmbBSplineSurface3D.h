/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BSplineSurface3D                                        #
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

#include "Shape/kmbSurface3D.h"
#include "Shape/kmbBSpline.h"
#include <vector>
#include <string>
#include <fstream>

namespace kmb{

class BSplineSurface3D : public Surface3D
{
private:

	unsigned int uOrder, vOrder;
	kmb::BSpline uBspline, vBspline;
	std::vector< kmb::Point3D > ctrlPts;
public:
	BSplineSurface3D(void);
	virtual ~BSplineSurface3D(void);
	void clear(void);
	virtual kmb::Surface3D::surfaceType getSurfaceType(void) const;
	virtual void getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const;
	virtual bool isDomain( double u, double v ) const;
	virtual bool isUDomain( double u ) const;
	virtual bool isVDomain( double v ) const;


	bool setOrder(unsigned int uOrder,unsigned int vOrder);
	bool getOrder(unsigned int &uOrder,unsigned int &vOrder) const;
	bool getKnotCount(unsigned int &uCount,unsigned int &vCount) const;
	void getDegree(unsigned int &uDegree,unsigned int &vDegree) const;
	int getCtrlPtCount(void) const;
	void appendUKnot(double k);
	void appendVKnot(double k);
	void appendCtrlPt(double x,double y,double z);
	bool getCtrlPt(int i,kmb::Point3D &pt) const;
	double getUKnot(int i) const;
	double getVKnot(int i) const;
	bool valid(void) const;
	virtual bool getPoint( double u, double v, kmb::Point3D& point ) const;
	virtual bool getDerivative( kmb::Surface3D::derivativeType d, double u, double v, kmb::Vector3D& tangent ) const;
	virtual int writeRNF( std::ofstream &output, std::string indent="  " ) const;
};

}
