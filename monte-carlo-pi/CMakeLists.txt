project(MonteCarloPi C)
add_executable(monte-carlo-pi main.c)
include_directories(${MPI_C_INCLUDE_PATH})
target_link_libraries(monte-carlo-pi rt common ${MPI_C_LIBRARIES})

