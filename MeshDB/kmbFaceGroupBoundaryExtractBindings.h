/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : FaceGroupBoundaryExtractBindings                        #
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

#include "MeshDB/kmbDataBindings.h"
#include <map>

namespace kmb{

class MeshData;

class FaceGroupBoundaryExtractBindings : public kmb::DataBindings
{
protected:
	kmb::MeshData* mesh;
	std::multimap< kmb::nodeIdType, kmb::Face > facemap;
public:
	FaceGroupBoundaryExtractBindings(void);
	virtual ~FaceGroupBoundaryExtractBindings(void);
	virtual const char* getContainerType(void) const;
	virtual void clear(void);
	virtual size_t getIdCount(void) const;
	virtual bool addId(kmb::Face f);
	virtual bool hasId(kmb::Face f) const;
	virtual bool deleteId(kmb::Face f);
	virtual bool replaceId(kmb::Face old_f,kmb::Face new_f);

	class _iteratorFGBE : public DataBindings::_iterator
	{
		friend class FaceGroupBoundaryExtractBindings;
	private:
		std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator fIter;
		std::multimap< kmb::nodeIdType, kmb::Face >::const_iterator endIter;
	public:
		_iteratorFGBE(void) : DataBindings::_iterator(){};
		virtual ~_iteratorFGBE(void){};
		virtual bool getFace(kmb::Face &f) const;
		virtual DataBindings::_iterator* operator++(void);
		virtual DataBindings::_iterator* operator++(int n);
		virtual DataBindings::_iterator* clone(void);
	};
	virtual DataBindings::iterator begin(void);
	virtual DataBindings::const_iterator begin(void) const;


	void setMesh(kmb::MeshData* mesh);
	size_t appendBody(kmb::bodyIdType bodyId);




	kmb::Face getNeighbor(kmb::Face f,bool reverse=false) const;
};

}
