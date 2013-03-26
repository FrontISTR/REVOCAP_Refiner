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
#include "Geometry/kmbPoint2DContainer.h"
#include "Matrix/kmbMatrix.h"
#include "Common/kmbIntegration.h"

bool
kmb::ElementEvaluator::getStiffMatrix(const kmb::ElementBase &element,kmb::SquareMatrix &stiff) const
{
	int nCount = element.getNodeCount();
	if( points == NULL && point2Ds == NULL && nCount != stiff.getSize() ){
		return false;
	}
	if( points ){
		kmb::Matrix3x3 jacobi;
		switch( element.getType() ){
			case kmb::TETRAHEDRON:
			{

				kmb::Point3D q[4];
				for(int i=0;i<4;++i){
					points->getPoint( element[i],q[i]);
				}
				jacobi.set(0,0,q[1].x()-q[0].x());
				jacobi.set(0,1,q[1].y()-q[0].y());
				jacobi.set(0,2,q[1].z()-q[0].z());
				jacobi.set(1,0,q[2].x()-q[0].x());
				jacobi.set(1,1,q[2].y()-q[0].y());
				jacobi.set(1,2,q[2].z()-q[0].z());
				jacobi.set(2,0,q[3].x()-q[0].x());
				jacobi.set(2,1,q[3].y()-q[0].y());
				jacobi.set(2,2,q[3].z()-q[0].z());
				double jac = jacobi.determinant();

				kmb::Vector3D v[4], u[4];
				v[0].setCoordinate(-1.0,-1.0,-1.0);
				v[1].setCoordinate( 1.0, 0.0, 0.0);
				v[2].setCoordinate( 0.0, 1.0, 0.0);
				v[3].setCoordinate( 0.0, 0.0, 1.0);
				jacobi.solve(v[0],u[0]);
				jacobi.solve(v[1],u[1]);
				jacobi.solve(v[2],u[2]);
				jacobi.solve(v[3],u[3]);

				stiff.zero();
				for(int i=0;i<4;++i){
					double v = (u[i]*u[i]) * jac / 6.0;
					stiff.set(i,i,v);
					for(int j=i+1;j<4;++j){
						v = (u[i]*u[j]) * jac / 6.0;
						stiff.set(i,j,v);
						stiff.set(j,i,v);
					}
				}
				return true;
			}
			default:
			{
				break;
			}
		}
	}else if( point2Ds ){
		kmb::Matrix2x2 jacobi;
		switch( element.getType() ){
			case kmb::TRIANGLE:
			{

				kmb::Point2D q[3];
				for(int i=0;i<3;++i){
					point2Ds->getPoint( element[i],q[i]);
				}
				jacobi.set(0,0,q[1].x()-q[0].x());
				jacobi.set(0,1,q[1].y()-q[0].y());
				jacobi.set(1,0,q[2].x()-q[0].x());
				jacobi.set(1,1,q[2].y()-q[0].y());
				double jac = jacobi.determinant();

				kmb::Vector2D v[3], u[3];
				v[0].setCoordinate(-1.0,-1.0);
				v[1].setCoordinate( 1.0, 0.0);
				v[2].setCoordinate( 0.0, 1.0);
				jacobi.solve(v[0],u[0]);
				jacobi.solve(v[1],u[1]);
				jacobi.solve(v[2],u[2]);

				stiff.zero();
				for(int i=0;i<3;++i){
					double v = (u[i]*u[i]) * jac / 2.0;
					stiff.set(i,i,v);
					for(int j=i+1;j<3;++j){
						v = (u[i]*u[j]) * jac / 2.0;
						stiff.set(i,j,v);
						stiff.set(j,i,v);
					}
				}
				return true;
			}
			case kmb::QUAD:
			{
				kmb::Point2D q[4];
				for(int i=0;i<4;++i){
					point2Ds->getPoint( element[i],q[i]);
				}

				stiff.zero();
				for(int si=0;si<2;++si){
					double s = kmb::Integration::Gauss_Point2[si];
					for(int ti=0;ti<2;++ti){
						double t = kmb::Integration::Gauss_Point2[ti];
						double w = kmb::Integration::Gauss_Weight2[si] * kmb::Integration::Gauss_Weight2[ti];

						jacobi.zero();
						jacobi.set(0,0,
							0.25 * (-q[0].x()+q[1].x()+q[2].x()-q[3].x()) +
							0.25 * ( q[0].x()-q[1].x()+q[2].x()-q[3].x()) * t );
						jacobi.set(0,1,
							0.25 * (-q[0].y()+q[1].y()+q[2].y()-q[3].y()) +
							0.25 * ( q[0].y()-q[1].y()+q[2].y()-q[3].y()) * t );
						jacobi.set(1,0,
							0.25 * (-q[0].x()-q[1].x()+q[2].x()+q[3].x()) +
							0.25 * ( q[0].x()-q[1].x()+q[2].x()-q[3].x()) * s );
						jacobi.set(1,1,
							0.25 * (-q[0].y()-q[1].y()+q[2].y()+q[3].y()) +
							0.25 * ( q[0].y()-q[1].y()+q[2].y()-q[3].y()) * s );
						double jac = jacobi.determinant();

						kmb::Vector2D v[4], u[4];
						v[0].setCoordinate(0.25*(-1.0+t),0.25*(-1.0+s));
						v[1].setCoordinate(0.25*( 1.0-t),0.25*(-1.0-s));
						v[2].setCoordinate(0.25*( 1.0+t),0.25*( 1.0+s));
						v[3].setCoordinate(0.25*(-1.0-t),0.25*( 1.0-s));
						jacobi.solve(v[0],u[0]);
						jacobi.solve(v[1],u[1]);
						jacobi.solve(v[2],u[2]);
						jacobi.solve(v[3],u[3]);
						for(int i=0;i<4;++i){
							double v = (u[i]*u[i]) * jac * w;
							stiff.add(i,i,v);
							for(int j=i+1;j<4;++j){
								v = (u[i]*u[j]) * jac * w;
								stiff.add(i,j,v);
								stiff.add(j,i,v);
							}
						}
					}
				}
				return true;
			}
			default:
				break;
		}
	}
	return false;
}
