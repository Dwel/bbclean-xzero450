cmake_minimum_required (VERSION 2.8)
project (logging)

if(USE_LOGGING EQUAL 1)
	find_package (Flogging REQUIRED)
	find_package (Threads)

	message ("Current src dir: ${CMAKE_CURRENT_SOURCE_DIR}")
	message ("Flogging includes: ${FLOGGING_INCLUDE_DIRS}")
	message ("Flogging srcs: ${FLOGGING_SOURCE_DIRS}")

  add_library (logging STATIC "${FLOGGING_SOURCE_DIRS}/trace_client/trace.cpp")
	#include_directories (${FLOGGING_INCLUDE_DIRS})
  #set_property(TARGET trace_client PROPERTY FOLDER "3rd/logging")

else(USE_LOGGING EQUAL 1)
	message ("no logging enabled, using dummy.")
  #find_package (Flogging REQUIRED)
	#include_directories (${FLOGGING_INCLUDE_DIRS})
  add_library (logging STATIC dummy.cpp)
endif(USE_LOGGING EQUAL 1)

set_property(TARGET logging PROPERTY FOLDER "3rd/logging")

#set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
#install (TARGETS logging LIBRARY DESTINATION .  ARCHIVE DESTINATION .)


