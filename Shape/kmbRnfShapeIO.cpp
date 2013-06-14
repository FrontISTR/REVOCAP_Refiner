/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : RnfShapeIO                                              #
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
#include "Shape/kmbRnfShapeIO.h"
#include "Shape/kmbNurbsSurface3D.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

kmb::RnfShapeIO::RnfShapeIO(void)
{
}

kmb::RnfShapeIO::~RnfShapeIO(void)
{
}

int
kmb::RnfShapeIO::loadFromFile(const char* filename, std::vector< kmb::Surface3D* >& surfaces)
{
	std::ifstream input( filename, std::ios_base::in );
	if( input.fail() ){
		return -1;
	}
	enum dataType{
		UNKNOWN = -1,
		SURFACE = 1
	};
	dataType dataflag = UNKNOWN;
	std::string line;
	while( !input.eof() ){
		std::getline( input, line );
		if( line.find_first_of('#') == 0 ){
			continue;
		}
		if( line.find("surface:") == 0 ){
			dataflag = SURFACE;
		}
		if( dataflag == SURFACE && line.find("- nurbs:") == 2 ){
			kmb::NurbsSurface3D* nurbs = new kmb::NurbsSurface3D();
			readNurbs( input, nurbs );
			surfaces.push_back( nurbs );
		}
	}
	input.close();
	return 0;
}

int
kmb::RnfShapeIO::readNurbs(std::ifstream &input,kmb::NurbsSurface3D* nurbs)
{
	if( nurbs == NULL ){
		return -1;
	}
	std::string line, str;
	std::istringstream tokenizer;
	long id;
	int uOrder=0, vOrder=0;
	double x=0.0,y=0.0,z=0.0,w=0.0;
	char comma=0;
	while( !input.eof() ){
		std::getline( input, line );
		if( line.find("id:") != std::string::npos ){
			tokenizer.str(line);
			tokenizer.clear();
			tokenizer >> str >> id;
			nurbs->setSurfaceId(id);
		}else if( line.find("uknots:") != std::string::npos ){
			std::getline( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			tokenizer >> str >> uOrder;
			std::getline( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			tokenizer >> str;
			tokenizer >> comma;
			comma = 0;
			while( tokenizer.good() && comma != ']'){
				tokenizer >> w >> comma;
				nurbs->appendUKnot(w);
			}
		}else if( line.find("vknots:") != std::string::npos ){
			std::getline( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			tokenizer >> str >> vOrder;
			std::getline( input, line );
			tokenizer.str(line);
			tokenizer.clear();
			tokenizer >> str;
			tokenizer >> comma;
			comma = 0;
			while( tokenizer.good() && comma != ']'){
				tokenizer >> w >> comma;
				nurbs->appendVKnot(w);
			}
		}else if( line.find("ctrlpts:") != std::string::npos ){
			nurbs->setOrder( uOrder, vOrder );
			while( !nurbs->valid() ){
				std::getline( input, line );
				tokenizer.str(line);
				tokenizer.clear();
				tokenizer >> str >> comma;
				tokenizer >> x >> comma;
				tokenizer >> y >> comma;
				tokenizer >> z >> comma;
				tokenizer >> w >> comma;
				nurbs->appendCtrlPt( x, y, z, w );
			}
			break;
		}
	}
	return 0;
}

int
kmb::RnfShapeIO::appendSurfaceHeaderToFile(const char* filename)
{
	std::ofstream output( filename, std::ios_base::app );
	output << "surface:" << std::endl;
	output.close();
	return 0;
}

int
kmb::RnfShapeIO::appendSurfaceToFile(const char* filename, kmb::Surface3D* surface)
{
	if( surface == NULL ){
		return -1;
	}
	std::ofstream output( filename, std::ios_base::app );

	int surfaceId = 0;
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

	output.close();
	return 0;
}
