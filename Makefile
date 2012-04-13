CXX 		:= g++
CXX_FLAGS 	:= -Wall -g -std=c++11
#CXX_FLAGS := -std=c++0x -O3 -DNDEBUG -march=core2
LIBS		:= -lboost_program_options -lboost_system
SRCS			:= AgentList.cpp Process.cpp ProcessList.cpp utils.cpp
EXE_SRCS		:= process_list.cc
OBJS 			:= $(SRCS:.cpp=.o)
#INCLUDE_PATHS := -I ~/workspace/Debug/usr/include
#VPATH := ../oldmodels
INSTALL_DIR := ../../bin

all: process_test pipeline_test process_list

process_list: process_list.o ${OBJS}
	$(CXX) $(CXX_FLAGS) -o $@ ${OBJS} process_list.o
	mv $@ ${INSTALL_DIR}

process_test: process_test.o ${OBJS}
	${CXX} $(CXX_FLAGS) -o $@ ${OBJS} process_test.o
	mv $@ ${INSTALL_DIR}

pipeline_test: pipeline_test.o Pipeline.o utils.o
	${CXX} $(CXX_FLAGS) -o $@ Pipeline.o utils.o pipeline_test.o
	mv $@ ${INSTALL_DIR}

include ../make_depend.mk

-include $(SRCS:.cpp=.d)
-include $(EXE_SRCS:.cc=.d)

clean:
	-rm *.o *.d *~
