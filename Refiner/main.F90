!/*----------------------------------------------------------------------
!#                                                                      #
!# Software Name : REVOCAP_Refiner version 1.1                          #
!# Sample Program By Fortran90                                          #
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
!
! REVOCAP_Refiner Sample Program
!

PROGRAM RefinerSample
  IMPLICIT NONE
  INCLUDE "rcapRefiner.inc"
  INTEGER*4 :: I,J,K

! ß“_”z—ñ
  DOUBLE PRECISION :: coords(3*5)
  DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: resultCoords
  INTEGER*4 :: nodeCount
  INTEGER*4 :: refineNodeCount
  INTEGER*4 :: appendNodeCount
  INTEGER*4 :: elementCount
  INTEGER*4 :: refineElementCount
  INTEGER*4 :: nodeId
  INTEGER*1 :: etype = RCAP_TETRAHEDRON
  INTEGER*4, DIMENSION(5) :: globalIds
  INTEGER*4, DIMENSION(1) :: nullIds
  INTEGER*4, DIMENSION(2*4) :: tetras
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: refineTetras
  INTEGER*4, DIMENSION(20) :: originalNode
  INTEGER*4, DIMENSION(3) :: ng0
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: result_ng0
  INTEGER*4 :: res
  CHARACTER(80) :: str

  INTEGER*4 :: nodeOffset = 1
  INTEGER*4 :: elementOffset = 1

! initialize : set initial node id and element id
  CALL rcapInitRefiner(nodeOffset,elementOffset)

  coords = (/&
       & 0.0, 0.0, 0.0, &
       & 1.0, 0.0, 0.0, &
       & 0.0, 1.0, 0.0, &
       & 0.0, 0.0, 1.0, &
       & 1.0, 1.0, 1.0  /)
  nodeCount = 5

  globalIds = (/ 100, 101, 102, 103, 104 /)
  nullIds = (/ 0 /)
  tetras = (/&
       & 1, 2, 3, 4, &
       & 2, 3, 4, 5  /)
  elementCount = 2

  ng0 = (/ 1, 2, 3 /)

! set global node id and coordinates
  CALL rcapSetNode64( nodeCount, coords, globalIds, 0 )

  PRINT *, "----- Original Model -----"

  nodeCount = rcapGetNodeCount()
  PRINT '("Node : Count ="I8)', nodeCount
  PRINT '(I8,3F15.8)', (I, coords(3*I-2), coords(3*I-1), coords(3*I), I=1, nodeCount)

  PRINT '("Element : Count ="I8)', elementCount
  PRINT '(I8,I8,4I8)', (I, etype, &
       & tetras(4*I-3), tetras(4*I-2), tetras(4*I-1), tetras(4*I), I=1, elementCount)

! set conditions which are updated simultaneously
! remark append CHAR(0)
  str = "innovation project"//CHAR(0)
  nodeCount = 3
  CALL rcapAppendNodeGroup(str,nodeCount,ng0)
  PRINT '("Node Group : Count ="I8)', nodeCount
  PRINT '(I8)', (ng0(I), I = 1, nodeCount)

! ---------------------- REFINE -----------------------------------------
  refineElementCount = rcapGetRefineElementCount( elementCount, RCAP_TETRAHEDRON )
  ALLOCATE( refineTetras(refineElementCount*4) )

  refineElementCount = rcapRefineElement( elementCount, RCAP_TETRAHEDRON, tetras, refineTetras )

  PRINT *, "----- Refined Model -----"

  refineNodeCount = rcapGetNodeCount()
  ALLOCATE( resultCoords(3*refineNodeCount) )
  CALL rcapGetNodeSeq64( refineNodeCount, 1, resultCoords )
  PRINT '("Node : Count ="I8)', refineNodeCount
  PRINT '(I8,3F15.8)', (I, &
       & resultCoords(3*I-2), resultCoords(3*I-1), resultCoords(3*I), I=1, refineNodeCount)

! check
  DO I = 1, refineNodeCount
    res = rcapGetOriginal( I, originalNode )
    IF ( res == RCAP_SEGMENT ) THEN
      IF( coords(3*originalNode(1)-2) + coords(3*originalNode(2)-2)  /= 2.0 * resultCoords(3*I-2) ) THEN
        PRINT *, "Refine Node Coordinate Error ", I, " => [", originalNode(1), ",", originalNode(2), "]"
        PRINT *, coords(3*originalNode(1)-2), ",", coords(3*originalNode(2)-2), ",", resultCoords(3*I-2)
      END IF
      IF( coords(3*originalNode(1)-1) + coords(3*originalNode(2)-1)  /= 2.0 * resultCoords(3*I-1) ) THEN
        PRINT *, "Refine Node Coordinate Error ", I, " => [", originalNode(1), ",", originalNode(2), "]"
        PRINT *, coords(3*originalNode(1)-1), ",", coords(3*originalNode(2)-1), ",", resultCoords(3*I-1)
      END IF
      IF( coords(3*originalNode(1)) + coords(3*originalNode(2))  /= 2.0 * resultCoords(3*I) ) THEN
        PRINT *, "Refine Node Coordinate Error ", I, " => [", originalNode(1), ",", originalNode(2), "]"
        PRINT *, coords(3*originalNode(1)), ",", coords(3*originalNode(2)), ",", resultCoords(3*I)
      END IF
    END IF
  END DO

  DEALLOCATE( resultCoords )

  PRINT '("Element : Count ="I8)', refineElementCount
  PRINT '(I8,I8,4I8)', (I, etype, &
       & refineTetras(4*I-3), refineTetras(4*I-2), refineTetras(4*I-1), refineTetras(4*I), &
       & I=1, refineElementCount)

  DEALLOCATE( refineTetras )

  CALL rcapCommit()

! get node group after refine
  str = "innovation project"//CHAR(0)
  res = rcapGetNodeGroupCount(str)
  ALLOCATE( result_ng0(res) )
  CALL rcapGetNodeGroup(str,res,result_ng0)

  PRINT '("Node Group : Count ="I8)', res
  PRINT '(I8)', (result_ng0(I), I=1, res)

  DEALLOCATE( result_ng0 )

! remark need clear refiner if you call refine process again
  CALL rcapClearRefiner()
  CALL rcapTermRefiner()

END PROGRAM RefinerSample
