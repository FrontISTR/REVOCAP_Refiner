/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Common Definitions for RevocapIO                                     #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2014/09/03     #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

namespace kmb{
	namespace RevocapIO{
		enum ErrorCode{
			kSuccess                   = 0,
			kError                     = -1,
			kFileOpenError             = 1000,
			kFileReadError             = 1001,
			kFileWriteError            = 1002,
			kHeaderError               = 1003,
			kHeaderSizeError           = 1004,
			kEndianError               = 2000,
			kNodeSizeError             = 3000,
			kNodeSizeNotImplemented    = 3001,
			kElementSizeError          = 4000,
			kElementSizeNotImplemented = 4001,
			kMeshNullError             = 5000,
			kDataNullError             = 6000,
		};
	}
}
