AM_CPPFLAGS = -I$(top_srcdir)

noinst_PROGRAMS = test-pipeline test-procman dummy

test_pipeline_SOURCES = test-pipeline.cpp
test_pipeline_LDADD = ../zen/process/libzen_process.la 

test_procman_SOURCES = test-procman.cpp
test_procman_LDADD = ../zen/process/libzen_process.la

dummy_SOURCES = dummy.cpp
