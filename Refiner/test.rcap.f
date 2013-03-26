      PROGRAM XXX
      IMPLICIT NONE
      INCLUDE "rcapRefiner.inc"

C
      INTEGER*4 NOFSTP,NOFSTE
      DATA NOFSTP /1/
      DATA NOFSTE /1/
C
      INTEGER*4 NP,NE,NER
      DATA NP /8/
      DATA NE /1/
      INTEGER*4 NODE(8),NODER(64)
      DATA NODE /1,2,3,4,5,6,7,8/
C
      REAL*4 CORD(24)
      DATA CORD /0.0, 0.0, 0.0,
     *           1.0, 0.0, 0.0,
     *           1.0, 1.0, 0.0,
     *           0.0, 1.0, 0.0,
     *           0.0, 0.0, 1.0,
     *           1.0, 0.0, 1.0,
     *           1.0, 1.0, 1.0,
     *           0.0, 1.0, 1.0/
C
      INTEGER*4 NEDUM,I,J,IE
C
      INTEGER(KIND=1) ITYPE(1),ITYPER(8)
C
      WRITE(6,*)
      WRITE(6,*) '[1]: INITILIZE'
      CALL RCAPINITREFINER(NOFSTP,NOFSTE)
C
      WRITE(6,*)
      WRITE(6,*) '[2]: SET COORDINATE'
      CALL RCAPSETNODE32(NP,CORD,-1,0)
C
      WRITE(6,*)
      WRITE(6,*) '[3]: REFINE ELEMENT'
C
      ITYPE (1)=RCAP_HEXAHEDRON
      NER      = 0
      NEDUM=RCAPGETREFINEELEMENTMULTICOUNT(NE,ITYPE,NER)
      WRITE(6,*) 'AFTER 1st CALL'
      WRITE(6,*) 'NEDUM',NEDUM
      WRITE(6,*) 'NER  ',NER
C
      ITYPER(1)= 0
      NODER    = 0
      NER      = 3
C     NER      = 8
      NEDUM=RCAPREFINEELEMENTMULTI(NE ,ITYPE ,NODE,
     *                             NER,ITYPER,NODER)
      WRITE(6,*) 'AFTER 2nd CALL'
      WRITE(6,*) 'NEDUM',NEDUM
      WRITE(6,*) 'NER  ',NER
C
C  ===DEBUG===
      WRITE(6,*) 'NODE TABLE'
      WRITE(6,'(8I8)') (NODE (I),I=1,8)
      WRITE(6,*) 'NODE TABLE (REFINED)'
      WRITE(6,'(8I8)') (NODER(I),I=1,64)
CC    STOP
C
      WRITE(6,*)
      WRITE(6,*) '[4]: CLEAR'
      CALL rcapClearRefiner()
C
      WRITE(6,*)
      WRITE(6,*) '[5]: TERMINATE'
      CALL rcapTermRefiner()
C
      STOP
      END
