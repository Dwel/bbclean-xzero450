include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

add_library(winamp SHARED winamp.cpp)
target_link_libraries(winamp blackbox bblib bbPlugin)
target_link_libraries(winamp version comctl32 gdiplus winmm msimg32)

set_property(TARGET winamp PROPERTY FOLDER "plugins/bbSeekBar/winamp")
install(TARGETS winamp RUNTIME DESTINATION plugins/bbSeekBar)

