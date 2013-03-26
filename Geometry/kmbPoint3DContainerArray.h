/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DContainerArray                                   #
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
#include "Geometry/kmbPoint3DContainer.h"

namespace kmb{

class Point3DContainerArray : public Point3DContainer
{
private:
	double* pointArray;
	size_t index;
	size_t size;
public:
	static const char* CONTAINER_TYPE;
	Point3DContainerArray(void);
	virtual ~Point3DContainerArray(void);
	virtual bool initialize(size_t size);
	virtual nodeIdType addPoint(const double x,const double y,const double z);
	virtual nodeIdType addPoint(const kmb::Point3D& point);
	virtual nodeIdType addPoint(const double x,const double y,const double z,const nodeIdType id);
	virtual nodeIdType addPoint(const kmb::Point3D& point,const nodeIdType id);
	virtual bool getXYZ(nodeIdType id,double &x,double &y,double &z) const;
	virtual bool getPoint(nodeIdType id,kmb::Point3D &point) const;
	virtual nodeIdType getMaxId(void) const;
	virtual nodeIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual bool deletePoint(nodeIdType id);
	virtual const char* getContainerType(void) const{
		return kmb::Point3DContainerArray::CONTAINER_TYPE;
	}

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::nodeIdType nodeId);
	virtual const_iterator find(kmb::nodeIdType nodeId) const;

	const double* getDoubleArray(void) const { return pointArray; }

protected:
	class _iteratorArray : public Point3DContainer::_iterator
	{
		friend class Point3DContainerArray;
	public:
		virtual nodeIdType getId(void) const;
		virtual bool getXYZ(double &x,double &y,double &z) const;
		virtual bool getPoint(kmb::Point3D &point) const;
		virtual double x() const;
		virtual double y() const;
		virtual double z() const;
		virtual bool setXYZ(double x,double y,double z) const;
		virtual bool setPoint(kmb::Point3D &point) const;
		virtual Point3DContainer::_iterator* operator++(void);
		virtual Point3DContainer::_iterator* operator++(int n);
		virtual Point3DContainer::_iterator* clone(void);
	private:
		_iteratorArray(void){};
		~_iteratorArray(void){};
		size_t index;
		const Point3DContainerArray* points;
	};
};

class Point3DContainerXYZArray : public Point3DContainer
{
private:
	double* xArray;
	double* yArray;
	double* zArray;
	size_t index;
	size_t size;
public:
	static const char* CONTAINER_TYPE;
	Point3DContainerXYZArray(void);
	virtual ~Point3DContainerXYZArray(void);
	virtual bool initialize(size_t size);
	virtual nodeIdType addPoint(const double x,const double y,const double z);
	virtual nodeIdType addPoint(const kmb::Point3D& point);
	virtual nodeIdType addPoint(const double x,const double y,const double z,const nodeIdType id);
	virtual nodeIdType addPoint(const kmb::Point3D& point,const nodeIdType id);
	virtual bool getXYZ(nodeIdType id,double &x,double &y,double &z) const;
	virtual bool getPoint(nodeIdType id,kmb::Point3D &point) const;
	virtual nodeIdType getMaxId(void) const;
	virtual nodeIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual bool deletePoint(nodeIdType id);
	virtual const char* getContainerType(void) const{
		return kmb::Point3DContainerXYZArray::CONTAINER_TYPE;
	}

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::nodeIdType nodeId);
	virtual const_iterator find(kmb::nodeIdType nodeId) const;

protected:
	class _iteratorXYZ : public Point3DContainer::_iterator
	{
		friend class Point3DContainerXYZArray;
	public:
		virtual nodeIdType getId(void) const;
		virtual bool getXYZ(double &x,double &y,double &z) const;
		virtual bool getPoint(kmb::Point3D &point) const;
		virtual double x() const;
		virtual double y() const;
		virtual double z() const;
		virtual bool setXYZ(double x,double y,double z) const;
		virtual bool setPoint(kmb::Point3D &point) const;
		virtual Point3DContainer::_iterator* operator++(void);
		virtual Point3DContainer::_iterator* operator++(int n);
		virtual Point3DContainer::_iterator* clone(void);
	private:
		_iteratorXYZ(void){};
		~_iteratorXYZ(void){};
		size_t index;
		const Point3DContainerXYZArray* points;
	};
};

}

