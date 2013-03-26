/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point3DContainer                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include <vector>
#include <cfloat>
#include "Geometry/kmbPoint3DContainer.h"

namespace kmb{

class Point3DContainerVect : public Point3DContainer
{
private:
	std::vector< kmb::Point3D* > points;
public:
	static const char* CONTAINER_TYPE;
	Point3DContainerVect(void);
	virtual ~Point3DContainerVect(void);
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
		return kmb::Point3DContainerVect::CONTAINER_TYPE;
	}

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::nodeIdType nodeId);
	virtual const_iterator find(kmb::nodeIdType nodeId) const;

protected:
	class _iteratorVect : public Point3DContainer::_iterator
	{
		friend class Point3DContainerVect;
	public:
		virtual nodeIdType getId(void) const;
		virtual bool getXYZ(double &x,double &y,double &z) const;
		virtual bool getPoint(kmb::Point3D &point) const;
		virtual double x() const;
		virtual double y() const;
		virtual double z() const;
		virtual bool setXYZ(double x,double y,double z) const;
		virtual bool setPoint(kmb::Point3D &point) const ;
		virtual Point3DContainer::_iterator* operator++(void);
		virtual Point3DContainer::_iterator* operator++(int n);
		virtual Point3DContainer::_iterator* clone(void);
	private:
		_iteratorVect(void){};
		~_iteratorVect(void){};
		int index;
		const Point3DContainerVect* vect;
	};
};

}

