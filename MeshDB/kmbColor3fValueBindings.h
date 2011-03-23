/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Color3fValueBindings                                    #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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

class Color3fValueBindings : public kmb::DataBindings
{
public:
	Color3fValueBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NODEVARIABLE);
	virtual ~Color3fValueBindings(void);
	virtual const char* getContainerType(void) const{
		return "Color3fValueBindings";
	};
	virtual void clear(void);
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val);
	virtual bool setPhysicalValue(kmb::idType id, double *value);
	virtual bool setValue(kmb::idType id, double value,int index=0);
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::idType id) const;
	virtual bool getPhysicalValue(kmb::idType id, double *val) const;
	virtual bool hasId(kmb::idType id) const;
	virtual bool deleteId(kmb::idType id);
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual size_t getIdCount() const;
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class Color3fValueBindings;
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
		const kmb::Color3fValueBindings* nodeValue;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	const float* getFloatArray(void) const;
	float* getFloatArray(void);
	bool setColorValue(kmb::idType id, float *rgb);
private:
	int size;
	float* values;
};

}
