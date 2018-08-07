/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : RevocapNeutralIO                                        #
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
/*
 * �����ł� YAML �`���ŏ����ꂽ REVOCAP Neutral Yaml Mesh Format ��
 * YAML ���C�u�������g�킸�ɓǂݍ��ރ��\�b�h����������
 *
 * ���̃o�[�W�����ł́A�L�q����Ă���^�O�̏o�Ă��鏇�Ԃ����ߑł��ɂȂ��Ă���̒���
 */
/**
 * REVOCAP Neutral Yaml Mesh Format �d�l
 * # ���擪�̍s�̓R�����g
 * node �^�O�͐ߓ_
 * node/size �͐ߓ_�̌�
 * node/coordinate �͍��W�z��
 * node/coordinate[i] �� [id,x,y,z]
 * bodyCount �͗v�f�O���[�v�̌�
 * element �^�O�͗v�f�O���[�v�̔z��
 * element[i] �͗v�f�O���[�v
 * element[i]/size �͗v�f�O���[�v�Ɋ܂܂��v�f
 * element[i]/connectivity �͗v�f�O���[�v�Ɋ܂܂��v�f�̐ߓ_�z��
 * element[i]/connectivity[j] �� [id,type,node[0],node[1],..,node[k]]
 */
#pragma once

#include <fstream>

namespace kmb{

class MeshData;

class RevocapNeutralIO
{
public:
	RevocapNeutralIO(void);
	virtual ~RevocapNeutralIO(void);
	int loadFromRNFFile(const char* filename,kmb::MeshData* mesh);
	int saveToRNFFile(const char* filename,kmb::MeshData* mesh);
protected:
	int readNode(std::ifstream &input,kmb::MeshData* mesh);
	int readElement(std::ifstream &input,kmb::MeshData* mesh);
	int readData(std::ifstream &input,kmb::MeshData* mesh);
	int readShape(std::ifstream &input,kmb::MeshData* mesh);
};

}
