/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : PatchGenerator                                          #
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
#ifdef OPENCASCADE

#include "Shape/kmbPatchGenerator.h"
#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbTriangle.h"
#include "Shape/kmbShapeData.h"
#include "Geometry/kmb_Point3DOctree.h"

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepMesh.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <Poly_Triangulation.hxx>
#include <Geom_Surface.hxx>

#include <vector>

kmb::PatchGenerator::PatchGenerator(void)
: deflection(15.0)
, incremental(1.0)
, tolerance(1.0e-5)
{
}

kmb::PatchGenerator::~PatchGenerator(void)
{
}

void
kmb::PatchGenerator::setDeflection(double d)
{
	deflection = d;
}

double
kmb::PatchGenerator::getDeflection(void)
{
	return deflection;
}

void
kmb::PatchGenerator::setIncremental(double d)
{
	incremental = d;
}

double
kmb::PatchGenerator::getIncremental(void)
{
	return incremental;
}

void
kmb::PatchGenerator::setTolerance(double d)
{
	tolerance = d;
}

double
kmb::PatchGenerator::getTolerance(void)
{
	return tolerance;
}

bool kmb::PatchGenerator::execute(kmb::ShapeData& shape,kmb::MeshData& mesh)
{
	if( shape.getShape().IsNull() ){
		return false;
	}

	if( mesh.getNodes() == NULL ){
		mesh.beginNode();
		mesh.endNode();
	}

	kmb::DataBindings* data = mesh.getDataBindingsPtr( "FaceHashCode" );
	if( data == NULL ){
		data = mesh.createDataBindings( "FaceHashCode", kmb::DataBindings::BODYVARIABLE, kmb::PhysicalValue::INTEGER );
	}

	kmb::Point3DOctree octree;

	BRepMesh::Mesh( shape.getShape(), static_cast<Standard_Real>(incremental) );


	TopExp_Explorer exFace;
	for( exFace.Init(shape.getShape(),TopAbs_FACE); exFace.More(); exFace.Next() ){
		TopoDS_Face face = TopoDS::Face(exFace.Current());
		if( !face.IsNull() ){

			ShapeAnalysis_Shell anaShell;
			anaShell.LoadShells(face);
			anaShell.CheckOrientedShells(face);

			Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
			ShapeAnalysis_Surface anaSurf(surf);
			if( anaSurf.HasSingularities(1.0e-6) ){
				std::cout << "Face Surf has Singularities " << std::endl;
			}

			ShapeAnalysis_ShapeContents anaContents;
			anaContents.Perform(face);
			if(anaContents.ModifyTrimmed3dMode()){
				std::cout << " modify trimmed mode " << std::endl;
			}

			if( !BRepTools::Triangulation(face, static_cast<Standard_Real>(deflection)) ){
				std::cout << "BRepTools false!" << endl;
				continue;
			}

			std::vector< kmb::nodeIdType > nodeLabels;
			TopLoc_Location location;
			Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
			for(Standard_Integer i = 0; i < triangulation->NbNodes(); ++i ){
				gp_Pnt point = triangulation->Nodes().Value(i+1);
				kmb::nodeIdType nearestId = kmb::nullNodeId;
				double dist = octree.getNearestPoint( point.Coord(1), point.Coord(2), point.Coord(3), mesh.getNodes(), nearestId );
				if( dist < tolerance ){
					nodeLabels.push_back( nearestId );
				}else{
					kmb:: nodeIdType nodeId = mesh.insertNode( point.Coord(1), point.Coord(2), point.Coord(3) );
					nodeLabels.push_back( nodeId );
					octree.appendSearchCache( mesh.getNodes(), nodeId );
				}
			}
			kmb::bodyIdType bodyId = mesh.beginElement( triangulation->NbTriangles() );
			kmb::nodeIdType nodeTable[3];
			for(Standard_Integer i = 0; i < triangulation->NbTriangles(); ++i ){
				Standard_Integer index0, index1, index2;
				triangulation->Triangles().Value(i+1).Get(index0,index1,index2);
				nodeTable[0] = nodeLabels[index0-1];
				nodeTable[1] = nodeLabels[index1-1];
				nodeTable[2] = nodeLabels[index2-1];
				kmb::Point3D n0,n1,n2;
				if( !kmb::Triangle::isCoincidentNodes(nodeTable[0],nodeTable[1],nodeTable[2]) ){
					mesh.addElement( kmb::TRIANGLE, nodeTable );
				}
			}
			mesh.endElement();
			long hashCode = static_cast<long>( face.HashCode( 0x80000000 ) );
			data->setPhysicalValue( bodyId, &hashCode );
		}
	}
	return true;
}

#endif
