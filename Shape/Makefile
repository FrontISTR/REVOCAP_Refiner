#
# https://github.com/tpaviot/oce.git
#

include ../OPTIONS
include ../MakefileConfig.in

LIBDIR = ../lib
REVOCAPDIR = ..
STATIC_LIB = libRcapShape.a

# for https://github.com/tpaviot/oce.git
# CASROOT is the target of "make install"
INCFLAGS = -I. -I$(CASROOT)/include/oce -I$(REVOCAPDIR)
LIBPATH =  -L$(LIBDIR)/$(ARCH) -L$(CASROOT)/Win64/lib
LIBS = -lRcapGeometry -lRcapMeshDB -lTKSTEPBase -lTKSTEPAttr -lTKSTEP209 \
 -lTKSTEP -lTKSTL -lTKVRML -lTKV3d -lTKV2d -lTKMesh -lTKTopAlgo -lTKService \
 -lTKPShape -lTKPrim -lTKPCAF -lTKOffset -lTKMath -lTKHLR -lTKGeomBase \
 -lTKGeomAlgo -lTKG3d -lTKG2d -lTKFillet -lTKFeat -lTKernel \
 -lTKCDF -lTKCAF -lTKLCAF -lTKXCAF -lTKBool -lPTKernel -lFWOSPlugin \
 -lTKBO -lTKShapeSchema -lTKXSBase -lTKShHealing -lTKBRep -lTKIGES -lTKXDEIGES -lTKBinL \
 -lRcapShape

# for OpenCASCADE 6.3
#INCFLAGS = -I. -I/usr/include/opencascade -I$(REVOCAPDIR)
#LIBPATH =  -L$(LIBDIR)/$(ARCH) -L$(CASROOT)/Linux/lib
#LIBS = -lRcapGeometry -lRcapMeshDB -lTKSTEPBase -lTKSTEPAttr -lTKSTEP209 \
# -lTKSTEP -lTKSTL -lTKVRML -lTKV3d -lTKV2d -lTKMesh -lTKTopAlgo -lTKService \
# -lTKPShape -lTKPrim -lTKPCAF -lTKOffset -lTKMath -lTKHLR -lTKGeomBase \
# -lTKGeomAlgo -lTKG3d -lTKG2d -lTKFillet -lTKFeat -lTKernel \
# -lTKCDF -lTKCAF -lTKLCAF -lTKXCAF -lTKBool -lPTKernel -lFWOSPlugin \
# -lTKBO -lTKShapeSchema -lTKXSBase -lTKShHealing -lTKBRep -lTKIGES -lTKXDEIGES -lTKBinL \
# -lRcapShape

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: clean static

static : $(LIBDIR)/$(ARCH)/$(STATIC_LIB)

$(LIBDIR)/$(ARCH)/$(STATIC_LIB) : $(OBJS)
	@if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi;
	@if [ ! -d $(LIBDIR)/$(ARCH) ]; then mkdir $(LIBDIR)/$(ARCH); fi;
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) $(OBJS)

clean:
	@-$(RM) *.o
	@-$(RM) $(LIBDIR)/$(ARCH)/$(STATIC_LIB)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $(INCFLAGS) $(OPENCASCADEFLAG) $<

test: test.cxx
	$(CXX) $^ $(CXXFLAGS) $(INCFLAGS) $(LIBPATH) $(LIBS) -o $@

test-clean:
	@-$(RM) test

