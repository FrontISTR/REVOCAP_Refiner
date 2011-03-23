/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : RevocapIOUtils                                          #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "RevocapIO/kmbRevocapIOUtils.h"
#include "Geometry/kmb_Common.h"

#include <algorithm>
#include <cctype>

std::string
kmb::RevocapIOUtils::getValue( std::string exp, std::string key, bool keyCaseSensitive )
{
	if( keyCaseSensitive ){
		std::string::size_type keyFirst = exp.find( key );
		if( keyFirst == std::string::npos ) return "";
		std::string::size_type eqEnd = exp.find( "=", keyFirst )+1;
		if( eqEnd == std::string::npos ) return "";
		std::string::size_type valFirst = exp.find_first_not_of( " ", eqEnd );
		if( valFirst == std::string::npos ) return "";
		std::string::size_type valEnd = exp.find_first_not_of( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-", valFirst );

		return exp.substr( valFirst, valEnd-valFirst);
	}else{

		std::string expDownCase = exp;
		std::string keyDownCase = key;
		std::transform( exp.begin(), exp.end(), expDownCase.begin(), tolower);
		std::transform( key.begin(), key.end(), keyDownCase.begin(), tolower);
		std::string::size_type keyFirst = expDownCase.find( keyDownCase );
		if( keyFirst == std::string::npos ) return "";
		std::string::size_type eqEnd = expDownCase.find( "=", keyFirst )+1;
		if( eqEnd == std::string::npos ) return "";
		std::string::size_type valFirst = expDownCase.find_first_not_of( " ", eqEnd );
		if( valFirst == std::string::npos ) return "";
		std::string::size_type valEnd = expDownCase.find_first_not_of( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-", valFirst );


		return exp.substr( valFirst, valEnd-valFirst);
	}
}
