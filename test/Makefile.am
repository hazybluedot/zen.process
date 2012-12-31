AM_CPPFLAGS = -I$(top_srcdir)

noinst_PROGRAMS = test-pipeline dummy

test_pipeline_SOURCES = test-pipeline.cpp
test_pipeline_CPPFLAGS = $(JSONCPP_CFLAGS) -I../zen/process
test_pipeline_LDADD = ../zen/process/libzen_process.la 

dummy_SOURCES = dummy.cpp
