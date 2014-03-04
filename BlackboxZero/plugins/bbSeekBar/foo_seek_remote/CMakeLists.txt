add_definitions(-DUNICODE -D_UNICODE)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
include_directories(${CMAKE_SOURCE_DIR}/3rd_party/foobar2000/foobar2000/SDK)

add_library(foo_seek_remote SHARED foo_seek_remote.cpp)
set_property(TARGET foo_seek_remote PROPERTY FOLDER "plugins/bbSeekBar/foo_seek_remote")
target_link_libraries(foo_seek_remote blackbox bblib bbPlugin)
target_link_libraries(foo_seek_remote version comctl32 gdiplus winmm msimg32 foobar2000SDK foobar2000pfcSDK foobar2000_component_client)

install(TARGETS foo_seek_remote RUNTIME DESTINATION plugins/bbSeekBar)

