!/*----------------------------------------------------------------------
!#                                                                      #
!# Software Name : REVOCAP_Refiner version 1.1                          #
!# Sample Program Multi By Fortran90                                    #
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
  INCLUDE "rcapRefiner.inc"
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
  INTEGER*1, DIMENSION(:), ALLOCATABLE :: refineElementTypes
  INTEGER*4, DIMENSION(20) :: originalNode
  INTEGER*4, DIMENSION(6) :: ng0
  INTEGER*4, DIMENSION(6) :: nv0
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: result_ng0
  INTEGER*4, DIMENSION(:), ALLOCATABLE :: result_nv0
  INTEGER*4 :: res
  CHARACTER(80) :: str

  INTEGER*4 :: nodeOffset = 1
  INTEGER*4 :: elementOffset = 1

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
  nv0 = (/ 1, 1, 2, 1, 2, 2 /)

! global Id と座標値を Refiner に教える
  CALL rcapSetNode64( nodeCount, coords, globalIds, 0 )
  PRINT *, "----- Original Model -----"

  nodeCount = rcapGetNodeCount()
  PRINT '("Node : Count ="I8)', nodeCount
  PRINT '(I8,3F15.8)', (I, coords(3*I-2), coords(3*I-1), coords(3*I), I=1, nodeCount)

  PRINT '("Element : Count = "I8)', elementCount
  J = 1
  DO I = 1, elementCount
    IF ( elementTypes(I) == RCAP_WEDGE ) THEN
      PRINT '(I8,I8,6I8)', I, elementTypes(I), &
	elements(J), elements(J+1), elements(J+2), &
	elements(J+3), elements(J+4), elements(J+5)
      J = J + 6
    ELSE IF ( elementTypes(I) == RCAP_HEXAHEDRON ) THEN
      PRINT '(I8,I8,8I8)', I, elementTypes(I), &
	elements(J), elements(J+1), elements(J+2), elements(J+3), &
	elements(J+4), elements(J+5), elements(J+6), elements(J+7)
      J = J + 8
    END IF
  END DO

! CHAR(0) を concat しておく
  str = "BND"//CHAR(0)
  nodeCount = 6
  CALL rcapAppendBNodeVarInt(str,nodeCount,ng0,nv0)
  PRINT '("Node Variable : Count ="I8)', nodeCount
  PRINT '(I8,I8)', (ng0(I), nv0(I), I=1, nodeCount)

! ---------------------- REFINE -----------------------------------------
! 要素の細分
! 細分後の節点配列の個数を調べる

  PRINT *, "----- Refined Model -----"
  refineNodeCount = rcapGetRefineElementMultiCount( elementCount, elementTypes, refineElementCount )
  ALLOCATE( refineElements(refineNodeCount) )
  ALLOCATE( refineElementTypes(refineElementCount) )

  res = rcapRefineElementMulti( elementCount, elementTypes, elements, refineElementCount, refineElementTypes, refineElements )

  refineNodeCount = rcapGetNodeCount()
  ALLOCATE( resultCoords(3*refineNodeCount) )
  CALL rcapGetNodeSeq64( refineNodeCount, 1, resultCoords )
  PRINT '("Node : Count ="I8)', refineNodeCount
  PRINT '(I8,3F15.8)', (I, resultCoords(3*I-2), resultCoords(3*I-1), resultCoords(3*I), &
       & I=1, refineNodeCount)
  DEALLOCATE( resultCoords )

  PRINT '("Element : Count ="I8)', refineElementCount
  J = 1
  DO I = 1, refineElementCount
    IF ( refineElementTypes(I) == RCAP_WEDGE ) THEN
      PRINT '(I8,I8,6I8)', I, refineElementTypes(I), &
	refineElements(J), refineElements(J+1), refineElements(J+2), &
	refineElements(J+3), refineElements(J+4), refineElements(J+5)
      J = J + 6
    ELSE IF ( refineElementTypes(I) == RCAP_HEXAHEDRON ) THEN
      PRINT '(I8,I8,8I8)', I, refineElementTypes(I), &
	refineElements(J), refineElements(J+1), refineElements(J+2), refineElements(J+3), &
	refineElements(J+4), refineElements(J+5), refineElements(J+6), refineElements(J+7)
      J = J + 8
    END IF
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

  PRINT '("Node Variable : Count ="I8)', res
  PRINT '(I8,I8)', (result_ng0(I), result_nv0(I), I=1, res)

  DEALLOCATE( result_ng0 )
  DEALLOCATE( result_nv0 )

! 第２段の細分の前にキャッシュをクリア
  CALL rcapClearRefiner()
  CALL rcapTermRefiner()

END PROGRAM RefinerSampleMulti
