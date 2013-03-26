/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FFbIO                                                   #
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




#include "RevocapIO/kmbFFbIO.h"

#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbScalarValueBindings.h"

#include <string>
#include <iostream>
#include <iomanip>

#ifdef WIN32
 #if _MSC_VER >= 1400
  #define sprintf sprintf_s
  #define strcpy strcpy_s
  #define strcat strcat_s
 #endif
#endif

#include <cstring>

kmb::FFbIO::FFbIO(void)
: FortranIO()
, readPost(false)
, offsetNodeId(1)
, offsetElementId(0)
{
}

kmb::FFbIO::~FFbIO(void)
{
}

bool
kmb::FFbIO::checkFormat(std::ifstream &input)
{

	input.seekg(0,std::ios::beg);
	char buf[80];
 	input.read(buf,80);
	if( strncmp( "#A_GF_V1", buf, 8) == 0 ){
		unformatFlag = false;
	}else if( strncmp( "#U_GF_V1", buf+4, 8) == 0 ){
		unformatFlag = true;

		input.seekg( 0, std::ios_base::beg );
		int size = 0;
		input.read(reinterpret_cast<char*>(&size),sizeof(int));
		if(size!=8){
			endianFlag=true;
		}
	}else{
		return false;
	}
	return true;
}

const char*
kmb::FFbIO::loadHeader(const char* filename)
{
	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	if( input.fail() ){
		return NULL;
	}
	if( !checkFormat( input ) ){
		input.close();
		return NULL;
	}
	input.close();

	std::string line;

	if( unformatFlag ){
		input.open( filename, std::ios_base::in|std::ios_base::binary );
		if( input.fail() ){
			return NULL;
		}
		readString( input, line );
		std::cout << line << std::endl;
		if( line != "#U_GF_V1" ){
			input.close();
			return NULL;
		}
	}else{
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return NULL;
		}
		readString( input, line );
		std::cout << line << std::endl;
		if( line != "#A_GF_V1" ){
			input.close();
			return NULL;
		}
	}

	line.clear();
	readHeader(input,header);
	std::cout << header;
	input.close();
	return header.c_str();
}

int
kmb::FFbIO::loadFromMeshFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}

	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	if( input.fail() ){
		return -1;
	}

	if( !checkFormat( input ) ){
		input.close();
		return -1;
	}
	input.close();

	std::string line;
	int res = 0;

	if( unformatFlag ){
		input.open( filename, std::ios_base::in|std::ios_base::binary );
		if( input.fail() ){
			return -1;
		}
		readString( input, line );

		if( line != "#U_GF_V1" ){
			input.close();
			return -1;
		}
	}else{
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		readString( input, line );

		if( line != "#A_GF_V1" ){
			input.close();
			return -1;
		}
	}

	res = readHeader(input,mesh);
	if( res == -1 ){
		input.close();
		return -1;
	}


	while( !input.eof() ){
		readString( input, line );
		if( line == "#NEW_SET" ){
			readNewSet(input,mesh);
		}else if( line == "#FLT_ARY" ){
			readString( input, line );
			if( line == "*GRID_3D" ){
				readGrid3D(input,mesh);
			}else if( readPost ){
				readHeaderFlt(input,mesh);
			}
		}else if( line == "#INT_ARY" ){
			readString( input, line );
			if( line == "*NODE_2D" ){
				readNode2D(input,mesh);
			}else if( line == "*NODE_3D" ){
				readNode3D(input,mesh);
			}else if( readPost ){
				readHeaderInt(input,mesh);
			}
		}else if( line == "#ENDFILE" ){
			break;
		}
	}

	input.close();
	return 0;
}

int
kmb::FFbIO::loadPostStep(const char* filename,kmb::MeshData* mesh,int step)
{
	if( mesh == NULL ){
		return -1;
	}

	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	if( input.fail() ){
		return -1;
	}

	if( !checkFormat( input ) ){
		input.close();
		return -1;
	}
	input.close();

	std::string line;
	int res = 0;

	if( unformatFlag ){
		input.open( filename, std::ios_base::in|std::ios_base::binary );
		if( input.fail() ){
			return -1;
		}
		readString( input, line );
		std::cout << line << std::endl;
		if( line != "#U_GF_V1" ){
			input.close();
			return -1;
		}
	}else{
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		readString( input, line );
		std::cout << line << std::endl;
		if( line != "#A_GF_V1" ){
			input.close();
			return -1;
		}
	}

	res = readHeader(input,mesh);
	if( res == -1 ){
		input.close();
		return -1;
	}


	while( !input.eof() ){
		readString( input, line );
		if( line == "#NEW_SET" ){
			readNewSet(input,mesh);
		}else if( line == "#FLT_ARY" ){
			readString( input, line );
			if( readPost || step == -1 ){
				if( line == "*PRES_2E" ){
					readPres2E(input,mesh);
					continue;
				}else if( line == "*PRES_3D" ){
					readPres3D(input,mesh);
					continue;
				}else if( line == "*PRES_3E" ){
					readPres3E(input,mesh);
					continue;
				}else if( line == "*VELO_3D" ){
					readVelo3D(input,mesh);
					continue;
				}else if( line == "*VELO_2D" ){
					readVelo2D(input,mesh);
					continue;
				}
			}
			readHeaderFlt(input,mesh);
		}else if( line == "#INT_ARY" ){
			readString( input, line );
			readHeaderInt(input,mesh);

			const kmb::PhysicalValue* v = mesh->getPhysicalValue( "PRESENT STEP" );
			if( v!=NULL && v->getType()==kmb::PhysicalValue::Integer && step==reinterpret_cast<const kmb::IntegerValue*>(v)->getValue() ){
				readPost = true;
			}else if( readPost == true ){
				readPost = false;
				break;
			}else{
				readPost = false;
			}
		}else if( line == "#ENDFILE" ){
			break;
		}
	}

	input.close();
	return 0;
}

int
kmb::FFbIO::loadFromBoundaryFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}

	std::ifstream input( filename, std::ios_base::in|std::ios_base::binary );
	if( input.fail() ){
		return -1;
	}

	if( !checkFormat( input ) ){
		input.close();
		return -1;
	}
	input.close();

	std::string line;
	int res = 0;

	if( unformatFlag ){
		input.open( filename, std::ios_base::in|std::ios_base::binary );
		if( input.fail() ){
			return -1;
		}
		readString( input, line );
		if( line != "#U_GF_V1" ){
			input.close();
			return -1;
		}
	}else{
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		readString( input, line );
		if( line != "#A_GF_V1" ){
			input.close();
			return -1;
		}
	}

	res = readHeader(input,mesh);
	if( res == -1 ){
		input.close();
		return -1;
	}


	while( !input.eof() ){
		readString( input, line );
		if( line == "#NEW_SET" ){
			readNewSet(input,mesh);
		}else if( line == "#FLT_ARY" ){
			readString( input, line );
			std::cout << "[" << line << "]" << std::endl;
			if( line == "*BC_IV3D" ){
				readBC3D( input, mesh , kmb::FFbIO::INLET, "BC_INLT" );
			}else if( line == "*BC_WV3D" ){
				readBC3D( input, mesh , kmb::FFbIO::MOVINGWALL, "BC_MWAL" );
			}else if( line == "*BC_TMPV" ){
				readBCV( input, mesh , kmb::FFbIO::TEMP, "BC_TEMP" );
			}else if( line == "*BC_HFXV" ){
				readBCV( input, mesh , kmb::FFbIO::HEAT, "BC_HFXV" );
			}
		}else if( line == "#INT_ARY" ){
			readString( input, line );
			std::cout << "[" << line << "]" << std::endl;
			if( line == "*BC_INLT" ){
				readBC( input, mesh , kmb::FFbIO::INLET, "BC_INLT" );
			}else if( line == "*BC_FREE" ){
				readBC( input, mesh , kmb::FFbIO::FREE, "BC_FREE" );
			}else if( line == "*BC_WALL" ){
				readBC( input, mesh , kmb::FFbIO::WALL, "BC_WALL" );
			}else if( line == "*BC_MWAL" ){
				readBC( input, mesh , kmb::FFbIO::MOVINGWALL, "BC_MWAL" );
			}else if( line == "*BC_CYCL" ){
				readBC( input, mesh , kmb::FFbIO::CYCLIC, "BC_CYCL" );
			}else if( line == "*BC_SYMT" ){
				readBC( input, mesh , kmb::FFbIO::SYMMETRIC, "BC_SYMT" );
			}else if( line == "*BC_TMPN" ){
				readBC( input, mesh , kmb::FFbIO::TEMP, "BC_TEMP" );
			}else if( line == "*BC_HFXN" ){
				readBC( input, mesh , kmb::FFbIO::HEAT, "BC_HEAT" );
			}else if( line == "*BC_BODY" ){
				readBC( input, mesh , kmb::FFbIO::BODY, "BC_BODY" );
			}else if( line == "*BC_INTR" ){
				readBCINTR( input, mesh , kmb::FFbIO::INTERCONNECT, "BC_INTR" );
			}else if( line.find_first_of("*BC_") == 0 ){
				readBC( input, mesh , kmb::FFbIO::CUSTOM, line.substr(1).c_str() );
			}
		}else if( line == "#ENDFILE" ){
			break;
		}
	}

	input.close();
	return 0;
}

