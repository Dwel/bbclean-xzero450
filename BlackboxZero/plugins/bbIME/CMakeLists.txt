set(bbIME_SOURCES
  BBIME.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

add_library(bbIME SHARED ${bbIME_SOURCES} ${bbIME_RESOURCES})
target_link_libraries(bbIME blackbox bblib bbPlugin)
target_link_libraries(bbIME version comctl32)

set_property(TARGET bbIME PROPERTY FOLDER "plugins/bbIME")

install(FILES readme.txt DESTINATION plugins/bbIME)
install(TARGETS bbIME RUNTIME DESTINATION plugins/bbIME)

