/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ScalarValueBindings                                     #
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

namespace kmb{

class ScalarValueBindings : public kmb::DataBindings
{
public:
	ScalarValueBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);

	ScalarValueBindings(size_t count,double* values,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);
	virtual ~ScalarValueBindings(void);
	virtual const char* getContainerType(void) const{
		return "ScalarValueBindings";
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
	class _iterator : public DataBindings::_iterator
	{
		friend class ScalarValueBindings;
		_iterator(void);
		virtual ~_iterator(void);
		virtual kmb::idType getId(void) const;
		virtual bool getFace(kmb::Face &f) const;
		virtual kmb::PhysicalValue* getValue(void) const;
		virtual bool getValue(double *value) const;
		virtual _iterator* operator++(void);
		virtual _iterator* operator++(int n);
		virtual _iterator* clone(void);
	private:
		int index;
		const kmb::ScalarValueBindings* nodeValue;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	const double* getDoubleArray(void) const;
	double* getDoubleArray(void);
private:
	int size;
	double* values;
	bool deletable;
};

}
