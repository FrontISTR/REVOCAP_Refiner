/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : SurfaceOperation                                        #
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
//
// 2次元要素からなる表面メッシュ（パッチ）に対する幾何処理・位相処理
//

#pragma once

#include <map>
#include <vector>
#include <string>
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "Common/kmbClassification.h"

namespace kmb{

class MeshData;
class Face;
class ElementContainer;
class NodeNeighborInfo;
class Point3DContainer;
class DataBindings;

class SurfaceOperation
{
protected:
	kmb::MeshData* mesh;
	kmb::ElementContainer* elements;
	kmb::NodeNeighborInfo* neighborInfo;
	kmb::Classification< kmb::elementIdType > dividedElementsInfo;
	std::set< std::pair<kmb::elementIdType, kmb::elementIdType> > edgesBetweenElements;
	kmb::Classification< kmb::Face > dividedFacesInfo;
	std::set< std::pair<kmb::Face,kmb::Face> > edgesBetweenFaces;
	// 表面から面グループを再構成するときに逆向きの表面からも許す
	bool permitReverse;
public:
	SurfaceOperation(void);
	virtual ~SurfaceOperation(void);
	void setMesh(kmb::MeshData* mesh);
	void setPermitReverse(bool f);
	//
	// divide surface by threshold of angles
	// surfIDs = array of divided surface id
	// original body Id = all frames
	//
	// if domain of body id does not consist of 2-dim element, do nothing
	//
	// WithRidge
	// surfIDs[divided surface id] = ridge id
	//
	void divideSurface(bodyIdType bodyId,double angle,std::vector<kmb::bodyIdType> &surfIDs);
	void divideSurfaceWithRidge(bodyIdType bodyId,double angle,std::map<kmb::bodyIdType,kmb::bodyIdType> &surfIDs);	
	// 
	// divide face group by threshold of angles
	// the names of divided face groups are postfixed "1" such as "face_1". 
	//
	// divideFaceGroup は分割の境界の稜線集合を Body として保存して、その bodyId を返す
	// divideFaceGroupWithRidge は分割した FaceGroup とその稜線の BodyId の連想配列を入れる
	// key = FaceGroup の名前、value = それを囲む稜線集合の BodyId
	// FaceGroup の stype は "Brep" にする。
	// 戻り値は全体の稜線の BodyId
	kmb::bodyIdType divideFaceGroup(std::string name,double angle,std::vector<std::string> &subFaceGroups);
	kmb::bodyIdType divideFaceGroupWithRidge(std::string name,double angle,std::map<std::string,kmb::bodyIdType> &subFaceGroupMapper);
	// divide => 無理やり分割する（角度を閾値にする）
	// decompose => 自然に分かれる
	// 連結成分に分割する
	void decomposeSurface(kmb::bodyIdType bodyID, std::set< kmb::bodyIdType > &components);
	// FaceGroup を連結成分に分割する
	bool decomposeFaceGroup(std::string faceGroup, std::vector<std::string> &subFaceGroups);
	// 連結成分ごとに分ける下請けメソッド
	// element id は保存
	void decomposeByElementContainer(kmb::ElementContainer* elements,std::set< kmb::ElementContainer* > &components);
	void clear(void);

	void setSurface(kmb::MeshData* mesh,kmb::bodyIdType bodyId);
	enum vertexType{
		Unknown,
		CONVEX,     // 頂点の周りの要素の角度の和が 360 度より小さい＆凸
		CONCAVE,    // 頂点の周りの要素の角度の和が 360 度より小さい＆凹
		FLAT,       // 頂点の周りの要素の角度の和が 360 度
		HYPERBOLIC  // 頂点の周りの要素の角度の和が 360 度より大きい
	};
	// NodeNeighborInfo を外から与える場合
	// ElementContainer が Element2D のみからなることはノーチェック
	static vertexType getVertexType
		(kmb::nodeIdType nodeId,
		 kmb::MeshData* mesh,
		 kmb::ElementContainer* elements,
		 kmb::NodeNeighborInfo* neighborInfo);
	vertexType getVertexType(kmb::nodeIdType nodeId) const;

	kmb::bodyIdType faceGroup2Surface(const char* name);
	bool surface2FaceGroup(kmb::bodyIdType bodyId,kmb::bodyIdType parentBodyId,const char* name);
	// parentFaceGroup で与えられる face のうち、bodyId で与えられる面と共通のものを name の
	// face group に登録する
	bool surface2FaceGroup(kmb::bodyIdType bodyId,const char* parentFaceGroup,const char* name);
protected:
	// append data to divideElementsInfo
	// return the number of divided groups
	size_t divideSurface(const kmb::ElementContainer* elements,const kmb::Point3DContainer* points,double angle);
	// 分割される面の三角形と四角形の個数
	// 2次要素も同じように数える
	bool getDividedSurfaceElementCount(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,size_t &triCount,size_t &quadCount) const;
	// get divided surface included an element with initElementId as id
	size_t getDividedSurface(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,kmb::ElementContainer* dividedSurf) const;
	size_t getTotalRidge(const kmb::ElementContainer* org,kmb::ElementContainer* ridges) const;
	size_t getSurfaceRidge(const kmb::ElementContainer* orgSurf,kmb::elementIdType initElementId,kmb::ElementContainer* ridges) const;
	// append data to divideFacesInfo
	// return the number of divided groups
	size_t divideFaceGroup(const kmb::DataBindings* data, const kmb::ElementContainer* elements, const kmb::Point3DContainer* points, double angle);
	size_t divideComponentsFaceGroup(const kmb::DataBindings* data, const kmb::ElementContainer* elements);
	// get divided face group included an initFace
	size_t getDividedFaceGroup(kmb::Face initFace,kmb::DataBindings* data) const;
	size_t getSurfaceRidge(const kmb::ElementContainer* elements,kmb::Face initFace,kmb::ElementContainer* ridges) const;
};

}
