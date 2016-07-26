/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PatchOperation                                          #
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

#include "MeshGen/kmbEdgeStitcher.h"
#include "Common/kmbCalculator.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbMatching.h"
#include "MeshGen/kmbMeshSmoother.h"
#include <cmath>

// local function
bool addTriangle( kmb::ElementContainer* triangles, kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2 )
{
	if( triangles && n0 != n1 && n1 != n2 && n2 != n0 ){
		kmb::nodeIdType tri[3] = {n0, n1, n2};
		triangles->addElement( kmb::TRIANGLE, tri );
		return true;
	}else{
		return false;
	}
}

kmb::EdgeStitcher::EdgeStitcher(void)
: mesh(NULL)
, points(NULL)
, f0(kmb::nullNodeId)
, fend(kmb::nullNodeId)
, b0(kmb::nullNodeId)
, bend(kmb::nullNodeId)
, normalCheck(false)
{
}

kmb::EdgeStitcher::~EdgeStitcher(void)
{
}

void
kmb::EdgeStitcher::setMesh(kmb::MeshData* mesh)
{
	this->mesh = mesh;
	if( mesh ){
		this->points = mesh->getNodes();
	}
}

bool
kmb::EdgeStitcher::setEdges(kmb::bodyIdType forwardId,kmb::bodyIdType backwardId)
{
	if( mesh ){
		kmb::ElementContainer* f = mesh->getBodyPtr(forwardId);
		kmb::ElementContainer* b = mesh->getBodyPtr(backwardId);
		if( f != NULL && f->isUniqueType( kmb::SEGMENT )  &&
			b != NULL && b->isUniqueType( kmb::SEGMENT )  )
		{
			this->forward.setEdges( f );
			this->backward.setEdges( b );
			normal.zero();
			return true;
		}
	}
	return false;
}

bool
kmb::EdgeStitcher::setEdges(const char* forward,const char* backward)
{
	if( mesh ){
		kmb::DataBindings* fData = mesh->getDataBindingsPtr(forward);
		kmb::DataBindings* bData = mesh->getDataBindingsPtr(backward);
		if( fData && fData->getBindingMode() == kmb::DataBindings::FaceGroup && fData->getTargetBodyId() != kmb::Body::nullBodyId &&
			bData && bData->getBindingMode() == kmb::DataBindings::FaceGroup && bData->getTargetBodyId() != kmb::Body::nullBodyId )
		{
			kmb::ElementContainer* fBody = mesh->getBodyPtr( fData->getTargetBodyId() );
			kmb::ElementContainer* bBody = mesh->getBodyPtr( bData->getTargetBodyId() );
			this->forward.setEdgesByFaceGroup( fData, fBody );
			this->backward.setEdgesByFaceGroup( bData, bBody );
			normal.zero();
			return true;
		}
	}
	return false;
}

bool
kmb::EdgeStitcher::setEdgesWithinDistance(kmb::bodyIdType forward,kmb::bodyIdType backward,double distance)
{
	if( mesh ){
		kmb::ElementContainer* f = mesh->getBodyPtr(forward);
		kmb::ElementContainer* b = mesh->getBodyPtr(backward);
		if( f != NULL && f->isUniqueType( kmb::SEGMENT )  &&
			b != NULL && b->isUniqueType( kmb::SEGMENT )  )
		{
			// f と b の距離が distance 以下
			// のものを登録する

			kmb::ElementContainerMap fnear;
			kmb::ElementContainerMap bnear;
			kmb::elementIdType nearestId;
			kmb::Point3D p0, p1;
			kmb::nodeIdType cell[2] = {kmb::nullNodeId,kmb::nullNodeId};
			double param = 0.0;

			kmb::ElementContainer::iterator eIterf = f->begin();
			while( !eIterf.isFinished() )
			{
				if( points->getPoint( eIterf.getCellId(0), p0 ) &&
					points->getPoint( eIterf.getCellId(1), p1 ) )
				{
					if( kmb::Matching::getDistanceEdgeToNodeWithParam( points, b, &p0, nearestId, param ) < distance &&
						kmb::Matching::getDistanceEdgeToNodeWithParam( points, b, &p1, nearestId, param ) < distance )
					{
						cell[0] = eIterf.getCellId(0);
						cell[1] = eIterf.getCellId(1);
						fnear.addElement( kmb::SEGMENT, cell );
					}
				}
				++eIterf;
			}

			kmb::ElementContainer::iterator eIterb = b->begin();
			while( !eIterb.isFinished() )
			{
				if( points->getPoint( eIterb.getCellId(0), p0 ) &&
					points->getPoint( eIterb.getCellId(1), p1 ) )
				{
					if( kmb::Matching::getDistanceEdgeToNodeWithParam( points, f, &p0, nearestId, param ) < distance &&
						kmb::Matching::getDistanceEdgeToNodeWithParam( points, f, &p1, nearestId, param ) < distance )
					{
						cell[0] = eIterb.getCellId(0);
						cell[1] = eIterb.getCellId(1);
						bnear.addElement( kmb::SEGMENT, cell );
					}
				}
				++eIterb;
			}
			// Polygon は内部で edges の複製を作る
			this->forward.setEdges( &fnear );
			this->backward.setEdges( &bnear );
			kmb::nodeIdType initF=kmb::nullNodeId, endF=kmb::nullNodeId, initB=kmb::nullNodeId, endB=kmb::nullNodeId;
			// 連結性チェック
			if( !this->forward.getEndPoints(initF,endF) || !this->backward.getEndPoints(initB,endB) ){
				// どちらかが連結ではない
				return false;
			}
			normal.zero();
			return true;
		}
	}
	return false;
}

