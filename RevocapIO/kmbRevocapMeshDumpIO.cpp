/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RevocapMeshDumpIO                                       #
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
// RevocapMeshDumpIO 仕様
//
// バイナリファイル
// version 情報 （文字列は８個のchar + バージョン番号は４バイト整数値）
// 型情報 int double 等の sizeof の記述
// nodeCount
// nodeId x y z (nodeCount分繰り返し)
// bodyCount
//  bodyName
//  elementCount (bodyCount分繰り返し）
//   elementType elementId nodeArray (elementCount分繰り返し）
// dbCount
//  boundingmode, idcount, name, stype, valuetype, targetbodyId (dbCount分繰り返し)
//   variable の場合: (id, value : idcount分繰り返し) 
//   group の場合: value, (id : idcount分繰り返し) 
//   global の場合: value
//
// 文字列は len + 実体(\0を除く)

#ifdef WIN32
// 引数の型が違うのでこのままでは使えない
// #if _MSC_VER >= 1400
//  #define fopen fopen_s
// #endif
#endif

#include "RevocapIO/kmbRevocapMeshDumpIO.h"
#include "MeshDB/kmbMeshData.h"
#include "Common/kmbCommon.h"
#include "RevocapIO/kmbDefineIO.h"

#include <fstream>
#include <cstdio>
#include <cstring>

const char* kmb::RevocapMeshDumpIO::headerString = "REVOMESH";
const int kmb::RevocapMeshDumpIO::headerLength = 8;

bool
kmb::RevocapMeshDumpIO::checkTypeSize(void)
{
	return
			sizeof(int32_t) == 4
		&&	sizeof(int64_t) == 8
		&&	sizeof(uint32_t) == 4
		&&	sizeof(uint64_t) == 8
		&&	sizeof(float32_t) == 4
		&&	sizeof(float64_t) == 8
		;
}

kmb::RevocapMeshDumpIO::RevocapMeshDumpIO(void)
//: fp(NULL)
{
}

kmb::RevocapMeshDumpIO::~RevocapMeshDumpIO(void)
{
}

