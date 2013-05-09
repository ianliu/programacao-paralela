project(OmpCountSort C)

find_package(C99 REQUIRED)
find_package(OpenMP REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/common")
add_definitions(${C99_C_FLAGS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

add_executable(omp-count-sort main.c)
target_link_libraries(omp-count-sort rt common m)

########################################################################
#                                 TESTS
########################################################################
add_test(OmpCountSort       echo "boya")
add_test(OmpCountSort-qsort echo "boya")
