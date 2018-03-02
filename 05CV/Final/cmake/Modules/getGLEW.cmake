
# finding glew



if(WIN32)
    find_path(GLEW_DIR NAMES include/GL/glew.h HINTS
        "$ENV{GLEW_DIR}"
    )
    message("GLEW_DIR is $ENV{GLEW_DIR}") 

    set(GLEW_INCLUDE_DIR ${GLEW_DIR}/include)
    find_library(GLEW_LIBRARY NAMES glew32s 
    	HINTS 
    	${GLEW_DIR}/lib/Release/x64
    )

endif() 





set(TARGET GLEW)
find_package(${TARGET})
message("${TARGET}_FOUND is ${${TARGET}_FOUND}")
if(GLEW_FOUND)
	set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
	set(PROJ_LIBRARIES ${PROJ_LIBRARIES} ${GLEW_LIBRARIES})
endif()