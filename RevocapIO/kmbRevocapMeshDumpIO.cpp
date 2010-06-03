/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : RevocapMeshDumpIO                                       #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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





















#ifdef WIN32




#endif

#include "RevocapIO/kmbRevocapMeshDumpIO.h"
#include "MeshDB/kmbMeshData.h"

#include <cstdio>
#include <cstring>

typedef unsigned int	uint4;
typedef unsigned long	uint8;
typedef int				int4;
typedef long			int8;
typedef float			float4;
typedef double			float8;

const char* kmb::RevocapMeshDumpIO::headerString = "REVOMESH";

bool
kmb::RevocapMeshDumpIO::checkTypeSize(void)
{
	return
			sizeof(int4) == 4
		&&	sizeof(int8) == 8
		&&	sizeof(uint4) == 4
		&&	sizeof(uint8) == 8
		&&	sizeof(float4) == 4
		&&	sizeof(float8) == 8
		;
}

kmb::RevocapMeshDumpIO::RevocapMeshDumpIO(void)
: fp(NULL)
{
}

kmb::RevocapMeshDumpIO::~RevocapMeshDumpIO(void)
{
}

bool
kmb::RevocapMeshDumpIO::getVersion(int &ver)
{
	size_t res = 0;
	if( fp != NULL )
	{
		fseek( fp, 0, SEEK_SET );
		char buf[16];
		res = fread( buf, sizeof(char), 8, fp );
		if( res < 8 ){
			return false;
		}
		buf[9] = 0;
		if( strncmp(buf,kmb::RevocapMeshDumpIO::headerString,
			strlen(kmb::RevocapMeshDumpIO::headerString)) == 0 ){
			int4 v = -1;
			res = fread( &v, sizeof(int4), 1, fp );
			if( res < 1 ){
			    return false;
			}
			ver = static_cast<int>(v);
			switch( ver )
			{
			case 1:

				sInfo.bodyCount = sizeof( size_t );
				sInfo.nodeCount = sizeof( size_t );
				sInfo.nodeId = sizeof( kmb::nodeIdType );
				sInfo.xyz = sizeof( double );
				sInfo.bodyCount = sizeof( size_t );
				sInfo.bodyId = sizeof( kmb::bodyIdType );
				sInfo.elementCount = sizeof( size_t );
				sInfo.elementId = sizeof( kmb::elementIdType );
				sInfo.elementType = sizeof( kmb::elementType );
				sInfo.dataCount = sizeof( int );
				sInfo.bindingMode = sizeof( int );
				sInfo.idCount = sizeof( size_t );
				sInfo.valueType = sizeof( int );
				sInfo.value = sizeof( double );
				sInfo.faceId = sizeof( kmb::idType );
				break;
			default:
				break;
			}
			return true;
		}
	}
	return false;;
}

bool
kmb::RevocapMeshDumpIO::setVersion(const int ver)
{
	if( fp != NULL ){
		fseek( fp, 0, SEEK_SET );
		fwrite( kmb::RevocapMeshDumpIO::headerString, sizeof(char), 8, fp );
		uint4 v = ver;
		fwrite( &v, sizeof(uint4), 1, fp );
		switch( ver )
		{
		case 1:
			sInfo.nodeCount = sizeof( size_t );
			sInfo.nodeId = sizeof( kmb::nodeIdType );
			sInfo.xyz = sizeof( double );
			sInfo.bodyCount = sizeof( size_t );
			sInfo.bodyId = sizeof( kmb::bodyIdType );
			sInfo.elementCount = sizeof( size_t );
			sInfo.elementId = sizeof( kmb::elementIdType );
			sInfo.elementType = sizeof( kmb::elementType );
			sInfo.dataCount = sizeof( int );
			sInfo.bindingMode = sizeof( int );
			sInfo.idCount = sizeof( size_t );
			sInfo.valueType = sizeof( int );
			sInfo.value = sizeof( double );
			sInfo.faceId = sizeof( kmb::idType );
			break;
		default:
			break;
		}
		return true;
	}else{
		return false;
	}
}

