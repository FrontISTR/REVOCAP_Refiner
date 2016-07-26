/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : MeshData                                                #
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

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbPhysicalValue.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementOctree.h"

#include "Geometry/kmbBoundingBox.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbOctree.h"
#include "Common/kmbCalculator.h"

#include <vector>
#include <string>

namespace kmb{

class Matrix4x4;

typedef std::multimap< std::string, kmb::DataBindings* > datamap;

class MeshData
{
protected:
	kmb::Point3DContainer* node3Ds;
	std::vector< kmb::ElementContainer* > bodies;
	kmb::ElementContainer* currentBody;
	std::multimap< std::string, kmb::DataBindings* > bindings;
	// 一時的なデータ保存用
	std::vector< kmb::DataBindings* > bindingsStack;
	int bindingsStackDim;
	std::string defaultSpecType;
	// the element ids are not duplicated even if they are not contained in different bodies
	elementIdType maxElementId;
	// coordinate system
	kmb::Matrix4x4* coordMatrix;
public:
	MeshData(void);
	virtual ~MeshData(void);
	// mesh を new した状態に戻す
	virtual void clearModel(void);
	// モデル全体の登録を開始
	// 基底クラスでは特に何もしない
	// 派生クラスでは目的に応じて外部の初期化ルーチンをコールバックするなど
	virtual void beginModel(void);
	// モデル全体の登録の終了
	// 基底クラスでは特に何もしない
	// 派生クラスでは目的に応じて外部の初期化ルーチンをコールバックするなど
	virtual void endModel(void);
	//-------------- 座標系 ---------------------//
	// コピーして使う
	void setCoordinate( const kmb::Matrix4x4& matrix );
	void translateCoordinate( double x, double y, double z );
	kmb::Matrix4x4* getCoordMatrix(void);
	const kmb::Matrix4x4* getCoordMatrix(void) const;
	//-------------- 節点管理 -------------------//
	int getNodeDim(void) const;
	// 節点の登録開始
	// コンテナを指定することができる
	virtual void beginNode(size_t size=0,const char* containerType=NULL);
	virtual void beginNode(size_t size,kmb::Point3DContainer* point3Ds);
	// 節点の登録
	virtual kmb::nodeIdType addNode(double x,double y,double z);
	// 節点の登録（Id付き）
	virtual kmb::nodeIdType addNodeWithId(double x,double y,double z, kmb::nodeIdType id);
	// 節点の登録終了
	// 基底クラスでは何もしない
	// 派生クラスのコールバックで使う
	virtual void endNode(void);
	// 節点番号の最大最小
	// 存在しないときに max < min
	// 次に番号を指定せずに追加したときには max + 1
	kmb::nodeIdType getMaxNodeId(void) const;
	kmb::nodeIdType getMinNodeId(void) const;
	// 節点コンテナの取得
	// each メソッド等はコンテナを取得してから
	const kmb::Point3DContainer* getNodes(void) const;
	kmb::Point3DContainer* getNodes(void);
	virtual const kmb::Point2DContainer* getNode2Ds(void) const{ return NULL; };
	kmb::Point3DContainer* replaceNodes(kmb::Point3DContainer* nodes);
	// 節点数
	virtual size_t getNodeCount(void) const;
	// 節点を直接 Point3D 型で取得
	virtual bool getNode(nodeIdType i,kmb::Point3D &node) const;
	bool getNodeXYZ(nodeIdType i,double &x,double &y,double &z) const;
	// endNode した後で節点の追加
	// 失敗したら kmb::nullNodeId を返す
	virtual kmb::nodeIdType insertNode(double x,double y,double z);
	// endNode した後で節点の追加 (id 付き)
	// 失敗したら kmb::nullNodeId を返す
	virtual kmb::nodeIdType insertNodeWithId(double x,double y,double z, kmb::nodeIdType id);
	// 既に登録済みの節点の座標を更新する
	virtual bool updateNode(double x,double y,double z, kmb::nodeIdType id);
	// コンテナタイプ
	const char* getNodeContainerType(void) const;
	virtual const kmb::BoundingBox getBoundingBox(void) const;
	//-------------- 要素管理 -------------------//
	// 要素の登録開始
	// set suitable container type
	// 以降 endElement が呼ばれるまで currentBody に登録される
	virtual kmb::bodyIdType beginElement(size_t size,kmb::ElementContainer* container);
	virtual kmb::bodyIdType beginElement(size_t size=0,const char* containerType=NULL);
	// 要素の追加
	virtual kmb::elementIdType addElement(kmb::elementType type,kmb::nodeIdType *ary);
	// 要素の追加（Id付き）
	virtual kmb::elementIdType addElementWithId(kmb::elementType type,kmb::nodeIdType *ary,kmb::elementIdType elementId);
	// 要素の登録終了
	virtual void endElement(void);
	// max element id means that larger id is not used
	kmb::elementIdType getMaxElementId(void) const;
	// beginElement endElement の後で要素の追加
	virtual kmb::elementIdType insertElement(kmb::bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary);
	virtual kmb::elementIdType insertElementWithId(kmb::bodyIdType bodyID,kmb::elementType type,kmb::nodeIdType *ary,kmb::elementIdType elementId);
	// 登録済みの要素の移動
	virtual bool moveElement(kmb::bodyIdType bodyId0,kmb::bodyIdType bodyId1,kmb::elementIdType elementId);
	// 要素グループの個数取得
	kmb::bodyIdType getBodyCount(void) const;
	// 要素番号から要素グループ番号の取得
	bodyIdType getBodyId(elementIdType elementId) const;
	// 要素グループコンテナの追加・置換
	// offset フラグは追加するコンテナの offset を自動的に書き換える
	virtual kmb::bodyIdType appendBody( kmb::Body* body, bool offset=true );
	virtual kmb::bodyIdType replaceBody( kmb::bodyIdType bodyId, kmb::Body* body, bool offset=true );
	// MeshData の外で ElementContainer を更新したときに
	// MeshData が持つ情報も更新するため
	// beginElement endElement で仮の Body を作って insertElement した場合は、最後にこのメソッドを呼び出して更新しておく
	virtual bool updateBody( kmb::bodyIdType bodyId );
	// 要素グループの中身を空にするだけで、bodyId は消さない
	virtual void clearBody(bodyIdType bodyId);
	// すべての要素グループを消去する
	virtual void removeAllBodies(void);
	// 要素グループのコンテナ取得
	kmb::Body* getBodyPtr(bodyIdType bodyId);
	const kmb::Body* getBodyPtr(bodyIdType bodyId) const;
	// 要素グループに含まれる要素の個数の取得
	size_t getElementCount(kmb::bodyIdType bodyId) const;
	size_t getElementCount(void) const;
	size_t getElementCountOfDim(int dim) const;
	// 要素グループの次元（表面メッシュなら2、体積メッシュなら3）
	int getDimension(kmb::bodyIdType bodyId) const;
	// 要素グループの最大次元（表面メッシュなら2、体積メッシュなら3）
	int getDimension(void) const;
	// 要素グループの次数（2次要素なら2）
	int getDegree(kmb::bodyIdType bodyId) const;
	// 要素グループに含まれる要素のタイプが一意かどうか
	bool isUniqueElementType(kmb::bodyIdType bodyId,kmb::elementType etype) const;
	// 要素の取得
	kmb::ElementContainer::iterator findElement(elementIdType elementID,bodyIdType bodyID=kmb::Body::nullBodyId);
	kmb::ElementContainer::const_iterator findElement(elementIdType elementID,bodyIdType bodyID=kmb::Body::nullBodyId) const;
	// 要素グループ名
	virtual const char* getBodyName(bodyIdType bodyId) const;
	virtual void setBodyName(bodyIdType bodyId,const char* name);
	kmb::bodyIdType getBodyIdByName(const char* name) const;
	// コンテナタイプ
	const char* getElementContainerType(bodyIdType bodyId) const;
	// 外部メッシュから要素グループの追加
	kmb::bodyIdType importBody(const kmb::MeshData& otherMesh,kmb::bodyIdType bodyId);
	//---------------------------------------------//
	//-------------- 物理量管理 -------------------//
	//---------------------------------------------//
	virtual const std::multimap< std::string, kmb::DataBindings* >& getDataBindingsMap(void) const;
	kmb::datamap::iterator beginDataIterator();
	kmb::datamap::const_iterator beginDataIterator() const;
	kmb::datamap::iterator endDataIterator();
	kmb::datamap::const_iterator endDataIterator() const;
	// 物理量の生成（派生クラスで実装）
	virtual kmb::DataBindings* createDataBindings(
		const char* name,
		kmb::DataBindings::bindingMode bmode,
		kmb::PhysicalValue::valueType vtype,
		const char* stype="",
		kmb::bodyIdType targetBodyId=kmb::Body::nullBodyId);
	// 物理量データの取得
	kmb::DataBindings* getDataBindingsPtr(const char* name,const char* stype=NULL);
	const kmb::DataBindings* getDataBindingsPtr(const char* name,const char* stype=NULL) const;
	// 物理量データの設定
	bool setDataBindingsPtr(const char* name,kmb::DataBindings* data,const char* stype=NULL);
	// 登録されている物理量の種類の個数
	virtual size_t getDataCount(const char* stype=NULL) const;
	bool hasData(const char* name,const char* stype=NULL) const;
	bool renameData(const char* oldname,const char* newname,const char* stype=NULL);
	bool deleteData(const char* name,const char* stype=NULL);
	bool clearData(const char* name,const char* stype=NULL);
	// name に登録されているデータの置き換え
	// 使わなくなった olddata はこのメソッドの外で delete すること
	// olddata は NULL であってはいけないが、newdata は NULL の場合も可
	bool replaceData(const kmb::DataBindings* olddata, kmb::DataBindings* newdata, const char* name,const char* stype=NULL);
	// id の個数
	size_t getIdCount(const char* name,const char* stype=NULL) const;
	// stype を省略したときに何を使うかを決めておく
	void setDefaultSpecType(const char* specType);
	// 登録されている物理量の設定モードを返す
	kmb::DataBindings::bindingMode getDataMode(const char* name, const char* stype=NULL) const;
	// 登録されている物理量の値の型を返す
	kmb::PhysicalValue::valueType getDataValueType(const char* name, const char* stype=NULL) const;
	// 登録されている物理量の識別子(SpecType)を返す
	std::string getDataSpecType(const char* name) const;
	// 登録されている物理量の識別子(SpecType)を変更する
	bool setDataSpecType(const char* name,const char* oldstype,const char* newstype);
	// コンテナタイプ
	const char* getDataContainerType(const char* name,const char* stype=NULL) const;
	// Id の登録
	virtual void addId(const char* name,kmb::idType id,const char* stype=NULL);
	virtual void addId(const char* name,kmb::Face f,const char* stype=NULL);
	// id が登録されているかどうか
	// 物理量に値を登録
	virtual void setPhysicalValue(const char* name,kmb::PhysicalValue* val,const char* stype=NULL);
	virtual void setPhysicalValueAtId(const char* name,kmb::idType id,kmb::PhysicalValue* val,const char* stype=NULL);
	virtual void setPhysicalValueAtId(const char* name,kmb::Face f,kmb::PhysicalValue* val,const char* stype=NULL);
	// 物理量を取得
	virtual const kmb::PhysicalValue* getPhysicalValue(const char* name,const char* stype=NULL) const;
	virtual const kmb::PhysicalValue* getPhysicalValueAtId(const char* name,kmb::idType id,const char* stype=NULL) const;
	virtual const kmb::PhysicalValue* getPhysicalValueAtId(const char* name,kmb::Face f,const char* stype=NULL) const;
	virtual bool getPhysicalValueAtId(const char* name,kmb::idType id,double *val,const char* stype=NULL) const;
	virtual bool getPhysicalValueAtId(const char* name,kmb::Face f,double *val,const char* stype=NULL) const;
	// 複数同時登録
	void appendTargetData(const char* name, const char* stype=NULL);
	void appendTargetDataPtr(kmb::DataBindings* data);
	void clearTargetData(void);
	int getTargetDataNum(void) const;
	int getTargetDataDim(void) const;
	virtual void setMultiPhysicalValues(kmb::idType id, std::vector< kmb::PhysicalValue* > &values);
	virtual void setMultiPhysicalValues(kmb::idType id, double* values);
	virtual void setMultiPhysicalValues(kmb::idType id, long* values);
	virtual void setMultiPhysicalValues(double* values);
	virtual void getMultiPhysicalValues(kmb::idType id, double* values) const;
	// auto name generation such as "prefix_0" "prefix_1"
	std::string getUniqueDataName(std::string prefix,int num=0);
	// 登録されているデータに ElementGroup / ElementVariable があれば、
	// orgElementId のデータを elementId が引き継ぐ
	void deriveTargetData(kmb::elementIdType elementId,kmb::elementIdType orgElementId);

