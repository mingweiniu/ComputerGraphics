cmake_minimum_required (VERSION 3.5)

set(PRROJ_NAME NPR_hw1) 
project (${PRROJ_NAME})

set(HEADERS ${HEADERS} OpenGLWindow.hpp TriangleWindow.hpp ) 
set(LIB_SOURCES ${LIB_SOURCES} OpenGLWindow.cpp TriangleWindow.cpp)
set(SOURCES ${SOURCES} main.cpp ${HEADERS}) 
set(CMAKE_AUTOMOC ON)

add_executable(${PRROJ_NAME} ${SOURCES})
set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})

set(QTGL_LIB "polygon")
add_library(${QTGL_LIB} ${LIB_SOURCES}) 
set(PROJ_LIBRARIES ${PROJ_LIBRARIES} ${QTGL_LIB})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(WIN32)
    find_path(CMAKE_PREFIX_PATH NAMES lib/cmake/Qt5/Qt5Config.cmake HINTS
        "$ENV{Qt5_ROOT}"
    )
    message("Qt5_ROOT is $ENV{Qt5_ROOT}") 
endif() 
message("CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

set(QT_FLAGS "-fPIC")
set(NEED_QT_COMPONENTS QTCORE QTGUI)
message("finding ${NEED_QT_COMPONENTS}")
foreach(COMPONENT Gui Core)
    message("find_package(Qt5${COMPONENT})")
    find_package("Qt5${COMPONENT}")
    message("Qt5${COMPONENT}_FOUND is ${Qt5${COMPONENT}_FOUND}")
    if("${Qt5${COMPONENT}_FOUND}")
        set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} "${Qt5${COMPONENT}_INCLUDE_DIRS}")
        set(PROJ_LIBRARIES ${PROJ_LIBRARIES} "Qt5::${COMPONENT}")
        #Open add_definitions  will error
        #add_definitions("${Qt5${COMPONENT}_DEFINITIONS}")
        qt5_use_modules(${PRROJ_NAME} ${COMPONENT})
    endif() 
endforeach(COMPONENT )

message("include_directories(${PROJ_INCLUDE_DIRS})")
include_directories(${PROJ_INCLUDE_DIRS})
message("target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})")
target_link_libraries(${PRROJ_NAME} ${PROJ_LIBRARIES})
set(CMAKE_CXX_STANDARD 14)

if(UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_FLAGS} -I/usr/local/include -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib") 
endif()
