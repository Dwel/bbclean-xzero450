project(bbMuse)
set(bbMuse_SOURCES
  #bbf_settings.cpp #@NOTE: this file is included from inside bbMuse.cpp
  BBmuse.cpp
)

set(bbMuse_CONFIGS BBmuse.rc)

set(bbMuse_RESOURCES BBmusePlayer.rc)
set(CMAKE_RC_COMPILER_INIT windres)
ENABLE_LANGUAGE(RC)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbMuse SHARED ${bbMuse_SOURCES})
target_link_libraries(bbMuse blackbox bblib bbPlugin)
target_link_libraries(bbMuse version comctl32 gdiplus winmm msimg32)

set_property(TARGET bbMuse PROPERTY FOLDER "plugins/bbMuse")

install(TARGETS bbMuse RUNTIME DESTINATION plugins/bbMuse)
if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbMuse/${bbMuse_CONFIGS}")
	install(FILES ${bbMuse_CONFIGS} DESTINATION plugins/bbMuse)
endif()

