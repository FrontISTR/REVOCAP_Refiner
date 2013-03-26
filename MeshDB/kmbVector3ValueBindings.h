/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Vector3ValueBindings                                    #
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

class Vector3ValueBindings : public kmb::DataBindings
{
public:
	Vector3ValueBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);

	Vector3ValueBindings(size_t count,double* values,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NodeVariable);
	virtual ~Vector3ValueBindings(void);
	virtual const char* getContainerType(void) const{
		return "Vector3ValueBindings";
	};
	virtual void clear(void);
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val);
	virtual bool setPhysicalValue(kmb::idType id, double *value);
	virtual bool setValue(kmb::idType id, double value,int index=0);
	virtual bool scalar(double r);
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::idType id) const;
	virtual bool getPhysicalValue(kmb::idType id, double *val) const;
	virtual bool hasId(kmb::idType id) const;
	virtual bool deleteId(kmb::idType id);
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual size_t getIdCount() const;
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class Vector3ValueBindings;
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
		const kmb::Vector3ValueBindings* nodeValue;
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