int
kmb::FFbIO::saveToMeshFile(const char* filename,const kmb::MeshData* mesh)
{
	int res = -1;
	if( mesh == NULL ){
		return res;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return res;
	}
	writeHeader( output, mesh );
	writeNewSet( output, mesh );
	writeGrid3D( output, mesh );
	kmb::bodyIdType bodyCount = mesh->getBodyCount();
	for(kmb::bodyIdType i=0;i<bodyCount;++i){
		const kmb::Body* body = mesh->getBodyPtr(i);
		if( body->getDimension() == 3 ){
			res = writeNode3D( output, mesh, i );
		}
	}
	writeFooter( output, mesh );
	output.close();
	return res;
}

int
kmb::FFbIO::saveToMeshFileVer6(const char* filename,const kmb::MeshData* mesh)
{
	int res = -1;
	if( mesh == NULL ){
		return res;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return res;
	}
	writeHeader( output, mesh );
	writeNewSet( output, mesh );
	writeGrid3D( output, mesh );
	kmb::bodyIdType bodyCount = mesh->getBodyCount();
	for(kmb::bodyIdType i=0;i<bodyCount;++i){
		const kmb::Body* body = mesh->getBodyPtr(i);
		if( body->getDimension() == 3 ){
			res = writeNode3DVer6( output, mesh, i );
		}
	}
	writeFooter( output, mesh );
	output.close();
	return res;
}

int
kmb::FFbIO::saveToBoundaryFile(const char* filename,const kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	writeHeader( output, mesh );
	writeNewSet( output, mesh );
	writeBC( output, mesh, kmb::FFbIO::INLET, "BC_INLT" );
	writeBC( output, mesh, kmb::FFbIO::FREE, "BC_FREE" );
	writeBC( output, mesh, kmb::FFbIO::WALL, "BC_WALL" );
	writeBC( output, mesh, kmb::FFbIO::MOVINGWALL, "BC_MWAL" );
	writeBC( output, mesh, kmb::FFbIO::CYCLIC, "BC_CYCL" );
	writeBC( output, mesh, kmb::FFbIO::SYMMETRIC, "BC_SYMT" );
	writeBC( output, mesh, kmb::FFbIO::TEMP, "BC_TEMP" );
	writeBC( output, mesh, kmb::FFbIO::HEAT, "BC_HEAT" );
	writeBC( output, mesh, kmb::FFbIO::BODY, "BC_BODY" );

	const std::multimap< std::string, kmb::DataBindings* > mapper = mesh->getDataBindingsMap();
	std::multimap< std::string, kmb::DataBindings*>::const_iterator dIter = mapper.begin();
	while( dIter != mapper.end() ){
		kmb::DataBindings* binding = dIter->second;
		if( binding != NULL &&
			binding->getSpecType().compare("CUSTOM") == 0 )
		{
			std::string name = dIter->first;
			writeBC( output, mesh, kmb::FFbIO::CUSTOM, name.c_str() );
		}
		++dIter;
	}
	writeFooter( output, mesh );
	output.close();
	return 0;
}

