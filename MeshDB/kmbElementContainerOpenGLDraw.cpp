/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementContainerOpenGLDraw                              #
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
#include "MeshDB/kmbElementContainerOpenGLDraw.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

const char* kmb::ElementContainerOpenGLDraw::CONTAINER_TYPE = "opengldraw";
unsigned int kmb::ElementContainerOpenGLDraw::unsignedNullNodeId = static_cast<unsigned int>(kmb::nullNodeId);

kmb::ElementContainerOpenGLDraw::ElementContainerOpenGLDraw(size_t triSize,size_t quadSize)
: ElementContainer()
, triIndex(0)
, triSize(0)
, triNodes(NULL)
, tri2Nodes(NULL)
, quadIndex(0)
, quadSize(0)
, quadNodes(NULL)
, quad2Nodes(NULL)
, triNormals(NULL)
, quadNormals(NULL)
{
	initialize( triSize, quadSize );
}

kmb::ElementContainerOpenGLDraw::~ElementContainerOpenGLDraw(void)
{
	clear();
}

void
kmb::ElementContainerOpenGLDraw::clear(void)
{
	kmb::ElementContainer::clear();

	if( triNodes != NULL ){
		delete[] triNodes;
		triNodes = NULL;
	}
	if( tri2Nodes != NULL ){
		delete[] tri2Nodes;
		tri2Nodes = NULL;
	}
	if( triNormals != NULL ){
		delete[] triNormals;
		triNormals = NULL;
	}
	triSize = 0;
	triIndex = 0;

	if( quadNodes != NULL ){
		delete[] quadNodes;
		quadNodes = NULL;
	}
	if( quad2Nodes != NULL ){
		delete[] quad2Nodes;
		quad2Nodes = NULL;
	}
	if( quadNormals != NULL ){
		delete[] quadNormals;
		quadNormals = NULL;
	}
	quadSize = 0;
	quadIndex = 0;

	elementIdMap.clear();
}

void
kmb::ElementContainerOpenGLDraw::initialize(size_t size)
{
	initialize(size,0);
}

void
kmb::ElementContainerOpenGLDraw::initialize(size_t triSize,size_t quadSize)
{
	clear();
	if( triSize > 0 ){
		triNodes = new unsigned int[3*triSize];
		this->triSize = triSize;
		this->triIndex = 0;
		for(size_t i=0;i<triSize;++i){
			triNodes[3*i]   = unsignedNullNodeId;
			triNodes[3*i+1] = unsignedNullNodeId;
			triNodes[3*i+2] = unsignedNullNodeId;
		}
	}
	if( quadSize > 0 ){
		quadNodes = new unsigned int[4*quadSize];
		this->quadSize = quadSize;
		this->quadIndex = 0;
		for(size_t i=0;i<quadSize;++i){
			quadNodes[4*i]   = unsignedNullNodeId;
			quadNodes[4*i+1] = unsignedNullNodeId;
			quadNodes[4*i+2] = unsignedNullNodeId;
			quadNodes[4*i+3] = unsignedNullNodeId;
		}
	}
}

void
kmb::ElementContainerOpenGLDraw::initSecondTri(void)
{
	if( tri2Nodes != NULL ){
		delete[] tri2Nodes;
		tri2Nodes = NULL;
	}
	tri2Nodes = new unsigned int[3*triSize];
	for(size_t i=0;i<triSize;++i){
		tri2Nodes[3*i]   = unsignedNullNodeId;
		tri2Nodes[3*i+1] = unsignedNullNodeId;
		tri2Nodes[3*i+2] = unsignedNullNodeId;
	}
}

void
kmb::ElementContainerOpenGLDraw::initSecondQuad(void)
{
	if( quad2Nodes != NULL ){
		delete[] quad2Nodes;
		quad2Nodes = NULL;
	}
	quad2Nodes = new unsigned int[4*quadSize];
	for(size_t i=0;i<quadSize;++i){
		quad2Nodes[4*i]   = unsignedNullNodeId;
		quad2Nodes[4*i+1] = unsignedNullNodeId;
		quad2Nodes[4*i+2] = unsignedNullNodeId;
		quad2Nodes[4*i+3] = unsignedNullNodeId;
	}
}

