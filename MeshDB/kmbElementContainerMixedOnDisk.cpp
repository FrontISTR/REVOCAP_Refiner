/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ElementContainerMixedOnDisk                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/

#include <string>
#include <cstdio>
#include "MeshDB/kmbElementContainerMixedOnDisk.h"
#include "MeshDB/kmbElement.h"

const char* kmb::ElementContainerMixedOnDisk::CONTAINER_TYPE = "mixed_ondisk";

const char kmb::ElementContainerMixedOnDisk::HEADER_STRING[80]
	= "RevocapMesh::ElementContainerMixedOnDisk                                      ";

kmb::ElementContainerMixedOnDisk::ElementContainerMixedOnDisk(unsigned long s,const char* filename)
: kmb::ElementContainer()
, size(s)
, index(0)
, nindex(0)
, count(0)
{
	std::string nodeTableFilename;
	std::string etypeTableFilename;
	std::string indexTableFilename;
	if( filename == NULL ){
		nodeTableFilename = "tmp_elem.nt";
		etypeTableFilename = "tmp_elem.et";
		indexTableFilename = "tmp_elem.it";
	}else{
		nodeTableFilename = filename;
		etypeTableFilename = filename;
		indexTableFilename = filename;
		nodeTableFilename.append(".nt");
		etypeTableFilename.append(".et");
		indexTableFilename.append(".it");
	}
	nodeTable.initialize( nodeTableFilename.c_str(), kmb::ElementContainerMixedOnDisk::HEADER_STRING, sizeof(kmb::nodeIdType), size );
	etypeTable.initialize( etypeTableFilename.c_str(), kmb::ElementContainerMixedOnDisk::HEADER_STRING, sizeof(kmb::elementType), size );
	indexTable.initialize( indexTableFilename.c_str(), kmb::ElementContainerMixedOnDisk::HEADER_STRING, sizeof(size_t), size );
}

kmb::ElementContainerMixedOnDisk::~ElementContainerMixedOnDisk(void)
{
	clear();
}

void
kmb::ElementContainerMixedOnDisk::initialize(size_t eSize)
{
	clear();
	nodeTable.terminate();
	etypeTable.terminate();
	indexTable.terminate();
	size = static_cast<unsigned long>(eSize);
	std::string nodeTableFilename = "tmp_elem.nt";
	std::string etypeTableFilename = "tmp_elem.et";
	std::string indexTableFilename = "tmp_elem.it";
	nodeTable.initialize( nodeTableFilename.c_str(), kmb::ElementContainerMixedOnDisk::HEADER_STRING, sizeof(kmb::nodeIdType), size );
	etypeTable.initialize( etypeTableFilename.c_str(), kmb::ElementContainerMixedOnDisk::HEADER_STRING, sizeof(kmb::elementType), size );
	indexTable.initialize( indexTableFilename.c_str(), kmb::ElementContainerMixedOnDisk::HEADER_STRING, sizeof(size_t), size );
}

const char*
kmb::ElementContainerMixedOnDisk::getContainerType(void) const
{
	return kmb::ElementContainerMixedOnDisk::CONTAINER_TYPE;
}

kmb::elementIdType
kmb::ElementContainerMixedOnDisk::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	kmb::elementType tmpType;
	while( /* index < size && */ etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&tmpType) ) ){
		if( tmpType != kmb::UNKNOWNTYPE ){
			++index;
		}else{
			break;
		}
	}
	etypeTable.setData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
	indexTable.setData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&nindex) );
	int len = kmb::Element::getNodeCount( etype );
	nodeTable.setData( static_cast<unsigned long>(nindex), reinterpret_cast<const char*>(nodes), len );
	++(this->typeCounter[ etype ]);
	nindex += len;
	++index;
	++count;
	return static_cast< kmb::elementIdType >( offsetId + index - 1 );
}

kmb::elementIdType
kmb::ElementContainerMixedOnDisk::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	kmb::elementIdType i = id - offsetId;
	if( i < 0 /* || static_cast<size_t>(i) >= size */ ){
		return kmb::Element::nullElementId;
	}else{
		kmb::elementType tmpType = kmb::UNKNOWNTYPE;
		if( etypeTable.getData( i, reinterpret_cast<char*>(&tmpType) ) && tmpType != kmb::UNKNOWNTYPE ){
			return kmb::Element::nullElementId;
		}
	}
	etypeTable.setData( i, reinterpret_cast<char*>(&etype) );
	indexTable.setData( i, reinterpret_cast<char*>(&nindex) );
	int len = kmb::Element::getNodeCount( etype );
	nodeTable.setData( static_cast< unsigned long >(nindex), reinterpret_cast<const char*>(nodes), len );
	++(this->typeCounter[ etype ]);
	nindex += len;
	++count;
	return id;
}

