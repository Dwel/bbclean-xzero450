project(bbSeekBar)

add_subdirectory(foo_seek_remote)
add_subdirectory(foobar2000)
add_subdirectory(winamp)

set(bbSeekBar_SOURCES
  bbseekbar.cpp
)

set(bbSeekBar_CONFIGS PlayerTime.rc)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbSeekBar SHARED ${bbSeekBar_SOURCES})
target_link_libraries(bbSeekBar blackbox bblib bbPlugin)
target_link_libraries(bbSeekBar version comctl32 gdiplus winmm msimg32)

set_property(TARGET bbSeekBar PROPERTY FOLDER "plugins/bbSeekBar")

install(TARGETS bbSeekBar RUNTIME DESTINATION plugins/bbSeekBar)
if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbSeekBar/${bbSeekBar_RESOURCES}")
	install(FILES ${bbSeekBar_RESOURCES} DESTINATION plugins/bbSeekBar)
endif()

