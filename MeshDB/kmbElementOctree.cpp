/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ElementOctree                                           #
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
#include "MeshDB/kmbElementOctree.h"
#include "MeshDB/kmbElementEvaluator.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbTypes.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbGeometry3D.h"



double
kmb::Element3DOctree::getDistanceSq(const double x,const double y,const double z,const kmb::elementIdType elementId) const
{
	kmb::ElementContainer::const_iterator elem = elements->find( elementId );
	if( !elem.isFinished() ){
		switch( elem.getDimension() ){
		case 3:
			{
				double v = evaluator->getMinInnerVolume( elem, x, y, z );
				if( v >= 0.0 ){
					return 0.0;
				}else{
					return evaluator->getDistanceSqOnBoundary(elem,x,y,z);
				}
			}
		case 2:
			{
				return evaluator->getDistanceSq(elem,x,y,z);
			}
		default:
			break;
		}
	}
	return DBL_MAX;
}

double
kmb::Element2DOctree::getDistanceSq(const double x,const double y,const double z,const kmb::elementIdType elementId) const
{
	if( points && elements && evaluator ){
		kmb::ElementContainer::const_iterator elem = elements->find(elementId);
		if( !elem.isFinished() ){
			return evaluator->getDistanceSq(elem,x,y,z);
		}
	}
	return DBL_MAX;
}

double
kmb::Element2DOctreeWithNormal::getDistanceSq(const double x,const double y,const double z,const kmb::elementIdType elementId) const
{
	if( points && elements && evaluator ){
		kmb::ElementContainer::const_iterator elem = elements->find(elementId);
		if( !elem.isFinished() ){
			kmb::Vector3D vect;
			evaluator->getNormalVector( elem, vect );
			if( vect*normal >= 0.0 ){
				return evaluator->getDistanceSq(elem,x,y,z);
			}
		}
	}
	return DBL_MAX;
}
