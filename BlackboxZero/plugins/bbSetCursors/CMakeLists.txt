set(bbSetCursors_SOURCES
  bbplugin.cpp
  configfile.cpp
  registry.cpp
  stringutils.cpp
  updatecursors.cpp
)

set(SetCursors_SOURCES
  main.cpp
  configfile.cpp
  registry.cpp
  stringutils.cpp
  updatecursors.cpp
)

#set(bbSetCursors_CONFIGS bbSetCursors.rc icons.rc)

#set(bbSetCursors_RESOURCES resource.rc)
#set(CMAKE_RC_COMPILER_INIT windres)
#ENABLE_LANGUAGE(RC)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

add_library(bbSetCursors SHARED ${bbSetCursors_SOURCES} ${bbSetCursors_RESOURCES})
set_property(TARGET bbSetCursors PROPERTY FOLDER "plugins/bbSetCursors")
target_link_libraries(bbSetCursors blackbox bblib bbPlugin)
target_link_libraries(bbSetCursors version comctl32)

add_executable(SetCursors WIN32 ${SetCursors_SOURCES} ${SetCursors_RESOURCES})
set_target_properties(SetCursors PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
target_link_libraries(SetCursors blackbox bblib)
target_link_libraries(SetCursors version comctl32)
set_property(TARGET SetCursors PROPERTY FOLDER "plugins/bbSetCursors")

install(TARGETS bbSetCursors RUNTIME DESTINATION plugins/bbSetCursors)
install(TARGETS SetCursors RUNTIME DESTINATION plugins/bbSetCursors)
#if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbSetCursors/${bbSetCursors_CONFIGS}")
#	install(FILES ${bbSetCursors_CONFIGS} DESTINATION plugins/bbSetCursors)
#endif()

