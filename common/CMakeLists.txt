find_package(C99 REQUIRED)

add_definitions(${C99_C_FLAGS})

add_library(common
	common.c
	common.h
	profiler.c
	profiler.h)
