
include ../OPTIONS
include ../MakefileConfig.in

LIBDIR = ../lib
BINDIR = ../bin
INCFLAGS = -I..
RCAPLIBS = -lRcapRefiner
RCAPLIBPATH = -L"$(LIBDIR)/$(ARCH)"
STATIC_LIB = libRcapRefiner.a

sampleF : main.F90 mainMulti.F90 test.rcap.f $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(F90) $(FFLAGS) -c main.F90
	$(F90) $(FFLAGS) -c mainMulti.F90
	$(F90) $(FFLAGS) -c test.rcap.f
	$(LD) $(LDFLAGS) -o sampleF main.o $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)
	$(LD) $(LDFLAGS) -o sampleMultiF mainMulti.o $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)
	$(LD) $(LDFLAGS) -o test.rcap test.rcap.o $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleC : main.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleC main.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleMultiStage : mainMultiStage.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleMultiStage mainMultiStage.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleNodeVariable : mainNV.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleNodeVariable mainNV.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleTetra : mainTetra.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleTetra mainTetra.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleHexa : mainHexa.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleHexa mainHexa.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleHexa2 : mainHexa2.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleHexa2 mainHexa2.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleMultiByType : mainMultiType.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleMultiByType mainMultiByType.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleMultiType : mainMultiType.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleMultiType mainMultiType.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

sampleFitting : mainFittingRefine1.c mainFittingRefine2.c mainFittingRefine3.c $(LIBDIR)/$(ARCH)/$(STATIC_LIB)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleFitting1 mainFittingRefine1.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleFitting2 mainFittingRefine2.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleFitting3 mainFittingRefine3.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)
	$(CC) $(CFLAGS) -D_CONSOLE -o sampleSmoothing mainSmoothingRefine.c $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

preFitting : mainPreFitting.cpp $(LIBDIR)/$(ARCH)/$(STATIC_LIB) bindir
	$(CXX) $(CXXFLAGS) -D_CONSOLE $(INCFLAGS) -o $(BINDIR)/$(ARCH)/rcapPreFitting mainPreFitting.cpp $(LIBPATH) $(RCAPLIBPATH) $(LIBS) $(RCAPLIBS)

rcapRefiner.o : rcapRefiner.cpp
	$(CXX) $(CXXFLAGS) -c -DFORTRAN90 $(INCFLAGS) rcapRefiner.cpp

static : rcapRefiner.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) rcapRefiner.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) ../Common/*.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) ../Matrix/*.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) ../Geometry/*.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) ../MeshDB/*.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) ../MeshGen/*.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) ../RevocapIO/*.o
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) ../Shape/*.o

.PHONY: clean
clean :
	@-$(RM) *.o a.out
	@-$(RM) sampleC sampleF sampleHexa sampleHexa2 sampleMultiByType sampleMultiF sampleMultiStage sampleMultiType sampleNodeVariable sampleRefineGF sampleFitting test.rcap
	@-$(RM) sampleC.exe sampleF.exe sampleHexa.exe sampleHexa2.exe sampleMultiByType.exe sampleMultiF.exe sampleMultiStage.exe sampleMultiType.exe sampleNodeVariable.exe sampleRefineGF.exe sampleFitting.exe test.rcap.exe
	@-$(RM) $(LIBDIR)/$(ARCH)/$(STATIC_LIB)

bindir:
	@if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR); fi;
	@if [ ! -d $(BINDIR)/$(ARCH) ]; then mkdir $(BINDIR)/$(ARCH); fi;

doc : Doxyfile
	$(DOXYGEN) Doxyfile

cleandoc :
	@-$(RM) docs
