project(OmpCountSort C)
add_definitions(-D_XOPEN_SOURCE=500)
add_executable(omp-count-sort main.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
target_link_libraries(omp-count-sort rt common m)

