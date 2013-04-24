cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(C99 REQUIRED)
find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/common")
add_definitions(${C99_C_FLAGS} -D_XOPEN_SOURCE=500)

add_subdirectory(common)
add_subdirectory(greeting)
add_subdirectory(histogram)
add_subdirectory(sumtree)
add_subdirectory(monte-carlo-pi)
add_subdirectory(omp-histogram)
