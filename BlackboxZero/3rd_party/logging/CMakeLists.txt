cmake_minimum_required (VERSION 2.8)
project (logging)

if(USE_LOGGING EQUAL 1)
  message ("Logging enabled, using Flogging...")
	find_package (Flogging REQUIRED)
	find_package (Threads)

	message ("\tCurrent src dir: ${CMAKE_CURRENT_SOURCE_DIR}")
	message ("\tFlogging includes: ${FLOGGING_INCLUDE_DIRS}")
	message ("\tFlogging srcs: ${FLOGGING_SOURCE_DIRS}")

  add_library (logging STATIC "${FLOGGING_SOURCE_DIRS}/trace_client/trace.cpp")

else(USE_LOGGING EQUAL 1)
  message ("Logging NOT enabled, using dummy.")
  add_library (logging STATIC dummy.cpp)
endif(USE_LOGGING EQUAL 1)

set_property(TARGET logging PROPERTY FOLDER "3rd/logging")

