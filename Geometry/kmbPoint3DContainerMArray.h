/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DContainerMArray                                  #
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
#include "Common/kmbBLArray.h"

namespace kmb{

class Point3DContainerMArray : public Point3DContainer
{
private:
	BLArray<double,3> pointArray;
	BLArrayIndex aIndex;
	size_t count;
public:
	static const char* CONTAINER_TYPE;
	Point3DContainerMArray(void);
	virtual ~Point3DContainerMArray(void);
	virtual bool initialize(size_t size=0);
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
		return kmb::Point3DContainerMArray::CONTAINER_TYPE;
	}

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::nodeIdType nodeId);
	virtual const_iterator find(kmb::nodeIdType nodeId) const;

protected:
	class _iteratorMArray : public Point3DContainer::_iterator
	{
		friend class Point3DContainerMArray;
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
		_iteratorMArray(void);
		virtual ~_iteratorMArray(void);
		Point3DContainerMArray* points;
		BLArrayIndex aIndex;
	};
};

}
