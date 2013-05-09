project(SumTree C)

find_package(C99 REQUIRED)
find_package(MPI REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/common")
add_definitions(${C99_C_FLAGS})
include_directories(${MPI_C_INCLUDE_PATH})

add_executable(sumtree main.c)
target_link_libraries(sumtree rt common ${MPI_C_LIBRARIES})
