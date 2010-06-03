/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ShapeData                                               #
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

#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh.hxx>
#include <BRepAlgo.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Surface.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <ShapeUpgrade_ShapeConvertToBezier.hxx>
#include <ShapeFix_Shape.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

#include <iostream>
#include <map>

#include "Shape/kmbShapeData.h"
#include "Shape/kmbBezierSurface3D.h"
#include "Shape/kmbNurbsSurface3D.h"
#include "MeshDB/kmbMeshData.h"

kmb::ShapeData::ShapeData(void)
: topo_shape(NULL)
{
	topo_shape = new TopoDS_Shape;
}

kmb::ShapeData::~ShapeData(void)
{
	if( topo_shape ){
		delete topo_shape;
	}
}

TopoDS_Shape&
kmb::ShapeData::getShape(void) const
{
	return *topo_shape;
}

bool
kmb::ShapeData::isValid(void) const
{
	if( topo_shape && !topo_shape->IsNull() ){
		if(BRepAlgo::IsValid(*topo_shape)){
			return true;
		}
	}
	return false;
}

void
kmb::ShapeData::test(void) const
{
	std::cout << "RevocapMesh::Shape test" << std::endl;

	TopExp_Explorer exCompound;
	for( exCompound.Init(*topo_shape,TopAbs_COMPOUND); exCompound.More(); exCompound.Next() ){
		TopoDS_Compound compound = TopoDS::Compound(exCompound.Current());
		std::cout << "COMPOUND " << compound.Closed() << std::endl;
	}

	TopExp_Explorer exCompSolid;
	for( exCompSolid.Init(*topo_shape,TopAbs_COMPSOLID); exCompSolid.More(); exCompSolid.Next() ){
		TopoDS_CompSolid compSolid = TopoDS::CompSolid(exCompSolid.Current());
		std::cout << "COMPSOLID " << compSolid.Closed() << std::endl;
	}

	TopExp_Explorer exSolid;
	for( exSolid.Init(*topo_shape,TopAbs_SOLID); exSolid.More(); exSolid.Next() ){
		TopoDS_Solid solid = TopoDS::Solid(exSolid.Current());
		std::cout << "SOLID " << solid.Closed() << std::endl;
		TopExp_Explorer exShell;
		for( exShell.Init(solid,TopAbs_SHELL); exShell.More(); exShell.Next() ){
			TopoDS_Shell shell = TopoDS::Shell(exShell.Current());
			std::cout << " SHELL " << shell.Closed() << std::endl;
			TopExp_Explorer exFace;
			for( exFace.Init(shell,TopAbs_FACE); exFace.More(); exFace.Next() ){
				TopoDS_Face face = TopoDS::Face(exFace.Current());
				std::cout << "  FACE " << face.Closed() << std::endl;















				Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
				ShapeAnalysis_Surface saSurf(surf);
				std::cout << "v-period " << surf->VPeriod() << std::endl;
				std::cout << "u-period " << surf->UPeriod() << std::endl;
				Standard_Real u1, u2, v1, v2;
				surf->Bounds( u1, u2, v1, v2 );
				std::cout << "Bounds " << u1 << " " << u2 << " " << v1 << " " << v2 << std::endl;
				std::cout << "Type " << surf->DynamicType()->Name() << std::endl;
				std::cout << "UClosed " << surf->IsUClosed() << std::endl;
				std::cout << "VClosed " << surf->IsVClosed() << std::endl;
				std::cout << "UPeriodic " << surf->IsUPeriodic() << std::endl;
				std::cout << "VPeriodic " << surf->IsVPeriodic() << std::endl;
				ShapeAnalysis_Surface anaSurf(surf);
				std::cout << "Singularities " << anaSurf.HasSingularities(1.0e-6) << " " <<
					anaSurf.NbSingularities(1.0e-6) << std::endl;
				std::cout << "Gap " << anaSurf.Gap() << std::endl;
			}
		}
	}

}

void
kmb::ShapeData::convertToBezier(void)
{
	ShapeUpgrade_ShapeConvertToBezier shapeBezier( *topo_shape );

	shapeBezier.Set3dConversion(Standard_True);
	shapeBezier.Set3dCircleConversion(Standard_True);
	shapeBezier.Set3dConicConversion(Standard_True);
	shapeBezier.Set3dLineConversion(Standard_True);
	shapeBezier.SetRevolutionMode(Standard_True);
	shapeBezier.SetExtrusionMode(Standard_True);

	shapeBezier.SetSurfaceConversion(Standard_True);
	shapeBezier.SetPlaneMode(Standard_True);
	shapeBezier.Perform();

	if(shapeBezier.Status(ShapeExtend_DONE)){
		TopoDS_Shape* tmp_shape = topo_shape;
		topo_shape = new TopoDS_Shape;
		*topo_shape = shapeBezier.Result();
		delete tmp_shape;
	}
}

