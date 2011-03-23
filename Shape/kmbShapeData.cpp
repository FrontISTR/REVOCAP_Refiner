/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : ShapeData                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
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
#include <ShapeCustom_ConvertToBSpline.hxx>
#include <ShapeFix_Shape.hxx>

#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <BRepClass3d_SolidExplorer.hxx>

#include <iostream>
#include <map>

#include "Shape/kmbShapeData.h"
#include "Shape/kmbBezierSurface3D.h"
#include "Shape/kmbNurbsSurface3D.h"
#include "Shape/kmbPlaneSurface3D.h"
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

kmb::BoundingBox
kmb::ShapeData::getBoundingBox(void) const
{
	kmb::BoundingBox bbox;

	BRepClass3d_SolidExplorer sexp( *topo_shape );
	Standard_Real xmin, xmax, ymin, ymax, zmin, zmax;
	sexp.Box().Get( xmin, ymin, zmin, xmax, ymax, zmax );

	bbox.setMinMax( xmin, ymin, zmin, xmax, ymax, zmax );

	return bbox;
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
	if( topo_shape==NULL ){
		return;
	}
	ShapeUpgrade_ShapeConvertToBezier shapeBezier( *topo_shape );


	shapeBezier.Set3dCircleConversion(Standard_True);

	shapeBezier.Set3dConicConversion(Standard_True);

	shapeBezier.Set3dLineConversion(Standard_True);


	shapeBezier.SetRevolutionMode(Standard_True);

	shapeBezier.SetExtrusionMode(Standard_True);

	shapeBezier.SetSurfaceSegmentMode(Standard_True);

	shapeBezier.SetSurfaceConversion(Standard_True);

	shapeBezier.SetPlaneMode(Standard_True);

	shapeBezier.Set3dConversion(Standard_True);


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

				for(int j=1;j<=nv;++j){
					for(int i=1;i<=nu;++i){
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

				for(int j=1;j<=nv;++j){
					for(int i=1;i<=nu;++i){
						gp_Pnt pt = bsp->Pole(i,j);
						nurbs->appendCtrlPt( pt.X(), pt.Y(), pt.Z(), bsp->Weight(i,j) );
					}
				}
				nurbs->setSurfaceId( face.HashCode( 0x80000000 ) );
				surfaces.push_back( nurbs );
			}else if( surf->IsKind( STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) ){
				std::cout << "not supported Geom_RectangularTrimmedSurface" << std::endl;
				Handle(Geom_RectangularTrimmedSurface) rts =
					Handle(Geom_RectangularTrimmedSurface)::DownCast(surf);
				Standard_Real u1,u2,v1,v2;
				rts->Bounds( u1, u2, v1, v2 );
				std::cout << "bounds " << u1 << " " << u2 << " " << v1 << " " << v2  << std::endl;

				rts->BasisSurface();
			}else if( surf->IsKind( STANDARD_TYPE(Geom_ElementarySurface) ) ){
				if( surf->IsKind( STANDARD_TYPE(Geom_ConicalSurface) ) ){
					std::cout << "not supported Geom_ConicalSurface" << std::endl;
					Handle(Geom_ConicalSurface) cone =
						Handle(Geom_ConicalSurface)::DownCast(surf);
				}else if( surf->IsKind( STANDARD_TYPE(Geom_CylindricalSurface) ) ){
					Handle(Geom_CylindricalSurface) cylinder =
						Handle(Geom_CylindricalSurface)::DownCast(surf);
					std::cout << "axis (" <<
						cylinder->Axis().Location().X() << ", " <<
						cylinder->Axis().Location().Y() << ", " <<
						cylinder->Axis().Location().Z() << ")" << std::endl;
					std::cout << "radius " << cylinder->Radius() << std::endl;
					std::cout << "center (" <<
						cylinder->Position().Location().X() << "," <<
						cylinder->Position().Location().Y() << "," <<
						cylinder->Position().Location().Z() << ")" << std::endl;
					std::cout << "uperiodic " << cylinder->IsUPeriodic() << std::endl;
					std::cout << "vperiodic " << cylinder->IsVPeriodic() << std::endl;
					std::cout << "uclosed " << cylinder->IsUClosed() << std::endl;
					std::cout << "vclosed " << cylinder->IsVClosed() << std::endl;
					Standard_Real u1,v1,u2,v2;
					cylinder->Bounds(u1,u2,v1,v2);
					std::cout << u1 << " " << u2 << " " << v1 << " " << v2 << std::endl;
					v1 = 0.0;
					v2 = 1.0;
					gp_Pnt uv11,uv12,uv21,uv22;
					cylinder->D0(u1,v1,uv11);
					cylinder->D0(u1,v2,uv12);
					cylinder->D0(u2,v1,uv21);
					cylinder->D0(u2,v2,uv22);
					std::cout << "uv11 (" <<
						uv11.X() << "," <<
						uv11.Y() << "," <<
						uv11.Z() << ")" << std::endl;
					std::cout << "uv12 (" <<
						uv12.X() << "," <<
						uv12.Y() << "," <<
						uv12.Z() << ")" << std::endl;
					std::cout << "uv21 (" <<
						uv21.X() << "," <<
						uv21.Y() << "," <<
						uv21.Z() << ")" << std::endl;
					std::cout << "uv22 (" <<
						uv12.X() << "," <<
						uv12.Y() << "," <<
						uv12.Z() << ")" << std::endl;
					ShapeAnalysis_Surface saSurf(surf);
					Standard_Real xMin,yMin,zMin,xMax,yMax,zMax;
					saSurf.GetBoxUF().Get(xMin,yMin,zMin,xMax,yMax,zMax);
					std::cout << "BoxUF " <<
						"(" <<
						xMin << "," <<
						yMin << "," <<
						zMin << ")" <<
						"(" <<
						xMax << "," <<
						yMax << "," <<
						zMax << ")" << std::endl;
					saSurf.GetBoxUL().Get(xMin,yMin,zMin,xMax,yMax,zMax);
					std::cout << "BoxUL " <<
						"(" <<
						xMin << "," <<
						yMin << "," <<
						zMin << ")" <<
						"(" <<
						xMax << "," <<
						yMax << "," <<
						zMax << ")" << std::endl;
					saSurf.GetBoxVF().Get(xMin,yMin,zMin,xMax,yMax,zMax);
					std::cout << "BoxVF " <<
						"(" <<
						xMin << "," <<
						yMin << "," <<
						zMin << ")" <<
						"(" <<
						xMax << "," <<
						yMax << "," <<
						zMax << ")" << std::endl;
					saSurf.GetBoxVL().Get(xMin,yMin,zMin,xMax,yMax,zMax);
					std::cout << "BoxVL " <<
						"(" <<
						xMin << "," <<
						yMin << "," <<
						zMin << ")" <<
						"(" <<
						xMax << "," <<
						yMax << "," <<
						zMax << ")" << std::endl;
					saSurf.Bounds(u1,u2,v1,v2);
					std::cout << "Bounds " <<
						"(" << u1 << "," << u2 << ")" <<
						"(" << v1 << "," << v2 << ")" << std::endl;


					kmb::NurbsSurface3D* col0 = new kmb::NurbsSurface3D();
					kmb::NurbsSurface3D* col1 = new kmb::NurbsSurface3D();
					col0->setOrder( 3, 2 );
					col1->setOrder( 3, 2 );
					col0->appendUKnot(0.0);
					col0->appendUKnot(0.0);
					col0->appendUKnot(0.0);
					col0->appendUKnot(0.5);
					col0->appendUKnot(0.5);
					col0->appendUKnot(1.0);
					col0->appendUKnot(1.0);
					col0->appendUKnot(1.0);
					col0->appendVKnot(0.0);
					col0->appendVKnot(0.0);
					col0->appendVKnot(1.0);
					col0->appendVKnot(1.0);
					col1->appendUKnot(0.0);
					col1->appendUKnot(0.0);
					col1->appendUKnot(0.0);
					col1->appendUKnot(0.5);
					col1->appendUKnot(0.5);
					col1->appendUKnot(1.0);
					col1->appendUKnot(1.0);
					col1->appendUKnot(1.0);
					col1->appendVKnot(0.0);
					col1->appendVKnot(0.0);
					col1->appendVKnot(1.0);
					col1->appendVKnot(1.0);

					gp_Pnt pt0,pt1,pt2,pt3,pt4,pt5,pt6,pt7;
					cylinder->D0(u1                 ,0.0,pt0);
					cylinder->D0((3.0*u1+1.0*u2)/4.0,0.0,pt1);
					cylinder->D0((u1+u2)/2.0        ,0.0,pt2);
					cylinder->D0((1.0*u1+3.0*u2)/4.0,0.0,pt3);
					cylinder->D0(u1                 ,1.0,pt4);
					cylinder->D0((3.0*u1+1.0*u2)/4.0,1.0,pt5);
					cylinder->D0((u1+u2)/2.0        ,1.0,pt6);
					cylinder->D0((1.0*u1+3.0*u2)/4.0,1.0,pt7);
					gp_Pnt c(
						cylinder->Position().Location().X(),
						cylinder->Position().Location().Y(),
						cylinder->Position().Location().Z()
					);
					double sq2 = 0.7071067811865475244008443621;

					col0->appendCtrlPt( pt0.X(), pt0.Y(), pt0.Z() );
					col0->appendCtrlPt(
						pt0.X() + pt1.X() - c.X(),
						pt0.Y() + pt1.Y() - c.Y(),
						pt0.Z() + pt1.Z() - c.Z(),
						sq2);
					col0->appendCtrlPt( pt1.X(), pt1.Y(), pt1.Z() );
					col0->appendCtrlPt(
						pt1.X() + pt2.X() - c.X(),
						pt1.Y() + pt2.Y() - c.Y(),
						pt1.Z() + pt2.Z() - c.Z(),
						sq2);
					col0->appendCtrlPt( pt2.X(), pt2.Y(), pt2.Z() );

					col1->appendCtrlPt( pt2.X(), pt2.Y(), pt2.Z() );
					col1->appendCtrlPt(
						pt2.X() + pt3.X() - c.X(),
						pt2.Y() + pt3.Y() - c.Y(),
						pt2.Z() + pt3.Z() - c.Z(),
						sq2);
					col1->appendCtrlPt( pt3.X(), pt3.Y(), pt3.Z() );
					col1->appendCtrlPt(
						pt3.X() + pt0.X() - c.X(),
						pt3.Y() + pt0.Y() - c.Y(),
						pt3.Z() + pt0.Z() - c.Z(),
						sq2);
					col1->appendCtrlPt( pt0.X(), pt0.Y(), pt0.Z() );

					c.SetCoord(
						c.X() + pt4.X() - pt0.X(),
						c.Y() + pt4.Y() - pt0.Y(),
						c.Z() + pt4.Z() - pt0.Z());

					col0->appendCtrlPt( pt4.X(), pt4.Y(), pt4.Z() );
					col0->appendCtrlPt(
						pt4.X() + pt5.X() - c.X(),
						pt4.Y() + pt5.Y() - c.Y(),
						pt4.Z() + pt5.Z() - c.Z(),
						sq2);
					col0->appendCtrlPt( pt5.X(), pt5.Y(), pt5.Z() );
					col0->appendCtrlPt(
						pt5.X() + pt6.X() - c.X(),
						pt5.Y() + pt6.Y() - c.Y(),
						pt5.Z() + pt6.Z() - c.Z(),
						sq2);
					col0->appendCtrlPt( pt6.X(), pt6.Y(), pt6.Z() );

					col1->appendCtrlPt( pt6.X(), pt6.Y(), pt6.Z() );
					col1->appendCtrlPt(
						pt6.X() + pt7.X() - c.X(),
						pt6.Y() + pt7.Y() - c.Y(),
						pt6.Z() + pt7.Z() - c.Z(),
						sq2);
					col1->appendCtrlPt( pt7.X(), pt7.Y(), pt7.Z() );
					col1->appendCtrlPt(
						pt7.X() + pt4.X() - c.X(),
						pt7.Y() + pt4.Y() - c.Y(),
						pt7.Z() + pt4.Z() - c.Z(),
						sq2);
					col1->appendCtrlPt( pt4.X(), pt4.Y(), pt4.Z() );

					col0->setSurfaceId( face.HashCode( 0x80000000 ) );
					col1->setSurfaceId( face.HashCode( 0x80000000 ) + 0x80000000 );
					surfaces.push_back( col0 );
					surfaces.push_back( col1 );
				}else if( surf->IsKind( STANDARD_TYPE(Geom_Plane) ) ){
					Handle(Geom_Plane) plane =
						Handle(Geom_Plane)::DownCast(surf);
					kmb::PlaneSurface3D* p3d = new kmb::PlaneSurface3D();
					p3d->setBasePoint(
						plane->Position().Location().X(),
						plane->Position().Location().Y(),
						plane->Position().Location().Z()
						);
					gp_Pnt u1,v1;
					plane->D0(1.0,0.0,u1);
					plane->D0(0.0,1.0,v1);
					p3d->setAxisVectors(
						u1.X() - plane->Position().Location().X(),
						u1.Y() - plane->Position().Location().Y(),
						u1.Z() - plane->Position().Location().Z(),
						v1.X() - plane->Position().Location().X(),
						v1.Y() - plane->Position().Location().Y(),
						v1.Z() - plane->Position().Location().Z()
						);
					p3d->setSurfaceId( face.HashCode( 0x80000000 ) );
					surfaces.push_back( p3d );
				}else if( surf->IsKind( STANDARD_TYPE(Geom_SphericalSurface) ) ){
					std::cout << "not supported Geom_SphericalSurface" << std::endl;
					Handle(Geom_SphericalSurface) sphere =
						Handle(Geom_SphericalSurface)::DownCast(surf);
					std::cout << "radius " << sphere->Radius() << std::endl;
					std::cout << "center (" <<
						sphere->Position().Location().X() << "," <<
						sphere->Position().Location().Y() << "," <<
						sphere->Position().Location().Z() << ")" << std::endl;

				}else if( surf->IsKind( STANDARD_TYPE(Geom_ToroidalSurface) ) ){
					std::cout << "not supported Geom_ToroidalSurface" << std::endl;
					Handle(Geom_ToroidalSurface) torus =
						Handle(Geom_ToroidalSurface)::DownCast(surf);
					std::cout << "minor radius " << torus->MinorRadius() << std::endl;
					std::cout << "major radius " << torus->MajorRadius() << std::endl;
					std::cout << "center (" <<
						torus->Position().Location().X() << "," <<
						torus->Position().Location().Y() << "," <<
						torus->Position().Location().Z() << ")" << std::endl;
				}
			}else if( surf->IsKind( STANDARD_TYPE(Geom_OffsetSurface) ) ){
				std::cout << "not supported Geom_OffsetSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_SweptSurface) ) ){
				if( surf->IsKind( STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) ) ){
					std::cout << "not supported Geom_SurfaceOfLinearExtrusion" << std::endl;
				}else if( surf->IsKind( STANDARD_TYPE(Geom_SurfaceOfRevolution) ) ){
					std::cout << "not supported Geom_SurfaceOfRevolution" << std::endl;
				}
			}else{
				std::cout << "not bezier, bspline surface" << std::endl;
				BRep_Tool::Surface(face)->DynamicType().Dump( std::cout );
				std::cout << std::endl;
			}
		}
	}
	return count;
}

int
kmb::ShapeData::saveToRNF(const char* filename,bool append) const
{
	std::ofstream output( filename, (append) ? std::ios_base::app : std::ios_base::out );
	if( output.fail() ){
		return -1;
	}
	std::vector< kmb::Surface3D* > surfaces;
	int surfaceId = 0;
	getSurfaces( surfaces );
	if( !append || (output.tellp() == std::ofstream::pos_type(std::ofstream::beg)) ){
		output << "# REVOCAP Neutral Yaml Format ver 0.1.1" << std::endl;
		output << "---" << std::endl;
	}
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
				case kmb::Surface3D::PLANE:
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