int
kmb::ElementContainerOpenGLDraw::calcNormals(kmb::Point3DContainer* points)
{
	int res = -1;
	if( points == NULL ){
		return res;
	}
	if( (triSize > 0 && triNormals) || (quadSize > 0 && quadNormals) ){

		res = 0;
		return res;
	}
	if( triSize > 0 ){
		if( triNormals == NULL ){
			triNormals = new float[3*triSize];
		}
		for(size_t i=0;i<triSize;++i){
			kmb::Vector3D normal = points->calcNormalVector( triNodes[3*i], triNodes[3*i+1], triNodes[3*i+2] );
			triNormals[3*i  ] = static_cast<float>( normal.x() );
			triNormals[3*i+1] = static_cast<float>( normal.y() );
			triNormals[3*i+2] = static_cast<float>( normal.z() );
		}
		res = 1;
	}
	if( quadSize > 0 ){
		if( quadNormals == NULL ){

			quadNormals = new float[3*quadSize];
		}
		for(size_t i=0;i<quadSize;++i){
			kmb::Vector3D normal = points->calcNormalVector( quadNodes[4*i], quadNodes[4*i+1], quadNodes[4*i+2], quadNodes[4*i+3] );
			quadNormals[3*i  ] = static_cast<float>( normal.x() );
			quadNormals[3*i+1] = static_cast<float>( normal.y() );
			quadNormals[3*i+2] = static_cast<float>( normal.z() );
		}
		res = 1;
	}
	return res;
}

kmb::elementIdType
kmb::ElementContainerOpenGLDraw::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( nodes == NULL ){
		return kmb::Element::nullElementId;
	}
	if( etype == kmb::TRIANGLE && triNodes != NULL ){
		while( triIndex < triSize && triNodes[3*triIndex] != unsignedNullNodeId ){
			++triIndex;
		}
		if( triIndex < triSize ){
			for(int i=0;i<3;++i){
				triNodes[3*triIndex+i] = nodes[i];
			}
			++(this->typeCounter[ kmb::TRIANGLE ]);
			kmb::elementIdType elemId = static_cast< kmb::elementIdType >( offsetId + triIndex + quadIndex );
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(elemId,triIndex) );
			++triIndex;
			return elemId;
		}
	}else if( etype == kmb::QUAD && quadNodes != NULL ){
		while( quadIndex < quadSize && quadNodes[4*quadIndex] != unsignedNullNodeId ){
			++quadIndex;
		}
		if( quadIndex < quadSize ){
			for(int i=0;i<4;++i){
				quadNodes[4*quadIndex+i] = nodes[i];
			}
			++(this->typeCounter[ kmb::QUAD ]);
			kmb::elementIdType elemId = static_cast< kmb::elementIdType >( offsetId + triIndex + quadIndex );
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(elemId,triSize+quadIndex) );
			++quadIndex;
			return elemId;
		}
	}else if( etype == kmb::TRIANGLE2 && triNodes != NULL ){

		if( tri2Nodes == NULL ){
			initSecondTri();
		}
		while( triIndex < triSize && triNodes[3*triIndex] != unsignedNullNodeId ){
			++triIndex;
		}
		if( triIndex < triSize ){
			for(int i=0;i<3;++i){
				triNodes[3*triIndex+i]  = nodes[i];
				tri2Nodes[3*triIndex+i] = nodes[i+3];
			}
			++(this->typeCounter[ kmb::TRIANGLE2 ]);
			kmb::elementIdType elemId = static_cast< kmb::elementIdType >( offsetId + triIndex + quadIndex );
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(elemId,triIndex) );
			++triIndex;
			return elemId;
		}
	}else if( etype == kmb::QUAD2 && quadNodes != NULL ){

		if( quad2Nodes == NULL ){
			initSecondQuad();
		}
		while( quadIndex < quadSize && quadNodes[4*quadIndex] != unsignedNullNodeId ){
			++quadIndex;
		}
		if( quadIndex < quadSize ){
			for(int i=0;i<4;++i){
				quadNodes[4*quadIndex+i] = nodes[i];
				quad2Nodes[4*quadIndex+i] = nodes[i+4];
			}
			++(this->typeCounter[ kmb::QUAD2 ]);
			kmb::elementIdType elemId = static_cast< kmb::elementIdType >( offsetId + triIndex + quadIndex );
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(elemId,triSize+quadIndex) );
			++quadIndex;
			return elemId;
		}
	}
	return kmb::Element::nullElementId;
}

