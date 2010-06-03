!/*----------------------------------------------------------------------
!#                                                                      #
!# Software Name : REVOCAP_Refiner version 0.4                          #
!# Sample Program Multi By Fortran90                                    #
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
! g++ -c -DGFORTRAN rcapRefiner.cpp
! gfortran mainMulti.F90 rcapRefiner.o -lstdc++ -lRcapRefiner -L../lib/i486-linux/
!
! 拡張子を F90 にすると、gfortran でプリプロセッサをかけてくれる
!
! 三角柱を縦に二つ重ねて、下のものに六面体をつけている
!
!
PROGRAM RefinerSampleMulti
  IMPLICIT NONE
! RCAP_SIZE_T の設定は include の前にやること
  integer, parameter :: RCAP_SIZE_T = 4
  INCLUDE "rcapRefiner90.inc"
  INTEGER*4 :: I,J,K

! 節点配列
  DOUBLE PRECISION :: coords(3*13)
  DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: resultCoords

  INTEGER*4 :: nodeCount
  INTEGER*4 :: refineNodeCount
  INTEGER*4 :: appendNodeCount
  INTEGER*4 :: elementCount
  INTEGER*4 :: refineElementCount
  INTEGER*4 :: nodeId

  INTEGER*4, DIMENSION(13) :: globalIds
  INTEGER*4, DIMENSION(1) :: nullIds
  INTEGER*4, DIMENSION(6+6+8) :: elements
  INTEGER*1, DIMENSION(3) :: elementTypes
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: refineElements
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: refineElementTypes
  INTEGER*4, DIMENSION(20) :: originalNode
  INTEGER*4, DIMENSION(6) :: ng0
  INTEGER*4, DIMENSION(6) :: nv0
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: result_ng0
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: result_nv0
  INTEGER*4 :: res
  CHARACTER(80) :: str

  INTEGER*4 :: nodeOffset = 1
  INTEGER*4 :: elementOffset = 1

  PRINT *, "TETRAHEDRON", RCAP_TETRAHEDRON
  PRINT *, "WEDGE", RCAP_WEDGE
  PRINT *, "HEXAHEDRON", RCAP_HEXAHEDRON

! 初期化：節点番号と要素番号の開始番号を指定する
  CALL rcapInitRefiner(nodeOffset,elementOffset)

  coords = (/&
        -1.0, 0.0, 0.0, &
         0.0, 0.0, 0.0, &
         0.0, 1.0, 0.0, &
        -1.0, 0.0, 1.0, &
         0.0, 0.0, 1.0, &
         0.0, 1.0, 1.0, &
        -1.0, 0.0, 2.0, &
         0.0, 0.0, 2.0, &
         0.0, 1.0, 2.0, &
         1.0, 0.0, 0.0, &
         1.0, 1.0, 0.0, &
         1.0, 0.0, 1.0, &
         1.0, 1.0, 1.0 /)
  nodeCount = 13

  globalIds = (/&
        100, 101, 102, 103, 104, &
        105, 106, 107, 108, 109, &
        110, 111, 112 /)
  nullIds = (/ 0 /)
  elements = (/&
        1,  2,  3,  4,  5,  6, &
        4,  5,  6,  7,  8,  9, &
        2, 10, 11,  3,  5, 12, 13,  6 /)
  elementTypes = (/ RCAP_WEDGE, RCAP_WEDGE, RCAP_HEXAHEDRON /)
  elementCount = 3

  ng0 = (/ 1, 2, 4, 5, 10, 12 /)
  nv0 = (/ 1, 1, 1, 1, 1, 1 /)

  PRINT *, "---------------------- ORIGINAL -----------------------------------------"
! global Id と座標値を Refiner に教える
  CALL rcapSetNode64( nodeCount, coords, globalIds, 0 )
  PRINT *, "Original Element Count ", elementCount

! CHAR(0) を concat しておく
  str = "BND"//CHAR(0)
  nodeCount = 6
  CALL rcapAppendBNodeVarInt(str,nodeCount,ng0,nv0)
  nodeCount = rcapGetNodeCount()
  PRINT *, "Original Node Count ", nodeCount

  PRINT *, "---------------------- REFINE -----------------------------------------"
! 要素の細分
! 細分後の節点配列の個数を調べる
  refineNodeCount = rcapRefineElementMulti( elementCount, elementTypes, elements, refineElementCount, -1, -1 )
  PRINT *, "Refine Node Table Count ", refineNodeCount
  PRINT *, "Refine Element Count ", refineElementCount
  ALLOCATE( refineElements(refineNodeCount) )
  ALLOCATE( refineElementTypes(refineElementCount) )
  refineElements(1) = 0
  refineElementTypes(1) = 0
  res = rcapRefineElementMulti( elementCount, elementTypes, elements, refineElementCount, refineElementTypes, refineElements )
  PRINT *, "Refined Element Count ", res
  DO I = 1, 8*2
    PRINT *, "NODE TABLE ", I, refineElements((I-1)*6+1:I*6)
  END DO
  DO I = 1, 8
    PRINT *, "NODE TABLE ", I+16, refineElements(96+(I-1)*8+1:96+I*8)
  END DO
  DEALLOCATE( refineElements )
  DEALLOCATE( refineElementTypes )

  CALL rcapCommit()

! 細分後の節点グループの取得
  str = "BND"//CHAR(0)
  res = rcapGetBNodeVarIntCount(str)
  ALLOCATE( result_ng0(res) )
  ALLOCATE( result_nv0(res) )
  CALL rcapGetBNodeVarInt(str,res,result_ng0,result_nv0)
  PRINT *, "Refined Node Group Count ", res
  DO I = 1, res
    PRINT *, "NODE VAR INT ", I, result_ng0(I), result_nv0(1)
  END DO
  DEALLOCATE( result_ng0 )
  DEALLOCATE( result_nv0 )

! 第２段の細分の前にキャッシュをクリア
  CALL rcapClearRefiner()
  CALL rcapTermRefiner()

END PROGRAM RefinerSampleMulti
