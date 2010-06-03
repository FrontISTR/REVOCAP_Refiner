/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ElementOctree                                           #
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
#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbElementContainer.h"
#include "Geometry/kmb_Point3DContainer.h"
#include "Geometry/kmb_Geometry3D.h"
#include "Geometry/kmb_BoundingBox.h"

namespace kmb{

class MeshData;

class ElementOctree
{
public:
	ElementOctree(int layer=0,int thres=256);
	virtual ~ElementOctree(void);
	void clear(void);





	void generateSearchCache(kmb::MeshData* mesh,kmb::bodyIdType bodyId,double safetyRatio=1.0);







	kmb::elementIdType searchElement(double x,double y,double z, const kmb::Point3DContainer* points, const kmb::ElementContainer* elements, double* coeff,double tolerance=0.0) const;


	kmb::elementIdType searchElementWithNormal(double x,double y,double z, const Vector3D &normal, const kmb::Point3DContainer* points, const kmb::ElementContainer* elements, double* coeff,double tolerance=0.0) const;
	int getLocalCount(void) const;
	size_t getCount(void) const;

private:
	int layer;
	int thres;
	int count;
	kmb::BoundingBox boundBox;
	kmb::Point3D center;
	kmb::elementIdType *elementIds;
	kmb::ElementOctree **children;
	int getChildIndex(double x,double y,double z) const;

	double getDistanceFromChildCell(double x,double y,double z,int i) const;

	void addElement(kmb::elementIdType elementId,kmb::BoundingBox* bbox,kmb::MeshData* mesh,double safetyRatio);

	void createChildren(kmb::MeshData* mesh,double safetyRatio);
};

}
