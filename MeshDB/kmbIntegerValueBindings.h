/*----------------------------------------------------------------------
#                                                                      #
# Software Name : Advance/REVOCAP_PrePost version 3.2                  #
# Class Name : IntegerValueBindings                                    #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/10/21     #
#                                                                      #
#      Copyright (C) 2013 Advancesoft corp.                            #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once
#include "MeshDB/kmbDataBindings.h"

namespace kmb{

class IntegerValueBindings : public kmb::DataBindings
{
public:
	IntegerValueBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);
	// 他で確保済みの配列を Data として使う
	IntegerValueBindings(size_t count,int* values,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);
	virtual ~IntegerValueBindings(void);
	virtual const char* getContainerType(void) const{
		return "IntegerValueBindings";
	};
	virtual void clear(void);
	void clear(int v);
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val);
	virtual bool setPhysicalValue(kmb::idType id,long* val);
	virtual bool setValue(kmb::idType id, long value,int index=0);
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::idType id) const;
	virtual bool getPhysicalValue(kmb::idType id,long* val) const;
	virtual bool hasId(kmb::idType id) const;
	virtual bool deleteId(kmb::idType id);
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual size_t getIdCount() const;
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class IntegerValueBindings;
		_iterator(void);
		virtual ~_iterator(void);
		virtual kmb::idType getId(void) const;
		virtual bool getFace(kmb::Face &f) const;
		virtual kmb::PhysicalValue* getValue(void) const;
		virtual bool getValue(long *value) const;
		virtual _iterator* operator++(void);
		virtual _iterator* operator++(int n);
		virtual _iterator* clone(void);
	private:
		int index;
		const kmb::IntegerValueBindings* nodeValue;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	const int* getIntArray(void) const;
	int* getIntArray(void);
private:
	int size;
	int* values;
	bool deallocatable;
};

}
