set(BBLIB2_STATIC true)

add_definitions(-DUNICODE -D_UNICODE)

set(Boost_DEBUG 0)
set(Boost_USE_STATIC_LIBS 1)
find_package(Boost)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions( "-DHAS_BOOST" )

  #set(BOOST_LIBS thread date_time system regex serialization)
	find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
endif()
message("boost: ${Boost_LIBRARIES}")

set(bblib2_SOURCES
  winutils.cpp
  colors.cpp
)

set(bblib2_HEADERS
	bblib2.h
  winutils.h
  colors.h
)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
add_definitions(-DBBLIB_COMPILING)
#NO_BBLIB = 1

if (BBLIB2_STATIC)
	add_definitions(-DBBLIB2_STATIC)
	add_library(bblib2 STATIC ${bblib2_SOURCES})
elseif (NOT BBLIB2_STATIC)
	add_library(bblib2 SHARED ${bblib2_SOURCES})
endif (BBLIB2_STATIC)

target_link_libraries(bblib2 version comctl32)
target_link_libraries(bblib2 logging)