kmb::elementIdType
kmb::ElementContainerOpenGLDraw::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	if( nodes == NULL ){
		return kmb::Element::nullElementId;
	}
	if( elementIdMap.find(id) != elementIdMap.end() ){

		return kmb::Element::nullElementId;
	}
	if( etype == kmb::TRIANGLE && triNodes != NULL ){
		while( triIndex < triSize && triNodes[3*triIndex] != unsignedNullNodeId ){
			++triIndex;
		}
		if( triIndex < triSize ){
			for(int i=0;i<3;++i){
				triNodes[3*triIndex+i] = nodes[i];
			}
			++(this->typeCounter[ kmb::TRIANGLE ]);
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(id,triIndex) );
			++triIndex;
			return id;
		}
	}else if( etype == kmb::QUAD && quadNodes != NULL ){
		while( quadIndex < quadSize && quadNodes[4*quadIndex] != unsignedNullNodeId ){
			++quadIndex;
		}
		if( quadIndex < quadSize ){
			for(int i=0;i<4;++i){
				quadNodes[4*quadIndex+i] = nodes[i];
			}
			++(this->typeCounter[ kmb::QUAD ]);
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(id,triSize+quadIndex) );
			++quadIndex;
			return id;
		}
	}else if( etype == kmb::TRIANGLE2 && triNodes != NULL ){

		if( tri2Nodes == NULL ){
			initSecondTri();
		}
		while( triIndex < triSize && triNodes[3*triIndex] != unsignedNullNodeId ){
			++triIndex;
		}
		if( triIndex < triSize ){
			for(int i=0;i<3;++i){
				triNodes[3*triIndex+i] = nodes[i];
				tri2Nodes[3*triIndex+i] = nodes[i+3];
			}
			++(this->typeCounter[ kmb::TRIANGLE2 ]);
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(id,triIndex) );
			++triIndex;
			return id;
		}
	}else if( etype == kmb::QUAD2 && quadNodes != NULL ){

		if( quad2Nodes == NULL ){
			initSecondQuad();
		}
		while( quadIndex < quadSize && quadNodes[4*quadIndex] != unsignedNullNodeId ){
			++quadIndex;
		}
		if( quadIndex < quadSize ){
			for(int i=0;i<4;++i){
				quadNodes[4*quadIndex+i] = nodes[i];
				quad2Nodes[4*quadIndex+i] = nodes[i+4];
			}
			++(this->typeCounter[ kmb::QUAD2 ]);
			elementIdMap.insert( std::pair< kmb::elementIdType, size_t >(id,triSize+quadIndex) );
			++quadIndex;
			return id;
		}
	}
	return kmb::Element::nullElementId;
}