void kmb::RevocapMeshDumpIO::setSizeInfoVersion1(void)
{
	sInfo.nodeCount = 4;
	sInfo.nodeId = 4;
	sInfo.xyz = 8;
	sInfo.bodyCount = 4;
	sInfo.bodyId = 4;
	sInfo.elementCount = 4;
	sInfo.elementId = 4;
	sInfo.elementType = 4;
	sInfo.dataCount = 4;
	sInfo.bindingMode = 4;
	sInfo.idCount = 4;
	sInfo.valueType = 4;
	sInfo.value = 8;
	sInfo.intValue = 4;
	sInfo.faceId = 4;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::getVersion(int &ver,std::istream &input)
{
	if( input.fail() ){
		return kmb::RevocapIO::kFileOpenError;
	}
	char buf[16];
	input.seekg(0, std::ios::beg);
	input.read(buf,8);
	if( input.fail() ){
		return kmb::RevocapIO::kFileReadError;
	}
	buf[8] = '\0';
	std::cout << buf << std::endl;
	if( strncmp(buf,kmb::RevocapMeshDumpIO::headerString,kmb::RevocapMeshDumpIO::headerLength) == 0 ){
		int32_t v = -1;
		input.read(reinterpret_cast<char*>(&v),sizeof(int32_t));
		if( input.fail() ){
			return kmb::RevocapIO::kFileReadError;
		}
		ver = static_cast<int>(v);
		std::cout << "version " << ver << std::endl;
		switch( ver )
		{
		case 1:
			// ver 1 の時はヘッダに書かれていないのでデフォルト値
			setSizeInfoVersion1();
			break;
		default:
			break;
		}
	}
	return kmb::RevocapIO::kSuccess;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::setVersion(const int ver,std::ostream &output)
{
	if( output.fail() ){
		return kmb::RevocapIO::kFileOpenError;
	}
	output.seekp(0, std::ios::beg);
	std::cout << kmb::RevocapMeshDumpIO::headerString << std::endl;
	output.write(kmb::RevocapMeshDumpIO::headerString,kmb::RevocapMeshDumpIO::headerLength);
	std::cout << "version " << ver << std::endl;
	uint32_t v = ver;
	output.write(reinterpret_cast<char*>(&v),sizeof(uint32_t));
	switch( ver )
	{
	case 1:
		setSizeInfoVersion1();
		break;
	default:
		break;
	}
	return kmb::RevocapIO::kSuccess;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::loadNodeData(kmb::MeshData* mesh,std::istream &input)
{
	if( input.fail() ){
		return kmb::RevocapIO::kFileReadError;
	}
	if( mesh == NULL ){
		return kmb::RevocapIO::kMeshNullError;
	}
	size_t nCount = 0;
	switch( sInfo.nodeCount )
	{
	case 4:
	{
		uint32_t n = 0;
		input.read(reinterpret_cast<char*>(&n),sizeof(uint32_t));
		nCount = static_cast<size_t>(n);
		break;
	}
	case 8:
	{
		uint64_t n = 0;
		input.read(reinterpret_cast<char*>(&n),sizeof(uint64_t));
		nCount = static_cast<size_t>(n);
		break;
	}
	default:
		return kmb::RevocapIO::kHeaderSizeError;
	}
	if( input.fail() ){
		return kmb::RevocapIO::kFileReadError;
	}
	std::cout << "nodeCount = " << nCount << std::endl;
	if( sInfo.nodeId == 4 && sInfo.xyz == 8 ){
		float64_t node[3] = {0.0,0.0,0.0};
		int32_t nodeId = kmb::nullNodeId;
		mesh->beginNode( nCount );
		int nSize = static_cast<int>(nCount);
		for(kmb::nodeIdType i=0;i<nSize;++i){
			input.read( reinterpret_cast<char*>(&nodeId), sizeof(int32_t) );
			if( input.fail() ){
				return kmb::RevocapIO::kFileReadError;
			}
			input.read( reinterpret_cast<char*>(node), sizeof(float64_t)*3 );
			if( input.fail() ){
				return kmb::RevocapIO::kFileReadError;
			}
			mesh->addNodeWithId(node[0],node[1],node[2],nodeId);
		}
		mesh->endNode();
		return kmb::RevocapIO::kSuccess;
	}
	return kmb::RevocapIO::kNodeSizeNotImplemented;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::saveNodeData(const kmb::MeshData* mesh,std::ostream &output)
{
	if( output.fail() ){
		return kmb::RevocapIO::kFileWriteError;
	}
	if( mesh == NULL ){
		return kmb::RevocapIO::kMeshNullError;
	}
	switch( sInfo.nodeCount )
	{
	case 4:
	{
		uint32_t nCount = static_cast<uint32_t>( mesh->getNodeCount() );
		output.write( reinterpret_cast<char*>(&nCount), sizeof(uint32_t) );
		break;
	}
	case 8:
	{
		uint64_t nCount = static_cast<uint64_t>( mesh->getNodeCount() );
		output.write( reinterpret_cast<char*>(&nCount), sizeof(uint64_t) );
		break;
	}
	default:
		return kmb::RevocapIO::kHeaderSizeError;
	}
	double x,y,z;
	if( sInfo.nodeId == 4 && sInfo.xyz == 8 ){
		float64_t node[3] = {0.0,0.0,0.0};
		int32_t nodeId = kmb::nullNodeId;
		kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
		kmb::Point3DContainer::const_iterator nEnd = mesh->getNodes()->end();
		while( nIter != nEnd ){
			nodeId = nIter.getId();
			nIter.getXYZ( x, y, z );
			node[0] = x;
			node[1] = y;
			node[2] = z;
			output.write( reinterpret_cast<char*>(&nodeId), sizeof(int32_t) );
			output.write( reinterpret_cast<char*>(node), sizeof(float64_t)*3 );
			++nIter;
		}
		return kmb::RevocapIO::kSuccess;
	}
	return kmb::RevocapIO::kSuccess;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::loadElementData(kmb::MeshData* mesh,std::istream &input)
{
	if( input.fail() ){
		return kmb::RevocapIO::kFileReadError;
	}
	if( mesh == NULL ){
		return kmb::RevocapIO::kMeshNullError;
	}
	kmb::bodyIdType bCount = 0;
	switch( sInfo.bodyCount )
	{
	case 4:
	{
		uint32_t n = 0;
		input.read( reinterpret_cast<char*>(&n), sizeof(uint32_t) );
		bCount = static_cast<kmb::bodyIdType>(n);
		break;
	}
	case 8:
	{
		uint64_t n = 0;
		input.read( reinterpret_cast<char*>(&n), sizeof(uint64_t) );
		bCount = static_cast<kmb::bodyIdType>(n);
		break;
	}
	default:
		return kmb::RevocapIO::kHeaderSizeError;
	}
	if( sInfo.elementCount == 4 &&
		sInfo.elementId == 4 &&
		sInfo.elementType == 4 &&
		sInfo.nodeId == 4 )
	{
		int32_t eType = kmb::UNKNOWNTYPE;
		int32_t elementId = kmb::Element::nullElementId;
		uint32_t len = 0;
		uint32_t eCount = 0;
		int32_t* nodes = new int32_t[kmb::Element::MAX_NODE_COUNT];
		kmb::nodeIdType* cells = new kmb::nodeIdType[kmb::Element::MAX_NODE_COUNT];
		size_t nCount = 0; // 要素ごとの節点個数
		for(kmb::bodyIdType bodyId=0;bodyId<bCount;++bodyId){
			input.read( reinterpret_cast<char*>(&len), sizeof(uint32_t) );
			if( input.fail() ){ return kmb::RevocapIO::kFileReadError; }
			char* name = new char[len+1];
			input.read( name, sizeof(char)*len );
			name[len] = '\0';
			input.read( reinterpret_cast<char*>(&eCount), sizeof(uint32_t) );
			if( input.fail() ){ return kmb::RevocapIO::kFileReadError; }
			std::cout << "bodyId " << bodyId << std::endl;
			std::cout << "elementCount " << eCount << std::endl;
			std::cout << "bodyName " << name << std::endl;
			mesh->beginElement( eCount );
			for(uint32_t i=0;i<eCount;++i){
				input.read( reinterpret_cast<char*>(&eType), sizeof(int32_t) );
				if( input.fail() ){ return kmb::RevocapIO::kFileReadError; }
				input.read( reinterpret_cast<char*>(&elementId), sizeof(int32_t) );
				if( input.fail() ){ return kmb::RevocapIO::kFileReadError; }
				nCount = static_cast<size_t>( kmb::Element::getNodeCount( static_cast<kmb::elementType>(eType) ) );
				input.read( reinterpret_cast<char*>(nodes), sizeof(int32_t)*nCount );
				if( input.fail() ){ return kmb::RevocapIO::kFileReadError; }
				for(unsigned int j=0;j<nCount;++j){
					cells[j] = nodes[j];
				}
				mesh->addElementWithId( static_cast<kmb::elementType>(eType), cells, elementId );
			}
			mesh->endElement();
			mesh->setBodyName(bodyId,name);
			delete[] name;
		}
		delete[] cells;
		delete[] nodes;
		return kmb::RevocapIO::kSuccess;
	}
	return kmb::RevocapIO::kElementSizeNotImplemented;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::saveElementData(const kmb::MeshData* mesh,std::ostream &output)
{
	std::cout << "saveElementData" << std::endl;
	if( output.fail() ){
		return kmb::RevocapIO::kFileWriteError;
	}
	if( mesh == NULL ){
		return kmb::RevocapIO::kMeshNullError;
	}
	kmb::bodyIdType bCount = 0;
	switch( sInfo.bodyCount )
	{
	case 4:
	{
		uint32_t n = static_cast<uint32_t>( mesh->getBodyCount() );
		output.write( reinterpret_cast<char*>(&n), sizeof(uint32_t) );
		bCount = static_cast<kmb::bodyIdType>(n);
		break;
	}
	case 8:
	{
		uint64_t n = static_cast<uint64_t>( mesh->getBodyCount() );
		output.write( reinterpret_cast<char*>(&n), sizeof(uint64_t) );
		bCount = static_cast<kmb::bodyIdType>(n);
		break;
	}
	default:
		return kmb::RevocapIO::kHeaderSizeError;
	}
	std::cout << (int)sInfo.elementCount << " " <<
		(int)sInfo.elementId << " " <<
		(int)sInfo.elementType << " " <<
		(int)sInfo.nodeId << std::endl;
	if( sInfo.elementCount == 4 &&
		sInfo.elementId == 4 &&
		sInfo.elementType == 4 &&
		sInfo.nodeId == 4 )
	{
		int32_t* nodes = new int32_t[kmb::Element::MAX_NODE_COUNT];
		for(kmb::bodyIdType i=0;i<bCount;++i){
			const kmb::Body* body = mesh->getBodyPtr(i);
			if( body != NULL ){
				const char* name = body->getBodyName();
				std::cout << name << std::endl;
				uint32_t len = (name==NULL) ? 0 : static_cast<uint32_t>(strlen(name));
				output.write( reinterpret_cast<char*>(&len), sizeof(uint32_t) );
				output.write( name, len );
				uint32_t eCount = static_cast<uint32_t>( mesh->getElementCount(i) );
				output.write( reinterpret_cast<char*>(&eCount), sizeof(uint32_t) );
				kmb::ElementContainer::const_iterator eIter = body->begin();
				kmb::ElementContainer::const_iterator eEnd = body->end();
				while( eIter != eEnd ){
					int32_t eType = eIter.getType();
					int32_t elementId = eIter.getId();
					if( eType != kmb::UNKNOWNTYPE && elementId != kmb::Element::nullElementId ){
						output.write( reinterpret_cast<char*>(&eType), sizeof(int32_t) );
						output.write( reinterpret_cast<char*>(&elementId), sizeof(int32_t) );
						int nSize = kmb::Element::getNodeCount( static_cast<kmb::elementType>(eType) );
						for(int j=0;j<nSize;++j){
							nodes[j] = eIter.getCellId(j);
						}
						output.write( reinterpret_cast<char*>(nodes), sizeof(int32_t)*nSize );
					}else{
						eType = kmb::UNKNOWNTYPE;
						output.write( reinterpret_cast<char*>(&eType), sizeof(int32_t) );
						output.write( reinterpret_cast<char*>(&elementId), sizeof(int32_t) );
					}
					++eIter;
				}
			}else{
				uint32_t eCount = 0;
				output.write( reinterpret_cast<char*>(&eCount), sizeof(uint32_t) );
			}
		}
		delete[] nodes;
		return kmb::RevocapIO::kSuccess;
	}
	return kmb::RevocapIO::kElementSizeNotImplemented;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::loadBindingData(kmb::MeshData* mesh,std::istream &input)
{
	if( input.fail() ){
		return kmb::RevocapIO::kFileReadError;
	}
	if( mesh == NULL ){
		return kmb::RevocapIO::kMeshNullError;
	}
	// bmode
	// idcount
	// name
	// stype
	// vtype
	// bodyId
	if( sInfo.dataCount == sizeof( uint32_t ) &&
		sInfo.bindingMode == sizeof( int32_t ) &&
		sInfo.idCount == sizeof( uint32_t ) &&
		sInfo.valueType == sizeof( int32_t ) &&
		sInfo.bodyId == sizeof( int32_t ) )
	{
		uint32_t dataNum = 0;
		input.read( reinterpret_cast<char*>(&dataNum), sizeof(uint32_t) );
		std::cout << "dataNum = " << dataNum << std::endl;
		for(uint32_t i=0;i<dataNum;++i){
			// bmode
			int32_t bmode = kmb::DataBindings::Unknown;
			input.read( reinterpret_cast<char*>(&bmode), sizeof(int32_t) );
			// idcount
			uint32_t idcount = 0;
			input.read( reinterpret_cast<char*>(&idcount), sizeof(uint32_t) );
			// name
			uint32_t len = 0;
			input.read( reinterpret_cast<char*>(&len), sizeof(uint32_t) );
			char* name = new char[len+1];
			input.read( name, sizeof(char)*len );
			name[len] = '\0';
			// stype
			uint32_t slen = 0;
			input.read( reinterpret_cast<char*>(&slen), sizeof(uint32_t) );
			char* stype = new char[slen+1];
			input.read( stype, sizeof(char)*slen );
			stype[slen] = '\0';
			// vtype
			int32_t vtype = kmb::PhysicalValue::Unknown;
			input.read( reinterpret_cast<char*>(&vtype), sizeof(int32_t) );
			// bodyId
			int32_t bodyId = kmb::Body::nullBodyId;
			input.read( reinterpret_cast<char*>(&bodyId), sizeof(int32_t) );
			std::cout << name << " " << bmode << " " << vtype << " " << idcount << std::endl;
			kmb::DataBindings* data = mesh->createDataBindings(name,
				static_cast<kmb::DataBindings::bindingMode>(bmode),
				static_cast<kmb::PhysicalValue::valueType>(vtype),
				stype,bodyId);
			delete[] name;
			delete[] stype;
			loadData(data,input,idcount);
		}
	}
	return kmb::RevocapIO::kSuccess;
}

kmb::RevocapIO::ErrorCode
kmb::RevocapMeshDumpIO::saveBindingData(const kmb::MeshData* mesh,std::ostream &output)
{
	if( output.fail() ){
		return kmb::RevocapIO::kFileWriteError;
	}
	if( mesh == NULL ){
		return kmb::RevocapIO::kMeshNullError;
	}
	if( sInfo.dataCount == sizeof( uint32_t ) &&
		sInfo.bindingMode == sizeof( int32_t ) &&
		sInfo.idCount == sizeof( uint32_t ) &&
		sInfo.valueType == sizeof( int32_t ) &&
		sInfo.bodyId == sizeof( int32_t ) )
	{
		uint32_t dataNum = static_cast<uint32_t>( mesh->getDataCount() );
		std::cout << "dataNum = " << dataNum << std::endl;
		output.write( reinterpret_cast<char*>(&dataNum), sizeof(uint32_t) );
		std::multimap< std::string, kmb::DataBindings* >::const_iterator dIter = mesh->getDataBindingsMap().begin();
		while( dIter != mesh->getDataBindingsMap().end() ){
			// bmode
			int32_t bmode = static_cast<int32_t>( dIter->second->getBindingMode() );
			output.write( reinterpret_cast<char*>(&bmode), sizeof(int32_t) );
			// idcount
			uint32_t idCount = static_cast<uint32_t>( dIter->second->getIdCount() );
			output.write( reinterpret_cast<char*>(&idCount), sizeof(uint32_t) );
			// name
			uint32_t len = static_cast<uint32_t>( dIter->first.size() );
			output.write( reinterpret_cast<char*>(&len), sizeof(uint32_t) );
			output.write( dIter->first.c_str(), len );
			// stype
			len = static_cast<uint32_t>( dIter->second->getSpecType().size() );
			output.write( reinterpret_cast<char*>(&len), sizeof(uint32_t) );
			output.write( dIter->second->getSpecType().c_str(), len );
			// vtype
			int32_t vtype = static_cast<int32_t>( dIter->second->getValueType() );
			output.write( reinterpret_cast<char*>(&vtype), sizeof(int32_t) );
			// bodyId
			int32_t bodyId = static_cast<int32_t>(dIter->second->getTargetBodyId());
			output.write( reinterpret_cast<char*>(&bodyId), sizeof(int32_t) );
			std::cout << dIter->first << " " << bmode << " " << vtype << " " << idCount << std::endl;
			saveData( dIter->second, output );
			++dIter;
		}
	}
	return kmb::RevocapIO::kSuccess;
}

kmb::RevocapIO::ErrorCode kmb::RevocapMeshDumpIO::loadData(kmb::DataBindings* data,std::istream &input,size_t count)
{
	if( input.fail() ){
		return kmb::RevocapIO::kFileReadError;
	}
	if( data == NULL ){
		return kmb::RevocapIO::kDataNullError;
	}
	if( sInfo.bodyId == sizeof( int32_t ) &&
		sInfo.elementId == sizeof( int32_t ) &&
		sInfo.nodeId == sizeof( int32_t ) &&
		sInfo.faceId == sizeof( int32_t ) &&
		sInfo.intValue == sizeof( int32_t ) &&
		sInfo.value == sizeof( float64_t ) )
	{
		switch(data->getBindingMode()){
		case kmb::DataBindings::BodyVariable:
		case kmb::DataBindings::ElementVariable:
		case kmb::DataBindings::NodeVariable:
			{
				uint32_t idCount = static_cast<uint32_t>(count);
				int32_t id = 0;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							input.read( reinterpret_cast<char*>(&v), sizeof(float64_t) );
							data->setPhysicalValue(id,&v);
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
							data->setPhysicalValue(id,v);
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
							data->setPhysicalValue(id,v);
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
							data->setPhysicalValue(id,v);
						}
					}
					break;
				default:
					break;
				}
				break;
			}
		case kmb::DataBindings::FaceVariable:
			{
				uint32_t idCount = static_cast<uint32_t>(count);
				int32_t id[2];
				kmb::Face f;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							input.read( reinterpret_cast<char*>(&v), sizeof(float64_t) );
							f.setId(id[0],id[1]);
							data->setPhysicalValue(f,&v);
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
							f.setId(id[0],id[1]);
							data->setPhysicalValue(f,v);
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
							f.setId(id[0],id[1]);
							data->setPhysicalValue(f,v);
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
							f.setId(id[0],id[1]);
							data->setPhysicalValue(f,v);
						}
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		case kmb::DataBindings::BodyGroup:
		case kmb::DataBindings::ElementGroup:
		case kmb::DataBindings::NodeGroup:
			{
				uint32_t idCount = static_cast<uint32_t>(count);
				int32_t id = 0;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						input.read( reinterpret_cast<char*>(&v), sizeof(float64_t) );
						data->setPhysicalValue(new kmb::ScalarValue(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							data->addId(id);
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
						data->setPhysicalValue(new kmb::Vector2Value(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							data->addId(id);
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
						data->setPhysicalValue(new kmb::Vector3Value(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							data->addId(id);
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
						data->setPhysicalValue(new kmb::Tensor6Value(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							data->addId(id);
						}
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		case kmb::DataBindings::FaceGroup:
			{
				uint32_t idCount = static_cast<uint32_t>(count);
				int32_t id[2];
				kmb::Face f;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						input.read( reinterpret_cast<char*>(&v), sizeof(float64_t) );
						data->setPhysicalValue(new kmb::ScalarValue(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							f.setId(id[0],id[1]);
							data->addId(f);
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
						data->setPhysicalValue(new kmb::Vector2Value(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							f.setId(id[0],id[1]);
							data->addId(f);
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
						data->setPhysicalValue(new kmb::Vector3Value(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							f.setId(id[0],id[1]);
							data->addId(f);
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
						data->setPhysicalValue(new kmb::Tensor6Value(v));
						for(unsigned int i=0;i<idCount;++i){
							input.read( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							f.setId(id[0],id[1]);
							data->addId(f);
						}
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		case kmb::DataBindings::Global:
			{
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << std::endl;
						float64_t v;
						input.read( reinterpret_cast<char*>(&v), sizeof(float64_t) );
						data->setPhysicalValue(new kmb::ScalarValue(v));
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << std::endl;
						float64_t v[2];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
						data->setPhysicalValue(new kmb::Vector2Value(v));
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << std::endl;
						float64_t v[3];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
						data->setPhysicalValue(new kmb::Vector3Value(v));
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << std::endl;
						float64_t v[6];
						input.read( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
						data->setPhysicalValue(new kmb::Tensor6Value(v));
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		default:
			break;
		}
	}
	return kmb::RevocapIO::kSuccess;
}

kmb::RevocapIO::ErrorCode kmb::RevocapMeshDumpIO::saveData(const kmb::DataBindings* data,std::ostream &output)
{
	if( output.fail() ){
		return kmb::RevocapIO::kFileWriteError;
	}
	if( data == NULL ){
		return kmb::RevocapIO::kDataNullError;
	}
	if( sInfo.bodyId == sizeof( int32_t ) &&
		sInfo.elementId == sizeof( int32_t ) &&
		sInfo.nodeId == sizeof( int32_t ) &&
		sInfo.faceId == sizeof( int32_t ) &&
		sInfo.intValue == sizeof( int32_t ) &&
		sInfo.value == sizeof( float64_t ) )
	{
		switch(data->getBindingMode()){
		case kmb::DataBindings::BodyVariable:
		case kmb::DataBindings::ElementVariable:
		case kmb::DataBindings::NodeVariable:
			{
				uint32_t idCount = static_cast<uint32_t>(data->getIdCount());
				int32_t id = 0;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						double d;
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							dIter.getValue(&d);
							v = static_cast<float64_t>(d);
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							output.write( reinterpret_cast<char*>(&v), sizeof(float64_t) );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						double d[2];
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							dIter.getValue(d);
							v[0] = static_cast<float64_t>(d[0]);
							v[1] = static_cast<float64_t>(d[1]);
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						double d[3];
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							dIter.getValue(d);
							v[0] = static_cast<float64_t>(d[0]);
							v[1] = static_cast<float64_t>(d[1]);
							v[2] = static_cast<float64_t>(d[2]);
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						double d[6];
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							dIter.getValue(d);
							v[0] = static_cast<float64_t>(d[0]);
							v[1] = static_cast<float64_t>(d[1]);
							v[2] = static_cast<float64_t>(d[2]);
							v[3] = static_cast<float64_t>(d[3]);
							v[4] = static_cast<float64_t>(d[4]);
							v[5] = static_cast<float64_t>(d[5]);
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
							++dIter;
						}
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		case kmb::DataBindings::FaceVariable:
			{
				uint32_t idCount = static_cast<uint32_t>(data->getIdCount());
				int32_t id[2];
				kmb::Face f;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						double d;
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							dIter.getValue(&d);
							v = static_cast<float64_t>(d);
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							output.write( reinterpret_cast<char*>(&v), sizeof(float64_t) );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						double d[2];
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							dIter.getValue(d);
							v[0] = static_cast<float64_t>(d[0]);
							v[1] = static_cast<float64_t>(d[1]);
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						double d[3];
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							dIter.getValue(d);
							v[0] = static_cast<float64_t>(d[0]);
							v[1] = static_cast<float64_t>(d[1]);
							v[2] = static_cast<float64_t>(d[2]);
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						double d[6];
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							dIter.getValue(d);
							v[0] = static_cast<float64_t>(d[0]);
							v[1] = static_cast<float64_t>(d[1]);
							v[2] = static_cast<float64_t>(d[2]);
							v[3] = static_cast<float64_t>(d[3]);
							v[4] = static_cast<float64_t>(d[4]);
							v[5] = static_cast<float64_t>(d[5]);
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
							++dIter;
						}
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		case kmb::DataBindings::BodyGroup:
		case kmb::DataBindings::ElementGroup:
		case kmb::DataBindings::NodeGroup:
			{
				uint32_t idCount = static_cast<uint32_t>(data->getIdCount());
				int32_t id = 0;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						double d;
						data->getPhysicalValue()->getValue(&d);
						v = static_cast<float64_t>(d);
						output.write( reinterpret_cast<char*>(&v), sizeof(float64_t) );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						double d[2];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						double d[3];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						v[2] = static_cast<float64_t>(d[2]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						double d[6];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						v[2] = static_cast<float64_t>(d[2]);
						v[3] = static_cast<float64_t>(d[3]);
						v[4] = static_cast<float64_t>(d[4]);
						v[5] = static_cast<float64_t>(d[5]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							id = static_cast<int32_t>(dIter.getId());
							output.write( reinterpret_cast<char*>(&id), sizeof(int32_t) );
							++dIter;
						}
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		case kmb::DataBindings::FaceGroup:
			{
				uint32_t idCount = static_cast<uint32_t>(data->getIdCount());
				int32_t id[2];
				kmb::Face f;
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << idCount << std::endl;
						float64_t v;
						double d;
						data->getPhysicalValue()->getValue(&d);
						v = static_cast<float64_t>(d);
						output.write( reinterpret_cast<char*>(&v), sizeof(float64_t) );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << idCount << std::endl;
						float64_t v[2];
						double d[2];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << idCount << std::endl;
						float64_t v[3];
						double d[3];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						v[2] = static_cast<float64_t>(d[2]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							++dIter;
						}
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << idCount << std::endl;
						float64_t v[6];
						double d[6];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						v[2] = static_cast<float64_t>(d[2]);
						v[3] = static_cast<float64_t>(d[3]);
						v[4] = static_cast<float64_t>(d[4]);
						v[5] = static_cast<float64_t>(d[5]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
						kmb::DataBindings::const_iterator dIter = data->begin();
						while( !dIter.isFinished() ){
							dIter.getFace(f);
							id[0] = static_cast<int32_t>(f.getElementId());
							id[1] = static_cast<int32_t>(f.getLocalFaceId());
							output.write( reinterpret_cast<char*>(id), sizeof(int32_t)*2 );
							++dIter;
						}
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		case kmb::DataBindings::Global:
			{
				switch(data->getValueType()){
				case kmb::PhysicalValue::Scalar:
					{
						std::cout << "Scalar " << std::endl;
						float64_t v;
						double d;
						data->getPhysicalValue()->getValue(&d);
						v = static_cast<float64_t>(d);
						output.write( reinterpret_cast<char*>(&v), sizeof(float64_t) );
					}
					break;
				case kmb::PhysicalValue::Vector2:
					{
						std::cout << "Vector2 " << std::endl;
						float64_t v[2];
						double d[2];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*2 );
					}
					break;
				case kmb::PhysicalValue::Vector3:
					{
						std::cout << "Vector3 " << std::endl;
						float64_t v[3];
						double d[3];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						v[2] = static_cast<float64_t>(d[2]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*3 );
					}
					break;
				case kmb::PhysicalValue::Tensor6:
					{
						std::cout << "Tensor6 " << std::endl;
						float64_t v[6];
						double d[6];
						data->getPhysicalValue()->getValue(d);
						v[0] = static_cast<float64_t>(d[0]);
						v[1] = static_cast<float64_t>(d[1]);
						v[2] = static_cast<float64_t>(d[2]);
						v[3] = static_cast<float64_t>(d[3]);
						v[4] = static_cast<float64_t>(d[4]);
						v[5] = static_cast<float64_t>(d[5]);
						output.write( reinterpret_cast<char*>(v), sizeof(float64_t)*6 );
					}
					break;
				default:
					break;
				}
				break;
			}
			break;
		default:
			break;
		}
	}
	return kmb::RevocapIO::kSuccess;
}

int
kmb::RevocapMeshDumpIO::loadMeshFromFile(const char *filename, kmb::MeshData *mesh )
{
	if( mesh==NULL ){
		return -1;
	}

	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	int version = 1;
	getVersion( version, input );

	loadNodeData( mesh, input );

	loadElementData( mesh, input );

	loadBindingData( mesh, input );

	input.close();
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveMeshToFile(const char *filename, const kmb::MeshData *mesh)
{
	if( mesh==NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out|std::ios_base::binary );
	int version = 1;
	setVersion( version, output );

	saveNodeData( mesh, output );

	saveElementData( mesh, output );

	saveBindingData( mesh, output );

	output.close();
	return 0;
}
