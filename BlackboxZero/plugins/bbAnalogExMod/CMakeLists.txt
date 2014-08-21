set(bbAnalogExMod_SOURCES BBAnalogExMod.cpp)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

add_library(bbAnalogExMod SHARED ${bbAnalogExMod_SOURCES} ${bbAnalogExMod_RESOURCES})

target_link_libraries(bbAnalogExMod blackbox bblib bbPlugin)
target_link_libraries(bbAnalogExMod version comctl32 shlwapi comdlg32 winmm gdiplus msimg32)

set_property(TARGET bbAnalogExMod PROPERTY FOLDER "plugins/bbAnalogExMod")

install(FILES readme.txt DESTINATION plugins/bbAnalogExMod)
install(TARGETS bbAnalogExMod RUNTIME DESTINATION plugins/bbAnalogExMod)
