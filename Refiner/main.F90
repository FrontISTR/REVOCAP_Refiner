!/*----------------------------------------------------------------------
!#                                                                      #
!# Software Name : REVOCAP_Refiner version 0.4                          #
!# Sample Program By Fortran90                                          #
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
!
! REVOCAP_Refiner サンプルプログラム
!
! gfortran main.F90 -lstdc++ -lRcapRefiner -L../lib/i486-linux/
!
! 拡張子を F90 にすると、gfortran でプリプロセッサをかけてくれる
!

PROGRAM RefinerSample
  IMPLICIT NONE
! RCAP_SIZE_T の設定は include の前にやる
!#ifdef LINUX64
!  integer, parameter :: RCAP_SIZE_T = 8
!#else
  integer, parameter :: RCAP_SIZE_T = 4
!#endif
  INCLUDE "rcapRefiner90.inc"
  INTEGER*4 :: I,J,K

! 節点配列
  DOUBLE PRECISION :: coords(3*5)
  DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: resultCoords
!#ifdef LINUX64
!  INTEGER*8 :: nodeCount
!  INTEGER*8 :: refineNodeCount
!  INTEGER*8 :: appendNodeCount
!  INTEGER*8 :: elementCount
!#else
  INTEGER*4 :: nodeCount
  INTEGER*4 :: refineNodeCount
  INTEGER*4 :: appendNodeCount
  INTEGER*4 :: elementCount
!#endif
  INTEGER*4 :: nodeId
  INTEGER*4, DIMENSION(5) :: globalIds
  INTEGER*4, DIMENSION(1) :: nullIds
  INTEGER*4, DIMENSION(2*4) :: tetras
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: refineTetras
!  INTEGER*4, DIMENSION(64) :: refineTetras
  INTEGER*4, DIMENSION(20) :: originalNode
  INTEGER*4, DIMENSION(3) :: ng0
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: result_ng0
!  INTEGER*4, DIMENSION(6) :: result_ng0
  INTEGER*4 :: res
  CHARACTER(80) :: str

  INTEGER*4 :: nodeOffset = 1
  INTEGER*4 :: elementOffset = 1

  PRINT *, "TETRAHEDRON", RCAP_TETRAHEDRON
  PRINT *, "HEXAHEDRON", RCAP_HEXAHEDRON

! 初期化：節点番号と要素番号の開始番号を指定する
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

#ifdef LINUX64
  PRINT *, "LINUX64 enable"
#else
  PRINT *, "LINUX64 disable"
#endif

  PRINT *, "---------------------- ORIGINAL -----------------------------------------"
! global Id と座標値を Refiner に教える
  CALL rcapSetNode64( nodeCount, coords, globalIds, 0 )
  PRINT *, "Original Element Count ", elementCount

! 細分時に同時に更新する節点グループの登録（案）
! CHAR(0) を concat しておく
  str = "innovation project"//CHAR(0)
  nodeCount = 3
  CALL rcapAppendNodeGroup(str,nodeCount,ng0)
  PRINT *, "Original Node Group Count ", nodeCount
  nodeCount = rcapGetNodeCount()
  PRINT *, "Original Node Count ", nodeCount

  PRINT *, "---------------------- REFINE -----------------------------------------"
! 要素の細分
  ALLOCATE( refineTetras(elementCount*4*8) )
  res = rcapRefineElement( elementCount, RCAP_TETRAHEDRON, tetras, refineTetras )
  PRINT *, "Refined Element Count ", res
  DEALLOCATE( refineTetras )

  CALL rcapCommit()

! 細分後の節点グループの取得
  str = "innovation project"//CHAR(0)
  res = rcapGetNodeGroupCount(str)
  ALLOCATE( result_ng0(res) )
  CALL rcapGetNodeGroup(str,res,result_ng0)
  PRINT *, "Refined Node Group Count ", res
  DEALLOCATE( result_ng0 )

! 細分後の節点の個数
  refineNodeCount = rcapGetNodeCount()
  PRINT *, "Refined Node Count ", refineNodeCount

! 細分後の節点の座標
  ALLOCATE( resultCoords(3*refineNodeCount) )
  CALL rcapGetNodeSeq64( refineNodeCount, 1, resultCoords )

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

! 第２段の細分の前にキャッシュをクリア
  CALL rcapClearRefiner()
  CALL rcapTermRefiner()

END PROGRAM RefinerSample
