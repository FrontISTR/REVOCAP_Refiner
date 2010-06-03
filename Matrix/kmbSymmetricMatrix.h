/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : SymmetricMatrix                                         #
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

namespace kmb{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

class SymmetricMatrix : public SquareMatrix
{
public:
	SymmetricMatrix(int size){};
	virtual ~SymmetricMatrix(void){};
};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
