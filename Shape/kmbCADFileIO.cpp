/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : CADFileIO                                               #
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
#ifdef OPENCASCADE

#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>

#include <StepToTopoDS_Builder.hxx>
#include <Interface_Static.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TDocStd_Document.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>

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
#include <TopoDS_Vertex.hxx>
#include <Standard_PrimitiveTypes.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>

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
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>

#include "Shape/kmbCADFileIO.h"
#include "Shape/kmbShapeData.h"
#include <iostream>

kmb::CADFileIO::CADFileIO(void)
{
}

kmb::CADFileIO::~CADFileIO(void)
{
}

void
kmb::CADFileIO::readIGES(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	IGESCAFControl_Reader igesReader;

	igesReader.PrintCheckLoad( true, IFSelect_GeneralInfo );
	igesReader.PrintCheckTransfer( true, IFSelect_GeneralInfo );
	igesReader.PrintStatsTransfer( 0, 0 );

	IFSelect_ReturnStatus res = igesReader.ReadFile( reinterpret_cast<Standard_CString>(filename) );
	if( res != IFSelect_RetDone ){
		return;
	}

	Handle(TColStd_HSequenceOfTransient) list = igesReader.GiveList();
	std::cout << "entity num : " << list->Length() << std::endl;

	igesReader.TransferList(list);

	shapeData->getShape() = igesReader.OneShape();
}

void
kmb::CADFileIO::readSTEP(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	STEPControl_Reader stepReader;

	stepReader.PrintCheckLoad( true, IFSelect_GeneralInfo );
	stepReader.PrintCheckTransfer( true, IFSelect_GeneralInfo );
	stepReader.PrintStatsTransfer( 0, 0 );

	IFSelect_ReturnStatus res = stepReader.ReadFile( reinterpret_cast<Standard_CString>(filename) );
	if( res != IFSelect_RetDone ){
		return;
	}
	Standard_Integer NbRoots = stepReader.NbRootsForTransfer();
	std::cout << "Number of roots in STEP file: "<< NbRoots << std::endl;
	Standard_Integer NbTrans = stepReader.TransferRoots();
	std::cout << "STEP roots transferred: " << NbTrans << std::endl;
	std::cout << "Number of resulting shapes is: "<< stepReader.NbShapes() << std::endl;
	shapeData->getShape() = stepReader.OneShape();
}

void
kmb::CADFileIO::readSTL(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	StlAPI_Reader stlReader;
	stlReader.Read( shapeData->getShape(), reinterpret_cast<Standard_CString>(filename) );
}

void
kmb::CADFileIO::readOCC(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	BRep_Builder aBuilder;
	BRepTools::Read( shapeData->getShape(), reinterpret_cast<Standard_CString>(filename), aBuilder );
}

