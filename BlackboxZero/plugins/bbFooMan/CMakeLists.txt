project(bbFooMan)
set(bbFooMan_SOURCES
  #bbf_settings.cpp #@NOTE: this file is included from inside bbfooman.cpp
  bbfooman.cpp
)

set(BBFooMan_CONFIGS bbfooman.rc)

set(bbFooMan_RESOURCES bbFoomanInt.rc)
set(CMAKE_RC_COMPILER_INIT windres)
ENABLE_LANGUAGE(RC)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbFooMan SHARED ${bbFooMan_SOURCES})
target_link_libraries(bbFooMan blackbox bblib bbPlugin)
target_link_libraries(bbFooMan version comctl32 gdiplus winmm msimg32)

set_property(TARGET bbFooMan PROPERTY FOLDER "plugins/bbFooMan")

install(TARGETS bbFooMan RUNTIME DESTINATION plugins/bbFooMan)
if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbFooMan/${bbFooMan_RESOURCES}")
	install(FILES ${bbFooMan_RESOURCES} DESTINATION plugins/bbFooMan)
endif()

