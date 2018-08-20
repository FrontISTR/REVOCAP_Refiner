include ../OPTIONS
include ../MakefileConfig.in

LIBDIR = ../lib
INCFLAGS = -I..
STATIC_LIB = libRcapMatrix.a
# CFLAGS += -DREVO_USE_LAPACK -I$(CLAPACKDIR)/include
# CFLAGS += -DREVO_USE_ATLAS
# CFLAGS += -DREVO_USE_CUDA

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

static: $(LIBDIR)/$(ARCH)/$(STATIC_LIB)

$(LIBDIR)/$(ARCH)/$(STATIC_LIB) : $(OBJS)
	@if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi;
	@if [ ! -d $(LIBDIR)/$(ARCH) ]; then mkdir $(LIBDIR)/$(ARCH); fi;
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) $(OBJS)

.PHONY: clean
clean:
	@-$(RM) *.o
	@-$(RM) $(LIBDIR)/$(ARCH)/$(STATIC_LIB)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $(INCFLAGS) $<
