project(PthTrapezoidal C)

set(CMAKE_THREAD_PREFER_PTHREAD True)

find_package(C99 REQUIRED)
find_package(Threads REQUIRED)

if (!${CMAKE_USE_PTHREADS_INIT})
  message(FATAL_ERROR "Could not find a valid pthreads library")
endif()

include_directories("${CMAKE_SOURCE_DIR}/common")
add_definitions(${C99_C_FLAGS})

add_executable(pth-trapezoidal main.c)
target_link_libraries(pth-trapezoidal m rt common ${CMAKE_THREAD_LIBS_INIT})