bool
kmb::EdgeStitcher::setEdgesWithInitNodes(kmb::bodyIdType forward,kmb::nodeIdType forwardInit,kmb::bodyIdType backward,kmb::nodeIdType backwardInit)
{
	if( setEdges(forward,backward) ){
		setInitialNode( forwardInit, backwardInit );
		return true;
	}
	return false;
}

kmb::bodyIdType
kmb::EdgeStitcher::stitchEdgesByTriangles(void)
{
	if( mesh && points && forward.getSize() > 0 && backward.getSize() > 0 ){
		setInitialNode();
		if( normalCheck ){
			setInitNormal();
		}else{
			normal.zero();
		}
		kmb::ElementContainerMap* result = NULL;
		result = new kmb::ElementContainerMap();
		while( this->stitchByTriangle(result) ){
		}
		if( result->getCount() > 0 ){
			return mesh->appendBody( result );
		}else{
			delete result;
		}
	}
	return kmb::Body::nullBodyId;
}

kmb::bodyIdType
kmb::EdgeStitcher::stitchEdgesByQuads(void)
{
	if( mesh && points && forward.getSize() > 0 && backward.getSize() > 0 ){
		setInitialNode();
		if( normalCheck ){
			setInitNormal();
		}else{
			normal.zero();
		}
		kmb::ElementContainerMap* result = NULL;
		result = new kmb::ElementContainerMap();
		while( this->stitchByQuad(result) ){
		}
		if( result->getCount() > 0 ){
			return mesh->appendBody( result );
		}else{
			delete result;
		}
	}
	return kmb::Body::nullBodyId;
}

void
kmb::EdgeStitcher::setNormalCheck(bool check)
{
	this->normal.zero();
	this->normalCheck = check;
}

bool
kmb::EdgeStitcher::getNormalCheck(void) const
{
	return this->normalCheck;
}

void
kmb::EdgeStitcher::setInitNormal(void)
{
	// 初期法線ベクトルは + edgesbackward の法線ベクトルと - edgesforward の法線ベクトルの和
	kmb::nodeIdType n0 = b0;
	kmb::nodeIdType n1 = backward.getNodeId( n0, -1 );
	kmb::nodeIdType n2 = backward.getNodeId( n1, -1 );
	do{
		normal += points->calcVerticalVector(n0,n1,n2);
		n1 = n2;
		n2 = backward.getNodeId( n1, -1 );
	}while( n2 != kmb::nullNodeId && n2 != b0 );
	n0 = f0;
	n1 = forward.getNodeId( n0, 1 );
	n2 = forward.getNodeId( n1, 1 );
	do{
		normal -= points->calcVerticalVector(n0,n1,n2);
		n1 = n2;
		n2 = forward.getNodeId( n1, 1 );
	}while( n2 != kmb::nullNodeId && n2 != f0 );
}

