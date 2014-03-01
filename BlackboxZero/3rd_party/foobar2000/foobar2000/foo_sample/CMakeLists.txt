project(foosample)

set(foosample_SOURCES
  contextmenu.cpp
  decode.cpp
  dsp.cpp
  initquit.cpp
  input_raw.cpp
  main.cpp
  mainmenu.cpp
  #PCH.cpp
  playback_state.cpp
  preferences.cpp
  ui_element.cpp
)

add_definitions(-DUNICODE -D_UNICODE)

include_directories(${CMAKE_SOURCE_DIR}/3rd_party/foobar2000/foobar2000/SDK)
include_directories(${CMAKE_SOURCE_DIR}/3rd_party/foobar2000/foobar2000/WTL/Include)

add_executable(foosample WIN32 ${foosample_SOURCES} ${foosample_RESOURCES})
include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(foosample version comctl32 foobar2000SDK foobar2000pfcSDK foobar2000_component_client foobar2000helpersSDK)


