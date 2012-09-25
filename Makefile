CXX 		:= g++
CXX_FLAGS 	:= -Wall -g -std=c++11
#CXX_FLAGS := -std=c++0x -O3 -DNDEBUG -march=core2
LIBS		:= -lboost_program_options -lboost_system -lboost_filesystem -ljsoncpp
SRCS			:= Pipeline.cpp utils.cpp selfpipetrick.cpp
EXE_SRCS		:= process_list.cc
OBJS 			:= $(SRCS:.cpp=.o)
#INCLUDE_PATHS := -I ~/workspace/Debug/usr/include
#VPATH := ../oldmodels
BINDIR := ../../bin/

all: process_test pipeline_test process_list

process_list: process_list.o ${OBJS}
	$(CXX) $(CXX_FLAGS) -o ${BINDIR}$@ ${OBJS} process_list.o ${LIBS}

process_test: process_test.o ${OBJS}
	${CXX} $(CXX_FLAGS) -o ${BINDIR}$@ ${OBJS} process_test.o ${LIBS}

pipeline_test: pipeline_test.o Pipeline.o utils.o selfpipetrick.o
	${CXX} $(CXX_FLAGS) -o ${BINDIR}$@ Pipeline.o utils.o pipeline_test.o selfpipetrick.o ${LIBS}

include ../make_depend.mk

-include $(SRCS:.cpp=.d)
-include $(EXE_SRCS:.cc=.d)

clean:
	-rm *.o *.d *~
