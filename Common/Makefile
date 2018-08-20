include ../OPTIONS
include ../MakefileConfig.in

LIBDIR = ../lib
STATIC_LIB = libRcapCommon.a

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: clean static

static: $(LIBDIR)/$(ARCH)/$(STATIC_LIB)

$(LIBDIR)/$(ARCH)/$(STATIC_LIB) : $(OBJS)
	@if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi;
	@if [ ! -d $(LIBDIR)/$(ARCH) ]; then mkdir $(LIBDIR)/$(ARCH); fi;
	$(AR) $(ARFLAGS) $(LIBDIR)/$(ARCH)/$(STATIC_LIB) $(OBJS)

clean:
	@-$(RM) *.o
	@-$(RM) $(LIBDIR)/$(ARCH)/$(STATIC_LIB)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -I.. $<

