/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ScalarValueFileBindings                                 #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
/*
 * バイナリファイル上にデータを格納してストリームでデータを走査する
 */
#pragma once
#include "MeshDB/kmbDataBindings.h"
#include "Geometry/kmb_ContainerOnDisk.h"
#include <fstream>

namespace kmb{

class ScalarValueBindings;

class ScalarValueFileBindings : public kmb::DataBindings, kmb::ContainerOnDisk
{
public:
	ScalarValueFileBindings(size_t count,const char* filename,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NODEVARIABLE);
	ScalarValueFileBindings(const char* filename,const kmb::ScalarValueBindings* data);
	virtual ~ScalarValueFileBindings(void);
	virtual const char* getContainerType(void) const{
		return "ScalarValueFileBindings";
	};
	virtual void clear(void);
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val);
	virtual bool setPhysicalValue(kmb::idType id,double* val);
	virtual bool setValue(kmb::idType id, double value,int index=0);
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::idType id) const;
	virtual bool getPhysicalValue(kmb::idType id,double *val) const;
	virtual bool hasId(kmb::idType id) const;
	virtual bool deleteId(kmb::idType id);
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual size_t getIdCount() const;
	kmb::ScalarValueBindings* createScalarValueBindings(void);
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class ScalarValueFileBindings;
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
		kmb::ScalarValueFileBindings* nodeValue;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
private:
	static const char HEADER_STRING[80];
};

}
