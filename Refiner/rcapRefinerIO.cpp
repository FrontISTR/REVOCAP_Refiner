/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.1                          #
# Program Name : rcapRefinerIO                                         #
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

#include "rcapRefiner.h"
#include "rcapRefinerStruct.h"
#include "Geometry/kmbIdTypes.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElement.h"
#include "MeshGen/kmbMeshRefiner.h"
#include "Geometry/kmbPoint3DContainerMArray.h"
#include "MeshDB/kmbElementContainerNArray.h"
#include "MeshDB/kmbElementContainerMixedArray.h"
#include "MeshDB/kmbBoundaryExtractor.h"
#include "MeshDB/kmbIntegerMapBindings.h"
#include "MeshGen/kmbMiddleNodeManager.h"

#include "RevocapIO/kmbFFbIO.h"
#include "RevocapIO/kmbHecmwIO.h"
#include "RevocapIO/kmbRevocapNeutralIO.h"
#include "RevocapIO/kmbRevocapCouplerIO.h"

#include "Shape/kmbRnfShapeIO.h"
#include "Shape/kmbSurface3D.h"
#include "Shape/kmbMiddleNodeManagerWithShape.h"

#include <stdio.h>
#include <string.h>
#include <vector>

#ifdef WIN32
 #if _MSC_VER >= 1400
  #define strcpy(ss0,ss1) strcpy_s(ss0,sizeof(ss1),ss1)
 #endif
#endif

extern struct rcapRefinerInfo rcapRefinerDoc;

int32_t rcapLoadGFFile( const char* gffile, const char* bounfile )
{
	kmb::FFbIO ffbIO;
	rcapRefinerDoc.mesh->clearModel();
	rcapRefinerDoc.mesh->beginModel();
	ffbIO.loadFromMeshFile( gffile, rcapRefinerDoc.mesh );
	ffbIO.loadFromBoundaryFile( bounfile, rcapRefinerDoc.mesh );
	rcapRefinerDoc.mesh->endModel();
	rcapRefinerDoc.maxElementId = rcapRefinerDoc.mesh->getMaxElementId();

	rcapRefinerDoc.refiner->setMesh( rcapRefinerDoc.mesh );
	rcapRefinerDoc.boundaryRefiner->setMesh( rcapRefinerDoc.mesh );










	return static_cast<int32_t>(rcapRefinerDoc.mesh->getElementCount());
}

int32_t rcapLoadHECFile( const char* hecfile )
{
	kmb::HecmwIO hecIO;
	rcapRefinerDoc.mesh->clearModel();
	hecIO.loadFromFile( hecfile, rcapRefinerDoc.mesh );
	return static_cast<int32_t>(rcapRefinerDoc.mesh->getElementCount());
}