int
kmb::FFbIO::saveToBoundaryFileAcoustic(const char* filename,const kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	writeHeader( output, mesh );
	writeNewSet( output, mesh );
	writeHeaderFlt( output, mesh, "STEP_FR", "FREQUENCY" );
	writeHeaderInt( output, mesh, "FREQ_FR", "FREQUENCY NUMBER" );
	writeBC( output, mesh, kmb::FFbIO::A_WALL, "BCA_WAL" );
	writeBC( output, mesh, kmb::FFbIO::A_NRBC, "BCA_NOR" );
	writeBC( output, mesh, kmb::FFbIO::CYCLIC, "BC_CYCL" );
	writeBC( output, mesh, kmb::FFbIO::SYMMETRIC, "BC_SYMT" );
	writeFooter( output, mesh );
	output.close();
	return 0;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif







int
kmb::FFbIO::readHeader(std::ifstream &input,kmb::MeshData* mesh)
{
	if( unformatFlag ){
		std::string str;
		std::vector<int> intArray;
		readArray<int>(input,intArray);
		int len = intArray[0];
		for(int i=0;i<len;++i){
			readString( input, str );
		}
	}else{
		int len = 0;
		std::string line;
		input >> len;
		if( len < 0 ){
			return -1;
		}
		std::getline( input, line );
		std::cout << line << std::endl;
		for(int i=0;i<len;++i){
			std::getline( input, line );
			std::cout << line << std::endl;
		}
	}
	return 0;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

int
kmb::FFbIO::readHeader(std::ifstream &input,std::string &str)
{
	if( unformatFlag ){
		std::string line;
		std::vector<int> intArray;
		readArray<int>(input,intArray);
		int len = intArray[0];
		for(int i=0;i<len;++i){
			readString( input, line );
			str.append( line );
			str.append( "\n" );
		}
	}else{
		int len = 0;
		std::string line;
		input >> len;
		if( len < 0 ){
			return -1;
		}
		std::getline( input, line );
		for(int i=0;i<len;++i){
			std::getline( input, line );
			str.append( line );
			str.append( "\n" );
		}
	}
	return 0;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

int
kmb::FFbIO::writeHeader(std::ofstream &output,const kmb::MeshData* mesh)
{
	output << "#A_GF_V1" << std::endl;
	output << "           1" << std::endl;
	output << "GF DATA FILE GENERATED BY REVOCAP" << std::endl;
	return 0;
}

int
kmb::FFbIO::writeFooter(std::ofstream &output,const kmb::MeshData* mesh)
{
	output << "#ENDFILE" << std::endl;
	return 0;
}







int
kmb::FFbIO::readNewSet(std::ifstream &input,kmb::MeshData* mesh)
{
	if( unformatFlag ){
		std::string str;
		std::vector<int> intArray;
		readArray<int>(input,intArray);
		int len = intArray[0];
		for(int i=0;i<len;++i){
			readString( input, str );
		}
	}else{
		int len = 0;
		std::string str;
		input >> len;
		readString( input, str );
		if( len < 0 ){
			return -1;
		}
		for(int i=0;i<len;++i){
			readString( input, str );
		}
	}
	return 0;
}

int
kmb::FFbIO::writeNewSet(std::ofstream &output,const kmb::MeshData* mesh)
{
	output << "#NEW_SET" << std::endl;
	output << "           0" << std::endl;
	return 0;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

int
kmb::FFbIO::readGrid3D(std::ifstream &input,kmb::MeshData* mesh)
{
	if( unformatFlag ){
		std::string line;
		readString( input, line );
		std::vector<unsigned int> uintArray;
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		unsigned int nodeCount = uintArray[1];
		if( dim != 3 ){
			return -1;
		}
		std::vector<float> nodeArray;
		nodeArray.reserve( nodeCount * 3 );
		readArray<float>( input, nodeArray );
		mesh->beginNode( nodeCount );
		for(unsigned int i=0;i<nodeCount;++i){
			mesh->addNode( nodeArray[3*i], nodeArray[3*i+1], nodeArray[3*i+2] );
		}
		mesh->endNode();
	}else{
		std::string line;
		std::getline( input, line );
		int dim=0, size=0;
		input >> dim >> size;
		if( dim != 3 ){
			return -1;
		}
		double x, y, z;
		mesh->beginNode( size );
		for(int i=0;i<size;++i){
			input >> x >> y >> z;
			mesh->addNode( x, y, z );
		}
		mesh->endNode();
	}
	return 0;
}

int
kmb::FFbIO::writeGrid3D(std::ofstream &output,const kmb::MeshData* mesh)
{
	output << "#FLT_ARY" << std::endl;
	output << "*GRID_3D" << std::endl;
	output << "GRID COORDINATES (3-D)" << std::endl;
	output.setf( std::ios::right );
	output.width(12);
	output << std::setw(12) << std::right << 3;
	output << std::setw(12) << std::right << mesh->getNodeCount() << std::endl;
	if( mesh->getNodes() == NULL ){
		return 0;
	}
	kmb::Point3DContainer::const_iterator pIter = mesh->getNodes()->begin();
	kmb::Point3DContainer::const_iterator eIter = mesh->getNodes()->end();
	int index = 0;
	double buf[7];
	while( pIter != eIter ){
		pIter.getXYZ( buf[index], buf[index+1], buf[index+2] );
		index += 3;
		if( index >= 5 ){
			for(int i=0;i<5;++i){
				writeFloat( output, static_cast<float>(buf[i]) );
			}
			output << std::endl;
			index -= 5;
			for(int i=0;i<index;++i){
				buf[i] = buf[i+5];
			}
		}
		++pIter;
	}
	for(int i=0;i<index;++i){
		writeFloat( output, static_cast<float>(buf[i]) );
	}
	if( index > 0 ){
		output << std::endl;
	}
	return 0;
}

int
kmb::FFbIO::readNode2D(std::ifstream &input,kmb::MeshData* mesh)
{
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<int> nodeArray;
		readString( input, line );
		readArray<unsigned int>( input, uintArray );
		unsigned int nsize = uintArray[0];
		unsigned int esize = uintArray[1];
		if( nsize == 3 ){
			kmb::nodeIdType nodes[3];
			nodeArray.reserve( esize * nsize );
			readArray<int>( input, nodeArray );
			mesh->beginElement( esize );
			for(size_t i=0;i<esize;++i){
				for(unsigned int j=0;j<nsize;++j){
					nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
				}
				mesh->addElement( kmb::TRIANGLE, nodes );
			}
			mesh->endElement();
		}else if( nsize == 4 ){
			kmb::nodeIdType nodes[4];
			nodeArray.reserve( esize * nsize );
			readArray<int>( input, nodeArray );
			mesh->beginElement( esize );
			for(size_t i=0;i<esize;++i){
				if( nodeArray[nsize*i+3] > 0 ){
					for(unsigned int j=0;j<4;++j){
						nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
					}
					mesh->addElement( kmb::QUAD, nodes );
				}else{
					for(unsigned int j=0;j<3;++j){
						nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
					}
					mesh->addElement( kmb::TRIANGLE, nodes );
				}
			}
			mesh->endElement();
		}
	}else{
		std::string line;
		std::getline( input, line );
		int nsize=0, esize=0;
		input >> nsize >> esize;
		if( nsize == 3 ){
			kmb::nodeIdType nodes[3];
			mesh->beginElement( esize );
			for(int i=0;i<esize;++i){
				input >> nodes[0] >> nodes[1] >> nodes[2];
				for(int j=0;j<3;++j){
					nodes[j] -= offsetNodeId;
				}
				mesh->addElement( kmb::TRIANGLE, nodes );
			}
			mesh->endElement();
		}else if( nsize == 4 ){
			kmb::nodeIdType nodes[4];
			mesh->beginElement( esize );
			for(int i=0;i<esize;++i){
				input >> nodes[0] >> nodes[1] >> nodes[2] >> nodes[3];
				for(int j=0;j<4;++j){
					nodes[j] -= offsetNodeId;
				}
				if( nodes[3] >= 0 ){
					mesh->addElement( kmb::QUAD, nodes );
				}else{
					mesh->addElement( kmb::TRIANGLE, nodes );
				}
			}
			mesh->endElement();
		}
	}
	return 0;
}


int
kmb::FFbIO::readNode3D(std::ifstream &input,kmb::MeshData* mesh)
{
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<int> nodeArray;
		readString( input, line );
		readArray<unsigned int>( input, uintArray );
		unsigned int nsize = uintArray[0];
		unsigned int esize = uintArray[1];
		if( nsize == 4 ){
			kmb::nodeIdType nodes[4];
			nodeArray.reserve( esize * nsize );
			readArray<int>( input, nodeArray );
			mesh->beginElement( esize );
			for(size_t i=0;i<esize;++i){
				for(unsigned int j=0;j<nsize;++j){
					nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
				}
				mesh->addElement( kmb::TETRAHEDRON, nodes );
			}
			mesh->endElement();
		}else if( nsize == 6 ){
			kmb::nodeIdType nodes[6];
			nodeArray.reserve( esize * nsize );
			readArray<int>( input, nodeArray );
			mesh->beginElement( esize );
			for(size_t i=0;i<esize;++i){
				for(unsigned int j=0;j<nsize;++j){
					nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
				}
				mesh->addElement( kmb::WEDGE, nodes );
			}
			mesh->endElement();
		}else if( nsize == 5 ){
			kmb::nodeIdType nodes[5];
			nodeArray.reserve( esize * nsize );
			readArray<int>( input, nodeArray );
			mesh->beginElement( esize );
			for(size_t i=0;i<esize;++i){
				nodes[0] = nodeArray[nsize*i+4] - offsetNodeId;
				nodes[1] = nodeArray[nsize*i+0] - offsetNodeId;
				nodes[2] = nodeArray[nsize*i+1] - offsetNodeId;
				nodes[3] = nodeArray[nsize*i+2] - offsetNodeId;
				nodes[4] = nodeArray[nsize*i+3] - offsetNodeId;
				mesh->addElement( kmb::PYRAMID, nodes );
			}
			mesh->endElement();
		}else if( nsize == 8 ){
			kmb::nodeIdType nodes[8];
			nodeArray.reserve( esize * nsize );
			readArray<int>( input, nodeArray );
			mesh->beginElement( esize );
			for(size_t i=0;i<esize;++i){
				if( nodeArray[nsize*i+6] > 0 ){
					for(unsigned int j=0;j<8;++j){
						nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
					}
					mesh->addElement( kmb::HEXAHEDRON, nodes );
				}else if( nodeArray[nsize*i+5] > 0 ){
					for(unsigned int j=0;j<6;++j){
						nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
					}
					mesh->addElement( kmb::WEDGE, nodes );
				}else if( nodeArray[nsize*i+4] > 0 ){
					nodes[0] = nodeArray[nsize*i+4] - offsetNodeId;
					nodes[1] = nodeArray[nsize*i+0] - offsetNodeId;
					nodes[2] = nodeArray[nsize*i+1] - offsetNodeId;
					nodes[3] = nodeArray[nsize*i+2] - offsetNodeId;
					nodes[4] = nodeArray[nsize*i+3] - offsetNodeId;
					mesh->addElement( kmb::PYRAMID, nodes );
				}else{
					for(unsigned int j=0;j<4;++j){
						nodes[j] = nodeArray[nsize*i+j] - offsetNodeId;
					}
					mesh->addElement( kmb::TETRAHEDRON, nodes );
				}
			}
			mesh->endElement();
		}
	}else{
		std::string line;
		std::getline( input, line );
		int nsize=0, esize=0;
		input >> nsize >> esize;
		if( nsize == 4 ){
			kmb::nodeIdType nodes[4];
			mesh->beginElement( esize );
			for(int i=0;i<esize;++i){
				input >> nodes[0] >> nodes[1] >> nodes[2] >> nodes[3];
				for(int j=0;j<4;++j){
					nodes[j] -= offsetNodeId;
				}
				mesh->addElement( kmb::TETRAHEDRON, nodes );
			}
			mesh->endElement();
		}else if( nsize == 6 ){
			kmb::nodeIdType nodes[6];
			mesh->beginElement( esize );
			for(int i=0;i<esize;++i){
				input >> nodes[0] >> nodes[1] >> nodes[2];
				input >> nodes[3] >> nodes[4] >> nodes[5];
				for(int j=0;j<6;++j){
					nodes[j] -= offsetNodeId;
				}
				mesh->addElement( kmb::WEDGE, nodes );
			}
			mesh->endElement();
		}else if( nsize == 8 ){
			kmb::nodeIdType nodes[8];
			mesh->beginElement( esize );
			for(int i=0;i<esize;++i){
				input >> nodes[0] >> nodes[1] >> nodes[2] >> nodes[3];
				input >> nodes[4] >> nodes[5] >> nodes[6] >> nodes[7];
				for(int j=0;j<8;++j){
					nodes[j] -= offsetNodeId;
				}
				if( nodes[6] >= 0 ){
					mesh->addElement( kmb::HEXAHEDRON, nodes );
				}else if( nodes[5] >= 0 ){
					mesh->addElement( kmb::WEDGE, nodes );
				}else if( nodes[4] >= 0 ){
					nodes[5] = nodes[4];
					nodes[4] = nodes[3];
					nodes[3] = nodes[2];
					nodes[2] = nodes[1];
					nodes[1] = nodes[0];
					nodes[0] = nodes[5];
					mesh->addElement( kmb::PYRAMID, nodes );
				}else{
					mesh->addElement( kmb::TETRAHEDRON, nodes );
				}
			}
			mesh->endElement();
		}
	}
	return 0;
}

int
kmb::FFbIO::writeNode2D(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId)
{
	output << "#INT_ARY" << std::endl;
	output << "*NODE_2D" << std::endl;
	output << "NODE TABLE (2-D)" << std::endl;
	kmb::nodeIdType nodes[15];
	int index = 0;
	output.width(12);
	output.setf( std::ios::right );
	const kmb::Body* body = mesh->getBodyPtr(bodyId);
	if( body ){
		if( body->isUniqueType(kmb::TRIANGLE) ){
			output << std::setw(12) << std::right << 3;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				nodes[index]   = eIter.getCellId(0) + offsetNodeId;
				nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
				nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
				index += 3;
				if( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 0;
		}else if( body->isUniqueType(kmb::QUAD) ){
			output << std::setw(12) << std::right << 4;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				nodes[index]   = eIter.getCellId(0) + offsetNodeId;
				nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
				nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
				nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
				index += 4;
				if( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 0;
		}
	}
	return 0;
}

int
kmb::FFbIO::writeNode3D(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId)
{
	output << "#INT_ARY" << std::endl;
	output << "*NODE_3D" << std::endl;
	output << "NODE TABLE (3-D)" << std::endl;
	kmb::nodeIdType nodes[15];
	int index = 0;
	output.width(12);
	output.setf( std::ios::right );
	const kmb::Body* body = mesh->getBodyPtr(bodyId);
	if( body ){
		if( body->isUniqueType(kmb::TETRAHEDRON) ){
			output << std::setw(12) << std::right << 4;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				nodes[index]   = eIter.getCellId(0) + offsetNodeId;
				nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
				nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
				nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
				index += 4;
				while( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 4;
		}else if( body->isUniqueType(kmb::PYRAMID) ){
			output << std::setw(12) << std::right << 5;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				nodes[index]   = eIter.getCellId(1) + offsetNodeId;
				nodes[index+1] = eIter.getCellId(2) + offsetNodeId;
				nodes[index+2] = eIter.getCellId(3) + offsetNodeId;
				nodes[index+3] = eIter.getCellId(4) + offsetNodeId;
				nodes[index+4] = eIter.getCellId(0) + offsetNodeId;
				index += 5;
				while( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 8;
		}else if( body->isUniqueType(kmb::WEDGE) ){
			output << std::setw(12) << std::right << 6;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				nodes[index]   = eIter.getCellId(0) + offsetNodeId;
				nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
				nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
				nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
				nodes[index+4] = eIter.getCellId(4) + offsetNodeId;
				nodes[index+5] = eIter.getCellId(5) + offsetNodeId;
				index += 6;
				while( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 8;
		}else if( body->isUniqueType(kmb::HEXAHEDRON) ){
			output << std::setw(12) << std::right << 8;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				nodes[index]   = eIter.getCellId(0) + offsetNodeId;
				nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
				nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
				nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
				nodes[index+4] = eIter.getCellId(4) + offsetNodeId;
				nodes[index+5] = eIter.getCellId(5) + offsetNodeId;
				nodes[index+6] = eIter.getCellId(6) + offsetNodeId;
				nodes[index+7] = eIter.getCellId(7) + offsetNodeId;
				index += 8;
				while( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 6;
		}else{
			output << std::setw(12) << std::right << 8;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				switch( eIter.getType() ){
				case kmb::TETRAHEDRON:
					nodes[index]   = eIter.getCellId(0) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
					nodes[index+4] = 0;
					nodes[index+5] = 0;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				case kmb::PYRAMID:
					nodes[index]   = eIter.getCellId(1) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(3) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(4) + offsetNodeId;
					nodes[index+4] = eIter.getCellId(0) + offsetNodeId;
					nodes[index+5] = 0;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				case kmb::WEDGE:
					nodes[index]   = eIter.getCellId(0) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
					nodes[index+4] = eIter.getCellId(4) + offsetNodeId;
					nodes[index+5] = eIter.getCellId(5) + offsetNodeId;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				case kmb::HEXAHEDRON:
					nodes[index]   = eIter.getCellId(0) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(3) + offsetNodeId;

					nodes[index+4] = eIter.getCellId(4) + offsetNodeId;
					nodes[index+5] = eIter.getCellId(5) + offsetNodeId;
					nodes[index+6] = eIter.getCellId(6) + offsetNodeId;
					nodes[index+7] = eIter.getCellId(7) + offsetNodeId;
					break;
				default:
					nodes[index]   = 0;
					nodes[index+1] = 0;
					nodes[index+2] = 0;
					nodes[index+3] = 0;
					nodes[index+4] = 0;
					nodes[index+5] = 0;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				}
				index += 8;
				while( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 6;
		}
	}
	return 0;
}

int
kmb::FFbIO::writeNode2DVer6(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId)
{
	output << "#INT_ARY" << std::endl;
	output << "*NODE_2D" << std::endl;
	output << "NODE TABLE (2-D)" << std::endl;
	kmb::nodeIdType nodes[15];
	int index = 0;
	output.width(12);
	output.setf( std::ios::right );
	const kmb::Body* body = mesh->getBodyPtr(bodyId);
	if( body ){
		if( body->getDimension() == 2 ){
			output << std::setw(12) << std::right << 4;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				switch( eIter.getType() ){
				case kmb::TRIANGLE:
					{
						nodes[index]   = eIter.getCellId(0) + offsetNodeId;
						nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
						nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
						nodes[index+3] = 0;
					}
					break;
				case kmb::QUAD:
					{
						nodes[index]   = eIter.getCellId(0) + offsetNodeId;
						nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
						nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
						nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
					}
					break;
				default:
					break;
				}
				index += 4;
				if( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 4;
		}
	}
	return 0;
}

int
kmb::FFbIO::writeNode3DVer6(std::ofstream &output,const kmb::MeshData* mesh,kmb::bodyIdType bodyId)
{
	output << "#INT_ARY" << std::endl;
	output << "*NODE_3D" << std::endl;
	output << "NODE TABLE (3-D)" << std::endl;
	kmb::nodeIdType nodes[15];
	int index = 0;
	output.width(12);
	output.setf( std::ios::right );
	const kmb::Body* body = mesh->getBodyPtr(bodyId);
	if( body ){
		if( body->getDimension() == 3 ){
			output << std::setw(12) << std::right << 8;
			output << std::setw(12) << body->getCount() << std::endl;
			kmb::ElementContainer::const_iterator eIter = body->begin();
			while( !eIter.isFinished() ){
				switch( eIter.getType() ){
				case kmb::TETRAHEDRON:
					nodes[index]   = eIter.getCellId(0) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
					nodes[index+4] = 0;
					nodes[index+5] = 0;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				case kmb::PYRAMID:
					nodes[index]   = eIter.getCellId(1) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(3) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(4) + offsetNodeId;
					nodes[index+4] = eIter.getCellId(0) + offsetNodeId;
					nodes[index+5] = 0;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				case kmb::WEDGE:
					nodes[index]   = eIter.getCellId(0) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
					nodes[index+4] = eIter.getCellId(4) + offsetNodeId;
					nodes[index+5] = eIter.getCellId(5) + offsetNodeId;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				case kmb::HEXAHEDRON:
					nodes[index]   = eIter.getCellId(0) + offsetNodeId;
					nodes[index+1] = eIter.getCellId(1) + offsetNodeId;
					nodes[index+2] = eIter.getCellId(2) + offsetNodeId;
					nodes[index+3] = eIter.getCellId(3) + offsetNodeId;
					nodes[index+4] = eIter.getCellId(4) + offsetNodeId;
					nodes[index+5] = eIter.getCellId(5) + offsetNodeId;
					nodes[index+6] = eIter.getCellId(6) + offsetNodeId;
					nodes[index+7] = eIter.getCellId(7) + offsetNodeId;
					break;
				default:
					nodes[index]   = 0;
					nodes[index+1] = 0;
					nodes[index+2] = 0;
					nodes[index+3] = 0;
					nodes[index+4] = 0;
					nodes[index+5] = 0;
					nodes[index+6] = 0;
					nodes[index+7] = 0;
					break;
				}
				index += 8;
				while( index >= 6 ){
					for(int i=0;i<6;++i){
						output << std::setw(12) << std::right << nodes[i];
					}
					output << std::endl;
					index -= 6;
					for(int i=0;i<index;++i){
						nodes[i] = nodes[i+6];
					}
				}
				++eIter;
			}
			for(int i=0;i<index;++i){
				output << std::setw(12) << std::right << nodes[i];
			}
			if( index > 0 ){
				output << std::endl;
			}
			return 6;
		}
	}
	return 0;
}

int
kmb::FFbIO::readHeaderFlt(std::ifstream &input,kmb::MeshData* mesh)
{
	if( unformatFlag ){
		std::string line;
		std::vector<int> intArray;
		std::vector<float> floatArray;
		readString( input, line );
		readArray<int>( input, intArray );

		if( intArray[0] == 1 && intArray[1] == 1 ){
			strip(line);
			mesh->createDataBindings( line.c_str(), kmb::DataBindings::Global, kmb::PhysicalValue::Scalar );
			readArray<float>( input, floatArray );
			mesh->setPhysicalValue( line.c_str(), new kmb::ScalarValue( floatArray[0] ) );
		}
	}else{
		std::string line,dummy;
		std::getline( input, line );

		strip(line);
		int dim=0, size=0;
		input >> dim >> size;
		std::getline( input, dummy );

		if( dim == 1 && size == 1 ){
			strip(line);
			mesh->createDataBindings( line.c_str(), kmb::DataBindings::Global, kmb::PhysicalValue::Scalar );
			double val = 0.0;
			input >> val;
			mesh->setPhysicalValue( line.c_str(), new kmb::ScalarValue( val ) );
		}
	}
	return 0;
}

int
kmb::FFbIO::readHeaderInt(std::ifstream &input,kmb::MeshData* mesh)
{
	if( unformatFlag ){
		std::string line;
		std::vector<int> intArray;
		readString( input, line );
		readArray<int>( input, intArray );

		if( intArray[0] == 1 && intArray[1] == 1 ){
			strip(line);
			mesh->createDataBindings( line.c_str(), kmb::DataBindings::Global, kmb::PhysicalValue::Integer );
			readArray<int>( input, intArray );
			mesh->setPhysicalValue( line.c_str(), new kmb::IntegerValue( intArray[0] ) );
		}
	}else{
		std::string line,dummy;
		std::getline( input, line );

		strip(line);




		int dim=0, size=0;
		input >> dim >> size;
		std::getline( input, dummy );

		if( dim == 1 && size == 1 ){
			strip(line);
			mesh->createDataBindings( line.c_str(), kmb::DataBindings::Global, kmb::PhysicalValue::Integer );
			long val = 0;
			input >> val;
			mesh->setPhysicalValue( line.c_str(), new kmb::IntegerValue( val ) );
		}
	}
	return 0;
}

int
kmb::FFbIO::writeHeaderFlt(std::ofstream &output,const kmb::MeshData* mesh,const char* name,const char* comment)
{
	const kmb::DataBindings* data = mesh->getDataBindingsPtr(name);
	if( data==NULL ||
		data->getBindingMode() != kmb::DataBindings::Global ||
		data->getValueType() != kmb::PhysicalValue::Scalar )
	{
		return -1;
	}
	double val = static_cast< kmb::ScalarValue* >( data->getPhysicalValue() )->getValue();
	output << "#FLT_ARY" << std::endl;
	output << "*" << name << std::endl;
	output << std::setw(12) << std::right << 1 << std::setw(12) << std::right << 1 << std::endl;
	output << std::setw(12) << std::right << std::showpoint << std::setprecision(6) << val << std::endl;
	return 0;
}

int
kmb::FFbIO::writeHeaderInt(std::ofstream &output,const kmb::MeshData* mesh,const char* name,const char* comment)
{
	const kmb::DataBindings* data = mesh->getDataBindingsPtr(name);
	if( data==NULL ||
		data->getBindingMode() != kmb::DataBindings::Global ||
		data->getValueType() != kmb::PhysicalValue::Integer )
	{
		return -1;
	}
	int val = static_cast< kmb::IntegerValue* >( data->getPhysicalValue() )->getValue();
	output << "#INT_ARY" << std::endl;
	output << "*" << name << std::endl;
	output << std::setw(12) << std::right << 1 << std::setw(12) << std::right << 1 << std::endl;
	output << std::setw(12) << std::right << val << std::endl;
	return 0;
}

int
kmb::FFbIO::readPres2E(std::ifstream &input,kmb::MeshData* mesh)
{
	kmb::DataBindings* data = NULL;
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<float> floatArray;
		readString( input, line );
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		size_t eCount = mesh->getElementCountOfDim(3);
		int size = ( eCount < uintArray[1] ) ? static_cast<int>(eCount) : static_cast<int>(uintArray[1]);
		if( dim == 1 ){
			readArray<float>( input, floatArray );
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
			}
			double val;
			for(int i=0;i<size;++i){
				val = floatArray[i];
				data->setPhysicalValue( i, &val );
			}
		}
	}else{
		std::string line,dummy;
		std::getline( input, line );
		strip( line );




		long dim=0, size=0;
		input >> dim >> size;
		long eCount = static_cast<long>(mesh->getElementCountOfDim(3));
		std::getline( input, dummy );
		if( dim == 1 ){
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
			}
			double val;
			for(long i=0;i<size;++i){
				input >> val;
				if( i < eCount ){
					data->setPhysicalValue( i, &val );
				}
			}
		}
	}
	return 0;
}

int
kmb::FFbIO::readPres3E(std::ifstream &input,kmb::MeshData* mesh)
{
	kmb::DataBindings* data = NULL;
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<float> floatArray;
		readString( input, line );
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		size_t eCount = mesh->getElementCountOfDim(3);
		int size = ( eCount < uintArray[1] ) ? static_cast<int>(eCount) : static_cast<int>(uintArray[1]);
		if( dim == 1 ){
			readArray<float>( input, floatArray );
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
			}
			double val;
			for(int i=0;i<size;++i){
				val = floatArray[i];
				data->setPhysicalValue( i, &val );
			}
		}
	}else{
		std::string line,dummy;
		std::getline( input, line );
		strip( line );




		long dim=0, size=0;
		input >> dim >> size;
		long eCount = static_cast<long>(mesh->getElementCountOfDim(3));
		std::getline( input, dummy );
		if( dim == 1 ){
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::ElementVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::ElementVariable, kmb::PhysicalValue::Scalar, "post" );
			}
			double val;
			for(long i=0;i<size;++i){
				input >> val;
				if( i < eCount ){
					data->setPhysicalValue( i, &val );
				}
			}
		}
	}
	return 0;
}

int
kmb::FFbIO::readPres3D(std::ifstream &input,kmb::MeshData* mesh)
{
	kmb::DataBindings* data = NULL;
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<float> floatArray;
		readString( input, line );
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		size_t nCount = mesh->getNodeCount();
		int size = ( nCount < uintArray[1] ) ? static_cast<int>(nCount) : static_cast<int>(uintArray[1]);
		if( dim == 1 ){
			readArray<float>( input, floatArray );
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::NodeVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
			}
			double val;
			for(int i=0;i<size;++i){
				val = floatArray[i];
				data->setPhysicalValue( i, &val );
			}
		}
	}else{
		std::string line,dummy;
		std::getline( input, line );
		strip( line );




		long dim=0, size=0;
		input >> dim >> size;
		long nCount = static_cast<long>(mesh->getNodeCount());
		std::getline( input, dummy );
		if( dim == 1 ){
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::NodeVariable ||
				data->getValueType() != kmb::PhysicalValue::Scalar ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar, "post" );
			}
			double val;
			for(long i=0;i<size;++i){
				input >> val;
				if( i < nCount ){
					data->setPhysicalValue( i, &val );
				}
			}
		}
	}
	return 0;
}