	// 変換系：データからボディを生成して登録
	kmb::bodyIdType faceGroupToBody(const char* faceG,const char* stype=NULL);
	// 変換系：データからボディを生成して登録、値は要素値に登録
	kmb::bodyIdType faceVariableToBody(const char* faceV,const char* elemV,const char* stype=NULL);
	// 変換系：データから指定されたボディに追加（ボディの追加はしない）
	size_t faceGroupToBody(const char* faceG,kmb::ElementContainer* body, const char* stype=NULL);
	size_t faceVariableToBody(const char* faceV,kmb::ElementContainer* body,const char* stype=NULL);
	// get node ids of binding data
	void getNodeSetFromDataBindings(std::set<kmb::nodeIdType>&nodeSet,const char* name,const char* stype=NULL) const;
	void getBoundingBoxOfData(kmb::BoundingBox &bbox,const kmb::DataBindings* data) const;
	// あらかじめデータは作っておくこと
	int convertData(const char* org, const char* conv, const char* orgstype=NULL,const char* convstype=NULL);
	int convertData(const kmb::DataBindings* orgData, kmb::DataBindings* convData);
	// 引き算 subtrahend に含まれる Id を　minuend からのぞく
	// FaceGroup NodeGroup ElementGroup に対応
	int subtractData(const char* subt, const char* minu, const char* subtstype=NULL,const char* minustype=NULL);
	// Body に含まれる節点をすべて節点グループにする
	// Body を FaceGroup ElementGroup に変換する
	int convertBodyToData(kmb::bodyIdType bodyId, const char* name,const char* stype=NULL);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

