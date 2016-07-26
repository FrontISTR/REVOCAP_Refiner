/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Collision                                               #
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
#include "MeshDB/kmbCollision.h"

#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbTriangle.h"
#include "MeshDB/kmbElementRelation.h"
#include "Geometry/kmbGeometry3D.h"
#include "Common/kmbCalculator.h"

#include <cassert>

kmb::Collision::collisionType
kmb::Collision::detectTriTri(kmb::ElementBase& tri0,kmb::ElementBase& tri1) const
{
	// 位相チェック
	int i0=0,i1=0;
	kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation( tri0, i0, tri1, i1 );
	// 2点共有している場合
	if( rel == kmb::ElementRelation::ADJACENT || rel == kmb::ElementRelation::ANTIADJACENT ){
		return kmb::Collision::TOPOLOGICAL_CONTACT;
	}
	// 1点共有している場合 => 三角形の残りの辺がもう一方の面と干渉するかどうか
	else if( rel == kmb::ElementRelation::COMMONNODE ){
		kmb::Collision::collisionType colType = kmb::Collision::TOPOLOGICAL_CONTACT;
		kmb::Point3D t0[3];
		kmb::Point3D t1[3];
		if( this->points &&
			tri0.getType() == kmb::TRIANGLE &&
			this->points->getPoint( tri0[0], t0[0] ) &&
			this->points->getPoint( tri0[1], t0[1] ) &&
			this->points->getPoint( tri0[2], t0[2] ) &&
			tri1.getType() == kmb::TRIANGLE &&
			this->points->getPoint( tri1[0], t1[0] ) &&
			this->points->getPoint( tri1[1], t1[1] ) &&
			this->points->getPoint( tri1[2], t1[2] ) )
		{
			switch( detectSegTri(
				t0[ kmb::Triangle::faceTable[i0][0] ],
				t0[ kmb::Triangle::faceTable[i0][1] ], t1[0], t1[1], t1[2] ) ){
			case kmb::Collision::DETECT:
				return kmb::Collision::DETECT;
			case kmb::Collision::GEOMETRICAL_CONTACT:
				colType = kmb::Collision::GEOMETRICAL_CONTACT;
				break;
			default:
				break;
			}
			switch( detectSegTri(
				t1[ kmb::Triangle::faceTable[i1][0] ],
				t1[ kmb::Triangle::faceTable[i1][1] ], t0[0], t0[1], t0[2] ) ){
			case kmb::Collision::DETECT:
				return kmb::Collision::DETECT;
			case kmb::Collision::GEOMETRICAL_CONTACT:
				colType = kmb::Collision::GEOMETRICAL_CONTACT;
				break;
			default:
				break;
			}
			return colType;
		}else{
			return kmb::Collision::COLLISION_ERROR;
		}
	}

	kmb::Point3D t0[3];
	kmb::Point3D t1[3];
	if( this->points &&
		tri0.getType() == kmb::TRIANGLE &&
		this->points->getPoint( tri0[0], t0[0] ) &&
		this->points->getPoint( tri0[1], t0[1] ) &&
		this->points->getPoint( tri0[2], t0[2] ) &&
		tri1.getType() == kmb::TRIANGLE &&
		this->points->getPoint( tri1[0], t1[0] ) &&
		this->points->getPoint( tri1[1], t1[1] ) &&
		this->points->getPoint( tri1[2], t1[2] ) )
	{
		return detectTriTri( t0[0], t0[1], t0[2], t1[0], t1[1], t1[2] );
	}else{
		return kmb::Collision::COLLISION_ERROR;
	}
}