void rcapRefineFFbModel( void )
{
	size_t dataNum = 0;
	dataNum += rcapRefinerDoc.mesh->getIdCount( "BC_INLT" );
	dataNum += rcapRefinerDoc.mesh->getIdCount( "BC_WALL" );
	dataNum += rcapRefinerDoc.mesh->getIdCount( "BC_FREE" );
	int32_t* nodeArray = new int32_t[dataNum];
	int32_t* nodeVars = new int32_t[dataNum];
	unsigned int i= 0,j=0;
	double inlet_v[3] = {0.0, 0.0, 1.0};
	kmb::DataBindings::iterator dIterInlt = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_INLT" )->begin();
	while( !dIterInlt.isFinished() ){
		nodeArray[i] = dIterInlt.getId();
		dIterInlt.getValue( inlet_v );
		nodeVars[i] = 1;
		++dIterInlt;
		++i;
	}
	kmb::DataBindings::iterator dIterWall = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_WALL" )->begin();
	while( !dIterWall.isFinished() ){
		nodeArray[i] = dIterWall.getId();
		nodeVars[i] = 3;
		++dIterWall;
		++i;
	}
	kmb::DataBindings::iterator dIterFree = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_FREE" )->begin();
	while( !dIterFree.isFinished() ){
		nodeArray[i] = dIterFree.getId();
		nodeVars[i] = 5;
		++dIterFree;
		++i;
	}
	rcapAppendBNodeVarInt( "BC", dataNum, nodeArray, nodeVars );
	delete[] nodeArray;
	delete[] nodeVars;


	size_t elementCount = rcapRefinerDoc.mesh->getElementCount(0);
	int32_t* nodeTable = new int32_t[elementCount*8];
	int8_t* typeTable = new int8_t[elementCount];

	kmb::ElementContainer::const_iterator eIter = rcapRefinerDoc.mesh->getBodyPtr(0)->begin();
	i = 0; j = 0;
	while( !eIter.isFinished() ){
		int num = eIter.getNodeCount();
		for(int k=0;k<num;++k){
			nodeTable[i+k] = eIter.getCellId(k);
		}
		typeTable[j] = eIter.getType();
		i += num;
		++j;
		++eIter;
	}

	size_t refineNum = 0;
	size_t refineNodeCount = 0;
	refineNodeCount = rcapRefineElementMulti( elementCount, typeTable, nodeTable, &refineNum, NULL, NULL );
	int32_t* refineNodeTable = new int32_t[refineNodeCount];
	int8_t* refineTypeTable = new int8_t[refineNum];
	refineNodeTable[0] = 0;
	rcapRefineElementMulti( elementCount, typeTable, nodeTable, &refineNum, refineTypeTable, refineNodeTable );

	rcapCommit();

	rcapRefinerDoc.mesh->clearBody(0);
	rcapRefinerDoc.mesh->beginElement( refineNum );
	kmb::nodeIdType nodes[20];
	i = 0;
	for(j=0;j<refineNum;++j){
		kmb::elementType etype = static_cast<kmb::elementType>(refineTypeTable[j]);
		int num = kmb::Element::getNodeCount( etype );
		for(int k=0;k<num;++k){
			nodes[k] = refineNodeTable[i+k];
		}
		i += num;
		rcapRefinerDoc.mesh->addElement( etype, nodes );
	}
	rcapRefinerDoc.mesh->endElement();


	kmb::DataBindings::iterator dIterBC = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC" )->begin();
	kmb::DataBindings* inlt = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_INLT" );
	kmb::DataBindings* wall = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_WALL" );
	kmb::DataBindings* free = rcapRefinerDoc.mesh->getDataBindingsPtr( "BC_FREE" );
	inlt->clear();
	wall->clear();
	free->clear();
	long l=0;
	while( !dIterBC.isFinished() ){
		if( dIterBC.getValue( &l ) ){
			switch(l){
				case 1:
					inlt->setPhysicalValue( dIterBC.getId(), inlet_v );
					break;
				case 3:
					wall->addId( dIterBC.getId() );
					break;
				case 5:
					free->addId( dIterBC.getId() );
					break;
				default:
					break;
			}
		}
		++dIterBC;
	}

	size_t nodeCount = rcapRefinerDoc.mesh->getNodeCount();
	int8_t etype;
	int32_t orgNodes[20];
	for(j=0;j<nodeCount;++j){
		for(int k=0;k<8;++k){
			orgNodes[k]= -1;
		}
		etype = rcapGetOriginal( j, orgNodes );
	}

	delete[] nodeTable;
	delete[] refineNodeTable;
	delete[] typeTable;
	delete[] refineTypeTable;

	rcapRefinerDoc.mesh->clearBody(1);
	rcapRefinerDoc.mesh->clearBody(2);
}

int32_t rcapSaveGFFile( const char* gffile, const char* bounfile )
{
	kmb::FFbIO ffbIO;
	ffbIO.saveToMeshFile( gffile, rcapRefinerDoc.mesh );
	ffbIO.saveToBoundaryFile( bounfile, rcapRefinerDoc.mesh );
	return static_cast<int32_t>(rcapRefinerDoc.mesh->getElementCount());
}

int32_t rcapSaveHECFile( const char* hecfile )
{
	kmb::HecmwIO hecIO;
	hecIO.saveToFile( hecfile, rcapRefinerDoc.mesh );
	return static_cast<int32_t>(rcapRefinerDoc.mesh->getElementCount());
}

int32_t rcapSaveRNFFile( const char* rnffile )
{
	kmb::RevocapNeutralIO rnfIO;
	rnfIO.saveToRNFFile( rnffile, rcapRefinerDoc.mesh );
	return static_cast<int32_t>(rcapRefinerDoc.mesh->getElementCount());
}

/* rcapxxx_  Ç∑Ç◊Çƒè¨ï∂éö */
#ifdef FORTRAN90
int32_t rcaploadgffile_( const char* gffile, const char* bounfile ){
	return rcapLoadGFFile( gffile, bounfile );
}

int32_t rcaploadhecfile_( const char* hecfile ){
	return rcapLoadHECFile( hecfile );
}

int32_t rcapsavegffile_( const char* gffile, const char* bounfile ){
	return rcapSaveGFFile( gffile, bounfile );
}

int32_t rcapsavehecfile_( const char* hecfile ){
	return rcapSaveHECFile( hecfile );
}

int32_t rcapsavernffile_( const char* hecfile ){
	return rcapSaveRNFFile( hecfile );
}

void rcaprefineffbmodel_(void){
	rcapRefineFFbModel();
}
#endif
