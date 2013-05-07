project(SumTree C)
add_definitions(-D_XOPEN_SOURCE)
add_executable(sumtree main.c)
include_directories(${MPI_C_INCLUDE_PATH})
target_link_libraries(sumtree rt common ${MPI_C_LIBRARIES})

