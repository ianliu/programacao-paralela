project(Greeting C)
add_executable(greeting main.c)
include_directories(${MPI_C_INCLUDE_PATH})
target_link_libraries(greeting common ${MPI_C_LIBRARIES})

