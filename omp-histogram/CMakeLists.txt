project(OmpHistogram C)
add_executable(omp-histogram main.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
target_link_libraries(omp-histogram rt common m)

