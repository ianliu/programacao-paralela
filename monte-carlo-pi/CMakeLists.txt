project(MonteCarloPi C)

find_package(C99 REQUIRED)
find_package(MPI REQUIRED)

add_definitions(${C99_C_FLAGS})
include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories(${MPI_C_INCLUDE_PATH})

add_executable(monte-carlo-pi main.c)
target_link_libraries(monte-carlo-pi rt common ${MPI_C_LIBRARIES})

