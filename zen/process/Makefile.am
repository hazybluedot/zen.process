lib_LTLIBRARIES = libzen_process.la
libzen_process_la_CPPFLAGS = $(JSON_CFLAGS) $(BOOST_CPPFLAGS)
libzen_process_la_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS)
libzen_process_la_LIBADD = $(JSON_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_FILESYSTEM_LIBS)
libzen_process_la_SOURCES = Pipeline.cpp SelfPipeTrickExec.cpp utils.cpp Pipe.cpp PipelineIO.cpp ProcessManager.cpp
