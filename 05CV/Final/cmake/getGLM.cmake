


if(WIN32)
	find_path(GLM_DIR NAMES lib/cmake/glm/glmConfig.cmake HINTS
		"$ENV{GLM_DIR}"
	)
	message("GLM_DIR is $ENV{GLM_DIR}") 
endif() 

find_package(GLM)

message("GLM_FOUND is ${GLM_FOUND}")
if(GLM_FOUND)
	set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
	message("set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})")
endif()
