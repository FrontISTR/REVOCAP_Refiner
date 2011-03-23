/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : ContainerOnDisk                                         #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "Geometry/kmb_ContainerOnDisk.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>








unsigned int kmb::ContainerOnDisk::HEADER_MAGIC_NUMBER = 0x5245564f;

kmb::ContainerOnDisk::ContainerOnDisk(void)
: headerSize(0)
, dataUnitSize(0)
, dataSize(0L)
, indexCounter(0L)
, maxIndex(-1L)
{
}

kmb::ContainerOnDisk::~ContainerOnDisk(void)
{
	terminate();
}

bool
kmb::ContainerOnDisk::initialize(const char* filename, const char headerString[80],unsigned int dataUnitSize,unsigned long dataSize)
{
#if defined(WIN32)
 #if _MSC_VER > 1300
	char buf[256];
	char* temp = _tempnam( "tmp", filename );
	strncpy_s( buf, temp, strlen(temp)+1 );
	if( strlen(buf) > 0 ){
 #else
	char buf[256];
	char* temp = _tempnam( "tmp", filename );
	strncpy( buf, temp, strlen(temp)+1 );
	if( strlen(buf) > 0 ){
 #endif
#else
	char buf[256];
	strncpy( buf, "tmp/", 4 );
	strncat( buf, filename, strlen(filename) );
	strncat( buf, ".XXXXXX", 7 );
	int fd = mkstemp( buf );
	if( fd != -1 ){
#endif
		this->fileBindings.open( buf, std::ios_base::out|std::ios_base::in|std::ios_base::binary|std::ios_base::trunc );
		this->dataUnitSize = dataUnitSize;
		this->dataSize = dataSize;
		this->fileBindings.seekp( 0, std::ios_base::beg );
		unsigned int num = HEADER_MAGIC_NUMBER;
		this->fileBindings.write( reinterpret_cast<const char*>(&num), sizeof(num) );
		this->fileBindings.write( headerString, 80 );
		headerSize = sizeof(HEADER_MAGIC_NUMBER) + 80 + sizeof(headerSize) + sizeof(dataUnitSize) + sizeof(dataSize);
		this->fileBindings.write( reinterpret_cast<const char*>(&headerSize), sizeof(headerSize) );
		this->fileBindings.write( reinterpret_cast<const char*>(&dataUnitSize), sizeof(dataUnitSize) );
		this->fileBindings.write( reinterpret_cast<const char*>(&dataSize), sizeof(dataSize) );
		return true;
	}else{
		return false;
	}
}

void
kmb::ContainerOnDisk::terminate(void)
{
	if( this->fileBindings.is_open() ){
		this->fileBindings.close();
	}
}

bool
kmb::ContainerOnDisk::setData( unsigned long index, const char* data, unsigned long num )
{
	this->fileBindings.seekp( static_cast<std::streamoff>(this->headerSize + this->dataUnitSize*index), std::ios_base::beg );
	if( this->fileBindings.fail() ){
		this->fileBindings.clear();
		return false;
	}
	this->fileBindings.write( data, this->dataUnitSize*num );
	this->fileBindings.flush();
	indexCounter += num;
	if( maxIndex < static_cast<long>(index+num-1) ){
		maxIndex = index+num-1;
	}
	return true;
}

bool
kmb::ContainerOnDisk::getData( unsigned long index, char* data, unsigned long num ) const
{
	if( maxIndex < static_cast<long>(index+num-1) ){
		return false;
	}
	kmb::ContainerOnDisk* self = const_cast<kmb::ContainerOnDisk*>(this);
	self->fileBindings.seekg( static_cast<std::streamoff>(this->headerSize + this->dataUnitSize*index), std::ios_base::beg );
	if( self->fileBindings.fail() ){
		self->fileBindings.clear();
		return false;
	}
	self->fileBindings.read( data, this->dataUnitSize*num );
	return true;
}

unsigned long
kmb::ContainerOnDisk::getSize(void) const
{
	return this->indexCounter;
}

bool
kmb::ContainerOnDisk::good(void) const
{
	return this->fileBindings.good();
}

long
kmb::ContainerOnDisk::getMaxIndex(void) const
{
	return this->maxIndex;
}

void
kmb::ContainerOnDisk::debug(void)
{
	printf("header size = %u\n", headerSize);
	printf("data unit size = %u\n", dataUnitSize);
	printf("data size = %lu\n", dataSize);
	printf("filebindings status : good %d\n", fileBindings.good());
	printf("filebindings status : fail %d\n", fileBindings.fail());
	printf("filebindings status : eof %d\n", fileBindings.eof());
	long tg = static_cast<long>( fileBindings.tellg() );
	printf("filebindings status : tellg %ld\n", tg);
	printf("filebindings status : indexCounter %lu\n", indexCounter);
	printf("filebindings status : maxIndex %ld\n", maxIndex);
}