void
kmb::CADFileIO::writeIGES(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	IGESControl_Controller::Init();
	IGESControl_Writer igesWriter(Interface_Static::CVal("XSTEP.iges.unit"),Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
	igesWriter.AddShape( shapeData->getShape() );
	igesWriter.ComputeModel();
	igesWriter.Write( reinterpret_cast<Standard_CString>(filename) );
}

void
kmb::CADFileIO::writeSTEP(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	STEPControl_Writer stepWriter;
	STEPControl_StepModelType aValue = STEPControl_ManifoldSolidBrep;
	IFSelect_ReturnStatus status = stepWriter.Transfer( shapeData->getShape(), aValue );
	if ( status != IFSelect_RetDone ){
		return;
	}
	stepWriter.Write( reinterpret_cast<Standard_CString>(filename) );
}

void
kmb::CADFileIO::writeSTL(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	TopoDS_Compound RES;
	BRep_Builder MKCP;
	MKCP.MakeCompound(RES);
	MKCP.Add(RES, shapeData->getShape() );
	StlAPI_Writer myStlWriter;
	myStlWriter.Write(RES, reinterpret_cast<Standard_CString>(filename) );
}

void
kmb::CADFileIO::writeOCC(char* filename,kmb::ShapeData* shapeData)
{
	if( filename==NULL || shapeData==NULL ){
		return;
	}
	BRepTools::Write(shapeData->getShape(),reinterpret_cast<Standard_CString>(filename));
}

int
kmb::CADFileIO::saveToBrepRNF(const char* filename,const kmb::ShapeData* shapeData) const
{
	if( shapeData == NULL || shapeData->getShape().IsNull() ){
		std::cerr << "RevocapShape : shape data is null" << std::endl;
		return -2;
	}
	std::ofstream output( filename );
	if( output.fail() ){
		std::cerr << "RevocapShape : can't open file " << filename << std::endl;
		return -1;
	}
	output << std::scientific;
	writeShapeHeader(output);
	int res = writeShape(output,shapeData->getShape());
	output.close();
	return res;
}

int
kmb::CADFileIO::writeShapeHeader(std::ofstream &output) const
{
	output << "# REVOCAP Brep Yaml Format ver 1.5" << std::endl;
	output << "---" << std::endl;
	return 0;
}

int
kmb::CADFileIO::writeShape(std::ofstream &output,const TopoDS_Shape &shape,int layer) const
{
	std::string margin = "";
	for(int i=0;i<layer;++i){
		margin.append("    ");
	}
	if( layer == 0 ){
		switch( shape.ShapeType() ){
		case TopAbs_COMPOUND:
			output << margin << "compounds:" << std::endl;
			break;
		case TopAbs_COMPSOLID:
			output << margin << "compsolids:" << std::endl;
			break;
		case TopAbs_SOLID:
			output << margin << "solids:" << std::endl;
			break;
		case TopAbs_SHELL:
			output << margin << "shells:" << std::endl;
			break;
		case TopAbs_FACE:
			output << margin << "faces:" << std::endl;
			break;
		case TopAbs_WIRE:
			output << margin << "wires:" << std::endl;
			break;
		case TopAbs_EDGE:
			output << margin << "edges:" << std::endl;
			break;
		case TopAbs_VERTEX:
			output << margin << "vertices:" << std::endl;
			break;
		case TopAbs_SHAPE:
			output << margin << "shapes:" << std::endl;
			break;
		default:
			std::cerr << "RevocapShape : unknown TopoDS shape type" << std::endl;
			break;
		}
	}
	switch( shape.ShapeType() ){
	case TopAbs_COMPOUND:
		output << margin << "  - type: compound" << std::endl;
		break;
	case TopAbs_COMPSOLID:
		output << margin << "  - type: compsolid" << std::endl;
		break;
	case TopAbs_SOLID:
		output << margin << "  - type: solid" << std::endl;
		break;
	case TopAbs_SHELL:
		output << margin << "  - type: shell" << std::endl;
		break;
	case TopAbs_FACE:
		output << margin << "  - type: face" << std::endl;
		break;
	case TopAbs_WIRE:
		output << margin << "  - type: wire" << std::endl;
		break;
	case TopAbs_EDGE:
		output << margin << "  - type: edge" << std::endl;
		break;
	case TopAbs_VERTEX:
		output << margin << "  - type: vertex" << std::endl;
		break;
	case TopAbs_SHAPE:
		output << margin << "  - type: shape" << std::endl;
		break;
	default:
		std::cerr << "RevocapShape : unknown TopoDS shape type" << std::endl;
		break;
	}
	output << margin << "    id: " << shape.HashCode( 0x80000000 ) << std::endl;
	if( shape.Free() ){
		output << margin << "    free: true" << std::endl;
	}
	if( shape.Modified() ){
		output << margin << "    modified: true" << std::endl;
	}
	if( shape.Checked() ){
		output << margin << "    checked: true" << std::endl;
	}
	if( shape.Orientable() ){
		output << margin << "    orientable: true" << std::endl;
	}
	if( shape.Closed() ){
		output << margin << "    closed: true" << std::endl;
	}
	if( shape.Infinite() ){
		output << margin << "    infinite: true" << std::endl;
	}
	if( shape.Convex() ){
		output << margin << "    convex: true" << std::endl;
	}
	switch( shape.ShapeType() ){
	case TopAbs_COMPOUND:
		{
			output << margin << "    compsolids:" << std::endl;
			TopExp_Explorer expCmp;
			for( expCmp.Init(shape,TopAbs_COMPSOLID); expCmp.More(); expCmp.Next() ){
				TopoDS_Shape child = expCmp.Current();
				writeShape(output,child,layer+1);
			}
			output << margin << "    solids:" << std::endl;
			TopExp_Explorer expSolid;
			for( expSolid.Init(shape,TopAbs_SOLID); expSolid.More(); expSolid.Next() ){
				TopoDS_Shape child = expSolid.Current();
				writeShape(output,child,layer+1);
			}
			break;
		}
	case TopAbs_COMPSOLID:
		{
			output << margin << "    solids:" << std::endl;
			TopExp_Explorer exp;
			for( exp.Init(shape,TopAbs_SOLID); exp.More(); exp.Next() ){
				TopoDS_Shape child = exp.Current();
				writeShape(output,child,layer+1);
			}
			break;
		}
	case TopAbs_SOLID:
		{
			output << margin << "    shells:" << std::endl;
			TopExp_Explorer exp;
			for( exp.Init(shape,TopAbs_SHELL); exp.More(); exp.Next() ){
				TopoDS_Shape child = exp.Current();
				writeShape(output,child,layer+1);
			}
			break;
		}
	case TopAbs_SHELL:
		{
			output << margin << "    faces:" << std::endl;
			TopExp_Explorer exp;
			for( exp.Init(shape,TopAbs_FACE); exp.More(); exp.Next() ){
				TopoDS_Shape child = exp.Current();
				writeShape(output,child,layer+1);
			}
			break;
		}
	case TopAbs_FACE:
		{
			TopoDS_Face face = TopoDS::Face(shape);
			Handle(Geom_Surface) surf = BRep_Tool::Surface( face );
			ShapeAnalysis_Surface anaSurf(surf);
			output << std::scientific;
			output << margin << "    singularities: " << anaSurf.NbSingularities(1.0e-6) << std::endl;
			output << margin << "    gap: " << anaSurf.Gap() << std::endl;
			if( surf->IsKind( STANDARD_TYPE(Geom_BezierSurface) ) ){
				output << margin << "    geometry: BezierSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_BSplineSurface) ) ){
				output << margin << "    geometry: BSplineSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) ){
				output << margin << "    geometry: RectangularTrimmedSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_ConicalSurface) ) ){
				output << margin << "    geometry: ConicalSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_CylindricalSurface) ) ){
				output << margin << "    geometry: CylindricalSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_Plane) ) ){
				output << margin << "    geometry: Plane" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_SphericalSurface) ) ){
				output << margin << "    geometry: SphericalSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_ToroidalSurface) ) ){
				output << margin << "    geometry: ToroidalSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_OffsetSurface) ) ){
				output << margin << "    geometry: OffsetSurface" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) ) ){
				output << margin << "    geometry: SurfaceOfLinearExtrusion" << std::endl;
			}else if( surf->IsKind( STANDARD_TYPE(Geom_SurfaceOfRevolution) ) ){
				output << margin << "    geometry: SurfaceOfRevolution" << std::endl;
			}
			output << margin << "    wires:" << std::endl;
			TopExp_Explorer exp;
			for( exp.Init(shape,TopAbs_WIRE); exp.More(); exp.Next() ){
				TopoDS_Shape child = exp.Current();
				writeShape(output,child,layer+1);
			}
			break;
		}
	case TopAbs_WIRE:
		{
			output << margin << "    edges:" << std::endl;
			TopExp_Explorer exp;
			for( exp.Init(shape,TopAbs_EDGE); exp.More(); exp.Next() ){
				TopoDS_Shape child = exp.Current();
				writeShape(output,child,layer+1);
			}
			break;
		}
	case TopAbs_EDGE:
		{
			TopoDS_Edge edge = TopoDS::Edge(shape);
			Standard_Real firstParam(0.0), lastParam(0.0);
			Handle(Geom_Curve) curve = BRep_Tool::Curve( edge, firstParam, lastParam );
			if( curve && !curve.IsNull() ){
				output << margin << "    firstParam: " << firstParam << std::endl;
				output << margin << "    lastParam: " << lastParam << std::endl;
				if( curve->IsKind( STANDARD_TYPE(Geom_OffsetCurve) ) ){
					output << margin << "    geometry: OffsetCurve" << std::endl;
				}else if( curve->IsKind( STANDARD_TYPE(Geom_Line) ) ){
					output << margin << "    geometry: Line" << std::endl;
					Handle(Geom_Line) line = Handle(Geom_Line)::DownCast(curve);
					output << margin << "    direction: [ " <<
						line->Position().Direction().X() << ", " <<
						line->Position().Direction().Y() << ", " <<
						line->Position().Direction().Z() << " ]" << std::endl;
					output << margin << "    location: [ " <<
						line->Position().Location().X() << ", " <<
						line->Position().Location().Y() << ", " <<
						line->Position().Location().Z() << " ]" << std::endl;
				}else if( curve->IsKind( STANDARD_TYPE(Geom_Parabola) ) ){
					output << margin << "    geometry: Parabola" << std::endl;
				}else if( curve->IsKind( STANDARD_TYPE(Geom_Hyperbola) ) ){
					output << margin << "    geometry: Hyperbola" << std::endl;
				}else if( curve->IsKind( STANDARD_TYPE(Geom_Ellipse) ) ){
					output << margin << "    geometry: Ellipse" << std::endl;
				}else if( curve->IsKind( STANDARD_TYPE(Geom_Circle) ) ){
					output << margin << "    geometry: Circle" << std::endl;
				}else if( curve->IsKind( STANDARD_TYPE(Geom_TrimmedCurve) ) ){
					output << margin << "    geometry: TrimmedCurve" << std::endl;
				}else if( curve->IsKind( STANDARD_TYPE(Geom_BSplineCurve) ) ){
					output << margin << "    geometry: BSplineCurve" << std::endl;
					Handle(Geom_BSplineCurve) bcurve = Handle(Geom_BSplineCurve)::DownCast(curve);
					output << margin << "    order: " << bcurve->Degree()+1 << std::endl;
					output << margin << "    knots: " << std::endl;
					int kn = bcurve->NbKnots();
					for(int i=1;i<=kn;++i){
						int multi = bcurve->Multiplicity(i);
						for(int j=0;j<multi;++j){
							output << margin << "      - " << bcurve->Knot(i) << std::endl;;
						}
					}
					output << margin << "    ctrlpts: " << std::endl;
					int cn = bcurve->NbPoles();
					for(int i=1;i<=cn;++i){
						gp_Pnt pt = bcurve->Pole(i);
						Standard_Real w = bcurve->Weight(i);
						output << margin << "      - [ " <<
							pt.X() << ", " <<
							pt.Y() << ", " <<
							pt.Z() << ", " <<
							w << " ]" << std::endl;;
					}
				}else if( curve->IsKind( STANDARD_TYPE(Geom_BezierCurve) ) ){
					output << margin << "    geometry: BezierCurve" << std::endl;
					Handle(Geom_BezierCurve) bcurve = Handle(Geom_BezierCurve)::DownCast(curve);
					output << margin << "    ctrlpts: " << std::endl;
					int cn = bcurve->NbPoles();
					for(int i=1;i<=cn;++i){
						gp_Pnt pt = bcurve->Pole(i);
						Standard_Real w = bcurve->Weight(i);
						output << margin << "      - [ " <<
							pt.X() << ", " <<
							pt.Y() << ", " <<
							pt.Z() << ", " <<
							w << " ]" << std::endl;;
					}
				}
			}
			output << margin << "    vertices:" << std::endl;
			TopExp_Explorer exp;
			for( exp.Init(shape,TopAbs_VERTEX); exp.More(); exp.Next() ){
				TopoDS_Shape child = exp.Current();
				writeShape(output,child,layer+1);
			}
			break;
		}
	case TopAbs_VERTEX:
		{
			TopoDS_Vertex vertex = TopoDS::Vertex(shape);
			Standard_Real x = BRep_Tool::Pnt(vertex).X();
			Standard_Real y = BRep_Tool::Pnt(vertex).Y();
			Standard_Real z = BRep_Tool::Pnt(vertex).Z();
			output << margin << "    position: [ " << x << ", " << y << ", " << z << " ]" << std::endl;
			break;
		}
	case TopAbs_SHAPE:
		break;
	default:
		break;
	}
	return 0;
}

#endif
