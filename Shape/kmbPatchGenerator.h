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
#pragma once

#ifdef OPENCASCADE

namespace kmb{

class MeshData;
class ShapeData;

class PatchGenerator
{
private:
	double deflection;
	double incremental;
	double tolerance;
public:
	PatchGenerator(void);
	virtual ~PatchGenerator(void);
	/*
	 * mesh ‚Ι‚Ν¶¬‚µ‚½ CAD –Κ‚Μ HashCode ‚π BODYVARIABLE ‚Μ Data –Ό FaceHashCode ‚Ε—^‚¦‚ι
	 */
	bool execute(kmb::ShapeData& shape,kmb::MeshData& mesh);
	/*
	 * Op`‰»‚·‚ι‚Μ•Σ‚Μ’·‚³‚ΜΕ¬’l‚π—^‚¦‚ι
	 * BRepTools::Triangulation
	 */
	void setDeflection(double d);
	double getDeflection(void);
	/*
	 * BRepMesh::Mesh
	 */
	void setIncremental(double d);
	double getIncremental(void);
	/*
	 * ’Έ“_‚π“―κ‹‚·‚ιθ‡’l
	 */
	void setTolerance(double d);
	double getTolerance(void);
};

}

#endif
