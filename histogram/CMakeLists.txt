project(Histogram C)
add_definitions(-D_XOPEN_SOURCE)
add_executable(histogram main.c)
include_directories(${MPI_C_INCLUDE_PATH})
target_link_libraries(histogram ${MPI_C_LIBRARIES})

