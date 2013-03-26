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





















#pragma once

#include <set>
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbFace.h"

namespace kmb{

class MeshData;
class DataBindings;
class FaceBucketArea;

template <typename T0,typename T1>
class MappingCoefficient
{
public:
	T0 master;
	T1 slave;
	double weight;
	bool operator<(const MappingCoefficient &other)const{
		return (master < other.master) ||
			(master==other.master && slave < other.slave) ||
			(master==other.master && slave == other.slave && weight < other.weight);
	}
};

class GridInterface
{
public:
	enum cachMode{
		ELEM2FACE,
		STR2FACE,
		NOCACHE
	};
protected:
	const kmb::MeshData* mesh0;
	kmb::MeshData* mesh1;
public:
	GridInterface(void);
	virtual ~GridInterface(void);
	void setupMesh(const kmb::MeshData* mesh0, kmb::MeshData* mesh1);

	virtual int interpolateData(const kmb::DataBindings* data0, kmb::DataBindings* data1);
protected:
	virtual void clearCache(void);
};

class GridInterfaceElem2Face : public GridInterface
{
protected:
	std::set< kmb::MappingCoefficient<kmb::elementIdType,kmb::Face> > cache;
public:
	GridInterfaceElem2Face(void);
	virtual ~GridInterfaceElem2Face(void);
	void setupMesh(const kmb::MeshData* mesh0, kmb::bodyIdType bodyId, kmb::MeshData* mesh1, const kmb::DataBindings* faceGroup);

	virtual int interpolateData(const kmb::DataBindings* data0, kmb::DataBindings* data1);
protected:
	void clearCache(void);
};



class GridInterfaceStr2Face : public GridInterface
{
protected:
	kmb::FaceBucketArea* bucket;
	std::set< kmb::MappingCoefficient<kmb::elementIdType,kmb::Face> > cache;
public:
	GridInterfaceStr2Face(void);
	virtual ~GridInterfaceStr2Face(void);
	void setupMesh(kmb::BoxRegion &box,int numx,int numy,int numz, kmb::MeshData* mesh1, const kmb::DataBindings* faceGroup);

	virtual int interpolateData(const kmb::DataBindings* data0, kmb::DataBindings* data1);
protected:
	void clearCache(void);
};

}
