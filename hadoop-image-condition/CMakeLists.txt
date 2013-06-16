project(HadoopImageCondition Java)

find_package(Java 1.6 COMPONENTS Development)
include(UseJava)

set(CMAKE_JAVA_INCLUDE_PATH hadoop-core-1.0.4.jar)
add_jar(wc WordCount.java)
add_jar(img-cond ImageCondition.java)

add_subdirectory(doc)
