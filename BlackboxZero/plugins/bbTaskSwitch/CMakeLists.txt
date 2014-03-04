set(bbTaskSwitch_SOURCES
  BBTaskSwitch.cpp
  cpp_adaptor.cpp
  misc.cpp
  mod_nix.cpp
)

#set(bbTaskSwitch_CONFIGS bbTaskSwitch.rc)
#set(bbTaskSwitch_RESOURCES resource.rc)
#set(CMAKE_RC_COMPILER_INIT windres)
#ENABLE_LANGUAGE(RC)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbTaskSwitch SHARED ${bbTaskSwitch_SOURCES} ${bbTaskSwitch_RESOURCES})
target_link_libraries(bbTaskSwitch blackbox bblib bbPlugin)
target_link_libraries(bbTaskSwitch version comctl32)

set_property(TARGET bbTaskSwitch PROPERTY FOLDER "plugins/bbTaskSwitch")

install(TARGETS bbTaskSwitch RUNTIME DESTINATION plugins/bbTaskSwitch)
#if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbTaskSwitch/${bbTaskSwitch_CONFIGS}")
#	install(FILES ${bbTaskSwitch_CONFIGS} DESTINATION plugins/bbTaskSwitch)
#endif()