int
kmb::FFbIO::readVelo3D(std::ifstream &input,kmb::MeshData* mesh)
{
	kmb::DataBindings* data = NULL;
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<float> floatArray;
		readString( input, line );
		std::cout << line << std::endl;
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		size_t nCount = mesh->getNodeCount();
		int size = ( nCount < uintArray[1] ) ? static_cast<int>(nCount) : static_cast<int>(uintArray[1]);
		if( dim == 3 ){
			readArray<float>( input, floatArray );
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::NodeVariable ||
				data->getValueType() != kmb::PhysicalValue::Vector3 ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3, "post" );
			}
			double val[3];
			for(int i=0;i<size;++i){
				val[0] = floatArray[3*i];
				val[1] = floatArray[3*i+1];
				val[2] = floatArray[3*i+2];
				data->setPhysicalValue( i, val );
			}
		}
	}else{
		std::string line,dummy;
		std::getline( input, line );
		std::cout << line << std::endl;
		strip( line );




		long dim=0, size=0;
		input >> dim >> size;
		long nCount = static_cast<long>(mesh->getNodeCount());
		std::getline( input, dummy );
		if( dim == 3 ){
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::NodeVariable ||
				data->getValueType() != kmb::PhysicalValue::Vector3 ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3, "post" );
			}
			double val[3];
			for(long i=0;i<size;++i){
				input >> val[0];
				input >> val[1];
				input >> val[2];
				if( i < nCount ){
					data->setPhysicalValue( i, val );
				}
			}
		}
	}
	return 0;
}

