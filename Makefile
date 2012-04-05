#CXX 		:= g++
#CXX_FLAGS 	:= -Wall -g -std=c++0x
#CXX_FLAGS := -std=c++0x -O3 -DNDEBUG -march=core2
#LIBS		:= -lboost_program_options -lboost_system
SRCS			:= AgentList.cpp Process.cpp
EXE_SRCS		:= process_list.cc
OBJS 			:= $(SRCS:.cpp=.o)
#INCLUDE_PATHS := -I ~/workspace/Debug/usr/include
#VPATH := ../oldmodels
INSTALL_DIR := ../../bin

all: process_list

process_list: process_list.o ${OBJS}
	${CXX} ${CXX_FLAGS} -o $@ ${OBJS} process_list.o
	mv $@ ${INSTALL_DIR}

#generate dependancy files at the same time as object compilation
%.o : %.cpp
	$(CC) $(CXX_FLAGS) $(INCLUDE_PATHS) -MMD -o $@ -c $< 
	@cp $*.d $*.P; \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	 	  -e '/^$$/ d' -e 's/$$/ :/' < $*.P >> $*.d; \
	  rm -f $*.P

-include $(SRCS:.cpp=.d)
-include $(EXE_SRCS:.cc=.d)

clean:
	-rm *.o *.d *~
