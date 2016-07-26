/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Common Definitions for Error                                         #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2014/11/21     #
#                                                                      #
----------------------------------------------------------------------*/

namespace kmb{

class ErrorCode{
public:
	enum errType{
		kSUCCESS = 0,
		kERROR = -1,
	};
};

}
