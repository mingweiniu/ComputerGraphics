cmake_minimum_required(VERSION 3.5)
# C++14 is required
set(CMAKE_CXX_STANDARD 14)

set(PRROJ_NAME "opencv_hw1")
set(My_LIB "hw1")

project (${PRROJ_NAME})


include_directories("${CMAKE_SOURCE_DIR}/include")


set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE My_LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
#file(GLOB_RECURSE My_LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/hw1/*.cpp")
file(GLOB_RECURSE My_HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
set(HEADERS ${HEADERS} ${My_HEADERS}) 
set(SOURCES ${SOURCES} main.cpp ${HEADERS}) 
set(LIB_SOURCES ${LIB_SOURCES})
add_executable(${PRROJ_NAME} ${SOURCES})



include(${CMAKE_SOURCE_DIR}/cmake/getBoost.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/getOpenCV.cmake)


message("include_directories(${PROJ_INCLUDE_DIRS})")
include_directories(${PROJ_INCLUDE_DIRS})

message("add_subdirectory(src/${My_LIB})")
add_subdirectory("src/${My_LIB}")
add_library("lib_${My_LIB}" ${My_LIB_SOURCES} ) 
set(PROJ_LIBRARIES ${PROJ_LIBRARIES} "lib_${My_LIB}")


message("target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})")
target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})

set(MY_CPP_FLAGS ${MY_CPP_FLAGS})
if(UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CPP_FLAGS} -I/usr/local/include -lboost_system -lboost_filesystem -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib") 
endif()