kmb::Collision::collisionType
kmb::Collision::detectTriTetra(kmb::ElementBase& tri,kmb::ElementBase& tetra) const
{
	// 位相チェック
	int i0=0,i1=0;
	kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation( tri, i0, tetra, i1 );
	// 3点共有している場合
	if( rel == kmb::ElementRelation::BOUNDARY || rel == kmb::ElementRelation::ANTIBOUNDARY ){
		return kmb::Collision::TOPOLOGICAL_CONTACT; // 位相的に接している
	}
	// 2点共有している場合 => 
	//  三角形の残り1点が四面体の中にあるかどうか
	//  四面体の二点共有していない面と三角形が干渉
	else if( rel == kmb::ElementRelation::COMMONEDGE ){
		kmb::Point3D n[3];
		kmb::Point3D t[4];
		if( this->points &&
			tri.getType() == kmb::TRIANGLE &&
			this->points->getPoint( tri[0], n[0] ) &&
			this->points->getPoint( tri[1], n[1] ) &&
			this->points->getPoint( tri[2], n[2] ) &&
			tetra.getType() == kmb::TETRAHEDRON &&
			this->points->getPoint( tetra[0], t[0] ) &&
			this->points->getPoint( tetra[1], t[1] ) &&
			this->points->getPoint( tetra[2], t[2] ) &&
			this->points->getPoint( tetra[3], t[3] ) )
		{
			kmb::Collision::collisionType colType = kmb::Collision::TOPOLOGICAL_CONTACT;
			// 内部チェックは残りの一点だけ
			double minor[4] = {0.0, 0.0, 0.0, 0.0};
			kmb::Point3D::calcMinorCoordinate( t[0], t[1], t[2], t[3], n[i0], minor );
			if( 0.0 < minor[0] && 0.0 < minor[1] && 0.0 < minor[2] && 0.0 < minor[3] ){
				return kmb::Collision::DETECT;
			}
			// ここで三角形と辺のチェック
			// tetra の共有している頂点の番号
			int f0 = kmb::Tetrahedron::edgeTable[i1][0];
			int f1 = kmb::Tetrahedron::edgeTable[i1][1];
			// tri の共有している頂点の番号
			int e0 = -1, e1 = -1;
			if( tetra.indexOf( tri[ (i0+1)%3 ] ) == f0 ){
				e0 = (i0+1)%3;
				e1 = (i0+2)%3;
			}else{
				e1 = (i0+1)%3;
				e0 = (i0+2)%3;
			}
			assert( tetra[f0] == tri[e0] );
			assert( tetra[f1] == tri[e1] );
			switch ( detectSegTri( n[i0], n[e0],
				t[ kmb::Tetrahedron::faceTable[f0][0] ],
				t[ kmb::Tetrahedron::faceTable[f0][1] ],
				t[ kmb::Tetrahedron::faceTable[f0][2] ]) ){
			case kmb::Collision::DETECT:
				return kmb::Collision::DETECT;
			case kmb::Collision::GEOMETRICAL_CONTACT:
				colType = kmb::Collision::GEOMETRICAL_CONTACT;
				break;
			default:
				break;
			}
			switch ( detectSegTri( n[i0], n[e1],
				t[ kmb::Tetrahedron::faceTable[f1][0] ],
				t[ kmb::Tetrahedron::faceTable[f1][1] ],
				t[ kmb::Tetrahedron::faceTable[f1][2] ]) ){
			case kmb::Collision::DETECT:
				return kmb::Collision::DETECT;
			case kmb::Collision::GEOMETRICAL_CONTACT:
				colType = kmb::Collision::GEOMETRICAL_CONTACT;
				break;
			default:
				break;
			}
			return colType;
		}else{
			return kmb::Collision::COLLISION_ERROR;
		}
	}
	// 1点共有している場合
	else if( rel == kmb::ElementRelation::COMMONNODE ){
		kmb::Point3D n[3];
		kmb::Point3D t[4];
		if( this->points &&
			tri.getType() == kmb::TRIANGLE &&
			this->points->getPoint( tri[0], n[0] ) &&
			this->points->getPoint( tri[1], n[1] ) &&
			this->points->getPoint( tri[2], n[2] ) &&
			tetra.getType() == kmb::TETRAHEDRON &&
			this->points->getPoint( tetra[0], t[0] ) &&
			this->points->getPoint( tetra[1], t[1] ) &&
			this->points->getPoint( tetra[2], t[2] ) &&
			this->points->getPoint( tetra[3], t[3] ) )
		{
			kmb::Collision::collisionType colType = kmb::Collision::TOPOLOGICAL_CONTACT;
			// 頂点が内部にあるとき
			double minor[4] = {0.0, 0.0, 0.0, 0.0};
			for(int i=0;i<3;++i){
				if( i != i0 ){
					kmb::Point3D::calcMinorCoordinate( t[0], t[1], t[2], t[3], n[i], minor );
					if( 0.0 < minor[0] && 0.0 < minor[1] && 0.0 < minor[2] && 0.0 < minor[3] ){
						return kmb::Collision::DETECT;
					}
				}
			}
			// 三角形の対辺と四面体の面
			for(int i=0;i<4;++i){
				switch( detectSegTri( n[ (i0+1)%3 ], n[ (i0+2)%3 ],
					t[ kmb::Tetrahedron::faceTable[i][0] ],
					t[ kmb::Tetrahedron::faceTable[i][1] ],
					t[ kmb::Tetrahedron::faceTable[i][2] ] ) ){
				case kmb::Collision::DETECT:
					return kmb::Collision::DETECT;
				case kmb::Collision::GEOMETRICAL_CONTACT:
					colType = kmb::Collision::GEOMETRICAL_CONTACT;
					break;
				default:
					break;
				}
			}
			// 三角形の共辺と四面体の対面
			switch( detectSegTri( n[i0], n[ (i0+1)%3 ],
				t[ kmb::Tetrahedron::faceTable[i1][0] ],
				t[ kmb::Tetrahedron::faceTable[i1][1] ],
				t[ kmb::Tetrahedron::faceTable[i1][2] ] ) ){
			case kmb::Collision::DETECT:
				return kmb::Collision::DETECT;
			case kmb::Collision::GEOMETRICAL_CONTACT:
				colType = kmb::Collision::GEOMETRICAL_CONTACT;
				break;
			default:
				break;
			}
			switch( detectSegTri( n[i0], n[ (i0+2)%3 ],
				t[ kmb::Tetrahedron::faceTable[i1][0] ],
				t[ kmb::Tetrahedron::faceTable[i1][1] ],
				t[ kmb::Tetrahedron::faceTable[i1][2] ] ) ){
			case kmb::Collision::DETECT:
				return kmb::Collision::DETECT;
			case kmb::Collision::GEOMETRICAL_CONTACT:
				colType = kmb::Collision::GEOMETRICAL_CONTACT;
				break;
			default:
				break;
			}
			return colType;
		}else{
			return kmb::Collision::COLLISION_ERROR;
		}
	}

	kmb::Point3D n[3];
	kmb::Point3D t[4];
	if( this->points &&
		tri.getType() == kmb::TRIANGLE &&
		this->points->getPoint( tri[0], n[0] ) &&
		this->points->getPoint( tri[1], n[1] ) &&
		this->points->getPoint( tri[2], n[2] ) &&
		tetra.getType() == kmb::TETRAHEDRON &&
		this->points->getPoint( tetra[0], t[0] ) &&
		this->points->getPoint( tetra[1], t[1] ) &&
		this->points->getPoint( tetra[2], t[2] ) &&
		this->points->getPoint( tetra[3], t[3] ) )
	{
		kmb::Collision::collisionType colType = kmb::Collision::NOT_COLLIDE;
		// 頂点が内部にあるとき
		double minor[4] = {0.0, 0.0, 0.0, 0.0};
		for(int i=0;i<3;++i){
			kmb::Point3D::calcMinorCoordinate( t[0], t[1], t[2], t[3], n[i], minor );
			if( 0.0 < minor[0] && 0.0 < minor[1] && 0.0 < minor[2] && 0.0 < minor[3] ){
				return kmb::Collision::DETECT;
			}
		}
		for(int i=0;i<4;++i){
			switch( detectTriTri( n[0], n[1], n[2],
				t[ kmb::Tetrahedron::faceTable[i][0] ],
				t[ kmb::Tetrahedron::faceTable[i][1] ],
				t[ kmb::Tetrahedron::faceTable[i][2] ] ) ){
			case kmb::Collision::DETECT:
				return kmb::Collision::DETECT;
			case kmb::Collision::GEOMETRICAL_CONTACT:
				colType = kmb::Collision::GEOMETRICAL_CONTACT;
				break;
			default:
				break;
			}
		}
		return colType;
	}else{
		return kmb::Collision::COLLISION_ERROR; // エラー
	}
}