bool
kmb::ElementContainerOpenGLDraw::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	std::map< kmb::elementIdType, size_t >::const_iterator eiter = elementIdMap.find(id);
	if( eiter != elementIdMap.end() ){
		int i = static_cast<int>( eiter->second);
		if( 0 <= i && i < static_cast<int>( triSize ) && triNodes && triNodes[3*i] != unsignedNullNodeId ){
			if( tri2Nodes && tri2Nodes[3*i] != unsignedNullNodeId ){
				etype = kmb::TRIANGLE2;
				for(int j=0;j<3;++j){
					nodes[j] = triNodes[3*i+j];
					nodes[j+3] = tri2Nodes[3*i+j];
				}
			}else{
				etype = kmb::TRIANGLE;
				for(int j=0;j<3;++j){
					nodes[j] = triNodes[3*i+j];
				}
			}
			return true;
		}else{
			i -= static_cast<int>(triSize);
			if( 0 <= i && i < static_cast<int>(quadSize) && quadNodes && quadNodes[4*i] != unsignedNullNodeId ){
				if( quad2Nodes && quad2Nodes[4*i] != unsignedNullNodeId ){
					etype = kmb::QUAD2;
					for(int j=0;j<4;++j){
						nodes[j] = quadNodes[4*i+j];
						nodes[j+4] = quad2Nodes[4*i+j];
					}
				}else{
					etype = kmb::QUAD;
					for(int j=0;j<4;++j){
						nodes[j] = quadNodes[4*i+j];
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool
kmb::ElementContainerOpenGLDraw::deleteElement(const kmb::elementIdType id)
{
	std::map< kmb::elementIdType, size_t >::iterator eiter = elementIdMap.find(id);
	if( eiter != elementIdMap.end() ){
		int i = static_cast<int>(eiter->second);
		if( 0 <= i && i < static_cast<int>(triSize) && triNodes && triNodes[3*i] != unsignedNullNodeId ){
			if( tri2Nodes && tri2Nodes[3*i] != unsignedNullNodeId ){
				for(int j=0;j<3;++j){
					triNodes[3*i+j] = unsignedNullNodeId;
					tri2Nodes[3*i+j] = unsignedNullNodeId;
				}
				--(this->typeCounter[ kmb::TRIANGLE2 ]);
			}else{
				for(int j=0;j<3;++j){
					triNodes[3*i+j] = unsignedNullNodeId;
				}
				--(this->typeCounter[ kmb::TRIANGLE ]);
			}
			elementIdMap.erase( eiter );
			return true;
		}else{
			i -= static_cast<int>(triSize);
			if( 0 <= i && i < static_cast<int>(quadSize) && quadNodes && quadNodes[4*i] != unsignedNullNodeId ){
				if( quad2Nodes && quad2Nodes[4*i] != unsignedNullNodeId ){
					for(int j=0;j<4;++j){
						quadNodes[4*i+j] = unsignedNullNodeId;
						quad2Nodes[4*i+j] = unsignedNullNodeId;
					}
					--(this->typeCounter[ kmb::QUAD2 ]);
					elementIdMap.erase( eiter );
				}else{
					for(int j=0;j<4;++j){
						quadNodes[4*i+j] = unsignedNullNodeId;
					}
					--(this->typeCounter[ kmb::QUAD ]);
					elementIdMap.erase( eiter );
				}
				return true;
			}
		}
	}
	return false;
}

bool
kmb::ElementContainerOpenGLDraw::includeElement(const kmb::elementIdType id) const
{
	std::map< kmb::elementIdType, size_t >::const_iterator eiter = elementIdMap.find(id);
	return ( eiter != elementIdMap.end() );
}


kmb::elementIdType
kmb::ElementContainerOpenGLDraw::getMaxId(void) const
{
	kmb::elementIdType maxId = kmb::Element::nullElementId;
	std::map< kmb::elementIdType, size_t >::const_reverse_iterator eiter = elementIdMap.rbegin();
	if( eiter != elementIdMap.rend() ){
		maxId = eiter->first + offsetId;
	}
	return maxId;
}

kmb::elementIdType
kmb::ElementContainerOpenGLDraw::getMinId(void) const
{
	kmb::elementIdType minId = kmb::Element::nullElementId;
	std::map< kmb::elementIdType, size_t >::const_iterator eiter = elementIdMap.begin();

	if( eiter != elementIdMap.end() ){
		minId = eiter->first + offsetId;
	}
	return minId;
}

size_t
kmb::ElementContainerOpenGLDraw::getCount(void) const
{
	return this->typeCounter[ kmb::TRIANGLE ] + this->typeCounter[ kmb::QUAD ] +
		this->typeCounter[ kmb::TRIANGLE2 ] + this->typeCounter[ kmb::QUAD2 ];
}

const char*
kmb::ElementContainerOpenGLDraw::getContainerType() const
{
	return kmb::ElementContainerOpenGLDraw::CONTAINER_TYPE;
}

kmb::ElementContainer::iterator
kmb::ElementContainerOpenGLDraw::begin(void)
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerOpenGLDraw::_iteratorOG* _it = NULL;
	_it = new kmb::ElementContainerOpenGLDraw::_iteratorOG();
	if( _it ){
		_it->elementContainer = this;
		_it->elementIter = this->elementIdMap.begin();
		_it->endIterator = this->elementIdMap.end();
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerOpenGLDraw::begin(void) const
{
	if( this->getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerOpenGLDraw::_iteratorOG* _it = NULL;
	_it = new kmb::ElementContainerOpenGLDraw::_iteratorOG();
	if( _it ){
		_it->elementContainer = this;
		_it->elementIter = this->elementIdMap.begin();
		_it->endIterator = this->elementIdMap.end();
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerOpenGLDraw::find(kmb::elementIdType id)
{
	std::map< kmb::elementIdType, size_t >::const_iterator eiter = elementIdMap.find( id-offsetId );
	if( eiter == elementIdMap.end() ){
		return kmb::ElementContainer::iterator(NULL);
	}
	size_t i = eiter->second;
	if( i < triSize && triNodes && triNodes[3*i] != unsignedNullNodeId ){
		kmb::ElementContainerOpenGLDraw::_iteratorOG* _it = NULL;
		_it = new kmb::ElementContainerOpenGLDraw::_iteratorOG();
		if( _it ){
			_it->elementContainer = this;
			_it->elementIter = eiter;
			_it->endIterator = this->elementIdMap.end();
		}
		return kmb::ElementContainer::iterator(_it,offsetId);
	}else{
		i -= triSize;
		if( i < quadSize && quadNodes && quadNodes[4*i] != unsignedNullNodeId ){
		kmb::ElementContainerOpenGLDraw::_iteratorOG* _it = NULL;
			_it = new kmb::ElementContainerOpenGLDraw::_iteratorOG();
			if( _it ){
				_it->elementContainer = this;
				_it->elementIter = eiter;
				_it->endIterator = this->elementIdMap.end();
			}
			return kmb::ElementContainer::iterator(_it,offsetId);
		}
		return kmb::ElementContainer::iterator(NULL);
	}
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerOpenGLDraw::find(kmb::elementIdType id) const
{
	std::map< kmb::elementIdType, size_t >::const_iterator eiter = elementIdMap.find( id-offsetId );
	if( eiter == elementIdMap.end() ){
		return kmb::ElementContainer::iterator(NULL);
	}
	size_t i = eiter->second;
	if( i < triSize && triNodes && triNodes[3*i] != unsignedNullNodeId ){
		kmb::ElementContainerOpenGLDraw::_iteratorOG* _it = NULL;
		_it = new kmb::ElementContainerOpenGLDraw::_iteratorOG();
		if( _it ){
			_it->elementContainer = this;
			_it->elementIter = eiter;
			_it->endIterator = this->elementIdMap.end();
		}
		return kmb::ElementContainer::const_iterator(_it,offsetId);
	}else{
		i -= triSize;
		if( i < quadSize && quadNodes && quadNodes[4*i] != unsignedNullNodeId ){
		kmb::ElementContainerOpenGLDraw::_iteratorOG* _it = NULL;
			_it = new kmb::ElementContainerOpenGLDraw::_iteratorOG();
			if( _it ){
				_it->elementContainer = this;
				_it->elementIter = eiter;
				_it->endIterator = this->elementIdMap.end();
			}
			return kmb::ElementContainer::const_iterator(_it,offsetId);
		}
		return kmb::ElementContainer::const_iterator(NULL);
	}
}



kmb::ElementContainerOpenGLDraw::_iteratorOG::_iteratorOG(void)
: elementContainer(NULL)
{
}

kmb::ElementContainerOpenGLDraw::_iteratorOG::~_iteratorOG(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerOpenGLDraw::_iteratorOG::clone(void)
{
	kmb::ElementContainerOpenGLDraw::_iteratorOG* obj = NULL;
	obj = new kmb::ElementContainerOpenGLDraw::_iteratorOG();
	obj->elementContainer = this->elementContainer;
	obj->elementIter = this->elementIter;
	obj->endIterator = this->endIterator;
	return obj;
}

kmb::elementIdType
kmb::ElementContainerOpenGLDraw::_iteratorOG::getId(void) const
{
	return static_cast< kmb::elementIdType >( elementIter->first + elementContainer->offsetId );
}

bool
kmb::ElementContainerOpenGLDraw::_iteratorOG::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	size_t index = elementIter->second;
	if( index < elementContainer->triSize && elementContainer->triNodes[ 3*index ] != unsignedNullNodeId ){
		if( elementContainer->tri2Nodes && elementContainer->tri2Nodes[ 3*index ] != unsignedNullNodeId ){
			etype = kmb::TRIANGLE2;
			for(int i=0;i<3;++i){
				nodes[i] = elementContainer->triNodes[ 3*index+i ];
				nodes[i+3] = elementContainer->tri2Nodes[ 3*index+i ];
			}
		}else{
			etype = kmb::TRIANGLE;
			for(int i=0;i<3;++i){
				nodes[i] = elementContainer->triNodes[ 3*index+i ];
			}
		}
		return true;
	}else{
		index -= elementContainer->triSize;
		if( index < elementContainer->quadSize && elementContainer->quadNodes[ 4*index ] != unsignedNullNodeId ){
			if( elementContainer->quad2Nodes && elementContainer->quad2Nodes[ 4*index ] != unsignedNullNodeId ){
				etype = kmb::QUAD2;
				for(int i=0;i<4;++i){
					nodes[i] = elementContainer->quadNodes[ 4*index+i ];
					nodes[i+4] = elementContainer->quad2Nodes[ 4*index+i ];
				}
			}else{
				etype = kmb::QUAD;
				for(int i=0;i<4;++i){
					nodes[i] = elementContainer->quadNodes[ 4*index+i ];
				}
			}
			return true;
		}
	}
	return false;
}

kmb::elementType
kmb::ElementContainerOpenGLDraw::_iteratorOG::getType(void) const
{
	size_t index = elementIter->second;
	if( index < elementContainer->triSize && elementContainer->triNodes[ 3*index ] != unsignedNullNodeId ){
		if( elementContainer->tri2Nodes && elementContainer->tri2Nodes[ 3*index ] != unsignedNullNodeId ){
			return kmb::TRIANGLE2;
		}else{
			return kmb::TRIANGLE;
		}
	}else{
		index -= elementContainer->triSize;
		if( index < elementContainer->quadSize && elementContainer->quadNodes[ 4*index ] != unsignedNullNodeId ){
			if( elementContainer->quad2Nodes && elementContainer->quad2Nodes[ 4*index ] != unsignedNullNodeId ){
				return kmb::QUAD2;
			}else{
				return kmb::QUAD;
			}
		}else{
		}
	}
	return kmb::UNKNOWNTYPE;
}

kmb::nodeIdType
kmb::ElementContainerOpenGLDraw::_iteratorOG::getCellId(int cellIndex) const
{
	size_t index = elementIter->second;
	if( index < elementContainer->triSize && elementContainer->triNodes[ 3*index ] != unsignedNullNodeId ){
		if( elementContainer->tri2Nodes && elementContainer->tri2Nodes[ 3*index ] != unsignedNullNodeId ){
			if( 0 <= cellIndex && cellIndex < 3 ){
				return elementContainer->triNodes[ 3*index+cellIndex ];
			}else if( 3 <= cellIndex && cellIndex < 6 ){
				return elementContainer->tri2Nodes[ 3*index+cellIndex-3 ];
			}
		}else{
			if( 0 <= cellIndex && cellIndex < 3 ){
				return elementContainer->triNodes[ 3*index+cellIndex ];
			}
		}
	}else{
		index -= elementContainer->triSize;
		if( index < elementContainer->quadSize && elementContainer->quadNodes[ 4*index ] != unsignedNullNodeId ){
			if( elementContainer->quad2Nodes && elementContainer->quad2Nodes[ 4*index ] != unsignedNullNodeId ){
				if( 0 <= cellIndex && cellIndex < 4){
					return elementContainer->quadNodes[ 4*index+cellIndex ];
				}else if( 4 <= cellIndex && cellIndex < 8){
					return elementContainer->quad2Nodes[ 4*index+cellIndex-4 ];
				}
			}else{
				if( 0 <= cellIndex && cellIndex < 4){
					return elementContainer->quadNodes[ 4*index+cellIndex ];
				}
			}
		}
	}
	return kmb::nullNodeId;
}

bool
kmb::ElementContainerOpenGLDraw::_iteratorOG::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	size_t index = elementIter->second;
	if( index < elementContainer->triSize && elementContainer->triNodes[ 3*index ] != unsignedNullNodeId ){
		if( elementContainer->tri2Nodes && elementContainer->tri2Nodes[ 3*index ] != unsignedNullNodeId ){
			if( 0 <= cellIndex && cellIndex < 3 ){
				elementContainer->triNodes[ 3*index+cellIndex ] = nodeId;
				return true;
			}else if( 3 <= cellIndex && cellIndex < 6 ){
				elementContainer->tri2Nodes[ 3*index+cellIndex-3 ] = nodeId;
				return true;
			}
		}else{
			if( 0 <= cellIndex && cellIndex < 3 ){
				elementContainer->triNodes[ 3*index+cellIndex ] = nodeId;
				return true;
			}
		}
	}else{
		index -= elementContainer->triSize;
		if( index < elementContainer->quadSize && elementContainer->quadNodes[ 4*index ] != unsignedNullNodeId ){
			if( elementContainer->quad2Nodes && elementContainer->quad2Nodes[ 4*index ] != unsignedNullNodeId ){
				if(0 <= cellIndex && cellIndex < 4){
					elementContainer->quadNodes[ 4*index+cellIndex ] = nodeId;
					return true;
				}else if( 4 <= cellIndex && cellIndex < 8){
					elementContainer->quad2Nodes[ 4*index+cellIndex-4 ] = nodeId;
					return true;
				}
			}else{
				if(0 <= cellIndex && cellIndex < 4){
					elementContainer->quadNodes[ 4*index+cellIndex ] = nodeId;
					return true;
				}
			}
		}
	}
	return false;
}

kmb::nodeIdType
kmb::ElementContainerOpenGLDraw::_iteratorOG::operator[](const int i) const
{
	size_t index = elementIter->second;
	if( index < elementContainer->triSize && elementContainer->triNodes[ 3*index ] != unsignedNullNodeId ){
		if( elementContainer->tri2Nodes && elementContainer->tri2Nodes[ 3*index ] != unsignedNullNodeId ){
			if( 0 <= i && i < 3 ){
				return elementContainer->triNodes[ 3*index+i ];
			}else if( 3 <= i && i < 6 ){
				return elementContainer->tri2Nodes[ 3*index+i-3 ];
			}
		}else{
			if( 0 <= i && i < 3 ){
				return elementContainer->triNodes[ 3*index+i ];
			}
		}
	}else{
		index -= elementContainer->triSize;
		if( index < elementContainer->quadSize && elementContainer->quadNodes[ 4*index ] != unsignedNullNodeId ){
			if( elementContainer->quad2Nodes && elementContainer->quad2Nodes[ 4*index ] != unsignedNullNodeId ){
				if( 0 <= i && i < 4){
					return elementContainer->quadNodes[ 4*index+i ];
				}else if( 4 <= i && i < 8){
					return elementContainer->quad2Nodes[ 4*index+i-4 ];
				}
			}else{
				if( 0 <= i && i < 4){
					return elementContainer->quadNodes[ 4*index+i ];
				}
			}
		}
	}
	return kmb::nullNodeId;
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerOpenGLDraw::_iteratorOG::operator++(void)
{
	++elementIter;
	if( elementIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerOpenGLDraw::_iteratorOG::operator++(int n)
{
	elementIter++;
	if( elementIter != endIterator ){
		return this;
	}else{
		return NULL;
	}
}

