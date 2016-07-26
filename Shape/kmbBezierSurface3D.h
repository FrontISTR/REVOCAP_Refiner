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
 * Bezier 曲面
 * m * n 個の制御点を与えると、(m-1) 次 * (n-1) 次の曲面が得られる
 * uOrder = n
 * uDegree = n-1
 *
 * 定義域は 0 <= u,v <= 1 とする
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
	// uOrder * vOrder の制御点を与える
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
	// 登録済みの制御点の個数と適合するなら true を返す
	// そうでなければ false を返す。この場合は制御点の設定が必要
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
