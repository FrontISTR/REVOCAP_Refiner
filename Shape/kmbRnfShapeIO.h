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

#pragma once

#include <vector>
#include <fstream>

namespace kmb{

class Surface3D;
class NurbsSurface3D;

class RnfShapeIO
{
public:
	RnfShapeIO(void);
	~RnfShapeIO(void);
	int loadFromFile(const char* filename, std::vector< kmb::Surface3D* >& surfaces);
	int appendSurfaceHeaderToFile(const char* filename);
	int appendSurfaceToFile(const char* filename, kmb::Surface3D* surface);
private:
	int readNurbs(std::ifstream &input,kmb::NurbsSurface3D* nurbs);

};

}
