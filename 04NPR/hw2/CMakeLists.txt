cmake_minimum_required (VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

set(PRROJ_NAME NPR_hw2)
project (${PRROJ_NAME})
set(HEADERS ${HEADERS} ${My_HEADERS}) 
set(SOURCES ${SOURCES} main.cpp ${HEADERS}) 
add_executable(${PRROJ_NAME} ${SOURCES})

include_directories("${CMAKE_SOURCE_DIR}/include")

# user define Find_packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

#finding dependencies
include(${CMAKE_SOURCE_DIR}/cmake/getGLFW.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/getGLEW.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/getGLM.cmake)


message("include_directories(${PROJ_INCLUDE_DIRS})")
include_directories(${PROJ_INCLUDE_DIRS})
message("target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})")
target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})

set(MY_CPP_FLAGS ${MY_CPP_FLAGS})
if(UNIX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CPP_FLAGS} -I/usr/local/include -Wall")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib")
endif()
