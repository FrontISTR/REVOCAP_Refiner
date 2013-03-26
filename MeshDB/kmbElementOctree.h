/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementOctree                                           #
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
#include "MeshDB/kmbElementContainer.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbBoundingBox.h"

#include "Geometry/kmbOctree.h"
#include "MeshDB/kmbElementEvaluator.h"

namespace kmb{

class MeshData;

class Element3DOctree : public Octree<kmb::elementIdType>
{
private:
	const kmb::Point3DContainer* points;
	const kmb::ElementContainer* elements;
	kmb::ElementEvaluator* evaluator;
public:
	Element3DOctree(size_t mCount=256)
	: Octree<kmb::elementIdType>(mCount)
	, points(NULL)
	, elements(NULL)
	, evaluator(NULL)
	{
		maxExpand = true;
	};

	virtual ~Element3DOctree(void){
		if( evaluator ){
			delete evaluator;
		}
	};

	void setContainer(const kmb::Point3DContainer* pts,const kmb::ElementContainer* elems){
		points = pts;
		elements = elems;
		evaluator = new kmb::ElementEvaluator(points);
	}

	int append(const kmb::elementIdType elementId,kmb::Octree<kmb::elementIdType>::OctreeNode* octNode=NULL)
	{
		if( points != NULL && elements != NULL ){
			if( octNode == NULL ){
				octNode = topNode;
			}
			kmb::BoundingBox bbox;
			kmb::ElementContainer::const_iterator elem = elements->find(elementId);
			if( !elem.isFinished() ){
				bbox.initialize();
				evaluator->getBoundingBox( elem, bbox );
				return octNode->appendByRegion(bbox,elementId);
			}
		}
		return 0;
	}

	int appendAll(kmb::Octree<kmb::elementIdType>::OctreeNode* octNode=NULL)
	{
		int res = 0;
		if( points != NULL && elements != NULL ){
			if( octNode == NULL ){
				octNode = topNode;
			}
			kmb::BoundingBox bbox;
			kmb::ElementContainer::const_iterator elem = elements->begin();
			while( !elem.isFinished() ){
				bbox.initialize();
				evaluator->getBoundingBox( elem, bbox );
				res += octNode->appendByRegion(bbox,elem.getId());
				++elem;
			}
		}
		return res;
	}

	double getDistanceSq(const double x,const double y,const double z,const kmb::elementIdType elementId) const;

};

class Element2DOctree : public Octree<kmb::elementIdType>
{
protected:
	const kmb::Point3DContainer* points;
	const kmb::ElementContainer* elements;
	kmb::ElementEvaluator* evaluator;
public:
	Element2DOctree(size_t mCount=256)
	: Octree<kmb::elementIdType>(mCount)
	, points(NULL)
	, elements(NULL)
	, evaluator(NULL)
	{
		maxExpand = true;
	};

	virtual ~Element2DOctree(void){
		if( evaluator ){
			delete evaluator;
		}
	};

	void setContainer(const kmb::Point3DContainer* pts,const kmb::ElementContainer* elems){
		points = pts;
		elements = elems;
		evaluator = new kmb::ElementEvaluator(points);
	}

	int append(const kmb::elementIdType elementId,kmb::Octree<kmb::elementIdType>::OctreeNode* octNode=NULL)
	{
		if( points != NULL && elements != NULL ){
			if( octNode == NULL ){
				octNode = topNode;
			}
			kmb::BoundingBox bbox;
			kmb::ElementContainer::const_iterator elem = elements->find(elementId);
			evaluator->getBoundingBox( elem, bbox );
			return octNode->appendByRegion(bbox,elementId);
		}
		return 0;
	}

	int appendAll(kmb::Octree<kmb::elementIdType>::OctreeNode* octNode=NULL)
	{
		int res = 0;
		if( points != NULL && elements != NULL ){
			if( octNode == NULL ){
				octNode = topNode;
			}
			kmb::BoundingBox bbox;
			kmb::ElementContainer::const_iterator elem = elements->begin();
			while( !elem.isFinished() ){
				bbox.initialize();
				evaluator->getBoundingBox( elem, bbox );
				res += octNode->appendByRegion(bbox,elem.getId());
				++elem;
			}
		}
		return res;
	}

	double getDistanceSq(const double x,const double y,const double z,const kmb::elementIdType elementId) const;
};

class Element2DOctreeWithNormal : public Element2DOctree
{
protected:
	kmb::Vector3D normal;
public:
	Element2DOctreeWithNormal(const kmb::Vector3D nvec,size_t mCount=256)
	: Element2DOctree(mCount)
	{
		normal = nvec;
	};

	virtual ~Element2DOctreeWithNormal(void){}

	void setNormal(const kmb::Vector3D nvec){
		normal = nvec;
	}

	double getDistanceSq(const double x,const double y,const double z,const kmb::elementIdType elementId) const;
};

}
