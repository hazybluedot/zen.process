lib_LTLIBRARIES = libzen_process.la
libzen_process_la_CPPFLAGS = $(JSON_CFLAGS)
libzen_process_la_LDFLAGS = $(JSON_LDFLAGS)
libzen_process_la_LIBADD = $(JSON_LIBS) 
libzen_process_la_SOURCES = Pipeline.cpp SelfPipeTrickExec.cpp utils.cpp Pipe.cpp PipelineIO.cpp ProcessManager.cpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ../../zen_process-0.9.pc
