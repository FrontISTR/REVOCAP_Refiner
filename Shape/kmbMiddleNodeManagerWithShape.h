/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FittingToSurface                                        #
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

#include "MeshGen/kmbMiddleNodeManager.h"
#include <vector>

namespace kmb{

template <typename T> class Vector2WithIntBindings;
template <typename T> class NodeMapperBindings;
class Surface3D;
class Point3D;
class MeshSmoother;

class MiddleNodeManagerWithShape : public kmb::MiddleNodeManager
{
protected:
	kmb::Vector2WithIntBindings<kmb::nodeIdType>* mappingToSurface;
	const std::vector< kmb::Surface3D* >* surfaces;
	// (u,v) 座標の中点ではなく、ユークリッド座標の中点の最近点を求める
	bool nearestFlag;
	int iterMax;
	double epsilon;
public:
	MiddleNodeManagerWithShape(void);
	virtual ~MiddleNodeManagerWithShape(void);
	void setMappingData(kmb::Vector2WithIntBindings<kmb::nodeIdType>* mapping);
	void setSurfaces(const std::vector< kmb::Surface3D* >* surfaces);
	void setNearestFlag(bool b);
	void setiterMax(int m);
	void setEpsilon(double e);
	// Refiner 用に Global 節点番号と局所節点番号を変換するときに使う
	// 節点番号の置き換え、戻り値は置き換えた個数
	size_t replaceNodeIds( const std::map<kmb::nodeIdType,kmb::nodeIdType> &nodeMapper );
	// 節点番号を与えて、適合対象の曲面の個数と、その曲面上の座標を返す
	// mappingToSurface に曲面が唯一与えられていれば、その曲面座標 (u,v) から空間座標を求める
	// 二つ以上与えられていればそれらの平均
	// 戻り値は曲面の個数 0 の時は座標は変化しない
	int getPointOnSurface(kmb::nodeIdType nodeId,kmb::Point3D &pt) const;
protected:
	// surface の上に乗る可能性のあるものを再定義
	virtual kmb::nodeIdType createMiddleNode(kmb::nodeIdType n0, kmb::nodeIdType n1);
	virtual kmb::nodeIdType createMiddleNode4(kmb::nodeIdType n0, kmb::nodeIdType n1,kmb::nodeIdType n2, kmb::nodeIdType n3);
};

}