void
kmb::EdgeStitcher::setInitialNode( kmb::nodeIdType forwardInit, kmb::nodeIdType backwardInit )
{
	this->f0 = forwardInit;
	this->b0 = backwardInit;
	if( f0 != kmb::nullNodeId && b0 != kmb::nullNodeId ){
		return;
	}
	kmb::nodeIdType initF=kmb::nullNodeId, endF=kmb::nullNodeId, initB=kmb::nullNodeId, endB=kmb::nullNodeId;
	// 連結性チェック
	if( !forward.getEndPoints(initF,endF) || !backward.getEndPoints(initB,endB) ){
		// どちらかが連結ではない
		return;
	}
	if( initF == kmb::nullNodeId && endB == kmb::nullNodeId ){
		// 両方ループの時
		if( f0 != kmb::nullNodeId ){
			initF = f0;
		}else if( b0 != kmb::nullNodeId ){
			endB = b0;
		}else{
			initF = forward.getEdges()->begin().getCellId(0);
		}
	}
	if( initF == kmb::nullNodeId && endB != kmb::nullNodeId ){
		// backward がループじゃない
		// forward の中から backward の最近点を探す
		kmb::Point3D node, target;
		if( points->getPoint( endB, node ) ){
			kmb::Minimizer min;
			kmb::ElementContainer::const_iterator eIter = forward.getEdges()->begin();
			while( !eIter.isFinished() ){
				if( points->getPoint( eIter.getCellId(0), target ) ){
					if( min.update( node.distanceSq( target ) ) ){
						initF = eIter.getCellId(0);
					}
				}
				++eIter;
			}
		}
	}
	if( initF != kmb::nullNodeId && endB == kmb::nullNodeId ){
		// backward の中から forward の最近点を探す
		kmb::Point3D node, target;
		if( points->getPoint( initF, node ) ){
			kmb::Minimizer min;
			kmb::ElementContainer::const_iterator eIter = backward.getEdges()->begin();
			while( !eIter.isFinished() ){
				if( points->getPoint( eIter.getCellId(0), target ) ){
					if( min.update( node.distanceSq( target ) ) ){
						endB = eIter.getCellId(0);
					}
				}
				++eIter;
			}
		}
	}
	f0 = initF;
	b0 = endB;
}

// 多角形 forward の点 f0 を順方向に
// 多角形 backward の点 b0 を逆方向に
// 三角形を作る（1ステップだけ）
// fend bend に到達したらそれ以上は進めない
bool
kmb::EdgeStitcher::stitchByTriangle( kmb::ElementContainer* result )
{
	if( result == NULL ){
		return false;
	}
	kmb::nodeIdType f1 = (f0!=fend) ? forward.getNodeId(f0,1) : kmb::nullNodeId;
	kmb::nodeIdType b1 = (b0!=bend) ? backward.getNodeId(b0,-1) : kmb::nullNodeId;
	if( normalCheck && normal * points->calcVerticalVector( f0, b0, b1) < 0.0 ){
		b1 = kmb::nullNodeId;
	}
	if( normalCheck && normal * points->calcVerticalVector( f0, b0, f1) < 0.0 ){
		f1 = kmb::nullNodeId;
	}
	if( f1 == kmb::nullNodeId ){
		if( b1 == kmb::nullNodeId ){
			return false;
		}else{
			// b1 のみ使える
			addTriangle( result, f0, b0, b1 );
			if( normalCheck ){
				normal = points->calcVerticalVector( f0, b0, b1 );
			}
			if( bend == kmb::nullNodeId ){
				bend = b0;
			}
			b0 = b1;
		}
	}else{
		if( b1 == kmb::nullNodeId ){
			// f1 のみ使える
			addTriangle( result, f0, b0, f1 );
			if( normalCheck ){
				normal = points->calcVerticalVector( f0, b0, f1 );
			}
			if( fend == kmb::nullNodeId ){
				fend = f0;
			}
			f0 = f1;
		}else{
			// 両方使える時は角度で判断
			kmb::Point3D pf0,pf1,pb0,pb1;
			if( points->getPoint(f0,pf0) &&
				points->getPoint(f1,pf1) &&
				points->getPoint(b0,pb0) &&
				points->getPoint(b1,pb1) )
			{
				// f0 f1 b1 b0 からなる四角形で cos が一番小さくなるところを分割するような三角形にする
				kmb::nodeIdType nextId = kmb::nullNodeId;
				kmb::Minimizer min;
				if( min.update( Point3D::cos( pb0, pf0, pf1 ) ) ){
					nextId = b1;
				}
				if( min.update( Point3D::cos( pf1, pb1, pb0 ) ) ){
					nextId = b1;
				}
				if( min.update( Point3D::cos( pb1, pb0, pf0 ) ) ){
					nextId = f1;
				}
				if( min.update( Point3D::cos( pf0, pf1, pb1 ) ) ){
					nextId = f1;
				}
				if( nextId == b1 ){
					addTriangle( result, f0, b0, b1 );
					if( normalCheck ){
						normal = points->calcVerticalVector( f0, b0, b1 );
					}
					if( bend == kmb::nullNodeId ){
						bend = b0;
					}
					b0 = b1;
				}else{
					addTriangle( result, f0, b0, f1 );
					if( normalCheck ){
						normal = points->calcVerticalVector( f0, b0, f1 );
					}
					if( fend == kmb::nullNodeId ){
						fend = f0;
					}
					f0 = f1;
				}
			}else{
				return false;
			}
		}
	}
	return true;
}

