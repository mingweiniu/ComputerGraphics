

# if windows
# you need add the directory of opencv to environment variable, and log out
# for example $ENV:OpenCV_DIR is "C:\\opencv\\build" 
# and this path has such "OpenCVConfig-version.cmake" files

if(WIN32)
	find_path(OpenCV_DIR OpenCVConfig.cmake HINTS
		"$ENV{CV_ROOT}"
		"$ENV{OpenCV_ROOT}"
		"$ENV{OpenCV_DIR}"
	)
	message("OpenCV_DIR is ${OpenCV_DIR}") 
endif() 


find_package(OpenCV 3.1 REQUIRED)
message("OpenCV_FOUND is ${OpenCV_FOUND}") 
if(OpenCV_FOUND)
	set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
	set(PROJ_LIBRARIES ${PROJ_LIBRARIES} ${OpenCV_LIBS})
endif() 