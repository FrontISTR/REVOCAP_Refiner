/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ScalarValueMArrayBindings                               #
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
#include "Common/kmbBLArray.h"

namespace kmb{

class ScalarValueMArrayBindings : public kmb::DataBindings
{
private:
	BLArray<double> vArray;
	BLArrayIndex aIndex;
	size_t count;
public:
	ScalarValueMArrayBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);
	virtual ~ScalarValueMArrayBindings(void);
	virtual const char* getContainerType(void) const{
		return "ScalarValueMArrayBindings";
	};
	virtual void clear(void);
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val);
	virtual bool setPhysicalValue(kmb::idType id,double* val);
	virtual bool setValue(kmb::idType id, double value,int index=0);
	virtual bool scalar(double r);
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::idType id) const;
	virtual bool getPhysicalValue(kmb::idType id,double *val) const;
	virtual bool hasId(kmb::idType id) const;
	virtual bool deleteId(kmb::idType id);
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual size_t getIdCount() const;
public:
	class _iteratorMArray : public DataBindings::_iterator
	{
		friend class ScalarValueMArrayBindings;
		_iteratorMArray(void);
		virtual ~_iteratorMArray(void);
		virtual kmb::idType getId(void) const;
		virtual bool getFace(kmb::Face &f) const;
		virtual kmb::PhysicalValue* getValue(void) const;
		virtual bool getValue(double *value) const;
		virtual DataBindings::_iterator* operator++(void);
		virtual DataBindings::_iterator* operator++(int n);
		virtual DataBindings::_iterator* clone(void);
	private:
		const kmb::ScalarValueMArrayBindings* values;
		kmb::BLArrayIndex aIndex;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
};

}
