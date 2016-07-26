/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : HecmwIO                                                 #
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
#pragma once

#include <string>
#include <vector>
#include "MeshDB/kmbElement.h"

namespace kmb{

class MeshData;
class ElementContainer;
class DataBindings;

class HecmwIO
{
public:
	enum solutionType{
		STATIC,
		NLSTATIC,
		HEAT,
		EIGEN,
		DYNAMIC
	};
private:
	struct egrpData{
		std::string egrpName;
		std::string materialName;
		size_t egrpCount;
		kmb::bodyIdType bodyId;
	};
	// 読み込むときに引いて、書き込むときに足す
	// ファイルの id が 1 から始まる時は、これらの offset は 1 にする
	kmb::nodeIdType offsetNodeId;
	kmb::elementIdType offsetElementId;
	solutionType soltype;
	// true のときは spectype に依らずに出力する
	bool dataFlag;
	bool resElementFlag;
	bool dummySectionFlag;
	std::string resHeader;
	std::vector< kmb::HecmwIO::egrpData > egrpInfo;
public:
	HecmwIO( kmb::nodeIdType offsetNodeId=1, kmb::elementIdType offsetElementId=1);
	~HecmwIO(void);
	void clear(void);

	// HEC msh ファイルを読み込む
	// すべての element はどこかの section に含まれているとする
	// SECTION で定義されている EGRP を MeshData の Body の単位にする
	// MATERIAL => BodyGroup, Hash値 (Data の名前は MATERIAL の名前で登録)
	//             young
	//             poisson
	//             density
	//             linear_exp
	//             specific_heat
	//             heat_conductivity
	// SECTION => BodyVariable, Hash値 (Data の名前も SpecType も SECTION)
	//         TYPE => "SOLID" or "SHELL" or "INTERFACE"
	//         THICKNESS => float
	//         INTEGPOINTS => integer
	//         GAPCON => float
	//         GAPRAD1 => float
	//         GAPRAD2 => float
	int loadFromFile(const char* filename,kmb::MeshData* mesh);
	int loadFromMW3File(const char* filename,kmb::MeshData* mesh) const;
	int loadFromFRSFile(const char* filename,kmb::MeshData* mesh) const;
	int loadFromResFile(const char* filename,kmb::MeshData* mesh) const;
	int loadFromResFileItem(const char* filename,kmb::DataBindings* data,const char* name) const;
	// アスキーデータの読み込み
	int loadFromResAsciiFile(const char* filename,kmb::MeshData* mesh) const;
	// バイナリデータの読み込み
	int loadFromResBinFile(const char* filename,kmb::MeshData* mesh) const;
	// メッシュファイル出力
	int saveToFile(const char* filename,const kmb::MeshData* mesh) const;
	// バイナリ形式で出力
	int saveToResFile(const char* filename,kmb::MeshData* mesh) const;
	// MW3 形式で出力
	int saveToFileMW3(const char* filename,const kmb::MeshData* mesh,const char* partName) const;
	// Ver.3.6 形式
	int saveToMeshFile_3_6(const char* filename,const kmb::MeshData* mesh) const;
	// Ver.4.4 形式
	int saveToMeshFile_4_4(const char* filename,const kmb::MeshData* mesh,const char* partName) const;
	// master の節点を slave の要素で評価して equation data を出力する
	// 絶対値が thresh 以下の項は出力しない
	// 要素座標が outThresh 以下の項があれば方程式を出力しない
	int appendEquationToFile(const char* filename,const kmb::MeshData* mesh,const char* master,const char* slave,double thresh=1.0e-10,double outThresh=-1.0e-6) const;
	// SGROUP を msh ファイルに追加する
	int appendSgroupToFile(const char* filename,const kmb::MeshData* mesh,const char* sgroup,const char* stype=NULL) const;
	int appendFooterToFile(const char* filename) const;
	solutionType setSolutionType( const char* soltype );
	const char* getSolutionType( void ) const;
	// NGROUP EGROUP SGROUP を読み書きする場合は true
	void setDataFlag(bool flag);
	// res ファイルで element の結果を読む場合は true
	void setResElementFlag(bool flag);
	// ダミーの SECTION を追加する場合は true
	void setDummySectionFlag(bool flag);

