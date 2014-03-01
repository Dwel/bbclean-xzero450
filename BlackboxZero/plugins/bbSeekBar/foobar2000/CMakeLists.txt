include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
include_directories(${CMAKE_SOURCE_DIR}/3rd_party/foobar2000/foobar2000/SDK)

add_library(foobar2000 SHARED foobar2000.cpp)
target_link_libraries(foobar2000 blackbox bblib bbPlugin)
target_link_libraries(foobar2000 version comctl32 gdiplus winmm msimg32 foobar2000SDK)

set_property(TARGET foobar2000 PROPERTY FOLDER "plugins/bbSeekBar/foobar2000")
install(TARGETS foobar2000 RUNTIME DESTINATION plugins/bbSeekBar)

