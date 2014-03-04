project(bbPlayer)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

add_library(bbPlayer SHARED BBPlayer.cpp)
set_property(TARGET bbPlayer PROPERTY FOLDER "plugins/bbPlayer")
target_link_libraries(bbPlayer blackbox bblib bbPlugin)
target_link_libraries(bbPlayer version comctl32 gdiplus winmm msimg32)

add_library(for_foobar2000 SHARED for_foobar2000.cpp)
target_link_libraries(for_foobar2000 blackbox bblib bbPlugin)
add_library(for_lilith SHARED for_lilith.cpp)
target_link_libraries(for_lilith blackbox bblib bbPlugin)
add_library(for_winamp SHARED for_winamp.cpp)
target_link_libraries(for_winamp blackbox bblib bbPlugin)
set_property(TARGET for_foobar2000 PROPERTY FOLDER "plugins/bbPlayer")
set_property(TARGET for_lilith PROPERTY FOLDER "plugins/bbPlayer")
set_property(TARGET for_winamp PROPERTY FOLDER "plugins/bbPlayer")

install(TARGETS bbPlayer RUNTIME DESTINATION plugins/bbPlayer)
install(TARGETS for_foobar2000 RUNTIME DESTINATION plugins/bbPlayer)
install(TARGETS for_lilith RUNTIME DESTINATION plugins/bbPlayer)
install(TARGETS for_winamp RUNTIME DESTINATION plugins/bbPlayer)

