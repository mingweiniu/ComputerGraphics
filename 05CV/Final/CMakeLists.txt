cmake_minimum_required (VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

set(PRROJ_NAME fin)
project (${PRROJ_NAME})

# user define Find_packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

#finding dependencies
include(${CMAKE_SOURCE_DIR}/cmake/getGLFW.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/getGLEW.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/getGLM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/getOpenCV.cmake)


set(MY_CPP_FLAGS ${MY_CPP_FLAGS})
if(UNIX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CPP_FLAGS} -I/usr/local/include -Wall")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib")
endif()



set(My_LIB "${PRROJ_NAME}")
message("add_subdirectory(src/${My_LIB})")
add_subdirectory("src/${My_LIB}")