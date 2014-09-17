include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

set(bbPlugin+_RESOURCES bbLeanBar.rc)
set(bbPlugin+_SOURCES
	bbPlugin+.cpp
	drawico+.cpp
	moreutils+.cpp
	sysmenu+.cpp
	tooltips+.cpp
)

SET(bbPlugin+_HEADERS
	bbPlugin+.h
	drawico+.h
	tooltips+.h
)

add_library(bbPlugin+ STATIC ${bbPlugin+_SOURCES} ${bbPlugin+_HEADERS})
target_link_libraries(bbPlugin+ blackbox bblib)

set_property(TARGET bbPlugin+ PROPERTY FOLDER "plugins/bbPlugin+")