int
kmb::FFbIO::readVelo2D(std::ifstream &input,kmb::MeshData* mesh)
{
	kmb::DataBindings* data = NULL;
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<float> floatArray;
		readString( input, line );
		std::cout << line << std::endl;
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		size_t nCount = mesh->getNodeCount();
		int size = ( nCount < uintArray[1] ) ? static_cast<int>(nCount) : static_cast<int>(uintArray[1]);
		if( dim == 2 ){
			readArray<float>( input, floatArray );
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::NodeVariable ||
				data->getValueType() != kmb::PhysicalValue::Vector2 ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector2, "post" );
			}
			double val[2];
			for(int i=0;i<size;++i){
				val[0] = floatArray[2*i];
				val[1] = floatArray[2*i+1];
				data->setPhysicalValue( i, val );
			}
		}
	}else{
		std::string line,dummy;
		std::getline( input, line );
		std::cout << line << std::endl;
		strip( line );




		long dim=0, size=0;
		input >> dim >> size;
		long nCount = static_cast<long>(mesh->getNodeCount());
		std::getline( input, dummy );
		if( dim == 2 ){
			strip(line);
			data = mesh->getDataBindingsPtr( line.c_str(), "post" );
			if( data && (
				data->getBindingMode() != kmb::DataBindings::NodeVariable ||
				data->getValueType() != kmb::PhysicalValue::Vector2 ) )
			{
				mesh->deleteData( line.c_str(), "post" );
				data = NULL;
			}
			if( data == NULL ){
				data = mesh->createDataBindings( line.c_str(), kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector2, "post" );
			}
			double val[2];
			for(long i=0;i<size;++i){
				input >> val[0];
				input >> val[1];
				if( i < nCount ){
					data->setPhysicalValue( i, val );
				}
			}
		}
	}
	return 0;
}


