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
#include <cmath>
#include <cfloat>

#include "MeshDB/kmbElementEvaluator.h"

#include "Geometry/kmbGeometry.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbPoint3DContainer.h"

#include "MeshDB/kmbQuad.h"

int
kmb::ElementEvaluator::getConcaveInQuad(const kmb::ElementBase &quad) const
{
	if( points == NULL ){
		return -1;
	}
	if( quad.getType() == kmb::QUAD ){
		kmb::Vector3D v0 = points->calcNormalVector( quad.getCellId(3), quad.getCellId(0), quad.getCellId(1) );
		kmb::Vector3D v1 = points->calcNormalVector( quad.getCellId(0), quad.getCellId(1), quad.getCellId(2) );
		kmb::Vector3D v2 = points->calcNormalVector( quad.getCellId(1), quad.getCellId(2), quad.getCellId(3) );
		kmb::Vector3D v3 = points->calcNormalVector( quad.getCellId(2), quad.getCellId(3), quad.getCellId(0) );
		double p01 = v0*v1;
		double p12 = v1*v2;
		double p23 = v2*v3;
		double p30 = v3*v0;
		if( p30 < 0.0 && p01 < 0.0 )	return 0;
		if( p01 < 0.0 && p12 < 0.0 )	return 1;
		if( p12 < 0.0 && p23 < 0.0 )	return 2;
		if( p23 < 0.0 && p30 < 0.0 )	return 3;
	}
	else if( quad.getType() == kmb::HEXAHEDRON ){
		kmb::Quad q;
		for(int i=0;i<6;++i){
			if( quad.getBoundaryElement(i,q) && getConcaveInQuad(q) != -1 ){
				return i;
			}
		}
	}
	return -1;
}