// 多角形 forward の点 f0 を順方向に
// 多角形 backward の点 b0 を逆方向に
// 四角形を作る
// fend bend に到達したらそれ以上は進めない
// 選択肢が複数ある時には法線ベクトルと辺の長さで判断する
bool
kmb::EdgeStitcher::stitchByQuad( kmb::ElementContainer* result )
{
	if( result == NULL ){
		return false;
	}
	kmb::nodeIdType f1 = (f0!=fend) ? forward.getNodeId(f0,1) : kmb::nullNodeId;
	kmb::nodeIdType f2 = (f1!=fend) ? forward.getNodeId(f1,1) : kmb::nullNodeId;
	kmb::nodeIdType b1 = (b0!=bend) ? backward.getNodeId(b0,-1) : kmb::nullNodeId;
	kmb::nodeIdType b2 = (b1!=bend) ? backward.getNodeId(b1,-1) : kmb::nullNodeId;
	unsigned int flag = 0;
	// 頂点を進めるかどうかの判定
	if( f1 != kmb::nullNodeId ) flag |= 0x01;
	if( f2 != kmb::nullNodeId ) flag |= 0x02;
	if( b1 != kmb::nullNodeId ) flag |= 0x04;
	if( b2 != kmb::nullNodeId ) flag |= 0x08;
	if( (flag&0x09)==0x09 && normalCheck && normal * points->calcNormalVector( f0, b0, f1 ) <= 0.0 ){
		f1 = kmb::nullNodeId;
		f2 = kmb::nullNodeId;
		flag &= ~0x03;
	}else if( (flag&0x06)==0x06 && normalCheck && normal * points->calcNormalVector( f1, b0, f2 ) <= 0.0 ){
		f2 = kmb::nullNodeId;
		flag &= ~0x02;
	}
	if( (flag&0x06)==0x06 && normalCheck && normal * points->calcNormalVector( f0, b0, b1 ) <= 0.0 ){
		b1 = kmb::nullNodeId;
		b2 = kmb::nullNodeId;
		flag &= ~0x0c;
	}else if( (flag&0x09)==0x09 && normalCheck && normal * points->calcNormalVector( f0, b1, b2 ) <= 0.0 ){
		b2 = kmb::nullNodeId;
		flag &= ~0x08;
	}
	kmb::nodeIdType nodes[4] = {f0,b0,kmb::nullNodeId,kmb::nullNodeId};
	switch(flag){
		case 0x00: // どちらの辺もすでに終点
		case 0x01: // f1 forward 1回しか進めない
		case 0x04: // b1 backward 1回しか進めない
			return false;
		case 0x03: // f1 f2
			nodes[2] = f2;
			nodes[3] = f1;
			result->addElement( kmb::QUAD, nodes );
			// ループの時のために終了点を覚えておく
			if( fend == kmb::nullNodeId ){
				fend = f0;
			}
			f0 = f2;
			normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
			return true;
		case 0x05: // f1 b1
			nodes[2] = b1;
			nodes[3] = f1;
			result->addElement( kmb::QUAD, nodes );
			// ループの時のために終了点を覚えておく
			if( fend == kmb::nullNodeId ){
				fend = f0;
			}
			if( bend == kmb::nullNodeId ){
				bend = b0;
			}
			f0 = f1;
			b0 = b1;
			normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
			return true;
		case 0x0c: // b1 b2
			nodes[2] = b1;
			nodes[3] = b2;
			result->addElement( kmb::QUAD, nodes );
			// ループの時のために終了点を覚えておく
			if( bend == kmb::nullNodeId ){
				bend = b0;
			}
			b0 = b2;
			normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
			return true;
		case 0x07: // f1 f2 b1
			{
				// f1 b1 と f1 f2 の比較
				double f1b1 = points->distanceSq(f1,b1) + points->distanceSq(b1,b0);
				double f1f2 = points->distanceSq(f1,f2) + points->distanceSq(f2,b0);
				if( fabs(f1b1) <= fabs(f1f2) ){
					// f1 b1
					nodes[2] = b1;
					nodes[3] = f1;
					result->addElement( kmb::QUAD, nodes );
					// ループの時のために終了点を覚えておく
					if( fend == kmb::nullNodeId ){
						fend = f0;
					}
					if( bend == kmb::nullNodeId ){
						bend = b0;
					}
					f0 = f1;
					b0 = b1;
					normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
					return true;
				}else{
					// f1 f2
					nodes[2] = f2;
					nodes[3] = f1;
					result->addElement( kmb::QUAD, nodes );
					// ループの時のために終了点を覚えておく
					if( fend == kmb::nullNodeId ){
						fend = f0;
					}
					f0 = f2;
					normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
					return true;
				}
			}
		case 0x0d: // f1 b1 b2
			{
				// f1 b1 と b1 b2 の比較
				double f1b1 = points->distanceSq(f0,f1) + points->distanceSq(f1,b1);
				double b2b1 = points->distanceSq(f0,b2) + points->distanceSq(b2,b1);
				if( fabs(f1b1) <= fabs(b2b1) ){
					// f1 b1
					nodes[2] = b1;
					nodes[3] = f1;
					result->addElement( kmb::QUAD, nodes );
					// ループの時のために終了点を覚えておく
					if( fend == kmb::nullNodeId ){
						fend = f0;
					}
					if( bend == kmb::nullNodeId ){
						bend = b0;
					}
					f0 = f1;
					b0 = b1;
					normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
					return true;
				}else{
					// b1 b2
					nodes[2] = b1;
					nodes[3] = b2;
					result->addElement( kmb::QUAD, nodes );
					// ループの時のために終了点を覚えておく
					if( bend == kmb::nullNodeId ){
						bend = b0;
					}
					b0 = b2;
					normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
					return true;
				}
			}
		case 0x0f: // f1 f2 b1 b2
			{
				// f1 b1 と f1 f2 と b1 b2 の比較
				double f1b1 = points->distanceSq(f0,f1) + points->distanceSq(f1,b1) + points->distanceSq(b1,b0);
				double f1f2 = points->distanceSq(f0,f1) + points->distanceSq(f1,f2) + points->distanceSq(f2,b0);
				double b2b1 = points->distanceSq(f0,b2) + points->distanceSq(b2,b1) + points->distanceSq(b1,b0);
				if( fabs(f1b1) <= fabs(f1f2) ){
					if( fabs(f1b1) <= fabs(b2b1) ){
						// f1 b1
						nodes[2] = b1;
						nodes[3] = f1;
						result->addElement( kmb::QUAD, nodes );
						// ループの時のために終了点を覚えておく
						if( fend == kmb::nullNodeId ){
							fend = f0;
						}
						// ループの時のために終了点を覚えておく
						if( bend == kmb::nullNodeId ){
							bend = b0;
						}
						f0 = f1;
						b0 = b1;
					}else{
						// b1 b2
						nodes[2] = b1;
						nodes[3] = b2;
						result->addElement( kmb::QUAD, nodes );
						// ループの時のために終了点を覚えておく
						if( bend == kmb::nullNodeId ){
							bend = b0;
						}
						b0 = b2;
					}
				}else{
					if( fabs(f1f2) <= fabs(b2b1) ){
						// f1 f2
						nodes[2] = f2;
						nodes[3] = f1;
						result->addElement( kmb::QUAD, nodes );
						// ループの時のために終了点を覚えておく
						if( fend == kmb::nullNodeId ){
							fend = f0;
						}
						f0 = f2;
					}else{
						// b1 b2
						nodes[2] = b1;
						nodes[3] = b2;
						result->addElement( kmb::QUAD, nodes );
						// ループの時のために終了点を覚えておく
						if( bend == kmb::nullNodeId ){
							bend = b0;
						}
						b0 = b2;
					}
				}
				normal = points->calcNormalVector( nodes[0], nodes[1], nodes[2], nodes[3] );
				return true;
			}
		default:
			break;
	}
	return false;
}

