/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementEvaluator                                        #
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

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbFace.h"
#include "MeshDB/kmbElementContainer.h"
#include "Geometry/kmbGeometry3D.h"

namespace kmb{

class Point3DContainer;
class Point2DContainer;
class BoundingBox;
class SquareMatrix;

class ElementEvaluator
{
public:
	ElementEvaluator(const kmb::Point3DContainer* points);
	ElementEvaluator(const kmb::Point2DContainer* point2Ds);
	virtual ~ElementEvaluator(void);

	double getAspectRatio(const kmb::ElementBase &eIter) const;
	double getAspectRatio(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;

	double getInscribedRadius(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;

	double getCircumRadius(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;

	double getEdgeLengthRatio(const kmb::ElementBase &eIter) const;
	double getMaxEdgeLength(const kmb::ElementBase &eIter) const;
	double getAverageEdgeLength(const kmb::ElementBase &eIter) const;

	double getMinAngleTri(const kmb::ElementBase &eIter) const;

	double getMinAngle(const kmb::ElementBase &eIter) const;

	double getMaxAngle(const kmb::ElementBase &eIter) const;

	double getVolume(const kmb::ElementBase &eIter) const;
	double getVolume(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;

	double getArea(const kmb::ElementBase &eIter) const;
	double getArea(const kmb::ElementBase &eIter,kmb::idType localFaceId) const;

	double getLength(const kmb::ElementBase &eIter) const;

	double getAngle(const kmb::ElementBase &eIter,kmb::nodeIdType nodeId) const;
	double getAngleByIndex(const kmb::ElementBase &eIter,int index) const;
	double getCosByIndex(const kmb::ElementBase &eIter,int index) const;


	double getSharpness(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3) const;

	bool getBoundingBox(const kmb::ElementBase &element,kmb::BoundingBox &bbox) const;
	double getBoundingBoxRadius(const kmb::ElementBase &element) const;


	bool getNormalVector(const kmb::ElementBase &element, kmb::Vector3D &vect) const;
	bool getNormalVector(const Face &face, const kmb::ElementContainer* elements, kmb::Vector3D &vect) const;
	bool getNormalVectorOfFace(const kmb::ElementBase &element, int index, kmb::Vector3D &vect) const;

	bool getNaturalCoordinates(const kmb::ElementBase &element,const double x,const double y,const double z,double* retvals) const;
	bool getNaturalCoordinatesOfFace(const kmb::ElementBase &element,int index,const double x,const double y,const double z,double* retvals) const;
	double checkShapeFunctionDomain(const kmb::ElementBase &element,double s,double t,double u);



	double getWeightElement(const kmb::ElementBase &element,const double x,const double y,const double z,double* weights) const;
	double getWeightElementFace(const kmb::ElementBase &element,int index,const double x,const double y,const double z,double* weights) const;

	bool getPhysicalCoordinates(const kmb::ElementBase &element,const double s,const double t,const double u,kmb::Point3D &target) const;


	double getMinInnerVolume(const kmb::ElementBase &element,const double x,const double y,const double z) const;


	double getDistanceSq(const kmb::ElementBase &element,const double x,const double y,const double z) const;

	double getDistanceSqOnBoundary(const kmb::ElementBase &element,const double x,const double y,const double z) const;



	double getAngleBetweenElements(const kmb::ElementBase &element0,const kmb::ElementBase &element1) const;


	double getCosBetweenElements(const kmb::ElementBase &element0,const kmb::ElementBase &element1) const;


	int getConcaveInQuad(const kmb::ElementBase &quad) const;


	bool getMinMaxJacobian(const kmb::ElementBase &element, double &min, double &max) const;


	int getDuplicationNodeIdCount(const kmb::ElementBase &element) const;



	bool getStiffMatrix(const kmb::ElementBase &element,kmb::SquareMatrix &stiff) const;
protected:
	const kmb::Point3DContainer* points;
	const kmb::Point2DContainer* point2Ds;
private:
	double getMaxAngleCos(const kmb::ElementBase &eIter) const;
	double getMinAngleCos(const kmb::ElementBase &eIter) const;
};

}