void
kmb::ShapeData::fixShape(double precision,double tolerance)
{
	ShapeFix_Shape sfShape;
	sfShape.Init(*topo_shape);
	sfShape.SetPrecision( static_cast<Standard_Real>(precision) );
	sfShape.SetMaxTolerance(  static_cast<Standard_Real>(tolerance) );
	sfShape.Perform();

	TopoDS_Shape* tmp_shape = topo_shape;
	topo_shape = new TopoDS_Shape;
	*topo_shape = sfShape.Shape();
	delete tmp_shape;
}




void
kmb::ShapeData::dropSmallEdge(double precision,double tolerance)
{
}

bool
kmb::ShapeData::isClosed(void) const
{
	if( topo_shape && !topo_shape->IsNull() ){
		if(BRep_Tool::IsClosed(*topo_shape)){
			return true;
		}
	}
	return false;
}

int
kmb::ShapeData::getSurfaces( std::vector<kmb::Surface3D*> &surfaces) const
{
	int count = 0;
	TopExp_Explorer exFace;
	for( exFace.Init(*topo_shape,TopAbs_FACE); exFace.More(); exFace.Next() ){
		TopoDS_Face face = TopoDS::Face(exFace.Current());
		if( !face.IsNull() ){
			Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
			if( surf->IsKind( STANDARD_TYPE(Geom_BezierSurface) ) ){
				count += 1;
				Handle(Geom_BezierSurface) bsf =
					Handle(Geom_BezierSurface)::DownCast(surf);
				kmb::BezierSurface3D* bezier = new kmb::BezierSurface3D();
				int nu = bsf->NbUPoles();
				int nv = bsf->NbVPoles();
				bezier->setOrder( nu, nv );
				for(int i=1;i<=nu;++i){
					for(int j=1;j<=nv;++j){
						gp_Pnt pt = bsf->Pole(i,j);
						bezier->appendCtrlPt( pt.X(), pt.Y(), pt.Z() );
					}
				}
				bezier->setSurfaceId( face.HashCode( 0x80000000 ) );
				surfaces.push_back( bezier );
			}else if( surf->IsKind( STANDARD_TYPE(Geom_BSplineSurface) ) ){
				count += 1;
				Handle(Geom_BSplineSurface) bsp =
					Handle(Geom_BSplineSurface)::DownCast(surf);
				kmb::NurbsSurface3D* nurbs = new kmb::NurbsSurface3D();
				nurbs->setOrder( bsp->UDegree()+1, bsp->VDegree()+1 );
				int ku = bsp->NbUKnots();
				for(int i=1;i<=ku;++i){
					int multi = bsp->UMultiplicity(i);
					for(int j=0;j<multi;++j){
						nurbs->appendUKnot( bsp->UKnot(i) );
					}
				}
				int kv = bsp->NbVKnots();
				for(int i=1;i<=kv;++i){
					int multi = bsp->VMultiplicity(i);
					for(int j=0;j<multi;++j){
						nurbs->appendVKnot( bsp->VKnot(i) );
					}
				}
				int nu = bsp->NbUPoles();
				int nv = bsp->NbVPoles();
				for(int i=1;i<=nu;++i){
					for(int j=1;j<=nv;++j){
						gp_Pnt pt = bsp->Pole(i,j);
						nurbs->appendCtrlPt( pt.X(), pt.Y(), pt.Z(), bsp->Weight(i,j) );
					}
				}
				nurbs->setSurfaceId( face.HashCode( 0x80000000 ) );
				surfaces.push_back( nurbs );
			}else{
				BRep_Tool::Surface(face)->DynamicType().Dump( std::cout );
				std::cout << std::endl;
			}
		}
	}
	return count;
}

int
kmb::ShapeData::saveToRNF(const char* filename) const
{
	std::ofstream output( filename, std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	std::vector< kmb::Surface3D* > surfaces;
	int surfaceId = 0;
	getSurfaces( surfaces );
	output << "# REVOCAP Neutral Yaml Format ver 0.1.1" << std::endl;
	output << "---" << std::endl;
	if( surfaces.size() > 0 ){
		output << "surface:" << std::endl;
		for( std::vector< kmb::Surface3D* >::iterator sIter = surfaces.begin();
			sIter != surfaces.end(); ++sIter)
		{
			kmb::Surface3D* surface = *sIter;
			if( surface ){
				surfaceId = surface->getSurfaceId();
				switch( surface->getSurfaceType() )
				{
				case kmb::Surface3D::BEZIER:
					output << "  - bezier:" << std::endl;
					output << "      id: " << surfaceId << std::endl;
					surface->writeRNF( output, "      " );
					break;
				case kmb::Surface3D::BSPLINE:
					output << "  - bspline:" << std::endl;
					output << "      id: " << surfaceId << std::endl;
					surface->writeRNF( output, "      " );
					break;
				case kmb::Surface3D::NURBS:
					output << "  - nurbs:" << std::endl;
					output << "      id: " << surfaceId << std::endl;
					surface->writeRNF( output, "      " );
					break;
				default:
					break;
				}
			}
			delete surface;
		}
	}
	surfaces.clear();
	output.close();
	return 0;
}

#endif