int
kmb::FFbIO::writeBC(std::ofstream &output,const kmb::MeshData* mesh,boundaryType btype,const char* name)
{
	const kmb::DataBindings* data = mesh->getDataBindingsPtr(name);
	if( data==NULL ||
		( data->getBindingMode() != kmb::DataBindings::NodeVariable && data->getBindingMode() != kmb::DataBindings::NodeGroup ) ||
		data->getIdCount() == 0 )
	{
		return -1;
	}
	{
		switch( btype ){
		case kmb::FFbIO::INLET:
			output << "#INT_ARY" << std::endl;
			output << "*BC_INLT" << std::endl;
			output << " INLET BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::FREE:
			output << "#INT_ARY" << std::endl;
			output << "*BC_FREE" << std::endl;
			output << " FREE BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::WALL:
			output << "#INT_ARY" << std::endl;
			output << "*BC_WALL" << std::endl;
			output << " WALL BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::MOVINGWALL:
			output << "#INT_ARY" << std::endl;
			output << "*BC_MWAL" << std::endl;
			output << " MOVING WALL BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::SYMMETRIC:
			output << "#INT_ARY" << std::endl;
			output << "*BC_SYMT" << std::endl;
			output << " SYMMETRIC BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::CYCLIC:
			output << "#INT_ARY" << std::endl;
			output << "*BC_CYCL" << std::endl;
			output << " CYCLIC BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::TEMP:
			output << "#INT_ARY" << std::endl;
			output << "*BC_TMPN" << std::endl;
			output << " TEMPERATURE BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::HEAT:
			output << "#INT_ARY" << std::endl;
			output << "*BC_HFXN" << std::endl;
			output << " HEAT FLUX BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::BODY:
			output << "#INT_ARY" << std::endl;
			output << "*BC_BODY" << std::endl;
			output << " FLUID BODY BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::CUSTOM:
			output << "#INT_ARY" << std::endl;
			output << "*" << name << std::endl;
			output << " CUSTOM BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::A_WALL:
			output << "#INT_ARY" << std::endl;
			output << "*BCA_WAL" << std::endl;
			output << " REFLECTING WALL BOUNDARY NODES" << std::endl;
			break;
		case kmb::FFbIO::A_NRBC:
			output << "#INT_ARY" << std::endl;
			output << "*BCA_NOR" << std::endl;
			output << " NO REFLECTING WALL BOUNDARY NODES" << std::endl;
			break;
		default:
			return -1;
		}
		output.width(12);
		output.setf( std::ios::right );
		kmb::nodeIdType nodes[6];
		int index = 0;
		output << std::setw(12) << std::right << 1;
		output << std::setw(12) << std::right << data->getIdCount() << std::endl;
		kmb::DataBindings::const_iterator dIter = data->begin();
		kmb::DataBindings::const_iterator eIter = data->end();
		while( dIter != eIter ){
			nodes[index] = dIter.getId();
			++index;
			if( index == 6 ){
				for(int i=0;i<6;++i){
					output << std::setw(12) << std::right << nodes[i] + offsetNodeId;
				}
				output << std::endl;
				index = 0;
			}
			++dIter;
		}
		for(int i=0;i<index;++i){
			output << std::setw(12) << std::right << nodes[i] + offsetNodeId;
		}
		if( index > 0 ){
			output << std::endl;
		}
	}
	if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
		data->getValueType() == kmb::PhysicalValue::Vector3 )
	{
		switch( btype ){
		case kmb::FFbIO::INLET:
			output << "#FLT_ARY" << std::endl;
			output << "*BC_IV3D" << std::endl;
			output << " INLET BOUNDARY VELOCITIES 3-D" << std::endl;
			break;
		case kmb::FFbIO::MOVINGWALL:
			output << "#FLT_ARY" << std::endl;
			output << "*BC_MV3D" << std::endl;
			output << " MOVING WALL BOUNDARY VELOCITIES 3-D" << std::endl;
			break;
		default:
			break;
		}
		int index = 0;
		double buf[7];
		output << std::setw(12) << std::right << 3;
		output << std::setw(12) << std::right << data->getIdCount() << std::endl;
		kmb::DataBindings::const_iterator dIter = data->begin();
		kmb::DataBindings::const_iterator eIter = data->end();
		while( dIter != eIter ){
			dIter.getValue( &(buf[index]) );
			index += 3;
			while( index >= 5 ){
				for(int i=0;i<5;++i){
					writeFloat( output, static_cast<float>(buf[i]) );
				}
				output << std::endl;
				index -= 5;
				for(int i=0;i<index;++i){
					buf[i] = buf[i+5];
				}
			}
			++dIter;
		}
		for(int i=0;i<index;++i){
			writeFloat( output, static_cast<float>(buf[i]) );
		}
		if( index > 0 ){
			output << std::endl;
		}
	}
	if( data->getBindingMode() == kmb::DataBindings::NodeVariable &&
		data->getValueType() == kmb::PhysicalValue::Scalar )
	{
		switch( btype ){
		case kmb::FFbIO::TEMP:
			output << "#FLT_ARY" << std::endl;
			output << "*BC_TMPV" << std::endl;
			output << " TEMPERATURE BOUNDARY VALUES" << std::endl;
			break;
		case kmb::FFbIO::HEAT:
			output << "#FLT_ARY" << std::endl;
			output << "*BC_HFXV" << std::endl;
			output << " HEAT FLUX BOUNDARY VALUES" << std::endl;
			break;
		default:
			break;
		}
		int index = 0;
		double buf[7];
		output << std::setw(12) << std::right << 1;
		output << std::setw(12) << std::right << data->getIdCount() << std::endl;
		kmb::DataBindings::const_iterator dIter = data->begin();
		kmb::DataBindings::const_iterator eIter = data->end();
		while( dIter != eIter ){
			dIter.getValue( &(buf[index]) );
			++index;
			while( index >= 5 ){
				for(int i=0;i<5;++i){
					writeFloat( output, static_cast<float>(buf[i]) );
				}
				output << std::endl;
				index -= 5;
				for(int i=0;i<index;++i){
					buf[i] = buf[i+5];
				}
			}
			++dIter;
		}
		for(int i=0;i<index;++i){
			writeFloat( output, static_cast<float>(buf[i]) );
		}
		if( index > 0 ){
			output << std::endl;
		}
	}
	return 0;
}

