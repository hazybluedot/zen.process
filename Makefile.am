ACLOCAL_AMFLAGS = -I m4

#===================================
# libzen_process : process library

lib_LTLIBRARIES = zen/process/libzen_process.la

zen_process_libzen_process_la_LIBADD = $(JSONCPP_LIBS) 
zen_process_libzen_process_la_CPPFLAGS= $(JSONCPP_CFLAGS)
zen_process_libzen_process_la_SOURCES = zen/process/Pipeline.cpp zen/process/SelfPipeTrickExec.cpp zen/process/utils.cpp zen/process/Pipe.cpp
zen_process_libzen_process_includedir=$(includedir)/zen_process-0.9/zen_process
zen_process_libzen_process_include_HEADERS = zen/process/Pipeline.hpp zen/process/Pipe.hpp zen/process/types.hpp zen/process/utils.hpp zen/process/SelfPipeTrickExec.hpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = zen_process-0.9.pc

#===================================
# test programs

noinst_PROGRAMS = test/pipeline test/dummy

test_pipeline_SOURCES = test/pipeline.cpp
test_pipeline_CPPFLAGS = $(JSONCPP_CFLAGS) -Izen/process
test_pipeline_LDADD = zen/process/libzen_process.la 

test_dummy_SOURCES = test/dummy.cpp
