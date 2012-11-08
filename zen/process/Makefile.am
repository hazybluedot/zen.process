lib_LTLIBRARIES = libzen_process.la
#libzen_process_la_CPPFLAGS = $(JSONCPP_CFLAGS)
#libzen_process_la_LDFLAGS = $(JSONCPP_LDFLAGS)
libzen_process_la_LIBADD = $(JSONCPP_LIBS) 
libzen_process_la_SOURCES = Pipeline.cpp SelfPipeTrickExec.cpp utils.cpp Pipe.cpp PipelineIO.cpp ProcessManager.cpp
libzen_process_includedir=$(includedir)/zen_process-0.9/zen_process
libzen_process_include_HEADERS = Pipeline.hpp Pipe.hpp types.hpp utils.hpp SelfPipeTrickExec.hpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ../../zen_process-0.9.pc
