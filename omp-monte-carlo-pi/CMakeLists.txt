project(OmpMonteCarloPi C)

find_package(C99 REQUIRED)
find_package(OpenMP REQUIRED)

add_definitions(${C99_C_FLAGS})
include_directories("${CMAKE_SOURCE_DIR}/common")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

add_executable(omp-monte-carlo-pi main.c)
target_link_libraries(omp-monte-carlo-pi rt common)
