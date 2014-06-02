cmake_minimum_required (VERSION 2.8)
project (logging)

if(USE_LOGGING EQUAL 1)
  message ("Logging enabled, using DbgToolkit...")
	find_package (DbgToolkit REQUIRED)
	find_package (Threads)

	message ("\tCurrent src dir: ${CMAKE_CURRENT_SOURCE_DIR}")
	message ("\tDbgToolkit includes: ${DBGTOOLKIT_INCLUDE_DIRS}")
	message ("\tDbgToolkit srcs: ${DBGTOOLKIT_SOURCE_DIRS}")

  add_library (logging STATIC "${DBGTOOLKIT_SOURCE_DIRS}/trace_client/trace.cpp")

else(USE_LOGGING EQUAL 1)
  message ("Logging NOT enabled, using dummy.")
  add_library (logging STATIC dummy.cpp)
endif(USE_LOGGING EQUAL 1)

set_property(TARGET logging PROPERTY FOLDER "3rd/logging")

