add_definitions(-D_POSIX_C_SOURCE=200809)
add_library(common
	common.c
	common.h
	profiler.c
	profiler.h)
