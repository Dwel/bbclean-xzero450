cmake_minimum_required (VERSION 2.8)
project (logging)

if(USE_LOGGING EQUAL 1)
	find_package (Flogging REQUIRED)
	find_package (Threads)

	message ("Current src dir: ${CMAKE_CURRENT_SOURCE_DIR}")
	message ("Flogging includes: ${FLOGGING_INCLUDE_DIRS}")
	message ("Flogging srcs: ${FLOGGING_SOURCE_DIRS}")

	#include_directories (${FLOGGING_INCLUDE_DIRS})

else(USE_LOGGING EQUAL 1)
	message ("no logging enabled, using dummy.")
	find_package (Flogging REQUIRED)
	#include_directories (${FLOGGING_INCLUDE_DIRS})
endif(USE_LOGGING EQUAL 1)

add_library (logging STATIC "${FLOGGING_SOURCE_DIRS}/trace_client/trace.cpp")

#set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
#install (TARGETS logging LIBRARY DESTINATION .  ARCHIVE DESTINATION .)