int
kmb::RevocapMeshDumpIO::loadNodeData(kmb::MeshData* mesh)
{
	size_t res = 0;
	if( fp && mesh ){
		size_t nCount = 0;
		switch( sInfo.nodeCount )
		{
		case 4:
			{
				uint4 n = 0;
				res = fread( &n, sizeof(uint4), 1, fp);
				nCount = static_cast<size_t>(n);
			}
			break;
		case 8:
			{
				uint8 n = 0;
				res = fread( &n, sizeof(uint8), 1, fp);
				nCount = static_cast<size_t>(n);
			}
			break;
		default:
			return 0;
		}
		if( sInfo.nodeId == 4 && sInfo.xyz == 8 && res == 1){
			float8 node[3] = {0.0,0.0,0.0};
			int4 nodeId = kmb::nullNodeId;
			mesh->beginNode( nCount );
			int nSize = static_cast<int>(nCount);
			for(kmb::nodeIdType i=0;i<nSize;++i){
				res = fread( &nodeId, sizeof(int4), 1, fp);
				if( res < 1 )	      break;
				res = fread( node, sizeof(float8), 3, fp);
				if( res < 3 )	      break;
				mesh->addNodeWithId(node[0],node[1],node[2],nodeId);
			}
			mesh->endNode();
			return nSize;
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveNodeData(const kmb::MeshData* mesh)
{
	if( fp && mesh ){
		int nSize = 0;
		switch( sInfo.nodeCount )
		{
		case 4:
			{
				uint4 nCount = static_cast<uint4>( mesh->getNodeCount() );
				fwrite( &nCount, sizeof(uint4), 1, fp);
				nSize = static_cast<int>( nCount );
			}
			break;
		case 8:
			{
				uint8 nCount = static_cast<uint8>( mesh->getNodeCount() );
				fwrite( &nCount, sizeof(uint8), 1, fp);
				nSize = static_cast<int>( nCount );
			}
			break;
		default:
			return 0;
		}

		if( sInfo.nodeId == 4 && sInfo.xyz == 8 ){
			float8 node[3] = {0.0,0.0,0.0};
			int4 nodeId = kmb::nullNodeId;
			kmb::Point3DContainer::const_iterator nIter = mesh->getNodes()->begin();
			kmb::Point3DContainer::const_iterator nEnd = mesh->getNodes()->end();
			while( nIter != nEnd ){
				nodeId = nIter.getId();
				nIter.getXYZ( node[0], node[1], node[2] );
				fwrite( &nodeId, sizeof(int4), 1, fp);
				fwrite( node, sizeof(float8), 3, fp);
				++nIter;
			}
			return nSize;
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::loadElementData(kmb::MeshData* mesh)
{
	size_t res = 0;
	if( fp && mesh ){
		kmb::bodyIdType bCount = 0;
		switch( sInfo.bodyCount )
		{
		case 4:
			{
				uint4 n = 0;
				res = fread( &n, sizeof(uint4), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		case 8:
			{
				uint8 n = 0;
				res = fread( &n, sizeof(uint8), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		default:
			return 0;
		}
		if( sInfo.elementCount == 4 &&
			sInfo.elementId == 4 &&
			sInfo.elementType == 4 &&
			sInfo.nodeId == 4 &&
			res == 1 )
		{
			int4 eType = kmb::UNKNOWNTYPE;
			int4 elementId = kmb::Element::nullElementId;
			uint4 eCount = 0;
			int4* nodes = new int4[kmb::Element::MAX_NODE_COUNT];
			size_t nCount = 0;
			for(kmb::bodyIdType i=0;i<bCount;++i){
				res = fread( &eCount, sizeof(uint4), 1, fp);
				if( res < 1 ) break;
				mesh->beginElement( eCount );
				for(uint4 i=0;i<eCount;++i){
					res = fread( &eType, sizeof(int4), 1, fp);
					if( res < 1 ) break;
					res = fread( &elementId, sizeof(int4), 1, fp);
					if( res < 1 ) break;
					nCount = static_cast<size_t>( kmb::Element::getNodeCount( static_cast<kmb::elementType>(eType) ) );
					res = fread( nodes, sizeof(int4), nCount, fp);
					if( res < nCount )	break;
					kmb::nodeIdType* cells = NULL;
					cells = new kmb::nodeIdType[nCount];
					for(unsigned int j=0;j<nCount;++j){
						cells[j] = nodes[j];
					}
					mesh->addElementWithId( static_cast<kmb::elementType>(eType), cells, elementId );
				}
				mesh->endElement();
			}
			delete[] nodes;
			return static_cast<int>( bCount );
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveElementData(const kmb::MeshData* mesh)
{
	if( fp && mesh ){
		kmb::bodyIdType bCount = 0;
		switch( sInfo.bodyCount )
		{
		case 4:
			{
				uint4 n = static_cast<uint4>( mesh->getBodyCount() );
				fwrite( &n, sizeof(uint4), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		case 8:
			{
				uint8 n = static_cast<uint8>( mesh->getBodyCount() );
				fwrite( &n, sizeof(uint8), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		default:
			return 0;
		}
		if( sInfo.elementCount == 4 &&
			sInfo.elementId == 4 &&
			sInfo.elementType == 4 &&
			sInfo.nodeId == 4 )
		{
			int4* nodes = new int4[kmb::Element::MAX_NODE_COUNT];
			for(kmb::bodyIdType i=0;i<bCount;++i){
				const kmb::Body* body = mesh->getBodyPtr(i);
				if( body != NULL ){
					uint4 eCount = static_cast<uint4>( mesh->getElementCount(i) );
					fwrite( &eCount, sizeof(uint4), 1, fp);
					kmb::ElementContainer::const_iterator eIter = body->begin();
					kmb::ElementContainer::const_iterator eEnd = body->end();
					while( eIter != eEnd ){
						int4 eType = eIter.getType();
						int4 elementId = eIter.getId();
						if( eType != kmb::UNKNOWNTYPE && elementId != kmb::Element::nullElementId ){
							fwrite( &eType, sizeof(int4), 1, fp);
							fwrite( &elementId, sizeof(int4), 1, fp);
							int nSize = kmb::Element::getNodeCount( static_cast<kmb::elementType>(eType) );
							for(int j=0;j<nSize;++j){
								nodes[j] = eIter.getCellId(j);
							}
							fwrite( nodes, sizeof(int4), nSize, fp);
						}else{
							int4 eType = kmb::UNKNOWNTYPE;
							fwrite( &eType, sizeof(int4), 1, fp);
							fwrite( &elementId, sizeof(int4), 1, fp);
						}
						++eIter;
					}
				}else{
					uint4 eCount = 0;
					fwrite( &eCount, sizeof(uint4), 1, fp);
				}
			}
			delete[] nodes;
			return static_cast<int>( bCount );
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::loadElementDataNeglectingId(kmb::MeshData* mesh)
{
	size_t res = 0;
	if( fp && mesh ){
		kmb::bodyIdType bCount = 0;
		switch( sInfo.bodyCount )
		{
		case 4:
			{
				uint4 n = 0;
				res = fread( &n, sizeof(uint4), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		case 8:
			{
				uint8 n = 0;
				res = fread( &n, sizeof(uint8), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		default:
			return 0;
		}
		if( sInfo.elementCount == 4 &&
			sInfo.elementId == 4 &&
			sInfo.elementType == 4 &&
			sInfo.nodeId == 4 &&
			res == 1 )
		{
			int appendedBodyNum = 0;
			int4 eType = kmb::UNKNOWNTYPE;
			int4 elementId = kmb::Element::nullElementId;
			uint4 eCount = 0;
			int4* nodes = new int4[kmb::Element::MAX_NODE_COUNT];
			size_t nCount = 0;
			for(kmb::bodyIdType i=0;i<bCount;++i){
				res = fread( &eCount, sizeof(uint4), 1, fp);
				if( eCount > 0 && res == 1 ){
					++appendedBodyNum;
					mesh->beginElement( eCount );
					for(uint4 i=0;i<eCount;++i){
						res = fread( &eType, sizeof(int4), 1, fp);
						if( res < 1 ) break;
						res = fread( &elementId, sizeof(int4), 1, fp);
						if( res < 1 ) break;
						nCount = static_cast< size_t >( kmb::Element::getNodeCount( static_cast<kmb::elementType>(eType) ) );
						res = fread( nodes, sizeof(int4), nCount, fp);
						if( res < nCount ) break;
						kmb::nodeIdType* cells = NULL;
						cells = new kmb::nodeIdType[nCount];
						for(unsigned int j=0;j<nCount;++j){
							cells[j] = nodes[j];
						}
						mesh->addElement( static_cast<kmb::elementType>(eType), cells );
					}
					mesh->endElement();
				}
			}
			delete[] nodes;
			return appendedBodyNum;
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveElementDataNeglectingId(const kmb::MeshData* mesh)
{
	if( fp && mesh ){
		kmb::bodyIdType bCount = 0;
		switch( sInfo.bodyCount )
		{
		case 4:
			{
				uint4 n = static_cast<uint4>( mesh->getBodyCount() );
				fwrite( &n, sizeof(uint4), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		case 8:
			{
				uint8 n = static_cast<uint8>( mesh->getBodyCount() );
				fwrite( &n, sizeof(uint8), 1, fp);
				bCount = static_cast<kmb::bodyIdType>(n);
			}
			break;
		default:
			return 0;
		}
		if( sInfo.elementCount == 4 &&
			sInfo.elementId == 4 &&
			sInfo.elementType == 4 &&
			sInfo.nodeId == 4 )
		{
			int4 elementId = 0;
			int4* nodes = new int4[kmb::Element::MAX_NODE_COUNT];
			for(kmb::bodyIdType i=0;i<bCount;++i){
				const kmb::Body* body = mesh->getBodyPtr(i);
				if( body != NULL ){
					uint4 eCount = static_cast<uint4>( mesh->getElementCount(i) );
					fwrite( &eCount, sizeof(uint4), 1, fp);
					kmb::ElementContainer::const_iterator eIter = body->begin();
					kmb::ElementContainer::const_iterator eEnd = body->end();
					while( eIter != eEnd ){
						int4 eType = eIter.getType();
						if( eType != kmb::UNKNOWNTYPE && elementId != kmb::Element::nullElementId ){
							fwrite( &eType, sizeof(int4), 1, fp);
							fwrite( &elementId, sizeof(int4), 1, fp);
							int nSize = kmb::Element::getNodeCount( static_cast<kmb::elementType>(eType) );
							for(int j=0;j<nSize;++j){
								nodes[j] = eIter.getCellId(j);
							}
							fwrite( nodes, sizeof(int4), nSize, fp);
						}else{
							int4 eType = kmb::UNKNOWNTYPE;
							fwrite( &eType, sizeof(int4), 1, fp);
							fwrite( &elementId, sizeof(int4), 1, fp);
						}
						++eIter;
						++elementId;
					}
				}else{
					uint4 eCount = 0;
					fwrite( &eCount, sizeof(uint4), 1, fp);
				}
			}
			delete[] nodes;
			return static_cast<int>( bCount );
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::loadBindingData(kmb::MeshData* mesh)
{
	size_t res = 0;
	if( fp && mesh && feof(fp) == 0 ){
		if( sInfo.dataCount == sizeof( size_t ) &&
			sInfo.bindingMode == sizeof( int ) &&
			sInfo.idCount == sizeof( size_t ) &&
			sInfo.valueType == sizeof( int ) &&
			sInfo.value == sizeof( double ) &&
			sInfo.bodyId == sizeof( kmb::bodyIdType ) )
		{
			size_t dataNum = 0;
			res = fread( &dataNum, sizeof(size_t), 1, fp);
			if( res < 1 ) return -1;
			while( dataNum > 0 ){
				uint4 num = 0;
				res = fread( &num, sizeof(uint4), 1, fp);
				if( res < 1 ) break;

				int bmode = 0;
				res = fread( &bmode, sizeof(int), 1, fp);
				if( res < 1 ) break;

				size_t idCount = 0;
				res = fread( &idCount, sizeof(size_t), 1, fp);
				if( res < 1 ) break;
				kmb::DataBindings** dataArray = new kmb::DataBindings*[num];
				for(uint4 i=0;i<num;++i){
					uint4 len = 0;

					res = fread( &len, sizeof(uint4), 1, fp);
					if( res < 1 ) break;
					char* name = NULL;
					name = new char[len+1];
					res = fread( name, sizeof(char), len, fp);
					if( res < 1 ) break;
					name[len] = '\0';

					res = fread( &len, sizeof(uint4), 1, fp);
					if( res < 1 ) break;
					char* stype = NULL;
					stype = new char[len+1];
					res = fread( stype, sizeof(char), len, fp);
					if( res < len ) break;
					stype[len] = '\0';

					int vtype = 0;
					res = fread( &vtype, sizeof(int), 1, fp);
					if( res < 1 ) break;

					kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
					res = fread( &bodyId, sizeof(kmb::bodyIdType), 1, fp);
					if( res < 1 ) break;
					kmb::DataBindings* data =
						mesh->createDataBindings
							(name,
							static_cast< kmb::DataBindings::bindingMode>(bmode),
							static_cast< kmb::PhysicalValue::valueType>(vtype),
							stype,bodyId);
					dataArray[i] = data;
				}
				loadBindingDataId( dataArray, num, idCount );
				delete[] dataArray;
				dataNum -= num;
			}
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveBindingData(const kmb::MeshData* mesh)
{
	if( fp && mesh ){
		if( sInfo.dataCount == sizeof( size_t ) &&
			sInfo.bindingMode == sizeof( int ) &&
			sInfo.idCount == sizeof( size_t ) &&
			sInfo.valueType == sizeof( int ) &&
			sInfo.value == sizeof( double ) &&
			sInfo.bodyId == sizeof( kmb::bodyIdType ) )
		{
			size_t dataNum = static_cast<size_t>( mesh->getDataCount() );
			fwrite( &dataNum, sizeof(size_t), 1, fp);
			std::multimap< std::string, kmb::DataBindings* >::const_iterator dIter = mesh->getDataBindingsMap().begin();
			while( dIter != mesh->getDataBindingsMap().end() ){
				uint4 num = 1;
				fwrite( &num, sizeof(uint4), 1, fp);

				int bmode = static_cast<int>( dIter->second->getBindingMode() );
				fwrite( &bmode, sizeof(int), 1, fp);

				size_t idCount = static_cast<size_t>( dIter->second->getIdCount() );
				fwrite( &idCount, sizeof(size_t), 1, fp);

				uint4 len = static_cast<uint4>( dIter->first.size() );
				fwrite( &len, sizeof(uint4), 1, fp);
				fwrite( dIter->first.c_str(), sizeof(char), len, fp );

				len = static_cast<uint4>( dIter->second->getSpecType().size() );
				fwrite( &len, sizeof(uint4), 1, fp);
				fwrite( dIter->second->getSpecType().c_str(), sizeof(char), len, fp );

				int vtype = static_cast<int>( dIter->second->getValueType() );
				fwrite( &vtype, sizeof(int), 1, fp);

				kmb::bodyIdType bodyId = dIter->second->getTargetBodyId();
				fwrite( &bodyId, sizeof( kmb::bodyIdType ), 1, fp );
				kmb::DataBindings** dataArray = new kmb::DataBindings*[1];
				dataArray[0] = dIter->second;
				saveBindingDataId( dataArray, 1, idCount );
				++dIter;
			}
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::loadBindingDataId(kmb::DataBindings** dataArray,int num,size_t idcount)
{
	size_t res = 0;
	if( num <= 0){
		return 0;
	}
	if( sInfo.value == sizeof( double ) &&
		sInfo.bodyId == sizeof( kmb::bodyIdType ) &&
		sInfo.elementId == sizeof( kmb::elementIdType ) &&
		sInfo.nodeId == sizeof( kmb::nodeIdType ) &&
		sInfo.faceId == sizeof( kmb::idType ) )
	{
		kmb::DataBindings::bindingMode bmode = dataArray[0]->getBindingMode();
		switch( bmode )
		{
		case kmb::DataBindings::BODYVARIABLE:
			{
				kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &bodyId, sizeof(kmb::bodyIdType), 1, fp);
					if( res < 1 ) break;
					for(int j=0;j<num;++j){
						loadValueAtId<kmb::bodyIdType>( reinterpret_cast<kmb::DataBindingsEach<kmb::bodyIdType>*>(dataArray[j]), bodyId );
					}
				}
			}
			break;
		case kmb::DataBindings::ELEMENTVARIABLE:
			{
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &elementId, sizeof(kmb::elementIdType), 1, fp);
					if( res < 1 ) break;
					for(int j=0;j<num;++j){
						loadValueAtId<kmb::elementIdType>( reinterpret_cast<kmb::DataBindingsEach<kmb::elementIdType>*>(dataArray[j]), elementId );
					}
				}
			}
			break;
		case kmb::DataBindings::NODEVARIABLE:
			{
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &nodeId, sizeof(kmb::nodeIdType), 1, fp);
					if( res < 1 ) break;
					for(int j=0;j<num;++j){
						loadValueAtId<kmb::nodeIdType>( reinterpret_cast<kmb::DataBindingsEach<kmb::nodeIdType>*>(dataArray[j]), nodeId );
					}
				}
			}
			break;
		case kmb::DataBindings::FACEVARIABLE:
			{
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				kmb::idType localId = -1;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &elementId, sizeof(kmb::elementIdType), 1, fp);
					if( res < 1 ) break;
					res = fread( &localId, sizeof(kmb::idType), 1, fp);
					if( res < 1 ) break;
					kmb::Face face( elementId, localId );
					for(int j=0;j<num;++j){
						loadValueAtId<kmb::Face>( reinterpret_cast<kmb::DataBindingsEach<kmb::Face>*>(dataArray[j]), face );
					}
				}
			}
			break;
		case kmb::DataBindings::BODYGROUP:
			{
				kmb::bodyIdType bodyId = kmb::Body::nullBodyId;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &bodyId, sizeof(kmb::bodyIdType), 1, fp);
					if( res < 1 ) break;
					for(int j=0;j<num;++j){
						reinterpret_cast< kmb::DataBindingsGroup< kmb::bodyIdType >* >
							( dataArray[j] )->addId( bodyId );
					}
				}
				for(int j=0;j<num;++j){
					loadValue( dataArray[j] );
				}
			}
			break;
		case kmb::DataBindings::ELEMENTGROUP:
			{
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &elementId, sizeof(kmb::elementIdType), 1, fp);
					if( res < 1 ) break;
					for(int j=0;j<num;++j){
						reinterpret_cast< kmb::DataBindingsGroup< kmb::elementIdType >* >
							( dataArray[j] )->addId( elementId );
					}
				}
				for(int j=0;j<num;++j){
					loadValue( dataArray[j] );
				}
			}
			break;
		case kmb::DataBindings::NODEGROUP:
			{
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &nodeId, sizeof(kmb::nodeIdType), 1, fp);
					if( res < 1 ) break;
					for(int j=0;j<num;++j){
						reinterpret_cast< kmb::DataBindingsGroup< kmb::nodeIdType >* >
							( dataArray[j] )->addId( nodeId );
					}
				}
				for(int j=0;j<num;++j){
					loadValue( dataArray[j] );
				}
			}
			break;
		case kmb::DataBindings::FACEGROUP:
			{
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				kmb::idType localId = -1;
				for(unsigned int i=0;i<idcount;++i){
					res = fread( &elementId, sizeof(kmb::elementIdType), 1, fp);
					if( res < 1 ) break;
					res = fread( &localId, sizeof(kmb::idType), 1, fp);
					if( res < 1 ) break;
					for(int j=0;j<num;++j){
						reinterpret_cast< kmb::DataBindingsGroup< kmb::Face >* >
							( dataArray[j] )->addId( kmb::Face(elementId,localId) );
					}
				}
				for(int j=0;j<num;++j){
					loadValue( dataArray[j] );
				}
			}
			break;
		case kmb::DataBindings::GLOBAL:
			for(int j=0;j<num;++j){
				loadValue( dataArray[j] );
			}
			break;
		default:
			break;
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveBindingDataId(kmb::DataBindings** dataArray,int num,size_t idcount)
{
	if( num <= 0 ){
		return 0;
	}
	if( sInfo.value == sizeof( double ) &&
		sInfo.bodyId == sizeof( kmb::bodyIdType ) &&
		sInfo.elementId == sizeof( kmb::elementIdType ) &&
		sInfo.nodeId == sizeof( kmb::nodeIdType ) &&
		sInfo.faceId == sizeof( kmb::idType ) )
	{
		kmb::DataBindings::bindingMode bmode = dataArray[0]->getBindingMode();
		switch( bmode )
		{
		case kmb::DataBindings::BODYVARIABLE:
			{
				kmb::DataBindings::iterator vIter = dataArray[0]->begin();
				kmb::DataBindings::iterator vEnd = dataArray[0]->end();
				while( vIter != vEnd ){
					kmb::bodyIdType bodyId = static_cast<kmb::bodyIdType>( vIter.getId() );
					fwrite( &bodyId, sizeof(kmb::bodyIdType), 1, fp);
					for(int j=0;j<num;++j){
						saveValueAtId<kmb::bodyIdType>( reinterpret_cast<kmb::DataBindingsEach<kmb::bodyIdType>*>(dataArray[j]), bodyId );
					}
					++vIter;
				}
			}
			break;
		case kmb::DataBindings::ELEMENTVARIABLE:
			{
				kmb::DataBindings::iterator vIter = dataArray[0]->begin();
				kmb::DataBindings::iterator vEnd = dataArray[0]->end();
				while( vIter != vEnd ){
					kmb::elementIdType elementId = static_cast<kmb::elementIdType>( vIter.getId() );
					fwrite( &elementId, sizeof(kmb::elementIdType), 1, fp);
					for(int j=0;j<num;++j){
						saveValueAtId<kmb::elementIdType>( reinterpret_cast<kmb::DataBindingsEach<kmb::elementIdType>*>(dataArray[j]), elementId );
					}
					++vIter;
				}
			}
			break;
		case kmb::DataBindings::NODEVARIABLE:
			{
				kmb::DataBindings::iterator vIter = dataArray[0]->begin();
				kmb::DataBindings::iterator vEnd = dataArray[0]->end();
				while( vIter != vEnd ){
					kmb::nodeIdType nodeId = static_cast<kmb::nodeIdType>( vIter.getId() );
					fwrite( &nodeId, sizeof(kmb::nodeIdType), 1, fp);
					for(int j=0;j<num;++j){
						saveValueAtId<kmb::nodeIdType>( reinterpret_cast<kmb::DataBindingsEach<kmb::nodeIdType>*>(dataArray[j]), nodeId );
					}
					++vIter;
				}
			}
			break;
		case kmb::DataBindings::FACEVARIABLE:
			{
				kmb::DataBindings::iterator vIter = dataArray[0]->begin();
				kmb::DataBindings::iterator vEnd = dataArray[0]->end();
				kmb::Face f;
				while( vIter != vEnd ){
					if( vIter.getFace(f) ){
						kmb::elementIdType elementId = f.getElementId();
						kmb::idType localId = f.getLocalFaceId();
						fwrite( &elementId, sizeof(kmb::elementIdType), 1, fp);
						fwrite( &localId, sizeof(kmb::idType), 1, fp);
						for(int j=0;j<num;++j){
							saveValueAtId<kmb::Face>( reinterpret_cast<kmb::DataBindingsEach<kmb::Face>*>(dataArray[j]), f );
						}
					}
					++vIter;
				}
			}
			break;
		case kmb::DataBindings::BODYGROUP:
			{
				kmb::DataBindings::iterator iIter = dataArray[0]->begin();
				kmb::DataBindings::iterator iEnd = dataArray[0]->end();
				while( iIter != iEnd ){
					kmb::bodyIdType bodyId = static_cast<kmb::bodyIdType>( iIter.getId() );
					fwrite( &bodyId, sizeof(kmb::bodyIdType), 1, fp);
					++iIter;
				}
				for(int j=0;j<num;++j){
					saveValue( dataArray[j] );
				}
				break;
			}
			break;
		case kmb::DataBindings::ELEMENTGROUP:
			{
				kmb::DataBindings::iterator iIter = dataArray[0]->begin();
				kmb::DataBindings::iterator iEnd = dataArray[0]->end();
				while( iIter != iEnd ){
					kmb::elementIdType elementId = static_cast<kmb::elementIdType>( iIter.getId() );
					fwrite( &elementId, sizeof(kmb::elementIdType), 1, fp);
					++iIter;
				}
				for(int j=0;j<num;++j){
					saveValue( dataArray[j] );
				}
				break;
			}
			break;
		case kmb::DataBindings::NODEGROUP:
			{
				kmb::DataBindings::iterator iIter = dataArray[0]->begin();
				kmb::DataBindings::iterator iEnd = dataArray[0]->end();
				while( iIter != iEnd ){
					kmb::nodeIdType nodeId = static_cast<kmb::nodeIdType>( iIter.getId() );
					fwrite( &nodeId, sizeof(kmb::nodeIdType), 1, fp);
					++iIter;
				}
				for(int j=0;j<num;++j){
					saveValue( dataArray[j] );
				}
				break;
			}
			break;
		case kmb::DataBindings::FACEGROUP:
			{
				kmb::Face f;
				kmb::DataBindings::iterator iIter = dataArray[0]->begin();
				kmb::DataBindings::iterator iEnd = dataArray[0]->end();
				while( iIter != iEnd ){
					if( iIter.getFace(f) ){
						kmb::elementIdType elementId = f.getElementId();
						kmb::idType localId = f.getLocalFaceId();
						fwrite( &elementId, sizeof(kmb::elementIdType), 1, fp);
						fwrite( &localId, sizeof(kmb::idType), 1, fp);
					}
					++iIter;
				}
				for(int j=0;j<num;++j){
					saveValue( dataArray[j] );
				}
				break;
			}
			break;
		case kmb::DataBindings::GLOBAL:
			for(int j=0;j<num;++j){
				saveValue( dataArray[j] );
			}
			break;
		default:
			break;
		}
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::loadValue(kmb::DataBindings* data)
{
	size_t res = 0;
	switch( data->getValueType() )
	{
	case kmb::PhysicalValue::SCALAR:
		{
			double v;
			res = fread( &v, sizeof(double), 1, fp );
			if( res == 1 ){
				data->setPhysicalValue( new kmb::ScalarValue(v) );
			}
		}
		break;
	case kmb::PhysicalValue::VECTOR3:
		{
			double v[3];
			res = fread( v, sizeof(double), 3, fp );
			if( res == 3 ){
				data->setPhysicalValue( new kmb::Vector3Value(v) );
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveValue(kmb::DataBindings* data)
{
	switch( data->getValueType() )
	{
	case kmb::PhysicalValue::SCALAR:
		{
			double v = reinterpret_cast< kmb::ScalarValue* >( data->getPhysicalValue() )->getValue();
			fwrite( &v, sizeof(double), 1, fp );
		}
		break;
	case kmb::PhysicalValue::VECTOR3:
		{
			kmb::Vector3Value* vData = reinterpret_cast< kmb::Vector3Value* >( data->getPhysicalValue() );
			double v[3];
			v[0] = vData->getValue(0);
			v[1] = vData->getValue(1);
			v[2] = vData->getValue(2);
			fwrite( v, sizeof(double), 3, fp );
		}
		break;
	default:
		break;
	}
	return 0;
}

template<typename T> int
kmb::RevocapMeshDumpIO::loadValueAtId(kmb::DataBindingsEach<T>* data,T t)
{
	size_t res = 0;
	switch( data->getValueType() )
	{
	case kmb::PhysicalValue::SCALAR:
		{
			double v;
			res = fread( &v, sizeof(double), 1, fp );
			if( res == 1 ){
				data->setPhysicalValue(t, new kmb::ScalarValue(v) );
			}
		}
		break;
	case kmb::PhysicalValue::VECTOR3:
		{
			double v[3];
			res = fread( v, sizeof(double), 3, fp );
			if( res == 3 ){
				data->setPhysicalValue(t, new kmb::Vector3Value(v) );
			}
		}
		break;
	case kmb::PhysicalValue::TENSOR6:
		{
			double v[6];
			res = fread( v, sizeof(double), 6, fp );
			if( res == 6 ){
				data->setPhysicalValue(t, new kmb::Tensor6Value(v) );
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

template<typename T> int
kmb::RevocapMeshDumpIO::saveValueAtId(kmb::DataBindingsEach<T>* data,T t)
{
	switch( data->getValueType() )
	{
	case kmb::PhysicalValue::SCALAR:
		{
			double v = 0.0;
			data->getPhysicalValue(t,&v);
			fwrite( &v, sizeof(double), 1, fp );
		}
		break;
	case kmb::PhysicalValue::VECTOR3:
		{
			double v[3];
			data->getPhysicalValue(t,v);
			fwrite( v, sizeof(double), 3, fp );
		}
		break;
	case kmb::PhysicalValue::TENSOR6:
		{
			double v[6];
			data->getPhysicalValue(t,v);
			fwrite( v, sizeof(double), 6, fp );
		}
		break;
	default:
		break;
	}
	return 0;
}

int
kmb::RevocapMeshDumpIO::loadFromFile(const char *filename, kmb::MeshData *mesh )
{
	if( mesh && (fp=fopen(filename,"rb")) == NULL ){
		return -1;
	}

	int version = 0;
	getVersion( version );

	loadNodeData( mesh );

	loadElementData( mesh );

	loadBindingData( mesh );

	fclose(fp);
	fp = NULL;

	return 0;
}

int
kmb::RevocapMeshDumpIO::loadFromFileNeglectingElementId(const char* filename,MeshData* mesh)
{
	if( mesh && (fp=fopen(filename,"rb")) == NULL ){
		return -1;
	}

	int version = 0;
	getVersion( version );

	loadNodeData( mesh );

	loadElementDataNeglectingId( mesh );

	loadBindingData( mesh );

	fclose(fp);
	fp = NULL;
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveToFile(const char *filename, const kmb::MeshData *mesh)
{
	if( mesh && (fp=fopen(filename,"wb")) == NULL ){
		return -1;
	}

	int version = 1;
	setVersion( version );

	saveNodeData( mesh );

	saveElementData( mesh );

	saveBindingData( mesh );

	fclose(fp);
	fp = NULL;
	return 0;
}

int
kmb::RevocapMeshDumpIO::saveToFileNeglectingElementId(const char* filename,const kmb::MeshData* mesh)
{
	if( mesh && (fp=fopen(filename,"wb")) == NULL ){
		return -1;
	}

	int version = 1;
	setVersion( version );

	saveNodeData( mesh );

	saveElementDataNeglectingId( mesh );

	saveBindingData( mesh );

	fclose(fp);
	fp = NULL;
	return 0;
}