int
kmb::FFbIO::readBC(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name)
{
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<int> nodeArray;
		readString( input, line );
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		unsigned int size = uintArray[1];
		if( dim != 1 ){
			return -1;
		}
		switch( btype ){
			case kmb::FFbIO::INLET:
				inlet.clear();
				inlet.reserve(size);
				{
					nodeArray.reserve( size );
					readArray<int>( input, nodeArray );
					for(unsigned int i=0;i<size;++i){
						inlet.push_back(nodeArray[i]-offsetNodeId);
					}
				}
				break;
			case kmb::FFbIO::MOVINGWALL:
				movingWall.clear();
				movingWall.reserve(size);
				{
					nodeArray.reserve( size );
					readArray<int>( input, nodeArray );
					for(unsigned int i=0;i<size;++i){
						movingWall.push_back(nodeArray[i]-offsetNodeId);
					}
				}
				break;
			case kmb::FFbIO::TEMP:
				temp.clear();
				temp.reserve(size);
				{
					nodeArray.reserve( size );
					readArray<int>( input, nodeArray );
					for(unsigned int i=0;i<size;++i){
						temp.push_back(nodeArray[i]-offsetNodeId);
					}
				}
				break;
			case kmb::FFbIO::HEAT:
				heat.clear();
				heat.reserve(size);
				{
					nodeArray.reserve( size );
					readArray<int>( input, nodeArray );
					for(unsigned int i=0;i<size;++i){
						heat.push_back(nodeArray[i]-offsetNodeId);
					}
				}
				break;
			default:
				{
					mesh->createDataBindings( name, kmb::DataBindings::NodeGroup, kmb::PhysicalValue::None );
					nodeArray.reserve( size );
					readArray<int>( input, nodeArray );
					for(unsigned int i=0;i<size;++i){
						mesh->addId(name,nodeArray[i]-offsetNodeId);
					}
				}
				break;
		}
	}else{
		std::string line;
		std::getline( input, line );
		int dim=0, size=0;
		input >> dim >> size;
		if( dim != 1 ){
			return -1;
		}
		switch( btype ){
			case kmb::FFbIO::INLET:
				inlet.clear();
				inlet.reserve(size);
				{
					kmb::nodeIdType nodeId;
					for(int i=0;i<size;++i){
						input >> nodeId;
						inlet.push_back(nodeId-offsetNodeId);
					}
				}
				break;
			case kmb::FFbIO::MOVINGWALL:
				movingWall.clear();
				movingWall.reserve(size);
				{
					kmb::nodeIdType nodeId;
					for(int i=0;i<size;++i){
						input >> nodeId;
						movingWall.push_back(nodeId-offsetNodeId);
					}
				}
				break;
			case kmb::FFbIO::TEMP:
				temp.clear();
				temp.reserve(size);
				{
					kmb::nodeIdType nodeId;
					for(int i=0;i<size;++i){
						input >> nodeId;
						temp.push_back(nodeId-offsetNodeId);
					}
				}
				break;
			case kmb::FFbIO::HEAT:
				heat.clear();
				heat.reserve(size);
				{
					kmb::nodeIdType nodeId;
					for(int i=0;i<size;++i){
						input >> nodeId;
						heat.push_back(nodeId-offsetNodeId);
					}
				}
				break;
			default:
				{
					mesh->createDataBindings( name, kmb::DataBindings::NodeGroup, kmb::PhysicalValue::None );
					kmb::nodeIdType nodeId;
					for(int i=0;i<size;++i){
						input >> nodeId;
						mesh->addId(name,nodeId-offsetNodeId);
					}
				}
				break;
		}
	}
	return 0;
}



