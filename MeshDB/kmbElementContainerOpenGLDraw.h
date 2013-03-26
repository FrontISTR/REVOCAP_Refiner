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
/**
 * OpenGL の drawElement の引数として渡すことができるような
 * 三角形または四角形の配列
 *
 * 三角形配列
 * 四角形配列
 * 三角形の elementId の配列
 * 四角形の elementId の配列
 * 三角形の個数
 * 四角形の個数
 *
 * elementId は順番がばらばらでもよい
 *
 */

#pragma once
#include "MeshDB/kmbElementContainer.h"
#include <map>

namespace kmb{

class ElementContainerOpenGLDraw : public ElementContainer
{
protected:
	size_t triIndex;
	size_t triSize;
	unsigned int* triNodes;

	unsigned int* tri2Nodes;
	size_t quadIndex;
	size_t quadSize;
	unsigned int* quadNodes;

	unsigned int* quad2Nodes;

	float* triNormals;
	float* quadNormals;



	std::map< kmb::elementIdType, size_t > elementIdMap;


	static unsigned int unsignedNullNodeId;
public:
	static const char* CONTAINER_TYPE;
	ElementContainerOpenGLDraw(size_t triSize,size_t quadSize);
	virtual ~ElementContainerOpenGLDraw(void);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes);
	virtual kmb::elementIdType addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id);
	virtual bool getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const;
	virtual bool deleteElement(const kmb::elementIdType id);
	virtual bool includeElement(const kmb::elementIdType id) const;
	virtual kmb::elementIdType getMaxId(void) const;
	virtual kmb::elementIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual void initialize(size_t triSize,size_t quadSize);
	virtual void initialize(size_t size);

	virtual const char* getContainerType(void) const;

	class _iteratorOG : public ElementContainer::_iterator
	{
		friend class ElementContainerOpenGLDraw;
	public:
		virtual kmb::elementIdType getId(void) const;
		virtual bool getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const;
		virtual kmb::elementType getType(void) const;
		virtual kmb::nodeIdType getCellId(int cellIndex) const;
		virtual bool setCellId(int cellIndex, kmb::nodeIdType nodeId);
		virtual kmb::nodeIdType operator[](const int i) const;
		virtual ElementContainer::_iterator* operator++(void);
		virtual ElementContainer::_iterator* operator++(int n);
		virtual ElementContainer::_iterator* clone(void);
	private:
		_iteratorOG(void);
		virtual ~_iteratorOG(void);
		std::map< kmb::elementIdType, size_t >::const_iterator elementIter;
		std::map< kmb::elementIdType, size_t >::const_iterator endIterator;
		const ElementContainerOpenGLDraw* elementContainer;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::elementIdType id);
	virtual const_iterator find(kmb::elementIdType id) const;




	int calcNormals(kmb::Point3DContainer* points);

	const unsigned int* getTriNodeTable(void) const { return triNodes; }
	const unsigned int* getQuadNodeTable(void) const { return quadNodes; }
	const float* getTriNormals(void) const { return triNormals; }
	const float* getQuadNormals(void) const { return quadNormals; }
	size_t getTriCount(void) const { return triIndex; }
	size_t getQuadCount(void) const { return quadIndex; }
protected:
	void initSecondTri(void);
	void initSecondQuad(void);
};

}
