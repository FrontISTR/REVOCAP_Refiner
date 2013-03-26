/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Sample Program PreFitting                                            #
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
/*
 * Fitting の前処理を行う
 *
 * 通常のメッシュの場合、表面上の節点に対して、NURBS 曲面のパラメータを逆算する
 *
 * Debug 用 => REVOCAP YAML メッシュ NodeGroup として "boundary" という名前で作っておけばよい
 */

#ifdef _CONSOLE

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbBoundaryExtractor.h"
#include "MeshDB/kmbVector2WithIntBindings.h"
#include "Shape/kmbSurface3D.h"
#include "Shape/kmbRnfShapeIO.h"
#include "Shape/kmbNurbsSurface3D.h"
#include "RevocapIO/kmbTetMeshMIO.h"
#include "RevocapIO/kmbHecmwIO.h"
#include "RevocapIO/kmbFFbIO.h"
#include "RevocapIO/kmbRevocapNeutralIO.h"
#include "Refiner/rcapRefinerMacros.h"

enum meshformat{
	ADVENTURE_TETMESH,
	HECMW,
	FFBGF,
	REVOCAP_YAML,
};

int main(int argc, char* argv[])
{
	printf( "REVOCAP Refiner Pre Fitting Data Generator\n" );
	if( argc < 4 ){
		printf( "USAGE :\n");
		printf( " rcapPreRefiner [-ahb] originalmesh shapedatafile outputfile\n" );
		printf( " -a : for ADVENTURE_TetMeshM format (default)\n" );
		printf( " -h : for HECMW_Mesh format\n" );
		printf( " -b : for FrontFlow/blue GF1 format\n" );
		return -1;
	}

	meshformat mtype = ADVENTURE_TETMESH;
	int i=1;
	while( argv[i][0] == '-' ){
		if( strlen(argv[i]) == 2 ){
			switch( argv[i][1] ){
			case 'a':
				printf( "ADVENTURE_TetMesh format is selected.\n");
				mtype = ADVENTURE_TETMESH;
				break;
			case 'h':
				printf( "HECMW_Mesh format is selected.\n");
				mtype = HECMW;
				break;
			case 'b':
				printf( "FrontFlow/blue GF format is selected.\n");
				mtype = FFBGF;
				break;
			case 'y':
				printf( "Revocap YAML Mesh format is selected.\n");
				mtype = REVOCAP_YAML;
				break;
			default:
				break;
			}
		}
		++i;
	}

	const char* meshfile = NULL;
	const char* shapefile = NULL;
	const char* outputfile = NULL;
	kmb::MeshDB mesh;
	std::vector< kmb::Surface3D* > surfaces;
	kmb::DataBindings* nodeGroup = NULL;
	kmb::RevocapNeutralIO rnfIO;

	if( argc > i ){
		meshfile = argv[i];
		printf("mesh file : %s\n", meshfile);
	}else{
		printf("error : no mesh file.\n");
		return -1;
	}
	if( argc > i+1 ){
		shapefile = argv[i+1];
		printf("shape file : %s\n", shapefile);
	}else{
		printf("error : no shape file.\n");
		return -1;
	}
	if( argc > i+2 ){
		outputfile = argv[i+2];
		printf("outputfile file : %s\n", outputfile);
	}else{
		printf("error : no output file.\n");
		return -1;
	}

	switch(mtype){
	case ADVENTURE_TETMESH:
	{
		printf( "ADVENTURE_TetMesh file is loading...\n");
		kmb::TetMeshMIO tetmesh;
		mesh.beginModel();
		tetmesh.loadFromFile( meshfile, &mesh );
		mesh.endModel();
		std::cout << "node count = " << mesh.getNodeCount() << std::endl;

		kmb::bodyIdType bCount = mesh.getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId < bCount;++bodyId){
			size_t eCount = mesh.getElementCount(bodyId);
			std::cout << "element count (" << bodyId << ") = " << eCount << std::endl;

		}
		break;
	}
	case HECMW:
	{
		printf( "HECMW_Mesh file is loading...\n");
		kmb::HecmwIO hec;
		mesh.beginModel();
		hec.loadFromFile( meshfile, &mesh );
		mesh.endModel();
		printf("node count = %zu\n", mesh.getNodeCount());
		kmb::bodyIdType bCount = mesh.getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId < bCount;++bodyId){
			size_t eCount = mesh.getElementCount(bodyId);
			printf("element count = %zu\n", eCount);
		}
		break;
	}
	case FFBGF:
	{
		printf( "FrontFlow/blue GF2 file is loading...\n");
		kmb::FFbIO gf;
		mesh.beginModel();
		gf.loadFromMeshFile( meshfile, &mesh );
		mesh.endModel();
		printf("node count = %zu\n", mesh.getNodeCount());
		kmb::bodyIdType bCount = mesh.getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId < bCount;++bodyId){
			size_t eCount = mesh.getElementCount(bodyId);
			printf("element count = %zu\n", eCount);
		}
		break;
	}
	case REVOCAP_YAML:
	{
		printf( "REVOCAP YAML file is loading...\n");
		mesh.beginModel();
		rnfIO.loadFromRNFFile( meshfile, &mesh );
		mesh.endModel();
		printf("node count = %zu\n", mesh.getNodeCount());
		kmb::bodyIdType bCount = mesh.getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId < bCount;++bodyId){
			size_t eCount = mesh.getElementCount(bodyId);
			printf("element count = %zu\n", eCount);
		}
		nodeGroup = mesh.getDataBindingsPtr("boundary");
		break;
	}
	default:
		break;
	}

	if( nodeGroup == NULL ){
		nodeGroup = mesh.createDataBindings("boundary",kmb::DataBindings::NodeGroup,kmb::PhysicalValue::None);
		kmb::bodyIdType bodyCount = mesh.getBodyCount();
		for(kmb::bodyIdType bodyId = 0;bodyId < bodyCount;++bodyId)
		{
			if( mesh.getDimension(bodyId) == 3 ){
				kmb::BoundaryExtractor bndext;
				bndext.setMesh( &mesh );
				kmb::ElementContainer* body = mesh.getBodyPtr(bodyId);
				bndext.appendBody(bodyId);
				bndext.getBoundaryNodeGroup( body, nodeGroup );
				bndext.clear();
			}
		}
	}

	kmb::Vector2WithIntBindings<kmb::nodeIdType>* fittingData =
		reinterpret_cast< kmb::Vector2WithIntBindings<kmb::nodeIdType>* >( mesh.createDataBindings("fitting",kmb::DataBindings::NodeVariable,kmb::PhysicalValue::Vector2withInt) );


	rnfIO.saveHeader( outputfile );

	{

		double epsilon = 1.0e-8;
		int iterMax = 1000;
		const char* tags[1];
		tags[0] = "epsilon";
		rnfIO.loadParameterFromRNFFile<double>( shapefile, 1, tags, epsilon );
		tags[0] = "itermax";
		rnfIO.loadParameterFromRNFFile<int>( shapefile, 1, tags, iterMax );
		double distthresh = 0.1;

		kmb::RnfShapeIO rnfshape;
		rnfshape.appendSurfaceHeaderToFile( outputfile );
		rnfshape.loadFromFile( shapefile, surfaces );

		long surfaceId = 0;
		std::cout << "surface count = " << surfaces.size() << std::endl;

		std::vector< kmb::Surface3D* >::iterator sIter = surfaces.begin();
		while( sIter != surfaces.end () ){
			kmb::Surface3D* surf = *sIter;
			if( surf ){
				surf->setSurfaceId( surfaceId );
				printf("Surface Id = %ld\n", surf->getSurfaceId() );
				surf->setEpsilon( epsilon );
				surf->setIterMax( iterMax );
				printf("Surface Newton-Raphson threshold = %f\n", epsilon );
				switch( surf->getSurfaceType() )
				{
				case kmb::Surface3D::BEZIER:
					printf("Bezier Surface\n");
					break;
				case kmb::Surface3D::BSPLINE:
					printf("B-Spline Surface\n");
					break;
				case kmb::Surface3D::NURBS:
					{
						printf("NURBS Surface\n");
						kmb::NurbsSurface3D* nurbs = reinterpret_cast<kmb::NurbsSurface3D*>(surf);
						unsigned int uKnot(0), vKnot(0);
						unsigned int uDegree(0), vDegree(0);
						nurbs->getKnotCount(uKnot,vKnot);
						nurbs->getDegree(uDegree,vDegree);
						printf("uKnot = %u, vKnot = %u\n", uKnot, vKnot);
						printf("uDegree = %u, vDegree = %u\n", uDegree, vDegree);
						printf("number of ctrl points : %d\n", nurbs->getCtrlPtCount());
						kmb::BoundingBox bbox;
						nurbs->getBoundingBox(bbox);
						bbox.expandDiameter(1.01);
						printf("Bounding Box (%f %f %f) - (%f %f %f)\n",
							bbox.minX(),bbox.minY(),bbox.minZ(),bbox.maxX(),bbox.maxY(),bbox.maxZ());

						kmb::DataBindings::iterator dIter = nodeGroup->begin();
						while( !dIter.isFinished() ){
							kmb::nodeIdType nodeId = dIter.getId();
							kmb::Point3D pt,pt0;
							double u,v;
							mesh.getNode(nodeId,pt);
							printf("fitting %d => ", nodeId);
							if( bbox.intersect(pt) != kmb::Region::OUTSIDE ){

								if( nurbs->getNearest(pt,u,v) ){

									nurbs->getPoint(u,v,pt0);
									if( pt.distance(pt0) < distthresh ){
										printf("%f %f\n", u, v);
										fittingData->setValue(nodeId,u,v,static_cast<long>(surfaceId));
									}else{
										printf("distance is so long\n");
									}
								}else{
									printf("can't find\n");
								}
							}else{
								printf("outside bounding box\n");
							}
							++dIter;
						}
						break;
					}
				default:
					break;
				}

				rnfshape.appendSurfaceToFile( outputfile, surf );
				++surfaceId;
			}
			++sIter;
		}
	}


	{
		kmb::DataBindings::iterator dIter = fittingData->begin();
		while( !dIter.isFinished() ){
			kmb::nodeIdType nodeId = dIter.getId();
			kmb::Point3D pt;
			mesh.getNode(nodeId,pt);
			long surfaceId;
			double uv[2];
			dIter.getValue(&surfaceId);
			dIter.getValue(uv);
			kmb::Surface3D* surf = surfaces[surfaceId];
			if( surf ){
				kmb::Point3D pt2;
				surf->getPoint(uv[0],uv[1],pt2);
				printf("%d %d %f\n", nodeId, surfaceId, pt.distance(pt2));
			}
			++dIter;
		}
	}


	rnfIO.appendDataToRNFFile( outputfile, &mesh, "fitting" );


	std::vector< kmb::Surface3D* >::iterator sIter = surfaces.begin();
	while( sIter != surfaces.end () ){
		kmb::Surface3D* surf = *sIter;
		if( surf ){
			delete surf;
		}
		++sIter;
	}
	surfaces.clear();

	kmb::DataBindings::iterator dIter = nodeGroup->begin();
	while( !dIter.isFinished() ){
		kmb::nodeIdType nodeId = dIter.getId();
		size_t s = fittingData->countId(nodeId);
		if( fittingData->countId(nodeId) != 1 ){
			printf("nodeId %d => number of fitting surface = %u\n", nodeId, s);
		}
		++dIter;
	}

	printf("pre-fitting status report\n");
	printf("boundary node count = %d\n", nodeGroup->getIdCount());
	printf("fitting node count = %d\n", fittingData->getIdCount());

	return 0;
}

#endif
