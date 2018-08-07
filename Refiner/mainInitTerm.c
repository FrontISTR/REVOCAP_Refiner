/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 0.4                          #
# Sample Program Initialize and Terminate                              #
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
 * gcc -D_CONSOLE mainInitTerm.c -L../lib/i486-linux/ -lstdc++ -lRcapRefiner
 *
 * �����������E�I�������̃e�X�g
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include <stdio.h>

int main(void)
{
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* �ߓ_�ԍ��̃I�t�Z�b�g�l��^���� */
	printf("rcapInitRefiner\n");
	rcapInitRefiner( nodeOffset, elementOffset );
	printf("rcapTermRefiner\n");
	rcapTermRefiner();
	printf("exit\n");
	return 0;
}

#endif
