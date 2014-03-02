set(bbDesktop_SOURCES
  bbPlugin.cpp
  configfile.cpp
  DrawIcons.cpp
  stringutils.cpp
    #<Unit filename="Shortcuts.cpp" />
)

set(bbDesktop_CONFIGS bbDesktop.rc icons.rc)

#set(bbDesktop_RESOURCES resource.rc)
#set(CMAKE_RC_COMPILER_INIT windres)
#ENABLE_LANGUAGE(RC)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbDesktop SHARED ${bbDesktop_SOURCES} ${bbDesktop_RESOURCES})
target_link_libraries(bbDesktop blackbox bblib bbPlugin)
target_link_libraries(bbDesktop version comctl32)

set_property(TARGET bbDesktop PROPERTY FOLDER "plugins/bbDesktop")

#install(FILES readme.txt DESTINATION plugins/bbDesktop)
install(TARGETS bbDesktop RUNTIME DESTINATION plugins/bbDesktop)
if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbDesktop/${bbDesktop_CONFIGS}")
	install(FILES ${bbDesktop_CONFIGS} DESTINATION plugins/bbDesktop)
endif()

