/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : GridInterface                                           #
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

#include "MeshDB/kmbGridInterface.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbFaceBucketArea.h"
#include "MeshDB/kmbElementEvaluator.h"

kmb::GridInterface::GridInterface(void)
: mesh0(NULL)
, mesh1(NULL)
{
}

kmb::GridInterface::~GridInterface(void)
{
}

void
kmb::GridInterface::clearCache(void)
{
}

void
kmb::GridInterface::setupMesh(const kmb::MeshData* mesh0, kmb::MeshData* mesh1)
{
	this->mesh0 = mesh0;
	this->mesh1 = mesh1;
	clearCache();
}

int
kmb::GridInterface::interpolateData(const kmb::DataBindings* data0, kmb::DataBindings* data1)
{
	if( !(mesh0 && mesh1 && data0 && data1) ){
		return -1;
	}
	if( data0->getBindingMode() == kmb::DataBindings::ElementVariable &&
		data1->getBindingMode() == kmb::DataBindings::FaceVariable )
	{
		data1->clear();

		return 0;
	}
	return 0;
}

kmb::GridInterfaceElem2Face::GridInterfaceElem2Face(void)
: cache()
{
}

kmb::GridInterfaceElem2Face::~GridInterfaceElem2Face(void)
{
	clearCache();
}

void
kmb::GridInterfaceElem2Face::clearCache(void)
{
	cache.clear();
}

void
kmb::GridInterfaceElem2Face::setupMesh(const kmb::MeshData* mesh0, kmb::bodyIdType bodyId, kmb::MeshData* mesh1, const kmb::DataBindings* faceGroup)
{
	this->mesh0 = mesh0;
	this->mesh1 = mesh1;

}

int
kmb::GridInterfaceElem2Face::interpolateData(const kmb::DataBindings* data0, kmb::DataBindings* data1)
{
	if( !(mesh0 && mesh1 && data0 && data1) ){
		return -1;
	}
	if( data0->getBindingMode() == kmb::DataBindings::ElementVariable &&
		data1->getBindingMode() == kmb::DataBindings::FaceVariable )
	{
		data1->clear();
		int num = 0;
		double val0, val1;
		std::set< kmb::MappingCoefficient<kmb::elementIdType,kmb::Face> >::iterator cIter = cache.begin();
		while( cIter != cache.end() ){
			data0->getPhysicalValue( cIter->master, &val0 );
			data1->getPhysicalValue( cIter->slave, &val1 );
			val1 += cIter->weight * val0;
			data1->setPhysicalValue( cIter->slave, &val1 );
			++cIter;
		}
		return num;
	}
	return 0;
}

kmb::GridInterfaceStr2Face::GridInterfaceStr2Face(void)
: bucket(NULL)
, cache()
{
}

kmb::GridInterfaceStr2Face::~GridInterfaceStr2Face(void)
{
	clearCache();
	if( bucket ){
		delete bucket;
		bucket = NULL;
	}
}

void
kmb::GridInterfaceStr2Face::setupMesh(kmb::BoxRegion &box,int numx,int numy,int numz, kmb::MeshData* mesh1,const kmb::DataBindings* faceGroup)
{
	this->mesh1 = mesh1;
	if( bucket == NULL ){
		bucket = new kmb::FaceBucketArea();
	}
	bucket->clear();
	bucket->setRegion( box );
	bucket->setGridSize( numx, numy, numz );
	kmb::ElementEvaluator evaluator( mesh1->getNodes() );
	kmb::Face f;
	double farea, area;
/*

	kmb::DataVariable< kmb::Face, double > faceArea;
	kmb::DataBindings::iterator fIter = faceGroup->begin();
	while( fIter.isFinished() ){
		fIter.getFace(f);
		kmb::ElementContainer::const_iterator eIter = mesh1->findElement( f.getElementId() );
		area = evaluator.getArea(eIter,f.getLocalFaceId());
		faceArea.setPhysicalValue(f,&area);
		++fIter;
	}
*/

	bucket->setContainer( mesh1, faceGroup );
	bucket->appendAll();
	kmb::FaceBucketArea::iterator bIter = bucket->begin();
	kmb::FaceBucketArea::iterator endIter = bucket->end();
	kmb::elementIdType elementId;
	while( bIter != endIter )
	{
		f = bIter.get().first;
		kmb::ElementContainer::const_iterator eIter = mesh1->findElement( f.getElementId() );
		farea = evaluator.getArea(eIter,f.getLocalFaceId());
		area = bIter.get().second / farea;
		elementId = static_cast<kmb::elementIdType>( bIter.getIndex() );
		kmb::MappingCoefficient<kmb::elementIdType,kmb::Face> c;
		c.master = elementId;
		c.slave = f;
		c.weight = area;
		cache.insert( c );
		++bIter;
	}
}

int
kmb::GridInterfaceStr2Face::interpolateData(const kmb::DataBindings* data0, kmb::DataBindings* data1)
{
	if( !(mesh1 && data0 && data1) ){
		return -1;
	}
	if( data0->getBindingMode() == kmb::DataBindings::ElementVariable &&
		data1->getBindingMode() == kmb::DataBindings::FaceVariable )
	{
		data1->clear();
		int num = 0;
		double val0, val1;
		std::set< kmb::MappingCoefficient<kmb::elementIdType,kmb::Face> >::iterator cIter = cache.begin();
		while( cIter != cache.end() ){
			val0 = 0.0;
			val1 = 0.0;
			data0->getPhysicalValue( cIter->master, &val0 );
			data1->getPhysicalValue( cIter->slave, &val1 );
			val1 += cIter->weight * val0;
			data1->setPhysicalValue( cIter->slave, &val1 );
			++cIter;
		}
		return num;
	}
	return 0;
}

void
kmb::GridInterfaceStr2Face::clearCache(void)
{
	if( bucket ){
		bucket->clear();
	}
	cache.clear();
}
