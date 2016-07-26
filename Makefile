# This is the makefile for REVOCAP_PrePost (mesh module).
#
# please edit OPTIONS file for the environmental information and macros.

include OPTIONS
include MakefileRefiner.in
include MakefileConfig.in

export ARCH
export CXXFLAG

export DATE = $(shell date +%Y%m%d)
LIBDIR = lib


Common: $(LIBDIR)/$(ARCH) $(LIBDIR)/$(ARCH)/libRcapCommon.a

$(LIBDIR)/$(ARCH)/libRcapCommon.a: force
	cd Common && $(MAKE) static

Matrix: Common $(LIBDIR)/$(ARCH)/libRcapMatrix.a

$(LIBDIR)/$(ARCH)/libRcapMatrix.a: force
	cd Matrix && $(MAKE) static

Geometry: Matrix Common $(LIBDIR)/$(ARCH)/libRcapGeometry.a

$(LIBDIR)/$(ARCH)/libRcapGeometry.a: force
	cd Geometry && $(MAKE) static

MeshDB: $(LIBDIR)/$(ARCH)/libRcapMeshDB.a

$(LIBDIR)/$(ARCH)/libRcapMeshDB.a: Geometry
	cd MeshDB && $(MAKE) static

MeshGen: MeshDB $(LIBDIR)/$(ARCH)/libRcapMeshGen.a

$(LIBDIR)/$(ARCH)/libRcapMeshGen.a: force
	cd MeshGen && $(MAKE) static

MeshGL: MeshDB $(LIBDIR)/$(ARCH)/libRcapMeshGL.a

$(LIBDIR)/$(ARCH)/libRcapMeshGL.a: force
	cd MeshGL && $(MAKE) static

RevocapIO: MeshDB $(LIBDIR)/$(ARCH)/libRcapIO.a

$(LIBDIR)/$(ARCH)/libRcapIO.a: force
	cd RevocapIO && $(MAKE) static

Shape: MeshDB $(LIBDIR)/$(ARCH)/libRcapShape.a

$(LIBDIR)/$(ARCH)/libRcapShape.a: force
	cd Shape && $(MAKE) static

.PHONY: clean Common Geometry MeshDB MeshGen MeshGL RevocapIO Shape
clean:
	cd Common && $(MAKE) clean
	cd Matrix && $(MAKE) clean
	cd Geometry && $(MAKE) clean
	cd MeshDB && $(MAKE) clean
	cd MeshGen && $(MAKE) clean
	@if [ -d MeshGL ]; then cd MeshGL && $(MAKE) clean; fi;
	@if [ -d Refiner ]; then cd Refiner && $(MAKE) clean; fi;
	cd RevocapIO && $(MAKE) clean
	cd Shape && $(MAKE) clean
	@if [ -d test ]; then cd test && $(MAKE) clean; fi;

$(LIBDIR)/$(ARCH):
	@if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi;
	@if [ ! -d $(LIBDIR)/$(ARCH) ]; then mkdir $(LIBDIR)/$(ARCH); fi;

cpp2cc:
	for fname in */*.cpp; do mv $fname ${fname%.cpp}.cc; done

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

doc: force
	$(DOXYGEN) Documents/Doxyfile

test: force
	cd Common $$ $(MAKE) static
	cd Matrix && $(MAKE) static
	cd Geometry && $(MAKE) static
	cd MeshDB && $(MAKE) static
	cd MeshGen && $(MAKE) static
	cd RevocapIO && $(MAKE) static
	cd Shape && $(MAKE) static
	cd test && $(MAKE) main

force: ;

arch:
	@echo $(ARCH)
