CXX 		:= g++
ifndef CXX_FLAGS
   CXX_FLAGS 	:= -Wall -ggdb -std=c++11
endif
CXX_FLAGS := ${CXX_FLAGS} -fPIC
LIBS		:= -lboost_program_options -lboost_system -lboost_filesystem -ljsoncpp
SRCS		:= Pipeline.cpp utils.cpp selfpipetrick.cpp
EXE_SRCS	:= process_list.cc
OBJS 		:= $(SRCS:.cpp=.o)
#INCLUDE_PATHS := -I ~/workspace/Debug/usr/include
BINDIR := ../../bin/
LIBDIR := ../../lib/

define compile_rule
        libtool --mode=compile \
        $(CC) $(CFLAGS) $(CPPFLAGS) -c $<
endef
define link_rule
        libtool --mode=link \
        $(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
endef

LIBS = libprocess.la
libprocess_OBJS = Pipeline.lo utils.lo selfpipetrick.lo

all: libzen_process.so process_test pipeline_test process_list

#%.lo: %.cpp
#	$(call compile_rule)

libzen_process.so: ${OBJS}
	$(CXX) $(CXX_FLAGS) --shared -o ${LIBDIR}$@ ${OBJS}

#libprocess.la: $(libmystuff_OBJS)
#	$(call link_rule)

process_list: process_list.o ${OBJS}
	$(CXX) $(CXX_FLAGS) -o ${BINDIR}$@ ${OBJS} process_list.o ${LIBS}

process_test: process_test.o ${OBJS}
	${CXX} $(CXX_FLAGS) -o ${BINDIR}$@ ${OBJS} process_test.o ${LIBS}

pipeline_test: pipeline_test.o Pipeline.o utils.o selfpipetrick.o
	${CXX} $(CXX_FLAGS) -o ${BINDIR}$@ Pipeline.o utils.o pipeline_test.o selfpipetrick.o ${LIBS}

#install/%.la: %.la
#	libtool --mode=install \
#	install -c $(notdir $@) $(LIBDIR)/$(notdir $@)
#install: $(addprefix install/,$(LIBS))
#	libtool --mode=finish $(libdir)install/%.la: %.la
#	libtool --mode=install \
#	install -c $(notdir $@) $(libdir)/$(notdir $@)
#install: $(addprefix install/,$(LIBS))
#	libtool --mode=finish $(LIBDIR)

include ../make_depend.mk

-include $(SRCS:.cpp=.d)
-include $(EXE_SRCS:.cc=.d)

clean:
	-rm *.o *.d *~