	void setResHeader(const char* header);
	void setOffsetElementId(kmb::elementIdType offset);
	void setOffsetNodeId(kmb::nodeIdType offset);
	kmb::elementIdType getOffsetElementId(void) const;
	kmb::nodeIdType getOffsetNodeId(void) const;

	// 局所面番号の変換
	int correctLocalFaceId( kmb::MeshData* mesh, const char* faceName, const char* stype=NULL );
protected:
	// type の変換
	// TETRAHEDRON <=> 341
	static int getHECType( kmb::elementType etype );
	static kmb::elementType getRevocapType( int etype );

	// カード(!KEYWORD) の次の行から読んで、次のカードを読み込んだらその行を line に代入して返す
	// Header
	int readHeader( std::ifstream &input, std::string &line ) const;
	// Node : mesh = NULL の時は個数だけを返す
	size_t readNode( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const;
	// Element : body = NULL の時は個数だけを返す
	size_t readElement( std::ifstream &input, std::string &line, kmb::ElementContainer* body ) const;
	// Section : mesh = NULL の時は空読み
	// 読むときは mesh に bodyId => Hash の DataVariable "SECTION" を作成する
	// SECTION のたびに Body を生成して、bodyId を発行する
	// Hash の内容は
	//  TYPE => "SOLID" or "SHELL" or "INTERFACE"
	//  THICKNESS => スカラー値
	//  INTEGPOINTS => 整数値
	//  GAPCON => スカラー値
	//  GAPRAD1 => スカラー値
	//  GAPRAD2 => スカラー値
	kmb::bodyIdType readSection( std::ifstream &input, std::string &line, kmb::MeshData* mesh );
	// Material : mesh = NULL の時は空読み
	int readMaterial( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const;
	// EGroup : mesh = NULL の時は egrpInfo への登録のみ
	size_t readEGroup( std::ifstream &input, std::string &line, kmb::MeshData* mesh, kmb::ElementContainer* parentBody=NULL );
	size_t readNGroup( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const;
	size_t readSGroup( std::ifstream &input, std::string &line, kmb::MeshData* mesh ) const;

	void writeHeader( std::ofstream &output, std::string version="" ) const;
	void writeNode( std::ofstream &output, const kmb::MeshData* mesh, std::string partName="" ) const;
	void writeElement( std::ofstream &output, const kmb::MeshData* mesh, kmb::bodyIdType bodyId, std::string partName="" ) const;
	void writeMaterial( std::ofstream &output, const kmb::MeshData* mesh ) const;
	void writeSection( std::ofstream &output, const kmb::MeshData* mesh, kmb::bodyIdType bodyId ) const;
	void writeEGroup( std::ofstream &output, const kmb::MeshData* mesh, std::string partName="" ) const;
	void writeNGroup( std::ofstream &output, const kmb::MeshData* mesh, std::string partName="" ) const;
	void writeSGroup( std::ofstream &output, const kmb::MeshData* mesh, std::string partName="" ) const;
	void writeAmplitude( std::ofstream &output, const kmb::MeshData* mesh, std::string partName="" ) const;
	void writeContactPair( std::ofstream &output, const kmb::MeshData* mesh, std::string card="!CONTACT_PAIR", std::string partName="" ) const;
	void writeAssemblyPair( std::ofstream &output, const kmb::MeshData* mesh, std::string partName="" ) const;
	void writeZero( std::ofstream &output, const kmb::MeshData* mesh ) const;
	void writeInitialCondition( std::ofstream &output, const kmb::MeshData* mesh ) const;

	int getEgrpInfoIndex( std::string name ) const;
	int createEgrpInfo( std::string name, std::string matname );

	// 面の変換 Fstr => Revo
	static int tetRmap[];
	static int wedRmap[];
	static int hexRmap[];
	static int pyrRmap[];
	// 面の変換 Revo => Fstr
	static int tetFmap[];
	static int wedFmap[];
	static int hexFmap[];
	static int pyrFmap[];
};

}
