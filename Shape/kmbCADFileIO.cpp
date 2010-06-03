/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : CADFileIO                                               #
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

#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StepToTopoDS_Builder.hxx>
#include <Interface_Static.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TDocStd_Document.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

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

#endif
