project(SumTree C)
add_executable(sumtree main.c)
include_directories(${MPI_C_INCLUDE_PATH})
target_link_libraries(sumtree common ${MPI_C_LIBRARIES})

