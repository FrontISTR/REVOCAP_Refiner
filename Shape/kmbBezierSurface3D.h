/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BezierSurface3D                                         #
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
/**
 * Bezier ã»ñ 
 * m * n å¬ÇÃêßå‰ì_Çó^Ç¶ÇÈÇ∆ÅA(m-1) éü * (n-1) éüÇÃã»ñ Ç™ìæÇÁÇÍÇÈ
 * uOrder = n
 * uDegree = n-1
 *
 * íËã`àÊÇÕ 0 <= u,v <= 1 Ç∆Ç∑ÇÈ
 */
#pragma once

#include "Shape/kmbSurface3D.h"
#include <vector>
#include <string>
#include <fstream>

namespace kmb{

class BezierSurface3D : public Surface3D
{
private:

	unsigned int uOrder, vOrder;
	std::vector< kmb::Point3D > ctrlPts;
public:
	BezierSurface3D(void);
	virtual ~BezierSurface3D(void);
	void clear(void);
	virtual kmb::Surface3D::surfaceType getSurfaceType(void) const;
	virtual bool isDomain( double u, double v ) const;
	virtual bool isUDomain( double u ) const;
	virtual bool isVDomain( double v ) const;
	virtual void getDomain( double &min_u, double &max_u, double &min_v, double &max_v ) const;


	bool setOrder(unsigned int uOrder,unsigned int vOrder);
	bool getOrder(unsigned int &uOrder,unsigned int &vOrder) const;
	void getDegree(unsigned int &uDegree,unsigned int &vDegree) const;
	virtual bool getPoint( double u, double v, kmb::Point3D& point ) const;
	virtual bool getDerivative( kmb::Surface3D::derivativeType d, double u, double v, kmb::Vector3D& tangent ) const;
	void appendCtrlPt(double x,double y,double z);
	bool getCtrlPt(int i,kmb::Point3D &pt) const;
	virtual int writeRNF( std::ofstream &output, std::string indent="  " ) const;
};

}