kmb::bodyIdType
kmb::EdgeStitcher::stitchByQuadsWithNodeCount(int fCount,int bCount)
{
	if( mesh && points && forward.getSize() > 0 && backward.getSize() > 0 ){
		setInitialNode();
		if( normalCheck ){
			setInitNormal();
		}else{
			normal.zero();
		}
		std::vector<kmb::nodeIdType> appendNodes;
		kmb::ElementContainerMap* result = NULL;
		result = new kmb::ElementContainerMap();
		// 節点の個数に応じてつなぎ方を変える
		// 4点以上節点を追加する場合は、もうちょっと工夫しないと、
		// 同じところにいくつも点を追加してしまう。
		if( forward.getSize() == 4 && backward.getSize() == 6 ){
			// forward の方が少ない場合は、適当に点を追加する
			kmb::Point3D backCenter;
			kmb::Point3D forwardCenter;
			if( backward.getCenter( points, backCenter ) && forward.getCenter( points, forwardCenter ) ){
				// 重心に一番近いところ
				kmb::nodeIdType a0 = forward.getNearestNode( points, backCenter );
				kmb::nodeIdType nodes[4] = {kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId,kmb::nullNodeId};
				if( a0 != kmb::nullNodeId ){
					kmb::nodeIdType a_next = forward.getNodeId( a0, 1 );
					kmb::nodeIdType a_prev = forward.getNodeId( a0, -1 );
					kmb::Point3D p_next,p_prev,p0;
					if( points->getPoint(a0,p0) && points->getPoint(a_next,p_next) && points->getPoint(a_prev,p_prev) ){
						kmb::Vector3D diffCenter( backCenter, forwardCenter );
						kmb::Vector3D v_next( p_next, p0 );
						kmb::Vector3D v_prev( p_prev, p0 );
						// 重心のずれとの方向を調べる
						if( v_next * diffCenter < v_prev * diffCenter ){
							nodes[0] = a0;
							nodes[1] = points->addPoint(p0);
							nodes[2] = points->addPoint(p_next);
							nodes[3] = a_next;
							// forward で [f0,f0,f1,f1] を追加
							// [f0,f1,f2,f3] とみなす
							// nodes[1] と nodes[2] は２重点
							forward.deleteSegment( nodes[0], nodes[3] );
							forward.addSegment( nodes[0], nodes[1] );
							forward.addSegment( nodes[1], nodes[2] );
							forward.addSegment( nodes[2], nodes[3] );
						}else{
							nodes[0] = a_prev;
							nodes[1] = points->addPoint(p_prev);
							nodes[2] = points->addPoint(p0);
							nodes[3] = a0;
							// forward で [f-1,f-1,f0,f0] を追加
							// [f-3,f-2,f-1,f0] とみなす
							// nodes[1] と nodes[2] は２重点
							forward.deleteSegment( nodes[0], nodes[3] );
							forward.addSegment( nodes[0], nodes[1] );
							forward.addSegment( nodes[1], nodes[2] );
							forward.addSegment( nodes[2], nodes[3] );
						}
						// nodes[1] と nodes[2] はあとで smoothing する
						appendNodes.push_back( nodes[1] );
						appendNodes.push_back( nodes[2] );
						result->addElement( kmb::QUAD, nodes );
						++fCount;
					}
				}
			}
		}
		if( forward.getSize() == backward.getSize() && fCount == bCount ){
			fend = f0;
			bend = b0;
			kmb::nodeIdType nodes[4] = {f0,b0,kmb::nullNodeId,kmb::nullNodeId};
			while( f0 != kmb::nullNodeId && b0 != kmb::nullNodeId ){
				kmb::nodeIdType f1 = forward.getNodeId(f0,1);
				kmb::nodeIdType b1 = backward.getNodeId(b0,-1);
				nodes[0] = f0;
				nodes[1] = b0;
				nodes[2] = b1;
				nodes[3] = f1;
				result->addElement( kmb::QUAD, nodes );
				f0 = ( f1 == fend ) ? kmb::nullNodeId : f1;
				b0 = ( b1 == bend ) ? kmb::nullNodeId : b1;
			}
		}
		if( result->getCount() > 0 ){
			kmb::bodyIdType bodyId = mesh->appendBody( result );
			kmb::MeshSmoother smoother(mesh);
			// ２回 smoothing をかける
			for(int i=0;i<2;++i){
				smoother.init();
				smoother.appendBody(bodyId);
				std::vector< kmb::nodeIdType >::iterator nIter = appendNodes.begin();
				while( nIter != appendNodes.end() ){
					smoother.smoothingNode(*nIter);
					++nIter;
				}
				smoother.commit();
			}
			return bodyId;
		}else{
			delete result;
		}
	}
	return kmb::Body::nullBodyId;
}