	// (x,y,z) から距離 tolerance 以下で最も近い要素を探す
	// solid 要素の時 tolerance = 0.0 とすると、(x,y,z) を含む点を探す
	virtual kmb::elementIdType searchElement(kmb::bodyIdType bodyId,double x,double y,double z,double tolerance=0.0) const{ return kmb::Element::nullElementId; };

	// 0次補間：最も近い節点での値を取り出す
	// NodeVariable のみ対応
	virtual int getValueOnNearestNode(const char* name,double x,double y,double z,double *values,const char* stype=NULL) const { return -1; };
	// 補間値を計算する
	virtual int getInterpolatedValueInBody(const char* name,kmb::bodyIdType bodyId,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const { return -1; };
	virtual int getInterpolatedValue(const char* name,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const { return -1; };
	virtual int getInterpolatedValueInData(const char* name,const char* target,double x,double y,double z,double *values,double tolerance=0.0,const char* stype=NULL) const{ return -1; };
	// TargetData でキャッシュしてあるデータについて補間値を求める
	// 戻り値は values に入れた数値の次元
	// 補間できなかったら 0 を、その他のエラーは -1 を返す
	virtual int getInterpolatedValuesInBody(kmb::bodyIdType bodyId,double x,double y,double z,double *values) const { return -1; };
	virtual int getInterpolatedValues(double x,double y,double z,double *values) const { return -1; };
	virtual int getInterpolatedValuesInData(const char* target,double x,double y,double z,double *values,double tolerance=0.0) const{ return -1;};

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

};

}