bool
kmb::ElementContainerMixedOnDisk::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	kmb::elementIdType i = id - offsetId;
	if( i < 0 /* || static_cast<size_t>(i) >= size */ ){
		return false;
	}else{
		kmb::elementType tmpType = kmb::UNKNOWNTYPE;
		if( !etypeTable.getData( i, reinterpret_cast<char*>(&tmpType) ) || tmpType == kmb::UNKNOWNTYPE ){
			return false;
		}
	}
	etypeTable.getData( i, reinterpret_cast<char*>(&etype) );
	size_t ni = 0;
	indexTable.getData( i, reinterpret_cast<char*>(&ni) );
	int len = kmb::Element::getNodeCount( etype );
	nodeTable.getData( static_cast<unsigned long>(ni), reinterpret_cast<char*>(nodes), len );
	return true;
}


bool
kmb::ElementContainerMixedOnDisk::deleteElement(const kmb::elementIdType id)
{
	kmb::elementIdType i = id - offsetId;
	if( i < 0 /* || static_cast<size_t>(i) >= size */ ){
		return false;
	}else{
		kmb::elementType tmpType = kmb::UNKNOWNTYPE;
		if( !etypeTable.getData( i, reinterpret_cast<char*>(&tmpType) ) || tmpType == kmb::UNKNOWNTYPE ){
			return false;
		}
	}
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	etypeTable.getData( i, reinterpret_cast<char*>(&etype) );
	--(this->typeCounter[ etype ]);
	--count;
	etype = kmb::UNKNOWNTYPE;
	size_t ni = 0;
	etypeTable.setData( i, reinterpret_cast<char*>(&etype) );
	indexTable.setData( i, reinterpret_cast<char*>(&ni) );
	return true;
}

bool
kmb::ElementContainerMixedOnDisk::includeElement(const kmb::elementIdType id) const
{
	kmb::elementIdType i = id - offsetId;
	if( i < 0 /* || static_cast<size_t>(i) >= size */ ){
		return false;
	}else{
		kmb::elementType tmpType = kmb::UNKNOWNTYPE;
		if( !etypeTable.getData( i, reinterpret_cast<char*>(&tmpType) ) || tmpType == kmb::UNKNOWNTYPE ){
			return false;
		}
	}
	return true;
}

kmb::elementIdType
kmb::ElementContainerMixedOnDisk::getMaxId(void) const
{
	return static_cast< kmb::elementIdType >( this->offsetId + this->etypeTable.getMaxIndex() );
}

kmb::elementIdType
kmb::ElementContainerMixedOnDisk::getMinId(void) const
{
	return this->offsetId;
}

size_t
kmb::ElementContainerMixedOnDisk::getCount(void) const
{
	return this->count;
}



kmb::ElementContainerMixedOnDisk::_iteratorMOD::_iteratorMOD(void)
: index(0)
, elementContainer(NULL)
{
}

kmb::ElementContainerMixedOnDisk::_iteratorMOD::~_iteratorMOD(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMixedOnDisk::_iteratorMOD::clone(void)
{
	kmb::ElementContainerMixedOnDisk::_iteratorMOD* obj = NULL;
	obj = new kmb::ElementContainerMixedOnDisk::_iteratorMOD();
	obj->elementContainer = this->elementContainer;
	obj->index = this->index;
	return obj;
}

kmb::elementIdType
kmb::ElementContainerMixedOnDisk::_iteratorMOD::getId(void) const
{
	return static_cast< kmb::elementIdType >( index );
}

bool
kmb::ElementContainerMixedOnDisk::_iteratorMOD::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	elementContainer->etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
	if( etype == kmb::UNKNOWNTYPE ){
		return false;
	}
	size_t ni = 0;
	elementContainer->indexTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&ni) );
	int len = kmb::Element::getNodeCount( etype );
	elementContainer->nodeTable.getData( static_cast<unsigned long>(ni), reinterpret_cast<char*>(nodes), len );
	return true;
}

kmb::elementType
kmb::ElementContainerMixedOnDisk::_iteratorMOD::getType(void) const
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	elementContainer->etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
	return etype;
}

kmb::nodeIdType
kmb::ElementContainerMixedOnDisk::_iteratorMOD::getCellId(int cellIndex) const
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	elementContainer->etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
	if( etype == kmb::UNKNOWNTYPE ){
		return kmb::nullNodeId;
	}
	size_t ni = 0;
	elementContainer->indexTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&ni) );
	int len = kmb::Element::getNodeCount( etype );
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	if( 0 <= cellIndex && cellIndex < len ){
		elementContainer->nodeTable.getData( static_cast<unsigned long>(ni+cellIndex), reinterpret_cast<char*>(&nodeId) );
		return nodeId;
	}
	return kmb::nullNodeId;
}

