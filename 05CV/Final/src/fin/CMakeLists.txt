

file(GLOB_RECURSE My_LIB_SOURCES "*.cpp")

set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include")
file(GLOB_RECURSE My_LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE My_HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
set(HEADERS ${HEADERS} ${My_HEADERS}) 

set(SOURCES ${SOURCES} main.cpp ${HEADERS}) 
set(LIB_SOURCES ${LIB_SOURCES})
add_executable(${PRROJ_NAME} ${SOURCES})

message("include_directories(${PROJ_INCLUDE_DIRS})")
include_directories(${PROJ_INCLUDE_DIRS})

add_library("lib_${My_LIB}" ${My_LIB_SOURCES} ) 
set(PROJ_LIBRARIES ${PROJ_LIBRARIES} "lib_${My_LIB}")

message("target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})")
target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})