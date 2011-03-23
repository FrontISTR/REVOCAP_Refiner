/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Tensor6ValueFileBindings                                #
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
/**
 * Tensor6ValueBindings
 * データの順番は xx yy zz xy yz zx
 */
/*
 * バイナリファイル上にデータを格納してストリームでデータを走査する
 */
#pragma once
#include "MeshDB/kmbDataBindings.h"
#include "Geometry/kmb_ContainerOnDisk.h"
#include <fstream>

namespace kmb{

class Tensor6ValueBindings;

class Tensor6ValueFileBindings : public kmb::DataBindings, public kmb::ContainerOnDisk
{
public:

	Tensor6ValueFileBindings(size_t count,const char* filename,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NODEVARIABLE);
	Tensor6ValueFileBindings(const char* filename,const kmb::Tensor6ValueBindings* data);
	virtual ~Tensor6ValueFileBindings(void);
	virtual const char* getContainerType(void) const{
		return "Tensor6ValueFileBindings";
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
	kmb::Tensor6ValueBindings* createTensor6ValueBindings(void);
public:
	class _iterator : public DataBindings::_iterator
	{
		friend class Tensor6ValueFileBindings;
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
		const kmb::Tensor6ValueFileBindings* nodeValue;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
private:
	static const char HEADER_STRING[80];
};

}
