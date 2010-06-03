!/*----------------------------------------------------------------------
!#                                                                      #
!# Software Name : REVOCAP_Refiner version 0.4                          #
!# Program Name : rcapRefiner                                           #
!#                                                                      #
!#                                Written by                            #
!#                                           K. Tokunaga 2010/03/23     #
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
! example: tetra faces
!     1 [2,3,4]
!     2 [1,4,3]
!     3 [1,2,4]
!     4 [1,3,2]

      integer(kind=2), dimension(12), parameter :: RCAP_TETRAFACES = (/
     & 2, 3, 4,
     & 1, 4, 3,
     & 1, 2, 4,
     & 1, 3, 2 /)

      integer(kind=2), dimension(24), parameter :: RCAP_HEXAFACES = (/
     & 4, 3, 2, 1,
     & 5, 6, 7, 8,
     & 2, 6, 5, 1,
     & 2, 3, 7, 6,
     & 4, 8, 7, 3,
     & 5, 8, 4, 1 /)

      integer(kind=2), dimension(20), parameter :: RCAP_PYRAFACES = (/
     & 1, 2, 3,-1,
     & 1, 3, 4,-1,
     & 1, 4, 5,-1,
     & 1, 5, 2,-1,
     & 5, 4, 3, 2 /)

      integer(kind=2), dimension(20), parameter :: RCAP_WEDGEFACES = (/
     & 1, 3, 2,-1,
     & 4, 5, 6,-1,
     & 1, 2, 5, 4,
     & 2, 3, 6, 5,
     & 3, 1, 4, 6 /)

!#ifdef LINUX64
!     integer(kind=2), parameter :: RCAP_SIZE_T = 8
!#else
!     integer(kind=2), parameter :: RCAP_SIZE_T = 4
!#endif

! C åæåÍÇ≈äOïîíËã`Ç≥ÇÍÇΩÇ‡ÇÃÇÕ interface êÈåæÇµÇƒÇÕÉ_ÉÅ
      interface
!        subroutine rcapInitRefiner( nodeOffset, elementOffset )
!          integer(kind=4), intent(in) :: nodeOffset
!          integer(kind=4), intent(in) :: elementOffset
!        end subroutine rcapInitRefiner

!        subroutine rcapClearRefiner()
!        end subroutine rcapClearRefiner

!        subroutine rcapTermRefiner()
!        end subroutine rcapTermRefiner

!        subroutine rcapSetCADFilename( filename )
!          character(80), intent(in) :: filename
!        end subroutine rcapSetCADFilename

!        subroutine rcapSetPartitionFilename( filename )
!          character(80), intent(in) :: filename
!        end subroutine rcapSetPartitionFilename

!        subroutine rcapSetNode64( num, coords, globalIds, localIds )
!          integer(kind=8), intent(in) :: num
!          double precision, intent(in) :: coords(:)
!          integer(kind=4), intent(in) :: globalIds(:)
!          integer(kind=4), intent(in) :: localIds(:)
!        end subroutine rcapSetNode64

!        subroutine rcapSetNode32( num, coords, globalIds, localIds )
!          integer(kind=8), intent(in) :: num
!          real, intent(in) :: coords(:)
!          integer(kind=4), intent(in) :: globalIds(:)
!          integer(kind=4), intent(in) :: localIds(:)
!        end subroutine rcapSetNode32

!        subroutine rcapGetNode64( num, localIds, coords )
!          integer(kind=8), intent(in) :: num
!          integer(kind=4), intent(in) :: localIds(:)
!          double precision, intent(inout) :: coords(:)
!        end subroutine rcapGetNode64

!        subroutine rcapGetNode32( num, localIds, coords )
!          integer(kind=8), intent(in) :: num
!          integer(kind=4), intent(in) :: localIds(:)
!          real, intent(inout) :: coords(:)
!        end subroutine rcapGetNode32

!        subroutine rcapGetNodeSeq64( num, initId, coords )
!          integer(kind=8), intent(in) :: num
!          integer(kind=8), intent(in) :: initId
!          double precision, intent(inout) :: coords(:)
!        end subroutine rcapGetNodeSeq64

!        subroutine rcapGetNodeSeq32( num, initId, coords )
!          integer(kind=8), intent(in) :: num
!          integer(kind=8), intent(in) :: initId
!          real ,intent(inout) :: coords(:)
!        end subroutine rcapGetNodeSeq32

!        integer(kind=8) function rcapGetNodeCount()
!        end function rcapGetNodeCount

!        integer(kind=8) function rcapRefineElement( num, etype, nodeArray, resultNodeArray )
!          integer(kind=8), intent(in) :: num
!          integer(kind=2), intent(in) :: etype
!          integer(kind=4), intent(in) :: nodeArray(:)
!          integer(kind=4), intent(inout) :: resultNodeArray(:)
!        end function rcapRefineElement

!        subroutine rcapAppendNodeGroup( dataname, num, nodeArray )
!          character*80, intent(in) :: dataname
!          integer(kind=8), intent(in) :: num
!          integer(kind=4), intent(in) :: nodeArray(:)
!        end subroutine rcapAppendNodeGroup

!        integer(kind=8) function rcapGetNodeGroupCount( dataname )
!          character*80, intent(in) :: dataname
!        end function rcapGetNodeGroupCount
      end interface

      external :: rcapGetVersion
      external :: rcapInitRefiner
      external :: rcapClearRefiner
      external :: rcapTermRefiner
      external :: rcapSetCADFilename
      external :: rcapSetPartitionFilename
      external :: rcapSetNode64
      external :: rcapSetNode32
      external :: rcapGetNode64
      external :: rcapGetNode32
      external :: rcapGetNodeSeq64
      external :: rcapGetNodeSeq32
      external :: rcapCommit
      integer(kind=4) :: rcapGetNodeCount
      integer(kind=4) :: rcapRefineElement
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
