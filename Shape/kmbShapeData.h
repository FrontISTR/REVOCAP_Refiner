/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ShapeData                                               #
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

#ifdef OPENCASCADE

#include <vector>

#include "Geometry/kmbBoundingBox.h"

class TopoDS_Shape;
class TopoDS_Face;
class Handle_Geom_Surface;

namespace kmb{

class Surface3D;
class MeshData;

class ShapeData
{
private:
	TopoDS_Shape* topo_shape;
public:
	ShapeData(void);
	virtual ~ShapeData(void);
	const TopoDS_Shape& getShape(void) const;
	TopoDS_Shape& getShape(void);
	void convertToBezier(void);
	void fixShape(double precision,double tolerance);
	void dropSmallEdge(double precision,double tolerance);
	bool isClosed(void) const;
	bool isValid(void) const;
	kmb::BoundingBox getBoundingBox(void) const;



	int getSurfaces( std::vector<kmb::Surface3D*> &surfaces) const;
	int saveToRNF(const char* filename, bool append=false ) const;
private:

	int getBSplineSurface( TopoDS_Face &face, Handle_Geom_Surface &surf, std::vector<kmb::Surface3D*> &surfaces) const;
	int getBezierSurface( TopoDS_Face &face, Handle_Geom_Surface &surf, std::vector<kmb::Surface3D*> &surfaces) const;
	int getCylindricalSurface( TopoDS_Face &face, Handle_Geom_Surface &surf, std::vector<kmb::Surface3D*> &surfaces) const;
	int getSphericalSurface( TopoDS_Face &face, Handle_Geom_Surface &surf, std::vector<kmb::Surface3D*> &surfaces) const;
	int getPlane( TopoDS_Face &face, Handle_Geom_Surface &surf, std::vector<kmb::Surface3D*> &surfaces) const;
};

}

#endif
