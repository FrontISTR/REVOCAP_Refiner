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
	 * mesh �ɂ͐������� CAD �ʂ� HashCode �� BODYVARIABLE �� Data �� FaceHashCode �ŗ^����
	 */
	bool execute(kmb::ShapeData& shape,kmb::MeshData& mesh);
	/*
	 * �O�p�`�����鎞�̕ӂ̒����̍ŏ��l��^����
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
	 * ���_�𓯈ꎋ����臒l
	 */
	void setTolerance(double d);
	double getTolerance(void);
};

}

#endif
