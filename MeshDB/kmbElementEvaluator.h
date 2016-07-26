/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementEvaluator                                        #
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
//
// 要素の品質評価のための関数群
// http://www.jama.or.jp/cgi-bin/pdq/download_pdq.cgi などを参考にする
//
// 局所的に評価できるもののみを対象にする
// 大域的に評価すべきものについては扱わない
//

#pragma once

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbElementContainer.h"
#include "Geometry/kmbGeometry3D.h"

namespace kmb{

class Point3DContainer;
class Point2DContainer;
class BoundingBox;
class SquareMatrix;

class ElementEvaluator
{
public:
	ElementEvaluator(const kmb::Point3DContainer* points);
	ElementEvaluator(const kmb::Point2DContainer* point2Ds);
	virtual ~ElementEvaluator(void);
	// 外接球の半径と内接球の半径の比
	double getAspectRatio(const kmb::ElementBase &eIter) const;
	double getAspectRatio(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;
	// 内接球の半径
	double getInscribedRadius(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;
	// 外接球の半径
	double getCircumRadius(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;
	// 最長辺の長さと最短辺の長さの比
	double getEdgeLengthRatio(const kmb::ElementBase &eIter) const;
	double getMaxEdgeLength(const kmb::ElementBase &eIter) const;
	double getAverageEdgeLength(const kmb::ElementBase &eIter) const;
	// 三角形の最小角度（面のうち三角形だけを調べる）
	double getMinAngleTri(const kmb::ElementBase &eIter) const;
	// 最小角度
	double getMinAngle(const kmb::ElementBase &eIter) const;
	// 最大角度
	double getMaxAngle(const kmb::ElementBase &eIter) const;
	// 体積
	double getVolume(const kmb::ElementBase &eIter) const;
	double getVolume(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;
	// 面積
	double getArea(const kmb::ElementBase &eIter) const;
	double getArea(const kmb::ElementBase &eIter,kmb::idType localFaceId) const;
	// 長さ
	double getLength(const kmb::ElementBase &eIter) const;
	// 角度
	double getAngle(const kmb::ElementBase &eIter,kmb::nodeIdType nodeId) const;
	double getAngleByIndex(const kmb::ElementBase &eIter,int index) const;
	double getCosByIndex(const kmb::ElementBase &eIter,int index) const;
	// n0 の尖り具合
	// n0->n3, n1->n3, n2->n3 を正規化してから三重積を取る
	double getSharpness(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;
	// Bounding Box
	bool getBoundingBox(const kmb::ElementBase &element,kmb::BoundingBox &bbox) const;
	double getBoundingBoxRadius(const kmb::ElementBase &element) const;
	// double getAngleBetweenFaces(const kmb::ElementBase &eIter,int index,int index) const;
	// 法線ベクトル
	bool getNormalVector(const kmb::ElementBase &element, kmb::Vector3D &vect) const;
	bool getNormalVector(const Face &face, const kmb::ElementContainer* elements, kmb::Vector3D &vect) const;
	bool getNormalVectorOfFace(const kmb::ElementBase &element, int index, kmb::Vector3D &vect) const;
	// 要素内自然座標系の取得
	bool getNaturalCoordinates(const kmb::ElementBase &element,const double x,const double y,const double z,double* retvals) const;
	bool getNaturalCoordinatesOfFace(const kmb::ElementBase &element,int index,const double x,const double y,const double z,double* retvals) const;
	double checkShapeFunctionDomain(const kmb::ElementBase &element,double s,double t,double u);
	// 要素の形状関数で評価したときの (x,y,z) の係数
	// (x,y,z) = \sum_i weights[i] * element[i].(x,y,z)
	// 戻り値は checkShapeFunctionDomain と同じ
	double getWeightElement(const kmb::ElementBase &element,const double x,const double y,const double z,double* weights) const;
	double getWeightElementFace(const kmb::ElementBase &element,int index,const double x,const double y,const double z,double* weights) const;
	// 要素内自然座標系から物理座標の取得
	bool getPhysicalCoordinates(const kmb::ElementBase &element,const double s,const double t,const double u,kmb::Point3D &target) const;
	// 要素の面と1点 (x,y,z) からなる４面体の体積の最小値
	// この値が正ならば、点 (x,y,z) は要素の内部にあるとしてよい
	double getMinInnerVolume(const kmb::ElementBase &element,const double x,const double y,const double z) const;
	// 要素と1点 (x,y,z) の距離
	// 2次元要素の時のみ
	double getDistanceSq(const kmb::ElementBase &element,const double x,const double y,const double z) const;
	// 要素の面と1点 (x,y,z) の距離の最小値
	double getDistanceSqOnBoundary(const kmb::ElementBase &element,const double x,const double y,const double z) const;
	// 四角形の対角線の長さの比
	// 四角形が同一平面にあるかどうか
	// 平面要素のなす角度
	double getAngleBetweenElements(const kmb::ElementBase &element0,const kmb::ElementBase &element1) const;
	// 面のなす角の cos = - 法線ベクトルの内積
	// この値が正の時が面と面が鋭角になっていることを意味する
	double getCosBetweenElements(const kmb::ElementBase &element0,const kmb::ElementBase &element1) const;
	// 四角形に凹点が存在するかどうか
	// 存在したらその点の index を返し、存在しなければ -1 を返す
	int getConcaveInQuad(const kmb::ElementBase &quad) const;
	// ヤコビアンの要素内の最大最小
	// QUAD or HEXAHEDRON の場合の distorsion element を探すため
	bool getMinMaxJacobian(const kmb::ElementBase &element, double &min, double &max) const;
	// 重複節点番号の個数
	// 全ての組み合わせの比較のうち等号になったものの個数
	int getDuplicationNodeIdCount(const kmb::ElementBase &element) const;

	// 要素剛性マトリクス
	// 節点の個数大きさのマトリクスに代入（インデックスは節点番号ではないことに注意）
	bool getStiffMatrix(const kmb::ElementBase &element,kmb::SquareMatrix &stiff) const;
protected:
	const kmb::Point3DContainer* points;
	const kmb::Point2DContainer* point2Ds;
private:
	double getMaxAngleCos(const kmb::ElementBase &eIter) const;
	double getMinAngleCos(const kmb::ElementBase &eIter) const;
};

}