bool
kmb::ElementContainerMixedOnDisk::_iteratorMOD::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	elementContainer->etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
	if( etype == kmb::UNKNOWNTYPE ){
		return false;
	}
	size_t ni = 0;
	elementContainer->indexTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&ni) );
	int len = kmb::Element::getNodeCount( etype );
	if( 0 <= cellIndex && cellIndex < len ){
		const_cast<kmb::ElementContainerMixedOnDisk*>(elementContainer)->nodeTable.setData( static_cast<unsigned long>(ni+cellIndex), reinterpret_cast<const char*>(&nodeId), len );
		return true;
	}
	return false;
}

kmb::nodeIdType
kmb::ElementContainerMixedOnDisk::_iteratorMOD::operator[](const int cellIndex) const
{
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	elementContainer->etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
	if( etype == kmb::UNKNOWNTYPE ){
		return kmb::nullNodeId;
	}
	size_t ni = 0;
	elementContainer->indexTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&ni) );
	int len = kmb::Element::getNodeCount( etype );
	kmb::nodeIdType nodeId = kmb::nullNodeId;
	if( 0 <= cellIndex && cellIndex < len ){
		elementContainer->nodeTable.getData( static_cast<unsigned long>(ni+cellIndex), reinterpret_cast<char*>(&nodeId) );
		return nodeId;
	}
	return kmb::nullNodeId;
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMixedOnDisk::_iteratorMOD::operator++(void)
{
	++index;
	while( index <= static_cast<unsigned long>(elementContainer->etypeTable.getMaxIndex()) ){
		kmb::elementType etype = kmb::UNKNOWNTYPE;
		elementContainer->etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
		if( etype == kmb::UNKNOWNTYPE ){
			++index;
		}else{
			break;
		}
	}
	if( index <= static_cast<unsigned long>(elementContainer->etypeTable.getMaxIndex()) ){
		return this;
	}else{
		return NULL;
	}
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

kmb::ElementContainer::_iterator*
kmb::ElementContainerMixedOnDisk::_iteratorMOD::operator++(int n)
{
	index++;
	while( index <= static_cast<unsigned long>(elementContainer->etypeTable.getMaxIndex()) ){
		kmb::elementType etype = kmb::UNKNOWNTYPE;
		elementContainer->etypeTable.getData( static_cast<unsigned long>(index), reinterpret_cast<char*>(&etype) );
		if( etype == kmb::UNKNOWNTYPE ){
			index++;
		}else{
			break;
		}
	}
	if( index <= static_cast<unsigned long>(elementContainer->etypeTable.getMaxIndex()) ){
		return this;
	}else{
		return NULL;
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

kmb::ElementContainer::iterator
kmb::ElementContainerMixedOnDisk::begin(void)
{
	if( getCount() == 0 || !nodeTable.good() || !etypeTable.good() || !indexTable.good() ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerMixedOnDisk::_iteratorMOD* _it = NULL;
	_it = new kmb::ElementContainerMixedOnDisk::_iteratorMOD();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMixedOnDisk::begin(void) const
{
	if( getCount() == 0 || !nodeTable.good() || !etypeTable.good() || !indexTable.good() ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerMixedOnDisk::_iteratorMOD* _it = NULL;
	_it = new kmb::ElementContainerMixedOnDisk::_iteratorMOD();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerMixedOnDisk::find(kmb::elementIdType id)
{
	int i = id - offsetId;
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( getCount() == 0 || !nodeTable.good() || !etypeTable.good() || !indexTable.good() || i < 0 /* || static_cast<size_t>(i) >= size */ ){
		return kmb::ElementContainer::iterator(NULL);
	}else{
		if( !etypeTable.getData( i, reinterpret_cast<char*>(&etype) ) || etype == kmb::UNKNOWNTYPE ){
			return kmb::ElementContainer::iterator(NULL);
		}
	}
	kmb::ElementContainerMixedOnDisk::_iteratorMOD* _it = NULL;
	_it = new kmb::ElementContainerMixedOnDisk::_iteratorMOD();
	if( _it ){
		_it->elementContainer = this;
		_it->index = i;
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMixedOnDisk::find(kmb::elementIdType id) const
{
	int i = id - offsetId;
	kmb::elementType etype = kmb::UNKNOWNTYPE;
	if( getCount() == 0 || !nodeTable.good() || !etypeTable.good() || !indexTable.good() || i < 0 /* || static_cast<size_t>(i) >= size */ ){
		return kmb::ElementContainer::const_iterator(NULL);
	}else{
		if( !etypeTable.getData( i, reinterpret_cast<char*>(&etype) ) || etype == kmb::UNKNOWNTYPE ){
			return kmb::ElementContainer::const_iterator(NULL);
		}
	}
	kmb::ElementContainerMixedOnDisk::_iteratorMOD* _it = NULL;
	_it = new kmb::ElementContainerMixedOnDisk::_iteratorMOD();
	if( _it ){
		_it->elementContainer = this;
		_it->index = i;
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}