int
kmb::FFbIO::readBCV(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name)
{
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<float> floatArray;
		readString( input, line );
		std::cout << line << std::endl;
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		unsigned int size = uintArray[1];
		if( dim != 1 ){
			return -1;
		}
		double val = 0.0;
		switch( btype ){
			case kmb::FFbIO::TEMP:
				if( temp.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar );
				mesh->appendTargetData( name );
				readArray<float>( input, floatArray );
				for(unsigned int i=0;i<size;++i){
					val = floatArray[i];
					mesh->setMultiPhysicalValues( temp[i], &val );
				}
				mesh->clearTargetData();
				inlet.clear();
				break;
			case kmb::FFbIO::HEAT:
				if( heat.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar );
				mesh->appendTargetData( name );
				readArray<float>( input, floatArray );
				for(unsigned int i=0;i<size;++i){
					val = floatArray[i];
					mesh->setMultiPhysicalValues( heat[i], &val );
				}
				mesh->clearTargetData();
				movingWall.clear();
				break;
			default:
				break;
		}
	}else{
		std::string line;
		std::getline( input, line );
		unsigned int dim=0, size=0;
		input >> dim >> size;
		if( dim != 1 ){
			return -1;
		}
		double val = 0.0;
		switch( btype ){
			case kmb::FFbIO::TEMP:
				if( temp.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar );
				mesh->appendTargetData( name );
				for(unsigned int i=0;i<size;++i){
					input >> val;
					mesh->setMultiPhysicalValues( temp[i], &val );
				}
				mesh->clearTargetData();
				break;
			case kmb::FFbIO::HEAT:
				if( heat.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Scalar );
				mesh->appendTargetData( name );
				for(unsigned int i=0;i<size;++i){
					input >> val;
					mesh->setMultiPhysicalValues( heat[i], &val );
				}
				mesh->clearTargetData();
				break;
			default:
				break;
		}
	}
	return 0;
}



int
kmb::FFbIO::readBC3D(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name)
{
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<float> floatArray;
		readString( input, line );
		std::cout << line << std::endl;
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		unsigned int size = uintArray[1];
		if( dim != 3 ){
			return -1;
		}
		double val[3];
		switch( btype ){
			case kmb::FFbIO::INLET:
				if( inlet.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3 );
				mesh->appendTargetData( name );
				readArray<float>( input, floatArray );
				for(unsigned int i=0;i<size;++i){
					val[0] = floatArray[3*i];
					val[1] = floatArray[3*i+1];
					val[2] = floatArray[3*i+2];
					mesh->setMultiPhysicalValues( inlet[i], val );
				}
				mesh->clearTargetData();
				inlet.clear();
				break;
			case kmb::FFbIO::MOVINGWALL:
				if( movingWall.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3 );
				mesh->appendTargetData( name );
				readArray<float>( input, floatArray );
				for(unsigned int i=0;i<size;++i){
					val[0] = floatArray[3*i];
					val[1] = floatArray[3*i+1];
					val[2] = floatArray[3*i+2];
					mesh->setMultiPhysicalValues( movingWall[i], val );
				}
				mesh->clearTargetData();
				movingWall.clear();
				break;
			default:
				break;
		}
	}else{
		std::string line;
		std::getline( input, line );
		unsigned int dim=0, size=0;
		input >> dim >> size;
		if( dim != 3 ){
			return -1;
		}
		double val[3];
		switch( btype ){
			case kmb::FFbIO::INLET:
				if( inlet.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3 );
				mesh->appendTargetData( name );
				for(unsigned int i=0;i<size;++i){
					input >> val[0];

					input >> val[1];
					input >> val[2];
					mesh->setMultiPhysicalValues( inlet[i], val );
				}
				mesh->clearTargetData();
				break;
			case kmb::FFbIO::MOVINGWALL:
				if( movingWall.size() != size ){
					return -1;
				}
				mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Vector3 );
				mesh->appendTargetData( name );
				for(unsigned int i=0;i<size;++i){
					input >> val[0];
					input >> val[1];
					input >> val[2];
					mesh->setMultiPhysicalValues( movingWall[i], val );
				}
				mesh->clearTargetData();
				break;
			default:
				break;
		}
	}
	return 0;
}

int
kmb::FFbIO::readBCINTR(std::ifstream &input,kmb::MeshData* mesh,boundaryType btype,const char* name)
{
	if( unformatFlag ){
		std::string line;
		std::vector<unsigned int> uintArray;
		std::vector<int> intArray;
		readString( input, line );
		std::cout << line << std::endl;
		readArray<unsigned int>( input, uintArray );
		unsigned int dim = uintArray[0];
		int size = static_cast<int>( uintArray[1] );
		if( dim != 3 ){
			return -1;
		}
		switch( btype ){
			case kmb::FFbIO::INTERCONNECT:
				{
					kmb::DataBindings* data = mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Array );
					mesh->appendTargetData( name );
					readArray<int>( input, intArray );
					for(int i=0;i<size;++i){
						kmb::ArrayValue* ary = new kmb::ArrayValue();
						ary->appendValue( new kmb::IntegerValue(intArray[3*i+1]) );
						ary->appendValue( new kmb::IntegerValue(intArray[3*i+2]) );
						data->setPhysicalValue( intArray[3*i] - offsetNodeId, ary );
					}
				}
				break;
			default:
				break;
		}
	}else{
		std::string line;
		std::getline( input, line );
		int dim=0, size=0;
		input >> dim >> size;
		if( dim != 3 ){
			return -1;
		}
		int intArray[3];
		switch( btype ){
			case kmb::FFbIO::INTERCONNECT:
				{
					kmb::DataBindings* data = mesh->createDataBindings( name, kmb::DataBindings::NodeVariable, kmb::PhysicalValue::Array );
					for(long i=0;i<size;++i){
						input >> intArray[0];
						input >> intArray[1];
						input >> intArray[2];
						kmb::ArrayValue* ary = new kmb::ArrayValue();
						ary->appendValue( new kmb::IntegerValue(intArray[1]) );
						ary->appendValue( new kmb::IntegerValue(intArray[2]) );
						data->setPhysicalValue( intArray[0] - offsetNodeId, ary );
					}
				}
				break;
			default:
				break;
		}
	}
	return 0;
}
