
message("boost setting multithread/static_libs/!static_runtime")
set(Boost_DEBUG ON) # Set to ON to enable debug output from FindBoost.
set(Boost_DETAILED_FAILURE_MSG ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)


set(NEED_BOOST_COMPONENTS filesystem system)

if(WIN32)
	add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
	find_path(BOOST_ROOT NAMES boost/version.hpp HINTS
		"$ENV{BOOST_ROOT}"
		"$ENV{BOOSTROOT}"
	)
endif()

message("find_package(Boost)")
find_package(Boost COMPONENTS ${NEED_BOOST_COMPONENTS} REQUIRED)
message("BOOST_ROOT is ${BOOST_ROOT}")

message("Boost_FOUND is ${Boost_FOUND}")
if(Boost_FOUND)
	#add_definitions(-DBOOST_ALL_NO_LIB)
	set(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
	set(PROJ_LIBRARIES ${PROJ_LIBRARIES} ${Boost_LIBRARIES})
endif()



set(BOOST_FLAGS "-DNDEBUG -DBOOST_UBLAS_NDEBUG")

set(MY_CPP_FLAGS "${MY_CPP_FLAGS} ${BOOST_FLAGS}")

