/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshRefiner                                             #
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
// 要素を分割または次数を上げる
//

#pragma once

#include "MeshDB/kmbElement.h"
#include <vector>
#include <string>

namespace kmb{

class MeshData;
class DataBindings;
class ElementContainer;
class MiddleNodeManager;

class MeshRefiner
{
public:
	enum interpolationRule{
		MAX,
		MIN,
		MIDDLE
	};
protected:
	struct DataPair{
		std::string name;
		std::string stype;
		const kmb::DataBindings* originalData;
		kmb::DataBindings* refinedData;
	};
	kmb::MeshData* mesh;
	kmb::MiddleNodeManager* middleMan;
	bool middleManDeletable;
	// dataPairs は
	// refine 前 : { name, stype, orgData, NULL }
	// refine 後 : { name, stype, orgData, refinedData }
	// となる
	std::vector< kmb::MeshRefiner::DataPair > dataPairs;

	void setSecondNodes( kmb::ElementBase& element );
	interpolationRule interpRule;
	// 2次要素の形状関数の補正をするか
	bool secondFitting;
public:
	MeshRefiner(void);
	virtual ~MeshRefiner(void);
	void clear(void);
	void setMesh(kmb::MeshData* mesh);
	void setMiddleNodeManager(kmb::MiddleNodeManager* manager);
	// 既に２次要素になっている ElementGroup について MiddleNodeManager を更新する
	void setSecondNodesByData(const char* name,const char* stype=NULL);
	// 既に２次要素になっている Body について MiddleNodeManager を更新する
	void setSecondNodesByBody(kmb::bodyIdType bodyId);
	// ２次要素に変換する
	kmb::bodyIdType convertToSecondBody(kmb::bodyIdType bodyId);
	// 細分後の要素の個数
	size_t getRefinedElementCount(const kmb::ElementContainer* orgBody) const;
	// 要素を細分する
	// offsetElementId を与えて、refine の前後では異なる elementId を与えている
	kmb::bodyIdType refineBody(kmb::bodyIdType orgBodyId);
	// 要素を細分する
	// targetBodyId の変更は bodyId を付与するためにこのメソッドの外でやる
	// refinedBody == NULL の時は細分した要素を登録せずに境界条件だけを更新する
	size_t refineBody(const kmb::ElementContainer* orgBody, kmb::ElementContainer* refinedBody);
	// 要素を細分した時に追加された節点について、元の節点を返す
	kmb::elementType getOriginal(kmb::nodeIdType middleNodeId, kmb::nodeIdType* originalNodes) const;
	// 辺または要素を与えて、その中点が与えられていれば、それを返す
	kmb::nodeIdType getMiddle(kmb::ElementBase &element) const;
	// refineBody するときに同時に更新するデータの登録
	void appendData(const char* dataname,const char* stype=NULL);
	size_t getDataCount(void) const;
	// dataPairs のクリア
	void clearData(void);
	// commit することで、refineElement の時の更新対象を refine 後の data に変更する
	// commit 前 : { name, stype, orgData, refinedData }
	// commit 後 : { name, stype, refinedData, NULL }
	// また、mesh に name stpe で登録されているデータを refinedData に置き換える
	// このメソッドを実行前 : mesh に登録されているデータ = orgData
	// このメソッドを実行後 : mesh に登録されているデータ = refinedData
	// となる
	void commitData(void);
	void setInterpolationMode(const char* mode);
	const char* getInterpolationMode(void) const;
	void setSecondFitting(bool flag);
	bool setSecondFitting(void) const;
protected:
	// body に追加する
	// refineBody が NULL のときは、節点グループの更新だけを行う
	void refineSegment( kmb::elementIdType elementId, const kmb::ElementBase &segment, kmb::ElementContainer* refinedBody );
	void refineSegment2( kmb::elementIdType elementId, const kmb::ElementBase &segment2, kmb::ElementContainer* refinedBody );
	void refineTriangle( kmb::elementIdType elementId, const kmb::ElementBase &triangle, kmb::ElementContainer* refinedBody );
	void refineTriangle2( kmb::elementIdType elementId, const kmb::ElementBase &triangle2, kmb::ElementContainer* refinedBody );
	void refineQuad( kmb::elementIdType elementId, const kmb::ElementBase &quad, kmb::ElementContainer* refinedBody );
	void refineQuad2( kmb::elementIdType elementId, const kmb::ElementBase &quad2, kmb::ElementContainer* refinedBody );
	void refineTetrahedron( kmb::elementIdType elementId, const kmb::ElementBase &tetra, kmb::ElementContainer* refinedBody );
	void refineTetrahedron2( kmb::elementIdType elementId, const kmb::ElementBase &tetra2, kmb::ElementContainer* refinedBody );
	void refineHexahedron( kmb::elementIdType elementId, const kmb::ElementBase &hexa, kmb::ElementContainer* refinedBody );
	void refineHexahedron2( kmb::elementIdType elementId, const kmb::ElementBase &hexa2, kmb::ElementContainer* refinedBody );
	void refineWedge( kmb::elementIdType elementId, const kmb::ElementBase &wedge, kmb::ElementContainer* refinedBody );
	void refineWedge2( kmb::elementIdType elementId, const kmb::ElementBase &wedge2, kmb::ElementContainer* refinedBody );
	void refinePyramid( kmb::elementIdType elementId, const kmb::ElementBase &pyramid, kmb::ElementContainer* refinedBody );
	void refinePyramid2( kmb::elementIdType elementId, const kmb::ElementBase &pyramid2, kmb::ElementContainer* refinedBody );

	bool nodeGroupUpdate( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n01, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeGroupUpdate( const kmb::ElementBase &elem, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeGroupUpdate( const kmb::ElementBase &elem, int faceIndex, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);

	bool nodeVariableUpdate( kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n01, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeVariableUpdate( const kmb::ElementBase &elem, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
	bool nodeVariableUpdate( const kmb::ElementBase &elem, int faceIndex, kmb::nodeIdType centerId, const kmb::DataBindings* originalData, kmb::DataBindings* refinedData);
};

}
