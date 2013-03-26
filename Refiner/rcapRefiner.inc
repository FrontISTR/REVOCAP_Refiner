!/*----------------------------------------------------------------------
!#                                                                      #
!# Software Name : REVOCAP_Refiner version 1.1                          #
!# Program Name : rcapRefiner                                           #
!#                                                                      #
!#                                Written by                            #
!#                                           K. Tokunaga 2012/03/23     #
!#                                                                      #
!#      Contact Address: IIS, The University of Tokyo CISS              #
!#                                                                      #
!#      "Large Scale Assembly, Structural Correspondence,               #
!#                                     Multi Dynamics Simulator"        #
!#                                                                      #
!----------------------------------------------------------------------*/
!C constants and external functions for REVOCAP Refiner

      integer(kind=1), parameter :: RCAP_UNKNOWNTYPE = -1
      integer(kind=1), parameter :: RCAP_SEGMENT = 0
      integer(kind=1), parameter :: RCAP_SEGMENT2 = 1
      integer(kind=1), parameter :: RCAP_TRIANGLE = 2
      integer(kind=1), parameter :: RCAP_TRIANGLE2 = 3
      integer(kind=1), parameter :: RCAP_QUAD = 4
      integer(kind=1), parameter :: RCAP_QUAD2 = 5
      integer(kind=1), parameter :: RCAP_TETRAHEDRON = 6
      integer(kind=1), parameter :: RCAP_TETRAHEDRON2 = 7
      integer(kind=1), parameter :: RCAP_WEDGE = 8
      integer(kind=1), parameter :: RCAP_WEDGE2 = 9
      integer(kind=1), parameter :: RCAP_PYRAMID = 10
      integer(kind=1), parameter :: RCAP_PYRAMID2 = 11
      integer(kind=1), parameter :: RCAP_HEXAHEDRON = 12
      integer(kind=1), parameter :: RCAP_HEXAHEDRON2 = 13

!C elements face relation table lists for REVOCAP Refiner

!      integer(kind=2), parameter :: RCAP_TETRAFACES(12) = (/
!     * 2, 3, 4,
!     * 1, 4, 3,
!     * 1, 2, 4,
!     * 1, 3, 2 /)

!      integer(kind=2), parameter :: RCAP_HEXAFACES(24) = (/
!     *  4, 3, 2, 1,
!     *  5, 6, 7, 8,
!     *  2, 6, 5, 1,
!     *  2, 3, 7, 6,
!     *  4, 8, 7, 3,
!     *  5, 8, 4, 1 /)

!      integer(kind=2), parameter :: RCAP_PYRAFACES(20) = (/
!     *  1, 2, 3,-1,
!     *  1, 3, 4,-1,
!     *  1, 4, 5,-1,
!     *  1, 5, 2,-1,
!     *  5, 4, 3, 2 /)

!      integer(kind=2), parameter :: RCAP_WEDGEFACES(20) = (/
!     *  1, 3, 2,-1,
!     *  4, 5, 6,-1,
!     *  1, 2, 5, 4,
!     *  2, 3, 6, 5,
!     *  3, 1, 4, 6 /)

      external :: rcapGetVersion
      external :: rcapInitRefiner
      external :: rcapClearRefiner
      external :: rcapTermRefiner
      external :: rcapSetCADFilename
      external :: rcapSetSecondFitting
      external :: rcapSetSmoothing
      external :: rcapSetPartitionFilename
      external :: rcapSetNode64
      external :: rcapSetNode32
      external :: rcapGetNode64
      external :: rcapGetNode32
      external :: rcapGetNodeSeq64
      external :: rcapGetNodeSeq32
      external :: rcapCommit
      integer(kind=4) :: rcapGetNodeCount
      integer(kind=4) :: rcapGetRefineElementCount
      integer(kind=4) :: rcapRefineElement
      integer(kind=4) :: rcapGetRefineElementMultiCount
      integer(kind=4) :: rcapRefineElementMulti

      external :: rcapAppendNodeGroup
      integer(kind=4) :: rcapGetNodeGroupCount
      external :: rcapGetNodeGroup

      external :: rcapAppendBNodeGroup
      integer(kind=4) :: rcapGetBNodeGroupCount
      external :: rcapGetBNodeGroup

      external :: rcapAppendBNodeVarInt
      integer(kind=4) :: rcapGetBNodeVarIntCount
      external :: rcapGetBNodeVarInt

      external :: rcapAppendElementGroup
      integer(kind=4) :: rcapGetElementGroupCount
      external :: rcapGetElementGroup

      external :: rcapAppendFaceGroup
      integer(kind=4) :: rcapGetFaceGroupCount
      external :: rcapGetFaceGroup

      integer(kind=2) :: rcapGetOriginal
      integer(kind=4) :: rcapGetMiddle

      external :: rcapGetInterpolateMode
      external :: rcapSetInterpolateMode

