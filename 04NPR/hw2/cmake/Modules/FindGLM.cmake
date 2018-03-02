
set(TARGET GLM)

if(WIN32)

find_path(${TARGET}_DIR NAMES include/GLFW/glfw3.h HINTS
	"$ENV{${TARGET}_DIR}"
	"$ENV{${TARGET}_ROOT}"
)

find_path(${TARGET}_INCLUDE_DIR NAMES glm/glm.hpp HINTS ${${TARGET}_DIR}/include)
set(${TARGET}_INCLUDE_DIRS ${${TARGET}_INCLUDE_DIRS} ${${TARGET}_INCLUDE_DIR})

endif()


if(${TARGET}_INCLUDE_DIRS)
	set(${TARGET}_FOUND 1)
endif() 
