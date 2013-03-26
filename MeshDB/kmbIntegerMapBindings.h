/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : IntegerMapBindings                                      #
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
/*
 * nodeId/elementId/bodyId => integer の対応を記録する
 * getPhysicalValue でポインタ値を得られないので注意する
 */

#pragma once
#include "MeshDB/kmbDataBindings.h"

namespace kmb{

class IntegerMapBindings : public kmb::DataBindings
{
public:
	IntegerMapBindings(kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);
	virtual ~IntegerMapBindings(void);
	virtual void clear(void);
	virtual size_t getIdCount(void) const;
	virtual bool hasId(kmb::idType id) const;
	virtual bool deleteId(kmb::idType id);
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual bool setPhysicalValue(kmb::idType id, kmb::PhysicalValue* value);
	virtual bool setPhysicalValue(kmb::idType id, long *value);
	virtual bool setValue(kmb::idType id, long value,int index=0);
	virtual bool getPhysicalValue(kmb::idType id, long *value) const;
	virtual const char* getContainerType(void) const;
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class IntegerMapBindings;
		virtual ~_iterator(void);
		virtual kmb::idType getId(void) const;
		virtual bool getValue(long *value) const;
		virtual _iterator* operator++(void);
		virtual _iterator* operator++(int n);
		virtual _iterator* clone(void);
	private:
		std::map<kmb::idType,int>::const_iterator dataIterator;
		std::map<kmb::idType,int>::const_iterator endIterator;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
private:
	std::map< kmb::idType, int > mapValues;
};
}
